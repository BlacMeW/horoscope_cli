#include <iostream>
#include <swephexp.h>

int main() {
    // Debug exact Swiss Ephemeris parameters used by both programs
    double julianDay = 2460581.5;
    double longitude = 100.5018;
    double latitude = 13.7563;
    
    // Extract date from Julian Day 
    int year, month, day, hour, minute;
    double second;
    swe_jdet_to_utc(julianDay, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);
    
    std::cout << "=== PARAMETER DEBUGGING ===" << std::endl;
    std::cout << "Julian Day: " << julianDay << std::endl;
    std::cout << "Date: " << year << "-" << month << "-" << day << std::endl;
    std::cout << "Time: " << hour << ":" << minute << ":" << second << std::endl;
    std::cout << "Longitude: " << longitude << std::endl;
    std::cout << "Latitude: " << latitude << std::endl;
    
    // Calculate timezone offset
    double hour_offset = longitude / 15.0;
    std::cout << "Longitude-based timezone: " << hour_offset << " hours" << std::endl;
    
    // Bangkok September atmospheric parameters
    std::cout << "September atmospheric (rainy season):" << std::endl;
    std::cout << "Pressure: 1008.0 mbar" << std::endl;
    std::cout << "Temperature: 28.0°C" << std::endl;
    
    // Test Swiss Ephemeris rise calculation
    double geopos[3] = {longitude, latitude, 0.0};
    double tret[10];
    char serr[256];
    int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR;
    
    swe_set_ephe_path("data");
    
    int result = swe_rise_trans(julianDay, SE_SUN, nullptr, flags, SE_CALC_RISE,
                               geopos, 1008.0, 28.0, tret, serr);
    
    if (result != ERR) {
        std::cout << "Swiss Ephemeris sunrise JD: " << tret[0] << std::endl;
        
        // Convert to calendar time
        swe_jdut1_to_utc(tret[0], 1, &year, &month, &day, &hour, &minute, &second);
        std::cout << "Sunrise UTC: " << hour << ":" << minute << ":" << (int)second << std::endl;
        
        // Apply timezone
        hour += (int)hour_offset;
        minute += (int)((hour_offset - (int)hour_offset) * 60.0);
        if (minute >= 60) {
            hour += 1;
            minute -= 60;
        }
        if (hour >= 24) hour -= 24;
        
        std::cout << "Sunrise Local: " << hour << ":" << minute << ":" << (int)second << std::endl;
    } else {
        std::cout << "Error: " << serr << std::endl;
    }
    
    swe_close();
    return 0;
}