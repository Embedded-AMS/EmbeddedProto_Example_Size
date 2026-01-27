#!/usr/bin/env python3
"""Generate expected wire format binary for simple_types scenario."""

import sys
sys.path.insert(0, '.')
from message_pb2 import SimpleTypes, TestEnum

def main():
    msg = SimpleTypes()
    msg.a_int32 = -12345
    msg.a_int64 = -9876543210
    msg.a_uint32 = 12345
    msg.a_uint64 = 9876543210
    msg.a_bool = True
    msg.a_float = 3.14159
    msg.a_double = 2.718281828459045
    msg.a_string = "Hello, Proto!"
    msg.a_bytes = bytes([0xDE, 0xAD, 0xBE, 0xEF])
    msg.a_enum = TestEnum.ENUM_TWO

    # Serialize to binary
    wire_data = msg.SerializeToString()

    # Write to file
    with open('expected_wire.bin', 'wb') as f:
        f.write(wire_data)

    # Print hex for verification
    print("Wire format ({} bytes):".format(len(wire_data)))
    print(' '.join('0x{:02X}'.format(b) for b in wire_data))

if __name__ == '__main__':
    main()
