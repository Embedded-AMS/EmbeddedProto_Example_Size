#!/usr/bin/env python3
"""
STM32 Flashing Script

This script automates flashing ELF/bin files to STM32 Nucleo boards via USB
using OpenOCD or st-flash.

Usage:
    python flash_stm32.py path/to/file.elf
    python flash_stm32.py path/to/file.elf --tool openocd
    python flash_stm32.py path/to/file.elf --tool stflash
    python flash_stm32.py --detect  # Check if board is connected
"""

import argparse
import subprocess
import sys
import shutil
from pathlib import Path
from dataclasses import dataclass
from typing import Optional, List, Tuple
from enum import Enum


class FlashTool(Enum):
    """Available flashing tools."""
    OPENOCD = "openocd"
    STFLASH = "st-flash"
    AUTO = "auto"


@dataclass
class FlashResult:
    """Result of a flash operation."""
    success: bool
    tool_used: str
    output: str
    error: Optional[str] = None


class STM32Flasher:
    """Class to handle STM32 flashing operations."""
    
    # OpenOCD configuration for STM32F4 with ST-Link
    OPENOCD_INTERFACE = "interface/stlink.cfg"
    OPENOCD_TARGET = "target/stm32f4x.cfg"
    
    # STM32F446RE flash base address
    FLASH_BASE_ADDRESS = 0x08000000
    
    def __init__(self):
        self.openocd_path = self._find_tool("openocd")
        self.stflash_path = self._find_tool("st-flash")
        self.stinfo_path = self._find_tool("st-info")
        self.objcopy_path = self._find_objcopy()
    
    def _find_tool(self, tool_name: str) -> Optional[str]:
        """Find a tool in PATH."""
        path = shutil.which(tool_name)
        return path
    
    def _find_objcopy(self) -> Optional[str]:
        """Find arm-none-eabi-objcopy for ELF to BIN conversion."""
        # Try arm-none-eabi-objcopy first
        path = shutil.which("arm-none-eabi-objcopy")
        if path:
            return path
        
        # Try to find in STM32CubeIDE installation
        import glob
        patterns = [
            "/home/*/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*/tools/bin/arm-none-eabi-objcopy",
            "/opt/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*/tools/bin/arm-none-eabi-objcopy",
        ]
        
        for pattern in patterns:
            matches = glob.glob(pattern)
            if matches:
                return sorted(matches, reverse=True)[0]  # Prefer newer versions
        
        return None
    
    def get_available_tools(self) -> List[str]:
        """Get list of available flashing tools."""
        tools = []
        if self.openocd_path:
            tools.append("openocd")
        if self.stflash_path:
            tools.append("st-flash")
        return tools
    
    def _run_command(self, cmd: List[str], timeout: int = 60) -> Tuple[int, str, str]:
        """Run a command and return (returncode, stdout, stderr)."""
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=timeout
            )
            return result.returncode, result.stdout, result.stderr
        except subprocess.TimeoutExpired:
            return -1, "", "Command timed out"
        except FileNotFoundError as e:
            return -1, "", f"Command not found: {e}"
    
    def detect_board_openocd(self) -> Tuple[bool, str]:
        """
        Detect if STM32 board is connected using OpenOCD.
        
        Returns:
            Tuple of (detected: bool, message: str)
        """
        if not self.openocd_path:
            return False, "OpenOCD not found"
        
        # Run OpenOCD with a simple init command that exits immediately
        cmd = [
            self.openocd_path,
            "-f", self.OPENOCD_INTERFACE,
            "-f", self.OPENOCD_TARGET,
            "-c", "init",
            "-c", "exit"
        ]
        
        returncode, stdout, stderr = self._run_command(cmd, timeout=10)
        
        # OpenOCD outputs to stderr, check for successful connection
        output = stderr + stdout
        
        if returncode == 0:
            return True, "STM32 board detected via ST-Link (OpenOCD)"
        
        # Check for common error messages
        if "no device found" in output.lower() or "unable to open" in output.lower():
            return False, "No ST-Link device found. Is the board connected?"
        if "permission denied" in output.lower():
            return False, "Permission denied. Try running with sudo or add udev rules."
        
        return False, f"Board detection failed: {output}"
    
    def detect_board_stflash(self) -> Tuple[bool, str]:
        """
        Detect if STM32 board is connected using st-info.
        
        Returns:
            Tuple of (detected: bool, message: str)
        """
        if not self.stinfo_path:
            return False, "st-info not found (install stlink-tools)"
        
        cmd = [self.stinfo_path, "--probe"]
        returncode, stdout, stderr = self._run_command(cmd, timeout=10)
        
        output = stdout + stderr
        
        if "Found" in output and "stlink" in output.lower():
            return True, f"STM32 board detected via ST-Link (st-info)\n{stdout.strip()}"
        
        if returncode != 0:
            if "permission denied" in output.lower():
                return False, "Permission denied. Try running with sudo or add udev rules."
            return False, f"Board detection failed: {output}"
        
        return False, "No ST-Link device found. Is the board connected?"
    
    def detect_board(self) -> Tuple[bool, str]:
        """
        Detect if STM32 board is connected using any available tool.
        
        Returns:
            Tuple of (detected: bool, message: str)
        """
        # Try st-info first (faster)
        if self.stinfo_path:
            detected, message = self.detect_board_stflash()
            if detected:
                return detected, message
        
        # Fall back to OpenOCD
        if self.openocd_path:
            detected, message = self.detect_board_openocd()
            if detected:
                return detected, message
        
        # No tools available
        available = self.get_available_tools()
        if not available:
            return False, "No flashing tools available. Install openocd or stlink-tools."
        
        return False, "No STM32 board detected. Is the board connected via USB?"
    
    def _convert_elf_to_bin(self, elf_path: Path) -> Optional[Path]:
        """Convert ELF file to BIN format for st-flash."""
        if not self.objcopy_path:
            return None
        
        bin_path = elf_path.with_suffix('.bin')
        
        cmd = [
            self.objcopy_path,
            "-O", "binary",
            str(elf_path),
            str(bin_path)
        ]
        
        returncode, stdout, stderr = self._run_command(cmd)
        
        if returncode == 0 and bin_path.exists():
            return bin_path
        
        return None
    
    def flash_openocd(self, file_path: Path) -> FlashResult:
        """
        Flash firmware using OpenOCD.
        
        Args:
            file_path: Path to ELF or BIN file
            
        Returns:
            FlashResult with success status and output
        """
        if not self.openocd_path:
            return FlashResult(
                success=False,
                tool_used="openocd",
                output="",
                error="OpenOCD not found"
            )
        
        # OpenOCD can flash ELF files directly
        cmd = [
            self.openocd_path,
            "-f", self.OPENOCD_INTERFACE,
            "-f", self.OPENOCD_TARGET,
            "-c", "program {} verify reset exit".format(str(file_path))
        ]
        
        print(f"Flashing with OpenOCD: {file_path}")
        returncode, stdout, stderr = self._run_command(cmd, timeout=120)
        
        output = stderr + stdout  # OpenOCD outputs to stderr
        
        if returncode == 0 and "verified" in output.lower():
            return FlashResult(
                success=True,
                tool_used="openocd",
                output=output
            )
        
        # Check for specific errors
        error_msg = None
        if "no device found" in output.lower():
            error_msg = "No ST-Link device found"
        elif "permission denied" in output.lower():
            error_msg = "Permission denied - try sudo or add udev rules"
        elif "failed" in output.lower():
            error_msg = "Flash programming failed"
        else:
            error_msg = f"Flash failed with return code {returncode}"
        
        return FlashResult(
            success=False,
            tool_used="openocd",
            output=output,
            error=error_msg
        )
    
    def flash_stflash(self, file_path: Path) -> FlashResult:
        """
        Flash firmware using st-flash.
        
        Args:
            file_path: Path to ELF or BIN file
            
        Returns:
            FlashResult with success status and output
        """
        if not self.stflash_path:
            return FlashResult(
                success=False,
                tool_used="st-flash",
                output="",
                error="st-flash not found (install stlink-tools)"
            )
        
        # st-flash requires BIN format, convert if ELF
        if file_path.suffix.lower() == '.elf':
            print(f"Converting ELF to BIN format...")
            bin_path = self._convert_elf_to_bin(file_path)
            if not bin_path:
                return FlashResult(
                    success=False,
                    tool_used="st-flash",
                    output="",
                    error="Failed to convert ELF to BIN (arm-none-eabi-objcopy not found)"
                )
            file_path = bin_path
        
        cmd = [
            self.stflash_path,
            "write",
            str(file_path),
            hex(self.FLASH_BASE_ADDRESS)
        ]
        
        print(f"Flashing with st-flash: {file_path}")
        returncode, stdout, stderr = self._run_command(cmd, timeout=120)
        
        output = stdout + stderr
        
        if returncode == 0:
            return FlashResult(
                success=True,
                tool_used="st-flash",
                output=output
            )
        
        # Check for specific errors
        error_msg = None
        if "no device found" in output.lower() or "couldn't find" in output.lower():
            error_msg = "No ST-Link device found"
        elif "permission denied" in output.lower():
            error_msg = "Permission denied - try sudo or add udev rules"
        else:
            error_msg = f"Flash failed with return code {returncode}"
        
        return FlashResult(
            success=False,
            tool_used="st-flash",
            output=output,
            error=error_msg
        )
    
    def flash(self, file_path: str, tool: FlashTool = FlashTool.AUTO) -> FlashResult:
        """
        Flash firmware to STM32 board.
        
        Args:
            file_path: Path to ELF or BIN file
            tool: Which tool to use (AUTO, OPENOCD, or STFLASH)
            
        Returns:
            FlashResult with success status and output
        """
        path = Path(file_path)
        
        if not path.exists():
            return FlashResult(
                success=False,
                tool_used="none",
                output="",
                error=f"File not found: {file_path}"
            )
        
        # Validate file extension
        if path.suffix.lower() not in ['.elf', '.bin']:
            return FlashResult(
                success=False,
                tool_used="none",
                output="",
                error=f"Unsupported file format: {path.suffix}. Use .elf or .bin"
            )
        
        # Select tool
        if tool == FlashTool.OPENOCD:
            return self.flash_openocd(path)
        elif tool == FlashTool.STFLASH:
            return self.flash_stflash(path)
        else:
            # Auto-select: prefer OpenOCD (can flash ELF directly)
            if self.openocd_path:
                return self.flash_openocd(path)
            elif self.stflash_path:
                return self.flash_stflash(path)
            else:
                return FlashResult(
                    success=False,
                    tool_used="none",
                    output="",
                    error="No flashing tools available. Install openocd or stlink-tools."
                )


def main():
    """Main entry point for standalone execution."""
    parser = argparse.ArgumentParser(
        description='Flash ELF/BIN files to STM32 Nucleo boards via USB',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python flash_stm32.py firmware.elf
    python flash_stm32.py firmware.elf --tool openocd
    python flash_stm32.py firmware.bin --tool stflash
    python flash_stm32.py --detect
    
Prerequisites:
    - OpenOCD: sudo apt install openocd
    - stlink-tools: sudo apt install stlink-tools
    
    For permission issues, add udev rules:
    sudo cp /usr/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/
    sudo udevadm control --reload-rules
    sudo udevadm trigger
        """
    )
    parser.add_argument(
        'file_path',
        nargs='?',
        help='Path to ELF or BIN file to flash'
    )
    parser.add_argument(
        '--tool', '-t',
        choices=['auto', 'openocd', 'stflash'],
        default='auto',
        help='Flashing tool to use (default: auto)'
    )
    parser.add_argument(
        '--detect', '-d',
        action='store_true',
        help='Detect if STM32 board is connected (do not flash)'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Show verbose output'
    )
    
    args = parser.parse_args()
    
    flasher = STM32Flasher()
    
    # Show available tools
    available_tools = flasher.get_available_tools()
    if args.verbose or args.detect:
        print(f"Available flashing tools: {', '.join(available_tools) if available_tools else 'none'}")
        if flasher.openocd_path:
            print(f"  OpenOCD: {flasher.openocd_path}")
        if flasher.stflash_path:
            print(f"  st-flash: {flasher.stflash_path}")
        if flasher.objcopy_path:
            print(f"  objcopy: {flasher.objcopy_path}")
        print()
    
    # Detection mode
    if args.detect:
        print("Detecting STM32 board...")
        detected, message = flasher.detect_board()
        print(message)
        return 0 if detected else 1
    
    # Flash mode - require file path
    if not args.file_path:
        parser.error("file_path is required when not using --detect")
    
    # Map tool argument to enum
    tool_map = {
        'auto': FlashTool.AUTO,
        'openocd': FlashTool.OPENOCD,
        'stflash': FlashTool.STFLASH
    }
    tool = tool_map[args.tool]
    
    # Flash the firmware
    print(f"Flashing: {args.file_path}")
    result = flasher.flash(args.file_path, tool)
    
    if result.success:
        print(f"\nFlash successful! (using {result.tool_used})")
        if args.verbose:
            print(f"\nOutput:\n{result.output}")
        return 0
    else:
        print(f"\nFlash failed: {result.error}")
        if args.verbose or result.output:
            print(f"\nOutput:\n{result.output}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
