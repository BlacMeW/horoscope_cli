/////////////////////////////////////////////////////////////////////////////
// File: myanmar_monthly_calendar.h
// Description: Myanmar Monthly Calendar Display Design
// Reference: Inspired by https://mmcal.blogspot.com/ design
/////////////////////////////////////////////////////////////////////////////

#ifndef MYANMAR_MONTHLY_CALENDAR_H
#define MYANMAR_MONTHLY_CALENDAR_H

#include "myanmar_calendar.h"
#include "astro_calendar.h"
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
};

class MyanmarMonthlyCalendar {
private:
    MyanmarCalendar myanmarCalendar;
    bool initialized;
    std::string lastError;

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

    // Quality and indicator methods
    std::string getDayQualityIndicator(const MyanmarMonthlyData::DayData& day) const;
    std::string getMyanmarSymbols(const MyanmarMonthlyData::DayData& day) const;
    std::string getMoonPhaseSymbol(MyanmarMoonPhase phase) const;
    std::string getWeekdaySymbol(MyanmarWeekday weekday) const;

    // Statistical methods
    void calculateMonthlyStatistics(MyanmarMonthlyData& monthData) const;
    void identifySpecialDays(MyanmarMonthlyData& monthData) const;

    // Tabulate helper methods
    tabulate::Table createCalendarTable(const MyanmarMonthlyData& monthData) const;
    void styleCalendarTable(tabulate::Table& table, const std::string& style) const;
    std::string formatCellContent(const MyanmarMonthlyData::DayData& day, const std::string& format) const;
    tabulate::Table createSummaryTable(const MyanmarMonthlyData& monthData) const;

public:
    MyanmarMonthlyCalendar();
    ~MyanmarMonthlyCalendar();

    bool initialize();

    // Main calendar generation methods
    MyanmarMonthlyData calculateMonthlyData(int year, int month, double latitude = 0.0, double longitude = 0.0) const;

    // Output format methods
    std::string generateMonthlyCalendar(const MyanmarMonthlyData& monthData, const std::string& format = "traditional") const;
    std::string generateTraditionalMyanmarCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateModernMyanmarCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateCompactMyanmarCalendar(const MyanmarMonthlyData& monthData) const;
    std::string generateBlogStyleMyanmarCalendar(const MyanmarMonthlyData& monthData) const;

    // Export methods
    std::string generateJSON(const MyanmarMonthlyData& monthData) const;
    std::string generateCSV(const MyanmarMonthlyData& monthData) const;
    std::string generateHTML(const MyanmarMonthlyData& monthData) const;

    // Utility methods
    std::string getLastError() const { return lastError; }
    bool isInitialized() const { return initialized; }

    // Format list
    static std::vector<std::string> getAvailableFormats() {
        return {"traditional", "modern", "compact", "blog-style", "tabulate", "tabulate-modern", "tabulate-classic", "tabulate-minimal", "json", "csv", "html"};
    }

    // Static utility methods
    static std::string getFormatDescription(const std::string& format);
    static bool isValidFormat(const std::string& format);
};

} // namespace Astro

#endif // MYANMAR_MONTHLY_CALENDAR_H
