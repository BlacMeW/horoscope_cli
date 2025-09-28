#include <iostream>
#include <iomanip>

// Test the zodiac mode persistence issue
// This test simulates the problem and proposed solution

struct TestPlanetPosition {
    double longitude;
    bool isSidereal;
};

// Simulate Swiss Ephemeris global state issue
static bool globalSiderealMode = false;
static int globalAyanamsa = 0;

// Simulate swe_set_sid_mode()
void simulate_swe_set_sid_mode(int ayanamsa, double t0, double ayan_t0) {
    globalSiderealMode = true;
    globalAyanamsa = ayanamsa;
    std::cout << "  [Swiss Ephemeris] Set sidereal mode: " << ayanamsa << std::endl;
}

// Simulate swe_calc() with the persistence bug
TestPlanetPosition simulate_swe_calc_buggy(double jd, int planet, int flags) {
    TestPlanetPosition pos;
    
    // Simulate calculation - in reality this would be complex astronomical math
    double baseLongitude = 280.46; // Example: Sun position
    
    if (globalSiderealMode) {
        // Bug: Swiss Ephemeris stays in sidereal mode even for tropical calculations!
        pos.longitude = baseLongitude - 23.85; // Apply ayanamsa correction
        pos.isSidereal = true;
        std::cout << "  [Swiss Ephemeris] Calculated in SIDEREAL mode (ayanamsa=" << globalAyanamsa << ")" << std::endl;
    } else {
        pos.longitude = baseLongitude; // Tropical calculation
        pos.isSidereal = false;
        std::cout << "  [Swiss Ephemeris] Calculated in TROPICAL mode" << std::endl;
    }
    
    return pos;
}

// Simulate swe_calc() with our flag-based fix
TestPlanetPosition simulate_swe_calc_fixed(double jd, int planet, int flags) {
    TestPlanetPosition pos;
    
    double baseLongitude = 280.46; // Example: Sun position
    
    // Fixed: Respect the SEFLG_SIDEREAL flag regardless of global mode
    const int SEFLG_SIDEREAL = 0x40000;
    
    if (flags & SEFLG_SIDEREAL) {
        pos.longitude = baseLongitude - 23.85; // Apply ayanamsa correction
        pos.isSidereal = true;
        std::cout << "  [Swiss Ephemeris] Calculated in SIDEREAL mode (flag-based)" << std::endl;
    } else {
        pos.longitude = baseLongitude; // Tropical calculation
        pos.isSidereal = false;
        std::cout << "  [Swiss Ephemeris] Calculated in TROPICAL mode (flag-based)" << std::endl;
    }
    
    return pos;
}

void testPersistenceBug() {
    std::cout << "=== Testing Swiss Ephemeris Mode Persistence Bug ===" << std::endl;
    std::cout << std::endl;
    
    // Reset state
    globalSiderealMode = false;
    globalAyanamsa = 0;
    
    std::cout << "Step 1: Calculate tropical position" << std::endl;
    auto pos1 = simulate_swe_calc_buggy(2451545.0, 0, 0); // No sidereal flag
    std::cout << "Result: " << std::fixed << std::setprecision(2) 
              << pos1.longitude << "° (" << (pos1.isSidereal ? "SIDEREAL" : "TROPICAL") << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Step 2: Set sidereal mode for calculation" << std::endl;
    simulate_swe_set_sid_mode(1, 0, 0); // Set Lahiri ayanamsa
    
    std::cout << "Step 3: Calculate sidereal position" << std::endl;
    auto pos2 = simulate_swe_calc_buggy(2451545.0, 0, 0x40000); // With sidereal flag
    std::cout << "Result: " << std::fixed << std::setprecision(2) 
              << pos2.longitude << "° (" << (pos2.isSidereal ? "SIDEREAL" : "TROPICAL") << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Step 4: Try to calculate tropical position again (BUG!)" << std::endl;
    auto pos3 = simulate_swe_calc_buggy(2451545.0, 0, 0); // No sidereal flag
    std::cout << "Result: " << std::fixed << std::setprecision(2) 
              << pos3.longitude << "° (" << (pos3.isSidereal ? "SIDEREAL" : "TROPICAL") << ")" << std::endl;
    
    if (pos3.isSidereal) {
        std::cout << "*** BUG DETECTED: Tropical calculation returned sidereal result! ***" << std::endl;
    }
    std::cout << std::endl;
}

void testFixedImplementation() {
    std::cout << "=== Testing FIXED Implementation ===" << std::endl;
    std::cout << std::endl;
    
    // Reset state
    globalSiderealMode = false;
    
    std::cout << "Step 1: Calculate tropical position (fixed)" << std::endl;
    auto pos1 = simulate_swe_calc_fixed(2451545.0, 0, 0); // No sidereal flag
    std::cout << "Result: " << std::fixed << std::setprecision(2) 
              << pos1.longitude << "° (" << (pos1.isSidereal ? "SIDEREAL" : "TROPICAL") << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Step 2: Set sidereal mode and calculate" << std::endl;
    simulate_swe_set_sid_mode(1, 0, 0);
    auto pos2 = simulate_swe_calc_fixed(2451545.0, 0, 0x40000); // With sidereal flag
    std::cout << "Result: " << std::fixed << std::setprecision(2) 
              << pos2.longitude << "° (" << (pos2.isSidereal ? "SIDEREAL" : "TROPICAL") << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Step 3: Calculate tropical position again (FIXED!)" << std::endl;
    auto pos3 = simulate_swe_calc_fixed(2451545.0, 0, 0); // No sidereal flag
    std::cout << "Result: " << std::fixed << std::setprecision(2) 
              << pos3.longitude << "° (" << (pos3.isSidereal ? "SIDEREAL" : "TROPICAL") << ")" << std::endl;
    
    if (!pos3.isSidereal && pos1.longitude == pos3.longitude) {
        std::cout << "✓ FIX VERIFIED: Tropical calculations consistent regardless of previous sidereal calls!" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Swiss Ephemeris Zodiac Mode Persistence Analysis" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    
    testPersistenceBug();
    testFixedImplementation();
    
    std::cout << "=== Analysis Summary ===" << std::endl;
    std::cout << "• Problem: swe_set_sid_mode() creates global persistent state" << std::endl;
    std::cout << "• Impact: Tropical calculations after sidereal become incorrect" << std::endl;
    std::cout << "• Solution: Rely on SEFLG_SIDEREAL flag in swe_calc() calls" << std::endl;
    std::cout << "• Result: Each calculation uses correct zodiac mode independently" << std::endl;
    
    return 0;
}