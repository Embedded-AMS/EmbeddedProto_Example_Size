import pandas as pd
import sys
import os
from pathlib import Path

def process_symbols(symbol_csv, binary_name):
    # Store master file in results directory
    root_dir = Path(__file__).parent.parent.absolute()
    results_dir = root_dir / 'results'
    results_dir.mkdir(exist_ok=True)
    master_file = results_dir / f"{binary_name.lower()}_master.csv"
    timestamp = Path(symbol_csv).stem.split('_')[-1]

    # Read new symbol data
    try:
        df_new = pd.read_csv(symbol_csv, sep=';')
        df_new = df_new[['Symbol', 'Type', 'Size']].drop_duplicates()
        df_new = df_new.pivot_table(index=['Symbol', 'Type'], 
                                  values='Size', 
                                  aggfunc='first').reset_index()
        df_new[timestamp] = df_new['Size']
        df_new = df_new.drop(columns=['Size'])
    except Exception as e:
        print(f"Error processing {symbol_csv}: {e}")
        return

    # Merge with existing data
    if master_file.exists():
        print(f"Reading existing master file with columns:")
        df_master = pd.read_csv(master_file)
        print(df_master.columns.tolist())
        
        # Add new timestamp column to master
        df_master[timestamp] = pd.Series(dtype='object')
        
        # Update values for matching symbols
        for idx, row in df_new.iterrows():
            mask = (df_master['Symbol'] == row['Symbol']) & (df_master['Type'] == row['Type'])
            if mask.any():
                df_master.loc[mask, timestamp] = row[timestamp]
            else:
                new_row = pd.DataFrame({
                    'Symbol': [row['Symbol']],
                    'Type': [row['Type']],
                    timestamp: [row[timestamp]]
                })
                df_master = pd.concat([df_master, new_row], ignore_index=True)
        
        # Sort columns chronologically
        timestamp_cols = [col for col in df_master.columns if col not in ['Symbol', 'Type']]
        print(f"Timestamp columns found: {timestamp_cols}")
        df_merged = df_master[['Symbol', 'Type'] + sorted(timestamp_cols)]
        print(f"Final columns: {df_merged.columns.tolist()}")
    else:
        print("Creating new master file")
        df_merged = df_new

    # Sort by Symbol for readability
    df_merged = df_merged.sort_values('Symbol')
    
    # Save with atomic write
    temp_file = master_file.with_suffix('.tmp')
    df_merged.to_csv(temp_file, index=False, sep=',')
    os.replace(temp_file, master_file)
    print(f"Updated {master_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 symbol_history.py [symbol_csv] [binary_name]")
        sys.exit(1)
        
    process_symbols(sys.argv[1], sys.argv[2])
