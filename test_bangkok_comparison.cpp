#include <iostream>
#include <iomanip>
#include <string>
#include "include/hindu_calendar.h"

using namespace Astro;

int main() {
    std::cout << "=== Bangkok Sunrise/Sunset Comparison with Drikpanchang ===\n\n";

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

    // Bangkok, Thailand coordinates (from geoname-id=1609350)
    BirthData bangkokData;
    bangkokData.year = 2025;
    bangkokData.month = 7;
    bangkokData.day = 27;
    bangkokData.hour = 12;
    bangkokData.minute = 0;
    bangkokData.second = 0;
    bangkokData.latitude = 13.7563;   // Bangkok coordinates
    bangkokData.longitude = 100.5018; // Bangkok coordinates
    bangkokData.timezone = 7.0;       // UTC+7 (ICT - Indochina Time)

    std::cout << "🌍 Testing for Bangkok, Thailand (13.76°N, 100.50°E)\n";
    std::cout << "📅 Date: July 27, 2025, 12:00 PM ICT\n";
    std::cout << "🌐 Reference: https://www.drikpanchang.com/?geoname-id=1609350\n\n";

    // Calculate Panchanga
    PanchangaData panchanga = hinduCalendar.calculatePanchanga(bangkokData);

    // Display comparison
    std::cout << "📊 SUNRISE/SUNSET COMPARISON:\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "Source               | Sunrise  | Sunset   | Notes\n";
    std::cout << "────────────────────────────────────────────────────────\n";
    std::cout << "Drikpanchang.com     | 06:01 AM | 06:48 PM | Reference\n";
    std::cout << "Our Calculation      | " << panchanga.getTimeString(panchanga.sunriseTime)
              << " | " << panchanga.getTimeString(panchanga.sunsetTime) << " | Swiss Ephemeris\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Calculate differences
    double drikSunriseHours = 6.0 + 1.0/60.0;  // 06:01 = 6.01667 hours
    double drikSunsetHours = 18.0 + 48.0/60.0; // 18:48 = 18.8 hours

    double sunriseDiff = panchanga.sunriseTime - drikSunriseHours;
    double sunsetDiff = panchanga.sunsetTime - drikSunsetHours;

    int sunriseMinDiff = static_cast<int>(sunriseDiff * 60);
    int sunsetMinDiff = static_cast<int>(sunsetDiff * 60);

    std::cout << "📏 DIFFERENCES:\n";
    std::cout << "   Sunrise difference: " << sunriseMinDiff << " minutes ";
    std::cout << (sunriseMinDiff > 0 ? "(Our calc is later)" : "(Our calc is earlier)") << "\n";
    std::cout << "   Sunset difference:  " << sunsetMinDiff << " minutes ";
    std::cout << (sunsetMinDiff > 0 ? "(Our calc is later)" : "(Our calc is earlier)") << "\n\n";

    // Show detailed calculation info
    std::cout << "🔍 DETAILED CALCULATION INFO:\n";
    std::cout << "   Day Length: " << std::fixed << std::setprecision(2) << panchanga.dayLength << " hours\n";
    std::cout << "   Night Length: " << panchanga.nightLength << " hours\n";
    std::cout << "   Julian Day: " << std::setprecision(6) << panchanga.julianDay << "\n";
    std::cout << "   Ayanamsa Value: " << std::setprecision(4) << panchanga.ayanamsaValue << "°\n\n";

    std::cout << "✅ Test completed!\n";

    return 0;
}
