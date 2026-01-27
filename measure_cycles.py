#!/usr/bin/env python3
"""
CPU Cycle Measurement Script for STM32

This script captures UART output from an STM32 board and parses CPU cycle
measurements for serialize/deserialize operations.

Usage:
    python measure_cycles.py                    # Auto-detect serial port
    python measure_cycles.py --port /dev/ttyACM0
    python measure_cycles.py --list             # List available ports
    python measure_cycles.py --timeout 10       # Custom timeout
"""

import argparse
import sys
import time
import re
from dataclasses import dataclass, asdict
from typing import Optional, List, Tuple
from enum import Enum

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("ERROR: pyserial not installed. Install with: pip install pyserial")
    sys.exit(1)


@dataclass
class CycleMetrics:
    """CPU cycle measurements from STM32."""
    serialize_cycles: int
    deserialize_cycles: int
    total_cycles: int
    library: Optional[str] = None
    scenario: Optional[str] = None
    
    def to_dict(self) -> dict:
        """Convert to dictionary."""
        return asdict(self)


@dataclass
class MeasurementResult:
    """Result of a cycle measurement operation."""
    success: bool
    metrics: Optional[CycleMetrics] = None
    raw_output: str = ""
    error: Optional[str] = None


class OutputFormat(Enum):
    """Output format options."""
    TEXT = "text"
    JSON = "json"
    CSV = "csv"


class STM32CycleMeasurer:
    """Class to capture and parse CPU cycle measurements from STM32."""
    
    # ST-Link VCP identifiers
    STLINK_VID = 0x0483  # STMicroelectronics
    STLINK_PIDS = [0x374B, 0x374E, 0x3752, 0x3753]  # Various ST-Link versions
    
    # Default serial settings (matching STM32 UART config)
    DEFAULT_BAUDRATE = 115200
    DEFAULT_TIMEOUT = 5.0  # seconds
    
    # Patterns to match in UART output
    CYCLE_PATTERNS = {
        'serialize': re.compile(r'CYCLES_SERIALIZE[:\s]+(\d+)', re.IGNORECASE),
        'deserialize': re.compile(r'CYCLES_DESERIALIZE[:\s]+(\d+)', re.IGNORECASE),
        'library': re.compile(r'LIBRARY[:\s]+(\w+)', re.IGNORECASE),
        'scenario': re.compile(r'SCENARIO[:\s]+(\w+)', re.IGNORECASE),
        'start': re.compile(r'MEASUREMENT_START', re.IGNORECASE),
        'end': re.compile(r'MEASUREMENT_END', re.IGNORECASE),
    }
    
    def __init__(self, port: Optional[str] = None, baudrate: int = DEFAULT_BAUDRATE,
                 timeout: float = DEFAULT_TIMEOUT):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial: Optional[serial.Serial] = None
    
    @classmethod
    def list_ports(cls, verbose: bool = False) -> List[Tuple[str, str, bool]]:
        """
        List available serial ports.
        
        Returns:
            List of tuples: (device, description, is_stlink)
        """
        ports = []
        for p in serial.tools.list_ports.comports():
            is_stlink = cls._is_stlink_port(p)
            if verbose or is_stlink or 'ACM' in p.device or 'USB' in p.device:
                ports.append((p.device, p.description or "Unknown", is_stlink))
        return ports
    
    @classmethod
    def _is_stlink_port(cls, port_info) -> bool:
        """Check if a port is an ST-Link VCP."""
        # Check by VID/PID
        if port_info.vid == cls.STLINK_VID:
            return True
        # Check by description
        desc = (port_info.description or "").upper()
        if 'STLINK' in desc or 'ST-LINK' in desc or 'STM32' in desc:
            return True
        return False
    
    @classmethod
    def auto_detect_port(cls) -> Optional[str]:
        """
        Auto-detect the ST-Link VCP port.
        
        Returns:
            Port device path or None if not found
        """
        for p in serial.tools.list_ports.comports():
            if cls._is_stlink_port(p):
                return p.device
        
        # Fallback: look for common patterns
        for p in serial.tools.list_ports.comports():
            if 'ACM' in p.device:  # Linux USB ACM device
                return p.device
            if 'usbmodem' in p.device.lower():  # macOS
                return p.device
        
        return None
    
    def connect(self) -> bool:
        """
        Connect to the serial port.
        
        Returns:
            True if connection successful
        """
        if self.port is None:
            self.port = self.auto_detect_port()
            if self.port is None:
                return False
        
        try:
            self.serial = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE
            )
            # Clear any pending data
            self.serial.reset_input_buffer()
            return True
        except serial.SerialException as e:
            print(f"ERROR: Could not open serial port {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from the serial port."""
        if self.serial and self.serial.is_open:
            self.serial.close()
            self.serial = None
    
    def _parse_output(self, output: str) -> Optional[CycleMetrics]:
        """
        Parse UART output to extract cycle measurements.
        
        Args:
            output: Raw UART output string
            
        Returns:
            CycleMetrics if parsing successful, None otherwise
        """
        serialize_cycles = None
        deserialize_cycles = None
        library = None
        scenario = None
        
        for line in output.split('\n'):
            line = line.strip()
            if not line:
                continue
            
            # Try to match serialize cycles
            match = self.CYCLE_PATTERNS['serialize'].search(line)
            if match:
                serialize_cycles = int(match.group(1))
                continue
            
            # Try to match deserialize cycles
            match = self.CYCLE_PATTERNS['deserialize'].search(line)
            if match:
                deserialize_cycles = int(match.group(1))
                continue
            
            # Try to match library name
            match = self.CYCLE_PATTERNS['library'].search(line)
            if match:
                library = match.group(1)
                continue
            
            # Try to match scenario name
            match = self.CYCLE_PATTERNS['scenario'].search(line)
            if match:
                scenario = match.group(1)
                continue
        
        # Validate we got the required measurements
        if serialize_cycles is not None and deserialize_cycles is not None:
            return CycleMetrics(
                serialize_cycles=serialize_cycles,
                deserialize_cycles=deserialize_cycles,
                total_cycles=serialize_cycles + deserialize_cycles,
                library=library,
                scenario=scenario
            )
        
        return None
    
    def capture_measurements(self, wait_for_start: bool = True,
                            reset_board: bool = False) -> MeasurementResult:
        """
        Capture cycle measurements from the STM32.
        
        Args:
            wait_for_start: Wait for MEASUREMENT_START marker
            reset_board: Send reset command before capturing
            
        Returns:
            MeasurementResult with captured metrics
        """
        if not self.serial or not self.serial.is_open:
            if not self.connect():
                return MeasurementResult(
                    success=False,
                    error=f"Could not connect to serial port: {self.port}"
                )
        
        output_lines = []
        start_time = time.time()
        found_start = not wait_for_start
        found_end = False
        
        print(f"Capturing from {self.port} (timeout: {self.timeout}s)...")
        
        try:
            while (time.time() - start_time) < self.timeout:
                if self.serial.in_waiting > 0:
                    try:
                        line = self.serial.readline().decode('utf-8', errors='replace').strip()
                        if line:
                            output_lines.append(line)
                            print(f"  > {line}")
                            
                            # Check for start marker
                            if self.CYCLE_PATTERNS['start'].search(line):
                                found_start = True
                            
                            # Check for end marker
                            if self.CYCLE_PATTERNS['end'].search(line):
                                found_end = True
                                break
                    except Exception as e:
                        print(f"  Warning: Error reading line: {e}")
                else:
                    time.sleep(0.01)  # Small delay to prevent busy-waiting
            
            raw_output = '\n'.join(output_lines)
            
            if not output_lines:
                return MeasurementResult(
                    success=False,
                    raw_output=raw_output,
                    error="No data received from STM32. Is the firmware running?"
                )
            
            if wait_for_start and not found_start:
                return MeasurementResult(
                    success=False,
                    raw_output=raw_output,
                    error="MEASUREMENT_START marker not found in output"
                )
            
            # Parse the output
            metrics = self._parse_output(raw_output)
            
            if metrics is None:
                return MeasurementResult(
                    success=False,
                    raw_output=raw_output,
                    error="Could not parse cycle measurements from output"
                )
            
            return MeasurementResult(
                success=True,
                metrics=metrics,
                raw_output=raw_output
            )
            
        except serial.SerialException as e:
            return MeasurementResult(
                success=False,
                raw_output='\n'.join(output_lines),
                error=f"Serial communication error: {e}"
            )
    
    def trigger_measurement(self) -> bool:
        """
        Trigger a new measurement by sending a command to the STM32.
        
        Returns:
            True if command sent successfully
        """
        if not self.serial or not self.serial.is_open:
            return False
        
        try:
            # Send a newline or specific command to trigger measurement
            self.serial.write(b'\n')
            self.serial.flush()
            return True
        except serial.SerialException:
            return False
    
    @staticmethod
    def reset_board() -> bool:
        """
        Reset the STM32 board using OpenOCD.
        
        Returns:
            True if reset successful
        """
        import subprocess
        import shutil
        
        openocd_path = shutil.which("openocd")
        if not openocd_path:
            print("WARNING: OpenOCD not found, cannot reset board")
            return False
        
        try:
            result = subprocess.run(
                [
                    openocd_path,
                    "-f", "interface/stlink.cfg",
                    "-f", "target/stm32f4x.cfg",
                    "-c", "init",
                    "-c", "reset run",
                    "-c", "exit"
                ],
                capture_output=True,
                timeout=10
            )
            return result.returncode == 0
        except subprocess.TimeoutExpired:
            print("WARNING: OpenOCD reset timed out")
            return False
        except Exception as e:
            print(f"WARNING: OpenOCD reset failed: {e}")
            return False


def format_output(result: MeasurementResult, fmt: OutputFormat) -> str:
    """Format the measurement result for output."""
    if not result.success or result.metrics is None:
        if fmt == OutputFormat.JSON:
            import json
            return json.dumps({"success": False, "error": result.error})
        return f"Measurement failed: {result.error}"
    
    metrics = result.metrics
    
    if fmt == OutputFormat.JSON:
        import json
        return json.dumps(metrics.to_dict(), indent=2)
    
    elif fmt == OutputFormat.CSV:
        headers = "serialize_cycles,deserialize_cycles,total_cycles,library,scenario"
        values = f"{metrics.serialize_cycles},{metrics.deserialize_cycles},{metrics.total_cycles},{metrics.library or ''},{metrics.scenario or ''}"
        return f"{headers}\n{values}"
    
    else:  # TEXT
        lines = [
            "CPU Cycle Measurements",
            "=" * 40,
            f"Serialize:    {metrics.serialize_cycles:,} cycles",
            f"Deserialize:  {metrics.deserialize_cycles:,} cycles",
            f"Total:        {metrics.total_cycles:,} cycles",
        ]
        if metrics.library:
            lines.append(f"Library:      {metrics.library}")
        if metrics.scenario:
            lines.append(f"Scenario:     {metrics.scenario}")
        return '\n'.join(lines)


def main():
    """Main entry point for standalone execution."""
    parser = argparse.ArgumentParser(
        description='Capture CPU cycle measurements from STM32 via UART',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python measure_cycles.py                    # Auto-detect port
    python measure_cycles.py --port /dev/ttyACM0
    python measure_cycles.py --list             # List available ports
    python measure_cycles.py --format json      # JSON output
    python measure_cycles.py --timeout 10       # 10 second timeout
    
Expected UART output format from STM32:
    MEASUREMENT_START
    LIBRARY: EmbeddedProto
    SCENARIO: simple_types
    CYCLES_SERIALIZE: 12345
    CYCLES_DESERIALIZE: 67890
    MEASUREMENT_END
        """
    )
    parser.add_argument(
        '--port', '-p',
        help='Serial port (auto-detected if not specified)'
    )
    parser.add_argument(
        '--baudrate', '-b',
        type=int,
        default=115200,
        help='Baud rate (default: 115200)'
    )
    parser.add_argument(
        '--timeout', '-t',
        type=float,
        default=5.0,
        help='Timeout in seconds (default: 5.0)'
    )
    parser.add_argument(
        '--format', '-f',
        choices=['text', 'json', 'csv'],
        default='text',
        help='Output format (default: text)'
    )
    parser.add_argument(
        '--list', '-l',
        action='store_true',
        help='List available serial ports'
    )
    parser.add_argument(
        '--no-wait',
        action='store_true',
        help='Do not wait for MEASUREMENT_START marker'
    )
    parser.add_argument(
        '--reset', '-r',
        action='store_true',
        help='Reset the STM32 board before capturing (requires OpenOCD)'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Show verbose output'
    )
    
    args = parser.parse_args()
    
    # List ports mode
    if args.list:
        print("Available serial ports:")
        ports = STM32CycleMeasurer.list_ports(verbose=args.verbose)
        if not ports:
            print("  No ports found")
        else:
            for device, description, is_stlink in ports:
                marker = " [ST-Link]" if is_stlink else ""
                print(f"  {device}: {description}{marker}")
        
        # Show auto-detected port
        auto_port = STM32CycleMeasurer.auto_detect_port()
        if auto_port:
            print(f"\nAuto-detected ST-Link port: {auto_port}")
        else:
            print("\nNo ST-Link port auto-detected")
        return 0
    
    # Measurement mode
    port = args.port
    if port is None:
        port = STM32CycleMeasurer.auto_detect_port()
        if port is None:
            print("ERROR: No serial port specified and auto-detection failed.")
            print("Use --port to specify the port or --list to see available ports.")
            return 1
        print(f"Auto-detected port: {port}")
    
    measurer = STM32CycleMeasurer(
        port=port,
        baudrate=args.baudrate,
        timeout=args.timeout
    )
    
    try:
        # Connect first so we're ready to capture
        if not measurer.connect():
            print(f"ERROR: Could not connect to {port}")
            return 1
        
        # Reset board if requested
        if args.reset:
            print("Resetting STM32 board...")
            if measurer.reset_board():
                print("Board reset successful")
                time.sleep(0.1)  # Small delay after reset
            else:
                print("WARNING: Board reset may have failed")
        
        result = measurer.capture_measurements(
            wait_for_start=not args.no_wait
        )
        
        # Format and print output
        fmt = OutputFormat(args.format)
        print(format_output(result, fmt))
        
        if args.verbose and result.raw_output:
            print("\n--- Raw Output ---")
            print(result.raw_output)
        
        return 0 if result.success else 1
        
    finally:
        measurer.disconnect()


if __name__ == "__main__":
    sys.exit(main())
