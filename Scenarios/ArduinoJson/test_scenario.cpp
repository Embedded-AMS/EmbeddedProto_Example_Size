#include <cstdint>

// ArduinoJson test scenario
void run_scenario() {
    // This would contain ArduinoJson specific scenario code
    // For now, just a simple implementation
    volatile uint32_t counter = 0;
    for(uint32_t i = 0; i < 1000; i++) {
        counter += i;
    }
    // In a real scenario, this would use ArduinoJson functionality
}