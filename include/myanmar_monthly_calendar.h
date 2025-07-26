/////////////////////////////////////////////////////////////////////////////
// File: myanmar_monthly_calendar.h
// Description: Myanmar Monthly Calendar Display Design
// Reference: Inspired by https://mmcal.blogspot.com/ design
/////////////////////////////////////////////////////////////////////////////

#ifndef MYANMAR_MONTHLY_CALENDAR_H
#define MYANMAR_MONTHLY_CALENDAR_H

#include "myanmar_calendar.h"
#include "astro_calendar.h"
#include "hindu_calendar.h"
#include <string>
#include <vector>
#include <map>

// Include tabulate for beautiful table formatting
#include "../../third_party/tabulate/single_include/tabulate/tabulate.hpp"

namespace Astro {

struct MyanmarMonthlyData {
    int gregorianYear;
    int gregorianMonth;
    std::string gregorianMonthName;

    // Include flags to know which calendars are active
    bool includeGregorian;
    bool includeHindu;
    bool includePlanetary;
    bool includeAdvancedAstro;
    long myanmarYear;
    MyanmarMonth myanmarMonth;
    std::string myanmarMonthName;

    struct DayData {
        int gregorianDay;
        long myanmarDay;
        MyanmarWeekday weekday;
        MyanmarMoonPhase moonPhase;
        bool isSabbath;
        bool isYatyaza;
        bool isPyathada;
        bool isThamanyo;
        bool isHoliday;
        std::vector<std::string> festivals;
        std::string qualityIndicator; // ⭐ ✅ ⚠️ 🚫
        double julianDay;

        // Enhanced multi-calendar system data
        struct GregorianData {
            int day;
            int month;
            int year;
            std::string dayOfWeek;
            std::string monthName;
            bool isWeekend;
            std::vector<std::string> holidays;
        } gregorian;

        struct HinduCalendarData {
            // Core Panchanga Elements
            Tithi tithi;
            std::string tithiName;
            HinduNakshatra nakshatra;
            std::string nakshatraName;
            Yoga yoga;
            std::string yogaName;
            Karana karana;
            std::string karanaName;

            // Hindu calendar date information
            HinduMonth hinduMonth;
            std::string hinduMonthName;
            int hinduDay;
            int hinduYear;

            // Enhanced Panchanga data (following DrikPanchang structure)
            std::string paksha;              // Shukla/Krishna Paksha
            std::string moonSign;            // Moon's zodiac sign (Rashi)
            int nakshatraPada;              // Nakshatra pada (1-4)
            int vikramSamvat;               // Vikram Samvat year
            std::string ritu;               // Season (Vasanta, Grishma, etc.)

            // Special days detection
            bool isEkadashi;
            bool isPurnima;
            bool isAmavasya;
            bool isAuspiciousDay;          // General auspicious day marker
            bool isInauspiciousDay;        // General inauspicious day marker

            // Festival and event information
            std::vector<std::string> festivals;
            std::vector<std::string> hinduFestivals;

            // Comprehensive Panchanga information (key-value pairs for detailed display)
            std::map<std::string, std::string> comprehensiveInfo;
        } hindu;

        struct PlanetaryData {
            std::vector<PlanetaryTransition> transitions;
            std::vector<std::string> significantEvents;
            std::string planetaryWeather; // Overall planetary influence
            bool hasRetrograde;
            bool hasConjunction;
            bool hasEclipse;
            std::vector<std::string> aspectsFormed;
        } planetary;
    };

    std::vector<DayData> days;

    // Monthly statistics
    int sabbathDays;
    int auspiciousDays;
    int inauspiciousDays;
    int festivalDays;

    // Monthly highlights
    std::vector<std::string> majorFestivals;
    std::vector<std::string> importantDays;
    std::map<int, std::string> specialDays; // day -> description

    // Multi-calendar monthly summaries
    struct MonthlyStats {
        // Gregorian stats
        int weekendDays;
        int holidayDays;
        std::vector<std::string> gregorianHolidays;

        // Hindu calendar stats
        int ekadashiDays;
        int purnimaCount;
        int amavasyaCount;
        std::vector<std::string> hinduFestivals;
        std::map<std::string, int> nakshatraCount;

        // Planetary stats
        int retrogradeEvents;
        int conjunctionEvents;
        int eclipseEvents;
        int signChangeEvents;
        std::vector<PlanetaryTransition> majorTransitions;
        std::string monthlyPlanetaryTrend;
    } stats;
};

class MyanmarMonthlyCalendar {
private:
    MyanmarCalendar myanmarCalendar;
    HinduCalendar hinduCalendar;
    AstroCalendar astroCalendar;
    bool initialized;
    std::string lastError;

    // Calendar inclusion options
    bool includeGregorian;
    bool includeHindu;
    bool includePlanetary;
    bool includeAdvancedAstro;

    // Helper methods for calendar layout
    std::string generateTraditionalLayout(const MyanmarMonthlyData& monthData) const;
    std::string generateModernLayout(const MyanmarMonthlyData& monthData) const;
    std::string generateCompactLayout(const MyanmarMonthlyData& monthData) const;
    std::string generateBlogStyleLayout(const MyanmarMonthlyData& monthData) const;

    // Tabulate-based beautiful table layouts
    std::string generateTabulateLayout(const MyanmarMonthlyData& monthData) const;
    std::string generateTabulateModern(const MyanmarMonthlyData& monthData) const;
    std::string generateTabulateClassic(const MyanmarMonthlyData& monthData) const;
    std::string generateTabulateMinimal(const MyanmarMonthlyData& monthData) const;

    // Layout helper methods
    std::string generateCalendarHeader(const MyanmarMonthlyData& monthData) const;
    std::string generateWeekdayHeaders() const;
    std::string generateMyanmarWeekdayHeaders() const;
    std::string generateCalendarGrid(const MyanmarMonthlyData& monthData, const std::string& style) const;
    std::string generateMonthlySummary(const MyanmarMonthlyData& monthData) const;
    std::string generateLegend() const;
    std::string generateMyanmarLegend() const;

    // Cell formatting methods
    std::string formatDayCell(const MyanmarMonthlyData::DayData& day, const std::string& style) const;
    std::string formatTraditionalCell(const MyanmarMonthlyData::DayData& day) const;
    std::string formatModernCell(const MyanmarMonthlyData::DayData& day) const;
    std::string formatCompactCell(const MyanmarMonthlyData::DayData& day) const;
    std::string formatBlogStyleCell(const MyanmarMonthlyData::DayData& day) const;

    // Unicode-aware string formatting utilities
    int getDisplayWidth(const std::string& str) const;
    std::string padToDisplayWidth(const std::string& str, int targetWidth, bool rightAlign = false) const;

    // Fixed-width formatting methods for multi-calendar view
    std::string formatFixedWidthDayCell(const MyanmarMonthlyData::DayData& day, const MyanmarMonthlyData& monthData) const;
    std::string formatFixedWidthQualityCell(const MyanmarMonthlyData::DayData& day, const MyanmarMonthlyData& monthData) const;    // Quality and indicator methods
    std::string getDayQualityIndicator(const MyanmarMonthlyData::DayData& day) const;
    std::string getMyanmarSymbols(const MyanmarMonthlyData::DayData& day) const;
    std::string getMoonPhaseSymbol(MyanmarMoonPhase phase) const;
    std::string getWeekdaySymbol(MyanmarWeekday weekday) const;

    // Statistical methods
    void calculateMonthlyStatistics(MyanmarMonthlyData& monthData) const;
    void identifySpecialDays(MyanmarMonthlyData& monthData) const;

    // Multi-calendar data calculation methods
    void calculateGregorianData(MyanmarMonthlyData::DayData& dayData, int year, int month, int day) const;
    void calculateHinduCalendarData(MyanmarMonthlyData::DayData& dayData, double julianDay) const;
    void calculatePlanetaryData(MyanmarMonthlyData::DayData& dayData, double julianDay) const;
    void calculateMultiCalendarStatistics(MyanmarMonthlyData& monthData) const;

    // Tabulate helper methods
    tabulate::Table createCalendarTable(const MyanmarMonthlyData& monthData) const;
    void styleCalendarTable(tabulate::Table& table, const std::string& style) const;
    std::string formatCellContent(const MyanmarMonthlyData::DayData& day, const std::string& format) const;
    tabulate::Table createSummaryTable(const MyanmarMonthlyData& monthData) const;

public:
    MyanmarMonthlyCalendar();
    ~MyanmarMonthlyCalendar();

    bool initialize();

    // Calendar system inclusion controls
    void setIncludeGregorian(bool include) { includeGregorian = include; }
    void setIncludeHindu(bool include) { includeHindu = include; }
    void setIncludePlanetary(bool include) { includePlanetary = include; }
    void setIncludeAdvancedAstro(bool include) { includeAdvancedAstro = include; }

    bool isGregorianIncluded() const { return includeGregorian; }
    bool isHinduIncluded() const { return includeHindu; }
    bool isPlanetaryIncluded() const { return includePlanetary; }
    bool isAdvancedAstroIncluded() const { return includeAdvancedAstro; }

    // Main calendar generation methods
    MyanmarMonthlyData calculateMonthlyData(int year, int month, double latitude = 0.0, double longitude = 0.0,
                                          bool includeGregorian = false, bool includeHindu = false,
                                          bool includePlanetary = false, bool includeAdvancedAstro = false) const;

    // Output format methods
    std::string generateMonthlyCalendar(const MyanmarMonthlyData& monthData, const std::string& format = "traditional") const;
    std::string generateTraditionalMyanmarCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateModernMyanmarCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateCompactMyanmarCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateBlogStyleMyanmarCalendar(const MyanmarMonthlyData& monthData) const;

    // Multi-calendar enhanced formats
    std::string generateMultiCalendarView(const MyanmarMonthlyData& monthData) const;
    std::string generatePlanetaryCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateHinduMyanmarCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateFullAstronomicalCalendar(const MyanmarMonthlyData& monthData) const;

    // Export methods
    std::string generateJSON(const MyanmarMonthlyData& monthData) const;
    std::string generateCSV(const MyanmarMonthlyData& monthData) const;
    std::string generateHTML(const MyanmarMonthlyData& monthData) const;

    // Utility methods
    std::string getLastError() const { return lastError; }
    bool isInitialized() const { return initialized; }

    // Format list
    static std::vector<std::string> getAvailableFormats() {
        return {"traditional", "modern", "compact", "blog-style", "tabulate", "tabulate-modern",
                "tabulate-classic", "tabulate-minimal", "json", "csv", "html",
                "multi-calendar", "planetary", "hindu-myanmar", "full-astronomical"};
    }

    // Static utility methods
    static std::string getFormatDescription(const std::string& format);
    static bool isValidFormat(const std::string& format);
};

} // namespace Astro

#endif // MYANMAR_MONTHLY_CALENDAR_H
