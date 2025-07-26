#pragma once

#include "astro_types.h"
#include <string>
#include <vector>

namespace Astro {

// Planetary motion types for color coding
enum class MotionType {
    DIRECT_FAST,        // Fast direct motion (green)
    DIRECT_NORMAL,      // Normal direct motion (white/default)
    DIRECT_SLOW,        // Slow direct motion (yellow)
    STATIONARY,         // Nearly stationary (blue)
    RETROGRADE          // Retrograde motion (red)
};

// Ephemeris table entry structure
struct EphemerisEntry {
    double julianDay;
    int year, month, day;
    double siderealTime;        // Greenwich Sidereal Time in hours
    std::vector<PlanetPosition> positions;

    std::string getDateString() const;
    std::string getJulianDateString() const;
    std::string getDayName() const;
    bool shouldUseJulianCalendar() const; // Returns true if date is before Oct 15, 1582
    double getAyanamsaValue(AyanamsaType ayanamsa) const;
    std::string getFormattedRow() const;
};

// Ephemeris table configuration
struct EphemerisConfig {
    BirthData startDate;
    BirthData endDate;
    int intervalDays;           // Interval between entries (1 = daily, 7 = weekly, etc.)
    std::vector<Planet> planets; // Planets to include in table
    bool showDegreeMinutes;     // Show degrees and minutes
    bool showSign;              // Show zodiac sign
    bool showRetrograde;        // Show retrograde status
    bool showSpeed;             // Show daily motion
    bool showDistance;          // Show distance from Earth
    bool showLatitude;          // Show ecliptic latitude
    bool showLatitudeOnly;      // Show ecliptic latitude only (not longitude)
    bool showDeclination;       // Show declination
    bool showRightAscension;    // Show right ascension
    bool showSiderealTime;      // Show sidereal time
    bool show3LineCoordinates;  // Show longitude, latitude, and declination in 3-line format
    bool compactFormat;         // Use compact Astrodienst-style format
    std::string format;         // Output format: "table", "csv", "json"

    // Zodiac system configuration
    ZodiacMode zodiacMode;      // Tropical or Sidereal zodiac
    AyanamsaType ayanamsa;      // Ayanamsa type for sidereal calculations

    // Display options
    bool useColors;             // Enable color coding for planetary motion
    bool showDayNames;          // Show short day names (Su, Mo, etc.)
    std::string calendarMode;   // Calendar display mode: "jul", "gregorian", "auto", "both"

    EphemerisConfig();
};

class EphemerisTable {
public:
    EphemerisTable();
    ~EphemerisTable();

    // Initialize with ephemeris data
    bool initialize(const std::string& ephePath = "");

    // Generate ephemeris table for date range
    std::string generateTable(const EphemerisConfig& config) const;

    // Generate table with string dates (convenience methods)
    std::string generateTable(const std::string& fromDate, const std::string& toDate, int intervalDays = 1) const;
    std::string generateCSVTable(const std::string& fromDate, const std::string& toDate, int intervalDays = 1) const;
    std::string generateJSONTable(const std::string& fromDate, const std::string& toDate, int intervalDays = 1) const;

    // Generate daily ephemeris for a month
    std::string generateMonthlyTable(int year, int month, const std::vector<Planet>& planets) const;

    // Generate yearly ephemeris (monthly entries)
    std::string generateYearlyTable(int year, const std::vector<Planet>& planets) const;

    // Generate ephemeris for specific dates
    std::string generateCustomDates(const std::vector<BirthData>& dates, const std::vector<Planet>& planets) const;

    // Generate transit table (planets relative to natal positions)
    std::string generateTransitTable(const BirthData& natalData, const BirthData& fromDate,
                                   const BirthData& toDate, int intervalDays = 1) const;

    // Generate progression table
    std::string generateProgressionTable(const BirthData& natalData, const BirthData& fromDate,
                                       const BirthData& toDate, const std::string& progressionType = "secondary") const;

    // Generate solar return table
    std::string generateSolarReturnTable(const BirthData& natalData, int fromYear, int toYear) const;

    // Generate aspect table between two sets of planets
    std::string generateAspectTable(const std::vector<PlanetPosition>& set1, const std::vector<PlanetPosition>& set2) const;

    // Export to different formats
    std::string exportToCSV(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const;
    std::string exportToJSON(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const;
    std::string exportToHTML(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const;

    // Generate planet position for single date
    EphemerisEntry generateSingleEntry(const BirthData& date, const std::vector<Planet>& planets) const;

    // Configuration methods for zodiac mode and ayanamsa
    static EphemerisConfig createSiderealConfig(AyanamsaType ayanamsa = AyanamsaType::LAHIRI);
    static EphemerisConfig createTropicalConfig();

    // Get error message
    std::string getLastError() const { return lastError; }

private:
    bool isInitialized;
    mutable std::string lastError;

    // Generate table entries for date range
    std::vector<EphemerisEntry> generateEntries(const EphemerisConfig& config) const;

    // Format table as ASCII
    std::string formatAsTable(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const;
    std::string formatAsCompactTable(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const;

    // Get column headers
    std::vector<std::string> getColumnHeaders(const EphemerisConfig& config) const;

    // Format planet position for table
    std::string formatPlanetPosition(const PlanetPosition& position, const EphemerisConfig& config) const;
    std::string format3LinePosition(const PlanetPosition& position, const EphemerisConfig& config) const;

    // Calculate column widths
    std::vector<int> calculateColumnWidths(const std::vector<EphemerisEntry>& entries,
                                          const EphemerisConfig& config) const;

    // Format table header
    std::string formatTableHeader(const std::vector<std::string>& headers,
                                 const std::vector<int>& widths) const;

    // Format table separator
    std::string formatTableSeparator(const std::vector<int>& widths) const;

    // Format single table row
    std::string formatTableRow(const EphemerisEntry& entry, const EphemerisConfig& config,
                              const std::vector<int>& widths) const;
    std::string format3LineTableRow(const EphemerisEntry& entry, const EphemerisConfig& config,
                                   const std::vector<int>& widths) const;

    // Helper functions for specific calculations
    double calculateDeclination(const PlanetPosition& position) const;
    double calculateRightAscension(const PlanetPosition& position) const;
    bool isRetrograde(const PlanetPosition& position) const;

    // Motion analysis and color formatting
    MotionType getMotionType(const PlanetPosition& position) const;
    std::string applyMotionColor(const std::string& text, MotionType motionType, bool useColors) const;
    std::string formatColoredPosition(const PlanetPosition& position, const EphemerisConfig& config) const;

    // Helper functions for ANSI color alignment
    size_t getVisualWidth(const std::string& text) const;
    std::string padStringToWidth(const std::string& text, int width, bool leftAlign = true) const;

    // Format degree values
    std::string formatDegrees(double degrees, bool showMinutes = true) const;
    std::string formatDegreeWithSign(double longitude) const;
    std::string formatDeclination(double declination) const;
    std::string formatLatitude(double latitude) const;
    std::string formatDistance(double distance) const;
    std::string formatRightAscension(double rightAscension) const;
    std::string formatSiderealTime(double siderealHours) const;
    std::string formatTime(double hours) const;

    // Compact format helpers
    std::string getPlanetAbbreviation(Planet planet) const;
    std::string formatCompactPosition(const PlanetPosition& position) const;
    std::string formatColoredCompactPosition(const PlanetPosition& position, const EphemerisConfig& config) const;
    char getSignCharacter(ZodiacSign sign) const;

    // Default planet lists
    std::vector<Planet> getDefaultPlanets() const;
    std::vector<Planet> getMajorPlanets() const;
    std::vector<Planet> getInnerPlanets() const;
    std::vector<Planet> getOuterPlanets() const;
};

} // namespace Astro
