#include <iostream>
#include <iomanip>
#include "swephexp.h"

int main() {
    std::cout << "=== VALIDATION: 2025-01-01 12:00:00 Bangkok Time (UTC+7) ===" << std::endl;
    
    // Convert Bangkok time to UTC: 12:00:00 - 7:00:00 = 05:00:00 UTC on same day
    double jd_utc = swe_julday(2025, 1, 1, 5.0, SE_GREG_CAL); // 05:00:00 UTC
    
    std::cout << "Date: 2025-01-01 05:00:00 UTC (12:00:00 Bangkok)" << std::endl;
    std::cout << "Julian Day: " << std::fixed << std::setprecision(6) << jd_utc << std::endl;
    std::cout << std::endl;
    
    double xx[6];
    char serr[256];
    
    // Sun position
    if (swe_calc(jd_utc, SE_SUN, SEFLG_SWIEPH, xx, serr) >= 0) {
        std::cout << "Sun longitude: " << std::fixed << std::setprecision(6) << xx[0] << "°" << std::endl;
        
        // Convert to sign position
        int sign = (int)(xx[0] / 30.0);
        double signPos = fmod(xx[0], 30.0);
        int degrees = (int)signPos;
        int minutes = (int)((signPos - degrees) * 60.0);
        
        const char* signs[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
                             "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
        
        std::cout << "Sun position: " << std::setfill('0') << std::setw(2) << degrees 
                  << "°" << std::setw(2) << minutes << "' " << signs[sign] << std::endl;
    }
    
    // Moon position
    if (swe_calc(jd_utc, SE_MOON, SEFLG_SWIEPH, xx, serr) >= 0) {
        std::cout << "Moon longitude: " << std::fixed << std::setprecision(6) << xx[0] << "°" << std::endl;
        
        // Convert to sign position
        int sign = (int)(xx[0] / 30.0);
        double signPos = fmod(xx[0], 30.0);
        int degrees = (int)signPos;
        int minutes = (int)((signPos - degrees) * 60.0);
        
        const char* signs[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
                             "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
        
        std::cout << "Moon position: " << std::setfill('0') << std::setw(2) << degrees 
                  << "°" << std::setw(2) << minutes << "' " << signs[sign] << std::endl;
    }
    
    std::cout << std::endl << "COMPARISON WITH CLI OUTPUT:" << std::endl;
    std::cout << "CLI shows: Sun 11°01' Capricorn, Moon 26°42' Capricorn" << std::endl;
    std::cout << "Swiss Ephemeris should show the correct values above." << std::endl;
    
    return 0;
}