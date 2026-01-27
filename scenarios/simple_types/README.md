# Simple Types Test Scenario

This scenario tests serialization and deserialization of all basic protobuf types.

## Message Definition

The `message.proto` file defines a `SimpleTypes` message with the following fields:

| Field | Type | Description |
|-------|------|-------------|
| a_int32 | int32 | Signed 32-bit integer |
| a_int64 | int64 | Signed 64-bit integer |
| a_uint32 | uint32 | Unsigned 32-bit integer |
| a_uint64 | uint64 | Unsigned 64-bit integer |
| a_bool | bool | Boolean value |
| a_float | float | 32-bit floating point |
| a_double | double | 64-bit floating point |
| a_string | string | UTF-8 string (max 32 chars) |
| a_bytes | bytes | Binary data (max 32 bytes) |
| a_enum | TestEnum | Enumeration value |

## Test Data

The `test_data.json` file contains known test values for each field.
The `expected_wire.bin` file contains the expected protobuf wire format output.

## Test Code

- `test_embeddedproto.cpp` - EmbeddedProto test implementation
- `test_nanopb.cpp` - NanoPB test implementation
- `test_arduinojson.cpp` - ArduinoJson test implementation (JSON format)

## NanoPB Options

The `message.options` file configures NanoPB to use fixed-size arrays for
string and bytes fields (32 bytes each) instead of callbacks.
