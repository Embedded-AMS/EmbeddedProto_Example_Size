# Nested Message Test Scenario

This scenario tests serialization/deserialization of nested messages with 2-3 levels of nesting and repeated fields.

## Purpose

Measure the overhead of message nesting in different serialization libraries:
- EmbeddedProto
- NanoPB
- ArduinoJson

## Message Structure

```
OuterMessage (Level 1)
├── id: int32
├── label: string (max 32 chars)
├── middle: MiddleMessage (Level 2)
│   ├── x, y, z: float
│   ├── inner: InnerMessage (Level 3)
│   │   ├── value: int32
│   │   └── name: string (max 16 chars)
│   └── values: repeated int32 (max 5 elements)
└── items: repeated MiddleMessage (max 3 elements)
```

## Files

| File | Description |
|------|-------------|
| `message.proto` | Protobuf message definition |
| `message.options` | NanoPB options for fixed-size fields |
| `test_data.json` | Test data values |
| `expected_wire.bin` | Expected protobuf wire format (127 bytes) |
| `test_embeddedproto.cpp` | EmbeddedProto test code |
| `test_nanopb.cpp` | NanoPB test code |
| `test_arduinojson.cpp` | ArduinoJson test code |
| `generate_wire.py` | Script to regenerate expected_wire.bin |

## Test Data

The test data includes:
- Outer message with id=42 and label="Test Outer Message"
- One middle message with coordinates (1.5, 2.5, 3.5) and 5 values
- Two items in the repeated field with different coordinates and values

## Running the Comparison

```bash
python run_comparison.py --scenario nested_message
```

## Expected Results

The nested message scenario typically shows:
- Higher flash usage due to nested serialize/deserialize code
- Higher RAM usage due to nested message structures
- More CPU cycles for serialization/deserialization

## Regenerating expected_wire.bin

If you modify the proto file or test data:

```bash
# Generate Python protobuf files first
protoc --python_out=../../generated/python -I. message.proto

# Then regenerate the wire format
python generate_wire.py
```
