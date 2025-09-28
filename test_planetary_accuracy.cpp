#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "third_party/swisseph/swephexp.h"

struct PlanetTest {
    int planet;
    const char* name;
};

int main() {
    std::cout << "=== PLANETARY POSITION ACCURACY TEST ===" << std::endl;
    
    // Test date: January 15, 2024, 12:30 UTC
    double jd = swe_julday(2024, 1, 15, 12.5, SE_GREG_CAL);
    std::cout << "Test Date: January 15, 2024, 12:30 UTC" << std::endl;
    std::cout << "Julian Day: " << std::fixed << std::setprecision(6) << jd << std::endl;
    
    std::vector<PlanetTest> planets = {
        {SE_SUN, "Sun"},
        {SE_MOON, "Moon"}, 
        {SE_MERCURY, "Mercury"},
        {SE_VENUS, "Venus"},
        {SE_MARS, "Mars"},
        {SE_JUPITER, "Jupiter"},
        {SE_SATURN, "Saturn"},
        {SE_URANUS, "Uranus"},
        {SE_NEPTUNE, "Neptune"},
        {SE_PLUTO, "Pluto"},
        {SE_TRUE_NODE, "North Node"}
    };
    
    char error[256];
    double position[6];
    
    std::cout << std::endl << "=== TROPICAL POSITIONS ===" << std::endl;
    std::cout << std::setw(12) << "Planet" << std::setw(12) << "Longitude" << std::setw(12) << "Latitude" 
              << std::setw(12) << "Distance" << std::setw(12) << "Speed" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& planet : planets) {
        int result = swe_calc(jd, planet.planet, SEFLG_SWIEPH | SEFLG_SPEED, position, error);
        if (result >= 0) {
            std::cout << std::setw(12) << planet.name 
                      << std::setw(12) << std::setprecision(6) << position[0]  // longitude
                      << std::setw(12) << std::setprecision(6) << position[1]  // latitude  
                      << std::setw(12) << std::setprecision(6) << position[2]  // distance
                      << std::setw(12) << std::setprecision(6) << position[3]  // speed
                      << std::endl;
        } else {
            std::cout << std::setw(12) << planet.name << " ERROR: " << error << std::endl;
        }
    }
    
    std::cout << std::endl << "=== SIDEREAL POSITIONS (Lahiri Ayanamsa) ===" << std::endl;
    
    // Set Lahiri ayanamsa
    swe_set_sid_mode(SE_SIDM_LAHIRI, 0, 0);
    double ayanamsa = swe_get_ayanamsa(jd);
    std::cout << "Ayanamsa: " << std::setprecision(6) << ayanamsa << "°" << std::endl << std::endl;
    
    std::cout << std::setw(12) << "Planet" << std::setw(12) << "Sidereal" << std::setw(12) << "Tropical" 
              << std::setw(12) << "Difference" << std::endl;
    std::cout << std::string(48, '-') << std::endl;
    
    for (const auto& planet : planets) {
        // Tropical
        int result1 = swe_calc(jd, planet.planet, SEFLG_SWIEPH, position, error);
        double tropical = position[0];
        
        // Sidereal  
        int result2 = swe_calc(jd, planet.planet, SEFLG_SWIEPH | SEFLG_SIDEREAL, position, error);
        double sidereal = position[0];
        
        if (result1 >= 0 && result2 >= 0) {
            double diff = tropical - sidereal;
            std::cout << std::setw(12) << planet.name
                      << std::setw(12) << std::setprecision(6) << sidereal
                      << std::setw(12) << std::setprecision(6) << tropical  
                      << std::setw(12) << std::setprecision(6) << diff
                      << std::endl;
        }
    }
    
    std::cout << std::endl << "=== COORDINATE VALIDATION ===" << std::endl;
    
    // Test specific known values for validation
    // Sun position on January 15, 2024 should be around 295° tropical (25° Capricorn)
    swe_calc(jd, SE_SUN, SEFLG_SWIEPH, position, error);
    double sun_lon = position[0];
    double expected_sun = 295.0;  // Approximate
    double sun_error = std::abs(sun_lon - expected_sun);
    
    std::cout << "Sun Validation:" << std::endl;
    std::cout << "  Calculated: " << sun_lon << "°" << std::endl;
    std::cout << "  Expected: ~" << expected_sun << "°" << std::endl;
    std::cout << "  Error: " << sun_error << "°" << std::endl;
    std::cout << "  Status: " << (sun_error < 5.0 ? "PASS" : "FAIL") << std::endl;
    
    return 0;
}
