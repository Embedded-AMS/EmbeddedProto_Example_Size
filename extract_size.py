#!/usr/bin/env python3
"""
ELF Size Extraction Script

This script extracts flash and RAM sizes from compiled ELF files using
arm-none-eabi-size.

Usage:
    python extract_size.py path/to/file.elf
    python extract_size.py path/to/file.elf --format json
    python extract_size.py path/to/file.elf --format csv
"""

import argparse
import subprocess
import json
import sys
import os
import glob
from pathlib import Path
from dataclasses import dataclass, asdict
from typing import Dict, Optional, List


@dataclass
class SizeMetrics:
    """Size metrics extracted from ELF file."""
    text: int       # Flash (code)
    data: int       # RAM (initialized data)
    bss: int        # RAM (uninitialized data)
    total_flash: int  # text + data (code + initialized data in flash)
    total_ram: int    # data + bss (total RAM usage)
    
    def to_dict(self) -> Dict[str, int]:
        """Convert to dictionary."""
        return asdict(self)
    
    def __str__(self) -> str:
        """Human-readable string representation."""
        return (
            f"Text (code):     {self.text:>10,} bytes\n"
            f"Data (init):     {self.data:>10,} bytes\n"
            f"BSS (uninit):    {self.bss:>10,} bytes\n"
            f"Total Flash:     {self.total_flash:>10,} bytes ({self.total_flash / 1024:.2f} KB)\n"
            f"Total RAM:       {self.total_ram:>10,} bytes ({self.total_ram / 1024:.2f} KB)"
        )


class ELFSizeExtractor:
    """Class to extract size metrics from ELF files."""
    
    # Known locations for ARM toolchain
    ARM_TOOLCHAIN_PATHS = [
        # STM32CubeIDE installations (common locations)
        "/home/bart/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.0.202410170706/tools/bin",
        "/home/bart/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.linux64_1.0.100.202403111256/tools/bin",
        "/opt/st/stm32cubeide_1.3.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.linux64_1.0.0.201904181610/tools/bin",
        # Standard locations
        "/usr/bin",
        "/usr/local/bin",
    ]
    
    # Glob patterns to search for ARM toolchain
    ARM_TOOLCHAIN_PATTERNS = [
        "/home/*/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*/tools/bin",
        "/opt/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*/tools/bin",
    ]
    
    def __init__(self):
        self.tool_path = self._find_arm_size_tool()
    
    def _find_arm_size_tool(self) -> Optional[str]:
        """Find the arm-none-eabi-size tool."""
        tool_name = "arm-none-eabi-size"
        
        # First check if it's in PATH
        try:
            result = subprocess.run(
                ["which", tool_name],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode == 0 and result.stdout.strip():
                return result.stdout.strip()
        except (subprocess.TimeoutExpired, FileNotFoundError):
            pass
        
        # Check known paths
        for path in self.ARM_TOOLCHAIN_PATHS:
            tool_full_path = Path(path) / tool_name
            if tool_full_path.exists():
                return str(tool_full_path)
        
        # Try glob patterns
        for pattern in self.ARM_TOOLCHAIN_PATTERNS:
            matches = glob.glob(pattern)
            for match in sorted(matches, reverse=True):  # Prefer newer versions
                tool_full_path = Path(match) / tool_name
                if tool_full_path.exists():
                    return str(tool_full_path)
        
        return None
    
    def _check_tool_available(self) -> bool:
        """Check if the size tool is available."""
        if not self.tool_path:
            return False
        
        try:
            result = subprocess.run(
                [self.tool_path, "--version"],
                capture_output=True,
                text=True,
                timeout=5
            )
            return result.returncode == 0
        except (FileNotFoundError, subprocess.TimeoutExpired):
            return False
    
    def extract_sizes(self, elf_path: str) -> SizeMetrics:
        """
        Extract size metrics from ELF file.
        
        Args:
            elf_path: Path to the ELF file
            
        Returns:
            SizeMetrics dataclass with extracted sizes
            
        Raises:
            FileNotFoundError: If ELF file doesn't exist
            RuntimeError: If arm-none-eabi-size tool is not available or fails
        """
        elf_path = Path(elf_path)
        
        if not elf_path.exists():
            raise FileNotFoundError(f"ELF file not found: {elf_path}")
        
        if not self._check_tool_available():
            raise RuntimeError(
                f"arm-none-eabi-size not found. Please install arm-none-eabi toolchain "
                f"or STM32CubeIDE."
            )
        
        # Run arm-none-eabi-size on the ELF file (Berkeley format - default)
        result = subprocess.run(
            [self.tool_path, str(elf_path)],
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            raise RuntimeError(f"Failed to extract size metrics: {result.stderr}")
        
        # Parse the output
        return self._parse_berkeley_output(result.stdout)
    
    def _parse_berkeley_output(self, output: str) -> SizeMetrics:
        """
        Parse the Berkeley format output from arm-none-eabi-size.
        
        Berkeley format example:
           text    data     bss     dec     hex filename
          23456    1234    5678   30368    76a0 file.elf
        """
        lines = output.strip().split('\n')
        
        if len(lines) < 2:
            raise RuntimeError(
                f"Unexpected output format from arm-none-eabi-size. "
                f"Expected at least 2 lines, got {len(lines)}"
            )
        
        # Parse the data line (second line)
        data_line = lines[1].strip()
        parts = data_line.split()
        
        if len(parts) < 4:
            raise RuntimeError(
                f"Unexpected output format from arm-none-eabi-size. "
                f"Expected at least 4 columns, got {len(parts)}: {data_line}"
            )
        
        try:
            text_size = int(parts[0])
            data_size = int(parts[1])
            bss_size = int(parts[2])
        except ValueError as e:
            raise RuntimeError(f"Failed to parse size values: {e}")
        
        # Calculate totals
        # Flash = text + data (code + initialized data stored in flash)
        # RAM = data + bss (initialized data copied to RAM + uninitialized data)
        total_flash = text_size + data_size
        total_ram = data_size + bss_size
        
        return SizeMetrics(
            text=text_size,
            data=data_size,
            bss=bss_size,
            total_flash=total_flash,
            total_ram=total_ram
        )
    
    def get_tool_version(self) -> Optional[str]:
        """Get the version of the arm-none-eabi-size tool."""
        if not self.tool_path:
            return None
        
        try:
            result = subprocess.run(
                [self.tool_path, "--version"],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode == 0:
                # First line typically contains version info
                return result.stdout.split('\n')[0].strip()
        except (subprocess.TimeoutExpired, FileNotFoundError):
            pass
        
        return None


def main():
    """Main entry point for standalone execution."""
    parser = argparse.ArgumentParser(
        description='Extract size metrics from ELF files using arm-none-eabi-size',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python extract_size.py firmware.elf
    python extract_size.py firmware.elf --format json
    python extract_size.py firmware.elf --format csv
    
Output sections:
    text  - Code and read-only data (stored in Flash)
    data  - Initialized data (stored in Flash, copied to RAM at startup)
    bss   - Uninitialized data (RAM only, zeroed at startup)
    
    Total Flash = text + data
    Total RAM   = data + bss
        """
    )
    parser.add_argument(
        'elf_path',
        help='Path to ELF file'
    )
    parser.add_argument(
        '--format', '-f',
        choices=['text', 'json', 'csv'],
        default='text',
        help='Output format (default: text)'
    )
    parser.add_argument(
        '--version', '-v',
        action='store_true',
        help='Show tool version information'
    )
    
    args = parser.parse_args()
    
    try:
        extractor = ELFSizeExtractor()
        
        # Show version if requested
        if args.version:
            tool_version = extractor.get_tool_version()
            if tool_version:
                print(f"Tool: {tool_version}")
            else:
                print("Tool version: unknown")
            print(f"Tool path: {extractor.tool_path or 'not found'}")
            print()
        
        # Extract metrics
        metrics = extractor.extract_sizes(args.elf_path)
        
        # Output in requested format
        if args.format == 'json':
            print(json.dumps(metrics.to_dict(), indent=2))
        elif args.format == 'csv':
            # CSV header and data
            print("text,data,bss,total_flash,total_ram")
            print(f"{metrics.text},{metrics.data},{metrics.bss},{metrics.total_flash},{metrics.total_ram}")
        else:
            # Human-readable text format
            print(f"Size metrics for: {args.elf_path}")
            print("-" * 50)
            print(metrics)
        
        return 0
        
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1
    except RuntimeError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
