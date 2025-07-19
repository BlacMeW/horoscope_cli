#pragma once

#include "astro_types.h"
#include "hindu_calendar.h"
#include "myanmar_calendar.h"
#include <string>
#include <vector>
#include <map>
#include <array>
#include <memory>

namespace Astro {

// Forward declarations
class HinduCalendar;
class MyanmarCalendar;
class EphemerisManager;
class PlanetCalculator;

// Planetary transition types
enum class TransitionType {
    SIGN_CHANGE,        // Planet changes zodiac sign
    RETROGRADE_START,   // Planet starts retrograde motion
    RETROGRADE_END,     // Planet ends retrograde motion
    CONJUNCTION,        // Two planets in conjunction
    ECLIPSE,           // Solar or lunar eclipse
    NEW_MOON,          // New moon
    FULL_MOON          // Full moon
};

// Structure for planetary transitions
struct PlanetaryTransition {
    TransitionType type;
    Planet planet;
    Planet secondPlanet = Planet::SUN;  // For conjunctions
    std::string description;
    double julianDay;
    std::string significance;
    bool isAuspicious;
    std::string recommendations;

    // Formatting method
    std::string getDescription() const;
    std::string getFormattedDate() const;
};

// Combined daily data from all calendar systems
struct AstroCalendarDay {
    // Gregorian date
    int gregorianYear;
    int gregorianMonth;
    int gregorianDay;
    std::string gregorianDateStr;

    // Julian Day
    double julianDay;

    // Hindu Calendar (Panchanga) data
    PanchangaData panchangaData;
    bool hasPanchangaData = false;

    // Myanmar Calendar data
    MyanmarCalendarData myanmarData;
    bool hasMyanmarData = false;

    // Planetary data
    std::vector<PlanetaryTransition> planetaryTransitions;
    std::string moonPhaseDescription;
    double sunLongitude;
    double moonLongitude;

    // Combined festivals and events
    std::vector<std::string> allFestivals;
    std::vector<std::string> allEvents;
    std::vector<std::string> astrologicalRecommendations;

    // Day quality assessment
    bool isAuspicious = false;
    bool isInauspicious = false;
    std::string dayQualityDescription;
    int auspiciousScore = 0; // 0-10 scale

    // Formatting methods
    std::string getShortSummary() const;
    std::string getDetailedDescription() const;
    std::string getFestivalsSummary() const;
    std::string getPlanetaryTransitionsSummary() const;
};

// Monthly calendar structure
struct AstroCalendarMonth {
    int year;
    int month;
    std::string monthName;
    std::vector<AstroCalendarDay> days;

    // Monthly summaries
    std::vector<PlanetaryTransition> monthlyTransitions;
    std::vector<std::string> monthlyFestivals;
    std::map<int, std::string> significantDays; // day -> description

    // Statistics
    int auspiciousDays = 0;
    int inauspiciousDays = 0;
    int neutralDays = 0;

    std::string getMonthSummary() const;
};

// Main Astro Calendar System
class AstroCalendar {
private:
    // Calendar systems
    std::unique_ptr<HinduCalendar> hinduCalendar;
    std::unique_ptr<MyanmarCalendar> myanmarCalendar;
    std::unique_ptr<EphemerisManager> ephemerisManager;
    std::unique_ptr<PlanetCalculator> planetCalculator;

    // Configuration
    double latitude = 0.0;
    double longitude = 0.0;
    bool includePlanetaryTransitions = false;
    bool includeAllFestivals = false;

    // Month names
    static const std::array<std::string, 12> monthNames;

    bool initialized = false;
    mutable std::string lastError;

    // Internal calculation methods
    AstroCalendarDay calculateDayData(double julianDay, int gregYear, int gregMonth, int gregDay) const;
    std::vector<PlanetaryTransition> calculatePlanetaryTransitions(double julianDay) const;
    std::vector<PlanetaryTransition> findMonthlyTransitions(int year, int month) const;

    // Festival and event compilation
    void compileFestivalsAndEvents(AstroCalendarDay& day) const;
    void calculateDayQuality(AstroCalendarDay& day) const;

    // Planetary calculations
    bool isPlanetRetrograde(Planet planet, double julianDay) const;
    double calculateConjunctionOrb(Planet planet1, Planet planet2, double julianDay) const;
    bool isEclipse(double julianDay) const;

    // Utility methods
    std::string getGregorianMonthName(int month) const;
    int getDaysInMonth(int year, int month) const;
    bool isLeapYear(int year) const;

    // Enhanced UI/UX helper methods
    std::string getWeekdayName(const AstroCalendarDay& day) const;
    std::string getTithiDescription(int tithi) const;
    std::string getEnhancedQualityIndicator(const AstroCalendarDay& day) const;
    std::string getEnhancedHinduInfo(const AstroCalendarDay& day) const;
    std::string getEnhancedMyanmarInfo(const AstroCalendarDay& day) const;
    std::string getEnhancedFestivalsDisplay(const AstroCalendarDay& day) const;
    std::string getEnhancedPlanetaryDisplay(const AstroCalendarDay& day) const;
    std::string getDetailedDayInfo(const AstroCalendarDay& day) const;
    std::string getEnhancedMonthlySummary(const AstroCalendarMonth& monthData) const;
    std::string abbreviateFestivalName(const std::string& festival) const;
    std::string getPlanetarySymbol(const PlanetaryTransition& transition) const;
    int countMoonPhases(const AstroCalendarMonth& monthData) const;
    std::string formatCalendarCell(const AstroCalendarDay& day) const;
    int getFirstDayOfMonth(int year, int month) const;
    int countExcellentDays(const AstroCalendarMonth& monthData) const;
    int countFestivalDays(const AstroCalendarMonth& monthData) const;
    int countPlanetaryDays(const AstroCalendarMonth& monthData) const;
    int countCautionDays(const AstroCalendarMonth& monthData) const;
    std::string formatAstroCalendarDate(const AstroCalendarDay& day) const;
    std::string getPlanetaryWeather(const std::vector<PlanetaryTransition>& transitions) const;
    std::string formatEnhancedDate(const AstroCalendarDay& day) const;
    std::string getSeasonName(int month) const;
    std::string generateDetailedDayView(const AstroCalendarDay& day) const;

public:
    AstroCalendar();
    ~AstroCalendar();

    // Initialize the calendar system
    bool initialize(double lat = 0.0, double lon = 0.0);

    // Configuration methods
    void setLocation(double lat, double lon) { latitude = lat; longitude = lon; }
    void setIncludePlanetaryTransitions(bool include) { includePlanetaryTransitions = include; }
    void setIncludeAllFestivals(bool include) { includeAllFestivals = include; }

    // Main calculation methods
    AstroCalendarDay calculateAstroCalendarDay(const BirthData& birthData) const;
    AstroCalendarDay calculateAstroCalendarDay(int year, int month, int day) const;
    AstroCalendarMonth calculateAstroCalendarMonth(int year, int month) const;

    // Output formatting methods
    std::string generateDayCalendar(const AstroCalendarDay& day, const std::string& format = "calendar") const;
    std::string generateMonthlyCalendar(const AstroCalendarMonth& monthData, const std::string& format = "calendar") const;
    std::string generateJSON(const AstroCalendarDay& day) const;
    std::string generateJSON(const AstroCalendarMonth& monthData) const;
    std::string generateCSV(const AstroCalendarMonth& monthData) const;
    std::string generateTable(const AstroCalendarMonth& monthData) const;

    // Calendar layout generators
    std::string generateCalendarLayout(const AstroCalendarMonth& monthData) const;
    std::string generateDetailedCalendarLayout(const AstroCalendarMonth& monthData) const;
    std::string generateCompactCalendarLayout(const AstroCalendarMonth& monthData) const;

public:
    // Utility methods
    std::string getLastError() const { return lastError; }
    bool isInitialized() const { return initialized; }

    // Static utility methods
    static double gregorianToJulianDay(int year, int month, int day);
    static void julianDayToGregorian(double jd, int& year, int& month, int& day);
    static std::string formatDate(int year, int month, int day);
};

// Utility functions
std::string formatAstroCalendarDate(const AstroCalendarDay& day);
std::string getSeasonName(int month);
std::string getMoonPhaseName(double lunarPhase);
std::string getPlanetaryWeather(const std::vector<PlanetaryTransition>& transitions);

} // namespace Astro
