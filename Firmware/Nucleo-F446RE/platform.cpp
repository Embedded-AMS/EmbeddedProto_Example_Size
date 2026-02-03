#include <cstdint>

// Platform-specific functions
extern "C" void platform_init() {
    // Initialize platform hardware
}

extern "C" void platform_delay(uint32_t ms) {
    // Simple delay implementation
    volatile uint32_t counter = ms * 1000;
    while(counter--) {
        asm("nop");
    }
}