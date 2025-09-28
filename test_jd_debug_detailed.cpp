#include <iostream>
#include <iomanip>
#include "astro_types.h"

int main() {
    std::cout << "=== STEP-BY-STEP JULIAN DAY DEBUG ===" << std::endl;
    
    // Test parameters
    int year = 2025, month = 1, day = 1;
    double hour = 12.0, minute = 0.0, second = 0.0;
    double timezone = 7.0; // Bangkok UTC+7
    
    std::cout << "Input: " << year << "-" << std::setfill('0') << std::setw(2) << month 
              << "-" << std::setw(2) << day << " " 
              << std::setw(2) << (int)hour << ":" 
              << std::setw(2) << (int)minute << ":" 
              << std::setw(2) << (int)second 
              << " (UTC+" << timezone << ")" << std::endl;
    
    // Step 1: Convert to decimal hours
    double localTime = hour + minute/60.0 + second/3600.0;
    std::cout << "Step 1 - Local time decimal: " << std::fixed << std::setprecision(6) << localTime << " hours" << std::endl;
    
    // Step 2: Convert to UTC
    double utcTime = localTime - timezone;
    std::cout << "Step 2 - UTC time: " << localTime << " - " << timezone << " = " << utcTime << " hours" << std::endl;
    
    // Step 3: Handle date changes
    int utcYear = year, utcMonth = month, utcDay = day;
    
    if (utcTime < 0.0) {
        std::cout << "Step 3a - UTC time is negative, rolling back to previous day" << std::endl;
        utcTime += 24.0;
        utcDay -= 1;
        // ... additional logic
    } else if (utcTime >= 24.0) {
        std::cout << "Step 3b - UTC time >= 24, rolling forward to next day" << std::endl;
        utcTime -= 24.0;
        utcDay += 1;
        // ... additional logic
    } else {
        std::cout << "Step 3c - UTC time is normal (0-24), no date change needed" << std::endl;
    }
    
    std::cout << "Final UTC date/time: " << utcYear << "-" << std::setfill('0') << std::setw(2) << utcMonth 
              << "-" << std::setw(2) << utcDay << " " 
              << std::setfill(' ') << std::fixed << std::setprecision(6) << utcTime << " hours" << std::endl;
    
    // Step 4: Calculate Julian Day (manual approximation)
    // JD for 2025-01-01 00:00:00 UTC is approximately 2460676.5
    double baseJD = 2460676.5; // Jan 1, 2025 00:00 UTC
    double jd = baseJD + utcTime / 24.0;
    std::cout << "Step 4 - Julian Day: " << std::fixed << std::setprecision(6) << jd << std::endl;
    
    // Step 5: Test with BirthData class
    Astro::BirthData birthData;
    birthData.year = year;
    birthData.month = month;
    birthData.day = day;
    birthData.hour = (int)hour;
    birthData.minute = (int)minute;
    birthData.second = (int)second;
    birthData.timezone = timezone;
    
    double birthDataJD = birthData.getJulianDay();
    std::cout << "Step 5 - BirthData JD: " << std::fixed << std::setprecision(6) << birthDataJD << std::endl;
    std::cout << "Match: " << (std::abs(jd - birthDataJD) < 0.000001 ? "YES" : "NO") << std::endl;
    
    return 0;
}