#include <iostream>
#include <iomanip>
#include "include/kp_system.h"

using namespace Astro;

int main() {
    std::cout << "KP System Nakshatra Validation" << std::endl;
    std::cout << "==============================" << std::endl;
    
    KPSystem kpSystem;
    if (!kpSystem.initialize()) {
        std::cerr << "Failed to initialize KP system" << std::endl;
        return 1;
    }
    
    // Test key nakshatra boundaries
    struct TestCase {
        double longitude;
        const char* expectedNakshatra;
    };
    
    TestCase testCases[] = {
        {0.0, "Ashwini"},
        {13.33, "Bharani"},
        {26.67, "Krittika"},
        {256.92, "Uttara Ashadha"}, // Sun's position
        {272.60, "Uttara Ashadha"}, // Moon's position
        {240.0, "Mula"},
        {253.33, "Purva Ashadha"},
        {266.67, "Uttara Ashadha"},
        {280.0, "Shravana"}
    };
    
    std::cout << std::setw(12) << "Longitude" << " | " 
              << std::setw(18) << "Expected" << " | " 
              << std::setw(18) << "Calculated" << " | " 
              << "Match" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    
    for (const auto& test : testCases) {
        KPPosition kpPos = kpSystem.calculateKPPosition(test.longitude);
        bool matches = (kpPos.nakshatra.name == test.expectedNakshatra);
        
        std::cout << std::setw(12) << std::fixed << std::setprecision(2) << test.longitude << " | "
                  << std::setw(18) << test.expectedNakshatra << " | "
                  << std::setw(18) << kpPos.nakshatra.name << " | "
                  << (matches ? "✓" : "✗") << std::endl;
    }
    
    return 0;
}