
# Purpose
To compare Embedded Proto to alternatives such as NanoPB and ArduinoJson in terms of performance.
Performance in this case is specified as flash, RAM, CPU, and buffer usage.
ArduinoJson is not a protobuf library but is considered a likely alternative to protobuf, so it is used as a comparison.

# Scenarios
Two sets live in `Scenarios/<Library>/`. The published set is what the website shows, six
messages shaped like real device traffic with the same values in every library
(`Scenarios/scenario_values.h`):

| Scenario | Message | Shows |
|---|---|---|
| `telemetry_serialize`, `telemetry_serialize_deserialize` | `Telemetry`, 7 mixed fields | the small message every device sends |
| `telemetry_batch_serialize_deserialize` | `TelemetryBatch`, 20 nested samples | the buffered upload |
| `configuration_serialize_deserialize` | `Configuration`, oneofs, string, optional | the complex message |
| `command_ack_serialize_deserialize` | `DeviceCommand` and `DeviceAck` | fixed cost per call |
| `firmware_chunk_serialize_deserialize` | `FirmwareChunk`, 256 resident bytes | the payload |
| `firmware_stream_serialize_deserialize` | `FirmwareStream`, streamed in 32 byte windows | the RAM story, protobuf libraries only |
| `schema_growth_01`, `_05`, `_20` | 1, 5 or 20 `Growth_NN` types round tripped | flash per message type in the schema |

The other scenarios (`simple_types_*`, `string_*`, `oneof_*`, `optional_*`, `repeated_*`,
`complete_demo_*`, `*_callback_*`) are the diagnostic set for the Embedded Proto developers.
They probe single features and encoder edge cases and are not meant for the website.

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

# Libraries
- Embedded Proto (`External/EmbeddedProto`), header only C++.
- nanopb (`External/Nanopb`), C, built with `PB_NO_ERRMSG`.
- FlatBuffers through flatcc (`External/flatcc`), the C implementation. Its compiler is built
  for the host once into `build/flatcc-host`, its runtime (builder, emitter, verifier, no JSON)
  is compiled for the target. The builder allocates its stacks and pages from the heap, which
  goes through a counting allocator so the RAM of a build is reported as the message size.
- zcbor (`External/zcbor`), Nordic's CBOR library, C code generated from CDDL schemas. The
  generator is installed from the submodule into `External/zcbor/venv` on first use. The
  schemas keep protobuf's numbered, optional fields as map entries keyed by field number.
- ArduinoJson (`External/ArduinoJson`), JSON, not protobuf, the alternative people consider.

# Per library options
The `.proto` files are shared by all libraries. Options which are specific to one library live
next to them in `Protofiles/`:
- `<name>.options` for nanopb (`max_count`, `max_size`, `FT_CALLBACK`); nanopb is built with
  `PB_NO_ERRMSG`, the one size option its documentation recommends for a small target
- `<name>.options.json` for Embedded Proto (`maxLength`, `callbackStorage`), passed to the
  generator as `--eams_opt=options_file=...`
- `<name>.fbs` for FlatBuffers, a hand written schema equivalent to the `.proto`
- `<name>.cddl` for zcbor, the same, all generated together into `Generated/cbor_messages_*`

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