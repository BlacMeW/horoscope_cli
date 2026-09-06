#include <iostream>
#include <iomanip>
#include "swephexp.h"

// Test the Julian Day calculation that's causing the planetary position errors

int main() {
    // Our test parameters: 2025-01-01 12:00:00 at Bangkok coordinates
    int year = 2025, month = 1, day = 1;
    double hour = 12.0, minute = 0.0, second = 0.0;
    double timezone = 7.0; // Bangkok is UTC+7
    double latitude = 13.7563, longitude = 100.5018;
    
    // Calculate local time as decimal hour
    double localTime = hour + minute/60.0 + second/3600.0;
    
    // CLI's current calculation (from BirthData::getJulianDay())
    double jdLocal = swe_julday(year, month, day, localTime, SE_GREG_CAL);
    double jdUTC_CLI = jdLocal - (timezone / 24.0);
    
    // Swiss Ephemeris direct UTC calculation
    double utcTime = localTime - timezone; // Convert local to UTC first
    if (utcTime < 0) {
        // Handle day rollback if needed
        utcTime += 24.0;
        day -= 1;
        if (day < 1) {
            month -= 1;
            if (month < 1) {
                month = 12;
                year -= 1;
            }
            // Get last day of previous month (simplified)
            day = 31; // Approximate - for this test it's fine
        }
    }
    double jdUTC_Correct = swe_julday(year, month, day, utcTime, SE_GREG_CAL);
    
    std::cout << "TEST: 2025-01-01 12:00:00 Bangkok Time (UTC+7)" << std::endl;
    std::cout << "=================================" << std::endl;
    
    std::cout << "Local time decimal: " << std::fixed << std::setprecision(6) << localTime << " hours" << std::endl;
    std::cout << "UTC time decimal: " << std::fixed << std::setprecision(6) << (localTime - timezone) << " hours" << std::endl;
    
    std::cout << std::endl;
    std::cout << "CLI Julian Day (current): " << std::fixed << std::setprecision(6) << jdUTC_CLI << std::endl;
    std::cout << "Correct Julian Day: " << std::fixed << std::setprecision(6) << jdUTC_Correct << std::endl;
    std::cout << "Difference: " << std::fixed << std::setprecision(6) << (jdUTC_CLI - jdUTC_Correct) << " days" << std::endl;
    std::cout << "Difference in hours: " << std::fixed << std::setprecision(2) << (jdUTC_CLI - jdUTC_Correct) * 24.0 << " hours" << std::endl;
    
    // Test planetary positions with both Julian Days
    double xx1[6], xx2[6];
    char serr[256];
    
    std::cout << std::endl << "PLANETARY POSITION COMPARISON:" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Sun with CLI Julian Day
    if (swe_calc(jdUTC_CLI, SE_SUN, SEFLG_SWIEPH, xx1, serr) >= 0) {
        std::cout << "Sun (CLI JD): " << std::fixed << std::setprecision(6) << xx1[0] << "°" << std::endl;
    }
    
    // Sun with correct Julian Day
    if (swe_calc(jdUTC_Correct, SE_SUN, SEFLG_SWIEPH, xx2, serr) >= 0) {
        std::cout << "Sun (Correct JD): " << std::fixed << std::setprecision(6) << xx2[0] << "°" << std::endl;
    }
    
    std::cout << "Sun difference: " << std::fixed << std::setprecision(6) << (xx1[0] - xx2[0]) << "°" << std::endl;
    
    // Moon with CLI Julian Day
    if (swe_calc(jdUTC_CLI, SE_MOON, SEFLG_SWIEPH, xx1, serr) >= 0) {
        std::cout << "Moon (CLI JD): " << std::fixed << std::setprecision(6) << xx1[0] << "°" << std::endl;
    }
    
    // Moon with correct Julian Day  
    if (swe_calc(jdUTC_Correct, SE_MOON, SEFLG_SWIEPH, xx2, serr) >= 0) {
        std::cout << "Moon (Correct JD): " << std::fixed << std::setprecision(6) << xx2[0] << "°" << std::endl;
    }
    
    std::cout << "Moon difference: " << std::fixed << std::setprecision(6) << (xx1[0] - xx2[0]) << "°" << std::endl;
    
    return 0;
}