#include <iostream>
#include <iomanip>
#include "third_party/swisseph/swephexp.h"

int main() {
    // Initialize Swiss Ephemeris
    swe_set_ephe_path("/DATA/FlutterCodes/githubCodes/horoscope_cli/data");
    
    std::cout << "=== TESTING SWISS EPHEMERIS WITH KNOWN LOCATIONS ===" << std::endl;
    
    struct Location {
        std::string name;
        double lat;
        double lon;
        double timezone;
        std::string expected_sunrise;
    };
    
    // Test with well-known locations
    Location locations[] = {
        {"New York", 40.7128, -74.0060, -5.0, "~07:00"},
        {"London", 51.5074, -0.1278, 0.0, "~07:30"}, 
        {"Bangkok", 13.7563, 100.5018, 7.0, "~06:06"}
    };
    
    double julianDay = 2460582.0; // 2024-09-28 12:00 UTC
    
    for (const auto& loc : locations) {
        double geopos[3] = {loc.lon, loc.lat, 0.0};
        double tret[10];
        char serr[AS_MAXCH];
        int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR;
        
        std::cout << "\n" << loc.name << " (" << loc.lat << ", " << loc.lon << "):" << std::endl;
        
        // Calculate sunrise
        int result = swe_rise_trans(julianDay, SE_SUN, nullptr, flags, SE_CALC_RISE,
                                   geopos, 1013.25, 15.0, tret, serr);
        
        if (result != ERR) {
            double utcTime = (tret[0] - floor(tret[0])) * 24.0;
            double localTime = utcTime + loc.timezone;
            if (localTime >= 24.0) localTime -= 24.0;
            if (localTime < 0.0) localTime += 24.0;
            
            int h = (int)localTime;
            int m = (int)((localTime - h) * 60);
            
            std::cout << "  Sunrise: " << std::setfill('0') << std::setw(2) << h 
                      << ":" << std::setfill('0') << std::setw(2) << m
                      << " (Expected: " << loc.expected_sunrise << ")" << std::endl;
            std::cout << "  UTC time: " << std::fixed << std::setprecision(2) << utcTime << std::endl;
        } else {
            std::cout << "  Error: " << serr << std::endl;
        }
    }
    
    return 0;
}