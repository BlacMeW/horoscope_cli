#include <iostream>
#include <iomanip>
#include <cmath>
#include "third_party/swisseph/swephexp.h"

int main() {
    // Test coordinates - Bangkok 
    double latitude = 13.7563;
    double longitude = 100.5018;
    double timezone = 7.0;  // UTC+7
    
    // Date: January 15, 2024, 12:30 PM local time
    double jd_original = swe_julday(2024, 1, 15, 12.5 - timezone, SE_GREG_CAL);  // UTC time
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "=== Julian Day Debug ===" << std::endl;
    std::cout << "Original JD (noon UTC): " << jd_original << std::endl;
    
    // The problematic calculation from Hindu calendar
    double timezoneOffset = 7.0;  // Bangkok
    double localMidnightJD = floor(jd_original - timezoneOffset / 24.0) + 0.5 + timezoneOffset / 24.0;
    
    std::cout << "Local midnight JD: " << localMidnightJD << std::endl;
    
    // Let's test Swiss Ephemeris with different JD values
    double geopos[3] = {longitude, latitude, 0.0};
    double riseTime;
    char error[256];
    
    // Test 1: Using original JD
    std::cout << "\n=== Test 1: Original JD ===" << std::endl;
    int result1 = swe_rise_trans(jd_original, SE_SUN, nullptr, SEFLG_SWIEPH, 
                                SE_CALC_RISE | SE_BIT_DISC_BOTTOM,
                                geopos, 1013.25, 15.0, &riseTime, error);
    if (result1 >= 0) {
        int year, month, day;
        double hour;
        swe_revjul(riseTime, SE_GREG_CAL, &year, &month, &day, &hour);
        std::cout << "Sunrise JD: " << riseTime << std::endl;
        std::cout << "Sunrise UTC: " << (int)hour << ":" << (int)((hour - (int)hour) * 60) << std::endl;
        
        // The Hindu calendar calculation
        double timeOffset = riseTime - jd_original;
        double hoursFromMidnight = timeOffset * 24.0;
        std::cout << "Time offset: " << timeOffset << " days" << std::endl;
        std::cout << "Hours from 'midnight': " << hoursFromMidnight << std::endl;
        
        // What the Hindu calendar stores
        double hindu_time = hoursFromMidnight - floor(hoursFromMidnight / 24.0) * 24.0;
        std::cout << "Hindu calendar time: " << hindu_time << std::endl;
        
        int h = (int)hindu_time;
        int m = (int)((hindu_time - h) * 60);
        int s = (int)(((hindu_time - h) * 60 - m) * 60);
        std::cout << "Formatted time: " << h << ":" << m << ":" << s << std::endl;
    }
    
    // Test 2: Using local midnight JD
    std::cout << "\n=== Test 2: Local Midnight JD ===" << std::endl;
    int result2 = swe_rise_trans(localMidnightJD, SE_SUN, nullptr, SEFLG_SWIEPH, 
                                SE_CALC_RISE | SE_BIT_DISC_BOTTOM,
                                geopos, 1013.25, 15.0, &riseTime, error);
    if (result2 >= 0) {
        std::cout << "Sunrise JD: " << riseTime << std::endl;
        
        double timeOffset = riseTime - localMidnightJD;
        double hoursFromMidnight = timeOffset * 24.0;
        std::cout << "Time offset: " << timeOffset << " days" << std::endl;
        std::cout << "Hours from midnight: " << hoursFromMidnight << std::endl;
        
        double hindu_time = hoursFromMidnight - floor(hoursFromMidnight / 24.0) * 24.0;
        std::cout << "Hindu calendar time: " << hindu_time << std::endl;
        
        int h = (int)hindu_time;
        int m = (int)((hindu_time - h) * 60);
        int s = (int)(((hindu_time - h) * 60 - m) * 60);
        std::cout << "Formatted time: " << h << ":" << m << ":" << s << std::endl;
    }
    
    return 0;
}
