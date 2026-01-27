import re
import sys
import os

def parse_map_file(file_path, search_pattern):
    """
    Parse the .map file and extract sizes based on the search pattern.

    Args:
    - file_path (str): Path to the .map file.
    - search_pattern (str): Wildcard pattern to search for.

    Returns:
    - tuple: Total size of matching entries in FLASH and RAM.
    """
    if not os.path.isfile(file_path):
        print(f"Error: File '{file_path}' not found.")
        return 0, 0

    section = None
    wildcard_pattern = search_pattern.replace('*', '.*')
    search_regex = re.compile(wildcard_pattern)

    flash_size = 0
    ram_size = 0

    with open(file_path, 'r') as file:
        for line in file:
            # Check if the line defines a new section
            section_match = re.match(r'^\s*\.(\S+)', line)
            if section_match:
                section = section_match.group(1)

            # Check if the line contains size and symbol information
            size_match = re.match(r'^\s*0x[0-9a-fA-F]+\s+0x([0-9a-fA-F]+)\s+(\S+)', line)
            if size_match and section:
                print(section)
                size_hex, symbol = size_match.groups()
                print(size_hex)
                print(symbols)
                if search_regex.search(symbol):
                    size = int(size_hex, 16)
                    if section in ['text', 'rodata']:  # Assuming .text and .rodata are in FLASH
                        flash_size += size
                    elif section in ['data', 'bss']:  # Assuming .data and .bss are in RAM
                        ram_size += size

    return flash_size, ram_size

def human_readable_size(size_bytes):
    """
    Convert a size in bytes to a human-readable format.

    Args:
    - size_bytes (int): Size in bytes.

    Returns:
    - str: Human-readable size.
    """
    if size_bytes == 0:
        return "0 B"

    size_names = ("B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB")
    i = int(min(len(size_names) - 1, (len(str(size_bytes)) - 1) // 3))
    p = 10 ** (i * 3)
    s = round(size_bytes / p, 2)
    
    return f"{s} {size_names[i]}"

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python analyze_map.py <path_to_map_file> <search_pattern>")
        sys.exit(1)

    map_file_path = sys.argv[1]
    search_pattern = sys.argv[2]

    flash_size, ram_size = parse_map_file(map_file_path, search_pattern)
    readable_flash_size = human_readable_size(flash_size)
    readable_ram_size = human_readable_size(ram_size)

    print(f"Total FLASH size for pattern '{search_pattern}': {readable_flash_size}")
    print(f"Total RAM size for pattern '{search_pattern}': {readable_ram_size}")
