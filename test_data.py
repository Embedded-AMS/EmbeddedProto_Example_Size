#
# Copyright (C) 2020-2024 Embedded AMS B.V. - All Rights Reserved
#
# This file is part of Embedded Proto.
#
# Embedded Proto is open source software: you can redistribute it and/or 
# modify it under the terms of the GNU General Public License as published 
# by the Free Software Foundation, version 3 of the license.
#
# Embedded Proto  is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
#
# For commercial and closed source application please visit:
# <https://EmbeddedProto.com/license/>.
#
# Embedded AMS B.V.
# Info:
#   info at EmbeddedProto dot com
#
# Postal address:
#   Atoomweg 2
#   1627 LE, Hoorn
#   the Netherlands
#

from sys import path
path.append('./generated/Python/')

import simple_types_pb2 as st


def print_msg(message):
    str = ""
    msg_str = message.SerializeToString()
    print(len(msg_str))
    print(msg_str)
    for x in msg_str:
      str += "0x{:02x}, ".format(x)

    print(str)
    print()


def test_simple_types():
    # A test function used to generate encoded data to test the implementation of the wireformatter
    # and header template.

    msg = st.Test_Simple_Types()
    msg.a_int32 = -2147483648
    msg.a_int64 = -9223372036854775808
    msg.a_uint32 = 0
    msg.a_uint64 = 0
    msg.a_sint32 = -2147483648
    msg.a_sint64 = -9223372036854775808
    msg.a_bool = 0
    msg.a_enum = 0
    msg.a_fixed64 = 0
    msg.a_sfixed64 = -9223372036854775808
    msg.a_double = 0
    msg.a_fixed32 = 0
    msg.a_sfixed32 = -2147483648
    msg.a_float = 0
    msg.a_nested_enum = st.Test_Simple_Types.NE_A

    print_msg(msg)


test_simple_types()
