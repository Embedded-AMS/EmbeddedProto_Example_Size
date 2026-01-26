import pandas as pd
import sys
import os
from pathlib import Path
from datetime import datetime

def parse_map_file(map_file):
    """Parse GCC linker map file into structured data"""
    symbols = []
    current_section = None
    
    with open(map_file, 'r') as f:
        lines = [line.strip() for line in f]
        
    # Process sections
    for line in lines:
        # Skip empty lines and lines without hex addresses
        if not line or '0x' not in line:
            continue
            
        parts = line.split()
        if len(parts) >= 3 and parts[1].startswith('0x'):
            try:
                # Get section name and size
                name = parts[0]
                size = int(parts[2], 16) if parts[2].startswith('0x') else int(parts[2])
                
                # Skip entries without proper section info
                if not name or name.startswith('('):
                    continue
                    
                # Determine section type
                if name.startswith('.text'):
                    section = 'text'
                    type = 'Function'
                elif name.startswith('.data'):
                    section = 'data'
                    type = 'Variable'
                elif name.startswith('.bss'):
                    section = 'bss'
                    type = 'Variable'
                elif name.startswith('.rodata'):
                    section = 'rodata'
                    type = 'Variable'
                else:
                    continue  # Skip other sections
                
                symbols.append({
                    'Symbol': name,
                    'Section': section,
                    'Type': type,
                    'Size': size
                })
            except (ValueError, IndexError):
                continue
                
    return symbols

def process_map_file(map_file, binary_name):
    """Process map file and update master CSV"""
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    root_dir = Path(__file__).parent.parent.absolute()
    results_dir = root_dir / 'results'
    results_dir.mkdir(exist_ok=True)
    master_file = results_dir / f"{binary_name.lower()}_map_symbols.csv"
    
    # Parse map data
    entries = parse_map_file(map_file)
    df_new = pd.DataFrame(entries)
    
    if df_new.empty:
        print(f"No valid entries found in {map_file}")
        return

    # Pivot and merge with existing data
    df_pivot = df_new.pivot_table(
        index=['Symbol', 'Section', 'Type'],
        values='Size',
        aggfunc='sum'
    ).reset_index()
    df_pivot[timestamp] = df_pivot['Size']
    df_pivot = df_pivot.drop(columns=['Size'])

    # Calculate section subtotals
    subtotals = []
    for section in df_pivot['Section'].unique():
        section_data = df_pivot[df_pivot['Section'] == section]
        section_size = section_data[timestamp].sum()
        
        if section_size > 0:
            subtotals.append({
                'Symbol': f'TOTAL_{section.upper()}',
                'Section': section,
                'Type': 'SUBTOTAL',
                timestamp: section_size
            })
    
    if subtotals:
        df_subtotals = pd.DataFrame(subtotals)
        df_pivot = pd.concat([df_pivot, df_subtotals], ignore_index=True)

    # Merge with existing master file
    if master_file.exists():
        df_master = pd.read_csv(master_file)
        
        # Merge on index columns
        index_cols = ['Symbol', 'Section', 'Type']
        df_merged = pd.merge(df_master, df_pivot, on=index_cols, how='outer')
        
        # Update values for the new timestamp
        for idx in df_pivot.index:
            mask = (
                (df_merged['Symbol'] == df_pivot.loc[idx, 'Symbol']) &
                (df_merged['Section'] == df_pivot.loc[idx, 'Section']) &
                (df_merged['Type'] == df_pivot.loc[idx, 'Type'])
            )
            if mask.any():
                df_merged.loc[mask, timestamp] = df_pivot.loc[idx, timestamp]
    else:
        df_merged = df_pivot

    # Sort the data
    sort_order = {
        'SUBTOTAL': 0,
        'Function': 1,
        'Variable': 2
    }
    
    df_merged['TypeOrder'] = df_merged['Type'].map(sort_order)
    df_merged = df_merged.sort_values(
        ['Section', 'TypeOrder', 'Symbol']
    ).drop(columns=['TypeOrder'])
    
    # Save with atomic write
    temp_file = master_file.with_suffix('.tmp')
    df_merged.to_csv(temp_file, index=False)
    os.replace(temp_file, master_file)
    print(f"Updated {master_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 map_symbol_tracker.py [map_file] [binary_name]")
        sys.exit(1)
        
    process_map_file(sys.argv[1], sys.argv[2])
