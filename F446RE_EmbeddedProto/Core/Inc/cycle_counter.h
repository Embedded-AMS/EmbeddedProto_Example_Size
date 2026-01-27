/**
 * @file cycle_counter.h
 * @brief DWT Cycle Counter for CPU cycle measurement on Cortex-M4
 * 
 * This module provides functions to measure CPU cycles using the DWT
 * (Data Watchpoint and Trace) cycle counter available on ARM Cortex-M3/M4.
 * 
 * Usage:
 *   CycleCounter_Init();
 *   CycleCounter_Reset();
 *   // ... code to measure ...
 *   uint32_t cycles = CycleCounter_Get();
 */

#ifndef INC_CYCLE_COUNTER_H_
#define INC_CYCLE_COUNTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* External UART handle (defined in main.cpp) */
extern UART_HandleTypeDef huart2;

/**
 * @brief Initialize the DWT cycle counter
 * 
 * This must be called once before using the cycle counter.
 * Enables the trace unit and cycle counter.
 */
static inline void CycleCounter_Init(void)
{
    /* Enable TRC (Trace) */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    /* Reset the cycle counter */
    DWT->CYCCNT = 0;
    
    /* Enable the cycle counter */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
 * @brief Reset the cycle counter to zero
 */
static inline void CycleCounter_Reset(void)
{
    DWT->CYCCNT = 0;
}

/**
 * @brief Get the current cycle count
 * @return Current value of the cycle counter
 */
static inline uint32_t CycleCounter_Get(void)
{
    return DWT->CYCCNT;
}

/**
 * @brief Measure cycles for a code block
 * 
 * Usage:
 *   uint32_t start = CycleCounter_Start();
 *   // ... code to measure ...
 *   uint32_t cycles = CycleCounter_Elapsed(start);
 */
static inline uint32_t CycleCounter_Start(void)
{
    return DWT->CYCCNT;
}

/**
 * @brief Get elapsed cycles since start
 * @param start The value returned by CycleCounter_Start()
 * @return Number of cycles elapsed
 */
static inline uint32_t CycleCounter_Elapsed(uint32_t start)
{
    return DWT->CYCCNT - start;
}

/**
 * @brief Send a string via UART
 * @param str String to send
 */
static inline void UART_SendString(const char* str)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/**
 * @brief Send a formatted line via UART
 * @param format Printf-style format string
 */
static inline void UART_Printf(const char* format, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    UART_SendString(buffer);
}

/**
 * @brief Structure to hold cycle measurement results
 */
typedef struct {
    uint32_t serialize_cycles;
    uint32_t deserialize_cycles;
    const char* library;
    const char* scenario;
} CycleMeasurement;

/**
 * @brief Output cycle measurements via UART in parseable format
 * @param measurement Pointer to measurement results
 */
static inline void CycleCounter_OutputResults(const CycleMeasurement* measurement)
{
    char buffer[64];
    
    UART_SendString("\r\nMEASUREMENT_START\r\n");
    
    if (measurement->library) {
        snprintf(buffer, sizeof(buffer), "LIBRARY: %s\r\n", measurement->library);
        UART_SendString(buffer);
    }
    
    if (measurement->scenario) {
        snprintf(buffer, sizeof(buffer), "SCENARIO: %s\r\n", measurement->scenario);
        UART_SendString(buffer);
    }
    
    snprintf(buffer, sizeof(buffer), "CYCLES_SERIALIZE: %lu\r\n", 
             (unsigned long)measurement->serialize_cycles);
    UART_SendString(buffer);
    
    snprintf(buffer, sizeof(buffer), "CYCLES_DESERIALIZE: %lu\r\n", 
             (unsigned long)measurement->deserialize_cycles);
    UART_SendString(buffer);
    
    snprintf(buffer, sizeof(buffer), "CYCLES_TOTAL: %lu\r\n", 
             (unsigned long)(measurement->serialize_cycles + measurement->deserialize_cycles));
    UART_SendString(buffer);
    
    UART_SendString("MEASUREMENT_END\r\n");
}

#ifdef __cplusplus
}
#endif

#endif /* INC_CYCLE_COUNTER_H_ */
