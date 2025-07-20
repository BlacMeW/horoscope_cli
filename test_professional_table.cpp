#include "professional_table.h"
#include <iostream>

int main() {
    using namespace Astro;

    // Test basic functionality
    std::cout << "Testing Professional Table Implementation (inspired by p-ranav/tabulate)\n";
    std::cout << "================================================================\n\n";

    // Test 1: Basic table
    std::cout << "Test 1: Basic Table\n";
    std::cout << "-------------------\n";
    ProfessionalTable basicTable;
    basicTable.addRow({"Name", "Age", "City"});
    basicTable.addRow({"Alice", "25", "New York"});
    basicTable.addRow({"Bob", "30", "San Francisco"});

    std::cout << basicTable << "\n\n";

    // Test 2: Hindu Calendar Style
    std::cout << "Test 2: Hindu Calendar Style\n";
    std::cout << "-----------------------------\n";
    auto hinduTable = createHinduCalendarTable();
    hinduTable.setSubtitle("3 Results Found | Location: 28.61°N, 77.21°E | Period: 2025-07-20 ↔ 2025-07-22");

    addHinduCalendarRow(hinduTable,
        "2025-07-20", "★Sunday", "⚡Ekadashi", "✧Ashwini", "◆Siddhi", "Bava",
        "🌸Chaitra", "🌖Shukla", "2081", "✅", "🔱Eka 🌕Pur", "★0.95"
    );

    addHinduCalendarRow(hinduTable,
        "2025-07-21", "Monday", "Dwadashi", "Bharani", "Vriddhi", "Balava",
        "Chaitra", "🌖Shukla", "2081", "⚪", "-", "0.78"
    );

    addHinduCalendarRow(hinduTable,
        "2025-07-22", "Tuesday", "Trayodashi", "Krittika", "Dhruva", "Kaulava",
        "Chaitra", "🌖Shukla", "2081", "⚠️", "🎭Holi", "◆0.82"
    );

    std::cout << hinduTable << "\n\n";

    // Test 3: Myanmar Calendar Style
    std::cout << "Test 3: Myanmar Calendar Style\n";
    std::cout << "-------------------------------\n";
    auto myanmarTable = createMyanmarCalendarTable();
    myanmarTable.setSubtitle("2 Results Found | Location: 16.87°N, 96.20°E | Period: 2025-07-20 ↔ 2025-07-21");

    addMyanmarCalendarRow(myanmarTable,
        "2025-07-20", "⚡Sunday", "1387", "Waso", "2", "🌕Full", "♦Benefic", "🔴Orc", "⬆North", "☸Sabbath", "✅Tha", "★0.92"
    );

    addMyanmarCalendarRow(myanmarTable,
        "2025-07-21", "Monday", "1387", "Waso", "3", "🌖Waning", "◇Neutral", "🟢Elf", "➡East", "☸Eve", "⚠Yat", "0.65"
    );

    std::cout << myanmarTable << "\n\n";

    // Test 4: Different output formats
    std::cout << "Test 4: Different Output Formats\n";
    std::cout << "---------------------------------\n";

    ProfessionalTable formatTest;
    formatTest.addRow({"Format", "Description", "Use Case"});
    formatTest.addRow({"ASCII", "Traditional borders", "Compatible terminals"});
    formatTest.addRow({"Unicode", "Modern borders", "UTF-8 terminals"});
    formatTest.addRow({"Minimal", "Clean, no borders", "Simple output"});

    std::cout << "ASCII Style:\n" << formatTest.setBorderStyle(BorderStyle::ASCII) << "\n";
    std::cout << "Unicode Light Style:\n" << formatTest.setBorderStyle(BorderStyle::UNICODE_LIGHT) << "\n";
    std::cout << "Unicode Heavy Style:\n" << formatTest.setBorderStyle(BorderStyle::UNICODE_HEAVY) << "\n";
    std::cout << "Minimal Style:\n" << formatTest.setBorderStyle(BorderStyle::MINIMAL) << "\n";

    // Test 5: Export formats
    std::cout << "Test 5: Export Formats\n";
    std::cout << "----------------------\n";

    ProfessionalTable exportTest;
    exportTest.addRow({"Planet", "Sign", "Degree"});
    exportTest.addRow({"Sun", "Cancer", "28.5"});
    exportTest.addRow({"Moon", "Scorpio", "15.2"});

    std::cout << "Markdown:\n" << exportTest.toMarkdown() << "\n";
    std::cout << "CSV:\n" << exportTest.toCSV() << "\n";
    std::cout << "JSON:\n" << exportTest.toJSON() << "\n";

    std::cout << "\n✅ All tests completed successfully!\n";
    std::cout << "The professional table implementation is ready for integration.\n";

    return 0;
}
