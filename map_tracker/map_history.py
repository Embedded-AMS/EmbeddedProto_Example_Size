import pandas as pd
import sys
import os
from pathlib import Path
from datetime import datetime

def parse_map_file(map_file):
    """Parse GCC linker map file into structured data"""
    sections = []
    current_section = None
    
    with open(map_file, 'r') as f:
        content = f.read()
        
        # Extract main sections and subsections
        section_types = [
            '.text', '.data', '.bss', '.rodata', '.eh_frame',
            '.init', '.fini', '.ctors', '.dtors', '.jcr',
            '.dynamic', '.got', '.got.plt', '.data.rel.ro',
            '.init_array', '.fini_array'
        ]
        
        for section in section_types:
            # Find section in map file
            section_start = content.find(f"\n{section} ")
            if section_start != -1:
                # Get the section line
                section_end = content.find('\n', section_start + 1)
                section_line = content[section_start:section_end].strip()
                
                # Parse section size
                parts = section_line.split()
                if len(parts) >= 2:
                    try:
                        # Size is in hex format
                        size = int(parts[1], 16) if parts[1].startswith('0x') else int(parts[1])
                        sections.append({
                            'Section': section,  # Keep full section name
                            'Component': 'Total',
                            'Object': 'Total',
                            'Type': 'SECTION',
                            'Size': size
                        })
                    except (ValueError, IndexError):
                        continue
                        
        # Extract object file contributions
        current_section = None
        for line in content.split('\n'):
            line = line.strip()
            
            # Track current section
            for section in section_types:
                if line.startswith(f"{section} "):
                    current_section = section
                    break
            if any(line.startswith(f"{section} ") for section in section_types):
                continue
                
            # Look for object file entries
            if '.o' in line or any(section in line for section in ['.text', '.data', '.bss', '.rodata', '.eh_frame']):
                parts = line.split()
                if len(parts) >= 2:
                    try:
                        # Skip entries that look like raw memory addresses
                        if any(p.startswith('0x') and len(p) >= 10 for p in parts):
                            continue
                            
                        # Extract object file info
                        if 'CMakeFiles/' in line:
                            obj_path = parts[0]
                            obj_file = obj_path.split('/')[-1]
                            path_parts = obj_path.split('/')
                            
                            # Determine component from path
                            if 'EmbeddedProto/src' in obj_path:
                                lib_component = 'EmbeddedProto'
                            elif 'test' in obj_path:
                                lib_component = 'test'
                            else:
                                lib_component = path_parts[-2] if len(path_parts) > 2 else 'main'
                        else:
                            # Handle special sections and subsections
                            obj_file = parts[0]
                            if obj_file.startswith('.'):
                                if 'rodata' in obj_file:
                                    lib_component = 'Read-Only Data'
                                elif 'text' in obj_file:
                                    lib_component = 'Code'
                                elif any(x in obj_file for x in ['init', 'fini', 'ctors', 'dtors']):
                                    lib_component = 'Initialization'
                                elif any(x in obj_file for x in ['got', 'plt', 'dynamic']):
                                    lib_component = 'Dynamic Linking'
                                elif 'eh_frame' in obj_file:
                                    lib_component = 'Exception Handling'
                                elif 'data' in obj_file:
                                    lib_component = 'Data'
                                elif 'bss' in obj_file:
                                    lib_component = 'BSS'
                                else:
                                    lib_component = 'System'
                            else:
                                lib_component = 'Other'
                        
                        # Find size in the line
                        size = 0
                        for part in parts:
                            try:
                                if '0x' in part:
                                    size = int(part, 16)
                                else:
                                    size = int(part)
                                break
                            except ValueError:
                                continue
                                
                        if size > 0 and current_section:
                            sections.append({
                                'Section': current_section,
                                'Component': lib_component,
                                'Object': obj_file,
                                'Type': 'OBJECT',
                                'Size': size
                            })
                    except (ValueError, IndexError):
                        continue

    return sections

def process_map(map_file, binary_name):
    """Process map file and update master CSV"""
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    root_dir = Path(__file__).parent.parent.absolute()
    results_dir = root_dir / 'results'
    results_dir.mkdir(exist_ok=True)
    master_file = results_dir / f"{binary_name.lower()}_map_master.csv"
    
    # Parse map data
    entries = parse_map_file(map_file)
    df_new = pd.DataFrame(entries)
    
    # Pivot and merge with existing data
    if df_new.empty:
        print(f"No valid entries found in {map_file}")
        return

    df_pivot = df_new.pivot_table(
        index=['Section', 'Component', 'Object', 'Type'],
        values='Size',
        aggfunc='sum'
    ).reset_index()
    df_pivot[timestamp] = df_pivot['Size']
    df_pivot = df_pivot.drop(columns=['Size'])

    # Calculate component subtotals
    subtotals = []
    for section in df_pivot['Section'].unique():
        section_data = df_pivot[df_pivot['Section'] == section]
        for component in section_data['Component'].unique():
            if component != 'Total':
                component_size = section_data[
                    (section_data['Component'] == component) & 
                    (section_data['Type'] == 'OBJECT')
                ][timestamp].sum()
                
                if component_size > 0:
                    subtotals.append({
                        'Section': section,
                        'Component': component,
                        'Object': 'Subtotal',
                        'Type': 'SUBTOTAL',
                        timestamp: component_size
                    })
    
    if subtotals:
        df_subtotals = pd.DataFrame(subtotals)
        df_pivot = pd.concat([df_pivot, df_subtotals], ignore_index=True)

    # Merge with existing master file
    if master_file.exists():
        df_master = pd.read_csv(master_file)
        
        # Merge on index columns
        index_cols = ['Section', 'Component', 'Object', 'Type']
        df_merged = pd.merge(df_master, df_pivot, on=index_cols, how='outer')
        
        # Update values for the new timestamp
        for idx in df_pivot.index:
            mask = (
                (df_merged['Section'] == df_pivot.loc[idx, 'Section']) &
                (df_merged['Component'] == df_pivot.loc[idx, 'Component']) &
                (df_merged['Object'] == df_pivot.loc[idx, 'Object']) &
                (df_merged['Type'] == df_pivot.loc[idx, 'Type'])
            )
            if mask.any():
                df_merged.loc[mask, timestamp] = df_pivot.loc[idx, timestamp]
    else:
        df_merged = df_pivot

    # Sort the data
    sort_order = {
        'SECTION': 0,
        'SUBTOTAL': 1,
        'OBJECT': 2
    }
    
    df_merged['TypeOrder'] = df_merged['Type'].map(sort_order)
    df_merged = df_merged.sort_values(
        ['Section', 'TypeOrder', 'Component', 'Object']
    ).drop(columns=['TypeOrder'])
    
    temp_file = master_file.with_suffix('.tmp')
    df_merged.to_csv(temp_file, index=False)
    os.replace(temp_file, master_file)
    print(f"Updated {master_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 map_history.py [map_file] [binary_name]")
        sys.exit(1)
        
    process_map(sys.argv[1], sys.argv[2])
