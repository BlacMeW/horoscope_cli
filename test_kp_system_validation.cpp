#include <iostream>
#include <iomanip>
#include <algorithm>
#include "kp_system.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "astro_types.h"

int main() {
    std::cout << "=== KP SYSTEM VALIDATION TEST ===" << std::endl;
    
    // Initialize the ephemeris manager
    Astro::EphemerisManager ephMgr;
    if (!ephMgr.initialize("./data")) {
        std::cerr << "Failed to initialize ephemeris manager!" << std::endl;
        return 1;
    }
    
    // Create test birth data
    Astro::BirthData birthData;
    birthData.year = 2025;
    birthData.month = 1;
    birthData.day = 1;
    birthData.hour = 12;
    birthData.minute = 0;
    birthData.second = 0;
    birthData.timezone = 7.0; // Bangkok
    birthData.latitude = 13.7563;
    birthData.longitude = 100.5018;
    
    std::cout << "Test Date: 2025-01-01 12:00:00 Bangkok (UTC+7)" << std::endl;
    std::cout << "Location: Bangkok (" << birthData.latitude << "°N, " << birthData.longitude << "°E)" << std::endl;
    std::cout << "Julian Day: " << std::fixed << std::setprecision(6) << birthData.getJulianDay() << std::endl;
    std::cout << std::endl;
    
    // Test planetary positions calculation
    Astro::PlanetCalculator planetCalc(ephMgr);
    planetCalc.setZodiacMode(Astro::ZodiacMode::SIDEREAL);
    planetCalc.setAyanamsa(Astro::AyanamsaType::KRISHNAMURTI);
    
    std::vector<Astro::PlanetPosition> positions;
    if (!planetCalc.calculateAllPlanets(birthData, positions)) {
        std::cerr << "Failed to calculate planetary positions: " << planetCalc.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "=== PLANETARY POSITIONS (Sidereal + Krishnamurti Ayanamsa) ===" << std::endl;
    for (const auto& pos : positions) {
        std::cout << std::setw(12) << Astro::planetToString(pos.planet) << ": "
                  << std::fixed << std::setprecision(6) << pos.longitude << "° "
                  << pos.getFormattedPosition() << std::endl;
    }
    std::cout << std::endl;
    
    // Test KP System
    Astro::KPSystem kpSystem;
    if (!kpSystem.initialize()) {
        std::cerr << "Failed to initialize KP system: " << kpSystem.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "=== KP SYSTEM ANALYSIS ===" << std::endl;
    
    // Test Sun's KP position
    if (!positions.empty()) {
        auto sunPos = std::find_if(positions.begin(), positions.end(), 
                                  [](const Astro::PlanetPosition& p) { return p.planet == Astro::Planet::SUN; });
        
        if (sunPos != positions.end()) {
            std::cout << "Sun Longitude (Sidereal): " << std::fixed << std::setprecision(6) << sunPos->longitude << "°" << std::endl;
            
            Astro::KPPosition kpPos = kpSystem.calculateKPPosition(sunPos->longitude);
            std::cout << "Sun KP Analysis:" << std::endl;
            std::cout << "  Sign: " << Astro::zodiacSignToString(kpPos.sign) << " (Lord: " << Astro::planetToString(kpPos.signLord) << ")" << std::endl;
            std::cout << "  Nakshatra: " << kpPos.nakshatra.name << " (Lord: " << Astro::planetToString(kpPos.nakshatra.lord) << ")" << std::endl;
            std::cout << "  Sub Lord: " << Astro::planetToString(kpPos.subLord) << std::endl;
            std::cout << "  Sub-Sub Lord: " << Astro::planetToString(kpPos.subSubLord) << std::endl;
            std::cout << "  Sub-Sub-Sub Lord: " << Astro::planetToString(kpPos.subSubSubLord) << std::endl;
            std::cout << std::endl;
        }
    }
    
    // Test nakshatra calculations for known positions
    std::cout << "=== NAKSHATRA VALIDATION ===" << std::endl;
    
    // Test specific nakshatra boundaries
    double testLongitudes[] = {0.0, 13.33, 26.67, 40.0, 360.0};
    const char* expectedNakshatras[] = {"Ashwini", "Bharani", "Krittika", "Rohini", "Ashwini"};
    
    for (int i = 0; i < 5; i++) {
        Astro::KPPosition testPos = kpSystem.calculateKPPosition(testLongitudes[i]);
        std::cout << "Longitude " << std::fixed << std::setprecision(2) << testLongitudes[i] 
                  << "° -> Nakshatra: " << testPos.nakshatra.name;
        
        if (std::string(testPos.nakshatra.name) == std::string(expectedNakshatras[i])) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (Expected: " << expectedNakshatras[i] << ")" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Test KP table generation
    std::cout << "=== KP TABLE GENERATION TEST ===" << std::endl;
    std::string kpTable = kpSystem.generateKPTable(positions);
    std::cout << kpTable.substr(0, 500) << "..." << std::endl; // Show first 500 chars
    std::cout << std::endl;
    
    // Test sub-lord calculation accuracy
    std::cout << "=== SUB-LORD CALCULATION VALIDATION ===" << std::endl;
    
    // Known test case: Ashwini nakshatra (0-13.33°) sub-lords
    double ashwiniStart = 0.0;
    double ashwiniEnd = 13.333333;
    
    std::cout << "Ashwini Nakshatra (0° - 13°20') Sub-Lord Distribution:" << std::endl;
    
    for (double testLong = ashwiniStart; testLong < ashwiniEnd; testLong += 1.48) { // Test every ~1.5°
        Astro::KPPosition pos = kpSystem.calculateKPPosition(testLong);
        std::cout << "  " << std::fixed << std::setprecision(2) << testLong 
                  << "° -> Sub-Lord: " << Astro::planetToString(pos.subLord) << std::endl;
    }
    
    std::cout << std::endl << "KP System validation completed!" << std::endl;
    
    return 0;
}