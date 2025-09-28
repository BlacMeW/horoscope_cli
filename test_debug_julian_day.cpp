#include <iostream>
#include <iomanip>
#include "astro_types.h"

int main() {
    std::cout << "=== DEBUGGING JULIAN DAY CALCULATION ===" << std::endl;
    
    // Create BirthData for our test case
    Astro::BirthData birthData;
    birthData.year = 2025;
    birthData.month = 1;
    birthData.day = 1;
    birthData.hour = 12;
    birthData.minute = 0;
    birthData.second = 0;
    birthData.timezone = 7.0;  // Bangkok UTC+7
    birthData.latitude = 13.7563;
    birthData.longitude = 100.5018;
    
    std::cout << "Input: 2025-01-01 12:00:00 Bangkok Time (UTC+7)" << std::endl;
    std::cout << "Timezone: " << birthData.timezone << std::endl;
    
    double julianDay = birthData.getJulianDay();
    std::cout << "Calculated Julian Day: " << std::fixed << std::setprecision(6) << julianDay << std::endl;
    
    // Manual calculation for comparison
    double localTime = 12.0 + 0.0/60.0 + 0.0/3600.0;  // 12.0
    double utcTime = localTime - 7.0;  // 5.0
    std::cout << "Local time: " << localTime << " hours" << std::endl;
    std::cout << "Expected UTC time: " << utcTime << " hours" << std::endl;
    std::cout << "Expected Julian Day for 2025-01-01 05:00:00 UTC: ~2460677.708333" << std::endl;
    
    return 0;
}