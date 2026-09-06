#include <iostream>
#include <iomanip>
#include "third_party/swisseph/swephexp.h"

int main() {
    std::cout << "=== SWE_RISE_TRANS OUTPUT ANALYSIS ===" << std::endl;
    
    // Initialize Swiss Ephemeris
    swe_set_ephe_path("/DATA/FlutterCodes/githubCodes/horoscope_cli/data");
    
    // Bangkok coordinates
    double latitude = 13.7563;
    double longitude = 100.5018;
    double geopos[3] = {longitude, latitude, 0.0};
    double tret[10];
    char serr[AS_MAXCH];
    int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR;
    
    // Test Julian Day: 2024-09-28 noon UTC
    double julianDay = 2460582.0; 
    
    std::cout << "Input Julian Day: " << std::fixed << std::setprecision(6) << julianDay << " (2024-09-28 12:00 UTC)" << std::endl;
    std::cout << "Location: Bangkok (13.7563°N, 100.5018°E)" << std::endl << std::endl;
    
    // Calculate sunrise
    int result = swe_rise_trans(julianDay, SE_SUN, nullptr, flags, SE_CALC_RISE,
                               geopos, 1013.25, 15.0, tret, serr);
    
    if (result != ERR) {
        std::cout << "SUNRISE CALCULATION:" << std::endl;
        std::cout << "Raw swe_rise_trans return: " << std::fixed << std::setprecision(6) << tret[0] << std::endl;
        
        // Breakdown the Julian Day
        double wholeDays = floor(tret[0]);
        double timeFraction = tret[0] - wholeDays;
        double timeHours = timeFraction * 24.0;
        
        std::cout << "Whole days: " << std::fixed << std::setprecision(0) << wholeDays << std::endl;
        std::cout << "Time fraction: " << std::fixed << std::setprecision(6) << timeFraction << std::endl;
        std::cout << "Time in hours: " << std::fixed << std::setprecision(6) << timeHours << std::endl;
        
        // Convert to HMS
        int hours = (int)timeHours;
        int minutes = (int)((timeHours - hours) * 60);
        int seconds = (int)(((timeHours - hours) * 60 - minutes) * 60);
        
        std::cout << "UTC Time: " << std::setfill('0') << std::setw(2) << hours << ":" 
                  << std::setfill('0') << std::setw(2) << minutes << ":" 
                  << std::setfill('0') << std::setw(2) << seconds << std::endl;
        
        // Try different timezone conversions
        double bangkok_add = timeHours + 7.0;
        if (bangkok_add >= 24.0) bangkok_add -= 24.0;
        
        double bangkok_sub = timeHours - 7.0;
        if (bangkok_sub < 0.0) bangkok_sub += 24.0;
        
        int h1 = (int)bangkok_add, m1 = (int)((bangkok_add - h1) * 60);
        int h2 = (int)bangkok_sub, m2 = (int)((bangkok_sub - h2) * 60);
        
        std::cout << "Bangkok Time (+7): " << std::setfill('0') << std::setw(2) << h1 << ":" 
                  << std::setfill('0') << std::setw(2) << m1 << std::endl;
        std::cout << "Bangkok Time (-7): " << std::setfill('0') << std::setw(2) << h2 << ":" 
                  << std::setfill('0') << std::setw(2) << m2 << std::endl;
        
    } else {
        std::cout << "Error: " << serr << std::endl;
    }
    
    std::cout << "\nExpected sunrise in Bangkok: ~06:06" << std::endl;
    
    return 0;
}