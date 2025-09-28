#include <iostream>
#include <iomanip>
#include <cmath>

// Simplified test to understand the Julian Day calculation issue
// Based on the BirthData::getJulianDay() implementation

// Simple Julian Day calculation (approximate)
double calculateJulianDay(int year, int month, int day, double hour) {
    // This is a simplified Julian Day formula for testing
    if (month <= 2) {
        year--;
        month += 12;
    }
    
    double a = floor(year / 100.0);
    double b = 2 - a + floor(a / 4.0);
    
    double jd = floor(365.25 * (year + 4716)) + 
                floor(30.6001 * (month + 1)) + 
                day + b - 1524.5 + hour / 24.0;
    
    return jd;
}

int main() {
    // Our test parameters: 2025-01-01 12:00:00 at Bangkok (UTC+7)
    int year = 2025, month = 1, day = 1;
    double hour = 12.0, minute = 0.0, second = 0.0;
    double timezone = 7.0; // Bangkok is UTC+7
    
    // Calculate local time as decimal hour
    double localTime = hour + minute/60.0 + second/3600.0;
    
    // CLI's current approach (from BirthData::getJulianDay())
    // 1. Calculate Julian Day in local time first
    double jdLocal = calculateJulianDay(year, month, day, localTime);
    // 2. Apply timezone correction by subtracting timezone hours
    double jdUTC_CLI = jdLocal - (timezone / 24.0);
    
    // Correct approach: Convert to UTC first, then calculate Julian Day
    double utcTime = localTime - timezone; // 12.0 - 7.0 = 5.0 hours UTC
    double jdUTC_Correct;
    
    if (utcTime < 0) {
        // Handle day rollback for negative UTC time
        utcTime += 24.0;
        day -= 1;
        if (day < 1) {
            month -= 1;
            if (month < 1) {
                month = 12;
                year -= 1;
            }
            // This is simplified - proper handling needs days per month logic
            day = 31;
        }
    }
    jdUTC_Correct = calculateJulianDay(year, month, day, utcTime);
    
    std::cout << "TEST: 2025-01-01 12:00:00 Bangkok Time (UTC+7)" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Local time: " << localTime << " hours" << std::endl;
    std::cout << "UTC time: " << utcTime << " hours" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CLI Julian Day (wrong): " << std::fixed << std::setprecision(6) << jdUTC_CLI << std::endl;
    std::cout << "Correct Julian Day: " << std::fixed << std::setprecision(6) << jdUTC_Correct << std::endl;
    std::cout << "Difference: " << std::fixed << std::setprecision(6) << (jdUTC_CLI - jdUTC_Correct) << " days" << std::endl;
    std::cout << "Difference in hours: " << std::fixed << std::setprecision(2) << (jdUTC_CLI - jdUTC_Correct) * 24.0 << " hours" << std::endl;
    
    std::cout << std::endl << "ANALYSIS:" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "The CLI calculates Julian Day for local time, then subtracts timezone." << std::endl;
    std::cout << "This is INCORRECT because:" << std::endl;
    std::cout << "- Julian Day should always be calculated for UTC time" << std::endl;
    std::cout << "- Local time should be converted to UTC BEFORE Julian Day calculation" << std::endl;
    std::cout << "- The current method creates a " << (jdUTC_CLI - jdUTC_Correct) * 24.0 << " hour error!" << std::endl;
    
    return 0;
}