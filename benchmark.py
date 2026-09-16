#!/usr/bin/env python3
"""
Benchmark script for comparing EmbeddedProto vs NanoPB vs ArduinoJson on STM32 Nucleo-F446RE.

This script handles CLI argument parsing and validation for benchmarking scenarios.
It serves as the foundation for the automated size comparison tool.
"""

import argparse
import sys
import os
import subprocess
import shutil
import time
import sqlite3
import logging
from pathlib import Path
from typing import List, Dict, Optional, Tuple
from dataclasses import dataclass
from datetime import datetime, UTC
import glob

# Try to import tabulate for pretty table formatting, fall back to manual formatting
try:
    from tabulate import tabulate
    TABULATE_AVAILABLE = True
except ImportError:
    TABULATE_AVAILABLE = False
    print("Note: 'tabulate' library not found. Using manual table formatting. "
          "Install with 'pip install tabulate' for better output.")

# Valid library choices
VALID_LIBRARIES = ['ArduinoJson', 'EmbeddedProto', 'NanoPB', 'Baseline']

# Library name mapping for External directory
LIBRARY_TO_EXTERNAL_DIR = {
    'NanoPB': 'Nanopb',
    'EmbeddedProto': 'EmbeddedProto',
    'ArduinoJson': 'ArduinoJson',
    'Baseline': None  # Baseline has no external library
}

@dataclass
class StaticSizes:
    """Data class to hold static size metrics (text, data, bss from arm-none-eabi-size)."""
    text_bytes: int
    data_bytes: int
    bss_bytes: int

@dataclass
class DynamicMetrics:
    """Data class to hold dynamic metrics from device."""
    cpu_cycles: Optional[int]
    n_bytes_in_buffer: Optional[int]

@dataclass
class GitInfo:
    """Data class to hold git repository information."""
    commit_hash: str
    branch: Optional[str]
    tag: Optional[str]
    is_modified: bool


def parse_cmake_optimization_flags(cmake_path: str = 'CMakeLists.txt') -> Dict[str, str]:
    """
    Parse optimization flags from CMakeLists.txt for each build type.

    Args:
        cmake_path: Path to the CMakeLists.txt file (default: 'CMakeLists.txt')

    Returns:
        Dictionary mapping build type (e.g., 'Release', 'Debug') to optimization flags string

    Raises:
        FileNotFoundError: If CMakeLists.txt doesn't exist
        ValueError: If no optimization flags could be parsed
    """
    import re

    cmake_file = Path(cmake_path)
    if not cmake_file.exists():
        raise FileNotFoundError(f"CMakeLists.txt not found at: {cmake_path}")

    try:
        content = cmake_file.read_text()
    except Exception as e:
        raise ValueError(f"Failed to read CMakeLists.txt: {str(e)}") from e

    flags = {}

    # Pattern to match: set(CMAKE_CXX_FLAGS_<TYPE> "flags" ...)
    # Example: set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "" FORCE)
    pattern = r'set\s*\(\s*CMAKE_CXX_FLAGS_(\w+)\s+"([^"]+)"'

    matches = re.findall(pattern, content)

    # Expand ${VAR} references to plain set(VAR "value") definitions in the same file,
    # so the stored flags show what the compiler actually received.
    variables = dict(re.findall(r'set\s*\(\s*(\w+)\s+"([^"$]*)"\s*\)', content))

    for build_type, flag_string in matches:
        for name, value in variables.items():
            flag_string = flag_string.replace('${' + name + '}', value)
        # Capitalize first letter, lowercase rest (e.g., RELEASE -> Release)
        build_type_normalized = build_type.capitalize()
        flags[build_type_normalized] = flag_string

    if not flags:
        raise ValueError(f"No CMAKE_CXX_FLAGS_* definitions found in {cmake_path}")

    return flags


def get_optimization_flags(build_type: str) -> str:
    """
    Get optimization flags for a specific build type by parsing CMakeLists.txt.

    Args:
        build_type: Build type ('Release' or 'Debug')

    Returns:
        Optimization flags string for the specified build type

    Raises:
        ValueError: If build type is not found in CMakeLists.txt
    """
    try:
        flags = parse_cmake_optimization_flags()
        if build_type not in flags:
            raise ValueError(f"Build type '{build_type}' not found in CMakeLists.txt. "
                           f"Available types: {', '.join(flags.keys())}")
        return flags[build_type]
    except FileNotFoundError:
        # Fallback to hardcoded defaults if CMakeLists.txt is not found
        fallback_flags = {
            'Release': '-Os -DNDEBUG -ffunction-sections -fdata-sections',
            'Debug': '-Og -g -DDEBUG'
        }
        return fallback_flags.get(build_type, '')


def get_valid_build_types() -> List[str]:
    """
    Get list of valid build types from CMakeLists.txt.

    Returns:
        List of valid build type names (e.g., ['Release', 'Debug'])
    """
    try:
        flags = parse_cmake_optimization_flags()
        return list(flags.keys())
    except (FileNotFoundError, ValueError):
        # Fallback to defaults
        return ['Release', 'Debug']

@dataclass
class BenchmarkResult:
    """Data class to hold complete benchmark results for database storage."""
    library: str
    scenario: str
    text_bytes: int
    data_bytes: int
    bss_bytes: int
    cpu_cycles: Optional[int]
    n_bytes_in_buffer: Optional[int]
    library_commit_hash: str
    library_branch: Optional[str]
    library_tag: Optional[str]
    library_modified: bool
    timestamp: str
    build_type: str
    optimization_flags: str
    benchmark_mode: str

def get_available_libraries() -> List[str]:
    """
    Get list of available libraries.

    Returns:
        List of available library names: ['ArduinoJson', 'EmbeddedProto', 'NanoPB', 'Baseline']
    """
    return VALID_LIBRARIES

def get_scenarios_for_library(library: str) -> List[str]:
    """
    Get list of available scenarios for a specific library.

    Args:
        library: Library name

    Returns:
        List of scenario names (without .cpp extension)

    Raises:
        FileNotFoundError: If Scenarios directory doesn't exist
        ValueError: If library is not valid or has no scenarios
    """
    scenarios_dir = Path('Scenarios')
    if not scenarios_dir.exists():
        raise FileNotFoundError("Scenarios directory not found. Expected to find 'Scenarios/' directory.")

    if library not in VALID_LIBRARIES:
        raise ValueError(f"Invalid library '{library}'. Valid libraries are: {', '.join(VALID_LIBRARIES)}")

    library_dir = scenarios_dir / library
    if not library_dir.exists() or not library_dir.is_dir():
        raise ValueError(f"No scenarios found for library '{library}'. Directory does not exist.")

    # Find all .cpp files, excluding helper files
    cpp_files = [
        f.stem for f in library_dir.glob('*.cpp')
        if not f.stem.startswith('helper_')
    ]

    if not cpp_files:
        raise ValueError(f"No valid scenarios found for library '{library}'. No .cpp files found (excluding helper_*).")

    return cpp_files

def validate_library_scenario_combination(library: str, scenario: str) -> bool:
    """
    Validate that a library/scenario combination is valid.

    Args:
        library: Library name
        scenario: Scenario name

    Returns:
        True if combination is valid, False otherwise
    """
    try:
        scenarios = get_scenarios_for_library(library)
        return scenario in scenarios
    except (FileNotFoundError, ValueError):
        return False

def get_all_scenario_combinations() -> List[Tuple[str, str]]:
    """
    Get all valid library/scenario combinations.

    Returns:
        List of (library, scenario) tuples
    """
    combinations = []
    for library in VALID_LIBRARIES:
        try:
            scenarios = get_scenarios_for_library(library)
            for scenario in scenarios:
                combinations.append((library, scenario))
        except (FileNotFoundError, ValueError):
            # Skip libraries with no valid scenarios
            continue

    return combinations

def discover_scenarios() -> Dict[str, List[str]]:
    """
    Discover available scenarios from the Scenarios directory structure.

    Returns:
        Dictionary mapping library names to lists of available scenario names.

    Note: This function is maintained for backward compatibility.
    """
    scenarios = {}
    for library in VALID_LIBRARIES:
        try:
            scenarios[library] = get_scenarios_for_library(library)
        except (FileNotFoundError, ValueError):
            # Skip libraries with no valid scenarios
            continue
    return scenarios

def get_library_git_info(library: str) -> Optional[GitInfo]:
    """
    Extract git information from library repositories in External/ directory.

    Args:
        library: Library name (e.g., 'EmbeddedProto', 'NanoPB', 'ArduinoJson', 'Baseline')

    Returns:
        GitInfo dataclass with commit_hash, branch, tag, and is_modified status
        Returns None if library has no external repository (e.g., Baseline)

    Raises:
        FileNotFoundError: If library directory doesn't exist
        ValueError: If library is not valid
    """
    # Validate library
    if library not in VALID_LIBRARIES:
        raise ValueError(f"Invalid library '{library}'. Valid libraries are: {', '.join(VALID_LIBRARIES)}")

    # Get external directory path (handle library name mapping)
    external_dir_name = LIBRARY_TO_EXTERNAL_DIR.get(library)
    if external_dir_name is None:
        # Baseline has no external library
        return None

    external_path = Path('External') / external_dir_name

    # Check if external directory exists
    if not external_path.exists():
        raise FileNotFoundError(f"External directory not found for library '{library}': {external_path}")

    # Check if it's a git repository
    git_dir = external_path / '.git'
    if not git_dir.exists():
        raise ValueError(f"Directory is not a git repository: {external_path}")

    try:
        # Get current commit hash
        commit_hash_result = subprocess.run(
            ['git', 'rev-parse', 'HEAD'],
            cwd=external_path,
            capture_output=True,
            text=True,
            check=True
        )
        commit_hash = commit_hash_result.stdout.strip()

        # Get current branch (handles detached HEAD)
        branch = None
        try:
            branch_result = subprocess.run(
                ['git', 'rev-parse', '--abbrev-ref', 'HEAD'],
                cwd=external_path,
                capture_output=True,
                text=True,
                check=True
            )
            branch = branch_result.stdout.strip()
            # Handle detached HEAD case
            if branch == 'HEAD':
                branch = None
        except subprocess.CalledProcessError:
            # If we can't get branch, that's okay - it might be detached HEAD
            branch = None

        # Get tag at HEAD (if exact match exists)
        tag = None
        try:
            tag_result = subprocess.run(
                ['git', 'describe', '--tags', '--exact-match'],
                cwd=external_path,
                capture_output=True,
                text=True,
                check=True
            )
            tag = tag_result.stdout.strip()
        except subprocess.CalledProcessError:
            # No exact tag match - that's okay
            tag = None

        # Check for uncommitted modifications
        status_result = subprocess.run(
            ['git', 'status', '--porcelain'],
            cwd=external_path,
            capture_output=True,
            text=True,
            check=True
        )
        is_modified = bool(status_result.stdout.strip())

        return GitInfo(
            commit_hash=commit_hash,
            branch=branch,
            tag=tag,
            is_modified=is_modified
        )

    except subprocess.CalledProcessError as e:
        error_msg = f"Failed to extract git information from {external_path}: {str(e)}"
        if e.stderr:
            error_msg += f"\nGit error: {e.stderr.strip()}"
        raise ValueError(error_msg) from e
    except Exception as e:
        raise ValueError(f"Unexpected error extracting git information: {str(e)}") from e

def print_error(message: str, exit_code: int = 1) -> None:
    """
    Print error message to stderr and exit with specified code.

    Args:
        message: Error message to display
        exit_code: Exit code to use
    """
    print(f"ERROR: {message}", file=sys.stderr)
    sys.exit(exit_code)

def list_scenario_combinations() -> None:
    """
    List all available library/scenario combinations.
    """
    combinations = get_all_scenario_combinations()

    if not combinations:
        print("No valid library/scenario combinations found.")
        return

    print("Available library/scenario combinations:")
    for library, scenario in combinations:
        print(f"  {library}/{scenario}")

    print(f"\nTotal combinations: {len(combinations)}")

def get_user_confirmation(prompt: str) -> bool:
    """
    Get user confirmation with yes/no prompt.

    Args:
        prompt: The prompt message to display

    Returns:
        True if user confirms (yes/y), False otherwise
    """
    while True:
        response = input(f"{prompt} (yes/no): ").strip().lower()
        if response in ['yes', 'y']:
            return True
        elif response in ['no', 'n']:
            return False
        else:
            print("Please enter 'yes' or 'no'.")


def clean_build_artifacts() -> None:
    """
    Remove all build results and generated files.

    This function cleans:
    - All subdirectories in build/
    - Generated .h, .c, .pb.h, .pb.c files in Generated/
    - Does NOT remove Generated/CMakeLists.txt
    """
    print("Cleaning build artifacts...")

    # Clean build directory
    build_dir = Path('build')
    if build_dir.exists():
        # Remove all library subdirectories in build/
        for item in build_dir.iterdir():
            if item.is_dir():
                print(f"  Removing {item}")
                shutil.rmtree(item)
            else:
                # Also remove any files in the build root
                print(f"  Removing {item}")
                item.unlink()
        print(f"Cleaned build directory: {build_dir}")
    else:
        print("Build directory does not exist, nothing to clean.")

    # Clean generated files in Generated/ directory
    generated_dir = Path('Generated')
    if generated_dir.exists():
        # Patterns for generated files to remove
        patterns = ['*.h', '*.c', '*.pb.h', '*.pb.c']
        files_to_preserve = ['CMakeLists.txt']

        for pattern in patterns:
            for file_path in generated_dir.glob(pattern):
                if file_path.name not in files_to_preserve:
                    print(f"  Removing {file_path}")
                    file_path.unlink()

        print(f"Cleaned generated files in: {generated_dir}")
    else:
        print("Generated directory does not exist, nothing to clean.")

    print("Clean completed.")


def build_firmware(
    library: str,
    scenario: str,
    build_type: str = 'Release',
    benchmark_mode: str = 'normal'
) -> str:
    """
    Build firmware for a specific library/scenario combination using CMake.

    Each library/scenario combination gets its own build directory to allow
    comparing build results without overwriting previous builds.

    Args:
        library: Library name (e.g., 'EmbeddedProto')
        scenario: Scenario name (e.g., 'simple_types_serialize_max')
        build_type: CMake build type ('Release' or 'Debug'), defaults to 'Release'
        benchmark_mode: Benchmark mode ('normal' or 'partial'), defaults to 'normal'

    Returns:
        Path to the built ELF file: 'build/{library}/{scenario}/bin/nucleo_f446re_app.elf'

    Raises:
        ValueError: If library/scenario combination is invalid
        FileNotFoundError: If required tools are not found
        subprocess.CalledProcessError: If build process fails
    """
    # Validate the library/scenario combination
    if not validate_library_scenario_combination(library, scenario):
        raise ValueError(f"Invalid library/scenario combination: {library}/{scenario}")

    # Validate build type
    valid_types = get_valid_build_types()
    if build_type not in valid_types:
        raise ValueError(f"Invalid build type '{build_type}'. Valid types are: {', '.join(valid_types)}")

    # Check if arm-none-eabi-gcc is available
    if not shutil.which('arm-none-eabi-gcc'):
        raise FileNotFoundError(
            "arm-none-eabi-gcc not found. Please install the ARM toolchain. "
            "On Ubuntu/Debian: sudo apt install gcc-arm-none-eabi"
        )

    # Check if cmake is available
    if not shutil.which('cmake'):
        raise FileNotFoundError(
            "cmake not found. Please install CMake. "
            "On Ubuntu/Debian: sudo apt install cmake"
        )

    # Validate benchmark mode
    if benchmark_mode not in ['normal', 'partial']:
        raise ValueError(f"Invalid benchmark mode '{benchmark_mode}'. Valid modes are: normal, partial")

    # Use library/mode/scenario-specific build directory
    # This prevents CMake cache contamination between benchmark modes.
    build_dir = Path('build') / library / benchmark_mode / scenario
    elf_path = build_dir / 'bin' / 'nucleo_f446re_app.elf'

    # Create build directory if it doesn't exist (no need to clean - CMake handles incremental builds)
    if not build_dir.exists():
        print(f"Creating build directory: {build_dir}")
        build_dir.mkdir(parents=True)

    print(f"Building {library}/{scenario} ({build_type}, mode={benchmark_mode})...")
    print(f"Build directory: {build_dir}")
    print(f"Optimization flags: {get_optimization_flags(build_type)}")

    try:
        # Run cmake configuration with build type
        print("Running cmake configuration...")
        cmake_config_result = subprocess.run(
            ['cmake', '-B', str(build_dir),
             f'-DLIBRARY={library}',
             f'-DSCENARIO={scenario}',
             f'-DBENCHMARK_MODE={benchmark_mode}',
             f'-DCMAKE_BUILD_TYPE={build_type}'],
            capture_output=True,
            text=True,
            check=True
        )
        print("CMake configuration completed successfully.")

        # Run cmake build
        print("Running cmake build...")
        cmake_build_result = subprocess.run(
            ['cmake', '--build', str(build_dir)],
            capture_output=True,
            text=True,
            check=True
        )
        print("CMake build completed successfully.")

        # Verify ELF file exists
        if not elf_path.exists():
            raise FileNotFoundError(f"Expected ELF file not found: {elf_path}")

        print(f"Build successful. ELF file: {elf_path}")
        return str(elf_path)

    except subprocess.CalledProcessError as e:
        # Display build error information
        error_output = e.stderr or e.stdout or "Unknown build error"
        print(f"Build failed with return code {e.returncode}:", file=sys.stderr)
        print(error_output, file=sys.stderr)

        # Check for common error patterns
        if "arm-none-eabi-gcc" in error_output and "not found" in error_output:
            raise FileNotFoundError(
                "arm-none-eabi-gcc not found. Please install the ARM toolchain. "
                "On Ubuntu/Debian: sudo apt install gcc-arm-none-eabi"
            ) from e
        elif "cmake" in error_output and "not found" in error_output:
            raise FileNotFoundError(
                "cmake not found. Please install CMake. "
                "On Ubuntu/Debian: sudo apt install cmake"
            ) from e
        else:
            # Re-raise with additional context
            raise subprocess.CalledProcessError(
                e.returncode,
                e.cmd,
                output=error_output
            ) from e

def extract_static_sizes(elf_path: str) -> StaticSizes:
    """
    Extract static size metrics from ELF file using arm-none-eabi-size.

    Args:
        elf_path: Path to the ELF file

    Returns:
        StaticSizes dataclass with flash_usage_bytes and static_ram_usage_bytes

    Raises:
        FileNotFoundError: If ELF file doesn't exist
        FileNotFoundError: If arm-none-eabi-size is not installed
        subprocess.CalledProcessError: If arm-none-eabi-size fails to run
    """
    # Validate ELF file exists
    elf_file = Path(elf_path)
    if not elf_file.exists():
        raise FileNotFoundError(f"ELF file not found: {elf_path}")

    # Check if arm-none-eabi-size is available
    if not shutil.which('arm-none-eabi-size'):
        raise FileNotFoundError(
            "arm-none-eabi-size not found. Please install the ARM toolchain. "
            "On Ubuntu/Debian: sudo apt install binutils-arm-none-eabi"
        )

    # Check if arm-none-eabi-nm is available for more accurate RAM calculation
    nm_available = shutil.which('arm-none-eabi-nm') is not None

    try:
        # Run arm-none-eabi-size (default format gives text, data, bss summary)
        result = subprocess.run(
            ['arm-none-eabi-size', elf_path],
            capture_output=True,
            text=True,
            check=True
        )

        # Parse output - expected format: "text    data     bss     dec     hex filename"
        lines = result.stdout.strip().split('\n')
        if len(lines) < 1:
            raise ValueError(f"Unexpected output format from arm-none-eabi-size: {result.stdout}")

        # Find the first line that contains numeric data (skip header line)
        numeric_line = None
        for line in lines:
            line = line.strip()
            if line and line[0].isdigit():
                numeric_line = line
                break

        if not numeric_line:
            raise ValueError(f"Could not find numeric data in arm-none-eabi-size output: {result.stdout}")

        # Parse the numeric line
        parts = numeric_line.split()
        if len(parts) < 4:
            raise ValueError(f"Unexpected output format from arm-none-eabi-size: {result.stdout}")

        # Extract text, data, bss values (they should be the first 3 numeric values)
        try:
            text_size = int(parts[0])
            data_size = int(parts[1])
            bss_size = int(parts[2])
        except (ValueError, IndexError) as e:
            raise ValueError(f"Could not parse size values from arm-none-eabi-size output: {result.stdout}") from e

        return StaticSizes(
            text_bytes=text_size,
            data_bytes=data_size,
            bss_bytes=bss_size
        )

    except subprocess.CalledProcessError as e:
        error_output = e.stderr or e.stdout or "Unknown error from arm-none-eabi-size"
        raise subprocess.CalledProcessError(
            e.returncode,
            e.cmd,
            output=f"arm-none-eabi-size failed: {error_output}"
        ) from e

def _calculate_static_ram_with_nm(elf_path: str, data_size: int) -> int:
    """
    Calculate static RAM usage using arm-none-eabi-nm.

    Args:
        elf_path: Path to the ELF file
        data_size: Data size from arm-none-eabi-size (used as fallback)

    Returns:
        Static RAM usage in bytes

    Raises:
        subprocess.CalledProcessError: If arm-none-eabi-nm fails to run
    """
    try:
        # Run arm-none-eabi-nm to get symbol addresses
        result = subprocess.run(
            ['arm-none-eabi-nm', '--defined-only', '--format=sysv', '--radix=d', elf_path],
            capture_output=True,
            text=True,
            check=True
        )

        # Parse symbol addresses
        ebss_addr = None
        sdata_addr = None

        for line in result.stdout.split('\n'):
            line = line.strip()
            if not line:
                continue

            # Parse nm output - it can be either space-separated or pipe-separated
            if '|' in line:
                # Pipe-separated format: symbol|address|type|size|section
                parts = line.split('|')
                if len(parts) >= 2:
                    symbol_name = parts[0].strip()
                    try:
                        address = int(parts[1].strip())
                        if symbol_name == '_ebss':
                            ebss_addr = address
                        elif symbol_name == '_sdata':
                            sdata_addr = address
                    except ValueError:
                        continue
            else:
                # Space-separated format: address type size section symbol
                parts = line.split()
                if len(parts) >= 4:
                    symbol_name = parts[-1]
                    if symbol_name == '_ebss':
                        ebss_addr = int(parts[0])
                    elif symbol_name == '_sdata':
                        sdata_addr = int(parts[0])

        # Calculate static RAM usage
        if ebss_addr is not None and sdata_addr is not None and ebss_addr > sdata_addr:
            # Static RAM usage is the difference between _ebss and _sdata
            return ebss_addr - sdata_addr
        else:
            # Fallback to data_size if we can't get proper symbols
            return data_size

    except subprocess.CalledProcessError as e:
        # If nm fails, fallback to data_size
        return data_size

    except subprocess.CalledProcessError as e:
        # Display build error information
        error_output = e.stderr or e.stdout or "Unknown build error"
        print(f"Build failed with return code {e.returncode}:", file=sys.stderr)
        print(error_output, file=sys.stderr)

        # Check for common error patterns
        if "arm-none-eabi-gcc" in error_output and "not found" in error_output:
            raise FileNotFoundError(
                "arm-none-eabi-gcc not found. Please install the ARM toolchain. "
                "On Ubuntu/Debian: sudo apt install gcc-arm-none-eabi"
            ) from e
        elif "cmake" in error_output and "not found" in error_output:
            raise FileNotFoundError(
                "cmake not found. Please install CMake. "
                "On Ubuntu/Debian: sudo apt install cmake"
            ) from e
        else:
            # Re-raise with additional context
            raise subprocess.CalledProcessError(
                e.returncode,
                e.cmd,
                output=error_output
            ) from e

def flash_device(elf_path: str) -> bool:
    """
    Flash the device using OpenOCD.

    Args:
        elf_path: Path to the ELF file to flash

    Returns:
        True if flashing succeeded, False if failed

    Raises:
        FileNotFoundError: If OpenOCD is not installed
    """
    # Check if OpenOCD is installed
    if not shutil.which('openocd'):
        raise FileNotFoundError(
            "openocd not found. Please install OpenOCD. "
            "On Ubuntu/Debian: sudo apt install openocd"
        )

    # Check if ELF file exists
    elf_file = Path(elf_path)
    if not elf_file.exists():
        raise FileNotFoundError(f"ELF file not found: {elf_path}")

    print(f"Flashing device with {elf_path}...")

    # OpenOCD command with retry logic (up to 3 attempts)
    openocd_cmd = [
        'openocd',
        '-f', 'interface/stlink.cfg',
        '-f', 'target/stm32f4x.cfg',
        '-c', f'program {elf_path} verify reset exit'
    ]

    max_attempts = 3
    attempt = 1

    while attempt <= max_attempts:
        print(f"Flashing attempt {attempt}/{max_attempts}...")

        try:
            # Run OpenOCD command
            result = subprocess.run(
                openocd_cmd,
                capture_output=True,
                text=True,
                check=True
            )

            print("Device flashing completed successfully.")
            return True

        except subprocess.CalledProcessError as e:
            error_output = e.stderr or e.stdout or "Unknown flashing error"

            # Check for ST-Link connection errors
            if any(pattern in error_output.lower() for pattern in
                   ['error:', 'failed', 'no device found', 'st-link', 'connection failed']):
                if attempt < max_attempts:
                    print(f"ST-Link connection issue detected. Retrying in 2 seconds... (Attempt {attempt}/{max_attempts})")
                    print(f"Error details: {error_output.strip()}")
                    time.sleep(2)
                    attempt += 1
                    continue
                else:
                    print(f"ST-Link connection failed after {max_attempts} attempts.")
                    print(f"Error: {error_output.strip()}")
                    print("Please ensure your STM32 Nucleo-F446RE board is connected via ST-Link.")
                    return False
            else:
                print(f"Flashing failed: {error_output.strip()}")
                return False

        except Exception as e:
            print(f"Unexpected error during flashing: {str(e)}")
            return False

    return False

def read_dynamic_metrics(elf_path: str, delay_seconds: float = 2) -> DynamicMetrics:
    """
    Read dynamic metrics (CPU cycles) from the running device via OpenOCD/SWD.

    Args:
        elf_path: Path to the ELF file
        delay_seconds: Time to wait for scenario to complete (default: 0.5 seconds)

    Returns:
        DynamicMetrics dataclass with cpu_cycles
        Returns None values if device is not flashed or metrics cannot be read

    Raises:
        FileNotFoundError: If required tools (arm-none-eabi-nm, openocd) are not installed
    """
    # Check if OpenOCD is installed
    if not shutil.which('openocd'):
        raise FileNotFoundError(
            "openocd not found. Please install OpenOCD. "
            "On Ubuntu/Debian: sudo apt install openocd"
        )

    # Check if arm-none-eabi-nm is available
    if not shutil.which('arm-none-eabi-nm'):
        raise FileNotFoundError(
            "arm-none-eabi-nm not found. Please install the ARM toolchain. "
            "On Ubuntu/Debian: sudo apt install binutils-arm-none-eabi"
        )

    # Check if ELF file exists
    elf_file = Path(elf_path)
    if not elf_file.exists():
        print("Warning: ELF file not found for dynamic metrics reading")
        return DynamicMetrics(cpu_cycles=None, n_bytes_in_buffer=None)

    print(f"Reading dynamic metrics from device...")

    try:
        # Get symbol addresses using arm-none-eabi-nm
        nm_result = subprocess.run(
            ['arm-none-eabi-nm', '--defined-only', '--format=sysv', '--radix=d', elf_path],
            capture_output=True,
            text=True,
            check=True
        )

        # Parse symbol addresses
        g_cpu_cycles_used_addr = None
        g_n_bytes_in_buffer_addr = None

        for line in nm_result.stdout.split('\n'):
            line = line.strip()
            if not line:
                continue

            # Parse nm output - it can be either space-separated or pipe-separated
            if '|' in line:
                # Pipe-separated format: symbol|address|type|size|section
                parts = line.split('|')
                if len(parts) >= 2:
                    symbol_name = parts[0].strip()
                    try:
                        address = int(parts[1].strip())
                        if symbol_name == 'g_cpu_cycles_used':
                            g_cpu_cycles_used_addr = address
                        elif symbol_name == 'g_n_bytes_in_buffer':
                            g_n_bytes_in_buffer_addr = address
                    except ValueError:
                        continue
            else:
                # Space-separated format: address type size section symbol
                parts = line.split()
                if len(parts) >= 4:
                    symbol_name = parts[-1]
                    if symbol_name == 'g_cpu_cycles_used':
                        g_cpu_cycles_used_addr = int(parts[0])
                    elif symbol_name == 'g_n_bytes_in_buffer':
                        g_n_bytes_in_buffer_addr = int(parts[0])

        if g_cpu_cycles_used_addr is None:
            print("Warning: Could not find required symbols in ELF file")
            return DynamicMetrics(cpu_cycles=None, n_bytes_in_buffer=None)

        print(f"Found symbols: g_cpu_cycles_used at 0x{g_cpu_cycles_used_addr:08X}")
        if g_n_bytes_in_buffer_addr is not None:
            print(f"  g_n_bytes_in_buffer at 0x{g_n_bytes_in_buffer_addr:08X}")
        else:
            print("  g_n_bytes_in_buffer not found (buffer size measurement not available)")

        # Wait for scenario to complete
        print(f"Waiting {delay_seconds} seconds for scenario to complete...")
        time.sleep(delay_seconds)

        # Read memory using OpenOCD via command-line arguments
        # Convert addresses to hex format for OpenOCD mdw command
        addr1_hex = f"0x{g_cpu_cycles_used_addr:08x}"

        # Build OpenOCD command list
        openocd_cmd = [
            'openocd',
            '-f', 'interface/stlink.cfg',
            '-f', 'target/stm32f4x.cfg',
            '-c', 'init',
            '-c', 'halt',
            '-c', f'mdw {addr1_hex}',
        ]

        # Add n_bytes_in_buffer reading if available
        if g_n_bytes_in_buffer_addr is not None:
            addr2_hex = f"0x{g_n_bytes_in_buffer_addr:08x}"
            openocd_cmd.extend([
                '-c', f'mdw {addr2_hex}',
            ])

        openocd_cmd.extend([
            '-c', 'resume',
            '-c', 'exit'
        ])

        try:
            # Run OpenOCD with commands passed via -c options
            result = subprocess.run(
                openocd_cmd,
                capture_output=True,
                text=True,
                check=True
            )

            # Parse OpenOCD output to extract memory values
            # OpenOCD outputs to stderr, so check both stdout and stderr
            all_output = result.stdout + "\n" + result.stderr
            cpu_cycles = None
            n_bytes_in_buffer = None

            # OpenOCD mdw output format: "0x20000028: 12345678"
            addr1_lower = f"0x{g_cpu_cycles_used_addr:08x}"
            addr2_lower = f"0x{g_n_bytes_in_buffer_addr:08x}" if g_n_bytes_in_buffer_addr is not None else None

            for line in all_output.split('\n'):
                line = line.strip().lower()
                if not line:
                    continue

                # Check if line contains the address
                if addr1_lower in line:
                    # Parse memory value: "0xaddress: value"
                    parts = line.split(':')
                    if len(parts) >= 2:
                        try:
                            # Value might be space-separated after colon
                            value_str = parts[1].strip().split()[0]
                            cpu_cycles = int(value_str, 16)
                        except (ValueError, IndexError):
                            pass
                elif addr2_lower is not None and addr2_lower in line:
                    # Parse memory value: "0xaddress: value"
                    parts = line.split(':')
                    if len(parts) >= 2:
                        try:
                            # Value might be space-separated after colon
                            value_str = parts[1].strip().split()[0]
                            n_bytes_in_buffer = int(value_str, 16)
                        except (ValueError, IndexError):
                            pass

            print(f"Dynamic metrics read successfully:")
            print(f"  CPU cycles: {cpu_cycles}" if cpu_cycles is not None else "  CPU cycles: N/A")
            print(f"  Buffer bytes: {n_bytes_in_buffer}" if n_bytes_in_buffer is not None else "  Buffer bytes: N/A")

            return DynamicMetrics(
                cpu_cycles=cpu_cycles,
                n_bytes_in_buffer=n_bytes_in_buffer
            )

        except subprocess.CalledProcessError as e:
            error_output = e.stderr or e.stdout or "Unknown OpenOCD error"
            print(f"Warning: Could not read dynamic metrics: {error_output.strip()}")
            return DynamicMetrics(cpu_cycles=None, n_bytes_in_buffer=None)

    except subprocess.CalledProcessError as e:
        print(f"Warning: Could not extract symbol information: {str(e)}")
        return DynamicMetrics(cpu_cycles=None, n_bytes_in_buffer=None)
    except Exception as e:
        print(f"Warning: Unexpected error reading dynamic metrics: {str(e)}")
        return DynamicMetrics(cpu_cycles=None, n_bytes_in_buffer=None)

def init_database() -> None:
    """
    Initialize the SQLite database with the required schema.

    Creates the benchmark_results table if it doesn't exist.
    """
    db_path = Path('benchmark_results.db')

    try:
        with sqlite3.connect(db_path) as conn:
            cursor = conn.cursor()

            # Create the benchmark_results table with the specified schema
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS benchmark_results (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    timestamp DATETIME NOT NULL,
                    library TEXT NOT NULL,
                    scenario TEXT NOT NULL,
                    benchmark_mode TEXT,
                    text_bytes INTEGER NOT NULL,
                    data_bytes INTEGER NOT NULL,
                    bss_bytes INTEGER NOT NULL,
                    cpu_cycles INTEGER,
                    n_bytes_in_buffer INTEGER,
                    library_commit_hash TEXT NOT NULL,
                    library_branch TEXT,
                    library_tag TEXT,
                    library_modified BOOLEAN NOT NULL,
                    build_type TEXT,
                    optimization_flags TEXT
                )
            ''')

            # Add bss_bytes column if it doesn't exist (for existing databases migrating from old schema)
            try:
                cursor.execute('ALTER TABLE benchmark_results ADD COLUMN bss_bytes INTEGER')
            except sqlite3.OperationalError:
                # Column already exists
                pass

            # Add benchmark_mode column if it doesn't exist (for existing databases)
            try:
                cursor.execute('ALTER TABLE benchmark_results ADD COLUMN benchmark_mode TEXT')
            except sqlite3.OperationalError:
                # Column already exists
                pass

            # Backfill legacy rows created before benchmark_mode existed.
            cursor.execute(
                "UPDATE benchmark_results SET benchmark_mode = 'normal' WHERE benchmark_mode IS NULL"
            )

            # Add n_bytes_in_buffer column if it doesn't exist (for existing databases)
            try:
                cursor.execute('ALTER TABLE benchmark_results ADD COLUMN n_bytes_in_buffer INTEGER')
            except sqlite3.OperationalError:
                # Column already exists
                pass

            # Add build_type column if it doesn't exist (for existing databases)
            try:
                cursor.execute('ALTER TABLE benchmark_results ADD COLUMN build_type TEXT')
            except sqlite3.OperationalError:
                # Column already exists
                pass

            # Add optimization_flags column if it doesn't exist (for existing databases)
            try:
                cursor.execute('ALTER TABLE benchmark_results ADD COLUMN optimization_flags TEXT')
            except sqlite3.OperationalError:
                # Column already exists
                pass

            # Create index for faster queries on library and scenario
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_library_scenario
                ON benchmark_results(library, scenario, timestamp)
            ''')

            conn.commit()
            print(f"Database initialized successfully: {db_path}")

    except sqlite3.Error as e:
        print(f"Warning: Database initialization failed: {str(e)}")
        # Continue execution even if database fails

def store_result(result: BenchmarkResult) -> None:
    """
    Store a benchmark result in the SQLite database.

    Args:
        result: BenchmarkResult dataclass containing all benchmark metrics

    Note: Database errors produce clear messages without crashing the script.
    """
    db_path = Path('benchmark_results.db')

    try:
        with sqlite3.connect(db_path) as conn:
            cursor = conn.cursor()

            # Insert the benchmark result
            cursor.execute('''
                INSERT INTO benchmark_results (
                    timestamp, library, scenario, benchmark_mode, text_bytes, data_bytes, bss_bytes,
                    cpu_cycles, n_bytes_in_buffer,
                    library_commit_hash, library_branch, library_tag, library_modified,
                    build_type, optimization_flags
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            ''', (
                result.timestamp,
                result.library,
                result.scenario,
                result.benchmark_mode,
                result.text_bytes,
                result.data_bytes,
                result.bss_bytes,
                result.cpu_cycles,
                result.n_bytes_in_buffer,
                result.library_commit_hash,
                result.library_branch,
                result.library_tag,
                1 if result.library_modified else 0,
                result.build_type,
                result.optimization_flags
            ))

            conn.commit()
            print(f"Benchmark result stored in database for {result.library}/{result.scenario} ({result.build_type})")

    except sqlite3.Error as e:
        print(f"Warning: Failed to store benchmark result in database: {str(e)}")
        # Continue execution even if database fails

def get_latest_results(
    library: Optional[str] = None,
    scenario: Optional[str] = None,
    benchmark_mode: Optional[str] = None
) -> List[BenchmarkResult]:
    """
    Query recent benchmark results with optional filtering.

    Args:
        library: Optional library filter
        scenario: Optional scenario filter
        benchmark_mode: Optional benchmark mode filter

    Returns:
        List of BenchmarkResult objects matching the criteria
    """
    db_path = Path('benchmark_results.db')
    results = []

    if not db_path.exists():
        return results  # Return empty list if database doesn't exist

    try:
        with sqlite3.connect(db_path) as conn:
            cursor = conn.cursor()

            # Build query based on filters.
            # Use explicit column ordering to remain compatible with migrated
            # databases where column insertion order can differ.
            query = '''
                SELECT
                    timestamp,
                    library,
                    scenario,
                    COALESCE(benchmark_mode, 'normal') AS benchmark_mode,
                    text_bytes,
                    data_bytes,
                    bss_bytes,
                    cpu_cycles,
                    n_bytes_in_buffer,
                    library_commit_hash,
                    library_branch,
                    library_tag,
                    library_modified,
                    build_type,
                    optimization_flags
                FROM benchmark_results
            '''
            conditions = []
            params = []

            if library:
                conditions.append('library = ?')
                params.append(library)
            if scenario:
                conditions.append('scenario = ?')
                params.append(scenario)
            if benchmark_mode:
                conditions.append('benchmark_mode = ?')
                params.append(benchmark_mode)

            if conditions:
                query += ' WHERE ' + ' AND '.join(conditions)

            query += ' ORDER BY timestamp DESC'

            cursor.execute(query, params)

            # Fetch all results and convert to BenchmarkResult objects
            for row in cursor.fetchall():
                result = BenchmarkResult(
                    timestamp=row[0],
                    library=row[1],
                    scenario=row[2],
                    benchmark_mode=row[3],
                    text_bytes=row[4],
                    data_bytes=row[5],
                    bss_bytes=row[6],
                    cpu_cycles=row[7],
                    n_bytes_in_buffer=row[8],
                    library_commit_hash=row[9],
                    library_branch=row[10],
                    library_tag=row[11],
                    library_modified=bool(row[12]),
                    build_type=row[13] if row[13] else 'Release',
                    optimization_flags=row[14] if row[14] else get_optimization_flags('Release')
                )
                results.append(result)

    except sqlite3.Error as e:
        print(f"Warning: Failed to query benchmark results: {str(e)}")

    return results

def create_benchmark_result(
    library: str,
    scenario: str,
    static_sizes: StaticSizes,
    dynamic_metrics: DynamicMetrics,
    git_info: Optional[GitInfo],
    build_type: str = 'Release',
    benchmark_mode: str = 'normal'
) -> BenchmarkResult:
    """
    Create a BenchmarkResult object from collected metrics.

    Args:
        library: Library name
        scenario: Scenario name
        static_sizes: Static size metrics
        dynamic_metrics: Dynamic metrics (may contain None values)
        git_info: Git information (may be None for Baseline)
        build_type: CMake build type ('Release' or 'Debug'), defaults to 'Release'
        benchmark_mode: Benchmark mode ('normal' or 'partial'), defaults to 'normal'

    Returns:
        BenchmarkResult object ready for database storage
    """
    # Use current UTC timestamp
    timestamp = datetime.now(UTC).isoformat()

    # Handle Baseline case where git_info is None
    if git_info is None:
        git_info = GitInfo(
            commit_hash="N/A",
            branch=None,
            tag=None,
            is_modified=False
        )

    # Get optimization flags for the build type by parsing CMakeLists.txt
    optimization_flags = get_optimization_flags(build_type)

    return BenchmarkResult(
        library=library,
        scenario=scenario,
        benchmark_mode=benchmark_mode,
        text_bytes=static_sizes.text_bytes,
        data_bytes=static_sizes.data_bytes,
        bss_bytes=static_sizes.bss_bytes,
        cpu_cycles=dynamic_metrics.cpu_cycles,
        n_bytes_in_buffer=dynamic_metrics.n_bytes_in_buffer,
        library_commit_hash=git_info.commit_hash,
        library_branch=git_info.branch,
        library_tag=git_info.tag,
        library_modified=git_info.is_modified,
        timestamp=timestamp,
        build_type=build_type,
        optimization_flags=optimization_flags
    )

def display_results(results: List[BenchmarkResult]) -> None:
    """
    Display benchmark results in a formatted table.

    Args:
        results: List of BenchmarkResult objects to display

    Acceptance Criteria:
    - Table columns: Library, Scenario, text (bytes), data (bytes), bss (bytes), CPU Cycles, Commit
    - CPU Cycles show 'N/A' if not measured (--flash not used)
    - Commit hash is truncated to first 8 characters
    - Table header is clearly separated from data rows
    - Summary row shows totals or averages if multiple results
    - Table is readable without horizontal scrolling on 120-char terminal
    """
    if not results:
        print("No benchmark results to display.")
        return

    # Prepare table data
    table_data = []
    total_text = 0
    total_data = 0
    total_bss = 0
    total_cpu_cycles = 0

    for result in results:
        # Format each row
        row = [
            result.library,
            result.scenario,
            result.benchmark_mode,
            f"{result.text_bytes:,}",
            f"{result.data_bytes:,}",
            f"{result.bss_bytes:,}",
            f"{result.cpu_cycles:,}" if result.cpu_cycles is not None else "N/A",
            result.library_commit_hash[:8] if result.library_commit_hash else "N/A"
        ]
        table_data.append(row)

        # Accumulate totals for summary
        total_text += result.text_bytes
        total_data += result.data_bytes
        total_bss += result.bss_bytes
        if result.cpu_cycles is not None:
            total_cpu_cycles += result.cpu_cycles

    # Define table headers
    headers = [
        "Library",
        "Scenario",
        "Mode",
        "text (bytes)",
        "data (bytes)",
        "bss (bytes)",
        "CPU Cycles",
        "Commit"
    ]

    # Display table using tabulate if available, otherwise manual formatting
    if TABULATE_AVAILABLE:
        print("\nBenchmark Results:")
        print(tabulate(table_data, headers=headers, tablefmt="grid"))

        # Add summary row if multiple results
        if len(results) > 1:
            summary_data = [
                ["TOTAL/AVG", "", "", f"{total_text:,}", f"{total_data:,}", f"{total_bss:,}"]
            ]
            if total_cpu_cycles > 0:
                summary_data[0].append(f"{total_cpu_cycles:,}")
            else:
                summary_data[0].append("N/A")

            summary_data[0].append("")

            print("\nSummary:")
            print(tabulate(summary_data, headers=headers, tablefmt="grid"))
    else:
        # Manual formatting fallback
        print("\nBenchmark Results:")

        # Calculate column widths
        col_widths = [max(len(str(item)) for item in col) for col in zip(*[headers] + table_data)]

        # Print header with separator
        header_line = " | ".join(f"{header:<{width}}" for header, width in zip(headers, col_widths))
        separator_line = "-+-".join("-" * width for width in col_widths)

        print(header_line)
        print(separator_line)

        # Print data rows
        for row in table_data:
            row_line = " | ".join(f"{item:<{width}}" for item, width in zip(row, col_widths))
            print(row_line)

        # Add summary row if multiple results
        if len(results) > 1:
            print(separator_line)

            # Build summary row
            summary_row = ["TOTAL/AVG", "", ""]
            summary_row.extend([f"{total_text:,}", f"{total_data:,}", f"{total_bss:,}"])

            if total_cpu_cycles > 0:
                summary_row.append(f"{total_cpu_cycles:,}")
            else:
                summary_row.append("N/A")

            summary_row.append("")

            summary_line = " | ".join(f"{item:<{width}}" for item, width in zip(summary_row, col_widths))
            print(summary_line)

    print(f"\nTotal results: {len(results)}")

def check_dependencies(skip_checks: bool = False, verbose: bool = False) -> bool:
    """
    Check if all required tools are installed.

    Args:
        skip_checks: If True, skip dependency checking (for CI/advanced users)
        verbose: If True, display detailed information including tool versions

    Returns:
        True if all required tools are available, False otherwise

    Raises:
        SystemExit: If required tools are missing and skip_checks is False

    Acceptance Criteria:
    - Checks for: arm-none-eabi-gcc, arm-none-eabi-size, arm-none-eabi-nm, cmake, openocd (if --flash)
    - Uses shutil.which() to locate executables
    - Missing required tool: exit with clear installation instructions
    - Missing optional tool (openocd without --flash): warning only
    - Displays version of each found tool with --verbose
    """
    if skip_checks:
        if verbose:
            print("Skipping dependency checks as requested")
        return True

    print("Checking required tools...")

    # Define required tools and their installation instructions
    required_tools = {
        'arm-none-eabi-gcc': {
            'install': 'sudo apt install gcc-arm-none-eabi',
            'description': 'ARM GCC compiler'
        },
        'arm-none-eabi-size': {
            'install': 'sudo apt install binutils-arm-none-eabi',
            'description': 'ARM size utility'
        },
        'arm-none-eabi-nm': {
            'install': 'sudo apt install binutils-arm-none-eabi',
            'description': 'ARM symbol table utility'
        },
        'cmake': {
            'install': 'sudo apt install cmake',
            'description': 'CMake build system'
        }
    }

    # Define optional tools
    optional_tools = {
        'openocd': {
            'install': 'sudo apt install openocd',
            'description': 'OpenOCD for device flashing and debugging'
        }
    }

    all_tools_ok = True
    missing_required = []
    missing_optional = []

    # Check required tools
    for tool_name, tool_info in required_tools.items():
        tool_path = shutil.which(tool_name)
        if tool_path:
            if verbose:
                try:
                    # Try to get version information
                    version_result = subprocess.run(
                        [tool_name, '--version'],
                        capture_output=True,
                        text=True,
                        timeout=5
                    )
                    version_info = version_result.stdout.strip().split('\n')[0]
                    print(f"✓ {tool_name}: Found at {tool_path}")
                    print(f"  Version: {version_info}")
                except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
                    print(f"✓ {tool_name}: Found at {tool_path}")
        else:
            missing_required.append((tool_name, tool_info))
            all_tools_ok = False

    # Check optional tools (only warn if missing)
    for tool_name, tool_info in optional_tools.items():
        tool_path = shutil.which(tool_name)
        if tool_path:
            if verbose:
                try:
                    # Try to get version information
                    version_result = subprocess.run(
                        [tool_name, '--version'],
                        capture_output=True,
                        text=True,
                        timeout=5
                    )
                    version_info = version_result.stdout.strip().split('\n')[0]
                    print(f"✓ {tool_name}: Found at {tool_path} (optional)")
                    print(f"  Version: {version_info}")
                except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
                    print(f"✓ {tool_name}: Found at {tool_path} (optional)")
        else:
            missing_optional.append((tool_name, tool_info))

    # Report missing required tools
    if missing_required:
        print("\nERROR: Missing required tools:", file=sys.stderr)
        for tool_name, tool_info in missing_required:
            print(f"  - {tool_name} ({tool_info['description']})", file=sys.stderr)
            print(f"    Install with: {tool_info['install']}", file=sys.stderr)

        print("\nPlease install the missing tools and try again.", file=sys.stderr)
        return False

    # Report missing optional tools (warning only)
    if missing_optional and verbose:
        print("\nWARNING: Missing optional tools (not required for basic functionality):")
        for tool_name, tool_info in missing_optional:
            print(f"  - {tool_name} ({tool_info['description']})", file=sys.stderr)
            print(f"    Install with: {tool_info['install']}", file=sys.stderr)

    if all_tools_ok:
        print("✓ All required tools are available.")
        return True
    else:
        return False

def main() -> None:
    """
    Main entry point for the benchmark script.
    """
    # Parse arguments first to check for --skip-checks and --verbose flags
    # We need to parse early to get these flags before running dependency checks
    temp_parser = argparse.ArgumentParser(add_help=False)
    temp_parser.add_argument('--skip-checks', action='store_true')
    temp_parser.add_argument('--verbose', action='store_true')
    temp_args, remaining_args = temp_parser.parse_known_args()

    # Run dependency checks early to fail fast if required tools are missing
    if not check_dependencies(skip_checks=temp_args.skip_checks, verbose=temp_args.verbose):
        sys.exit(1)

    # Initialize database at startup
    init_database()

    # Set up argument parser
    parser = argparse.ArgumentParser(
        description="Benchmark EmbeddedProto vs NanoPB vs ArduinoJson on STM32 Nucleo-F446RE",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    # Add arguments
    parser.add_argument(
        '--library',
        choices=VALID_LIBRARIES,
        help="Library to benchmark (choices: ArduinoJson, EmbeddedProto, NanoPB, Baseline)"
    )

    parser.add_argument(
        '--scenario',
        help="Scenario to run (validated against existing scenario files)"
    )

    parser.add_argument(
        '--flash',
        action='store_true',
        help="Enable device flashing after build"
    )

    parser.add_argument(
        '--list-combinations',
        action='store_true',
        help="List all available library/scenario combinations and exit"
    )

    parser.add_argument(
        '--clean',
        action='store_true',
        help="Remove all build results and generated files, then exit"
    )

    parser.add_argument(
        '--skip-checks',
        action='store_true',
        help="Skip tool dependency checking (for CI/advanced users)"
    )

    parser.add_argument(
        '--verbose',
        action='store_true',
        help="Enable verbose output with detailed tool information"
    )

    parser.add_argument(
        '--mode',
        choices=['normal', 'partial'],
        default='normal',
        help="Benchmark mode for serialization path selection (default: normal)"
    )

    # Build type arguments (mutually exclusive, default: release)
    build_type_group = parser.add_mutually_exclusive_group()
    build_type_group.add_argument(
        '--release',
        action='store_true',
        default=True,
        help="Build with Release optimization (-Os, default)"
    )
    build_type_group.add_argument(
        '--debug',
        action='store_true',
        help="Build with Debug optimization (-Og)"
    )

    # Parse arguments
    args = parser.parse_args()

    # Determine build type from arguments
    build_type = 'Debug' if args.debug else 'Release'
    benchmark_mode = args.mode

    # Handle --list-combinations flag
    if args.list_combinations:
        list_scenario_combinations()
        sys.exit(0)

    # Handle --clean flag
    if args.clean:
        clean_build_artifacts()
        sys.exit(0)

    # Discover available scenarios
    scenarios = {}
    for library in VALID_LIBRARIES:
        try:
            scenarios[library] = get_scenarios_for_library(library)
        except (FileNotFoundError, ValueError):
            # Skip libraries with no valid scenarios
            continue

    # Validate arguments and determine what to run
    if args.library and args.scenario:
        # Both library and scenario specified - run single combination
        if not validate_library_scenario_combination(args.library, args.scenario):
            available_scenarios = scenarios.get(args.library, [])
            print_error(
                f"Scenario '{args.scenario}' not found for library '{args.library}'. "
                f"Available scenarios: {', '.join(available_scenarios) if available_scenarios else 'None'}"
            )

        print(
            f"Running benchmark for library '{args.library}' with scenario "
            f"'{args.scenario}' (mode={benchmark_mode})"
        )
        if args.flash:
            print("Device flashing enabled")

        # Get git information for the library
        try:
            git_info = get_library_git_info(args.library)
            if git_info:
                print(f"Library git information:")
                print(f"  Commit: {git_info.commit_hash}")
                print(f"  Branch: {git_info.branch or 'N/A'}")
                print(f"  Tag: {git_info.tag or 'N/A'}")
                print(f"  Modified: {'Yes' if git_info.is_modified else 'No'}")
            else:
                print(f"Library '{args.library}' has no external git repository")
        except Exception as e:
            print(f"Warning: Could not extract git information: {str(e)}")

        try:
            elf_path = build_firmware(args.library, args.scenario, build_type, benchmark_mode)
            print(f"Build completed successfully. ELF file: {elf_path}")

            # Extract static sizes (SCT-004 implementation)
            try:
                static_sizes = extract_static_sizes(elf_path)
                print(f"Static sizes extracted:")
                print(f"  text: {static_sizes.text_bytes} bytes")
                print(f"  data: {static_sizes.data_bytes} bytes")
                print(f"  bss: {static_sizes.bss_bytes} bytes")
            except Exception as e:
                print(f"Warning: Could not extract static sizes: {str(e)}")

            if args.flash:
                try:
                    flash_success = flash_device(elf_path)
                    if not flash_success:
                        print("Warning: Device flashing failed, but build was successful.")
                    else:
                        # Read dynamic metrics after successful flashing
                        try:
                            dynamic_metrics = read_dynamic_metrics(elf_path)
                            if dynamic_metrics.cpu_cycles is not None:
                                print(f"CPU cycles: {dynamic_metrics.cpu_cycles}")
                        except FileNotFoundError as e:
                            print(f"Warning: Could not read dynamic metrics: {str(e)}")
                        except Exception as e:
                            print(f"Warning: Unexpected error reading dynamic metrics: {str(e)}")
                except FileNotFoundError as e:
                    print(f"Warning: Could not flash device: {str(e)}")
                except Exception as e:
                    print(f"Warning: Unexpected error during flashing: {str(e)}")

            # Store results in database
            try:
                # Create benchmark result object
                benchmark_result = create_benchmark_result(
                    library=args.library,
                    scenario=args.scenario,
                    static_sizes=static_sizes,
                    dynamic_metrics=dynamic_metrics if args.flash else DynamicMetrics(None, None),
                    git_info=git_info,
                    build_type=build_type,
                    benchmark_mode=benchmark_mode
                )

                # Store in database
                store_result(benchmark_result)
            except Exception as e:
                print(f"Warning: Failed to store benchmark results: {str(e)}")

        except Exception as e:
            print_error(f"Build failed: {str(e)}")

    elif args.library:
        # Only library specified - run all scenarios for that library
        if args.library not in scenarios:
            available_libraries = get_available_libraries()
            print_error(
                f"Library '{args.library}' has no available scenarios. "
                f"Available libraries: {', '.join(available_libraries)}"
            )

        library_scenarios = scenarios[args.library]
        total_scenarios = len(library_scenarios)
        print(
            f"Running all scenarios for library '{args.library}' "
            f"(mode={benchmark_mode}): {', '.join(library_scenarios)}"
        )
        if args.flash:
            print("Device flashing enabled")

        # Run all scenarios for the specified library
        success_count = 0
        failure_count = 0

        for i, scenario in enumerate(library_scenarios, 1):
            print(f"\n[{i}/{total_scenarios}] Building {args.library}/{scenario}...")
            try:
                elf_path = build_firmware(args.library, scenario, build_type, benchmark_mode)
                print(f"Build completed successfully. ELF file: {elf_path}")

                # Extract static sizes (SCT-004 implementation)
                try:
                    static_sizes = extract_static_sizes(elf_path)
                    print(f"Static sizes extracted:")
                    print(f"  text: {static_sizes.text_bytes} bytes")
                    print(f"  data: {static_sizes.data_bytes} bytes")
                    print(f"  bss: {static_sizes.bss_bytes} bytes")
                except Exception as e:
                    print(f"Warning: Could not extract static sizes: {str(e)}")

                success_count += 1
                if args.flash:
                    try:
                        flash_success = flash_device(elf_path)
                        if not flash_success:
                            print("Warning: Device flashing failed, but build was successful.")
                        else:
                            # Read dynamic metrics after successful flashing
                            try:
                                dynamic_metrics = read_dynamic_metrics(elf_path)
                                if dynamic_metrics.cpu_cycles is not None:
                                    print(f"CPU cycles: {dynamic_metrics.cpu_cycles}")
                            except FileNotFoundError as e:
                                print(f"Warning: Could not read dynamic metrics: {str(e)}")
                            except Exception as e:
                                print(f"Warning: Unexpected error reading dynamic metrics: {str(e)}")
                    except FileNotFoundError as e:
                        print(f"Warning: Could not flash device: {str(e)}")
                    except Exception as e:
                        print(f"Warning: Unexpected error during flashing: {str(e)}")

                # Store results in database
                try:
                    # Get git information for the library
                    git_info = get_library_git_info(args.library)

                    # Create benchmark result object
                    benchmark_result = create_benchmark_result(
                        library=args.library,
                        scenario=scenario,
                        static_sizes=static_sizes,
                        dynamic_metrics=dynamic_metrics if args.flash else DynamicMetrics(None, None),
                        git_info=git_info,
                        build_type=build_type,
                        benchmark_mode=benchmark_mode
                    )

                    # Store in database
                    store_result(benchmark_result)
                except Exception as e:
                    print(f"Warning: Failed to store benchmark results for {args.library}/{scenario}: {str(e)}")

            except Exception as e:
                print_error(f"Build failed for {args.library}/{scenario}: {str(e)}")
                failure_count += 1

        print(f"\nSummary for library '{args.library}': {success_count} succeeded, {failure_count} failed, 0 skipped")

    elif args.scenario:
        # Only scenario specified - run that scenario for all libraries that have it
        libraries_with_scenario = []
        for lib in VALID_LIBRARIES:
            try:
                lib_scenarios = get_scenarios_for_library(lib)
                if args.scenario in lib_scenarios:
                    libraries_with_scenario.append(lib)
            except (FileNotFoundError, ValueError):
                continue

        if not libraries_with_scenario:
            print_error(
                f"Scenario '{args.scenario}' not found in any library. "
                f"Available scenarios vary by library."
            )

        total_libraries = len(libraries_with_scenario)
        print(
            f"Running scenario '{args.scenario}' for libraries "
            f"(mode={benchmark_mode}): {', '.join(libraries_with_scenario)}"
        )
        if args.flash:
            print("Device flashing enabled")

        # Run the scenario for all libraries that have it
        success_count = 0
        failure_count = 0

        for i, lib in enumerate(libraries_with_scenario, 1):
            print(f"\n[{i}/{total_libraries}] Building {lib}/{args.scenario}...")
            try:
                elf_path = build_firmware(lib, args.scenario, build_type, benchmark_mode)
                print(f"Build completed successfully. ELF file: {elf_path}")

                # Extract static sizes (SCT-004 implementation)
                try:
                    static_sizes = extract_static_sizes(elf_path)
                    print(f"Static sizes extracted:")
                    print(f"  text: {static_sizes.text_bytes} bytes")
                    print(f"  data: {static_sizes.data_bytes} bytes")
                    print(f"  bss: {static_sizes.bss_bytes} bytes")
                except Exception as e:
                    print(f"Warning: Could not extract static sizes: {str(e)}")

                success_count += 1
                if args.flash:
                    try:
                        flash_success = flash_device(elf_path)
                        if not flash_success:
                            print("Warning: Device flashing failed, but build was successful.")
                        else:
                            # Read dynamic metrics after successful flashing
                            try:
                                dynamic_metrics = read_dynamic_metrics(elf_path)
                                if dynamic_metrics.cpu_cycles is not None:
                                    print(f"CPU cycles: {dynamic_metrics.cpu_cycles}")
                            except FileNotFoundError as e:
                                print(f"Warning: Could not read dynamic metrics: {str(e)}")
                            except Exception as e:
                                print(f"Warning: Unexpected error reading dynamic metrics: {str(e)}")
                    except FileNotFoundError as e:
                        print(f"Warning: Could not flash device: {str(e)}")
                    except Exception as e:
                        print(f"Warning: Unexpected error during flashing: {str(e)}")

                # Store results in database
                try:
                    # Get git information for the library
                    git_info = get_library_git_info(lib)

                    # Create benchmark result object
                    benchmark_result = create_benchmark_result(
                        library=lib,
                        scenario=args.scenario,
                        static_sizes=static_sizes,
                        dynamic_metrics=dynamic_metrics if args.flash else DynamicMetrics(None, None),
                        git_info=git_info,
                        build_type=build_type,
                        benchmark_mode=benchmark_mode
                    )

                    # Store in database
                    store_result(benchmark_result)
                except Exception as e:
                    print(f"Warning: Failed to store benchmark results for {lib}/{args.scenario}: {str(e)}")

            except Exception as e:
                print_error(f"Build failed for {lib}/{args.scenario}: {str(e)}")
                failure_count += 1

        print(f"\nSummary for scenario '{args.scenario}': {success_count} succeeded, {failure_count} failed, 0 skipped")

    else:
        # Neither library nor scenario specified - run all combinations
        combinations = get_all_scenario_combinations()
        if not combinations:
            print_error("No valid library/scenario combinations found. Check Scenarios directory.")

        print(f"Running all {len(combinations)} library/scenario combinations (mode={benchmark_mode}):")
        for lib, scenario in combinations:
            print(f"  - {lib}/{scenario}")
        if args.flash:
            print("Device flashing enabled")

        # Add confirmation prompt for running all combinations
        if not get_user_confirmation(f"Do you want to run all {len(combinations)} combinations?"):
            print("Operation cancelled by user.")
            sys.exit(0)

        # Run all combinations
        success_count = 0
        failure_count = 0
        skipped_count = 0

        for i, (lib, scenario) in enumerate(combinations, 1):
            print(f"\n[{i}/{len(combinations)}] Building {lib}/{scenario}...")
            try:
                elf_path = build_firmware(lib, scenario, build_type, benchmark_mode)
                print(f"Build completed successfully. ELF file: {elf_path}")

                # Extract static sizes (SCT-004 implementation)
                try:
                    static_sizes = extract_static_sizes(elf_path)
                    print(f"Static sizes extracted:")
                    print(f"  text: {static_sizes.text_bytes} bytes")
                    print(f"  data: {static_sizes.data_bytes} bytes")
                    print(f"  bss: {static_sizes.bss_bytes} bytes")
                except Exception as e:
                    print(f"Warning: Could not extract static sizes: {str(e)}")

                success_count += 1
                if args.flash:
                    try:
                        flash_success = flash_device(elf_path)
                        if not flash_success:
                            print("Warning: Device flashing failed, but build was successful.")
                        else:
                            # Read dynamic metrics after successful flashing
                            try:
                                dynamic_metrics = read_dynamic_metrics(elf_path)
                                if dynamic_metrics.cpu_cycles is not None:
                                    print(f"CPU cycles: {dynamic_metrics.cpu_cycles}")
                            except FileNotFoundError as e:
                                print(f"Warning: Could not read dynamic metrics: {str(e)}")
                            except Exception as e:
                                print(f"Warning: Unexpected error reading dynamic metrics: {str(e)}")
                    except FileNotFoundError as e:
                        print(f"Warning: Could not flash device: {str(e)}")
                    except Exception as e:
                        print(f"Warning: Unexpected error during flashing: {str(e)}")

                # Store results in database
                try:
                    # Get git information for the library
                    git_info = get_library_git_info(lib)

                    # Create benchmark result object
                    benchmark_result = create_benchmark_result(
                        library=lib,
                        scenario=scenario,
                        static_sizes=static_sizes,
                        dynamic_metrics=dynamic_metrics if args.flash else DynamicMetrics(None, None),
                        git_info=git_info,
                        build_type=build_type,
                        benchmark_mode=benchmark_mode
                    )

                    # Store in database
                    store_result(benchmark_result)
                except Exception as e:
                    print(f"Warning: Failed to store benchmark results for {lib}/{scenario}: {str(e)}")

            except Exception as e:
                print_error(f"Build failed for {lib}/{scenario}: {str(e)}")
                failure_count += 1

        print(f"\nSummary for all combinations: {success_count} succeeded, {failure_count} failed, {skipped_count} skipped")

if __name__ == "__main__":
    main()
