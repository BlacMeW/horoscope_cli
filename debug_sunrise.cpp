#include <iostream>
#include <iomanip>
#include "third_party/swisseph/swephexp.h"

int main() {
    // Test coordinates - Bangkok
    double lat = 13.7563;
    double lon = 100.5018;
    double elevation = 0.0;
    
    // Date: January 15, 2024
    double jd = swe_julday(2024, 1, 15, 12.0, SE_GREG_CAL);  // Noon UTC
    
    std::cout << "Testing Swiss Ephemeris sunrise calculation" << std::endl;
    std::cout << "Date: January 15, 2024" << std::endl;
    std::cout << "Location: " << lat << "N, " << lon << "E" << std::endl;
    std::cout << "Julian Day: " << std::fixed << std::setprecision(6) << jd << std::endl;
    
    // Test simple swe_rise_trans
    double geopos[3] = {lon, lat, elevation};
    double riseTime;
    char error[256];
    
    int result = swe_rise_trans(jd, SE_SUN, nullptr, SEFLG_SWIEPH, 
                               SE_CALC_RISE | SE_BIT_DISC_BOTTOM,
                               geopos, 1013.25, 15.0, &riseTime, error);
    
    if (result >= 0) {
        // Convert JD to time
        int year, month, day;
        double hour;
        swe_revjul(riseTime, SE_GREG_CAL, &year, &month, &day, &hour);
        
        int h = (int)hour;
        int m = (int)((hour - h) * 60);
        int s = (int)(((hour - h) * 60 - m) * 60);
        
        std::cout << "Sunrise UTC: " << h << ":" << m << ":" << s << std::endl;
        
        // Convert to Bangkok time (UTC+7)
        hour += 7.0;
        if (hour >= 24.0) {
            hour -= 24.0;
            day++;
        }
        
        h = (int)hour;
        m = (int)((hour - h) * 60);
        s = (int)(((hour - h) * 60 - m) * 60);
        
        std::cout << "Sunrise Bangkok time: " << h << ":" << m << ":" << s << std::endl;
        
    } else {
        std::cout << "Error: " << error << std::endl;
    }
    
    return 0;
}
