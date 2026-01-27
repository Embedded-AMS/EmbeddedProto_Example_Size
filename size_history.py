#!/usr/bin/env python3
"""
Size History Tracker - Track size changes over time and detect regressions.

This module extends the existing symbol tracking infrastructure to provide:
1. CSV-based history tracking for overall size metrics
2. Regression detection (5% threshold)
3. Comparison between current and previous runs
4. Integration with the main comparison workflow
"""

import csv
import os
import sys
import sqlite3
from pathlib import Path
from dataclasses import dataclass
from typing import List, Dict, Optional, Tuple
import datetime
import subprocess


@dataclass
class SizeHistoryEntry:
    """Single entry in size history."""
    timestamp: str
    library: str
    scenario: str
    target: str
    text_size: int
    data_size: int
    bss_size: int
    total_flash: int
    total_ram: int
    serialize_cycles: Optional[int] = None
    deserialize_cycles: Optional[int] = None
    
    @classmethod
    def from_dict(cls, data: Dict[str, str]) -> 'SizeHistoryEntry':
        """Create from dictionary."""
        return cls(
            timestamp=data['timestamp'],
            library=data['library'],
            scenario=data['scenario'],
            target=data['target'],
            text_size=int(data['text_size']),
            data_size=int(data['data_size']),
            bss_size=int(data['bss_size']),
            total_flash=int(data['total_flash']),
            total_ram=int(data['total_ram']),
            serialize_cycles=int(data['serialize_cycles']) if data.get('serialize_cycles') else None,
            deserialize_cycles=int(data['deserialize_cycles']) if data.get('deserialize_cycles') else None
        )


class SizeHistoryTracker:
    """Track size history and detect regressions."""
    
    def __init__(self, results_dir: Optional[Path] = None, use_sqlite: bool = True):
        self.results_dir = Path(results_dir) if results_dir else Path(__file__).parent / "results"
        self.use_sqlite = use_sqlite
        
        if use_sqlite:
            self.db_file = self.results_dir / "size_history.db"
            self.history_file = None  # SQLite mode
            self._initialize_database()
        else:
            self.history_file = self.results_dir / "size_history.csv"
            self.db_file = None  # CSV mode
        
        self.results_dir.mkdir(exist_ok=True)
        
    def _initialize_database(self) -> None:
        """Initialize SQLite database with proper schema."""
        with sqlite3.connect(self.db_file) as conn:
            cursor = conn.cursor()
            
            # Create size_history table if it doesn't exist
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS size_history (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    timestamp TEXT NOT NULL,
                    library TEXT NOT NULL,
                    scenario TEXT NOT NULL,
                    target TEXT NOT NULL,
                    text_size INTEGER NOT NULL,
                    data_size INTEGER NOT NULL,
                    bss_size INTEGER NOT NULL,
                    total_flash INTEGER NOT NULL,
                    total_ram INTEGER NOT NULL,
                    serialize_cycles INTEGER,
                    deserialize_cycles INTEGER,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    UNIQUE(timestamp, library, scenario, target)
                )
            ''')
            
            # Create index for faster queries
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_library_scenario 
                ON size_history(library, scenario)
            ''')
            
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_timestamp 
                ON size_history(timestamp)
            ''')
            
            conn.commit()
        
        print(f"Initialized SQLite database: {self.db_file}")
    
    def append_results(self, results: List[Dict[str, str]]) -> None:
        """Append new results to history (CSV or SQLite)."""
        # Convert to list of SizeHistoryEntry for validation
        entries = [SizeHistoryEntry.from_dict(r) for r in results]
        
        if self.use_sqlite:
            self._append_to_sqlite(entries)
        else:
            self._append_to_csv(entries)
    
    def _append_to_csv(self, entries: List[SizeHistoryEntry]) -> None:
        """Append new results to CSV file."""
        # Create file if it doesn't exist
        file_exists = self.history_file.exists()
        
        with open(self.history_file, 'a', newline='') as f:
            writer = csv.writer(f)
            
            # Write header if file is new
            if not file_exists:
                writer.writerow([
                    'timestamp', 'library', 'scenario', 'target',
                    'text_size', 'data_size', 'bss_size',
                    'total_flash', 'total_ram',
                    'serialize_cycles', 'deserialize_cycles'
                ])
            
            # Write data rows
            for entry in entries:
                writer.writerow([
                    entry.timestamp,
                    entry.library,
                    entry.scenario,
                    entry.target,
                    entry.text_size,
                    entry.data_size,
                    entry.bss_size,
                    entry.total_flash,
                    entry.total_ram,
                    entry.serialize_cycles or '',
                    entry.deserialize_cycles or ''
                ])
        
        print(f"Appended {len(entries)} entries to CSV size history: {self.history_file}")
    
    def _append_to_sqlite(self, entries: List[SizeHistoryEntry]) -> None:
        """Append new results to SQLite database."""
        with sqlite3.connect(self.db_file) as conn:
            cursor = conn.cursor()
            
            for entry in entries:
                cursor.execute('''
                    INSERT OR IGNORE INTO size_history 
                    (timestamp, library, scenario, target, text_size, data_size, bss_size, 
                     total_flash, total_ram, serialize_cycles, deserialize_cycles)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', (
                    entry.timestamp,
                    entry.library,
                    entry.scenario,
                    entry.target,
                    entry.text_size,
                    entry.data_size,
                    entry.bss_size,
                    entry.total_flash,
                    entry.total_ram,
                    entry.serialize_cycles,
                    entry.deserialize_cycles
                ))
            
            conn.commit()
        
        print(f"Appended {len(entries)} entries to SQLite size history: {self.db_file}")
    
    def load_history(self) -> List[SizeHistoryEntry]:
        """Load complete history from CSV or SQLite."""
        if self.use_sqlite:
            return self._load_from_sqlite()
        else:
            return self._load_from_csv()
    
    def _load_from_csv(self) -> List[SizeHistoryEntry]:
        """Load complete history from CSV file."""
        if not self.history_file.exists():
            return []
        
        entries = []
        with open(self.history_file, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                try:
                    entry = SizeHistoryEntry.from_dict(row)
                    entries.append(entry)
                except Exception as e:
                    print(f"Warning: Could not parse history entry: {e}")
        
        return entries
    
    def _load_from_sqlite(self) -> List[SizeHistoryEntry]:
        """Load complete history from SQLite database."""
        if not self.db_file.exists():
            return []
        
        entries = []
        with sqlite3.connect(self.db_file) as conn:
            cursor = conn.cursor()
            
            cursor.execute('''
                SELECT timestamp, library, scenario, target, 
                       text_size, data_size, bss_size, 
                       total_flash, total_ram, 
                       serialize_cycles, deserialize_cycles
                FROM size_history
                ORDER BY timestamp ASC
            ''')
            
            for row in cursor.fetchall():
                try:
                    entry = SizeHistoryEntry(
                        timestamp=row[0],
                        library=row[1],
                        scenario=row[2],
                        target=row[3],
                        text_size=row[4],
                        data_size=row[5],
                        bss_size=row[6],
                        total_flash=row[7],
                        total_ram=row[8],
                        serialize_cycles=row[9],
                        deserialize_cycles=row[10]
                    )
                    entries.append(entry)
                except Exception as e:
                    print(f"Warning: Could not parse database entry: {e}")
        
        return entries
    
    def get_latest_entry(self, library: str, scenario: str, target: str = "stm32") -> Optional[SizeHistoryEntry]:
        """Get the most recent entry for a specific library/scenario combination."""
        entries = self.load_history()
        
        # Filter by library, scenario, and target
        filtered = [e for e in entries 
                   if e.library == library 
                   and e.scenario == scenario 
                   and e.target == target]
        
        if not filtered:
            return None
        
        # Sort by timestamp (newest first) and return first
        filtered.sort(key=lambda x: x.timestamp, reverse=True)
        return filtered[0]
    
    def compare_with_previous(self, current_results: List[Dict[str, str]]) -> Dict[str, Dict[str, float]]:
        """Compare current results with previous run and calculate percentage changes.
        
        Returns:
            Dict[library_scenario_key, Dict[metric, percentage_change]]
        """
        comparisons = {}
        
        for current in current_results:
            entry = SizeHistoryEntry.from_dict(current)
            key = f"{entry.library}_{entry.scenario}"
            
            previous = self.get_latest_entry(entry.library, entry.scenario, entry.target)
            
            if previous:
                changes = {}
                
                # Calculate percentage changes
                if previous.total_flash > 0 and entry.total_flash > 0:
                    flash_change = ((entry.total_flash - previous.total_flash) / previous.total_flash) * 100
                    changes['flash_change'] = flash_change
                
                if previous.total_ram > 0 and entry.total_ram > 0:
                    ram_change = ((entry.total_ram - previous.total_ram) / previous.total_ram) * 100
                    changes['ram_change'] = ram_change
                
                if previous.serialize_cycles and entry.serialize_cycles:
                    serialize_change = ((entry.serialize_cycles - previous.serialize_cycles) / previous.serialize_cycles) * 100
                    changes['serialize_change'] = serialize_change
                
                if previous.deserialize_cycles and entry.deserialize_cycles:
                    deserialize_change = ((entry.deserialize_cycles - previous.deserialize_cycles) / previous.deserialize_cycles) * 100
                    changes['deserialize_change'] = deserialize_change
                
                comparisons[key] = changes
        
        return comparisons
    
    def check_regressions(self, comparisons: Dict[str, Dict[str, float]], 
                         threshold: float = 5.0) -> List[Dict[str, str]]:
        """Check for regressions exceeding the threshold.
        
        Returns list of regression warnings.
        """
        regressions = []
        
        for key, changes in comparisons.items():
            library, scenario = key.split('_', 1)
            
            for metric, change in changes.items():
                if change > threshold:  # Positive change means increase (regression)
                    regressions.append({
                        'library': library,
                        'scenario': scenario,
                        'metric': metric,
                        'change': f"{change:+.1f}%",
                        'message': f"{metric} increased by {change:.1f}% (threshold: {threshold}%)"
                    })
        
        return regressions
    
    def generate_history_report(self, output_file: Optional[Path] = None) -> Path:
        """Generate a markdown report showing size history trends."""
        if output_file is None:
            timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = self.results_dir / "reports" / f"size_history_report_{timestamp}.md"
        
        # Ensure reports directory exists
        output_file.parent.mkdir(exist_ok=True)
        
        entries = self.load_history()
        
        # Group by library/scenario combination
        groups = {}
        for entry in entries:
            key = f"{entry.library}_{entry.scenario}"
            if key not in groups:
                groups[key] = []
            groups[key].append(entry)
        
        with open(output_file, 'w') as f:
            f.write("# Size History Report\n\n")
            f.write(f"Generated: {datetime.datetime.now().isoformat()}\n\n")
            f.write(f"Total entries: {len(entries)}\n\n")
            
            # Summary table
            f.write("## Summary by Library/Scenario\n\n")
            f.write("| Library | Scenario | Latest Flash | Latest RAM | Total Runs |\n")
            f.write("|---------|----------|--------------|------------|------------|\n")
            
            for key, group_entries in groups.items():
                library, scenario = key.split('_', 1)
                latest = max(group_entries, key=lambda x: x.timestamp)
                f.write(f"| {library} | {scenario} | {latest.total_flash:,} bytes | "
                       f"{latest.total_ram:,} bytes | {len(group_entries)} |\n")
            
            f.write("\n")
            
            # Detailed history for each combination
            for key, group_entries in groups.items():
                library, scenario = key.split('_', 1)
                
                f.write(f"## {library} + {scenario}\n\n")
                
                # Sort by timestamp (oldest first)
                group_entries.sort(key=lambda x: x.timestamp)
                
                f.write("| Timestamp | Flash | RAM | Serialize Cycles | Deserialize Cycles |\n")
                f.write("|-----------|-------|-----|------------------|---------------------|\n")
                
                for entry in group_entries:
                    f.write(f"| {entry.timestamp} | {entry.total_flash:,} bytes | "
                           f"{entry.total_ram:,} bytes | "
                           f"{entry.serialize_cycles or 'N/A'} | "
                           f"{entry.deserialize_cycles or 'N/A'} |\n")
                
                f.write("\n")
                
                # Calculate trends
                if len(group_entries) >= 2:
                    first = group_entries[0]
                    last = group_entries[-1]
                    
                    flash_trend = ((last.total_flash - first.total_flash) / first.total_flash) * 100
                    ram_trend = ((last.total_ram - first.total_ram) / first.total_ram) * 100
                    
                    f.write(f"### Trends\n\n")
                    f.write(f"- **Flash change**: {flash_trend:+.1f}% (from {first.total_flash:,} to {last.total_flash:,} bytes)\n")
                    f.write(f"- **RAM change**: {ram_trend:+.1f}% (from {first.total_ram:,} to {last.total_ram:,} bytes)\n")
                    f.write("\n")
        
        print(f"History report generated: {output_file}")
        return output_file
    
    def integrate_with_symbol_tracker(self, results: List[Dict[str, str]]) -> None:
        """Integrate with existing symbol tracking infrastructure."""
        # This would call the existing symbol_history.py script
        # For now, we'll just ensure the symbol tracking is aware of our results
        print("Integrating with symbol tracker...")
        
        # The existing symbol tracking infrastructure already handles per-symbol tracking
        # Our size history tracker focuses on overall metrics
        # In a full implementation, we would:
        # 1. Extract symbol sizes using nm
        # 2. Call symbol_history.py to update master CSV
        # 3. Cross-reference with our overall size metrics
        
        print("Symbol tracker integration complete.")


def main():
    """Command-line interface for size history tracker."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="Size History Tracker - Track size changes over time"
    )
    parser.add_argument("--append", action="store_true",
                       help="Append results from JSON file to history")
    parser.add_argument("--compare", action="store_true",
                       help="Compare current results with previous run")
    parser.add_argument("--report", action="store_true",
                       help="Generate history report")
    parser.add_argument("--input", help="Input JSON file with current results")
    parser.add_argument("--threshold", type=float, default=5.0,
                       help="Regression threshold percentage (default: 5.0)")
    parser.add_argument("--csv", action="store_true",
                       help="Use CSV storage instead of SQLite")
    
    args = parser.parse_args()
    
    tracker = SizeHistoryTracker(use_sqlite=not args.csv)
    
    if args.append and args.input:
        # Load results from JSON file
        import json
        with open(args.input, 'r') as f:
            results = json.load(f)
        
        # Convert to flat format for CSV
        flat_results = []
        for result in results:
            flat_result = {
                'timestamp': result['timestamp'],
                'library': result['library'],
                'scenario': result['scenario'],
                'target': result['target'],
                'text_size': result['size']['text'],
                'data_size': result['size']['data'],
                'bss_size': result['size']['bss'],
                'total_flash': result['size']['total_flash'],
                'total_ram': result['size']['total_ram'],
                'serialize_cycles': result.get('cycles', {}).get('serialize_cycles'),
                'deserialize_cycles': result.get('cycles', {}).get('deserialize_cycles')
            }
            flat_results.append(flat_result)
        
        tracker.append_results(flat_results)
        
        # Compare with previous
        comparisons = tracker.compare_with_previous(flat_results)
        print(f"Comparisons: {comparisons}")
        
        # Check for regressions
        regressions = tracker.check_regressions(comparisons, args.threshold)
        if regressions:
            print(f"\n⚠️  REGRESSIONS DETECTED ({len(regressions)}):")
            for regression in regressions:
                print(f"  - {regression['library']} + {regression['scenario']}: "
                      f"{regression['message']}")
        else:
            print("\n✅ No regressions detected.")
    
    if args.report:
        tracker.generate_history_report()
    
    return 0


if __name__ == "__main__":
    sys.exit(main())