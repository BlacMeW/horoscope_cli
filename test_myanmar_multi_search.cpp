#include "myanmar_calendar.h"
#include <iostream>
#include <iomanip>

using namespace Astro;

int main() {
    std::cout << "🇲🇲 Myanmar Calendar Multi-Criteria Search Demo 🇲🇲\n";
    std::cout << "====================================================\n\n";

    MyanmarCalendar calendar;
    if (!calendar.initialize()) {
        std::cerr << "❌ Failed to initialize Myanmar calendar\n";
        return 1;
    }

    // Example 1: Full Moon OR Yatyaza (your original request)
    std::cout << "🌕 Example 1: Full Moon OR Yatyaza Days\n";
    std::cout << "---------------------------------------\n";

    auto results1 = calendar.searchFullMoonOrYatyaza("2024-01-01", "2024-01-31", 16.8409, 96.1735);
    std::cout << "Found " << results1.size() << " dates that are either full moon or yatyaza:\n";

    for (const auto& result : results1) {
        std::cout << "  📅 " << result.gregorianDate
                  << " - " << result.matchDescription
                  << " (Score: " << std::fixed << std::setprecision(2) << result.matchScore << ")\n";
    }
    std::cout << "\n";

    // Example 2: Sabbath AND Full Moon
    std::cout << "🕯️ Example 2: Sabbath AND Full Moon Days\n";
    std::cout << "----------------------------------------\n";

    auto results2 = calendar.searchSabbathAndFullMoon("2024-01-01", "2024-03-31", 16.8409, 96.1735);
    std::cout << "Found " << results2.size() << " dates that are both sabbath and full moon:\n";

    for (const auto& result : results2) {
        std::cout << "  📅 " << result.gregorianDate
                  << " - " << result.matchDescription
                  << " (Score: " << std::fixed << std::setprecision(2) << result.matchScore << ")\n";
    }
    std::cout << "\n";

    // Example 3: Custom Multi-Criteria Search with OR Logic
    std::cout << "⚡ Example 3: Custom Multi-Criteria (Sabbath OR Thamanyo OR Pyathada)\n";
    std::cout << "---------------------------------------------------------------------\n";

    MyanmarCalendar::SearchCriteria criteria;
    criteria.searchSabbath = true;
    criteria.searchThamanyo = true;
    criteria.searchPyathada = true;

    auto results3 = calendar.searchMultiCriteria(criteria, MyanmarCalendar::LogicMode::OR,
                                                "2024-01-01", "2024-01-15", 16.8409, 96.1735);
    std::cout << "Found " << results3.size() << " dates matching sabbath OR thamanyo OR pyathada:\n";

    for (const auto& result : results3) {
        std::cout << "  📅 " << result.gregorianDate
                  << " - " << result.matchDescription
                  << " (Score: " << std::fixed << std::setprecision(2) << result.matchScore << ")\n";
    }
    std::cout << "\n";

    // Example 4: AND Logic - Year AND Month
    std::cout << "🗓️ Example 4: Year AND Month Search\n";
    std::cout << "-----------------------------------\n";

    MyanmarCalendar::SearchCriteria criteria2;
    criteria2.exactYear = 1385;
    criteria2.exactMonth = static_cast<int>(MyanmarMonth::TAGU); // Tagu month

    auto results4 = calendar.searchMultiCriteria(criteria2, MyanmarCalendar::LogicMode::AND,
                                                "2023-12-01", "2024-06-01", 16.8409, 96.1735);
    std::cout << "Found " << results4.size() << " dates in Myanmar year 1385 AND Tagu month:\n";

    for (const auto& result : results4) {
        std::cout << "  📅 " << result.gregorianDate
                  << " - " << result.matchDescription
                  << " (Score: " << std::fixed << std::setprecision(2) << result.matchScore << ")\n";
    }
    std::cout << "\n";

    std::cout << "✅ Multi-criteria search demo completed!\n";
    std::cout << "\n📖 Usage Summary:\n";
    std::cout << "• Use LogicMode::AND for all criteria to match (default)\n";
    std::cout << "• Use LogicMode::OR for any criteria to match\n";
    std::cout << "• Helper methods available for common searches:\n";
    std::cout << "  - searchFullMoonOrYatyaza()\n";
    std::cout << "  - searchSabbathAndFullMoon()\n";
    std::cout << "  - searchMultiCriteria() for custom combinations\n";

    return 0;
}
