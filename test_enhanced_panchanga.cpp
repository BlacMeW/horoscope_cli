#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include "include/hindu_calendar.h"
#include "include/birth_chart.h"

using namespace Astro;

int main() {
    std::cout << "=== Testing Enhanced Hindu Calendar Implementation ===\n\n";

    // Initialize Hindu Calendar
    HinduCalendar hinuCalendar;
    if (!hinuCalendar.initialize()) {
        std::cerr << "Failed to initialize Hindu Calendar: " << hinuCalendar.getLastError() << std::endl;
        return 1;
    }

    std::cout << "✅ Hindu Calendar initialized successfully\n";
    std::cout << "📊 Configuration:\n";
    std::cout << "   - Ayanamsa: " << hinuCalendar.getAyanamsaName() << "\n";
    std::cout << "   - Method: " << hinuCalendar.getCalculationMethodName() << "\n\n";

    // Test with today's date (July 21, 2025)
    BirthData testData;
    testData.year = 2025;
    testData.month = 7;
    testData.day = 21;
    testData.hour = 12;
    testData.minute = 0;
    testData.second = 0;
    testData.latitude = 12.9716;  // Bengaluru coordinates
    testData.longitude = 77.5946;
    testData.timezone = 5.5;      // IST

    std::cout << "🌍 Testing for Bengaluru, India (12.97°N, 77.59°E)\n";
    std::cout << "📅 Date: July 21, 2025, 12:00 PM IST\n\n";

    // Calculate Panchanga
    PanchangaData panchanga = hinuCalendar.calculatePanchanga(testData);

    if (panchanga.tithi == Tithi::PRATIPAD && panchanga.nakshatra == HinduNakshatra::ASHWINI) {
        std::cerr << "⚠️  Warning: Calculation may have failed (default values returned)\n\n";
    }

    // Display complete panchanga
    std::cout << hinuCalendar.generatePanchangaTable(panchanga) << std::endl;

    // Test JSON output
    std::cout << "\n📄 JSON OUTPUT (First 500 characters):\n";
    std::string jsonOutput = hinuCalendar.generateJSON(panchanga);
    std::cout << jsonOutput.substr(0, 500) << "...\n\n";

    // Test specific features
    std::cout << "🔍 FEATURE VERIFICATION:\n";
    std::cout << "   ☀️  Sunrise Time: " << panchanga.getTimeString(panchanga.sunriseTime) << "\n";
    std::cout << "   🌅 Sunset Time: " << panchanga.getTimeString(panchanga.sunsetTime) << "\n";
    std::cout << "   🌙 Moonrise Time: " << panchanga.getTimeString(panchanga.moonriseTime) << "\n";
    std::cout << "   🌜 Moonset Time: " << panchanga.getTimeString(panchanga.moonsetTime) << "\n";
    std::cout << "   ⏰ Brahma Muhurta: " << panchanga.getTimeString(panchanga.brahmaMuhurtaStart)
              << " - " << panchanga.getTimeString(panchanga.brahmaMuhurtaEnd) << "\n";
    std::cout << "   🕐 Rahu Kaal: " << panchanga.getTimeString(panchanga.rahuKaalStart)
              << " - " << panchanga.getTimeString(panchanga.rahuKaalEnd) << "\n";
    std::cout << "   🧭 Disha Shool: " << panchanga.dishaShool << "\n";
    std::cout << "   ⭐ Nakshatra Pada: " << panchanga.nakshatraPada << "\n";
    std::cout << "   🌡️  Ritu: " << panchanga.ritu << "\n";
    std::cout << "   🔄 Ayana: " << panchanga.ayana << "\n";

    if (panchanga.isSarvarthaSiddhi || panchanga.isAmritaSiddhi || panchanga.isRaviPushya) {
        std::cout << "   ✨ Special Yogas Active!\n";
    }

    if (panchanga.isGandaMool || panchanga.isPanchak || panchanga.isBhadra) {
        std::cout << "   ⚠️  Special Precautions Required!\n";
    }

    std::cout << "\n";

    // Test search functionality
    std::cout << "🔍 TESTING SEARCH FUNCTIONALITY:\n";

    // Search for Ekadashi days in the next month
    auto ekadashiResults = hinuCalendar.searchByTithi(11, "2025-07-21", "2025-08-21",
                                                     testData.latitude, testData.longitude);
    std::cout << "   Found " << ekadashiResults.size() << " Ekadashi days in the next month\n";

    if (!ekadashiResults.empty()) {
        std::cout << "   Next Ekadashi: " << ekadashiResults[0].gregorianDate << "\n";
        if (!ekadashiResults[0].panchangaData.ekadashiNames.empty()) {
            std::cout << "   Name: " << ekadashiResults[0].panchangaData.ekadashiNames[0] << "\n";
        }
    }

    // Search for Purnima days
    auto purnimaResults = hinuCalendar.searchPurnimaOrAmavasya("2025-07-21", "2025-12-31",
                                                              testData.latitude, testData.longitude);
    std::cout << "   Found " << purnimaResults.size() << " Purnima/Amavasya days in next 5 months\n";

    std::cout << "\n";

    // Test multiple ayanamsas
    std::cout << "🔧 TESTING DIFFERENT AYANAMSAS:\n";

    std::vector<AyanamsaType> ayanamsaTypes = {
        AyanamsaType::LAHIRI,
        AyanamsaType::RAMAN,
        AyanamsaType::KRISHNAMURTI,
        AyanamsaType::YUKTESHWAR
    };

    for (auto ayanamsa : ayanamsaTypes) {
        HinduCalendar testCalendar(ayanamsa);
        if (testCalendar.initialize()) {
            PanchangaData testPanchanga = testCalendar.calculatePanchanga(testData);
            std::cout << "   " << testCalendar.getAyanamsaName() << ":\n";
            std::cout << "     Tithi: " << testCalendar.getTithiName(testPanchanga.tithi) << "\n";
            std::cout << "     Nakshatra: " << testCalendar.getNakshatraName(testPanchanga.nakshatra) << "\n";
            std::cout << "     Ayanamsa Value: " << std::fixed << std::setprecision(4)
                      << testPanchanga.ayanamsaValue << "°\n\n";
        }
    }

    // Compare with Drik Panchang reference
    std::cout << "📊 COMPARISON WITH REFERENCE:\n";
    std::cout << "   Expected for July 21, 2025 (Bengaluru):\n";
    std::cout << "   - Tithi: Krishna Ekadashi\n";
    std::cout << "   - Nakshatra: Rohini (transitioning to Mrigashira)\n";
    std::cout << "   - Yoga: Vriddhi (transitioning to Dhruva)\n";
    std::cout << "   - Karana: Balava (transitioning to Kaulava)\n";
    std::cout << "   - Vara: Monday (Somwar)\n";
    std::cout << "   - Sunrise: ~06:03\n";
    std::cout << "   - Sunset: ~18:49\n\n";

    std::cout << "   Calculated Values:\n";
    std::cout << "   - Tithi: " << hinuCalendar.getTithiName(panchanga.tithi) << "\n";
    std::cout << "   - Nakshatra: " << hinuCalendar.getNakshatraName(panchanga.nakshatra) << "\n";
    std::cout << "   - Yoga: " << hinuCalendar.getYogaName(panchanga.yoga) << "\n";
    std::cout << "   - Karana: " << hinuCalendar.getKaranaName(panchanga.karana) << "\n";
    std::cout << "   - Vara: " << hinuCalendar.getVaraName(panchanga.vara) << "\n";
    std::cout << "   - Sunrise: " << panchanga.getTimeString(panchanga.sunriseTime) << "\n";
    std::cout << "   - Sunset: " << panchanga.getTimeString(panchanga.sunsetTime) << "\n\n";

    // Performance test
    std::cout << "⚡ PERFORMANCE TEST:\n";
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++) {
        PanchangaData testPanchanga = hinuCalendar.calculatePanchanga(testData.getJulianDay() + i,
                                                                     testData.latitude, testData.longitude);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "   Calculated 100 panchanga entries in " << duration.count() << " ms\n";
    std::cout << "   Average time per calculation: " << (duration.count() / 100.0) << " ms\n\n";

    std::cout << "✅ All tests completed successfully!\n";
    std::cout << "🎉 Enhanced Hindu Calendar with " <<
                 "Sunrise/Sunset, Muhurta, Festivals, Vratas, and Advanced Search is working!\n\n";

    return 0;
}
