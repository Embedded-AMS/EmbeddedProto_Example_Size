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
    
    def generate_embeddedproto_code(self) -> bool:
        """Generate EmbeddedProto code from proto files."""
        print("Generating EmbeddedProto code...")
        
        # Ensure output directory exists
        eams_output = self.generated_dir / "EAMS"
        eams_output.mkdir(parents=True, exist_ok=True)
        
        # Get the virtual environment
        env = self._get_venv_env()
        
        # Find proto files
        proto_dir = self.base_dir / "MessageDescriptions"
        proto_files = list(proto_dir.glob("*.proto"))
        
        if not proto_files:
            print("ERROR: No proto files found in MessageDescriptions/")
            return False
        
        for proto_file in proto_files:
            print(f"  Generating from {proto_file.name}...")
            cmd = [
                "embeddedproto",
                f"-I{proto_dir}",
                f"--eams_out={eams_output}",
                str(proto_file)
            ]
            
            result = self._run_command(cmd, cwd=self.base_dir, env=env)
            if result.returncode != 0:
                print(f"ERROR generating EmbeddedProto code: {result.stderr}")
                return False
        
        print("EmbeddedProto code generation successful")
        return True
    
    def generate_nanopb_code(self, scenario: Optional[str] = None) -> bool:
        """Generate NanoPB code from proto files.
        
        Args:
            scenario: Optional scenario name (e.g., 'simple_types'). If provided,
                     generates from scenarios/<scenario>/message.proto with options.
                     If None, generates from MessageDescriptions/*.proto.
        """
        print("Generating NanoPB code...")
        
        # Ensure output directory exists
        nanopb_output = self.generated_dir / "Nanopb"
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
        
        # Determine proto files to generate
        if scenario:
            # Generate from scenario directory
            scenario_dir = self.base_dir / "scenarios" / scenario
            if not scenario_dir.exists():
                print(f"ERROR: Scenario directory not found: {scenario_dir}")
                return False
            
            proto_file = scenario_dir / "message.proto"
            if not proto_file.exists():
                print(f"ERROR: Proto file not found: {proto_file}")
                return False
            
            proto_files = [proto_file]
            proto_dir = scenario_dir
        else:
            # Generate from MessageDescriptions (legacy)
            proto_dir = self.base_dir / "MessageDescriptions"
            proto_files = list(proto_dir.glob("*.proto"))
            
            if not proto_files:
                print("ERROR: No proto files found in MessageDescriptions/")
                return False
        
        # Use venv environment for NanoPB generator (needs protobuf Python library)
        env = self._get_venv_env()
        
        for proto_file in proto_files:
            print(f"  Generating from {proto_file.name}...")
            cmd = [
                str(protoc_path),
                f"--plugin=protoc-gen-nanopb={nanopb_plugin}",
                f"-I{proto_dir}",
                f"--nanopb_out={nanopb_output}",
                str(proto_file)
            ]
            
            result = self._run_command(cmd, cwd=self.base_dir, env=env)
            if result.returncode != 0:
                print(f"ERROR generating NanoPB code: {result.stderr}")
                return False
        
        print("NanoPB code generation successful")
        return True
    
    def clean_build(self, library: str) -> bool:
        """Clean the CMake build directory for a library."""
        build_dir = self.project_dir / "build" / library
        
        if build_dir.exists():
            print(f"Cleaning build directory: {build_dir}")
            shutil.rmtree(build_dir)
        
        return True
    
    def build_stm32_project(self, library: str, build_type: str = "Release", 
                            clean: bool = False) -> BuildResult:
        """Build the STM32 project for the specified library using CMake."""
        print(f"\n{'='*60}")
        print(f"Building STM32 project for {library} ({build_type})")
        print(f"{'='*60}\n")
        
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
        
        # Build directory per library to avoid conflicts
        build_dir = self.project_dir / "build" / library
        
        # Clean if requested
        if clean:
            self.clean_build(library)
        
        # Create build directory
        build_dir.mkdir(parents=True, exist_ok=True)
        
        # Run CMake configure
        print("Running CMake configure...")
        env = self._get_build_env()
        
        cmake_cmd = [
            "cmake",
            f"-DLIBRARY={library}",
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
                        help='Scenario name (e.g., simple_types) for code generation from scenarios/ directory')
    
    args = parser.parse_args()
    
    builder = STM32Builder()
    
    try:
        # Generate code if needed
        if not args.no_generate:
            if args.library == "embeddedproto":
                if not builder.generate_embeddedproto_code():
                    print("Code generation failed")
                    sys.exit(1)
            elif args.library == "nanopb":
                if not builder.generate_nanopb_code(scenario=args.scenario):
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
