#include <iostream>
#include <iomanip>
#include <vector>
#include "third_party/swisseph/swephexp.h"

int main() {
    // Initialize Swiss Ephemeris
    swe_set_ephe_path("/DATA/FlutterCodes/githubCodes/horoscope_cli/data");
    
    // Bangkok coordinates
    double latitude = 13.7563;
    double longitude = 100.5018;
    double timezone = 7.0;
    
    std::cout << "=== FINDING CORRECT JULIAN DAY FOR SUNRISE ~06:06 ===" << std::endl;
    
    // Test different Julian Day values to find which gives sunrise ~06:06
    std::vector<double> testJDs = {
        2460581.0,   // 2024-09-27 12:00 UTC
        2460581.5,   // 2024-09-28 00:00 UTC  
        2460582.0,   // 2024-09-28 12:00 UTC
        2460582.5,   // 2024-09-29 00:00 UTC
        2460582.7,   // 2024-09-28 16:48 UTC (local midnight in Bangkok)
    };
    
    for (double jd : testJDs) {
        double geopos[3] = {longitude, latitude, 0.0};
        double tret[10];
        char serr[AS_MAXCH];
        int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR;
        
        int result = swe_rise_trans(jd, SE_SUN, nullptr, flags, SE_CALC_RISE,
                                   geopos, 1013.25, 15.0, tret, serr);
        
        if (result != ERR) {
            double utcTime = (tret[0] - floor(tret[0])) * 24.0;
            double localTime = utcTime + timezone;
            if (localTime >= 24.0) localTime -= 24.0;
            if (localTime < 0.0) localTime += 24.0;
            
            int h = (int)localTime;
            int m = (int)((localTime - h) * 60);
            
            std::cout << "JD " << std::fixed << std::setprecision(1) << jd 
                      << " -> Sunrise " << std::setfill('0') << std::setw(2) << h 
                      << ":" << std::setfill('0') << std::setw(2) << m
                      << " (UTC " << std::setprecision(2) << utcTime << ")" << std::endl;
        }
    }
    
    return 0;
}