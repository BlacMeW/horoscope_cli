#include <iostream>
#include <iomanip>
#include "swephexp.h"

int main() {
    std::cout << "=== SWISS EPHEMERIS VALIDATION: 2025-01-01 05:00 UTC ===" << std::endl;
    
    // Initialize Swiss Ephemeris with our data path
    swe_set_ephe_path("./data");
    
    // Julian Day for 2025-01-01 05:00:00 UTC
    double julianDay = 2460676.708333;
    
    std::cout << "Julian Day: " << std::fixed << std::setprecision(6) << julianDay << std::endl;
    std::cout << "Date: 2025-01-01 05:00:00 UTC" << std::endl;
    std::cout << std::endl;
    
    double xx[6];
    char serr[256];
    
    // Calculate Sun position
    if (swe_calc(julianDay, SE_SUN, SEFLG_SWIEPH, xx, serr) >= 0) {
        double sunLong = xx[0];
        int sign = (int)(sunLong / 30.0);
        double signPos = fmod(sunLong, 30.0);
        int degrees = (int)signPos;
        int minutes = (int)((signPos - degrees) * 60.0);
        
        const char* signs[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
                              "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
        
        std::cout << "Swiss Ephemeris Sun:" << std::endl;
        std::cout << "  Longitude: " << std::fixed << std::setprecision(6) << sunLong << "°" << std::endl;
        std::cout << "  Position: " << std::setfill('0') << std::setw(2) << degrees 
                  << "°" << std::setw(2) << minutes << "' " << signs[sign] << std::endl;
    } else {
        std::cout << "Error calculating Sun: " << serr << std::endl;
    }
    
    // Calculate Moon position  
    if (swe_calc(julianDay, SE_MOON, SEFLG_SWIEPH, xx, serr) >= 0) {
        double moonLong = xx[0];
        int sign = (int)(moonLong / 30.0);
        double signPos = fmod(moonLong, 30.0);
        int degrees = (int)signPos;
        int minutes = (int)((signPos - degrees) * 60.0);
        
        const char* signs[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
                              "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
        
        std::cout << "Swiss Ephemeris Moon:" << std::endl;
        std::cout << "  Longitude: " << std::fixed << std::setprecision(6) << moonLong << "°" << std::endl;
        std::cout << "  Position: " << std::setfill('0') << std::setw(2) << degrees 
                  << "°" << std::setw(2) << minutes << "' " << signs[sign] << std::endl;
    } else {
        std::cout << "Error calculating Moon: " << serr << std::endl;
    }
    
    std::cout << std::endl << "COMPARISON:" << std::endl;
    std::cout << "CLI shows: Sun 11°01' Capricorn, Moon 26°42' Capricorn" << std::endl;
    std::cout << "If Swiss Ephemeris matches CLI, then CLI is CORRECT!" << std::endl;
    
    return 0;
}