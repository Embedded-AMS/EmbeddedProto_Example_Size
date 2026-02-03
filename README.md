
# Purpose
To compare Embedded Proto to alterantives such as NanoPB and ArduinoJson in terms of performance.
Performance in this case is specified as Flash, RAM, CPU and buffer usage.
ArduinoJson is not a protobuf library but is is ocnsidered an likely alternative to protobuf. It is thus used as a comparison.

# Building the code
1. Clean previous build results: `rm -rf build/`
2. Create the build scripts: `cmake -B build -DLIBRARY=TARGET_LIBIBRARY -DSCENARIO=TARGET_SCENARIO`, where you specify TARGET_LIBIBRARY and TARGET_SCENARIO.
3. Build the code: `cmake --build build`

# Flashing the code on a Nucleo-F446RE
Run the script ./flash.sh

# Internal workings
* The TARGET_LIBIRARY specifies for which test results are generated. This is done by using it to generate proto files and scenarios. You will see libraries have dedicated sub folders in the folders ./External and ./Scenarios. 
* The TARGET_SCENARIO specifis which cpp file to build from ./Scenarios/TARTEG_LIBRARY/TARTEG_SCENARIO.cpp. 
* Arm GNU Non EABI build tools are used to obtain the amount of flash and statically allocated RAM used by a library scenario combination. 
* OpenOCD is used to flash the code onto a Nucleo-F466RE dev board. Using SWD features the dynamically used RAM, CPU cycles and buffer usage required to execute the comination are obtaind to the host pc.
* All results are stored in a sqlite database.