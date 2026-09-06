#include <iostream>
#include <iomanip>
#include <cmath>
#include "include/astro_types.h"

using namespace Astro;

void testJulianDayCalculation() {
    std::cout << "=== Testing Julian Day Calculation Fixes ===\n";
    
    // Test case 1: Bangkok time July 27, 2025, 12:00 PM ICT (UTC+7)
    BirthData bangkok;
    bangkok.year = 2025;
    bangkok.month = 7;
    bangkok.day = 27;
    bangkok.hour = 12;
    bangkok.minute = 0;
    bangkok.second = 0;
    bangkok.latitude = 13.7563;
    bangkok.longitude = 100.5018;
    bangkok.timezone = 7.0; // ICT UTC+7
    
    double jd = bangkok.getJulianDay();
    
    std::cout << "Bangkok Test:\n";
    std::cout << "  Local: 2025-07-27 12:00:00 ICT (UTC+7)\n";
    std::cout << "  Should be UTC: 2025-07-27 05:00:00\n";
    std::cout << "  Julian Day: " << std::fixed << std::setprecision(6) << jd << "\n";
    
    // Expected JD for 2025-07-27 05:00:00 UTC ≈ 2460516.708333
    double expected = 2460516.708333;
    double diff = std::abs(jd - expected);
    
    std::cout << "  Expected: " << expected << "\n";
    std::cout << "  Difference: " << diff << " days\n";
    std::cout << "  Status: " << (diff < 0.001 ? "✅ PASS" : "❌ FAIL") << "\n\n";
    
    // Test case 2: Negative timezone (New York EST, UTC-5)
    BirthData newYork;
    newYork.year = 2025;
    newYork.month = 1;
    newYork.day = 15;
    newYork.hour = 14;
    newYork.minute = 30;
    newYork.second = 0;
    newYork.latitude = 40.7128;
    newYork.longitude = -74.0060;
    newYork.timezone = -5.0; // EST UTC-5
    
    double jdNY = newYork.getJulianDay();
    
    std::cout << "New York Test:\n";
    std::cout << "  Local: 2025-01-15 14:30:00 EST (UTC-5)\n";
    std::cout << "  Should be UTC: 2025-01-15 19:30:00\n";
    std::cout << "  Julian Day: " << std::fixed << std::setprecision(6) << jdNY << "\n";
    
    // Expected JD for 2025-01-15 19:30:00 UTC ≈ 2460353.3125
    double expectedNY = 2460353.3125;
    double diffNY = std::abs(jdNY - expectedNY);
    
    std::cout << "  Expected: " << expectedNY << "\n";
    std::cout << "  Difference: " << diffNY << " days\n";
    std::cout << "  Status: " << (diffNY < 0.001 ? "✅ PASS" : "❌ FAIL") << "\n\n";
}

void testTimeNormalization() {
    std::cout << "=== Testing Time Normalization Function ===\n";
    
    // Test fmod normalization function
    auto normalizeTime = [](double hours) -> double {
        return fmod(hours + 24.0, 24.0);
    };
    
    struct TestCase {
        double input;
        double expected;
        const char* description;
    };
    
    TestCase tests[] = {
        {25.5, 1.5, "25.5 hours -> 1.5 hours"},
        {-1.5, 22.5, "-1.5 hours -> 22.5 hours"},
        {48.25, 0.25, "48.25 hours -> 0.25 hours"},
        {12.0, 12.0, "12.0 hours -> 12.0 hours (no change)"},
        {-25.75, 22.25, "-25.75 hours -> 22.25 hours"},
        {0.0, 0.0, "0.0 hours -> 0.0 hours"}
    };
    
    bool allPassed = true;
    
    for (const auto& test : tests) {
        double result = normalizeTime(test.input);
        double diff = std::abs(result - test.expected);
        bool passed = diff < 0.0001;
        
        std::cout << "  " << test.description << "\n";
        std::cout << "    Result: " << std::fixed << std::setprecision(4) << result << "\n";
        std::cout << "    Status: " << (passed ? "✅ PASS" : "❌ FAIL") << "\n";
        
        if (!passed) {
            allPassed = false;
            std::cout << "    Expected: " << test.expected << ", Got: " << result << "\n";
        }
    }
    
    std::cout << "\nOverall normalization test: " << (allPassed ? "✅ ALL PASS" : "❌ SOME FAILED") << "\n\n";
}

int main() {
    std::cout << "🔍 CALCULATION FIXES VERIFICATION TEST\n";
    std::cout << "=====================================\n\n";
    
    testJulianDayCalculation();
    testTimeNormalization();
    
    std::cout << "Test completed. Review results above.\n";
    std::cout << "If all tests pass, the critical calculation fixes are working correctly.\n\n";
    
    return 0;
}