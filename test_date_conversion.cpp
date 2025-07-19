#include "hindu_calendar.h"
#include <iostream>
#include <string>

using namespace Astro;

int main() {
    std::cout << "=== HINDU CALENDAR DATE CONVERSION UTILITY ===" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << std::endl;

    // Initialize Hindu Calendar with different configurations
    HinduCalendar calendar1; // Default: Lahiri Ayanamsa
    HinduCalendar calendar2(AyanamsaType::RAMAN); // B.V. Raman
    HinduCalendar calendar3(AyanamsaType::KRISHNAMURTI); // K.S. Krishnamurti

    if (!calendar1.initialize()) {
        std::cerr << "Error: Failed to initialize calendar1: " << calendar1.getLastError() << std::endl;
        return 1;
    }

    if (!calendar2.initialize()) {
        std::cerr << "Error: Failed to initialize calendar2: " << calendar2.getLastError() << std::endl;
        return 1;
    }

    if (!calendar3.initialize()) {
        std::cerr << "Error: Failed to initialize calendar3: " << calendar3.getLastError() << std::endl;
        return 1;
    }

    std::cout << "1. CONFIGURATION COMPARISON" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "Calendar 1: " << calendar1.getAyanamsaName() << " - " << calendar1.getCalculationMethodName() << std::endl;
    std::cout << "Calendar 2: " << calendar2.getAyanamsaName() << " - " << calendar2.getCalculationMethodName() << std::endl;
    std::cout << "Calendar 3: " << calendar3.getAyanamsaName() << " - " << calendar3.getCalculationMethodName() << std::endl;
    std::cout << std::endl;

    std::cout << "2. DATE CONVERSION EXAMPLES" << std::endl;
    std::cout << "===========================" << std::endl;

    // Test date conversions
    std::string gregorianDate = "2024-03-15";
    std::cout << "Gregorian Date: " << gregorianDate << std::endl;
    std::cout << "Hindu Date (Lahiri): " << calendar1.gregorianDateToHindu(2024, 3, 15) << std::endl;
    std::cout << "Hindu Date (Raman): " << calendar2.gregorianDateToHindu(2024, 3, 15) << std::endl;
    std::cout << "Hindu Date (KP): " << calendar3.gregorianDateToHindu(2024, 3, 15) << std::endl;
    std::cout << std::endl;

    std::cout << "3. PANCHANGA COMPARISON FOR SAME DATE" << std::endl;
    std::cout << "=====================================" << std::endl;

    double julianDay = calendar1.gregorianDateToJulianDay(2024, 3, 15, 12.0);

    PanchangaData panchanga1 = calendar1.calculatePanchanga(julianDay, 28.6139, 77.2090);
    PanchangaData panchanga2 = calendar2.calculatePanchanga(julianDay, 28.6139, 77.2090);
    PanchangaData panchanga3 = calendar3.calculatePanchanga(julianDay, 28.6139, 77.2090);

    std::cout << "Using Lahiri Ayanamsa:" << std::endl;
    std::cout << "  Sun: " << calendar1.getRashiName(panchanga1.sunRashi) << " ("
              << panchanga1.sunLongitude << "°)" << std::endl;
    std::cout << "  Moon: " << calendar1.getRashiName(panchanga1.moonRashi) << " ("
              << panchanga1.moonLongitude << "°)" << std::endl;
    std::cout << "  Nakshatra: " << calendar1.getNakshatraName(panchanga1.nakshatra) << std::endl;

    std::cout << "Using Raman Ayanamsa:" << std::endl;
    std::cout << "  Sun: " << calendar2.getRashiName(panchanga2.sunRashi) << " ("
              << panchanga2.sunLongitude << "°)" << std::endl;
    std::cout << "  Moon: " << calendar2.getRashiName(panchanga2.moonRashi) << " ("
              << panchanga2.moonLongitude << "°)" << std::endl;
    std::cout << "  Nakshatra: " << calendar2.getNakshatraName(panchanga2.nakshatra) << std::endl;

    std::cout << "Using Krishnamurti Ayanamsa:" << std::endl;
    std::cout << "  Sun: " << calendar3.getRashiName(panchanga3.sunRashi) << " ("
              << panchanga3.sunLongitude << "°)" << std::endl;
    std::cout << "  Moon: " << calendar3.getRashiName(panchanga3.moonRashi) << " ("
              << panchanga3.moonLongitude << "°)" << std::endl;
    std::cout << "  Nakshatra: " << calendar3.getNakshatraName(panchanga3.nakshatra) << std::endl;

    std::cout << std::endl;
    std::cout << "4. REVERSE DATE CONVERSION TEST" << std::endl;
    std::cout << "===============================" << std::endl;

    // Test Hindu to Gregorian conversion
    std::string hinduToGreg = calendar1.hinduDateToGregorian(2081, 12, 1, false); // Shukla Paksha
    std::cout << "Hindu Date: Phalguna 1, 2081 VS (Shukla) -> Gregorian: " << hinduToGreg << std::endl;

    hinduToGreg = calendar1.hinduDateToGregorian(2081, 12, 1, true); // Krishna Paksha
    std::cout << "Hindu Date: Phalguna 1, 2081 VS (Krishna) -> Gregorian: " << hinduToGreg << std::endl;

    std::cout << std::endl;
    std::cout << "=== SUMMARY ===" << std::endl;
    std::cout << "The enhanced Hindu calendar now provides:" << std::endl;
    std::cout << "• Multiple ayanamsa options for accurate sidereal calculations" << std::endl;
    std::cout << "• Bidirectional date conversion (Hindu ↔ Gregorian)" << std::endl;
    std::cout << "• Configuration-dependent results showing calculation differences" << std::endl;
    std::cout << "• Swiss Ephemeris integration for maximum astronomical accuracy" << std::endl;
    std::cout << "• Compatibility with traditional pancanga systems" << std::endl;

    return 0;
}
