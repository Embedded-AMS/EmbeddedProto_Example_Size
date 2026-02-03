/**
 * @file blinky.cpp
 * @brief Baseline scenario - simple LED blink
 *
 * This scenario provides a baseline measurement with minimal code overhead.
 * It simply toggles the on-board LED (LD2) each time run_scenario() is called.
 */

#include "main.h"

extern "C" void run_scenario()
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}
