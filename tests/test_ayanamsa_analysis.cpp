// Test ayanamsa calculation accuracy
// Note: This is a conceptual test - in practice we'd need Swiss Ephemeris linked
#include <iostream>
#include <iomanip>
#include <cmath>

// Test data: Known planetary positions for validation
void testAyanamsaAccuracy() {
    std::cout << "=== Ayanamsa Calculation Analysis ===" << std::endl;
    
    // Example: Sun's position on January 1, 2000, 12:00 UT
    // Known tropical longitude: approximately 280.46° (10°46' Capricorn)
    // With Lahiri ayanamsa (≈ 23.85° in year 2000):
    // Expected sidereal longitude: 280.46° - 23.85° = 256.61° (16°37' Sagittarius)
    
    double julianDay = 2451545.0; // January 1, 2000, 12:00 UT
    double tropicalLongitude = 280.46; // Sun's tropical position
    double lahiriAyanamsa2000 = 23.85; // Approximate Lahiri ayanamsa for year 2000
    
    std::cout << "Test Case: Sun's position on January 1, 2000" << std::endl;
    std::cout << "Julian Day: " << std::fixed << std::setprecision(1) << julianDay << std::endl;
    std::cout << "Expected Tropical Longitude: " << tropicalLongitude << "°" << std::endl;
    std::cout << "Expected Lahiri Ayanamsa: " << lahiriAyanamsa2000 << "°" << std::endl;
    
    double expectedSiderealLongitude = tropicalLongitude - lahiriAyanamsa2000;
    if (expectedSiderealLongitude < 0) expectedSiderealLongitude += 360.0;
    
    std::cout << "Expected Sidereal Longitude: " << expectedSiderealLongitude << "°" << std::endl;
    
    // Convert to sign notation
    int tropicalSign = (int)(tropicalLongitude / 30.0);
    int siderealSign = (int)(expectedSiderealLongitude / 30.0);
    double tropicalDegInSign = fmod(tropicalLongitude, 30.0);
    double siderealDegInSign = fmod(expectedSiderealLongitude, 30.0);
    
    const char* signs[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
                          "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
    
    std::cout << "Tropical: " << std::setprecision(1) << tropicalDegInSign 
              << "° " << signs[tropicalSign] << std::endl;
    std::cout << "Sidereal: " << siderealDegInSign 
              << "° " << signs[siderealSign] << std::endl;
    
    std::cout << std::endl;
    
    // Analysis of potential issues in ayanamsa calculations
    std::cout << "=== Potential Ayanamsa Issues to Check ===" << std::endl;
    std::cout << "1. Epoch accuracy: Ayanamsa values change over time" << std::endl;
    std::cout << "2. Calculation order: Swiss Ephemeris requires specific sequence" << std::endl;
    std::cout << "3. Mode persistence: swe_set_sid_mode() affects subsequent calls" << std::endl;
    std::cout << "4. Flag consistency: SEFLG_SIDEREAL must be used with set mode" << std::endl;
    std::cout << std::endl;
}

void analyzeAyanamsaImplementation() {
    std::cout << "=== Ayanamsa Implementation Analysis ===" << std::endl;
    
    // Known ayanamsa values for different systems (approximate for year 2000)
    struct AyanamsaData {
        const char* name;
        double value2000;
    } ayanamsas[] = {
        {"Fagan-Bradley", 24.74},
        {"Lahiri", 23.85},
        {"Raman", 21.45},
        {"Krishnamurti", 23.29}
    };
    
    std::cout << "Common Ayanamsa Values (circa year 2000):" << std::endl;
    for (const auto& ay : ayanamsas) {
        std::cout << "  " << std::setw(15) << std::left << ay.name 
                  << ": " << std::fixed << std::setprecision(2) << ay.value2000 << "°" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Key Implementation Points:" << std::endl;
    std::cout << "• Swiss Ephemeris handles ayanamsa calculations internally" << std::endl;
    std::cout << "• Our mapping to SE_SIDM_* constants must be correct" << std::endl;
    std::cout << "• swe_set_sid_mode() must be called before swe_calc()" << std::endl;
    std::cout << "• SEFLG_SIDEREAL flag must be included in calculations" << std::endl;
    std::cout << "• Mode setting is global and affects subsequent calculations" << std::endl;
    std::cout << std::endl;
}

void identifyPotentialIssues() {
    std::cout << "=== Potential Issues in Current Implementation ===" << std::endl;
    
    std::cout << "1. **Threading Safety**: swe_set_sid_mode() is global" << std::endl;
    std::cout << "   → Multiple concurrent calculations could interfere" << std::endl;
    std::cout << "   → Consider using thread-local storage or synchronization" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. **Mode Persistence**: Sidereal mode persists across calls" << std::endl;
    std::cout << "   → Tropical calculations after sidereal might be affected" << std::endl;
    std::cout << "   → Should reset to tropical mode after sidereal calculations" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. **Error Handling**: Swiss Ephemeris error states" << std::endl;
    std::cout << "   → Invalid ayanamsa types might not be caught properly" << std::endl;
    std::cout << "   → Should validate ayanamsa support for given date range" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4. **Consistency Check**: All calculation paths should use same mode" << std::endl;
    std::cout << "   → Planet positions, house cusps, aspects must all use same zodiac" << std::endl;
    std::cout << "   → Mixing tropical/sidereal in single chart would be incorrect" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "Ayanamsa Calculation Analysis" << std::endl;
    std::cout << "=============================" << std::endl << std::endl;
    
    testAyanamsaAccuracy();
    analyzeAyanamsaImplementation();
    identifyPotentialIssues();
    
    return 0;
}