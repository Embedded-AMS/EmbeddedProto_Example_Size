#include <cstdint>

// NanoPB test scenario
void run_scenario() {
    // This would contain NanoPB specific scenario code
    // For now, just a simple implementation
    volatile uint32_t counter = 0;
    for(uint32_t i = 0; i < 3000; i++) {
        counter += i * 3;
    }
    // In a real scenario, this would use NanoPB functionality
}