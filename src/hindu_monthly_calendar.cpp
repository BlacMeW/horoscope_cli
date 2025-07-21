#include "hindu_monthly_calendar.h"
#include "ephemeris_manager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <map>

extern "C" {
#include "swephexp.h"
}

namespace Astro {

HinduMonthlyCalendar::HinduMonthlyCalendar()
    : latitude(28.6139), longitude(77.2090), initialized(false) {
    // Default to New Delhi coordinates
}

HinduMonthlyCalendar::HinduMonthlyCalendar(double lat, double lon, const DisplayOptions& options)
    : latitude(lat), longitude(lon), displayOptions(options), initialized(false) {
}

HinduMonthlyCalendar::~HinduMonthlyCalendar() {
}

bool HinduMonthlyCalendar::initialize() {
    try {
        // Initialize the Hindu calendar with specified options
        if (!hinduCalendar.initialize(displayOptions.ayanamsa, displayOptions.calculationMethod)) {
            lastError = "Failed to initialize Hindu calendar: " + hinduCalendar.getLastError();
            return false;
        }

        initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError = "Initialization error: " + std::string(e.what());
        return false;
    }
}

bool HinduMonthlyCalendar::initialize(double lat, double lon, const DisplayOptions& options) {
    latitude = lat;
    longitude = lon;
    displayOptions = options;
    return initialize();
}

// Static helper method
HinduMonthlyCalendar::DisplayOptions HinduMonthlyCalendar::getDefaultDisplayOptions() {
    DisplayOptions options;
    return options;
}

void HinduMonthlyCalendar::setLocation(double lat, double lon) {
    latitude = lat;
    longitude = lon;
}

void HinduMonthlyCalendar::setDisplayOptions(const DisplayOptions& options) {
    displayOptions = options;
    if (initialized) {
        // Reinitialize if ayanamsa or calculation method changed
        hinduCalendar.initialize(displayOptions.ayanamsa, displayOptions.calculationMethod);
    }
}

HinduMonthlyCalendar::MonthlyData HinduMonthlyCalendar::generateMonthlyData(int year, int month) const {
    if (!initialized) {
        throw std::runtime_error("Hindu Monthly Calendar not initialized");
    }

    MonthlyData monthData;
    monthData.year = year;
    monthData.month = month;
    monthData.isBCYear = (year < 0);
    monthData.daysInMonth = getDaysInMonth(year, month);
    monthData.firstDayWeekday = getFirstDayOfMonth(year, month);
    monthData.monthName = getMonthName(month);

    // Calculate daily panchanga for each day of the month
    for (int day = 1; day <= monthData.daysInMonth; day++) {
        double jd = calculateJulianDay(year, month, day, monthData.isBCYear);
        monthData.julianDays.push_back(jd);

        // Calculate panchanga for this day
        PanchangaData panchanga = hinduCalendar.calculatePanchanga(jd, latitude, longitude);
        monthData.dailyPanchanga.push_back(panchanga);

        // Format dates
        std::stringstream gregorianDate;
        gregorianDate << std::setfill('0') << std::setw(2) << day;
        monthData.gregorianDates.push_back(gregorianDate.str());

        std::stringstream hinduDate;
        hinduDate << panchanga.day << "/" << static_cast<int>(panchanga.month);
        monthData.hinduDates.push_back(hinduDate.str());
    }

    // Set Hindu year and month from first day
    if (!monthData.dailyPanchanga.empty()) {
        monthData.hinduYear = monthData.dailyPanchanga[0].vikramYear;
        monthData.hinduMonth = monthData.dailyPanchanga[0].month;
        monthData.hinduMonthName = getHinduMonthName(monthData.hinduMonth);
        monthData.currentRitu = monthData.dailyPanchanga[0].ritu;
        monthData.currentAyana = monthData.dailyPanchanga[0].ayana;
    }

    // Calculate monthly summaries
    calculateMonthlySummary(monthData);
    identifyMonthlyFestivals(monthData);

    return monthData;
}

HinduMonthlyCalendar::MonthlyData HinduMonthlyCalendar::generateMonthlyData(const std::string& dateStr) const {
    int year, month, day;
    bool isBCDate;

    if (!parseDate(dateStr, year, month, day, isBCDate)) {
        throw std::invalid_argument("Invalid date format: " + dateStr);
    }

    MonthlyData monthData = generateMonthlyData(year, month);
    monthData.isBCYear = isBCDate;
    return monthData;
}

std::string HinduMonthlyCalendar::generateCalendar(int year, int month) const {
    MonthlyData monthData = generateMonthlyData(year, month);
    return generateEnhancedHinduCalendar(monthData);
}

std::string HinduMonthlyCalendar::generateCalendar(const std::string& dateStr) const {
    int year, month, day;
    bool isBCDate;

    if (!parseDate(dateStr, year, month, day, isBCDate)) {
        throw std::invalid_argument("Invalid date format: " + dateStr);
    }

    return generateCalendar(year, month);
}

std::string HinduMonthlyCalendar::generateHTMLCalendar(int year, int month) const {
    MonthlyData monthData = generateMonthlyData(year, month);

    std::stringstream html;

    // HTML header
    html << generateHTMLHeader(monthData);

    // HTML table
    html << generateHTMLTable(monthData);

    // HTML footer
    html << generateHTMLFooter(monthData);

    return html.str();
}

std::string HinduMonthlyCalendar::generateHTMLCalendar(const std::string& dateStr) const {
    int year, month, day;
    bool isBCDate;

    if (!parseDate(dateStr, year, month, day, isBCDate)) {
        throw std::invalid_argument("Invalid date format: " + dateStr);
    }

    return generateHTMLCalendar(year, month);
}

std::string HinduMonthlyCalendar::generateBCCalendar(int bcYear, int month) const {
    // BC years are represented as negative numbers
    return generateCalendar(-bcYear, month);
}

HinduMonthlyCalendar::MonthlyData HinduMonthlyCalendar::generateBCMonthlyData(int bcYear, int month) const {
    return generateMonthlyData(-bcYear, month);
}

// Helper methods implementation

bool HinduMonthlyCalendar::isLeapYear(int year) const {
    if (year < 0) year = -year; // Handle BC years
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

int HinduMonthlyCalendar::getDaysInMonth(int year, int month) const {
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month < 1 || month > 12) return 0;

    int days = daysInMonth[month - 1];
    if (month == 2 && isLeapYear(year)) {
        days = 29;
    }

    return days;
}

int HinduMonthlyCalendar::getFirstDayOfMonth(int year, int month) const {
    double jd = calculateJulianDay(year, month, 1, year < 0);

    // Calculate day of week (0 = Sunday, 6 = Saturday)
    int dayOfWeek = static_cast<int>(std::fmod(jd + 1.5, 7));
    return dayOfWeek;
}

std::string HinduMonthlyCalendar::getMonthName(int month) const {
    static const std::vector<std::string> monthNames = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    if (month < 1 || month > 12) return "Invalid";
    return monthNames[month - 1];
}

std::string HinduMonthlyCalendar::getWeekdayName(int weekday, bool abbreviated) const {
    static const std::vector<std::string> fullNames = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };
    static const std::vector<std::string> shortNames = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    if (weekday < 0 || weekday > 6) return "Invalid";
    return abbreviated ? shortNames[weekday] : fullNames[weekday];
}

std::string HinduMonthlyCalendar::getHinduMonthName(HinduMonth month) const {
    static const std::map<HinduMonth, std::string> monthNames = {
        {HinduMonth::CHAITRA, "Chaitra"},
        {HinduMonth::VAISHAKHA, "Vaishakha"},
        {HinduMonth::JYESHTHA, "Jyeshtha"},
        {HinduMonth::ASHADHA, "Ashadha"},
        {HinduMonth::SHRAVANA, "Shravana"},
        {HinduMonth::BHADRAPADA, "Bhadrapada"},
        {HinduMonth::ASHWINI_MONTH, "Ashwini"},
        {HinduMonth::KARTIKA, "Kartika"},
        {HinduMonth::MARGASHIRSHA, "Margashirsha"},
        {HinduMonth::PAUSHA, "Pausha"},
        {HinduMonth::MAGHA, "Magha"},
        {HinduMonth::PHALGUNA, "Phalguna"}
    };

    auto it = monthNames.find(month);
    return (it != monthNames.end()) ? it->second : "Unknown";
}

std::string HinduMonthlyCalendar::getTithiName(Tithi tithi, bool abbreviated) const {
    return hinduCalendar.getTithiName(tithi);
}

std::string HinduMonthlyCalendar::getNakshatraName(HinduNakshatra nakshatra, bool abbreviated) const {
    return hinduCalendar.getNakshatraName(nakshatra);
}

std::string HinduMonthlyCalendar::formatDateCell(const PanchangaData& panchanga,
                                                const std::string& gregorianDate,
                                                const std::string& hinduDate,
                                                int day) const {
    std::stringstream cell;

    // Start with day number
    cell << std::setw(2) << day;

    if (displayOptions.showTithi) {
        cell << " " << static_cast<int>(panchanga.tithi);
    }

    if (displayOptions.showNakshatra) {
        // Show first letter of nakshatra
        std::string nakName = getNakshatraName(panchanga.nakshatra);
        if (!nakName.empty()) {
            cell << nakName.substr(0, 1);
        }
    }

    // Highlight special days
    std::string cellContent = cell.str();
    if (displayOptions.showSpecialDays) {
        cellContent = highlightSpecialDay(panchanga, cellContent);
    }

    // Pad to cell width
    while (cellContent.length() < static_cast<size_t>(displayOptions.cellWidth)) {
        cellContent += " ";
    }

    // Truncate if too long
    if (cellContent.length() > static_cast<size_t>(displayOptions.cellWidth)) {
        cellContent = cellContent.substr(0, displayOptions.cellWidth);
    }

    return cellContent;
}

std::string HinduMonthlyCalendar::formatWeekdayHeaders() const {
    std::stringstream headers;

    for (int i = 0; i < 7; i++) {
        std::string dayName = getWeekdayName(i, true);

        // Center the day name in the cell
        while (dayName.length() < static_cast<size_t>(displayOptions.cellWidth)) {
            dayName = " " + dayName + " ";
            if (dayName.length() > static_cast<size_t>(displayOptions.cellWidth)) {
                dayName = dayName.substr(0, displayOptions.cellWidth);
            }
        }

        headers << dayName;
        if (i < 6) headers << displayOptions.separator;
    }

    return headers.str();
}

std::string HinduMonthlyCalendar::formatTableBorder(bool isTop, bool isBottom) const {
    std::string border;
    int totalWidth = displayOptions.cellWidth * 7 + 6; // 6 separators
    border = std::string(totalWidth, '-');
    return border;
}

std::string HinduMonthlyCalendar::formatMonthTitle(const MonthlyData& monthData) const {
    std::stringstream title;

    if (monthData.isBCYear) {
        title << monthData.monthName << " " << (-monthData.year) << " BC";
    } else {
        title << monthData.monthName << " " << monthData.year;
    }

    title << " (" << monthData.hinduMonthName << " " << monthData.hinduYear << " VS)";

    if (!monthData.currentRitu.empty()) {
        title << " - " << monthData.currentRitu;
    }

    if (!monthData.currentAyana.empty()) {
        title << " (" << monthData.currentAyana << ")";
    }

    return title.str();
}

std::string HinduMonthlyCalendar::formatMonthSummary(const MonthlyData& monthData) const {
    std::stringstream summary;

    summary << "\n=== Month Summary ===\n";

    if (monthData.ekadashiCount > 0) {
        summary << "Ekadashi days: " << monthData.ekadashiCount << "\n";
    }

    if (monthData.purnimaCount > 0) {
        summary << "Purnima days: " << monthData.purnimaCount << "\n";
    }

    if (monthData.amavasyaCount > 0) {
        summary << "Amavasya days: " << monthData.amavasyaCount << "\n";
    }

    if (!monthData.monthlyFestivals.empty()) {
        summary << "\nFestivals and Special Events:\n";
        for (const auto& festival : monthData.monthlyFestivals) {
            summary << "• " << festival << "\n";
        }
    }

    return summary.str();
}

std::string HinduMonthlyCalendar::generateEnhancedHinduCalendar(const MonthlyData& monthData) const {
    std::stringstream ss;

    // Header with consistent formatting inspired by Myanmar multi-calendar
    ss << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                        🕉️ ENHANCED HINDU CALENDAR 🕉️                               ║\n";
    ss << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║  " << monthData.monthName << " " << (monthData.isBCYear ? abs(monthData.year) : monthData.year);

    if (monthData.isBCYear) {
        ss << " BC";
    } else {
        ss << " CE";
    }

    ss << " | " << monthData.hinduMonthName << " " << monthData.hinduYear << " VS";
    ss << " | " << monthData.currentRitu << " (" << monthData.currentAyana << ")";

    // Pad to consistent width
    std::string headerContent = monthData.monthName + " " + std::to_string(monthData.isBCYear ? abs(monthData.year) : monthData.year) +
                               (monthData.isBCYear ? " BC" : " CE") + " | " +
                               monthData.hinduMonthName + " " + std::to_string(monthData.hinduYear) + " VS | " +
                               monthData.currentRitu + " (" + monthData.currentAyana + ")";

    size_t targetLength = 80; // Target width inside borders
    if (headerContent.length() < targetLength) {
        for (size_t i = headerContent.length(); i < targetLength; i++) {
            ss << " ";
        }
    }
    ss << "║\n";
    ss << "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n";

    // Create fixed-width table with wider columns for better Panchanga data display
    ss << "┌────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┐\n";
    ss << "│Week│      SUN      │      MON      │      TUE      │      WED      │      THU      │      FRI      │      SAT      │\n";
    ss << "├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤\n";

    // Enhanced legend for Hindu calendar format
    ss << "│Fmt │[GG][T##][P]   │Hindu Day      │ Tithi/        │ Nakshatra/    │ Yoga/         │ Karana/       │ Festivals/    │\n";
    ss << "│    │Gregorian      │Tithi+Paksha   │ Special       │ Star Info     │ Combination   │ Half-Tithi    │ Events        │\n";
    ss << "├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤\n";

    // Calculate first day of week (0=Sunday)
    int firstDayOfWeek = monthData.firstDayWeekday;

    int dayIndex = 0;
    int weekNumber = 1;

    while (dayIndex < monthData.daysInMonth || weekNumber == 1) {
        // Week row with fixed-width cells
        ss << "│ " << std::setw(2) << weekNumber << " │";

        // Track day indices for this week
        std::vector<int> weekDayIndices(7, -1);

        for (int col = 0; col < 7; col++) {
            if (weekNumber == 1 && col < firstDayOfWeek) {
                ss << "               │"; // 15 spaces for empty cell
                weekDayIndices[col] = -1;
            } else if (dayIndex < monthData.daysInMonth) {
                const auto& panchanga = monthData.dailyPanchanga[dayIndex];
                std::string cellContent = formatEnhancedDateCell(panchanga, dayIndex + 1, monthData);
                ss << cellContent << "│";
                weekDayIndices[col] = dayIndex;
                dayIndex++;
            } else {
                ss << "               │"; // 15 spaces for empty cell
                weekDayIndices[col] = -1;
            }
        }
        ss << "\n";

        // Quality row with Panchanga details using stored indices
        ss << "│    │";

        for (int col = 0; col < 7; col++) {
            if (weekDayIndices[col] >= 0) {
                const auto& panchanga = monthData.dailyPanchanga[weekDayIndices[col]];
                std::string qualityCell = formatEnhancedQualityCell(panchanga, monthData);
                ss << qualityCell << "│";
            } else {
                ss << "               │"; // 15 spaces for empty cell
            }
        }
        ss << "\n";

        // Add row separator (except for last row)
        if (dayIndex < monthData.daysInMonth) {
            ss << "├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤\n";
        }

        weekNumber++;

        // Break if we've processed all days and this week is complete
        if (dayIndex >= monthData.daysInMonth && weekNumber > 1) {
            break;
        }
    }

    ss << "└────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┘\n\n";

    // Enhanced legend with comprehensive Hindu calendar information
    ss << "🕉️ ENHANCED HINDU CALENDAR LEGEND:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "Day Format: [GG][T##][P][JD][V] = Gregorian / Hindu Tithi+Paksha / Julian+Varna\n";
    ss << "           GG = Gregorian day (1-31)\n";
    ss << "           T## = Tithi number (1-15 in each Paksha)\n";
    ss << "           P = Paksha (S=Shukla/Waxing, K=Krishna/Waning)\n";
    ss << "           JD = Julian Day (last 3 digits for astronomical reference)\n";
    ss << "           V = Varna Day (B=Brahmin, K=Kshatriya, V=Vaishya, S=Shudra)\n\n";

    ss << "🌟 PANCHANGA INDICATORS:\n";
    ss << "   Special Days: Ek=Ekadashi, Pu=Purnima, Am=Amavasya, Sa=Sankranti\n";
    ss << "   Nakshatra: First 3 letters of current Nakshatra\n";
    ss << "   Quality: ✨ = Highly Auspicious, * = Auspicious, ! = Caution, # = Festival, . = Normal\n";
    ss << "   Yoga: Combination of Sun and Moon positions\n";
    ss << "   Karana: Half-Tithi periods\n\n";

    ss << "🎯 VARNA (SAVARNA) CLASSIFICATION:\n";
    ss << "   Traditional Hindu social-spiritual classification based on planetary weekday:\n";
    ss << "   B = Brahmin (Sun/Sunday, Jupiter/Thursday) - Spiritual/Intellectual\n";
    ss << "   K = Kshatriya (Mars/Tuesday) - Warrior/Administrative\n";
    ss << "   V = Vaishya (Mercury/Wednesday, Venus/Friday) - Merchant/Agricultural\n";
    ss << "   S = Shudra (Saturn/Saturday, Moon/Monday) - Service/Labor\n";
    ss << "   Note: Based on day ruler as per traditional Jyotisha texts\n\n";

    ss << "🌙 LUNAR PHASES: 🌕=Full, 🌗=Waxing, 🌑=New, 🌘=Waning\n";
    ss << "🎯 SPECIAL EVENTS: Festival days, Vratams, and significant observances\n";
    ss << "Quality: ✨ = Highly Auspicious * = Auspicious ! = Caution # = Festival . = Normal\n\n";

    // Comprehensive Hindu calendar summary
    ss << "📊 MONTHLY PANCHANGA STATISTICS:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "🕉️  Hindu Calendar: " << monthData.ekadashiCount << " Ekadashis, "
       << monthData.purnimaCount << " Purimas, " << monthData.amavasyaCount << " Amavasyas\n";

    // Add comprehensive Hindu calendar summary
    ss << "\n🔸 COMPREHENSIVE PANCHANGA OVERVIEW:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";

    ss << "📅 Month: " << monthData.hinduMonthName << " " << monthData.hinduYear << " VS\n";
    ss << "🌱 Season (Ritu): " << monthData.currentRitu << "\n";
    ss << "🌞 Ayana: " << monthData.currentAyana << "\n";
    if (monthData.isBCYear) {
        ss << "🏛️  Era: " << abs(monthData.year) << " BC (Before Common Era)\n";
    }
    ss << "\n";

    // Show special days throughout the month
    ss << "🌟 SPECIAL HINDU DAYS & FESTIVALS:\n";
    if (!monthData.monthlyFestivals.empty()) {
        for (const auto& festival : monthData.monthlyFestivals) {
            ss << "   " << festival << "\n";
        }
    } else {
        ss << "   No major festivals this month\n";
    }

    // Show Nakshatra distribution if we have the data
    if (!monthData.nakshatraCount.empty()) {
        ss << "\n🌌 NAKSHATRA DISTRIBUTION:\n";
        for (const auto& [nakshatra, count] : monthData.nakshatraCount) {
            if (count > 0) {
                ss << "   " << static_cast<int>(nakshatra) << ": " << count << " day(s)\n";
            }
        }
    }

    // Show Tithi distribution if we have the data
    if (!monthData.tithiCount.empty()) {
        ss << "\n🌙 TITHI PROGRESSION:\n";
        int shuklaCount = 0, krishnaCount = 0;
        for (const auto& [tithi, count] : monthData.tithiCount) {
            if (static_cast<int>(tithi) <= 15) shuklaCount += count;
            else krishnaCount += count;
        }
        ss << "   Shukla Paksha (Waxing): " << shuklaCount << " days\n";
        ss << "   Krishna Paksha (Waning): " << krishnaCount << " days\n";
    }

    return ss.str();
}

std::string HinduMonthlyCalendar::formatEnhancedDateCell(const PanchangaData& panchanga, int day, const MonthlyData& monthData) const {
    std::stringstream ss;

    // Enhanced format: [GG][T##][P][JD][V] - 15 characters
    // GG = Gregorian day (2 chars, right-aligned)
    // T## = Tithi number (3 chars)
    // P = Paksha indicator (1 char: S=Shukla, K=Krishna)
    // JD = Julian Day (last 3 digits)
    // V = Varna indicator (1 char: B=Brahmin, K=Kshatriya, V=Vaishya, S=Shudra)

    ss << std::setw(2) << std::setfill(' ') << day;

    // Add Tithi number (1-15, but show actual tithi)
    int tithiNumber = static_cast<int>(panchanga.tithi);
    if (tithiNumber > 15) tithiNumber -= 15; // Krishna paksha tithis
    ss << "T" << std::setw(2) << std::setfill('0') << tithiNumber;

    // Add Paksha indicator (S for Shukla, K for Krishna)
    if (static_cast<int>(panchanga.tithi) <= 15) {
        ss << "S";
    } else {
        ss << "K";
    }

    // Add Julian Day (last 3 digits for compactness)
    long jdInt = static_cast<long>(panchanga.julianDay);
    int jdLast3 = jdInt % 1000;
    ss << std::setw(3) << std::setfill('0') << jdLast3;

    // Add Varna indicator from day-based Varna
    char varnaChar = '.';
    if (!panchanga.varnaDay.empty()) {
        switch (panchanga.varnaDay[0]) {
            case 'B': varnaChar = 'B'; break; // Brahmin
            case 'K': varnaChar = 'K'; break; // Kshatriya
            case 'V': varnaChar = 'V'; break; // Vaishya
            case 'S': varnaChar = 'S'; break; // Shudra
            default: varnaChar = '.'; break;
        }
    }
    ss << varnaChar;

    // Add final space to reach 15 characters
    ss << " ";

    std::string result = ss.str();

    // Ensure exactly 15 characters
    if (result.length() > 15) {
        result = result.substr(0, 15);
    } else if (result.length() < 15) {
        result += std::string(15 - result.length(), ' ');
    }

    return result;
}

std::string HinduMonthlyCalendar::formatEnhancedQualityCell(const PanchangaData& panchanga, const MonthlyData& monthData) const {
    std::stringstream ss;

    // Enhanced fixed format: Quality + Panchanga indicators - exactly 15 characters

    // Quality indicator (1 char) - Enhanced for Hindu calendar
    bool isEkadashi = (static_cast<int>(panchanga.tithi) == 11 || static_cast<int>(panchanga.tithi) == 26);
    bool isPurnima = (static_cast<int>(panchanga.tithi) == 15);
    bool isAmavasya = (static_cast<int>(panchanga.tithi) == 30);

    if (isPurnima || isAmavasya) {
        ss << "✨"; // Highly auspicious - full/new moon
    } else if (isEkadashi) {
        ss << "*"; // Auspicious - Ekadashi
    } else if (static_cast<int>(panchanga.tithi) == 4 || static_cast<int>(panchanga.tithi) == 9 ||
               static_cast<int>(panchanga.tithi) == 14 || static_cast<int>(panchanga.tithi) == 19) {
        ss << "!"; // Caution days
    } else {
        ss << "."; // Normal
    }

    // Hindu special days (3 chars for enhanced coverage)
    if (isEkadashi) {
        ss << " Ek"; // Ekadashi
    } else if (isPurnima) {
        ss << " Pu"; // Purnima
    } else if (isAmavasya) {
        ss << " Am"; // Amavasya
    } else {
        ss << "   "; // Normal
    }

    // Nakshatra indicator (3 chars - abbreviated)
    std::string nakshatraName = getNakshatraName(panchanga.nakshatra, true);
    if (nakshatraName.length() >= 3) {
        ss << " " << nakshatraName.substr(0, 3);
    } else {
        ss << " " << std::setw(3) << std::left << nakshatraName;
    }

    // Additional quality indicators (7 chars remaining)
    std::string moonPhase;
    if (isPurnima) moonPhase = "   F"; // Full
    else if (isAmavasya) moonPhase = "   N"; // New
    else if (static_cast<int>(panchanga.tithi) < 15) moonPhase = "   W"; // Waxing
    else moonPhase = "   w"; // Waning

    ss << moonPhase;

    // Ensure exactly 15 characters
    std::string result = ss.str();
    if (result.length() > 15) {
        result = result.substr(0, 15);
    } else if (result.length() < 15) {
        result += std::string(15 - result.length(), ' ');
    }

    return result;
}

std::string HinduMonthlyCalendar::highlightSpecialDay(const PanchangaData& panchanga,
                                                     const std::string& text) const {
    if (!displayOptions.colorOutput) {
        return text;
    }

    std::string highlighted = text;

    if (panchanga.isEkadashi) {
        highlighted = getColorCode("yellow") + text + resetColor();
    } else if (panchanga.isPurnima) {
        highlighted = getColorCode("blue") + text + resetColor();
    } else if (panchanga.isAmavasya) {
        highlighted = getColorCode("red") + text + resetColor();
    } else if (panchanga.isSankranti) {
        highlighted = getColorCode("green") + text + resetColor();
    }

    return highlighted;
}

std::string HinduMonthlyCalendar::getColorCode(const std::string& colorName) const {
    static const std::map<std::string, std::string> colors = {
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"}
    };

    auto it = colors.find(colorName);
    return (it != colors.end()) ? it->second : "";
}

std::string HinduMonthlyCalendar::resetColor() const {
    return "\033[0m";
}

void HinduMonthlyCalendar::calculateMonthlySummary(MonthlyData& monthData) const {
    monthData.ekadashiCount = 0;
    monthData.purnimaCount = 0;
    monthData.amavasyaCount = 0;

    // Count special days and collect statistics
    for (const auto& panchanga : monthData.dailyPanchanga) {
        if (panchanga.isEkadashi) monthData.ekadashiCount++;
        if (panchanga.isPurnima) monthData.purnimaCount++;
        if (panchanga.isAmavasya) monthData.amavasyaCount++;

        // Count nakshatras, tithis, yogas
        monthData.nakshatraCount[panchanga.nakshatra]++;
        monthData.tithiCount[panchanga.tithi]++;
        monthData.yogaCount[panchanga.yoga]++;
    }
}

void HinduMonthlyCalendar::identifyMonthlyFestivals(MonthlyData& monthData) const {
    // This would be filled with festival identification logic
    // For now, we'll add a placeholder

    for (size_t i = 0; i < monthData.dailyPanchanga.size(); i++) {
        const auto& panchanga = monthData.dailyPanchanga[i];

        if (!panchanga.festivals.empty()) {
            for (const auto& festival : panchanga.festivals) {
                std::string festivalInfo = festival + " (Day " + std::to_string(i + 1) + ")";
                monthData.monthlyFestivals.push_back(festivalInfo);
            }
        }

        if (!panchanga.specialEvents.empty()) {
            for (const auto& event : panchanga.specialEvents) {
                std::string eventInfo = event + " (Day " + std::to_string(i + 1) + ")";
                monthData.monthlyEvents.push_back(eventInfo);
            }
        }
    }
}

bool HinduMonthlyCalendar::parseDate(const std::string& dateStr, int& year, int& month, int& day, bool& isBCDate) const {
    isBCDate = false;
    day = 1; // Default day

    // Handle BC dates
    std::string workStr = dateStr;
    if (workStr.find("BC-") != std::string::npos) {
        isBCDate = true;
        size_t bcPos = workStr.find("BC-");
        std::string yearStr = workStr.substr(0, bcPos);
        std::string dateStr = workStr.substr(bcPos + 3);
        year = std::stoi(yearStr);
        if (year > 0) year = -year; // Store BC years as negative for internal processing

        // Parse month from the remaining string
        if (!dateStr.empty()) {
            month = std::stoi(dateStr);
        } else {
            month = 1; // Default month
        }
        return true;
    } else if (workStr.find(" BC") != std::string::npos || workStr.find("BC") != std::string::npos) {
        isBCDate = true;
        size_t bcPos = workStr.find(" BC");
        if (bcPos == std::string::npos) bcPos = workStr.find("BC");
        workStr = workStr.substr(0, bcPos);
    }

    // Parse YYYY-MM or YYYY-MM-DD format
    std::stringstream ss(workStr);
    std::string token;

    if (std::getline(ss, token, '-')) {
        year = std::stoi(token);
        if (isBCDate && year > 0) year = -year; // Store BC years as negative for internal processing
    } else {
        return false;
    }

    if (std::getline(ss, token, '-')) {
        month = std::stoi(token);
    } else {
        return false;
    }

    if (std::getline(ss, token)) {
        day = std::stoi(token);
    }

    return true;
}

double HinduMonthlyCalendar::calculateJulianDay(int year, int month, int day, bool isBCDate) const {
    // Use Swiss Ephemeris Julian Day calculation which handles BC dates
    double jd;
    int gregflag = SE_GREG_CAL; // Gregorian calendar

    // Convert BC years to astronomical year numbering
    // 1 BC = year 0, 2 BC = year -1, etc.
    int astroYear = year;
    if (isBCDate || year < 0) {
        if (year > 0) {
            astroYear = 1 - year; // 1 BC becomes 0, 2 BC becomes -1, etc.
        }
    }

    // Calculate Julian Day at noon (12:00 UTC)
    jd = swe_julday(astroYear, month, day, 12.0, gregflag);

    return jd;
}

std::vector<int> HinduMonthlyCalendar::getEkadashiDays(int year, int month) const {
    MonthlyData monthData = generateMonthlyData(year, month);
    std::vector<int> ekadashiDays;

    for (size_t i = 0; i < monthData.dailyPanchanga.size(); i++) {
        if (monthData.dailyPanchanga[i].isEkadashi) {
            ekadashiDays.push_back(i + 1); // Day number (1-based)
        }
    }

    return ekadashiDays;
}

std::vector<int> HinduMonthlyCalendar::getPurnimaDays(int year, int month) const {
    MonthlyData monthData = generateMonthlyData(year, month);
    std::vector<int> purnimaDays;

    for (size_t i = 0; i < monthData.dailyPanchanga.size(); i++) {
        if (monthData.dailyPanchanga[i].isPurnima) {
            purnimaDays.push_back(i + 1);
        }
    }

    return purnimaDays;
}

std::vector<int> HinduMonthlyCalendar::getAmavasyaDays(int year, int month) const {
    MonthlyData monthData = generateMonthlyData(year, month);
    std::vector<int> amavasyaDays;

    for (size_t i = 0; i < monthData.dailyPanchanga.size(); i++) {
        if (monthData.dailyPanchanga[i].isAmavasya) {
            amavasyaDays.push_back(i + 1);
        }
    }

    return amavasyaDays;
}

std::string HinduMonthlyCalendar::exportToCSV(const MonthlyData& monthData) const {
    std::stringstream csv;

    // Header
    csv << "Day,Gregorian Date,Hindu Date,Tithi,Nakshatra,Yoga,Karana,Festivals\n";

    // Data rows
    for (size_t i = 0; i < monthData.dailyPanchanga.size(); i++) {
        const auto& panchanga = monthData.dailyPanchanga[i];

        csv << (i + 1) << ",";
        csv << monthData.year << "-" << std::setfill('0') << std::setw(2) << monthData.month
            << "-" << std::setw(2) << (i + 1) << ",";
        csv << monthData.hinduDates[i] << ",";
        csv << getTithiName(panchanga.tithi) << ",";
        csv << getNakshatraName(panchanga.nakshatra) << ",";
        csv << hinduCalendar.getYogaName(panchanga.yoga) << ",";
        csv << hinduCalendar.getKaranaName(panchanga.karana) << ",";

        // Festivals
        for (size_t j = 0; j < panchanga.festivals.size(); j++) {
            if (j > 0) csv << "; ";
            csv << panchanga.festivals[j];
        }
        csv << "\n";
    }

    return csv.str();
}

std::string HinduMonthlyCalendar::exportToJSON(const MonthlyData& monthData) const {
    std::stringstream json;

    json << "{\n";
    json << "  \"year\": " << monthData.year << ",\n";
    json << "  \"month\": " << monthData.month << ",\n";
    json << "  \"isBCYear\": " << (monthData.isBCYear ? "true" : "false") << ",\n";
    json << "  \"monthName\": \"" << monthData.monthName << "\",\n";
    json << "  \"hinduYear\": " << monthData.hinduYear << ",\n";
    json << "  \"hinduMonthName\": \"" << monthData.hinduMonthName << "\",\n";
    json << "  \"currentRitu\": \"" << monthData.currentRitu << "\",\n";
    json << "  \"currentAyana\": \"" << monthData.currentAyana << "\",\n";
    json << "  \"daysInMonth\": " << monthData.daysInMonth << ",\n";
    json << "  \"ekadashiCount\": " << monthData.ekadashiCount << ",\n";
    json << "  \"purnimaCount\": " << monthData.purnimaCount << ",\n";
    json << "  \"amavasyaCount\": " << monthData.amavasyaCount << ",\n";
    json << "  \"days\": [\n";

    for (size_t i = 0; i < monthData.dailyPanchanga.size(); i++) {
        const auto& panchanga = monthData.dailyPanchanga[i];

        json << "    {\n";
        json << "      \"day\": " << (i + 1) << ",\n";
        json << "      \"gregorianDate\": \"" << monthData.year << "-"
             << std::setfill('0') << std::setw(2) << monthData.month
             << "-" << std::setw(2) << (i + 1) << "\",\n";
        json << "      \"hinduDate\": \"" << monthData.hinduDates[i] << "\",\n";
        json << "      \"julianDay\": " << monthData.julianDays[i] << ",\n";
        json << "      \"tithi\": " << static_cast<int>(panchanga.tithi) << ",\n";
        json << "      \"tithiName\": \"" << getTithiName(panchanga.tithi) << "\",\n";
        json << "      \"nakshatra\": " << static_cast<int>(panchanga.nakshatra) << ",\n";
        json << "      \"nakshatraName\": \"" << getNakshatraName(panchanga.nakshatra) << "\",\n";
        json << "      \"yoga\": " << static_cast<int>(panchanga.yoga) << ",\n";
        json << "      \"yogaName\": \"" << hinduCalendar.getYogaName(panchanga.yoga) << "\",\n";
        json << "      \"karana\": " << static_cast<int>(panchanga.karana) << ",\n";
        json << "      \"karanaName\": \"" << hinduCalendar.getKaranaName(panchanga.karana) << "\",\n";
        json << "      \"isEkadashi\": " << (panchanga.isEkadashi ? "true" : "false") << ",\n";
        json << "      \"isPurnima\": " << (panchanga.isPurnima ? "true" : "false") << ",\n";
        json << "      \"isAmavasya\": " << (panchanga.isAmavasya ? "true" : "false") << ",\n";
        json << "      \"isSankranti\": " << (panchanga.isSankranti ? "true" : "false") << ",\n";
        json << "      \"festivals\": [";
        for (size_t j = 0; j < panchanga.festivals.size(); j++) {
            if (j > 0) json << ", ";
            json << "\"" << panchanga.festivals[j] << "\"";
        }
        json << "]\n";
        json << "    }";
        if (i < monthData.dailyPanchanga.size() - 1) json << ",";
        json << "\n";
    }

    json << "  ],\n";
    json << "  \"monthlyFestivals\": [";
    for (size_t i = 0; i < monthData.monthlyFestivals.size(); i++) {
        if (i > 0) json << ", ";
        json << "\"" << monthData.monthlyFestivals[i] << "\"";
    }
    json << "]\n";
    json << "}\n";

    return json.str();
}

std::string HinduMonthlyCalendar::exportToXML(const MonthlyData& monthData) const {
    std::stringstream xml;

    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<hinduMonthlyCalendar>\n";
    xml << "  <metadata>\n";
    xml << "    <year>" << monthData.year << "</year>\n";
    xml << "    <month>" << monthData.month << "</month>\n";
    xml << "    <isBCYear>" << (monthData.isBCYear ? "true" : "false") << "</isBCYear>\n";
    xml << "    <monthName>" << monthData.monthName << "</monthName>\n";
    xml << "    <hinduYear>" << monthData.hinduYear << "</hinduYear>\n";
    xml << "    <hinduMonthName>" << monthData.hinduMonthName << "</hinduMonthName>\n";
    xml << "    <currentRitu>" << monthData.currentRitu << "</currentRitu>\n";
    xml << "    <currentAyana>" << monthData.currentAyana << "</currentAyana>\n";
    xml << "    <daysInMonth>" << monthData.daysInMonth << "</daysInMonth>\n";
    xml << "    <ekadashiCount>" << monthData.ekadashiCount << "</ekadashiCount>\n";
    xml << "    <purnimaCount>" << monthData.purnimaCount << "</purnimaCount>\n";
    xml << "    <amavasyaCount>" << monthData.amavasyaCount << "</amavasyaCount>\n";
    xml << "  </metadata>\n";
    xml << "  <days>\n";

    for (size_t i = 0; i < monthData.dailyPanchanga.size(); i++) {
        const auto& panchanga = monthData.dailyPanchanga[i];

        xml << "    <day number=\"" << (i + 1) << "\">\n";
        xml << "      <gregorianDate>" << monthData.year << "-"
            << std::setfill('0') << std::setw(2) << monthData.month
            << "-" << std::setw(2) << (i + 1) << "</gregorianDate>\n";
        xml << "      <hinduDate>" << monthData.hinduDates[i] << "</hinduDate>\n";
        xml << "      <julianDay>" << monthData.julianDays[i] << "</julianDay>\n";
        xml << "      <panchanga>\n";
        xml << "        <tithi number=\"" << static_cast<int>(panchanga.tithi) << "\">"
            << getTithiName(panchanga.tithi) << "</tithi>\n";
        xml << "        <nakshatra number=\"" << static_cast<int>(panchanga.nakshatra) << "\">"
            << getNakshatraName(panchanga.nakshatra) << "</nakshatra>\n";
        xml << "        <yoga number=\"" << static_cast<int>(panchanga.yoga) << "\">"
            << hinduCalendar.getYogaName(panchanga.yoga) << "</yoga>\n";
        xml << "        <karana number=\"" << static_cast<int>(panchanga.karana) << "\">"
            << hinduCalendar.getKaranaName(panchanga.karana) << "</karana>\n";
        xml << "      </panchanga>\n";
        xml << "      <specialDays>\n";
        xml << "        <isEkadashi>" << (panchanga.isEkadashi ? "true" : "false") << "</isEkadashi>\n";
        xml << "        <isPurnima>" << (panchanga.isPurnima ? "true" : "false") << "</isPurnima>\n";
        xml << "        <isAmavasya>" << (panchanga.isAmavasya ? "true" : "false") << "</isAmavasya>\n";
        xml << "        <isSankranti>" << (panchanga.isSankranti ? "true" : "false") << "</isSankranti>\n";
        xml << "      </specialDays>\n";
        xml << "      <festivals>\n";
        for (const auto& festival : panchanga.festivals) {
            xml << "        <festival>" << festival << "</festival>\n";
        }
        xml << "      </festivals>\n";
        xml << "    </day>\n";
    }

    xml << "  </days>\n";
    xml << "  <monthlyFestivals>\n";
    for (const auto& festival : monthData.monthlyFestivals) {
        xml << "    <festival>" << festival << "</festival>\n";
    }
    xml << "  </monthlyFestivals>\n";
    xml << "</hinduMonthlyCalendar>\n";

    return xml.str();
}

std::string HinduMonthlyCalendar::generateHTMLHeader(const MonthlyData& monthData) const {
    std::stringstream html;

    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"en\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>Hindu Calendar - " << formatMonthTitle(monthData) << "</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html << "        .calendar-title { text-align: center; color: #8B4513; margin-bottom: 20px; }\n";
    html << "        .calendar-table { width: 100%; border-collapse: collapse; }\n";
    html << "        .calendar-table th, .calendar-table td { border: 1px solid #ddd; padding: 10px; text-align: center; }\n";
    html << "        .calendar-table th { background-color: #f2f2f2; font-weight: bold; }\n";
    html << "        .day-cell { vertical-align: top; height: 100px; width: 14.28%; }\n";
    html << "        .day-number { font-weight: bold; color: #333; }\n";
    html << "        .tithi { font-size: 0.8em; color: #666; }\n";
    html << "        .nakshatra { font-size: 0.8em; color: #666; }\n";
    html << "        .ekadashi { background-color: #FFF8DC; }\n";
    html << "        .purnima { background-color: #E6E6FA; }\n";
    html << "        .amavasya { background-color: #F0F8FF; }\n";
    html << "        .sankranti { background-color: #F0FFF0; }\n";
    html << "        .festival { font-size: 0.7em; color: #B22222; font-weight: bold; }\n";
    html << "        .summary { margin-top: 20px; padding: 15px; background-color: #f9f9f9; border-radius: 5px; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <h1 class=\"calendar-title\">" << formatMonthTitle(monthData) << "</h1>\n";

    return html.str();
}

std::string HinduMonthlyCalendar::generateHTMLTable(const MonthlyData& monthData) const {
    std::stringstream html;

    html << "    <table class=\"calendar-table\">\n";

    // Header row
    html << "        <tr>\n";
    for (int i = 0; i < 7; i++) {
        html << "            <th>" << getWeekdayName(i) << "</th>\n";
    }
    html << "        </tr>\n";

    // Calendar rows
    int currentDay = 1;
    int week = 0;

    while (currentDay <= monthData.daysInMonth) {
        html << "        <tr>\n";

        for (int dayOfWeek = 0; dayOfWeek < 7; dayOfWeek++) {
            if (week == 0 && dayOfWeek < monthData.firstDayWeekday) {
                // Empty cell
                html << "            <td class=\"day-cell\"></td>\n";
            } else if (currentDay <= monthData.daysInMonth) {
                const PanchangaData& panchanga = monthData.dailyPanchanga[currentDay - 1];
                const std::string& hinduDate = monthData.hinduDates[currentDay - 1];

                html << generateHTMLCell(panchanga, monthData.gregorianDates[currentDay - 1], hinduDate, currentDay);
                currentDay++;
            } else {
                // Empty cell after month ends
                html << "            <td class=\"day-cell\"></td>\n";
            }
        }

        html << "        </tr>\n";
        week++;
    }

    html << "    </table>\n";

    return html.str();
}

std::string HinduMonthlyCalendar::generateHTMLFooter(const MonthlyData& monthData) const {
    std::stringstream html;

    html << "    <div class=\"summary\">\n";
    html << "        <h3>Month Summary</h3>\n";

    if (monthData.ekadashiCount > 0) {
        html << "        <p><strong>Ekadashi days:</strong> " << monthData.ekadashiCount << "</p>\n";
    }

    if (monthData.purnimaCount > 0) {
        html << "        <p><strong>Purnima days:</strong> " << monthData.purnimaCount << "</p>\n";
    }

    if (monthData.amavasyaCount > 0) {
        html << "        <p><strong>Amavasya days:</strong> " << monthData.amavasyaCount << "</p>\n";
    }

    if (!monthData.monthlyFestivals.empty()) {
        html << "        <h4>Festivals and Special Events:</h4>\n";
        html << "        <ul>\n";
        for (const auto& festival : monthData.monthlyFestivals) {
            html << "            <li>" << festival << "</li>\n";
        }
        html << "        </ul>\n";
    }

    html << "    </div>\n";
    html << "</body>\n";
    html << "</html>\n";

    return html.str();
}

std::string HinduMonthlyCalendar::generateHTMLCell(const PanchangaData& panchanga,
                                                  const std::string& gregorianDate,
                                                  const std::string& hinduDate,
                                                  int day) const {
    std::stringstream html;

    // Determine cell class based on special days
    std::string cellClass = "day-cell";
    if (panchanga.isEkadashi) {
        cellClass += " ekadashi";
    } else if (panchanga.isPurnima) {
        cellClass += " purnima";
    } else if (panchanga.isAmavasya) {
        cellClass += " amavasya";
    } else if (panchanga.isSankranti) {
        cellClass += " sankranti";
    }

    html << "            <td class=\"" << cellClass << "\">\n";
    html << "                <div class=\"day-number\">" << day << "</div>\n";

    if (displayOptions.showTithi) {
        html << "                <div class=\"tithi\">T:" << static_cast<int>(panchanga.tithi) << "</div>\n";
    }

    if (displayOptions.showNakshatra) {
        std::string nakName = getNakshatraName(panchanga.nakshatra);
        if (nakName.length() > 8) nakName = nakName.substr(0, 8) + "...";
        html << "                <div class=\"nakshatra\">N:" << nakName << "</div>\n";
    }

    // Add festivals
    if (!panchanga.festivals.empty() && displayOptions.showFestivals) {
        for (const auto& festival : panchanga.festivals) {
            std::string festName = festival;
            if (festName.length() > 15) festName = festName.substr(0, 15) + "...";
            html << "                <div class=\"festival\">" << festName << "</div>\n";
        }
    }

    html << "            </td>\n";

    return html.str();
}

bool HinduMonthlyCalendar::isValidDate(int year, int month, int day) const {
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;

    int daysInMonth = getDaysInMonth(year, month);
    return day <= daysInMonth;
}

std::string HinduMonthlyCalendar::getCurrentDate() const {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::stringstream date;
    date << (localTime->tm_year + 1900) << "-"
         << std::setfill('0') << std::setw(2) << (localTime->tm_mon + 1) << "-"
         << std::setw(2) << localTime->tm_mday;

    return date.str();
}

// Static utility methods
std::string HinduMonthlyCalendar::formatYear(int year, bool isBCDate) {
    std::stringstream yearStr;
    if (isBCDate) {
        yearStr << year << " BC";
    } else {
        yearStr << year;
    }
    return yearStr.str();
}

bool HinduMonthlyCalendar::isValidBCYear(int year) {
    // For Hindu calendar calculations, we can go back to Kali Yuga start (3102 BC)
    return year >= 1 && year <= 5000; // Reasonable range for BC calculations
}

// Utility namespace implementations
namespace HinduCalendarUtils {

bool isBCYear(const std::string& yearStr) {
    return yearStr.find("BC") != std::string::npos || yearStr.find("BCE") != std::string::npos;
}

int parseBCYear(const std::string& yearStr) {
    std::string numStr = yearStr;

    // Remove BC/BCE suffix
    size_t bcPos = numStr.find(" BC");
    if (bcPos == std::string::npos) bcPos = numStr.find("BC");
    if (bcPos == std::string::npos) bcPos = numStr.find(" BCE");
    if (bcPos == std::string::npos) bcPos = numStr.find("BCE");

    if (bcPos != std::string::npos) {
        numStr = numStr.substr(0, bcPos);
    }

    return std::stoi(numStr);
}

std::string formatBCYear(int year) {
    return std::to_string(year) + " BC";
}

std::string padString(const std::string& str, int width, char padChar) {
    if (static_cast<int>(str.length()) >= width) {
        return str;
    }

    int padding = width - str.length();
    return str + std::string(padding, padChar);
}

std::string centerString(const std::string& str, int width) {
    if (static_cast<int>(str.length()) >= width) {
        return str.substr(0, width);
    }

    int padding = width - str.length();
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;

    return std::string(leftPad, ' ') + str + std::string(rightPad, ' ');
}

std::string truncateString(const std::string& str, int maxLength) {
    if (static_cast<int>(str.length()) <= maxLength) {
        return str;
    }

    return str.substr(0, maxLength - 3) + "...";
}

} // namespace HinduCalendarUtils

} // namespace Astro
