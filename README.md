
# Purpose
To compare Embedded Proto to alternatives such as NanoPB and ArduinoJson in terms of performance.
Performance in this case is specified as flash, RAM, CPU, and buffer usage.
ArduinoJson is not a protobuf library but is considered a likely alternative to protobuf, so it is used as a comparison.

# Compiler flags
All C++ is compiled with `-Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
-fno-threadsafe-statics -fno-use-cxa-atexit` and linked with `--gc-sections`, the flags an
embedded IDE sets for a C++ project. Without `-fno-exceptions -fno-rtti` GCC links the exception
unwinder and RTTI into every C++ build, about 4.5 kB that nanopb (plain C) never pays. See
`CMakeLists.txt`, the flags are stored with every result in the database.

# What is measured
- `text`, `data` and `bss` from `arm-none-eabi-size`, compare against the `Baseline/blinky` scenario
  to get the cost of a library.
- `cpu_cycles` from the DWT cycle counter around `run_scenario()`, read over SWD.
- `n_bytes_in_buffer`, the number of bytes serialized, which every library must agree on.
- `message_size_bytes`, the RAM the scenario's message object takes. A scenario reports each
  message it creates with `report_message_size()` and the largest is kept. For Embedded Proto and
  nanopb that is `sizeof()` of the message. ArduinoJson keeps its data on the heap, so its
  documents use a counting allocator and report the peak heap use plus `sizeof()`.

# Per library options
The `.proto` files are shared by all libraries. Options which are specific to one library live
next to them in `Protofiles/`:
- `<name>.options` for nanopb (`max_count`, `max_size`, `FT_CALLBACK`)
- `<name>.options.json` for Embedded Proto (`maxLength`, `callbackStorage`), passed to the
  generator as `--eams_opt=options_file=...`

# Building the code
1. Clean previous build results: `rm -rf build/`
2. Create build scripts:
   `cmake -B build -DLIBRARY=TARGET_LIBRARY -DSCENARIO=TARGET_SCENARIO`
3. Build the code: `cmake --build build`

# Running benchmarks
Use `benchmark.py` for build + measurement + database storage.

Example (single scenario):
```bash
python3 benchmark.py --library EmbeddedProto --scenario simple_types_serialize_max
```

Benchmark mode selection:
```bash
python3 benchmark.py --library EmbeddedProto --scenario simple_types_serialize_max --mode normal
python3 benchmark.py --library EmbeddedProto --scenario simple_types_serialize_max --mode partial
```

- `--mode normal` uses regular serialization.
- `--mode partial` enables partial serialization for EmbeddedProto scenarios.
- The selected mode is stored in `benchmark_results.db` in column `benchmark_mode`.

# Querying mode-specific results
Show latest EmbeddedProto results with mode:
```bash
sqlite3 benchmark_results.db "SELECT timestamp, library, scenario, benchmark_mode, text_bytes, data_bytes, bss_bytes, cpu_cycles FROM benchmark_results WHERE library='EmbeddedProto' ORDER BY timestamp DESC LIMIT 10;"
```

Filter a specific scenario by mode:
```bash
sqlite3 benchmark_results.db "SELECT timestamp, scenario, benchmark_mode, text_bytes, data_bytes, bss_bytes FROM benchmark_results WHERE library='EmbeddedProto' AND scenario='simple_types_serialize_max' AND benchmark_mode='partial' ORDER BY timestamp DESC LIMIT 5;"
```

# Flashing the code on a Nucleo-F446RE
Run `./flash.sh`

# Internal workings
- `LIBRARY` selects which test set is built (see `./External` and `./Scenarios`).
- `SCENARIO` selects which file is built from `./Scenarios/<LIBRARY>/<SCENARIO>.cpp`.
- Arm GNU EABI tools are used to extract flash and static RAM usage.
- OpenOCD is used to flash a Nucleo-F446RE and read dynamic metrics (CPU cycles and buffer usage).
- All results are stored in a SQLite database.