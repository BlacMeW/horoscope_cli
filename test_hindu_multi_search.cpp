/*
 * Test program for Hindu Calendar multi-criteria search with AND/OR logic
 *
 * This program demonstrates the enhanced Hindu calendar search functionality
 * with logical operators (AND/OR) for combining multiple search criteria.
 *
 * Example usage:
 *   g++ -std=c++17 -I./include -I./third_party/swisseph test_hindu_multi_search.cpp \
 *       src/hindu_calendar.cpp src/astro_types.cpp third_party/swisseph/sweph.c \
 *       third_party/swisseph/swephlib.c -o test_hindu_multi_search -lm
 *   ./test_hindu_multi_search
 */

#include "hindu_calendar.h"
#include <iostream>
#include <iomanip>

using namespace Astro;

void printSearchResults(const std::vector<HinduCalendar::SearchResult>& results, const std::string& description) {
    std::cout << "\n═══════════════════════════════════════════════════════════════════\n";
    std::cout << "🕉️  " << description << " 🕉️\n";
    std::cout << "═══════════════════════════════════════════════════════════════════\n";

    if (results.empty()) {
        std::cout << "No results found.\n";
        return;
    }

    std::cout << "Found " << results.size() << " matching days:\n\n";

    for (const auto& result : results) {
        std::cout << "📅 Date: " << result.gregorianDate
                  << " (Score: ⭐ " << std::fixed << std::setprecision(1) << result.matchScore << ")\n";

        std::cout << "   🕉️  Year: " << result.panchangaData.year
                  << ", Month: " << static_cast<int>(result.panchangaData.month) << "\n";

        std::cout << "   🌙 Tithi: " << static_cast<int>(result.panchangaData.tithi)
                  << ", Vara: " << static_cast<int>(result.panchangaData.vara) << "\n";

        std::cout << "   ⭐ Nakshatra: " << static_cast<int>(result.panchangaData.nakshatra)
                  << ", Yoga: " << static_cast<int>(result.panchangaData.yoga) << "\n";

        std::cout << "   ✨ " << result.matchDescription << "\n";

        if (result.panchangaData.isEkadashi) std::cout << "   🙏 Ekadashi Day\n";
        if (result.panchangaData.isPurnima) std::cout << "   🌕 Purnima (Full Moon)\n";
        if (result.panchangaData.isAmavasya) std::cout << "   🌑 Amavasya (New Moon)\n";
        if (result.panchangaData.isSankranti) std::cout << "   ☀️  Sankranti Day\n";

        std::cout << "\n";
    }

    std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
}

int main() {
    std::cout << "🕉️ ═══════════════════════════════════════════════════════════════════ 🕉️\n";
    std::cout << "    HINDU CALENDAR MULTI-CRITERIA SEARCH WITH AND/OR LOGIC TEST\n";
    std::cout << "🕉️ ═══════════════════════════════════════════════════════════════════ 🕉️\n\n";

    // Initialize Hindu calendar
    HinduCalendar hinduCalendar;
    if (!hinduCalendar.initialize()) {
        std::cerr << "Error: Failed to initialize Hindu Calendar system: "
                  << hinduCalendar.getLastError() << std::endl;
        return 1;
    }

    double latitude = 28.6139;   // New Delhi
    double longitude = 77.2090;
    std::string startDate = "2024-01-01";
    std::string endDate = "2024-01-31";

    std::cout << "🔍 Search Period: " << startDate << " to " << endDate << "\n";
    std::cout << "📍 Location: New Delhi (" << latitude << "°N, " << longitude << "°E)\n\n";

    // Test 1: Search for Purnima OR Ekadashi using OR logic
    try {
        std::cout << "🧪 TEST 1: Finding Purnima OR Ekadashi days using OR logic\n";
        auto results1 = hinduCalendar.searchPurnimaOrAmavasya(startDate, endDate, latitude, longitude);
        printSearchResults(results1, "PURNIMA OR AMAVASYA DAYS (OR Logic)");

        // Manual criteria with OR logic
        HinduCalendar::SearchCriteria criteria1;
        criteria1.searchPurnima = true;
        criteria1.searchEkadashi = true;
        criteria1.logicMode = HinduCalendar::LogicMode::OR;
        criteria1.searchStartDate = startDate;
        criteria1.searchEndDate = endDate;

        auto results1_manual = hinduCalendar.searchHinduCalendar(criteria1, latitude, longitude);
        printSearchResults(results1_manual, "PURNIMA OR EKADASHI DAYS (Manual OR Logic)");

    } catch (const std::exception& e) {
        std::cerr << "Error in Test 1: " << e.what() << std::endl;
    }

    // Test 2: Search for specific Nakshatra AND Yoga using AND logic
    try {
        std::cout << "🧪 TEST 2: Finding specific Nakshatra (1=Ashwini) AND Yoga (5=Shobhana) using AND logic\n";

        HinduCalendar::SearchCriteria criteria2;
        criteria2.exactNakshatra = 1; // Ashwini
        criteria2.exactYoga = 5;      // Shobhana
        criteria2.logicMode = HinduCalendar::LogicMode::AND;
        criteria2.searchStartDate = startDate;
        criteria2.searchEndDate = "2024-03-31"; // Extended search period

        auto results2 = hinduCalendar.searchHinduCalendar(criteria2, latitude, longitude);
        printSearchResults(results2, "ASHWINI NAKSHATRA AND SHOBHANA YOGA (AND Logic)");

    } catch (const std::exception& e) {
        std::cerr << "Error in Test 2: " << e.what() << std::endl;
    }

    // Test 3: Complex multi-criteria search with OR logic
    try {
        std::cout << "🧪 TEST 3: Complex search - (Ekadashi OR Purnima OR Amavasya) using OR logic\n";

        HinduCalendar::SearchCriteria criteria3;
        criteria3.searchEkadashi = true;
        criteria3.searchPurnima = true;
        criteria3.searchAmavasya = true;
        criteria3.logicMode = HinduCalendar::LogicMode::OR;
        criteria3.searchStartDate = startDate;
        criteria3.searchEndDate = endDate;

        auto results3 = hinduCalendar.searchHinduCalendar(criteria3, latitude, longitude);
        printSearchResults(results3, "EKADASHI OR PURNIMA OR AMAVASYA (OR Logic)");

    } catch (const std::exception& e) {
        std::cerr << "Error in Test 3: " << e.what() << std::endl;
    }

    // Test 4: Tithi range search with specific weekday using AND logic
    try {
        std::cout << "🧪 TEST 4: Finding Tithi 13-15 AND Sunday (0) using AND logic\n";

        HinduCalendar::SearchCriteria criteria4;
        criteria4.tithiRangeStart = 13;
        criteria4.tithiRangeEnd = 15;
        criteria4.exactWeekday = 0; // Sunday
        criteria4.logicMode = HinduCalendar::LogicMode::AND;
        criteria4.searchStartDate = startDate;
        criteria4.searchEndDate = "2024-02-29";

        auto results4 = hinduCalendar.searchHinduCalendar(criteria4, latitude, longitude);
        printSearchResults(results4, "TITHI 13-15 AND SUNDAY (AND Logic)");

    } catch (const std::exception& e) {
        std::cerr << "Error in Test 4: " << e.what() << std::endl;
    }

    // Test 5: Using helper methods
    try {
        std::cout << "🧪 TEST 5: Using helper method - searchEkadashiAndPurnima\n";

        auto results5 = hinduCalendar.searchEkadashiAndPurnima(startDate, endDate, latitude, longitude);
        printSearchResults(results5, "EKADASHI AND PURNIMA (Helper Method)");

    } catch (const std::exception& e) {
        std::cerr << "Error in Test 5: " << e.what() << std::endl;
    }

    std::cout << "🎉 Hindu Calendar multi-criteria search testing completed!\n";
    std::cout << "✨ The Hindu calendar now supports flexible AND/OR logic for combining\n";
    std::cout << "   multiple search criteria including Tithis, Nakshatras, Yogas, and special days.\n\n";

    return 0;
}
