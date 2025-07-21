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

        // Convert to Julian Day using the standard astronomical formula
        // This formula handles the Gregorian calendar correctly
        int a, b, y, m;
        if (month <= 2) {
            y = year - 1;
            m = month + 12;
        } else {
            y = year;
            m = month;
        }

        a = y / 100;
        b = 2 - a + a / 4;

        double jd = static_cast<int>(365.25 * (y + 4716)) +
                   static_cast<int>(30.6001 * (m + 1)) +
                   day + b - 1524.5;
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
            if (dayData.isSabbath || dayData.isThamanyo) {
                dayData.qualityIndicator = "⭐"; // Excellent/Religious
            } else if (dayData.isPyathada || dayData.isYatyaza) {
                dayData.qualityIndicator = "⚠️"; // Inauspicious
            } else if (dayData.isHoliday) {
                dayData.qualityIndicator = "🎉"; // Festival
            } else {
                dayData.qualityIndicator = "⚪"; // Neutral
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
            dayData.qualityIndicator = "⚪";
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
    ss << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                          🇲🇲 MYANMAR CALENDAR 🇲🇲                                ║\n";
    ss << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║  " << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " CE   |   " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME";

    // Calculate padding
    int used = monthData.gregorianMonthName.length() + std::to_string(monthData.gregorianYear).length() +
               monthData.myanmarMonthName.length() + std::to_string(monthData.myanmarYear).length() + 15;
    int padding = 82 - used;
    ss << std::string(padding, ' ') << "║\n";

    ss << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";

    // Weekday headers (both English and Myanmar)
    ss << "║   Sun   │   Mon   │   Tue   │   Wed   │   Thu   │   Fri   │   Sat   ║\n";
    ss << "║ တနင်္ဂနွေ │ တနင်္လာ │ အင်္ဂါ  │ ဗုဒ္ဓဟူး │ ကြာသပတေး │ သောကြာ │ စနေ    ║\n";
    ss << "╠═════════╪═════════╪═════════╪═════════╪═════════╪═════════╪═════════╣\n";

    // Find first day of month using Julian Day calculation
    int a = (14 - monthData.gregorianMonth) / 12;
    int y = monthData.gregorianYear - a;
    int m = monthData.gregorianMonth + 12 * a - 3;
    double firstJD = 1 + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 + 1721119.5;
    int firstDayOfWeek = static_cast<int>(firstJD + 1.5) % 7; // 0=Sunday

    // Generate calendar grid
    int currentWeek = 0;
    int dayIndex = 0;

    while (dayIndex < monthData.days.size()) {
        ss << "║";

        for (int weekDay = 0; weekDay < 7; weekDay++) {
            if (currentWeek == 0 && weekDay < firstDayOfWeek) {
                // Empty cells before first day
                ss << "         │";
            } else if (dayIndex < monthData.days.size()) {
                const auto& day = monthData.days[dayIndex];
                ss << formatTraditionalCell(day) << "│";
                dayIndex++;
            } else {
                // Empty cells after last day
                ss << "         │";
            }
        }

        ss << "\n";

        if (dayIndex < monthData.days.size()) {
            ss << "╠═════════╪═════════╪═════════╪═════════╪═════════╪═════════╪═════════╣\n";
        }

        currentWeek++;
    }

    ss << "╚═════════╧═════════╧═════════╧═════════╧═════════╧═════════╧═════════╝\n";

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

    // Find first day of month using Julian Day calculation
    int a = (14 - monthData.gregorianMonth) / 12;
    int y = monthData.gregorianYear - a;
    int m = monthData.gregorianMonth + 12 * a - 3;
    double firstJD = 1 + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 + 1721119.5;
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

    // Find first day of month using Julian Day calculation
    int a = (14 - monthData.gregorianMonth) / 12;
    int y = monthData.gregorianYear - a;
    int m = monthData.gregorianMonth + 12 * a - 3;
    double firstJD = 1 + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 + 1721119.5;
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
    ss << "⭐Religious ⚠️Caution 🎉Festival ⚪Normal\n";

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

    // Find first day of month using Julian Day calculation
    int a = (14 - monthData.gregorianMonth) / 12;
    int y = monthData.gregorianYear - a;
    int m = monthData.gregorianMonth + 12 * a - 3;
    double firstJD = 1 + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 + 1721119.5;
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
    ss << "🌟 MONTH HIGHLIGHTS:\n";
    ss << "───────────────────\n";
    for (const auto& festival : monthData.majorFestivals) {
        ss << "🎉 " << festival << "\n";
    }
    for (const auto& [day, description] : monthData.specialDays) {
        ss << "⭐ " << day << ": " << description << "\n";
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
// Cell Formatting Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarMonthlyCalendar::formatTraditionalCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: " DD/mm ⭐"  (9 chars)
    ss << std::setw(2) << std::right << day.gregorianDay;
    ss << "/";
    ss << std::setw(2) << std::setfill('0') << day.myanmarDay << std::setfill(' ');
    ss << " ";

    // Add quality indicator
    if (day.qualityIndicator == "⭐") ss << "⭐";
    else if (day.qualityIndicator == "⚠️") ss << "⚠";
    else if (day.qualityIndicator == "🎉") ss << "🎉";
    else ss << " ";

    // Add Myanmar astrological indicators
    if (day.isSabbath) ss << "S";
    else if (day.isPyathada) ss << "P";
    else if (day.isYatyaza) ss << "Y";
    else ss << " ";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::formatModernCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: " DD ⭐M"  (7 chars)
    ss << std::setw(2) << std::right << day.gregorianDay << " ";

    // Quality indicator
    if (day.qualityIndicator == "⭐") ss << "⭐";
    else if (day.qualityIndicator == "⚠️") ss << "⚠";
    else if (day.qualityIndicator == "🎉") ss << "🎉";
    else ss << " ";

    // Moon phase indicator
    ss << getMoonPhaseSymbol(day.moonPhase);

    // Padding
    ss << " ";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::formatCompactCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: "DD⭐"  (4 chars including space)
    ss << std::setw(2) << std::right << day.gregorianDay;

    if (day.qualityIndicator == "⭐") ss << "⭐";
    else if (day.qualityIndicator == "⚠️") ss << "⚠";
    else if (day.qualityIndicator == "🎉") ss << "🎉";
    else ss << " ";

    ss << " ";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::formatBlogStyleCell(const MyanmarMonthlyData::DayData& day) const {
    std::stringstream ss;

    // Format: " DD    ⭐"  (9 chars)
    ss << std::setw(2) << std::right << day.gregorianDay;

    // Add spaces and indicators
    ss << "   ";

    if (day.qualityIndicator == "⭐") ss << "⭐";
    else if (day.qualityIndicator == "⚠️") ss << "⚠";
    else if (day.qualityIndicator == "🎉") ss << "🎉";
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

    ss << "\n📊 MONTHLY SUMMARY:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "🙏 Buddhist Sabbath Days: " << monthData.sabbathDays << "\n";
    ss << "✅ Auspicious Days: " << monthData.auspiciousDays << "\n";
    ss << "⚠️  Inauspicious Days: " << monthData.inauspiciousDays << "\n";
    ss << "🎉 Festival Days: " << monthData.festivalDays << "\n\n";

    if (!monthData.majorFestivals.empty()) {
        ss << "🌟 MAJOR FESTIVALS:\n";
        for (const auto& festival : monthData.majorFestivals) {
            ss << "   • " << festival << "\n";
        }
        ss << "\n";
    }

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateMyanmarLegend() const {
    std::stringstream ss;

    ss << "🔮 MYANMAR CALENDAR LEGEND:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "⭐ Religious/Excellent Day    ⚠️  Inauspicious Day    🎉 Festival Day    ⚪ Normal Day\n";
    ss << "S = Sabbath    P = Pyathada    Y = Yatyaza    T = Thamanyo\n";
    ss << "● New Moon    ◐ Waxing    ○ Full Moon    ◑ Waning\n";
    ss << "☀ Sun ☽ Mon ♂ Tue ☿ Wed ♃ Thu ♀ Fri ♄ Sat\n\n";

    return ss.str();
}

std::string MyanmarMonthlyCalendar::generateLegend() const {
    std::stringstream ss;

    ss << "Legend: ⭐Religious ⚠️Caution 🎉Festival ⚪Normal | ●◐○◑ Moon Phases\n";

    return ss.str();
}

void MyanmarMonthlyCalendar::calculateMonthlyStatistics(MyanmarMonthlyData& monthData) const {
    monthData.sabbathDays = 0;
    monthData.auspiciousDays = 0;
    monthData.inauspiciousDays = 0;
    monthData.festivalDays = 0;

    for (const auto& day : monthData.days) {
        if (day.isSabbath) monthData.sabbathDays++;
        if (day.isThamanyo) monthData.auspiciousDays++;
        if (day.isPyathada || day.isYatyaza) monthData.inauspiciousDays++;
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
    ss << "🌟 MONTH HIGHLIGHTS:\n";
    ss << "───────────────────\n";
    for (const auto& festival : monthData.majorFestivals) {
        ss << "🎉 " << festival << "\n";
    }
    for (const auto& [day, description] : monthData.specialDays) {
        ss << "⭐ " << day << ": " << description << "\n";
    }

    ss << "\n📊 MONTHLY STATISTICS:\n";
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

    ss << "⭐Religious ⚠️Caution 🎉Festival ⚪Normal\n";

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
        if (day.qualityIndicator == "⭐") ss << "*";
        else if (day.qualityIndicator == "⚠️") ss << "!";
        else if (day.qualityIndicator == "🎉") ss << "F";
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

    ss << "<h1>🇲🇲 Myanmar Calendar</h1>\n";
    ss << "<h2>" << monthData.gregorianMonthName << " " << monthData.gregorianYear;
    ss << " | " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME</h2>\n";

    ss << "<table>\n";
    ss << "<tr><th>Sun</th><th>Mon</th><th>Tue</th><th>Wed</th><th>Thu</th><th>Fri</th><th>Sat</th></tr>\n";

    // Find first day of month using Julian Day calculation
    int a = (14 - monthData.gregorianMonth) / 12;
    int y = monthData.gregorianYear - a;
    int m = monthData.gregorianMonth + 12 * a - 3;
    double firstJD = 1 + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 + 1721119.5;
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
                else if (day.isThamanyo) cssClass = "auspicious";
                else if (day.isPyathada || day.isYatyaza) cssClass = "inauspicious";
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
    ss << "<p>⭐ Religious Day | ⚠️ Inauspicious | 🎉 Festival | ⚪ Normal</p>\n";
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
        PanchangaData panchanga = hinduCalendar.calculatePanchanga(julianDay, 0.0, 0.0);

        // Core Panchanga Elements (as per DrikPanchang structure)
        dayData.hindu.tithi = panchanga.tithi;
        dayData.hindu.tithiName = hinduCalendar.getTithiName(panchanga.tithi);
        dayData.hindu.nakshatra = panchanga.nakshatra;
        dayData.hindu.nakshatraName = hinduCalendar.getNakshatraName(panchanga.nakshatra);
        dayData.hindu.yoga = panchanga.yoga;
        dayData.hindu.yogaName = hinduCalendar.getYogaName(panchanga.yoga);
        dayData.hindu.karana = panchanga.karana;
        dayData.hindu.karanaName = hinduCalendar.getKaranaName(panchanga.karana);

        // Enhanced Hindu calendar date calculation based on Tithi and Paksha system
        // Following DrikPanchang's comprehensive approach
        int tithiValue = static_cast<int>(panchanga.tithi);

        // Calculate Paksha and day within paksha (as per DrikPanchang structure)
        std::string pakshaName;
        int dayInPaksha;

        if (tithiValue <= 14) {
            // Shukla Paksha (Waxing Moon): Tithi 0-14 maps to days 1-15
            pakshaName = "Shukla Paksha";
            dayInPaksha = tithiValue + 1;
            dayData.hindu.hinduDay = dayInPaksha;
        } else {
            // Krishna Paksha (Waning Moon): Tithi 15-29 maps to days 1-15
            pakshaName = "Krishna Paksha";
            dayInPaksha = tithiValue - 14;
            dayData.hindu.hinduDay = dayInPaksha;
        }

        // Store Paksha information
        dayData.hindu.paksha = pakshaName;

        // Hindu month and year from panchanga
        dayData.hindu.hinduMonth = panchanga.month;
        dayData.hindu.hinduYear = panchanga.year;
        dayData.hindu.hinduMonthName = hinduCalendar.getHinduMonthName(panchanga.month);

        // Enhanced special day detection (following DrikPanchang's comprehensive approach)
        dayData.hindu.isEkadashi = (tithiValue == 10 || tithiValue == 25); // 11th day of each Paksha
        dayData.hindu.isPurnima = (tithiValue == 14); // 15th day of Shukla Paksha (Full Moon)
        dayData.hindu.isAmavasya = (tithiValue == 29); // 15th day of Krishna Paksha (New Moon)

        // Additional special days as per DrikPanchang
        bool isDwadashi = (tithiValue == 11 || tithiValue == 26); // 12th day of each Paksha
        bool isTrayodashi = (tithiValue == 12 || tithiValue == 27); // 13th day of each Paksha
        bool isChaturdashi = (tithiValue == 13 || tithiValue == 28); // 14th day of each Paksha
        bool isPratipada = (tithiValue == 0 || tithiValue == 15); // 1st day of each Paksha
        bool isDwitiya = (tithiValue == 1 || tithiValue == 16); // 2nd day of each Paksha
        bool isTritiya = (tithiValue == 2 || tithiValue == 17); // 3rd day of each Paksha
        bool isChaturthi = (tithiValue == 3 || tithiValue == 18); // 4th day of each Paksha (Ganesh Chaturthi)
        bool isPanchami = (tithiValue == 4 || tithiValue == 19); // 5th day of each Paksha
        bool isShashti = (tithiValue == 5 || tithiValue == 20); // 6th day of each Paksha
        bool isSaptami = (tithiValue == 6 || tithiValue == 21); // 7th day of each Paksha
        bool isAshtami = (tithiValue == 7 || tithiValue == 22); // 8th day of each Paksha (Krishna Janmashtami)
        bool isNavami = (tithiValue == 8 || tithiValue == 23); // 9th day of each Paksha
        bool isDashami = (tithiValue == 9 || tithiValue == 24); // 10th day of each Paksha

        // Moon sign and rashi calculations (simplified version of DrikPanchang's approach)
        int moonRashi = static_cast<int>(panchanga.nakshatra) / 2; // Approximate moon sign (simplified)
        std::vector<std::string> rashiNames = {
            "Mesha", "Vrishabha", "Mithuna", "Karka", "Simha", "Kanya",
            "Tula", "Vrishchika", "Dhanu", "Makara", "Kumbha", "Meena"
        };
        if (moonRashi >= 0 && moonRashi < 12) {
            dayData.hindu.moonSign = rashiNames[moonRashi];
        } else {
            dayData.hindu.moonSign = "Unknown";
        }

        // Vikram Samvat calculation (approximate)
        dayData.hindu.vikramSamvat = dayData.hindu.hinduYear + 57; // Approximate conversion

        // Store additional Panchanga elements for comprehensive display
        dayData.hindu.nakshatraPada = (static_cast<int>(panchanga.nakshatra) * 4 / 27) + 1; // Simplified calculation

        // Auspicious and inauspicious time calculations (simplified)
        dayData.hindu.isAuspiciousDay = false;
        dayData.hindu.isInauspiciousDay = false;

        // Mark auspicious days
        if (dayData.hindu.isPurnima || dayData.hindu.isEkadashi || isChaturthi || isAshtami || isNavami) {
            dayData.hindu.isAuspiciousDay = true;
        }

        // Mark inauspicious days (basic implementation)
        if (dayData.hindu.isAmavasya || isChaturdashi) {
            dayData.hindu.isInauspiciousDay = true;
        }

        // Season calculation (Ritu) - simplified
        int currentMonth = static_cast<int>(dayData.hindu.hinduMonth);
        std::vector<std::string> ritus = {
            "Vasanta", "Grishma", "Varsha", "Sharad", "Shishira", "Hemanta"
        };
        dayData.hindu.ritu = ritus[currentMonth % 6];

        // Store comprehensive Hindu calendar information for display
        dayData.hindu.comprehensiveInfo = {
            {"Paksha", pakshaName},
            {"Day in Paksha", std::to_string(dayInPaksha)},
            {"Moon Sign", dayData.hindu.moonSign},
            {"Nakshatra Pada", std::to_string(dayData.hindu.nakshatraPada)},
            {"Vikram Samvat", std::to_string(dayData.hindu.vikramSamvat)},
            {"Ritu", dayData.hindu.ritu}
        };

        // Festival detection based on Tithi and special combinations
        if (dayData.hindu.isEkadashi) {
            if (tithiValue == 10) {
                dayData.hindu.festivals.push_back("Shukla Ekadashi");
            } else {
                dayData.hindu.festivals.push_back("Krishna Ekadashi");
            }
        }

        if (dayData.hindu.isPurnima) {
            dayData.hindu.festivals.push_back("Purnima");
        }

        if (dayData.hindu.isAmavasya) {
            dayData.hindu.festivals.push_back("Amavasya");
        }

        if (isChaturthi && tithiValue == 3) {
            dayData.hindu.festivals.push_back("Ganesh Chaturthi");
        }

        if (isAshtami && tithiValue == 7) {
            dayData.hindu.festivals.push_back("Krishna Janmashtami");
        }

    } catch (...) {
        // Set default values if calculation fails
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

    // Header with consistent formatting
    ss << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                        🌍 MULTI-CALENDAR VIEW 🌍                                 ║\n";
    ss << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║  " << monthData.gregorianMonthName << " " << monthData.gregorianYear << " CE";
    ss << " | " << monthData.myanmarMonthName << " " << monthData.myanmarYear << " ME";

    if (includeHindu && !monthData.days.empty()) {
        ss << " | " << monthData.days[0].hindu.hinduMonthName << " " << monthData.days[0].hindu.hinduYear;
    }

    // Pad to consistent width
    std::string headerContent = monthData.gregorianMonthName + " " + std::to_string(monthData.gregorianYear) + " CE | " +
                               monthData.myanmarMonthName + " " + std::to_string(monthData.myanmarYear) + " ME";
    if (includeHindu && !monthData.days.empty()) {
        headerContent += " | " + monthData.days[0].hindu.hinduMonthName + " " + std::to_string(monthData.days[0].hindu.hinduYear);
    }

    size_t targetLength = 80; // Target width inside borders
    if (headerContent.length() < targetLength) {
        for (size_t i = headerContent.length(); i < targetLength; i++) {
            ss << " ";
        }
    }
    ss << "║\n";
    ss << "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n";

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

    if (includeHindu) {
        ss << "🕉️  HINDU CALENDAR INDICATORS:\n";
        ss << "   Special Days: Ek=Ekadashi, Pu=Purnima, Am=Amavasya, Fe=Festival\n";
        ss << "   Nakshatra: First 3 letters of current Nakshatra\n";
        ss << "   Quality: * = Auspicious, ! = Caution, # = Festival, . = Normal\n\n";
    }
    if (includePlanetary) {
        ss << "Planetary: R=Retrograde C=Conjunction X=Eclipse T=Transit\n";
    }
    ss << "🌙 MOON PHASES: F=Full, W=Waxing, N=New, w=Waning\n";
    ss << "🇲🇲 MYANMAR DAYS: Tha=Thamanyo, Pya=Pyathada, Yat=Yatyaza, Sab=Sabbath\n";
    ss << "Quality: * = Auspicious ! = Caution # = Festival . = Normal\n\n";

    // Statistics with consistent formatting
    ss << "📊 MONTHLY STATISTICS:\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════\n";
    ss << "Myanmar Calendar: " << monthData.sabbathDays << " Sabbaths, "
       << monthData.auspiciousDays << " Auspicious, " << monthData.inauspiciousDays << " Inauspicious\n";

    if (includeGregorian) {
        ss << "Gregorian Calendar: " << monthData.stats.weekendDays << " Weekend days, "
           << monthData.stats.holidayDays << " Holidays\n";
    }

    if (includeHindu) {
        ss << "🕉️  Hindu Calendar: " << monthData.stats.ekadashiDays << " Ekadashis, "
           << monthData.stats.purnimaCount << " Full Moons, "
           << monthData.stats.amavasyaCount << " New Moons\n";

        // Add comprehensive Hindu calendar summary
        ss << "\n🔸 COMPREHENSIVE HINDU CALENDAR SUMMARY:\n";
        ss << "═══════════════════════════════════════════════════════════════════════════════\n";

        if (!monthData.days.empty()) {
            const auto& firstDay = monthData.days[0];
            const auto& lastDay = monthData.days[monthData.days.size() - 1];

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
    if (day.isSabbath || day.isThamanyo || day.hindu.isAuspiciousDay) {
        ss << "*"; // Auspicious
    } else if (day.isPyathada || day.isYatyaza || day.hindu.isInauspiciousDay) {
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
