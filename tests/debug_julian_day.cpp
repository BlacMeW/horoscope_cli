#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "third_party/swisseph/swephexp.h"

// Function to convert calendar date to Julian Day (from swisseph)
double dateToJulianDay(int year, int month, int day, double hour = 12.0) {
    int calendar = SE_GREG_CAL; // Gregorian calendar
    
    return swe_julday(year, month, day, hour, calendar);
}

int main() {
    std::cout << "=== JULIAN DAY DEBUGGING ===" << std::endl;
    
    // Initialize Swiss Ephemeris
    swe_set_ephe_path("/DATA/FlutterCodes/githubCodes/horoscope_cli/data");
    
    // Calculate Julian Day for 2024-09-28 at different times
    double jd_midnight_utc = dateToJulianDay(2024, 9, 28, 0.0);     // Midnight UTC
    double jd_noon_utc = dateToJulianDay(2024, 9, 28, 12.0);       // Noon UTC  
    double jd_midnight_local = dateToJulianDay(2024, 9, 28, 0.0 - 7.0); // Midnight Bangkok time in UTC
    
    std::cout << "2024-09-28 00:00 UTC: JD = " << std::fixed << std::setprecision(6) << jd_midnight_utc << std::endl;
    std::cout << "2024-09-28 12:00 UTC: JD = " << std::fixed << std::setprecision(6) << jd_noon_utc << std::endl;
    std::cout << "2024-09-28 00:00 Bangkok (17:00 UTC prev day): JD = " << std::fixed << std::setprecision(6) << jd_midnight_local << std::endl;
    
    // Test sunset calculation with different Julian Day values
    double latitude = 13.7563;
    double longitude = 100.5018;
    double timezone = 7.0;
    
    std::cout << "\n=== SUNSET CALCULATION COMPARISON ===" << std::endl;
    
    // Test with each Julian Day
    std::vector<std::pair<std::string, double>> test_jds = {
        {"Midnight UTC", jd_midnight_utc},
        {"Noon UTC", jd_noon_utc},
        {"Midnight Local", jd_midnight_local}
    };
    
    for (const auto& test : test_jds) {
        double geopos[3] = {longitude, latitude, 0.0};
        double tret[10];
        char serr[AS_MAXCH];
        int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR;
        
        // Calculate sunrise
        int result_rise = swe_rise_trans(test.second, SE_SUN, nullptr, flags, SE_CALC_RISE,
                                        geopos, 1013.25, 15.0, tret, serr);
        
        double sunrise_local = -1;
        if (result_rise != ERR) {
            sunrise_local = (tret[0] - floor(tret[0])) * 24.0 + timezone;
            if (sunrise_local >= 24.0) sunrise_local -= 24.0;
            if (sunrise_local < 0.0) sunrise_local += 24.0;
        }
        
        // Calculate sunset
        int result_set = swe_rise_trans(test.second, SE_SUN, nullptr, flags, SE_CALC_SET,
                                       geopos, 1013.25, 15.0, tret, serr);
        
        double sunset_local = -1;
        if (result_set != ERR) {
            sunset_local = (tret[0] - floor(tret[0])) * 24.0 + timezone;
            if (sunset_local >= 24.0) sunset_local -= 24.0;
            if (sunset_local < 0.0) sunset_local += 24.0;
        }
        
        std::cout << test.first << " (JD " << std::fixed << std::setprecision(1) << test.second << "): ";
        std::cout << "Sunrise " << std::setfill('0') << std::setw(2) << (int)sunrise_local << ":" 
                  << std::setfill('0') << std::setw(2) << (int)((sunrise_local - (int)sunrise_local) * 60) << ", ";
        std::cout << "Sunset " << std::setfill('0') << std::setw(2) << (int)sunset_local << ":" 
                  << std::setfill('0') << std::setw(2) << (int)((sunset_local - (int)sunset_local) * 60) << std::endl;
    }
    
    return 0;
}