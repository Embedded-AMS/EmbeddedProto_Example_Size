#!/usr/bin/env python3
"""
STM32 Build Automation Script

This script automates building the EmbeddedProto test project for STM32 Nucleo.
It handles code generation, compilation using CMake, and produces an ELF file.

Usage:
    python build_stm32.py --library embeddedproto
    python build_stm32.py --library nanopb
    python build_stm32.py --library arduinojson
    python build_stm32.py --library embeddedproto --clean
"""

import os
import subprocess
import argparse
import sys
import shutil
import glob
from pathlib import Path
from dataclasses import dataclass
from typing import Optional, Dict, List


@dataclass
class BuildResult:
    """Result of a build operation."""
    success: bool
    output: str
    elf_path: Optional[str] = None
    error: Optional[str] = None


class STM32Builder:
    """Class to handle STM32 build automation using CMake."""
    
    # Known locations for ARM toolchain
    ARM_TOOLCHAIN_PATHS = [
        # STM32CubeIDE installations
        "/home/bart/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.linux64_1.0.0.202410170706/tools/bin",
        "/home/bart/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.linux64_1.0.100.202403111256/tools/bin",
        # Standard locations
        "/usr/bin",
        "/usr/local/bin",
    ]
    
    def __init__(self, base_dir: Optional[Path] = None):
        self.base_dir = Path(base_dir) if base_dir else Path(__file__).parent.absolute()
        self.project_dir = self.base_dir / "F446RE_EmbeddedProto"
        self.generated_dir = self.base_dir / "generated"
        self.results_dir = self.base_dir / "results"
        self.embeddedproto_dir = self.base_dir / "EmbeddedProto"
        
        # Ensure directories exist
        self.generated_dir.mkdir(exist_ok=True)
        self.results_dir.mkdir(exist_ok=True)
        
        # Find ARM toolchain
        self.arm_toolchain_path = self._find_arm_toolchain()
        
    def _find_arm_toolchain(self) -> Optional[str]:
        """Find the ARM toolchain in known locations."""
        for path in self.ARM_TOOLCHAIN_PATHS:
            gcc_path = Path(path) / "arm-none-eabi-gcc"
            if gcc_path.exists():
                print(f"Found ARM toolchain at: {path}")
                return path
        
        # Try to find it using glob patterns
        patterns = [
            "/home/*/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*/tools/bin",
            "/opt/st/stm32cubeide*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32*/tools/bin",
        ]
        
        for pattern in patterns:
            matches = glob.glob(pattern)
            for match in sorted(matches, reverse=True):  # Prefer newer versions
                gcc_path = Path(match) / "arm-none-eabi-gcc"
                if gcc_path.exists():
                    print(f"Found ARM toolchain at: {match}")
                    return match
        
        return None
    
    def _get_build_env(self) -> Dict[str, str]:
        """Get environment with ARM toolchain in PATH."""
        env = os.environ.copy()
        if self.arm_toolchain_path:
            env['PATH'] = f"{self.arm_toolchain_path}:{env.get('PATH', '')}"
        return env
    
    def _get_venv_env(self) -> Dict[str, str]:
        """Get environment with EmbeddedProto venv activated."""
        env = self._get_build_env()
        venv_path = self.embeddedproto_dir / "venv"
        if venv_path.exists():
            env['VIRTUAL_ENV'] = str(venv_path)
            env['PATH'] = f"{venv_path / 'bin'}:{env['PATH']}"
        return env
    
    def _run_command(self, cmd: List[str], cwd: Optional[Path] = None, 
                     env: Optional[Dict] = None, capture_output: bool = True) -> subprocess.CompletedProcess:
        """Run a command and return the result."""
        if env is None:
            env = self._get_build_env()
        
        try:
            result = subprocess.run(
                cmd, 
                cwd=cwd, 
                capture_output=capture_output, 
                text=True,
                check=False,
                env=env
            )
            return result
        except Exception as e:
            raise RuntimeError(f"Failed to run command: {' '.join(cmd)}") from e
    
    def check_toolchain(self) -> bool:
        """Check if ARM toolchain is available."""
        if not self.arm_toolchain_path:
            print("ERROR: ARM toolchain not found!")
            print("Please install STM32CubeIDE or arm-none-eabi-gcc toolchain.")
            return False
        
        # Verify gcc works
        result = self._run_command(["arm-none-eabi-gcc", "--version"])
        if result.returncode != 0:
            print("ERROR: arm-none-eabi-gcc not working properly")
            return False
        
        # Extract version info
        version_line = result.stdout.split('\n')[0] if result.stdout else "unknown"
        print(f"ARM GCC version: {version_line}")
        return True
    
    def generate_embeddedproto_code(self, scenario: str) -> bool:
        """Generate EmbeddedProto code for a specific scenario.
        
        Args:
            scenario: Scenario name (e.g., 'simple_types', 'nested_message')
        
        Returns:
            True if code generation successful
        """
        print(f"Generating EmbeddedProto code for scenario: {scenario}")
        
        # Validate scenario
        if not scenario:
            print("ERROR: Scenario must be specified")
            return False
        
        # Get scenario directory - try both old and new locations for backward compatibility
        scenario_dir = self.base_dir / "F446RE_EmbeddedProto" / "Core" / "scenarios" / scenario
        if not scenario_dir.exists():
            # Fallback to old location for backward compatibility
            scenario_dir = self.base_dir / "scenarios" / scenario
            if not scenario_dir.exists():
                print(f"ERROR: Scenario directory not found: {scenario_dir}")
                return False
        
        # Find proto file
        # Try both old and new naming conventions for backward compatibility
        proto_file = scenario_dir / f"{scenario}.proto"
        if not proto_file.exists():
            # Fallback to old naming convention
            proto_file = scenario_dir / "message.proto"
            if not proto_file.exists():
                print(f"ERROR: Proto file not found: {scenario_dir}/{scenario}.proto or {scenario_dir}/message.proto")
                return False
        
        # Create scenario-specific output directory
        scenario_output = self.generated_dir / "EAMS" / scenario
        scenario_output.mkdir(parents=True, exist_ok=True)
        
        # Generate code
        print(f"  Generating from {proto_file.name}...")
        cmd = [
            "embeddedproto",
            f"-I{scenario_dir}",
            f"--eams_out={scenario_output}",
            str(proto_file)
        ]
        
        env = self._get_venv_env()
        result = self._run_command(cmd, cwd=self.base_dir, env=env)
        if result.returncode != 0:
            print(f"ERROR generating EmbeddedProto code: {result.stderr}")
            return False
        
        print(f"EmbeddedProto code generation successful for {scenario}")
        return True
    
    def generate_nanopb_code(self, scenario: str) -> bool:
        """Generate NanoPB code for a specific scenario.
        
        Args:
            scenario: Scenario name (e.g., 'simple_types', 'nested_message')
        
        Returns:
            True if code generation successful
        """
        print(f"Generating NanoPB code for scenario: {scenario}")
        
        # Validate scenario
        if not scenario:
            print("ERROR: Scenario must be specified")
            return False
        
        # Get scenario directory
        scenario_dir = self.base_dir / "scenarios" / scenario
        if not scenario_dir.exists():
            print(f"ERROR: Scenario directory not found: {scenario_dir}")
            return False
        
        # Find proto file
        # Try both old and new naming conventions for backward compatibility
        proto_file = scenario_dir / f"{scenario}.proto"
        if not proto_file.exists():
            # Fallback to old naming convention
            proto_file = scenario_dir / "message.proto"
            if not proto_file.exists():
                print(f"ERROR: Proto file not found: {scenario_dir}/{scenario}.proto or {scenario_dir}/message.proto")
                return False
        
        # Create scenario-specific output directory
        nanopb_output = self.generated_dir / "Nanopb" / scenario
        nanopb_output.mkdir(parents=True, exist_ok=True)
        
        # Find protoc
        protoc_path = self.base_dir / "GoolgeProtobuf" / "bazel-bin" / "protoc"
        if not protoc_path.exists():
            print(f"ERROR: protoc not found at {protoc_path}")
            return False
        
        # Find nanopb plugin
        nanopb_plugin = self.base_dir / "Nanopb" / "generator" / "protoc-gen-nanopb"
        if not nanopb_plugin.exists():
            print(f"ERROR: nanopb plugin not found at {nanopb_plugin}")
            return False
        
        # Check for options file
        options_file = proto_file.with_suffix('.options')
        
        # Use venv environment for NanoPB generator (needs protobuf Python library)
        env = self._get_venv_env()
        
        print(f"  Generating from {proto_file.name}...")
        cmd = [
            str(protoc_path),
            f"--plugin=protoc-gen-nanopb={nanopb_plugin}",
            f"-I{scenario_dir}",
        ]
        
        # Add options file if it exists
        if options_file.exists():
            cmd.append(f"--nanopb_opt=-f{options_file}")
        
        cmd.extend([
            f"--nanopb_out={nanopb_output}",
            str(proto_file)
        ])
        
        result = self._run_command(cmd, cwd=self.base_dir, env=env)
        if result.returncode != 0:
            print(f"ERROR generating NanoPB code: {result.stderr}")
            return False
        
        print(f"NanoPB code generation successful for {scenario}")
        return True
    
    def clean_build(self, library: str) -> bool:
        """Clean the CMake build directory for a library."""
        build_dir = self.project_dir / "build" / library
        
        if build_dir.exists():
            print(f"Cleaning build directory: {build_dir}")
            shutil.rmtree(build_dir)
        
        return True
    
    def build_stm32_project(self, library: str, scenario: str, 
                            build_type: str = "Release", 
                            clean: bool = False) -> BuildResult:
        """Build the STM32 project for the specified library and scenario using CMake."""
        print(f"\n{'='*60}")
        print(f"Building STM32 project for {library} + {scenario} ({build_type})")
        print(f"{'='*60}\n")
        
        # Validate scenario
        if not scenario:
            return BuildResult(
                success=False,
                output="",
                error="Scenario must be specified"
            )
        
        # Check toolchain
        if not self.check_toolchain():
            return BuildResult(
                success=False,
                output="",
                error="ARM toolchain not found"
            )
        
        # Check CMakeLists.txt exists
        cmake_file = self.project_dir / "CMakeLists.txt"
        if not cmake_file.exists():
            return BuildResult(
                success=False,
                output="",
                error=f"CMakeLists.txt not found at {cmake_file}"
            )
        
        # Build directory per library and scenario to avoid conflicts
        build_dir = self.project_dir / "build" / library / scenario
        
        # Clean if requested
        if clean:
            if build_dir.exists():
                print(f"Cleaning build directory: {build_dir}")
                shutil.rmtree(build_dir)
        
        # Create build directory
        build_dir.mkdir(parents=True, exist_ok=True)
        
        # Run CMake configure
        print("Running CMake configure...")
        env = self._get_build_env()
        
        cmake_cmd = [
            "cmake",
            f"-DLIBRARY={library}",
            f"-DSCENARIO={scenario}",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            str(self.project_dir)
        ]
        
        result = self._run_command(cmake_cmd, cwd=build_dir, env=env)
        
        if result.returncode != 0:
            output = (result.stdout or "") + "\n" + (result.stderr or "")
            print(f"CMake configure failed:\n{output}")
            return BuildResult(
                success=False,
                output=output,
                error="CMake configure failed"
            )
        
        # Run make
        print("Running make...")
        make_cmd = ["make", "-j4"]
        
        result = self._run_command(make_cmd, cwd=build_dir, env=env)
        
        # Combine stdout and stderr for output
        output = (result.stdout or "") + "\n" + (result.stderr or "")
        
        print(f"Make return code: {result.returncode}")
        
        if result.returncode != 0:
            print(f"Build failed with output:\n{output}")
            return BuildResult(
                success=False,
                output=output,
                error=f"Build failed with return code {result.returncode}"
            )
        
        # Find the ELF file
        elf_path = build_dir / "EmbeddedProto_Example_Size.elf"
        
        print(f"Looking for ELF file at: {elf_path}")
        
        if not elf_path.exists():
            # Check what files were created
            print("ELF file not found. Checking what files exist in build directory:")
            for f in build_dir.iterdir():
                if f.is_file():
                    print(f"  {f.name}")
            
            return BuildResult(
                success=False,
                output=output,
                error=f"ELF file not found at {elf_path}"
            )
        
        print(f"\nBuild successful!")
        print(f"ELF file: {elf_path}")
        
        return BuildResult(
            success=True,
            output=output,
            elf_path=str(elf_path)
        )
    
    def get_size_info(self, elf_path: str) -> Optional[Dict[str, int]]:
        """Get size information from ELF file using arm-none-eabi-size."""
        print(f"\nExtracting size info from {elf_path}...")
        
        result = self._run_command(["arm-none-eabi-size", elf_path])
        
        if result.returncode != 0:
            print(f"ERROR: Failed to get size info: {result.stderr}")
            return None
        
        # Parse the output (Berkeley format)
        # Example:
        #    text    data     bss     dec     hex filename
        #   23456    1234    5678   30368    76a0 file.elf
        lines = result.stdout.strip().split('\n')
        if len(lines) < 2:
            print("ERROR: Unexpected size output format")
            return None
        
        parts = lines[1].split()
        if len(parts) < 4:
            print("ERROR: Unexpected size output format")
            return None
        
        size_info = {
            'text': int(parts[0]),
            'data': int(parts[1]),
            'bss': int(parts[2]),
            'total': int(parts[3]),
            'total_flash': int(parts[0]) + int(parts[1]),  # text + data
            'total_ram': int(parts[1]) + int(parts[2])     # data + bss
        }
        
        print(f"  Text (Flash):  {size_info['text']:,} bytes")
        print(f"  Data:          {size_info['data']:,} bytes")
        print(f"  BSS:           {size_info['bss']:,} bytes")
        print(f"  Total Flash:   {size_info['total_flash']:,} bytes")
        print(f"  Total RAM:     {size_info['total_ram']:,} bytes")
        
        return size_info


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description='STM32 Build Automation for EmbeddedProto Size Comparison',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python build_stm32.py --library embeddedproto
    python build_stm32.py --library nanopb --clean
    python build_stm32.py --library nanopb --scenario simple_types
    python build_stm32.py --library arduinojson --build-type Debug
        """
    )
    parser.add_argument('--library', 
                        choices=['embeddedproto', 'nanopb', 'arduinojson'], 
                        required=True, 
                        help='Library to build')
    parser.add_argument('--build-type',
                        choices=['Release', 'Debug'],
                        default='Release',
                        help='Build type (default: Release)')
    parser.add_argument('--clean', 
                        action='store_true', 
                        help='Clean before building')
    parser.add_argument('--generate-only', 
                        action='store_true', 
                        help='Only generate code, do not build')
    parser.add_argument('--no-generate',
                        action='store_true',
                        help='Skip code generation')
    parser.add_argument('--scenario',
                        required=True,
                        help='Scenario name (e.g., simple_types, nested_message)')
    
    args = parser.parse_args()
    
    builder = STM32Builder()
    
    try:
        # Generate code if needed
        if not args.no_generate:
            if args.library == "embeddedproto":
                if not builder.generate_embeddedproto_code(args.scenario):
                    print("Code generation failed")
                    sys.exit(1)
            elif args.library == "nanopb":
                if not builder.generate_nanopb_code(args.scenario):
                    print("Code generation failed")
                    sys.exit(1)
            elif args.library == "arduinojson":
                # ArduinoJson doesn't need code generation
                print("ArduinoJson: No code generation needed")
        
        if args.generate_only:
            print("Code generation completed successfully")
            sys.exit(0)
        
        # Build the project
        build_result = builder.build_stm32_project(
            args.library,
            scenario=args.scenario,
            build_type=args.build_type,
            clean=args.clean
        )
        
        if not build_result.success:
            print(f"\nBuild failed: {build_result.error}")
            if build_result.output:
                print(f"\nBuild output:\n{build_result.output}")
            sys.exit(1)
        
        # Extract size metrics
        if build_result.elf_path:
            size_info = builder.get_size_info(build_result.elf_path)
            if size_info:
                print(f"\n{'='*60}")
                print("Build completed successfully!")
                print(f"{'='*60}")
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
