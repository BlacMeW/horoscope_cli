#include <iostream>
#include <iomanip>
#include <sstream>
#include "third_party/swisseph/swephexp.h"

std::string formatTime(double timeInHours) {
    if (timeInHours < 0) timeInHours += 24.0;
    if (timeInHours >= 24.0) timeInHours -= 24.0;
    
    int hours = static_cast<int>(timeInHours);
    int minutes = static_cast<int>((timeInHours - hours) * 60);
    int seconds = static_cast<int>(((timeInHours - hours) * 60 - minutes) * 60);
    
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setfill('0') << std::setw(2) << minutes << ":"
       << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

int main() {
    std::cout << "=== DIAGNOSING SUNRISE/SUNSET CALCULATION ERROR ===" << std::endl;
    
    // Bangkok coordinates
    double latitude = 13.7563;
    double longitude = 100.5018; 
    double julianDay = 2460581.5; // 2024-09-28
    double timezone = 7.0;
    
    // Initialize Swiss Ephemeris
    swe_set_ephe_path("/DATA/FlutterCodes/githubCodes/horoscope_cli/data");
    
    std::cout << "Location: Bangkok (" << latitude << "°N, " << longitude << "°E)" << std::endl;
    std::cout << "Date: 2024-09-28, JD = " << std::setprecision(10) << julianDay << std::endl;
    std::cout << "Timezone: UTC+" << timezone << std::endl << std::endl;
    
    double geopos[3] = {longitude, latitude, 0.0}; // longitude, latitude, altitude
    double tret[10];
    char serr[AS_MAXCH];
    int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR;
    int result;
    
    std::cout << "=== STEP-BY-STEP ANALYSIS ===" << std::endl;
    
    // 1. Test basic rise calculation
    std::cout << "1. Raw swe_rise_trans() RISE output:" << std::endl;
    result = swe_rise_trans(julianDay, SE_SUN, nullptr, flags, SE_CALC_RISE,
                           geopos, 1013.25, 15.0, tret, serr);
    
    if (result != ERR) {
        std::cout << "   Result code: " << result << std::endl;
        std::cout << "   Raw Julian Day: " << std::setprecision(10) << tret[0] << std::endl;
        
        // Extract time portion
        double timeFraction = tret[0] - floor(tret[0]);
        double utcHours = timeFraction * 24.0;
        std::cout << "   Time fraction: " << timeFraction << std::endl;
        std::cout << "   UTC hours: " << utcHours << " = " << formatTime(utcHours) << " UTC" << std::endl;
        
        // Apply timezone
        double localHours = utcHours + timezone;
        std::cout << "   Local hours (UTC+" << timezone << "): " << localHours << " = " << formatTime(localHours) << std::endl;
        
        // Handle day overflow
        while (localHours >= 24.0) localHours -= 24.0;
        while (localHours < 0.0) localHours += 24.0;
        std::cout << "   Normalized local: " << formatTime(localHours) << std::endl;
        
    } else {
        std::cout << "   ERROR: " << serr << std::endl;
    }
    
    std::cout << std::endl << "2. Raw swe_rise_trans() SET output:" << std::endl;
    result = swe_rise_trans(julianDay, SE_SUN, nullptr, flags, SE_CALC_SET,
                           geopos, 1013.25, 15.0, tret, serr);
    
    if (result != ERR) {
        std::cout << "   Result code: " << result << std::endl;
        std::cout << "   Raw Julian Day: " << std::setprecision(10) << tret[0] << std::endl;
        
        // Extract time portion
        double timeFraction = tret[0] - floor(tret[0]);
        double utcHours = timeFraction * 24.0;
        std::cout << "   Time fraction: " << timeFraction << std::endl;
        std::cout << "   UTC hours: " << utcHours << " = " << formatTime(utcHours) << " UTC" << std::endl;
        
        // Apply timezone
        double localHours = utcHours + timezone;
        std::cout << "   Local hours (UTC+" << timezone << "): " << localHours << " = " << formatTime(localHours) << std::endl;
        
        // Handle day overflow
        while (localHours >= 24.0) localHours -= 24.0;
        while (localHours < 0.0) localHours += 24.0;
        std::cout << "   Normalized local: " << formatTime(localHours) << std::endl;
        
    } else {
        std::cout << "   ERROR: " << serr << std::endl;
    }
    
    std::cout << std::endl << "=== EXPECTED VALUES FOR BANGKOK ===" << std::endl;
    std::cout << "Expected Sunrise: ~06:00-06:30 (6 AM)" << std::endl;
    std::cout << "Expected Sunset:  ~18:00-18:30 (6 PM)" << std::endl;
    
    // Test different Julian Day representations
    std::cout << std::endl << "=== TESTING DIFFERENT TIME INPUTS ===" << std::endl;
    
    // Test midnight vs noon
    double jdMidnight = floor(julianDay) + 0.5; // Midnight UTC
    double jdNoon = floor(julianDay);            // Noon UTC
    
    std::cout << "Original JD: " << julianDay << " (assumed midnight local)" << std::endl;
    std::cout << "JD Midnight: " << jdMidnight << std::endl;
    std::cout << "JD Noon:     " << jdNoon << std::endl;
    
    // Test with midnight
    std::cout << "\nTesting with JD for midnight UTC:" << std::endl;
    result = swe_rise_trans(jdMidnight, SE_SUN, nullptr, flags, SE_CALC_RISE,
                           geopos, 1013.25, 15.0, tret, serr);
    if (result != ERR) {
        double timeFraction = tret[0] - floor(tret[0]);
        double localHours = timeFraction * 24.0 + timezone;
        while (localHours >= 24.0) localHours -= 24.0;
        while (localHours < 0.0) localHours += 24.0;
        std::cout << "   Sunrise: " << formatTime(localHours) << std::endl;
    }
    
    swe_close();
    return 0;
}