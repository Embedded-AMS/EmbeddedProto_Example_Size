#!/usr/bin/env python3
"""
Generate expected wire format binary for nested_message scenario.

This script uses the Python protobuf library to serialize the test data
and save it as expected_wire.bin for verification.

Usage:
    python generate_wire.py
"""

import sys
import os

# Add the generated Python protobuf files to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../../generated/python'))

try:
    import message_pb2
except ImportError:
    print("ERROR: message_pb2 not found. Generate Python protobuf files first:")
    print("  protoc --python_out=../../generated/python -I. message.proto")
    sys.exit(1)


def main():
    # Create the message with test data
    outer = message_pb2.OuterMessage()
    outer.id = 42
    outer.label = "Test Outer Message"
    
    # Middle message
    outer.middle.x = 1.5
    outer.middle.y = 2.5
    outer.middle.z = 3.5
    outer.middle.inner.value = 100
    outer.middle.inner.name = "Inner1"
    outer.middle.values.extend([10, 20, 30, 40, 50])
    
    # Items (repeated MiddleMessage)
    item1 = outer.items.add()
    item1.x = 4.0
    item1.y = 5.0
    item1.z = 6.0
    item1.inner.value = 200
    item1.inner.name = "Inner2"
    item1.values.extend([1, 2, 3])
    
    item2 = outer.items.add()
    item2.x = 7.0
    item2.y = 8.0
    item2.z = 9.0
    item2.inner.value = 300
    item2.inner.name = "Inner3"
    item2.values.extend([4, 5])
    
    # Serialize to binary
    wire_data = outer.SerializeToString()
    
    # Save to file
    output_file = os.path.join(os.path.dirname(__file__), 'expected_wire.bin')
    with open(output_file, 'wb') as f:
        f.write(wire_data)
    
    print(f"Generated {output_file}")
    print(f"Size: {len(wire_data)} bytes")
    print(f"Hex: {wire_data.hex()}")


if __name__ == '__main__':
    main()
