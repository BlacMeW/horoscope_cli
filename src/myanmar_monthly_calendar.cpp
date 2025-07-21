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

MyanmarMonthlyCalendar::MyanmarMonthlyCalendar() : initialized(false) {
}

MyanmarMonthlyCalendar::~MyanmarMonthlyCalendar() {
}

bool MyanmarMonthlyCalendar::initialize() {
    if (!myanmarCalendar.initialize()) {
        lastError = "Failed to initialize Myanmar calendar";
        return false;
    }
    initialized = true;
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Main Calendar Generation Methods
/////////////////////////////////////////////////////////////////////////////

MyanmarMonthlyData MyanmarMonthlyCalendar::calculateMonthlyData(int year, int month, double latitude, double longitude) const {
    MyanmarMonthlyData monthData;

    monthData.gregorianYear = year;
    monthData.gregorianMonth = month;

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

        // Convert to Julian Day using a standard formula
        // This is a simplified Julian Day calculation for Gregorian dates
        int a = (14 - month) / 12;
        int y = year - a;
        int m = month + 12 * a - 3;
        double jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 + 1721119.5;
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
    } else {
        return "Unknown format";
    }
}

bool MyanmarMonthlyCalendar::isValidFormat(const std::string& format) {
    std::vector<std::string> validFormats = getAvailableFormats();
    return std::find(validFormats.begin(), validFormats.end(), format) != validFormats.end();
}

} // namespace Astro
