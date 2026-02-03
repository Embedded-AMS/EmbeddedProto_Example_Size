#include <cstdint>

// EmbeddedProto test scenario
void run_scenario() {
    // This would contain EmbeddedProto specific scenario code
    // For now, just a simple implementation
    volatile uint32_t counter = 0;
    for(uint32_t i = 0; i < 2000; i++) {
        counter += i * 2;
    }
    // In a real scenario, this would use EmbeddedProto functionality
}