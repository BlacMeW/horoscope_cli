#include <iostream>
#include <iomanip>
#include <string>
#include "include/hindu_calendar.h"

using namespace Astro;

int main() {
    std::cout << "=== Bangkok Sunrise/Sunset Comparison (Current Date) ===\n\n";

    // Initialize Hindu Calendar
    HinduCalendar hinduCalendar;
    if (!hinduCalendar.initialize()) {
        std::cerr << "Failed to initialize Hindu Calendar: " << hinduCalendar.getLastError() << std::endl;
        return 1;
    }

    std::cout << "✅ Hindu Calendar initialized successfully\n";
    std::cout << "📊 Configuration:\n";
    std::cout << "   - Ayanamsa: " << hinduCalendar.getAyanamsaName() << "\n";
    std::cout << "   - Method: " << hinduCalendar.getCalculationMethodName() << "\n\n";

    // Bangkok, Thailand coordinates - using current date (July 27, 2024)
    BirthData bangkokData;
    bangkokData.year = 2024;      // Current year within Swiss Ephemeris range
    bangkokData.month = 7;
    bangkokData.day = 27;
    bangkokData.hour = 12;
    bangkokData.minute = 0;
    bangkokData.second = 0;
    bangkokData.latitude = 13.7563;   // Bangkok coordinates
    bangkokData.longitude = 100.5018; // Bangkok coordinates
    bangkokData.timezone = 7.0;       // UTC+7 (ICT - Indochina Time)

    std::cout << "🌍 Testing for Bangkok, Thailand (13.76°N, 100.50°E)\n";
    std::cout << "📅 Date: July 27, 2024, 12:00 PM ICT (within Swiss Ephemeris range)\n";
    std::cout << "🌐 Reference: July 2024 Bangkok sunrise/sunset\n\n";

    // Calculate Panchanga
    PanchangaData panchanga = hinduCalendar.calculatePanchanga(bangkokData);

    // Display calculation results
    std::cout << "📊 ACTUAL CALCULATION RESULTS:\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "Parameter            | Value                 | Notes\n";
    std::cout << "────────────────────────────────────────────────────────\n";
    std::cout << "Sunrise Time         | " << panchanga.getTimeString(panchanga.sunriseTime) << "          | Local ICT time\n";
    std::cout << "Sunset Time          | " << panchanga.getTimeString(panchanga.sunsetTime) << "          | Local ICT time\n";
    std::cout << "Moonrise Time        | " << panchanga.getTimeString(panchanga.moonriseTime) << "          | Local ICT time\n";
    std::cout << "Moonset Time         | " << panchanga.getTimeString(panchanga.moonsetTime) << "          | Local ICT time\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Show detailed calculation info
    std::cout << "🔍 DETAILED CALCULATION INFO:\n";
    std::cout << "   Day Length: " << std::fixed << std::setprecision(2) << panchanga.dayLength << " hours\n";
    std::cout << "   Night Length: " << panchanga.nightLength << " hours\n";
    std::cout << "   Julian Day: " << std::setprecision(6) << panchanga.julianDay << "\n";
    std::cout << "   Ayanamsa Value: " << std::setprecision(4) << panchanga.ayanamsaValue << "°\n";
    std::cout << "   Sun Longitude: " << panchanga.sunLongitude << "°\n";
    std::cout << "   Moon Longitude: " << panchanga.moonLongitude << "°\n\n";

    // Test the timezone calculation logic we identified
    std::cout << "🔧 TIMEZONE ANALYSIS:\n";
    std::cout << "   Bangkok Longitude: " << bangkokData.longitude << "°E\n";
    std::cout << "   Solar Time Offset: " << (bangkokData.longitude / 15.0) << " hours from GMT\n";
    std::cout << "   Civil Time (ICT): UTC+7.0 hours\n";
    std::cout << "   Difference: " << (7.0 - bangkokData.longitude / 15.0) * 60 << " minutes\n";
    std::cout << "   (Civil time is ahead of solar time by this amount)\n\n";

    // Expected vs actual sunrise for July 27, 2024 in Bangkok
    double expectedSunrise = 6.0 + 2.0/60.0;  // Approximately 06:02 for July in Bangkok
    double expectedSunset = 18.0 + 47.0/60.0; // Approximately 18:47 for July in Bangkok

    double sunriseDiff = panchanga.sunriseTime - expectedSunrise;
    double sunsetDiff = panchanga.sunsetTime - expectedSunset;

    int sunriseMinDiff = static_cast<int>(sunriseDiff * 60);
    int sunsetMinDiff = static_cast<int>(sunsetDiff * 60);

    std::cout << "📏 ESTIMATED DIFFERENCES (vs typical July Bangkok times):\n";
    std::cout << "   Expected Sunrise: ~06:02, Calculated: " << panchanga.getTimeString(panchanga.sunriseTime)
              << " (" << sunriseMinDiff << " min diff)\n";
    std::cout << "   Expected Sunset: ~18:47, Calculated: " << panchanga.getTimeString(panchanga.sunsetTime)
              << " (" << sunsetMinDiff << " min diff)\n\n";

    std::cout << "✅ Current date test completed!\n";

    return 0;
}