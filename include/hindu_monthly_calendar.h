#pragma once

#include "hindu_calendar.h"
#include "astro_types.h"
#include <string>
#include <vector>
#include <map>

namespace Astro {

/**
 * Hindu Monthly Calendar - Displays a full month in tabular format
 * Supports BC/BCE dates and various Hindu calendar systems
 */
class HinduMonthlyCalendar {
public:
    // Calendar display options
    struct DisplayOptions {
        bool showGregorianDates = true;        // Show Gregorian dates
        bool showHinduDates = true;            // Show Hindu month/day
        bool showTithi = true;                 // Show Tithi
        bool showNakshatra = true;             // Show Nakshatra
        bool showYoga = false;                 // Show Yoga
        bool showKarana = false;               // Show Karana
        bool showWeekday = true;               // Show weekday names
        bool showMuhurta = false;              // Show auspicious timings
        bool showFestivals = true;             // Show festivals and special events
        bool showRashiInfo = false;            // Show Sun/Moon rashi
        bool showLunarPhase = false;           // Show lunar phase
        bool showSpecialDays = true;           // Highlight Ekadashi, Purnima, etc.
        bool showVrataInfo = false;            // Show Vrata information
        bool showPackedLayout = false;         // Use compact layout
        bool colorOutput = true;               // Use colored output
        bool htmlOutput = false;               // Generate HTML output
        bool useUnicode = true;                // Use Unicode symbols

        // Calendar system preferences
        AyanamsaType ayanamsa = AyanamsaType::LAHIRI;
        CalculationMethod calculationMethod = CalculationMethod::DRIK_SIDDHANTA;
        CalendarSystem calendarSystem = CalendarSystem::LUNI_SOLAR;

        // Language preferences
        bool useEnglish = true;                // Use English names
        bool useSanskrit = false;              // Use Sanskrit names
        bool useDevanagari = false;            // Use Devanagari script

        // Table formatting
        int cellWidth = 12;                    // Width of each calendar cell
        char separator = '|';                  // Cell separator
        bool showHeaders = true;               // Show column headers
        bool showBorders = true;               // Show table borders

        // Time zone and location
        double timeZoneOffset = 5.5;           // Time zone offset from UTC
        std::string timeZoneName = "IST";      // Time zone name
    };

    // Monthly calendar data structure
    struct MonthlyData {
        int year;                              // Gregorian year
        int month;                             // Gregorian month (1-12)
        int hinduYear;                         // Hindu year (Vikram Samvat)
        HinduMonth hinduMonth;                 // Hindu month
        std::string monthName;                 // Month name
        std::string hinduMonthName;            // Hindu month name
        int daysInMonth;                       // Number of days in the month
        int firstDayWeekday;                   // Weekday of first day (0=Sunday)
        bool isBCYear;                         // Whether this is a BC year

        // Daily data for each day of the month
        std::vector<PanchangaData> dailyPanchanga;
        std::vector<double> julianDays;        // Julian day for each date
        std::vector<std::string> gregorianDates; // Formatted Gregorian dates
        std::vector<std::string> hinduDates;   // Formatted Hindu dates

        // Monthly summaries
        std::vector<std::string> monthlyFestivals;  // Major festivals this month
        std::vector<std::string> monthlyEvents;     // Important events
        int ekadashiCount;                          // Number of Ekadashi days
        int purnimaCount;                           // Number of Purnima days
        int amavasyaCount;                          // Number of Amavasya days

        // Seasonal information
        std::string currentRitu;               // Current season (Ritu)
        std::string currentAyana;              // Current Ayana (Uttarayana/Dakshinayana)

        // Summary statistics
        std::map<HinduNakshatra, int> nakshatraCount;  // Count of each nakshatra
        std::map<Tithi, int> tithiCount;               // Count of each tithi
        std::map<Yoga, int> yogaCount;                 // Count of each yoga
    };

    // Calendar navigation
    struct NavigationInfo {
        std::string previousMonth;             // Previous month identifier
        std::string nextMonth;                 // Next month identifier
        std::string currentMonth;              // Current month identifier
        bool canGoPrevious;                    // Can navigate to previous month
        bool canGoNext;                        // Can navigate to next month
    };

private:
    HinduCalendar hinduCalendar;
    DisplayOptions displayOptions;
    double latitude;
    double longitude;
    bool initialized;
    mutable std::string lastError;

    // Helper methods for date calculations
    bool isLeapYear(int year) const;
    int getDaysInMonth(int year, int month) const;
    int getFirstDayOfMonth(int year, int month) const;
    std::string getMonthName(int month) const;
    std::string getWeekdayName(int weekday, bool abbreviated = false) const;

    // Hindu calendar specific helpers
    std::string getHinduMonthName(HinduMonth month) const;
    std::string getTithiName(Tithi tithi, bool abbreviated = false) const;
    std::string getNakshatraName(HinduNakshatra nakshatra, bool abbreviated = false) const;
    std::string getYogaName(Yoga yoga, bool abbreviated = false) const;
    std::string getKaranaName(Karana karana, bool abbreviated = false) const;

    // Formatting helpers
    std::string formatDateCell(const PanchangaData& panchanga, const std::string& gregorianDate,
                              const std::string& hinduDate, int day) const;
    std::string formatHeaderRow() const;
    std::string formatWeekdayHeaders() const;
    std::string formatTableBorder(bool isTop = false, bool isBottom = false) const;
    std::string formatMonthTitle(const MonthlyData& monthData) const;
    std::string formatMonthSummary(const MonthlyData& monthData) const;

    // Enhanced calendar formatting methods
    std::string generateEnhancedHinduCalendar(const MonthlyData& monthData) const;
    std::string formatEnhancedDateCell(const PanchangaData& panchanga, int day, const MonthlyData& monthData) const;
    std::string formatEnhancedQualityCell(const PanchangaData& panchanga, const MonthlyData& monthData) const;

    // Color and styling
    std::string getColorCode(const std::string& colorName) const;
    std::string resetColor() const;
    std::string highlightSpecialDay(const PanchangaData& panchanga, const std::string& text) const;

    // HTML output helpers
    std::string generateHTMLHeader(const MonthlyData& monthData) const;
    std::string generateHTMLTable(const MonthlyData& monthData) const;
    std::string generateHTMLFooter(const MonthlyData& monthData) const;
    std::string generateHTMLCell(const PanchangaData& panchanga, const std::string& gregorianDate,
                                const std::string& hinduDate, int day) const;

    // Festival and event identification
    void identifyMonthlyFestivals(MonthlyData& monthData) const;
    void calculateMonthlySummary(MonthlyData& monthData) const;

    // BC date handling
    bool parseDate(const std::string& dateStr, int& year, int& month, int& day, bool& isBCDate) const;
    std::string formatBCDate(int year, int month, int day, bool isBCDate) const;
    double calculateJulianDay(int year, int month, int day, bool isBCDate) const;

public:
    HinduMonthlyCalendar();
    HinduMonthlyCalendar(double latitude, double longitude, const DisplayOptions& options);
    ~HinduMonthlyCalendar();

    // Initialization
    bool initialize();
    bool initialize(double latitude, double longitude, const DisplayOptions& options);

    // Configuration
    void setLocation(double latitude, double longitude);
    void setDisplayOptions(const DisplayOptions& options);
    DisplayOptions getDisplayOptions() const { return displayOptions; }

    // Static helper to get default options
    static DisplayOptions getDefaultDisplayOptions();

    // Main calendar generation methods
    MonthlyData generateMonthlyData(int year, int month) const;
    MonthlyData generateMonthlyData(const std::string& dateStr) const; // Format: "YYYY-MM" or "YYYY-MM-DD"

    // Calendar display methods
    std::string generateCalendar(int year, int month) const;
    std::string generateCalendar(const std::string& dateStr) const;
    std::string generateHTMLCalendar(int year, int month) const;
    std::string generateHTMLCalendar(const std::string& dateStr) const;

    // BC date support
    std::string generateBCCalendar(int bcYear, int month) const;  // For BC dates
    MonthlyData generateBCMonthlyData(int bcYear, int month) const;

    // Multi-month generation
    std::vector<std::string> generateMultipleMonths(int startYear, int startMonth,
                                                   int endYear, int endMonth) const;
    std::vector<MonthlyData> generateMultipleMonthsData(int startYear, int startMonth,
                                                       int endYear, int endMonth) const;

    // Navigation helpers
    NavigationInfo getNavigationInfo(int year, int month) const;
    std::pair<int, int> getPreviousMonth(int year, int month) const;
    std::pair<int, int> getNextMonth(int year, int month) const;

    // Special date queries
    std::vector<int> getEkadashiDays(int year, int month) const;
    std::vector<int> getPurnimaDays(int year, int month) const;
    std::vector<int> getAmavasyaDays(int year, int month) const;
    std::vector<int> getSankrantiDays(int year, int month) const;
    std::vector<int> getFestivalDays(int year, int month) const;

    // Export methods
    std::string exportToCSV(const MonthlyData& monthData) const;
    std::string exportToJSON(const MonthlyData& monthData) const;
    std::string exportToXML(const MonthlyData& monthData) const;
    bool exportToFile(const MonthlyData& monthData, const std::string& filename,
                     const std::string& format = "txt") const;

    // Utility methods
    bool isValidDate(int year, int month, int day = 1) const;
    bool isBCDateValid(int bcYear, int month, int day = 1) const;
    std::string getCurrentDate() const;
    std::string getCurrentHinduDate() const;

    // Error handling
    std::string getLastError() const { return lastError; }
    bool isInitialized() const { return initialized; }

    // Static utility methods
    static std::string formatYear(int year, bool isBCDate = false);
    static std::string parseYearString(const std::string& yearStr, bool& isBCDate);
    static bool isValidBCYear(int year);
    static double bcYearToJulianDay(int bcYear, int month = 1, int day = 1);

    // Calendar comparison and analysis
    std::string compareWithModernCalendar(int year, int month) const;
    std::string analyzeMonth(const MonthlyData& monthData) const;
    std::vector<std::string> getMonthlyRecommendations(const MonthlyData& monthData) const;

    // Advanced formatting options
    struct AdvancedFormatting {
        bool useAdvancedUnicode = false;       // Use advanced Unicode symbols
        bool showMoonPhases = false;           // Show moon phase symbols
        bool showPlanetaryInfo = false;        // Show planetary positions
        bool showNakshatraPada = false;        // Show Nakshatra Pada
        bool showKaranaTiming = false;         // Show Karana timing
        bool showRahuKaal = false;             // Show Rahu Kaal timing
        bool showYamakanta = false;            // Show Yamakanta timing
        bool showGulikai = false;              // Show Gulikai timing
        std::string customCSS = "";            // Custom CSS for HTML output
        std::string customHeader = "";         // Custom header text
        std::string customFooter = "";         // Custom footer text
    };

    void setAdvancedFormatting(const AdvancedFormatting& formatting);
    AdvancedFormatting getAdvancedFormatting() const;

private:
    AdvancedFormatting advancedFormatting;
};

// Utility functions for Hindu monthly calendar
namespace HinduCalendarUtils {
    // Date conversion utilities
    std::string gregorianToHinduMonth(int gregYear, int gregMonth);
    std::pair<int, int> hinduToGregorianMonth(int hinduYear, HinduMonth hinduMonth);

    // BC date utilities
    bool isBCYear(const std::string& yearStr);
    int parseBCYear(const std::string& yearStr);
    std::string formatBCYear(int year);

    // Calendar system utilities
    std::string getAyanamsaName(AyanamsaType ayanamsa);
    std::string getCalculationMethodName(CalculationMethod method);
    std::string getCalendarSystemName(CalendarSystem system);

    // Festival and event utilities
    std::vector<std::string> getMonthlyFestivals(int month, HinduMonth hinduMonth);
    std::vector<std::string> getSeasonalEvents(const std::string& ritu);

    // Formatting utilities
    std::string padString(const std::string& str, int width, char padChar = ' ');
    std::string centerString(const std::string& str, int width);
    std::string truncateString(const std::string& str, int maxLength);
    std::string wrapText(const std::string& text, int maxWidth);
}

} // namespace Astro
