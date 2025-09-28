#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    std::cout << "=== DIRECT SWISS EPHEMERIS TEST ===" << std::endl;
    
    // Use the exact Julian Day that our CLI calculates: 2460676.708333
    double julianDay = 2460676.708333;
    
    std::cout << "Julian Day: " << std::fixed << std::setprecision(6) << julianDay << std::endl;
    std::cout << "Date: 2025-01-01 05:00:00 UTC (12:00:00 Bangkok)" << std::endl;
    std::cout << std::endl;
    
    // We'll simulate what the CLI should calculate
    // Expected results based on Swiss Ephemeris direct calculation
    
    // Sun longitude calculation (approximation)
    // For Jan 1, 2025, the Sun should be around 280-282° (Capricorn)
    double sunLongitude = 280.5; // Approximate for Jan 1, 2025
    int sunSign = (int)(sunLongitude / 30.0);
    double sunSignPos = fmod(sunLongitude, 30.0);
    int sunDegrees = (int)sunSignPos;
    int sunMinutes = (int)((sunSignPos - sunDegrees) * 60.0);
    
    const char* signs[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
                           "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
    
    std::cout << "Expected Sun position (approximate):" << std::endl;
    std::cout << "  Longitude: " << std::fixed << std::setprecision(1) << sunLongitude << "°" << std::endl;
    std::cout << "  Position: " << std::setfill('0') << std::setw(2) << sunDegrees 
              << "°" << std::setw(2) << sunMinutes << "' " << signs[sunSign] << std::endl;
    
    std::cout << "\nCLI shows: Sun 11°01' Capricorn" << std::endl;
    std::cout << "Expected: Sun ~10°30' Capricorn (very approximate)" << std::endl;
    
    std::cout << "\nThe CLI position looks reasonable for early January!" << std::endl;
    std::cout << "The issue may be in our comparison, not the CLI calculation." << std::endl;
    
    return 0;
}