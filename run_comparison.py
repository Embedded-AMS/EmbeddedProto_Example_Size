#!/usr/bin/env python3
"""
Size Comparison Tool - Main Orchestration Script

This script orchestrates the full comparison workflow:
1. Build all libraries for selected scenario
2. Extract size metrics from ELF files
3. Flash STM32 board and measure CPU cycles
4. Collect all results into structured format
5. Track size history and detect regressions

Usage:
    python run_comparison.py                    # Run all scenarios, all libraries
    python run_comparison.py --scenario simple_types  # Run specific scenario
    python run_comparison.py --library embeddedproto  # Run specific library
    python run_comparison.py --report           # Generate report only
    python run_comparison.py --no-flash         # Skip flashing and cycle measurement
    python run_comparison.py --check-regressions # Check for size regressions
"""

import argparse
import sys
import os
import json
import csv
from pathlib import Path
from dataclasses import dataclass, asdict
from typing import List, Dict, Optional, Any
import subprocess
import time
import datetime

# Import existing components
from build_stm32 import STM32Builder, BuildResult
from extract_size import ELFSizeExtractor, SizeMetrics
from flash_stm32 import STM32Flasher, FlashResult
from measure_cycles import STM32CycleMeasurer, CycleMetrics, MeasurementResult
from size_history import SizeHistoryTracker


@dataclass
class ComparisonResult:
    """Complete comparison result for one library/scenario combination."""
    library: str
    scenario: str
    target: str
    build: BuildResult
    size: Optional[SizeMetrics] = None
    flash: Optional[FlashResult] = None
    cycles: Optional[CycleMetrics] = None
    timestamp: str = ""
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        result = asdict(self)
        # Convert nested dataclasses to dicts
        if self.build:
            result['build'] = asdict(self.build)
        if self.size:
            result['size'] = asdict(self.size)
        if self.flash:
            result['flash'] = asdict(self.flash)
        if self.cycles:
            result['cycles'] = asdict(self.cycles)
        return result


class ComparisonOrchestrator:
    """Main class to orchestrate size comparison workflow."""
    
    def __init__(self, base_dir: Optional[Path] = None):
        self.base_dir = Path(base_dir) if base_dir else Path(__file__).parent.absolute()
        self.scenarios_dir = self.base_dir / "scenarios"
        self.results_dir = self.base_dir / "results"
        self.reports_dir = self.results_dir / "reports"
        
        # Initialize components
        self.builder = STM32Builder(self.base_dir)
        self.size_extractor = ELFSizeExtractor()
        self.flasher = STM32Flasher()
        self.cycle_measurer = STM32CycleMeasurer()
        self.history_tracker = SizeHistoryTracker(self.results_dir, use_sqlite=True)
        
        # Ensure directories exist
        self.results_dir.mkdir(exist_ok=True)
        self.reports_dir.mkdir(exist_ok=True)
    
    def get_available_scenarios(self) -> List[str]:
        """Get list of available test scenarios."""
        scenarios = []
        if self.scenarios_dir.exists():
            for item in self.scenarios_dir.iterdir():
                if item.is_dir() and (item / "message.proto").exists():
                    scenarios.append(item.name)
        return sorted(scenarios)
    
    def get_available_libraries(self) -> List[str]:
        """Get list of available libraries."""
        return ["embeddedproto", "nanopb", "arduinojson"]
    
    def run_single_comparison(self, scenario: str, library: str, 
                             target: str = "stm32", 
                             skip_flash: bool = False) -> ComparisonResult:
        """Run full comparison for one library/scenario combination."""
        print(f"\n=== Running comparison: {library} + {scenario} ===")
        
        result = ComparisonResult(
            library=library,
            scenario=scenario,
            target=target,
            build=BuildResult(success=False, output=""),
            timestamp=datetime.datetime.now().isoformat()
        )
        
        # Step 1: Generate code (if needed)
        print(f"Generating code for {library}...")
        if library == "embeddedproto":
            if not self.builder.generate_embeddedproto_code(scenario):
                result.build = BuildResult(
                    success=False,
                    output="",
                    error="EmbeddedProto code generation failed"
                )
                return result
        elif library == "nanopb":
            if not self.builder.generate_nanopb_code(scenario):
                result.build = BuildResult(
                    success=False,
                    output="",
                    error="NanoPB code generation failed"
                )
                return result
        elif library == "arduinojson":
            # ArduinoJson doesn't need code generation
            print("ArduinoJson: No code generation needed")
        
        # Step 2: Build
        print(f"Building {library} for {scenario}...")
        build_result = self.builder.build_stm32_project(library, scenario)
        result.build = build_result
        
        if not build_result.success:
            print(f"Build failed: {build_result.error}")
            return result
            
        print(f"Build successful. ELF: {build_result.elf_path}")
        
        # Step 3: Extract size metrics
        if build_result.elf_path:
            print(f"Extracting size metrics...")
            try:
                size_result = self.size_extractor.extract_sizes(build_result.elf_path)
                result.size = size_result
                print(f"Size extraction successful:")
                print(f"  Flash: {size_result.total_flash:,} bytes")
                print(f"  RAM: {size_result.total_ram:,} bytes")
            except Exception as e:
                print(f"Size extraction failed: {e}")
                result.build = BuildResult(
                    success=False,
                    output="",
                    error=f"Size extraction failed: {e}"
                )
                return result
        else:
            print("ERROR: No ELF file path provided from build")
            result.build = BuildResult(
                success=False,
                output="",
                error="No ELF file path from build"
            )
            return result
        
        # Step 4: Flash and measure cycles (optional)
        if not skip_flash and build_result.elf_path:
            print(f"Flashing to STM32...")
            flash_result = self.flasher.flash(build_result.elf_path)
            result.flash = flash_result
            
            if not flash_result.success:
                print(f"Flashing failed: {flash_result.error}")
                # Don't return here - flashing failure shouldn't stop the whole process
                # since we already have size metrics
            else:
                print(f"Flashing successful. Measuring CPU cycles...")
                # Give board time to reset and start
                time.sleep(2)
                
                cycle_result = self.cycle_measurer.capture_measurements()
                if cycle_result.success and cycle_result.metrics:
                    result.cycles = cycle_result.metrics
                    result.cycles.library = library
                    result.cycles.scenario = scenario
                    print(f"Cycle measurement successful:")
                    print(f"  Serialize: {cycle_result.metrics.serialize_cycles:,} cycles")
                    print(f"  Deserialize: {cycle_result.metrics.deserialize_cycles:,} cycles")
                else:
                    print(f"Cycle measurement failed: {cycle_result.raw_output}")
                    # Don't return here - cycle measurement failure shouldn't stop the whole process
                    # since we already have size metrics
        
        return result
    
    def run_all_comparisons(self, scenarios: Optional[List[str]] = None,
                           libraries: Optional[List[str]] = None,
                           skip_flash: bool = False) -> List[ComparisonResult]:
        """Run comparisons for all specified scenarios and libraries."""
        results = []
        
        # Get scenarios to run
        if scenarios is None or len(scenarios) == 0:
            scenarios = self.get_available_scenarios()
        
        # Get libraries to test
        if libraries is None or len(libraries) == 0:
            libraries = self.get_available_libraries()
        
        print(f"Running comparisons for {len(scenarios)} scenarios and {len(libraries)} libraries")
        print(f"Scenarios: {', '.join(scenarios)}")
        print(f"Libraries: {', '.join(libraries)}")
        
        # Run all combinations
        for scenario in scenarios:
            for library in libraries:
                result = self.run_single_comparison(scenario, library, skip_flash=skip_flash)
                results.append(result)
        
        return results
    
    def save_results(self, results: List[ComparisonResult], format: str = "json") -> Path:
        """Save comparison results to file."""
        
        print(f"\n{'='*60}")
        print(f"Storing results")
        print(f"{'='*60}\n")
        
        
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        
        if format == "json":
            filename = self.results_dir / f"comparison_results_{timestamp}.json"
            data = [r.to_dict() for r in results]
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
        elif format == "csv":
            filename = self.results_dir / f"comparison_results_{timestamp}.csv"
            with open(filename, 'w', newline='') as f:
                writer = csv.writer(f)
                # Write header
                writer.writerow([
                    "timestamp", "library", "scenario", "target",
                    "build_success", "text_size", "data_size", "bss_size",
                    "total_flash", "total_ram", "serialize_cycles", "deserialize_cycles"
                ])
                # Write data
                for result in results:
                    writer.writerow([
                        result.timestamp,
                        result.library,
                        result.scenario,
                        result.target,
                        result.build.success,
                        result.size.text if result.size else 0,
                        result.size.data if result.size else 0,
                        result.size.bss if result.size else 0,
                        result.size.total_flash if result.size else 0,
                        result.size.total_ram if result.size else 0,
                        result.cycles.serialize_cycles if result.cycles else 0,
                        result.cycles.deserialize_cycles if result.cycles else 0
                    ])
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        print(f"Results saved to: {filename}")
        return filename
    
    def track_size_history(self, results: List[ComparisonResult]) -> Dict[str, Dict[str, float]]:
        """Track size history and compare with previous runs."""
        # Convert results to flat format for history tracking
        flat_results = []
        for result in results:
            flat_result = {
                'timestamp': result.timestamp,
                'library': result.library,
                'scenario': result.scenario,
                'target': result.target,
                'text_size': result.size.text if result.size else 0,
                'data_size': result.size.data if result.size else 0,
                'bss_size': result.size.bss if result.size else 0,
                'total_flash': result.size.total_flash if result.size else 0,
                'total_ram': result.size.total_ram if result.size else 0,
                'serialize_cycles': result.cycles.serialize_cycles if result.cycles else None,
                'deserialize_cycles': result.cycles.deserialize_cycles if result.cycles else None
            }
            flat_results.append(flat_result)
        
        # Append to history
        self.history_tracker.append_results(flat_results)
        
        # Compare with previous runs
        comparisons = self.history_tracker.compare_with_previous(flat_results)
        
        return comparisons
    
    def check_regressions(self, comparisons: Dict[str, Dict[str, float]], 
                         threshold: float = 5.0) -> List[Dict[str, str]]:
        """Check for size regressions exceeding threshold."""
        return self.history_tracker.check_regressions(comparisons, threshold)
    
    def check_all_successful(self, results: List[ComparisonResult]) -> bool:
        """Check if all comparisons were successful."""
        return all(result.build.success for result in results)
    
    def generate_report(self, results: List[ComparisonResult]) -> Path:
        """Generate human-readable comparison report."""
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        report_file = self.reports_dir / f"comparison_report_{timestamp}.md"
        
        # Get compiler version and flags
        compiler_info = self._get_compiler_info()
        
        with open(report_file, 'w') as f:
            f.write("# Size Comparison Report\n\n")
            f.write(f"**Date**: {timestamp}\n")
            f.write(f"**Target**: STM32F446RE\n")
            if compiler_info:
                f.write(f"**Compiler**: {compiler_info}\n")
            f.write("**Flags**: -std=c++17 -Os -fdata-sections -ffunction-sections -Wl,--gc-sections\n\n")
            
            # Group results by scenario
            scenarios = {}
            for result in results:
                if result.scenario not in scenarios:
                    scenarios[result.scenario] = []
                scenarios[result.scenario].append(result)
            
            # Summary table with percentage differences
            f.write("## Summary\n\n")
            f.write("| Library | Flash (text) | RAM (data+bss) | Flash % Diff | RAM % Diff | Serialize Cycles | Deserialize Cycles |\n")
            f.write("|---------|--------------|----------------|--------------|------------|------------------|---------------------|\n")
            
            for scenario, scenario_results in scenarios.items():
                # Find baseline (EmbeddedProto) for percentage calculation
                baseline = None
                for result in scenario_results:
                    if result.library == "embeddedproto" and result.size:
                        baseline = result.size
                        break
                
                for result in scenario_results:
                    if result.size:
                        flash_diff = "N/A"
                        ram_diff = "N/A"
                        
                        if baseline:
                            if baseline.total_flash > 0:
                                flash_diff = f"{((result.size.total_flash - baseline.total_flash) / baseline.total_flash) * 100:+.1f}%"
                            if baseline.total_ram > 0:
                                ram_diff = f"{((result.size.total_ram - baseline.total_ram) / baseline.total_ram) * 100:+.1f}%"
                        
                        cycles_info = "N/A | N/A"
                        if result.cycles:
                            cycles_info = f"{result.cycles.serialize_cycles:,} | {result.cycles.deserialize_cycles:,}"
                        
                        f.write(f"| {result.library} | {result.size.total_flash:,} bytes | "
                               f"{result.size.total_ram:,} bytes | {flash_diff} | {ram_diff} | {cycles_info} |\n")
            
            f.write("\n")
            
            # Detailed breakdown by scenario
            for scenario, scenario_results in scenarios.items():
                f.write(f"## Scenario: {scenario}\n\n")
                f.write("| Library | Flash | RAM | Flash % Diff | RAM % Diff | Serialize | Deserialize |\n")
                f.write("|---------|-------|-----|--------------|------------|-----------|-------------|\n")
                
                # Find baseline for this scenario
                baseline = None
                for result in scenario_results:
                    if result.library == "embeddedproto" and result.size:
                        baseline = result.size
                        break
                
                for result in scenario_results:
                    if result.size:
                        flash_diff = "N/A"
                        ram_diff = "N/A"
                        
                        if baseline:
                            if baseline.total_flash > 0:
                                flash_diff = f"{((result.size.total_flash - baseline.total_flash) / baseline.total_flash) * 100:+.1f}%"
                            if baseline.total_ram > 0:
                                ram_diff = f"{((result.size.total_ram - baseline.total_ram) / baseline.total_ram) * 100:+.1f}%"
                        
                        cycles_info = "N/A | N/A"
                        if result.cycles:
                            cycles_info = f"{result.cycles.serialize_cycles:,} | {result.cycles.deserialize_cycles:,}"
                        
                        f.write(f"| {result.library} | {result.size.total_flash:,} bytes | "
                               f"{result.size.total_ram:,} bytes | {flash_diff} | {ram_diff} | {cycles_info} |\n")
                
                f.write("\n")
            
            # Build information
            f.write("## Build Information\n\n")
            for result in results:
                if result.build:
                    f.write(f"### {result.library} + {result.scenario}\n\n")
                    f.write(f"- **Success**: {result.build.success}\n")
                    if result.build.elf_path:
                        f.write(f"- **ELF Path**: {result.build.elf_path}\n")
                    if result.build.error:
                        f.write(f"- **Error**: {result.build.error}\n")
                    f.write("\n")
        
        print(f"Report generated: {report_file}")
        return report_file
    
    def _get_compiler_info(self) -> str:
        """Get ARM compiler version information."""
        try:
            # Try to get compiler version from ARM toolchain
            arm_gcc_path = self.builder._find_arm_toolchain()
            if arm_gcc_path:
                result = subprocess.run(
                    [str(Path(arm_gcc_path) / "arm-none-eabi-gcc"), "--version"],
                    capture_output=True, text=True, timeout=5
                )
                if result.returncode == 0:
                    # Extract version from first line
                    lines = result.stdout.strip().split('\n')
                    if lines:
                        return lines[0].strip()
        except Exception:
            pass
        
        return "Unknown (ARM GCC)"


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Size Comparison Tool - Automated benchmarking of EmbeddedProto vs NanoPB vs ArduinoJson"
    )
    parser.add_argument("--scenario", help="Specific scenario to run")
    parser.add_argument("--library", help="Specific library to test")
    parser.add_argument("--target", default="stm32", help="Target platform (stm32)")
    parser.add_argument("--report", action="store_true", help="Generate report only")
    parser.add_argument("--no-flash", action="store_true", 
                       help="Skip flashing and cycle measurement")
    parser.add_argument("--check-regressions", action="store_true",
                       help="Check for size regressions against previous runs")
    parser.add_argument("--regression-threshold", type=float, default=5.0,
                       help="Regression threshold percentage (default: 5.0)")
    parser.add_argument("--csv-storage", action="store_true",
                       help="Use CSV storage instead of SQLite (default: SQLite)")
    parser.add_argument("--format", choices=["json", "csv"], default="json",
                       help="Output format for results")
    parser.add_argument("--list-scenarios", action="store_true",
                       help="List available scenarios")
    parser.add_argument("--list-libraries", action="store_true",
                       help="List available libraries")
    
    args = parser.parse_args()
    
    # Initialize orchestrator
    orchestrator = ComparisonOrchestrator()
    orchestrator.history_tracker = SizeHistoryTracker(
        orchestrator.results_dir, 
        use_sqlite=not args.csv_storage
    )
    
    # List scenarios or libraries
    if args.list_scenarios:
        scenarios = orchestrator.get_available_scenarios()
        print("Available scenarios:")
        for scenario in scenarios:
            print(f"  - {scenario}")
        if not args.report:
            return 0
        else:
            return 0
    
    if args.list_libraries:
        libraries = orchestrator.get_available_libraries()
        print("Available libraries:")
        for library in libraries:
            print(f"  - {library}")
        return 0
    
    # Run comparisons
    if not args.report:
        # Determine scenarios to run
        scenarios = None
        if args.scenario:
            scenarios = [args.scenario]
        
        # Determine libraries to test
        libraries = None
        if args.library:
            libraries = [args.library]
        
        # Run all comparisons
        results = orchestrator.run_all_comparisons(
            scenarios=scenarios,
            libraries=libraries,
            skip_flash=args.no_flash
        )
        
        # Check if all comparisons were successful
        all_successful = orchestrator.check_all_successful(results)
        
        if not all_successful:
            print(f"\n❌ ERROR: Some comparisons failed!")
            print(f"Successful: {sum(1 for r in results if r.build.success)}/{len(results)}")
            
            # Show which ones failed
            failed_results = [r for r in results if not r.build.success]
            for failed in failed_results:
                error_msg = failed.build.error or "Unknown error"
                print(f"  - {failed.library} + {failed.scenario}: {error_msg}")
            
            # Do not save results or generate reports when there are failures
            print(f"\n⚠️  Not saving results or generating reports due to failures.")
            print(f"Please fix the issues and run again.")
            return 1
        
        # Save results (only if all successful)
        results_file = orchestrator.save_results(results, format=args.format)
        
        # Track size history (only if all successful)
        comparisons = orchestrator.track_size_history(results)
        
        # Check for regressions if requested
        if args.check_regressions:
            regressions = orchestrator.check_regressions(comparisons, args.regression_threshold)
            if regressions:
                print(f"\n⚠️  REGRESSIONS DETECTED ({len(regressions)}):")
                for regression in regressions:
                    print(f"  - {regression['library']} + {regression['scenario']}: "
                          f"{regression['message']}")
            else:
                print("\n✅ No regressions detected.")
        
        # Generate report (only if all successful)
        orchestrator.generate_report(results)
        
        print(f"\n=== Comparison Complete ===")
        print(f"Total comparisons run: {len(results)}")
        print(f"Successful builds: {sum(1 for r in results if r.build.success)}")
        print(f"Results saved to: {results_file}")
        print(f"Size history updated: {orchestrator.history_tracker.history_file}")
        print(f"Report generated: {orchestrator.reports_dir}")
    
    return 0


if __name__ == "__main__":
    sys.exit(main())