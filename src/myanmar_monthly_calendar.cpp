/////////////////////////////////////////////////////////////////////////////
// File: myanmar_monthly_calendar.cpp
// Description: Myanmar Monthly Calendar Display Implementation
// Reference: Inspired by https://mmcal.blogspot.com/ design
/////////////////////////////////////////////////////////////////////////////

#include "myanmar_monthly_calendar.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

extern "C" {
#include "swephexp.h"
}

namespace Astro {

/////////////////////////////////////////////////////////////////////////////
// Constructor and Initialization
/////////////////////////////////////////////////////////////////////////////

MyanmarMonthlyCalendar::MyanmarMonthlyCalendar()
    : initialized(false), includeGregorian(true), includeHindu(false),
      includePlanetary(false), includeAdvancedAstro(false) {
}

MyanmarMonthlyCalendar::~MyanmarMonthlyCalendar() {
}

bool MyanmarMonthlyCalendar::initialize() {
    if (!myanmarCalendar.initialize()) {
        lastError = "Failed to initialize Myanmar calendar";
        return false;
    }

    if (!hinduCalendar.initialize()) {
        lastError = "Failed to initialize Hindu calendar";
        return false;
    }

    if (!astroCalendar.initialize()) {
        lastError = "Failed to initialize Astro calendar";
        return false;
    }

    initialized = true;
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Main Calendar Generation Methods
/////////////////////////////////////////////////////////////////////////////

MyanmarMonthlyData MyanmarMonthlyCalendar::calculateMonthlyData(int year, int month, double latitude, double longitude,
                                                              bool includeGregorian, bool includeHindu,
                                                              bool includePlanetary, bool includeAdvancedAstro) const {
    MyanmarMonthlyData monthData;

    monthData.gregorianYear = year;
    monthData.gregorianMonth = month;

    // Store include flags
    monthData.includeGregorian = includeGregorian;
    monthData.includeHindu = includeHindu;
    monthData.includePlanetary = includePlanetary;
    monthData.includeAdvancedAstro = includeAdvancedAstro;

    // Month names
    std::vector<std::string> monthNames = {"January", "February", "March", "April", "May", "June",
                                         "July", "August", "September", "October", "November", "December"};
    monthData.gregorianMonthName = monthNames[month - 1];

    // Calculate days in month
    int daysInMonth = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        daysInMonth = 30;
    } else if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        daysInMonth = isLeap ? 29 : 28;
    }

    // Calculate Myanmar calendar data for each day
    for (int day = 1; day <= daysInMonth; day++) {
        MyanmarMonthlyData::DayData dayData;
        dayData.gregorianDay = day;

        // Convert to Julian Day using Swiss Ephemeris for consistency
        // This ensures the same calculation as used in individual Myanmar calendar calculations
        // Use noon (12.0) as the time to be consistent with other calendar calculations
        double jd = swe_julday(year, month, day, 12.0, SE_GREG_CAL);
        dayData.julianDay = jd;

        // Calculate Myanmar calendar data
        try {
            MyanmarCalendarData myanmarData = myanmarCalendar.calculateMyanmarCalendar(jd);

            // Extract Myanmar date information
            dayData.myanmarDay = myanmarData.dayOfMonth;
            dayData.weekday = myanmarData.weekday;
            dayData.moonPhase = myanmarData.moonPhase;
            dayData.isSabbath = myanmarData.isSabbath;
            dayData.isYatyaza = myanmarData.isYatyaza;
            dayData.isPyathada = myanmarData.isPyathada;
            dayData.isThamanyo = myanmarData.isThamanyo;
            dayData.festivals = myanmarData.festivals;
            dayData.isHoliday = !myanmarData.festivals.empty();

            // Set Myanmar year and month from first day
            if (day == 1) {
                monthData.myanmarYear = myanmarData.myanmarYear;
                monthData.myanmarMonth = myanmarData.month;
                monthData.myanmarMonthName = myanmarCalendar.getMyanmarMonthName(myanmarData.month);
            }

            // Determine day quality
            if (dayData.isSabbath) {
                dayData.qualityIndicator = "*"; // Religious / Sabbath
            } else if (dayData.isYatyaza) {
                dayData.qualityIndicator = "+"; // Auspicious (Yatyaza)
            } else if (dayData.isPyathada || dayData.isThamanyo) {
                dayData.qualityIndicator = "!"; // Inauspicious / Caution
            } else if (dayData.isHoliday) {
                dayData.qualityIndicator = "#"; // Festival
            } else {
                dayData.qualityIndicator = "."; // Neutral
            }

            // Calculate additional calendar systems if requested
            if (includeGregorian) {
                calculateGregorianData(dayData, year, month, day);
            }

            if (includeHindu) {
                calculateHinduCalendarData(dayData, jd);
            }

            if (includePlanetary || includeAdvancedAstro) {
                calculatePlanetaryData(dayData, jd);
            }

        } catch (...) {
            // Fill with default values if calculation fails
            dayData.myanmarDay = 0;
            dayData.weekday = MyanmarWeekday::SATURDAY;
            dayData.moonPhase = MyanmarMoonPhase::NEW_MOON;
            dayData.qualityIndicator = ".";

            // Initialize Hindu data with default values to prevent segfault
            if (includeHindu) {
                dayData.hindu.hinduDay = 1;
                dayData.hindu.hinduYear = 2024;
                dayData.hindu.hinduMonthName = "Unknown";
                dayData.hindu.paksha = "Unknown";
                dayData.hindu.nakshatraName = "Unknown";
                dayData.hindu.vikramSamvat = 2081;
                dayData.hindu.ritu = "Unknown";
                dayData.hindu.isEkadashi = false;
                dayData.hindu.isPurnima = false;
                dayData.hindu.isAmavasya = false;
                dayData.hindu.isAuspiciousDay = false;
                dayData.hindu.isInauspiciousDay = false;
            }
        }

        monthData.days.push_back(dayData);
    }

    // Calculate statistics and identify special days
    calculateMonthlyStatistics(monthData);
    calculateMultiCalendarStatistics(monthData);
    identifySpecialDays(monthData);

    return monthData;
}

/////////////////////////////////////////////////////////////////////////////
// Output Format Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::generateMonthlyCalendar(const MyanmarMonthlyData& monthData, const std::string& format) const {
    if (format == "traditional") {
        return generateTraditionalLayout(monthData);
    } else if (format == "modern") {
        return generateModernLayout(monthData);
    } else if (format == "compact") {
        return generateCompactLayout(monthData);
    } else if (format == "blog-style") {
        return generateBlogStyleLayout(monthData);
    } else if (format == "tabulate") {
        return generateTabulateLayout(monthData);
    } else if (format == "tabulate-modern") {
        return generateTabulateModern(monthData);
    } else if (format == "tabulate-classic") {
        return generateTabulateClassic(monthData);
    } else if (format == "tabulate-minimal") {
        return generateTabulateMinimal(monthData);
    } else if (format == "json") {
        return generateJSON(monthData);
    } else if (format == "csv") {
        return generateCSV(monthData);
    } else if (format == "html") {
        return generateHTML(monthData);
    } else if (format == "multi-calendar") {
        return generateMultiCalendarView(monthData);
    } else if (format == "planetary") {
        return generatePlanetaryCalendar(monthData);
    } else if (format == "hindu-myanmar") {
        return generateHinduMyanmarCalendar(monthData);
    } else if (format == "full-astronomical") {
        return generateFullAstronomicalCalendar(monthData);
    } else {
        return generateTabulateLayout(monthData); // Default to tabulate
    }
}

std::string MyanmarMonthlyCalendar::generateTraditionalMyanmarCalendar(const MyanmarMonthlyData& monthData) const {
    return generateTraditionalLayout(monthData);
}

std::string MyanmarMonthlyCalendar::generateModernMyanmarCalendar(const MyanmarMonthlyData& monthData) const {
    return generateModernLayout(monthData);
}

std::string MyanmarMonthlyCalendar::generateCompactMyanmarCalendar(const MyanmarMonthlyData& monthData) const {
    return generateCompactLayout(monthData);
}

std::string MyanmarMonthlyCalendar::generateBlogStyleMyanmarCalendar(const MyanmarMonthlyData& monthData) const {
    return generateBlogStyleLayout(monthData);
}

/////////////////////////////////////////////////////////////////////////////
// Layout Generation Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::generateTraditionalLayout(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Traditional Myanmar calendar header
    ss << "╔══════════════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                            MYANMAR CALENDAR                                              ║\n";
    ss << "╠══════════════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║  " << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " CE   |   " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME";

    // Calculate padding
    int used = monthData.gregorianMonthName.length() + std::to_string(monthData.gregorianYear).length() +
               monthData.myanmarMonthName.length() + std::to_string(monthData.myanmarYear).length() + 15;
    int padding = 88 - used;
    ss << std::string(padding, ' ') << "║\n";

    ss << "╠══════════════════════════════════════════════════════════════════════════════════════════╣\n";

    // Weekday headers (both English and Myanmar) - 12 chars per column with proper Unicode alignment
    ss << "║    Sun     │    Mon     │    Tue     │    Wed     │    Thu     │    Fri     │    Sat     ║\n";

    // Skip Myanmar weekday headers - using English only

    ss << "╠════════════╪════════════╪════════════╪════════════╪════════════╪════════════╪════════════╣\n";

    // Find first day of month using Swiss Ephemeris for consistency
    double firstJD = swe_julday(monthData.gregorianYear, monthData.gregorianMonth, 1, 12.0, SE_GREG_CAL);
    int firstDayOfWeek = static_cast<int>(firstJD + 1.5) % 7; // 0=Sunday

    // Generate calendar grid
    int currentWeek = 0;
    int dayIndex = 0;

    while (dayIndex < monthData.days.size()) {
        ss << "║";

        for (int weekDay = 0; weekDay < 7; weekDay++) {
            if (currentWeek == 0 && weekDay < firstDayOfWeek) {
                // Empty cells before first day
                ss << "            │";
            } else if (dayIndex < monthData.days.size()) {
                const auto& day = monthData.days[dayIndex];
                ss << formatTraditionalCell(day) << "│";
                dayIndex++;
            } else {
                // Empty cells after last day
                ss << "            │";
            }
        }

        ss << "\n";

        if (dayIndex < monthData.days.size()) {
            ss << "╠════════════╪════════════╪════════════╪════════════╪════════════╪════════════╪════════════╣\n";
        }

        currentWeek++;
    }

    ss << "╚════════════╧════════════╧════════════╧════════════╧════════════╧════════════╧════════════╝\n";

    // Add monthly summary
    ss << generateMonthlySummary(monthData);
    ss << generateMyanmarLegend();

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateModernLayout(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Modern header with more visual elements
    ss << "┌─────────────────────────────────────────────────────────────────────────────────┐\n";
    ss << "│                    🌙✨ " << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " - " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME ✨🌙                    │\n";
    ss << "├─────────────────────────────────────────────────────────────────────────────────┤\n";

    // Modern weekday headers
    ss << "│  SUN  │  MON  │  TUE  │  WED  │  THU  │  FRI  │  SAT  │\n";
    ss << "├───────┼───────┼───────┼───────┼───────┼───────┼───────┤\n";

    // Find first day of month using Swiss Ephemeris for consistency
    double firstJD = swe_julday(monthData.gregorianYear, monthData.gregorianMonth, 1, 12.0, SE_GREG_CAL);
    int firstDayOfWeek = static_cast<int>(firstJD + 1.5) % 7; // 0=Sunday

    // Generate modern calendar grid
    int dayIndex = 0;
    int currentWeek = 0;

    while (dayIndex < monthData.days.size()) {
        ss << "│";

        for (int weekDay = 0; weekDay < 7; weekDay++) {
            if (currentWeek == 0 && weekDay < firstDayOfWeek) {
                ss << "       │";
            } else if (dayIndex < monthData.days.size()) {
                const auto& day = monthData.days[dayIndex];
                ss << formatModernCell(day) << "│";
                dayIndex++;
            } else {
                ss << "       │";
            }
        }

        ss << "\n";

        if (dayIndex < monthData.days.size()) {
            ss << "├───────┼───────┼───────┼───────┼───────┼───────┼───────┤\n";
        }

        currentWeek++;
    }

    ss << "└───────┴───────┴───────┴───────┴───────┴───────┴───────┘\n";

    // Add modern summary
    ss << "\n🎯 Monthly Overview:\n";
    ss << "   Religious Days: " << monthData.sabbathDays << " | ";
    ss << "Auspicious: " << monthData.auspiciousDays << " | ";
    ss << "Festivals: " << monthData.festivalDays << "\n\n";

    ss << generateLegend();

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateCompactLayout(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Compact header
    ss << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " | " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME\n";
    ss << std::string(60, '=') << "\n";

    // Simple weekday headers
    ss << " S   M   T   W   T   F   S\n";
    ss << std::string(27, '-') << "\n";

    // Find first day of month using Swiss Ephemeris for consistency
    double firstJD = swe_julday(monthData.gregorianYear, monthData.gregorianMonth, 1, 12.0, SE_GREG_CAL);
    int firstDayOfWeek = static_cast<int>(firstJD + 1.5) % 7; // 0=Sunday

    // Generate compact grid
    int dayIndex = 0;
    int currentWeek = 0;

    while (dayIndex < monthData.days.size()) {
        for (int weekDay = 0; weekDay < 7; weekDay++) {
            if (currentWeek == 0 && weekDay < firstDayOfWeek) {
                ss << "    ";
            } else if (dayIndex < monthData.days.size()) {
                const auto& day = monthData.days[dayIndex];
                ss << formatCompactCell(day);
                dayIndex++;
            } else {
                ss << "    ";
            }
        }
        ss << "\n";
        currentWeek++;
    }

    ss << std::string(27, '-') << "\n";
    ss << "*Religious !Caution #Festival .Normal\n";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateBlogStyleLayout(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Blog-style header inspired by mmcal.blogspot.com
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "               Myanmar Calendar - " << monthData.gregorianMonthName << " " << monthData.gregorianYear << "\n";
    ss << "               " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n\n";

    // Blog-style calendar grid with more spacing
    ss << "┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐\n";
    ss << "│ SUNDAY  │ MONDAY  │ TUESDAY │ WEDNESD │ THURSDY │ FRIDAY  │ SATRDYY │\n";
    ss << "├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤\n";

    // Find first day of month using Swiss Ephemeris for consistency
    double firstJD = swe_julday(monthData.gregorianYear, monthData.gregorianMonth, 1, 12.0, SE_GREG_CAL);
    int firstDayOfWeek = static_cast<int>(firstJD + 1.5) % 7; // 0=Sunday

    // Generate blog-style grid
    int dayIndex = 0;
    int currentWeek = 0;

    while (dayIndex < monthData.days.size()) {
        ss << "│";

        for (int weekDay = 0; weekDay < 7; weekDay++) {
            if (currentWeek == 0 && weekDay < firstDayOfWeek) {
                ss << "         │";
            } else if (dayIndex < monthData.days.size()) {
                const auto& day = monthData.days[dayIndex];
                ss << formatBlogStyleCell(day) << "│";
                dayIndex++;
            } else {
                ss << "         │";
            }
        }

        ss << "\n";

        if (dayIndex < monthData.days.size()) {
            ss << "├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤\n";
        }

        currentWeek++;
    }

    ss << "└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┘\n\n";

    // Blog-style summary with highlights
    ss << "* MONTH HIGHLIGHTS:\n";
    ss << "───────────────────\n";
    for (const auto& festival : monthData.majorFestivals) {
        ss << "# " << festival << "\n";
    }
    for (const auto& [day, description] : monthData.specialDays) {
        ss << "* " << day << ": " << description << "\n";
    }

    ss << "\n📊 MONTHLY STATISTICS:\n";
    ss << "─────────────────────\n";
    ss << "Buddhist Sabbath Days: " << monthData.sabbathDays << "\n";
    ss << "Auspicious Days: " << monthData.auspiciousDays << "\n";
    ss << "Festival Days: " << monthData.festivalDays << "\n";
    ss << "Days to avoid: " << monthData.inauspiciousDays << "\n";

    return ss.str();
}

/////////////////////////////////////////////////////////////////////////////
// Unicode-aware string formatting utilities
/////////////////////////////////////////////////////////////////////////////

// Calculate display width of a string considering Unicode characters
int MyanmarMonthlyCalendar::getDisplayWidth(const std::string& str) const {
    int width = 0;
    size_t i = 0;
    while (i < str.length()) {
        unsigned char c = str[i];
        if (c < 0x80) {
            // ASCII character
            width += 1;
            i += 1;
        } else if (c < 0xC0) {
            // Invalid UTF-8 continuation byte, skip
            i += 1;
        } else if (c < 0xE0) {
            // 2-byte UTF-8 character
            width += 1; // Most 2-byte chars are single width
            i += 2;
        } else if (c < 0xF0) {
            // 3-byte UTF-8 character (includes Myanmar script and some emojis)
            if (c >= 0xE1 && i + 2 < str.length() &&
                (unsigned char)str[i+1] >= 0x80 && (unsigned char)str[i+1] <= 0xAF) {
                // Myanmar Unicode range U+1000-U+109F
                width += 2; // Myanmar characters are wide
            } else {
                width += 1; // Other 3-byte chars (some emojis, symbols)
            }
            i += 3;
        } else {
            // 4-byte UTF-8 character (most emojis)
            width += 2; // Emojis are typically double-width
            i += 4;
        }
    }
    return width;
}

// Pad string to exact display width considering Unicode characters
std::string MyanmarMonthlyCalendar::padToDisplayWidth(const std::string& str, int targetWidth, bool rightAlign) const {
    int currentWidth = getDisplayWidth(str);
    int padding = targetWidth - currentWidth;

    if (padding <= 0) {
        return str; // String is already at or exceeds target width
    }

    std::string paddingStr(padding, ' ');
    if (rightAlign) {
        return paddingStr + str;
    } else {
        return str + paddingStr;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Cell Formatting Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::formatTraditionalCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: " DD/mm ⭐SP" with exact 12 display characters
    ss << " ";
    ss << std::setw(2) << std::right << day.gregorianDay;
    ss << "/";
    ss << std::setw(2) << std::setfill('0') << day.myanmarDay << std::setfill(' ');
    ss << " ";

    // Add quality indicator (now using simple ASCII characters)
    std::string indicator;
    if (day.qualityIndicator == "*") indicator = "*"; // 1 display width
    else if (day.qualityIndicator == "!") indicator = "!"; // 1 display width
    else if (day.qualityIndicator == "#") indicator = "#"; // 1 display width
    else indicator = " "; // 1 display width

    ss << indicator;

    // Add Myanmar astrological indicators (ASCII, 1 char each)
    if (day.isSabbath) ss << "S";
    else if (day.isPyathada) ss << "P";
    else if (day.isYatyaza) ss << "Y";
    else if (day.isThamanyo) ss << "T";
    else ss << " ";

    // Pad to exactly 12 display characters
    std::string result = ss.str();
    return padToDisplayWidth(result, 12, false);
}

std::string MyanmarMonthlyCalendar::formatModernCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: " DD ⭐M"  (7 chars)
    ss << std::setw(2) << std::right << day.gregorianDay << " ";

    // Quality indicator (using simple ASCII characters)
    if (day.qualityIndicator == "*") ss << "*";
    else if (day.qualityIndicator == "!") ss << "!";
    else if (day.qualityIndicator == "#") ss << "#";
    else ss << " ";

    // Moon phase indicator
    ss << getMoonPhaseSymbol(day.moonPhase);

    // Padding
    ss << " ";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::formatCompactCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: "DD*"  (4 chars including space)
    ss << std::setw(2) << std::right << day.gregorianDay;

    if (day.qualityIndicator == "*") ss << "*";
    else if (day.qualityIndicator == "!") ss << "!";
    else if (day.qualityIndicator == "#") ss << "#";
    else ss << " ";

    ss << " ";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::formatBlogStyleCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: " DD    *"  (9 chars)
    ss << std::setw(2) << std::right << day.gregorianDay;

    // Add spaces and indicators
    ss << "   ";

    if (day.qualityIndicator == "*") ss << "*";
    else if (day.qualityIndicator == "!") ss << "!";
    else if (day.qualityIndicator == "#") ss << "#";
    else ss << " ";

    // Myanmar indicators
    if (day.isSabbath) ss << "S";
    else if (day.isYatyaza) ss << "Y";
    else if (day.isPyathada) ss << "P";
    else ss << " ";

    ss << " ";

    return ss.str();
}

/////////////////////////////////////////////////////////////////////////////
// Helper Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::getMoonPhaseSymbol(MyanmarMoonPhase phase) const {
    switch (phase) {
        case MyanmarMoonPhase::NEW_MOON: return "●";
        case MyanmarMoonPhase::WAXING: return "◐";
        case MyanmarMoonPhase::FULL_MOON: return "○";
        case MyanmarMoonPhase::WANING: return "◑";
        default: return "·";
    }
}

std::string MyanmarMonthlyCalendar::getWeekdaySymbol(MyanmarWeekday weekday) const {
    switch (weekday) {
        case MyanmarWeekday::SUNDAY: return "☀";
        case MyanmarWeekday::MONDAY: return "☽";
        case MyanmarWeekday::TUESDAY: return "♂";
        case MyanmarWeekday::WEDNESDAY: return "☿";
        case MyanmarWeekday::THURSDAY: return "♃";
        case MyanmarWeekday::FRIDAY: return "♀";
        case MyanmarWeekday::SATURDAY: return "♄";
        default: return "·";
    }
}

std::string MyanmarMonthlyCalendar::generateMonthlySummary(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    ss << "\n* MONTHLY SUMMARY:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "* Buddhist Sabbath Days: " << monthData.sabbathDays << "\n";
    ss << "* Auspicious Days: " << monthData.auspiciousDays << "\n";
    ss << "!  Inauspicious Days: " << monthData.inauspiciousDays << "\n";
    ss << "# Festival Days: " << monthData.festivalDays << "\n\n";

    if (!monthData.majorFestivals.empty()) {
        ss << "* MAJOR FESTIVALS:\n";
        for (const auto& festival : monthData.majorFestivals) {
            ss << "   • " << festival << "\n";
        }
        ss << "\n";
    }

    // Add Julian Day reference footer
    ss << "* JULIAN DAY REFERENCE (Complete Values):\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";

    // Display Julian Days in rows of 7 (weekly format)
    for (size_t i = 0; i < monthData.days.size(); i += 7) {
        ss << "Days ";

        // Day range for this row
        int startDay = i + 1;
        int endDay = std::min(static_cast<int>(i + 7), static_cast<int>(monthData.days.size()));

        if (startDay < 10) ss << " ";
        ss << startDay << "-";
        if (endDay < 10) ss << " ";
        ss << endDay << ": ";

        // Julian days for this week
        for (size_t j = i; j < std::min(i + 7, monthData.days.size()); j++) {
            ss << static_cast<int>(monthData.days[j].julianDay);
            if (j < std::min(i + 7, monthData.days.size()) - 1) {
                ss << " ";
            }
        }
        ss << "\n";
    }

    ss << "Note: Julian Day is the continuous count of days since beginning of Julian period\n";
    ss << "      Used for astronomical calculations and date conversions\n\n";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateMyanmarLegend() const {
    std::stringstream ss;

    ss << "* MYANMAR CALENDAR LEGEND:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "* Religious/Excellent Day    !  Inauspicious Day    # Festival Day    . Normal Day\n";
    ss << "S = Sabbath    P = Pyathada    Y = Yatyaza    T = Thamanyo\n";
    ss << "● New Moon    ◐ Waxing    ○ Full Moon    ◑ Waning\n";
    ss << "☀ Sun ☽ Mon ♂ Tue ☿ Wed ♃ Thu ♀ Fri ♄ Sat\n\n";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateLegend() const {
    std::stringstream ss;

    ss << "Legend: *Religious !Caution #Festival .Normal | ●◐○◑ Moon Phases\n";

    return ss.str();
}

void MyanmarMonthlyCalendar::calculateMonthlyStatistics(MyanmarMonthlyData& monthData) const {
    monthData.sabbathDays = 0;
    monthData.auspiciousDays = 0;
    monthData.inauspiciousDays = 0;
    monthData.festivalDays = 0;

    for (const auto& day : monthData.days) {
        if (day.isSabbath) monthData.sabbathDays++;
        if (day.isYatyaza) monthData.auspiciousDays++;
        if (day.isPyathada || day.isThamanyo) monthData.inauspiciousDays++;
        if (day.isHoliday) monthData.festivalDays++;
    }
}

void MyanmarMonthlyCalendar::identifySpecialDays(MyanmarMonthlyData& monthData) const {
    for (const auto& day : monthData.days) {
        for (const auto& festival : day.festivals) {
            if (festival.find("Full Moon") != std::string::npos ||
                festival.find("New Moon") != std::string::npos ||
                festival.find("Thingyan") != std::string::npos) {
                monthData.majorFestivals.push_back(festival);
                monthData.specialDays[day.gregorianDay] = festival;
            }
        }

        if (day.isSabbath && day.moonPhase == MyanmarMoonPhase::FULL_MOON) {
            monthData.specialDays[day.gregorianDay] = "Full Moon Sabbath";
        }
    }

    // Remove duplicates
    std::sort(monthData.majorFestivals.begin(), monthData.majorFestivals.end());
    monthData.majorFestivals.erase(std::unique(monthData.majorFestivals.begin(), monthData.majorFestivals.end()),
                                  monthData.majorFestivals.end());
}

/////////////////////////////////////////////////////////////////////////////
// Tabulate-based Layout Generation Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::generateTabulateLayout(const MyanmarMonthlyData& monthData) const {
    try {
        std::stringstream ss;

        // Simple test without complex tabulate features
        ss << "Myanmar Monthly Calendar - " << monthData.gregorianMonthName << " " << monthData.gregorianYear << "\n\n";

        // Simple text-based calendar
        ss << "Sun\tMon\tTue\tWed\tThu\tFri\tSat\n";
        ss << "---\t---\t---\t---\t---\t---\t---\n";

        // Calculate first day of week for the month
        int firstDayOfWeek = 0;
        if (!monthData.days.empty()) {
            double jd = monthData.days[0].julianDay;
            firstDayOfWeek = static_cast<int>(jd + 1.5) % 7;
        }

        // Add empty cells for days before the first day of the month
        for (int i = 0; i < firstDayOfWeek; i++) {
            ss << "\t";
        }

        // Add each day
        for (size_t i = 0; i < monthData.days.size(); i++) {
            const auto& day = monthData.days[i];
            ss << day.gregorianDay;

            if ((i + firstDayOfWeek) % 7 == 6) {  // Saturday, end of week
                ss << "\n";
            } else {
                ss << "\t";
            }
        }

        return ss.str();
    } catch (const std::exception& e) {
        return "Error generating tabulate layout: " + std::string(e.what());
    } catch (...) {
        return "Unknown error in tabulate layout generation";
    }
}

std::string MyanmarMonthlyCalendar::generateTabulateModern(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Create main calendar table
    tabulate::Table calendarTable = createCalendarTable(monthData);
    styleCalendarTable(calendarTable, "modern");

    // Modern header
    ss << "┌─────────────────────────────────────────────────────────────────────────────────┐\n";
    ss << "│                    🌙✨ " << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " - " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME ✨🌙                    │\n";
    ss << "└─────────────────────────────────────────────────────────────────────────────────┘\n\n";

    // Add the calendar table
    ss << calendarTable << "\n\n";

    // Modern summary
    ss << "🎯 Monthly Overview:\n";
    ss << "   Religious Days: " << monthData.sabbathDays << " | ";
    ss << "Auspicious: " << monthData.auspiciousDays << " | ";
    ss << "Festivals: " << monthData.festivalDays << "\n\n";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateTabulateClassic(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Create main calendar table
    tabulate::Table calendarTable = createCalendarTable(monthData);
    styleCalendarTable(calendarTable, "classic");

    // Classic header
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "               Myanmar Calendar - " << monthData.gregorianMonthName << " " << monthData.gregorianYear << "\n";
    ss << "               " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n\n";

    // Add the calendar table
    ss << calendarTable << "\n\n";

    // Classic summary with highlights
    ss << "* MONTH HIGHLIGHTS:\n";
    ss << "───────────────────\n";
    for (const auto& festival : monthData.majorFestivals) {
        ss << "# " << festival << "\n";
    }
    for (const auto& [day, description] : monthData.specialDays) {
        ss << "* " << day << ": " << description << "\n";
    }

    ss << "\n* MONTHLY STATISTICS:\n";
    ss << "─────────────────────\n";
    ss << "Buddhist Sabbath Days: " << monthData.sabbathDays << "\n";
    ss << "Auspicious Days: " << monthData.auspiciousDays << "\n";
    ss << "Festival Days: " << monthData.festivalDays << "\n";
    ss << "Days to avoid: " << monthData.inauspiciousDays << "\n";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateTabulateMinimal(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Create main calendar table
    tabulate::Table calendarTable = createCalendarTable(monthData);
    styleCalendarTable(calendarTable, "minimal");

    // Minimal header
    ss << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " | " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME\n";
    ss << std::string(60, '=') << "\n\n";

    // Add the calendar table
    ss << calendarTable << "\n\n";

    ss << "*Religious !Caution #Festival .Normal\n";

    return ss.str();
}

/////////////////////////////////////////////////////////////////////////////
// Tabulate Helper Methods
/////////////////////////////////////////////////////////////////////////////

tabulate::Table MyanmarMonthlyCalendar::createCalendarTable(const MyanmarMonthlyData& monthData) const {
    using namespace tabulate;

    Table table;

    // Add header row with weekdays
    table.add_row({"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"});

    // Calculate first day of month (0=Sunday)
    int firstDayOfWeek = 0;
    if (!monthData.days.empty()) {
        // Use the first day's data to determine the start day
        double jd = monthData.days[0].julianDay;
        firstDayOfWeek = static_cast<int>(jd + 1.5) % 7;
    }

    // Generate calendar rows
    int dayIndex = 0;
    int currentWeek = 0;

    while (dayIndex < monthData.days.size() || currentWeek == 0) {
        tabulate::Table::Row_t currentRow;
        for (int weekDay = 0; weekDay < 7; weekDay++) {
            if (currentWeek == 0 && weekDay < firstDayOfWeek) {
                // Empty cells before first day
                currentRow.push_back("");
            } else if (dayIndex < monthData.days.size()) {
                const auto& day = monthData.days[dayIndex];
                currentRow.push_back(formatCellContent(day, "tabulate"));
                dayIndex++;
            } else {
                // Empty cells after last day
                currentRow.push_back("");
            }
        }

        table.add_row(currentRow);
        currentWeek++;

        // Exit if we've added all days
        if (dayIndex >= monthData.days.size()) {
            break;
        }
    }

    return table;
}

void MyanmarMonthlyCalendar::styleCalendarTable(tabulate::Table& table, const std::string& style) const {
    using namespace tabulate;

    if (style == "modern") {
        table.format()
            .font_style({FontStyle::bold})
            .border_top("─")
            .border_bottom("─")
            .border_left("│")
            .border_right("│")
            .corner("┼");

        // Style header row
        for (size_t i = 0; i < 7; ++i) {
            table[0][i].format()
                .font_color(Color::cyan)
                .font_style({FontStyle::bold})
                .font_align(FontAlign::center);
        }
    } else if (style == "classic") {
        table.format()
            .font_style({FontStyle::bold})
            .border_top("═")
            .border_bottom("═")
            .border_left("║")
            .border_right("║")
            .corner("╬");

        // Style header row
        for (size_t i = 0; i < 7; ++i) {
            table[0][i].format()
                .font_color(Color::yellow)
                .font_style({FontStyle::bold})
                .font_align(FontAlign::center);
        }
    } else if (style == "minimal") {
        table.format()
            .border_top("")
            .border_bottom("")
            .border_left("")
            .border_right("")
            .corner("")
            .column_separator("  ");

        // Style header row
        for (size_t i = 0; i < 7; ++i) {
            table[0][i].format()
                .font_style({FontStyle::underline})
                .font_align(FontAlign::center);
        }
    } else { // default
        table.format()
            .font_style({FontStyle::bold})
            .border_top("─")
            .border_bottom("─")
            .border_left("│")
            .border_right("│")
            .corner("┼");

        // Style header row
        for (size_t i = 0; i < 7; ++i) {
            table[0][i].format()
                .font_color(Color::blue)
                .font_style({FontStyle::bold})
                .font_align(FontAlign::center);
        }
    }

    // Style all cells to center align with proper width
    for (size_t i = 0; i < table.size(); ++i) {
        for (size_t j = 0; j < table[i].size(); ++j) {
            if (i == 0) { // Header row
                table[i][j].format()
                    .font_align(FontAlign::center)
                    .width(10);
            } else { // Data rows
                table[i][j].format()
                    .font_align(FontAlign::center)
                    .width(10)
                    .padding_left(1)
                    .padding_right(1);
            }
        }
    }
}

std::string MyanmarMonthlyCalendar::formatCellContent(const MyanmarMonthlyData::DayData& day, const std::string& format) const {
    std::stringstream ss;

    if (format == "tabulate") {
        // Fixed-width ASCII format: exactly 8 characters per cell
        // Format: "DD ME:DD" (7 chars) + quality (1 char) = 8 chars total
        ss << std::setw(2) << std::setfill('0') << day.gregorianDay;
        ss << " ME:";
        ss << std::setw(2) << std::setfill('0') << day.myanmarDay;

        // Quality indicator (single ASCII character)
        if (day.qualityIndicator == "*") ss << "*";
        else if (day.qualityIndicator == "!") ss << "!";
        else if (day.qualityIndicator == "#") ss << "#";
        else ss << " ";

        // NO newlines - keep everything on single line
        // Myanmar day indicators as suffix characters
        std::string result = ss.str();
        if (day.isYatyaza) result += "Y";
        if (day.isSabbath) result += "S";
        if (day.isPyathada) result += "P";

        return result;

    } else {
        // Default format - just the day number
        ss << day.gregorianDay;
    }

    return ss.str();
}

tabulate::Table MyanmarMonthlyCalendar::createSummaryTable(const MyanmarMonthlyData& monthData) const {
    using namespace tabulate;

    Table table;

    // Add title
    table.add_row({"📊 MONTHLY SUMMARY"});
    table[0][0].format().font_align(FontAlign::center).font_style({FontStyle::bold});

    // Add statistics
    table.add_row({"Buddhist Sabbath Days", std::to_string(monthData.sabbathDays)});
    table.add_row({"Auspicious Days", std::to_string(monthData.auspiciousDays)});
    table.add_row({"Inauspicious Days", std::to_string(monthData.inauspiciousDays)});
    table.add_row({"Festival Days", std::to_string(monthData.festivalDays)});

    // Style the table
    table.format()
        .border_top("─")
        .border_bottom("─")
        .border_left("│")
        .border_right("│")
        .corner("┼");

    return table;
}

/////////////////////////////////////////////////////////////////////////////
// Export Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::generateJSON(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    ss << "{\n";
    ss << "  \"calendar\": {\n";
    ss << "    \"type\": \"myanmar_monthly\",\n";
    ss << "    \"gregorian\": {\n";
    ss << "      \"year\": " << monthData.gregorianYear << ",\n";
    ss << "      \"month\": " << monthData.gregorianMonth << ",\n";
    ss << "      \"monthName\": \"" << monthData.gregorianMonthName << "\"\n";
    ss << "    },\n";
    ss << "    \"myanmar\": {\n";
    ss << "      \"year\": " << monthData.myanmarYear << ",\n";
    ss << "      \"monthName\": \"" << monthData.myanmarMonthName << "\"\n";
    ss << "    },\n";
    ss << "    \"statistics\": {\n";
    ss << "      \"sabbathDays\": " << monthData.sabbathDays << ",\n";
    ss << "      \"auspiciousDays\": " << monthData.auspiciousDays << ",\n";
    ss << "      \"inauspiciousDays\": " << monthData.inauspiciousDays << ",\n";
    ss << "      \"festivalDays\": " << monthData.festivalDays << "\n";
    ss << "    },\n";
    ss << "    \"days\": [\n";

    for (size_t i = 0; i < monthData.days.size(); ++i) {
        const auto& day = monthData.days[i];
        if (i > 0) ss << ",\n";

        ss << "      {\n";
        ss << "        \"gregorianDay\": " << day.gregorianDay << ",\n";
        ss << "        \"myanmarDay\": " << day.myanmarDay << ",\n";
        ss << "        \"weekday\": \"" << myanmarCalendar.getMyanmarWeekdayName(day.weekday) << "\",\n";
        ss << "        \"moonPhase\": \"" << myanmarCalendar.getMoonPhaseName(day.moonPhase) << "\",\n";
        ss << "        \"qualityIndicator\": \"" << day.qualityIndicator << "\",\n";
        ss << "        \"astrological\": {\n";
        ss << "          \"sabbath\": " << (day.isSabbath ? "true" : "false") << ",\n";
        ss << "          \"yatyaza\": " << (day.isYatyaza ? "true" : "false") << ",\n";
        ss << "          \"pyathada\": " << (day.isPyathada ? "true" : "false") << ",\n";
        ss << "          \"thamanyo\": " << (day.isThamanyo ? "true" : "false") << "\n";
        ss << "        },\n";
        ss << "        \"festivals\": [";
        for (size_t j = 0; j < day.festivals.size(); ++j) {
            if (j > 0) ss << ", ";
            ss << "\"" << day.festivals[j] << "\"";
        }
        ss << "]\n";
        ss << "      }";
    }

    ss << "\n    ]\n";
    ss << "  }\n";
    ss << "}";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateCSV(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // CSV header
    ss << "GregorianDay,MyanmarDay,Weekday,MoonPhase,Sabbath,Yatyaza,Pyathada,Thamanyo,Festivals,QualityIndicator\n";

    for (const auto& day : monthData.days) {
        ss << day.gregorianDay << ",";
        ss << day.myanmarDay << ",";
        ss << myanmarCalendar.getMyanmarWeekdayName(day.weekday) << ",";
        ss << myanmarCalendar.getMoonPhaseName(day.moonPhase) << ",";
        ss << (day.isSabbath ? "Yes" : "No") << ",";
        ss << (day.isYatyaza ? "Yes" : "No") << ",";
        ss << (day.isPyathada ? "Yes" : "No") << ",";
        ss << (day.isThamanyo ? "Yes" : "No") << ",";

        // Festivals (quoted and semicolon-separated)
        ss << "\"";
        for (size_t i = 0; i < day.festivals.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << day.festivals[i];
        }
        ss << "\",";

        ss << day.qualityIndicator << "\n";
    }

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateHTML(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n<head>\n";
    ss << "<title>Myanmar Calendar - " << monthData.gregorianMonthName << " " << monthData.gregorianYear << "</title>\n";
    ss << "<style>\n";
    ss << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    ss << "table { border-collapse: collapse; width: 100%; max-width: 800px; }\n";
    ss << "th, td { border: 1px solid #ddd; padding: 10px; text-align: center; }\n";
    ss << "th { background-color: #f2f2f2; }\n";
    ss << ".sabbath { background-color: #e6f3ff; }\n";
    ss << ".auspicious { background-color: #e6ffe6; }\n";
    ss << ".inauspicious { background-color: #ffe6e6; }\n";
    ss << ".festival { background-color: #fff2e6; }\n";
    ss << "</style>\n";
    ss << "</head>\n<body>\n";

    ss << "<h1>Myanmar Calendar</h1>\n";
    ss << "<h2>" << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " | " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME</h2>\n";

    ss << "<table>\n";
    ss << "<tr><th>Sun</th><th>Mon</th><th>Tue</th><th>Wed</th><th>Thu</th><th>Fri</th><th>Sat</th></tr>\n";

    // Find first day of month using Swiss Ephemeris for consistency
    double firstJD = swe_julday(monthData.gregorianYear, monthData.gregorianMonth, 1, 12.0, SE_GREG_CAL);
    int firstDayOfWeek = static_cast<int>(firstJD + 1.5) % 7; // 0=Sunday

    // Generate HTML table rows
    int dayIndex = 0;
    int currentWeek = 0;

    while (dayIndex < monthData.days.size()) {
        ss << "<tr>";

        for (int weekDay = 0; weekDay < 7; weekDay++) {
            if (currentWeek == 0 && weekDay < firstDayOfWeek) {
                ss << "<td></td>";
            } else if (dayIndex < monthData.days.size()) {
                const auto& day = monthData.days[dayIndex];

                std::string cssClass = "";
                if (day.isSabbath) cssClass = "sabbath";
                else if (day.isYatyaza) cssClass = "auspicious";
                else if (day.isPyathada || day.isThamanyo) cssClass = "inauspicious";
                else if (day.isHoliday) cssClass = "festival";

                ss << "<td class=\"" << cssClass << "\">";
                ss << "<strong>" << day.gregorianDay << "</strong><br>";
                ss << "ME " << day.myanmarDay << "<br>";
                ss << day.qualityIndicator << " " << getMoonPhaseSymbol(day.moonPhase);
                ss << "</td>";

                dayIndex++;
            } else {
                ss << "<td></td>";
            }
        }

        ss << "</tr>\n";
        currentWeek++;
    }

    ss << "</table>\n";

    // Add legend
    ss << "<h3>Legend:</h3>\n";
    ss << "<p>* Religious Day | ! Inauspicious | # Festival | . Normal</p>\n";
    ss << "<p>● New Moon | ◐ Waxing | ○ Full Moon | ◑ Waning</p>\n";

    ss << "</body>\n</html>";

    return ss.str();
}

/////////////////////////////////////////////////////////////////////////////
// Utility Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::getFormatDescription(const std::string& format) {
    if (format == "traditional") {
        return "Traditional Myanmar calendar with Myanmar script and detailed astrological information";
    } else if (format == "modern") {
        return "Modern layout with visual elements and easy-to-read formatting";
    } else if (format == "compact") {
        return "Compact view suitable for quick reference";
    } else if (format == "blog-style") {
        return "Blog-style layout inspired by mmcal.blogspot.com with highlights";
    } else if (format == "tabulate") {
        return "Beautiful table layout using tabulate library with default styling";
    } else if (format == "tabulate-modern") {
        return "Modern table layout with contemporary styling and colors";
    } else if (format == "tabulate-classic") {
        return "Classic table layout with traditional borders and styling";
    } else if (format == "tabulate-minimal") {
        return "Minimal table layout with clean, simple appearance";
    } else if (format == "json") {
        return "JSON format for programming and web applications";
    } else if (format == "csv") {
        return "CSV format for spreadsheets and data analysis";
    } else if (format == "html") {
        return "HTML format for web display with CSS styling";
    } else if (format == "multi-calendar") {
        return "Multi-calendar view showing Myanmar, Gregorian, and Hindu calendars";
    } else if (format == "planetary") {
        return "Planetary calendar focusing on astronomical events and transitions";
    } else if (format == "hindu-myanmar") {
        return "Combined Hindu and Myanmar calendar with Vedic astrology";
    } else if (format == "full-astronomical") {
        return "Complete astronomical calendar with all planetary and celestial data";
    } else {
        return "Unknown format";
    }
}

/////////////////////////////////////////////////////////////////////////////
// Multi-Calendar Data Calculation Methods
/////////////////////////////////////////////////////////////////////////////

void MyanmarMonthlyCalendar::calculateGregorianData(MyanmarMonthlyData::DayData& dayData, int year, int month, int day) const {
    dayData.gregorian.day = day;
    dayData.gregorian.month = month;
    dayData.gregorian.year = year;

    // Calculate day of week
    std::tm timeinfo = {};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    std::mktime(&timeinfo);

    const char* dayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    const char* monthNames[] = {"January", "February", "March", "April", "May", "June",
                                "July", "August", "September", "October", "November", "December"};

    dayData.gregorian.dayOfWeek = dayNames[timeinfo.tm_wday];
    dayData.gregorian.monthName = monthNames[month - 1];
    dayData.gregorian.isWeekend = (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6);

    // Add some common holidays (can be expanded)
    if (month == 1 && day == 1) {
        dayData.gregorian.holidays.push_back("New Year's Day");
    } else if (month == 12 && day == 25) {
        dayData.gregorian.holidays.push_back("Christmas Day");
    }
}

void MyanmarMonthlyCalendar::calculateHinduCalendarData(MyanmarMonthlyData::DayData& dayData, double julianDay) const {
    try {
        // Initialize all values with defaults first
        dayData.hindu.hinduDay = 0;
        dayData.hindu.hinduYear = 0;
        dayData.hindu.hinduMonthName = "Unknown";
        dayData.hindu.paksha = "Unknown";
        dayData.hindu.nakshatraName = "Unknown";
        dayData.hindu.vikramSamvat = 0;
        dayData.hindu.ritu = "Unknown";
        dayData.hindu.isEkadashi = false;
        dayData.hindu.isPurnima = false;
        dayData.hindu.isAmavasya = false;
        dayData.hindu.isAuspiciousDay = false;
        dayData.hindu.isInauspiciousDay = false;
        dayData.hindu.tithiName = "Unknown";
        dayData.hindu.yogaName = "Unknown";
        dayData.hindu.karanaName = "Unknown";
        dayData.hindu.moonSign = "Unknown";
        dayData.hindu.nakshatraPada = 0;

        // Clear collections safely
        dayData.hindu.festivals.clear();
        dayData.hindu.comprehensiveInfo.clear();

        // Now calculate the Panchanga data carefully
        try {
            // Calculate panchanga with default location (Delhi)
            double latitude = 28.6139;   // Delhi latitude
            double longitude = 77.2090;  // Delhi longitude

            // Call calculatePanchanga and manage memory carefully
            PanchangaData panchanga = hinduCalendar.calculatePanchanga(julianDay, latitude, longitude);

            // Copy data safely from panchanga to dayData
            dayData.hindu.tithiName = hinduCalendar.getTithiName(panchanga.tithi);
            dayData.hindu.nakshatraName = hinduCalendar.getNakshatraName(panchanga.nakshatra);
            dayData.hindu.yogaName = hinduCalendar.getYogaName(panchanga.yoga);
            dayData.hindu.karanaName = hinduCalendar.getKaranaName(panchanga.karana);
            dayData.hindu.hinduMonthName = hinduCalendar.getHinduMonthName(panchanga.month);
            dayData.hindu.paksha = panchanga.isShukla ? "Shukla Paksha" : "Krishna Paksha";
            dayData.hindu.moonSign = hinduCalendar.getRashiName(panchanga.moonRashi);
            dayData.hindu.ritu = panchanga.ritu;
            dayData.hindu.hinduDay = panchanga.day;
            dayData.hindu.hinduYear = panchanga.year;
            dayData.hindu.vikramSamvat = panchanga.vikramYear;
            dayData.hindu.nakshatraPada = panchanga.nakshatraPada;

            // Copy boolean flags
            dayData.hindu.isEkadashi = panchanga.isEkadashi;
            dayData.hindu.isPurnima = panchanga.isPurnima;
            dayData.hindu.isAmavasya = panchanga.isAmavasya;
            dayData.hindu.isAuspiciousDay = panchanga.isSarvarthaSiddhi || panchanga.isAmritaSiddhi;
            dayData.hindu.isInauspiciousDay = false; // Can be set based on other criteria

            // Copy festivals safely
            try {
                if (!panchanga.festivals.empty()) {
                    dayData.hindu.festivals.reserve(panchanga.festivals.size());
                    for (const auto& festival : panchanga.festivals) {
                        dayData.hindu.festivals.push_back(festival);
                    }
                }
            } catch (const std::exception& e) {
                // If festival copying fails, just continue without festivals
                dayData.hindu.festivals.clear();
            }

            // Create comprehensive info map
            try {
                dayData.hindu.comprehensiveInfo.clear();
                dayData.hindu.comprehensiveInfo["Tithi"] = dayData.hindu.tithiName;
                dayData.hindu.comprehensiveInfo["Nakshatra"] = dayData.hindu.nakshatraName;
                dayData.hindu.comprehensiveInfo["Yoga"] = dayData.hindu.yogaName;
                dayData.hindu.comprehensiveInfo["Karana"] = dayData.hindu.karanaName;
                dayData.hindu.comprehensiveInfo["Paksha"] = dayData.hindu.paksha;
                dayData.hindu.comprehensiveInfo["Ritu"] = dayData.hindu.ritu;
            } catch (const std::exception& e) {
                dayData.hindu.comprehensiveInfo.clear();
                dayData.hindu.comprehensiveInfo["Status"] = "Hindu calendar data available";
            }

        } catch (const std::exception& e) {
            // If panchanga calculation fails, keep default values
            // Already set above, so nothing more to do
        }

    } catch (const std::exception& e) {
        // Set safe default values if anything fails
        dayData.hindu.tithiName = "Unknown";
        dayData.hindu.nakshatraName = "Unknown";
        dayData.hindu.yogaName = "Unknown";
        dayData.hindu.karanaName = "Unknown";
        dayData.hindu.hinduMonthName = "Unknown";
        dayData.hindu.paksha = "Unknown";
        dayData.hindu.moonSign = "Unknown";
        dayData.hindu.ritu = "Unknown";
        dayData.hindu.hinduDay = 0;
        dayData.hindu.hinduYear = 0;
        dayData.hindu.vikramSamvat = 0;
        dayData.hindu.nakshatraPada = 0;
        dayData.hindu.isAuspiciousDay = false;
        dayData.hindu.isInauspiciousDay = false;
        dayData.hindu.festivals.clear();
        dayData.hindu.comprehensiveInfo.clear();
    }
}

void MyanmarMonthlyCalendar::calculatePlanetaryData(MyanmarMonthlyData::DayData& dayData, double julianDay) const {
    try {
        // Since the AstroCalendar methods are private, we'll use simplified planetary calculations
        // Convert Julian day to Gregorian to use astroCalendar
        int year, month, day;
        AstroCalendar::julianDayToGregorian(julianDay, year, month, day);

        // Calculate astro calendar day which contains planetary information
        AstroCalendarDay astroDay = astroCalendar.calculateAstroCalendarDay(year, month, day);

        // Initialize planetary data with default values
        dayData.planetary.hasRetrograde = false;
        dayData.planetary.hasConjunction = false;
        dayData.planetary.hasEclipse = false;
        dayData.planetary.significantEvents = {};

        // Basic planetary weather assessment based on available data
        if (!astroDay.allFestivals.empty()) {
            dayData.planetary.planetaryWeather = "Favorable - Festival Day";
        } else if (astroDay.auspiciousScore > 7) {
            dayData.planetary.planetaryWeather = "Highly Favorable";
        } else if (astroDay.auspiciousScore > 4) {
            dayData.planetary.planetaryWeather = "Moderately Favorable";
        } else if (astroDay.auspiciousScore > 2) {
            dayData.planetary.planetaryWeather = "Neutral";
        } else {
            dayData.planetary.planetaryWeather = "Challenging";
        }

    } catch (...) {
        dayData.planetary.planetaryWeather = "Unknown";
        dayData.planetary.hasRetrograde = false;
        dayData.planetary.hasConjunction = false;
        dayData.planetary.hasEclipse = false;
        dayData.planetary.significantEvents = {};
    }
}

void MyanmarMonthlyCalendar::calculateMultiCalendarStatistics(MyanmarMonthlyData& monthData) const {
    // Initialize stats
    monthData.stats.weekendDays = 0;
    monthData.stats.holidayDays = 0;
    monthData.stats.ekadashiDays = 0;
    monthData.stats.purnimaCount = 0;
    monthData.stats.amavasyaCount = 0;
    monthData.stats.retrogradeEvents = 0;
    monthData.stats.conjunctionEvents = 0;
    monthData.stats.eclipseEvents = 0;
    monthData.stats.signChangeEvents = 0;

    // Calculate statistics from daily data
    for (const auto& day : monthData.days) {
        if (includeGregorian) {
            if (day.gregorian.isWeekend) monthData.stats.weekendDays++;
            if (!day.gregorian.holidays.empty()) {
                monthData.stats.holidayDays++;
                for (const auto& holiday : day.gregorian.holidays) {
                    monthData.stats.gregorianHolidays.push_back(holiday);
                }
            }
        }

        if (includeHindu) {
            if (day.hindu.isEkadashi) monthData.stats.ekadashiDays++;
            if (day.hindu.isPurnima) monthData.stats.purnimaCount++;
            if (day.hindu.isAmavasya) monthData.stats.amavasyaCount++;

            // Count nakshatra occurrences
            monthData.stats.nakshatraCount[day.hindu.nakshatraName]++;

            // Collect Hindu festivals
            for (const auto& festival : day.hindu.hinduFestivals) {
                monthData.stats.hinduFestivals.push_back(festival);
            }
        }

        if (includePlanetary) {
            if (day.planetary.hasRetrograde) monthData.stats.retrogradeEvents++;
            if (day.planetary.hasConjunction) monthData.stats.conjunctionEvents++;
            if (day.planetary.hasEclipse) monthData.stats.eclipseEvents++;

            // Collect major transitions
            for (const auto& transition : day.planetary.transitions) {
                if (transition.type == TransitionType::SIGN_CHANGE) {
                    monthData.stats.signChangeEvents++;
                }
                monthData.stats.majorTransitions.push_back(transition);
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// Multi-Calendar Output Format Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::generateMultiCalendarView(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    auto repeatStr = [](const std::string& s, size_t count) -> std::string {
        std::string res;
        res.reserve(s.length() * count);
        for (size_t i = 0; i < count; ++i) res += s;
        return res;
    };

    // Header with consistent formatting matching the 118-column table below
    ss << "╔" << repeatStr("═", 116) << "╗\n";
    ss << "║" << std::string(45, ' ') << "🌍 MULTI-CALENDAR VIEW 🌍" << std::string(46, ' ') << "║\n";
    ss << "╠" << repeatStr("═", 116) << "╣\n";
    ss << "║  " << monthData.gregorianMonthName << " " << monthData.gregorianYear << " CE";
    ss << " | " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME";

    if (monthData.includeHindu && !monthData.days.empty() &&
        !monthData.days[0].hindu.hinduMonthName.empty()) {
        ss << " | " << monthData.days[0].hindu.hinduMonthName << " " << monthData.days[0].hindu.hinduYear;
    }

    // Pad to consistent width matching table (116 inner width)
    std::string headerContent = monthData.gregorianMonthName + " " + std::to_string(monthData.gregorianYear) + " CE | " +
                               monthData.myanmarMonthName + " " + std::to_string(monthData.myanmarYear) + " ME";
    if (monthData.includeHindu && !monthData.days.empty() &&
        !monthData.days[0].hindu.hinduMonthName.empty()) {
        headerContent += " | " + monthData.days[0].hindu.hinduMonthName + " " + std::to_string(monthData.days[0].hindu.hinduYear);
    }

    size_t targetLength = 112; // 116 inner width - 4 spaces (2 left, 2 right)
    if (headerContent.length() < targetLength) {
        for (size_t i = headerContent.length(); i < targetLength; i++) {
            ss << " ";
        }
    }
    ss << "  ║\n";
    ss << "╚" << repeatStr("═", 116) << "╝\n\n";

    // Create fixed-width table with wider columns for better data display
    ss << "┌────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┬───────────────┐\n";
    ss << "│Week│      SUN      │      MON      │      TUE      │      WED      │      THU      │      FRI      │      SAT      │\n";
    ss << "├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤\n";

    // Enhanced legend for the new format with wider columns
    ss << "│Fmt │[GG][H##][P]   │[M##] Day      │ Quality/      │ Nakshatra/    │ Myanmar/      │ Moon Phase    │ Festivals/    │\n";
    ss << "│    │Gregorian      │Hindu+Paksha   │ Special       │ Hindu Info    │ Astro Info    │ Indicator     │ Events        │\n";
    ss << "├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤\n";

    // Calculate first day of week
    int firstDayOfWeek = 0;
    if (!monthData.days.empty()) {
        double jd = monthData.days[0].julianDay;
        firstDayOfWeek = static_cast<int>(jd + 1.5) % 7;
    }

    int dayIndex = 0;
    int weekNumber = 1;

    while (dayIndex < static_cast<int>(monthData.days.size()) || weekNumber == 1) {
        // Store the starting day index for this week
        int weekStartDayIndex = dayIndex;

        // Week row with fixed-width cells
        ss << "│ " << std::setw(2) << weekNumber << " │";

        // Track day indices for this week
        std::vector<int> weekDayIndices(7, -1);

        for (int col = 0; col < 7; col++) {
            if (weekNumber == 1 && col < firstDayOfWeek) {
                ss << "               │"; // 15 spaces for empty cell
                weekDayIndices[col] = -1;
            } else if (dayIndex < static_cast<int>(monthData.days.size())) {
                const auto& day = monthData.days[dayIndex];
                std::string cellContent = formatFixedWidthDayCell(day, monthData);
                ss << cellContent << "│";
                weekDayIndices[col] = dayIndex;
                dayIndex++;
            } else {
                ss << "               │"; // 15 spaces for empty cell
                weekDayIndices[col] = -1;
            }
        }
        ss << "\n";

        // Quality row with fixed-width indicators using stored indices
        ss << "│    │";

        for (int col = 0; col < 7; col++) {
            if (weekDayIndices[col] >= 0) {
                const auto& day = monthData.days[weekDayIndices[col]];
                std::string qualityCell = formatFixedWidthQualityCell(day, monthData);
                ss << qualityCell << "│";
            } else {
                ss << "               │"; // 15 spaces for empty cell
            }
        }
        ss << "\n";

        // Add row separator (except for last row)
        if (dayIndex < static_cast<int>(monthData.days.size())) {
            ss << "├────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤\n";
        }

        weekNumber++;

        // Break if we've processed all days and this week is complete
        if (dayIndex >= static_cast<int>(monthData.days.size()) && weekNumber > 1) {
            break;
        }
    }

    ss << "└────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┴───────────────┘\n\n";

    // Enhanced legend with comprehensive Hindu calendar information
    ss << "📅 ENHANCED MULTI-CALENDAR LEGEND:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "Day Format: [GG][H##][P][M##] = Gregorian / Hindu+Paksha / Myanmar\n";
    ss << "           GG = Gregorian day (1-31)\n";
    ss << "           H## = Hindu day (1-15 in each Paksha)\n";
    ss << "           P = Paksha (S=Shukla/Waxing, K=Krishna/Waning)\n";
    ss << "           M## = Myanmar day (1-30)\n\n";

    if (monthData.includeHindu) {
        ss << "🕉️  HINDU CALENDAR INDICATORS:\n";
        ss << "   Special Days: Ek=Ekadashi, Pu=Purnima, Am=Amavasya, Fe=Festival\n";
        ss << "   Nakshatra: First 3 letters of current Nakshatra\n";
        ss << "   Quality: * = Auspicious, ! = Caution, # = Festival, . = Normal\n\n";
    }
    if (monthData.includePlanetary) {
        ss << "Planetary: R=Retrograde C=Conjunction X=Eclipse T=Transit\n";
    }
    ss << "🌙 MOON PHASES: F=Full, W=Waxing, N=New, w=Waning\n";
    ss << "MYANMAR DAYS: Tha=Thamanyo, Pya=Pyathada, Yat=Yatyaza, Sab=Sabbath\n";
    ss << "Quality: * = Auspicious ! = Caution # = Festival . = Normal\n\n";

    // Statistics with consistent formatting
    ss << "📊 MONTHLY STATISTICS:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "Myanmar Calendar: " << monthData.sabbathDays << " Sabbaths, "
       << monthData.auspiciousDays << " Auspicious, " << monthData.inauspiciousDays << " Inauspicious\n";

    if (monthData.includeGregorian) {
        ss << "Gregorian Calendar: " << monthData.stats.weekendDays << " Weekend days, "
           << monthData.stats.holidayDays << " Holidays\n";
    }

    if (monthData.includeHindu) {
        ss << "🕉️  Hindu Calendar: " << monthData.stats.ekadashiDays << " Ekadashis, "
           << monthData.stats.purnimaCount << " Full Moons, "
           << monthData.stats.amavasyaCount << " New Moons\n";

        // Add comprehensive Hindu calendar summary
        ss << "\n🔸 COMPREHENSIVE HINDU CALENDAR SUMMARY:\n";
        ss << "═══════════════════════════════════════════════════════════════════════════════\n";

        if (!monthData.days.empty()) {
            const auto& firstDay = monthData.days[0];

            ss << "📅 Month: " << firstDay.hindu.hinduMonthName << " " << firstDay.hindu.hinduYear << "\n";
            ss << "📅 Vikram Samvat: " << firstDay.hindu.vikramSamvat << "\n";
            ss << "🌱 Season (Ritu): " << firstDay.hindu.ritu << "\n\n";

            // Show special days throughout the month
            ss << "🌟 SPECIAL HINDU DAYS:\n";
            for (size_t i = 0; i < monthData.days.size(); i++) {
                const auto& day = monthData.days[i];
                if (day.hindu.isEkadashi || day.hindu.isPurnima || day.hindu.isAmavasya ||
                    !day.hindu.festivals.empty()) {
                    ss << "   " << std::setw(2) << day.gregorianDay << ": ";

                    if (day.hindu.isEkadashi) ss << "Ekadashi ";
                    if (day.hindu.isPurnima) ss << "Purnima ";
                    if (day.hindu.isAmavasya) ss << "Amavasya ";

                    for (const auto& festival : day.hindu.festivals) {
                        ss << festival << " ";
                    }

                    ss << "(" << day.hindu.paksha << " - Day " << day.hindu.hinduDay << ")\n";
                }
            }

            // Show Nakshatra distribution
            ss << "\n🌌 NAKSHATRA OVERVIEW:\n";
            std::map<std::string, int> nakshatraCount;
            for (const auto& day : monthData.days) {
                nakshatraCount[day.hindu.nakshatraName]++;
            }
            for (const auto& [nakshatra, count] : nakshatraCount) {
                if (count > 0) {
                    ss << "   " << nakshatra << ": " << count << " day(s)\n";
                }
            }

            // Show Paksha progression
            ss << "\n🌙 PAKSHA PROGRESSION:\n";
            int shuklaCount = 0, krishnaCount = 0;
            for (const auto& day : monthData.days) {
                if (day.hindu.paksha == "Shukla Paksha") shuklaCount++;
                else if (day.hindu.paksha == "Krishna Paksha") krishnaCount++;
            }
            ss << "   Shukla Paksha (Waxing): " << shuklaCount << " days\n";
            ss << "   Krishna Paksha (Waning): " << krishnaCount << " days\n";
        }
    }

    if (includePlanetary) {
        ss << "Planetary Events: " << monthData.stats.retrogradeEvents << " Retrogrades, "
           << monthData.stats.conjunctionEvents << " Conjunctions, "
           << monthData.stats.eclipseEvents << " Eclipses\n";
    }

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generatePlanetaryCalendar(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    ss << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                        🪐 PLANETARY CALENDAR 🪐                                 ║\n";
    ss << "║  " << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " - Astronomical Events & Planetary Transitions                          ║\n";
    ss << "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n";

    // Show major planetary events for the month
    ss << "🌟 MAJOR PLANETARY EVENTS:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";

    for (size_t i = 0; i < monthData.days.size(); i++) {
        const auto& day = monthData.days[i];
        if (!day.planetary.significantEvents.empty()) {
            ss << day.gregorianDay << ": ";
            for (const auto& event : day.planetary.significantEvents) {
                ss << event << " ";
            }
            ss << "\n";
        }
    }

    ss << "\n📊 PLANETARY STATISTICS:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "Retrograde Events: " << monthData.stats.retrogradeEvents << "\n";
    ss << "Conjunctions: " << monthData.stats.conjunctionEvents << "\n";
    ss << "Sign Changes: " << monthData.stats.signChangeEvents << "\n";
    ss << "Eclipse Events: " << monthData.stats.eclipseEvents << "\n";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateHinduMyanmarCalendar(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    ss << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                    🕉️ HINDU-MYANMAR CALENDAR 🕉️                              ║\n";
    ss << "║  " << monthData.gregorianMonthName << " " << monthData.gregorianYear << " CE";
    ss << " | " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME";

    if (!monthData.days.empty()) {
        ss << " | " << monthData.days[0].hindu.hinduMonthName << " " << monthData.days[0].hindu.hinduYear;
    }

    ss << "           ║\n";
    ss << "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n";

    // Create detailed Hindu-Myanmar calendar
    ss << "🌙 VEDIC CALENDAR WITH MYANMAR TRADITIONS:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";

    for (const auto& day : monthData.days) {
        ss << std::setw(2) << day.gregorianDay << ": ";
        ss << "Tithi: " << day.hindu.tithiName << " | ";
        ss << "Nakshatra: " << day.hindu.nakshatraName << " | ";
        ss << "Myanmar: " << day.myanmarDay << " " << day.qualityIndicator;

        if (day.hindu.isEkadashi || day.hindu.isPurnima || day.hindu.isAmavasya ||
            day.isSabbath || day.isPyathada || day.isYatyaza) {
            ss << " | Special: ";
            if (day.hindu.isEkadashi) ss << "Ekadashi ";
            if (day.hindu.isPurnima) ss << "Purnima ";
            if (day.hindu.isAmavasya) ss << "Amavasya ";
            if (day.isSabbath) ss << "Sabbath ";
            if (day.isPyathada) ss << "Pyathada ";
            if (day.isYatyaza) ss << "Yatyaza ";
        }
        ss << "\n";
    }

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateFullAstronomicalCalendar(const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    ss << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                   🌌 FULL ASTRONOMICAL CALENDAR 🌌                             ║\n";
    ss << "║              Complete Multi-Cultural Astronomical Reference                      ║\n";
    ss << "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n";

    // Combine all calendar systems and astronomical data
    ss << generateMultiCalendarView(monthData) << "\n";
    ss << generatePlanetaryCalendar(monthData) << "\n";

    // Add additional astronomical details
    ss << "🔬 DETAILED ASTRONOMICAL DATA:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";

    for (const auto& day : monthData.days) {
        if (!day.planetary.transitions.empty() || day.hindu.isEkadashi ||
            day.hindu.isPurnima || day.hindu.isAmavasya) {

            ss << "Day " << day.gregorianDay << " (" << day.gregorian.dayOfWeek << "):\n";
            ss << "  Gregorian: " << day.gregorian.day << " " << day.gregorian.monthName << " " << day.gregorian.year << "\n";
            ss << "  Myanmar: " << day.myanmarDay << " " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME\n";
            ss << "  Hindu: " << day.hindu.hinduDay << " " << day.hindu.hinduMonthName << " " << day.hindu.hinduYear << "\n";
            ss << "  Tithi: " << day.hindu.tithiName << " | Nakshatra: " << day.hindu.nakshatraName << "\n";
            ss << "  Yoga: " << day.hindu.yogaName << " | Karana: " << day.hindu.karanaName << "\n";

            if (!day.planetary.transitions.empty()) {
                ss << "  Planetary Events: ";
                for (const auto& event : day.planetary.significantEvents) {
                    ss << event << " ";
                }
                ss << "\n";
            }

            ss << "\n";
        }
    }

    return ss.str();
}

std::string MyanmarMonthlyCalendar::formatFixedWidthDayCell(const MyanmarMonthlyData::DayData& day, const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Enhanced fixed format: [GG][H##][P][M##] - exactly 15 characters including spaces
    // GG = Gregorian day (2 chars, right-aligned)
    // H## = Hindu day (3 chars if Hindu calendar included)
    // P = Paksha indicator (1 char: S=Shukla, K=Krishna)
    // M## = Myanmar day (3 chars)

    ss << std::setw(2) << std::setfill(' ') << day.gregorianDay;

    if (monthData.includeHindu) {
        ss << " H" << std::setw(2) << std::setfill('0') << day.hindu.hinduDay;

        // Add Paksha indicator (S for Shukla, K for Krishna)
        if (day.hindu.paksha == "Shukla Paksha") {
            ss << "S";
        } else if (day.hindu.paksha == "Krishna Paksha") {
            ss << "K";
        } else {
            ss << " ";
        }
    } else {
        ss << "     "; // 5 spaces for H##P
    }

    ss << " M" << std::setw(2) << std::setfill('0') << day.myanmarDay;

    // Ensure exactly 15 characters
    std::string result = ss.str();
    if (result.length() > 15) {
        result = result.substr(0, 15);
    } else if (result.length() < 15) {
        result += std::string(15 - result.length(), ' ');
    }

    return result;
}

std::string MyanmarMonthlyCalendar::formatFixedWidthQualityCell(const MyanmarMonthlyData::DayData& day, const MyanmarMonthlyData& monthData) const {
    std::stringstream ss;

    // Enhanced fixed format: Quality + Special indicators - exactly 15 characters

    // Quality indicator (1 char)
    if (day.isSabbath || day.isYatyaza || day.hindu.isAuspiciousDay) {
        ss << "*"; // Auspicious
    } else if (day.isPyathada || day.isThamanyo || day.hindu.isInauspiciousDay) {
        ss << "!"; // Caution
    } else if (day.isHoliday || !day.festivals.empty() || !day.hindu.festivals.empty()) {
        ss << "#"; // Festival
    } else {
        ss << "."; // Normal
    }

    // Hindu special days (3 chars for enhanced coverage)
    if (monthData.includeHindu) {
        if (day.hindu.isEkadashi) {
            ss << " Ek"; // Ekadashi
        } else if (day.hindu.isPurnima) {
            ss << " Pu"; // Purnima
        } else if (day.hindu.isAmavasya) {
            ss << " Am"; // Amavasya
        } else if (!day.hindu.festivals.empty()) {
            ss << " Fe"; // Festival
        } else {
            ss << "   "; // Normal
        }
    } else {
        ss << "   ";
    }

    // Nakshatra indicator (3 chars - abbreviated)
    if (monthData.includeHindu && !day.hindu.nakshatraName.empty()) {
        std::string nakshatraAbbrev = day.hindu.nakshatraName.substr(0, 3);
        ss << " " << nakshatraAbbrev;
    } else {
        ss << "    ";
    }

    // Planetary indicators (2 chars)
    if (monthData.includePlanetary) {
        if (day.planetary.hasEclipse) {
            ss << " X";
        } else if (day.planetary.hasConjunction) {
            ss << " C";
        } else if (day.planetary.hasRetrograde) {
            ss << " R";
        } else {
            ss << "  ";
        }
    } else {
        ss << "  ";
    }

    // Myanmar specific indicators (4 chars)
    if (day.isSabbath) {
        ss << " Sab"; // Sabbath
    } else if (day.isThamanyo) {
        ss << " Tha"; // Thamanyo
    } else if (day.isPyathada) {
        ss << " Pya"; // Pyathada
    } else if (day.isYatyaza) {
        ss << " Yat"; // Yatyaza
    } else {
        ss << "    "; // Normal
    }

    // Moon phase indicator (1 char)
    switch (day.moonPhase) {
        case MyanmarMoonPhase::FULL_MOON:
            ss << "F";
            break;
        case MyanmarMoonPhase::WAXING:
            ss << "W";
            break;
        case MyanmarMoonPhase::NEW_MOON:
            ss << "N";
            break;
        case MyanmarMoonPhase::WANING:
            ss << "w";
            break;
        default:
            ss << " ";
            break;
    }

    // Ensure exactly 15 characters
    std::string result = ss.str();
    if (result.length() > 15) {
        result = result.substr(0, 15);
    } else if (result.length() < 15) {
        result += std::string(15 - result.length(), ' ');
    }

    return result;
}

bool MyanmarMonthlyCalendar::isValidFormat(const std::string& format) {
    std::vector<std::string> validFormats = getAvailableFormats();
    return std::find(validFormats.begin(), validFormats.end(), format) != validFormats.end();
}

} // namespace Astro
