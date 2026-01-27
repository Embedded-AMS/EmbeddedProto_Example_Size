/**
 * @file nanopb.h
 * @brief NanoPB test functions for size comparison benchmarking
 * 
 * This file provides test functions for NanoPB serialization/deserialization
 * with CPU cycle measurement support.
 */

#ifndef INC_NANOPB_H_
#define INC_NANOPB_H_

#include "simple_types.pb.h"
#include "nested_message.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "cycle_counter.h"
#include <string.h>

/* Buffer size for serialization */
constexpr uint32_t NANOPB_BUFFER_SIZE = 128;

/* Test data values (must match scenarios/simple_types/test_data.json) */
constexpr int32_t  TEST_INT32  = -12345;
constexpr int64_t  TEST_INT64  = -9876543210LL;
constexpr uint32_t TEST_UINT32 = 12345;
constexpr uint64_t TEST_UINT64 = 9876543210ULL;
constexpr bool     TEST_BOOL   = true;
constexpr float    TEST_FLOAT  = 3.14159f;
constexpr double   TEST_DOUBLE = 2.718281828459045;
const char*        TEST_STRING = "Hello, Proto!";
constexpr uint8_t  TEST_BYTES_DATA[] = {0xDE, 0xAD, 0xBE, 0xEF};
constexpr size_t   TEST_BYTES_LEN = sizeof(TEST_BYTES_DATA);

/* Message instance */
Test_Simple_Types msg;

/**
 * @brief Populate message with test data
 */
static inline void populate_nanopb_message(Test_Simple_Types& m)
{
    m.a_int32 = TEST_INT32;
    m.a_int64 = TEST_INT64;
    m.a_uint32 = TEST_UINT32;
    m.a_uint64 = TEST_UINT64;
    m.a_bool = TEST_BOOL;
    m.a_float = TEST_FLOAT;
    m.a_double = TEST_DOUBLE;
    /* Note: string and bytes fields depend on the generated code structure */
}

/**
 * @brief Test NanoPB serialization/deserialization with cycle measurement
 * 
 * This function measures CPU cycles for serialize and deserialize operations
 * and outputs the results via UART.
 */
void test_nanopb()
{
    /* Set up test data */
    populate_nanopb_message(msg);

    /* Prepare buffers */
    uint8_t obuffer[NANOPB_BUFFER_SIZE];
    uint8_t ibuffer[NANOPB_BUFFER_SIZE];
    
    /* Measure serialize cycles */
    uint32_t start_serialize = CycleCounter_Start();
    pb_ostream_t ostream = pb_ostream_from_buffer(obuffer, sizeof(obuffer));
    bool encode_status = pb_encode(&ostream, Test_Simple_Types_fields, &msg);
    uint32_t serialize_cycles = CycleCounter_Elapsed(start_serialize);
    
    /* Copy serialized data to input buffer for deserialization */
    size_t serialized_size = ostream.bytes_written;
    memcpy(ibuffer, obuffer, serialized_size);
    
    /* Clear message before deserialize to ensure we're actually reading */
    memset(&msg, 0, sizeof(msg));
    
    /* Measure deserialize cycles */
    uint32_t start_deserialize = CycleCounter_Start();
    pb_istream_t istream = pb_istream_from_buffer(ibuffer, serialized_size);
    bool decode_status = pb_decode(&istream, Test_Simple_Types_fields, &msg);
    uint32_t deserialize_cycles = CycleCounter_Elapsed(start_deserialize);
    
    /* Prevent unused variable warnings */
    (void)encode_status;
    (void)decode_status;
    
    /* Output results via UART */
    CycleMeasurement measurement = {
        .serialize_cycles = serialize_cycles,
        .deserialize_cycles = deserialize_cycles,
        .library = "NanoPB",
        .scenario = "simple_types"
    };
    CycleCounter_OutputResults(&measurement);
}

/* Array of messages for array test */
Test_Simple_Types msgArray[100];

/**
 * @brief Test NanoPB array serialization/deserialization with cycle measurement
 * 
 * This function measures CPU cycles for serialize and deserialize operations
 * on an array of messages and outputs the average results via UART.
 */
void test_nanopb_array()
{
    uint32_t total_serialize_cycles = 0;
    uint32_t total_deserialize_cycles = 0;
    const uint32_t num_iterations = 100;
    
    for(auto& m : msgArray) {
        /* Set up test data */
        populate_nanopb_message(m);
        
        /* Prepare buffers */
        uint8_t obuffer[NANOPB_BUFFER_SIZE];
        uint8_t ibuffer[NANOPB_BUFFER_SIZE];
        
        /* Measure serialize cycles */
        uint32_t start_serialize = CycleCounter_Start();
        pb_ostream_t ostream = pb_ostream_from_buffer(obuffer, sizeof(obuffer));
        pb_encode(&ostream, Test_Simple_Types_fields, &m);
        total_serialize_cycles += CycleCounter_Elapsed(start_serialize);
        
        /* Copy serialized data to input buffer */
        size_t serialized_size = ostream.bytes_written;
        memcpy(ibuffer, obuffer, serialized_size);
        
        /* Clear message before deserialize */
        memset(&m, 0, sizeof(m));
        
        /* Measure deserialize cycles */
        uint32_t start_deserialize = CycleCounter_Start();
        pb_istream_t istream = pb_istream_from_buffer(ibuffer, serialized_size);
        pb_decode(&istream, Test_Simple_Types_fields, &m);
        total_deserialize_cycles += CycleCounter_Elapsed(start_deserialize);
    }
    
    /* Output average results via UART */
    CycleMeasurement measurement = {
        .serialize_cycles = total_serialize_cycles / num_iterations,
        .deserialize_cycles = total_deserialize_cycles / num_iterations,
        .library = "NanoPB",
        .scenario = "simple_types_array_avg"
    };
    CycleCounter_OutputResults(&measurement);
}

/* Nested message instance for future use */
demo_space_message_e msg_c;
void test_message_c()
{
    /* Placeholder for nested message tests */
}

#endif /* INC_NANOPB_H_ */
