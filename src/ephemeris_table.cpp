#include "ephemeris_table.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "astro_types.h"
#include <swephexp.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <map>

// ANSI color codes for terminal output
namespace AnsiColors {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string WHITE = "\033[37m";
}

namespace Astro {

EphemerisConfig::EphemerisConfig()
    : intervalDays(1), showDegreeMinutes(true), showSign(true), showRetrograde(true),
      showSpeed(false), showDistance(false), showLatitude(false), showLatitudeOnly(false),
      showDeclination(false), showRightAscension(false), showSiderealTime(false),
      show3LineCoordinates(false), compactFormat(false), format("table"), zodiacMode(ZodiacMode::TROPICAL),
      ayanamsa(AyanamsaType::LAHIRI), useColors(true), showDayNames(true),
      calendarMode("auto") {

    // Default planets
    planets = {Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS, Planet::MARS,
               Planet::JUPITER, Planet::SATURN, Planet::URANUS, Planet::NEPTUNE, Planet::PLUTO};
}EphemerisTable::EphemerisTable() : isInitialized(false) {
}

EphemerisTable::~EphemerisTable() {
}

bool EphemerisTable::initialize(const std::string& ephePath) {
    EphemerisManager ephMgr;
    if (!ephMgr.initialize(ephePath)) {
        lastError = "Failed to initialize ephemeris manager: " + ephMgr.getLastError();
        return false;
    }

    isInitialized = true;
    return true;
}

std::string EphemerisTable::generateTable(const EphemerisConfig& config) const {
    if (!isInitialized) {
        lastError = "Ephemeris table not initialized";
        return "";
    }

    std::vector<EphemerisEntry> entries = generateEntries(config);

    // Check if entries generation failed due to error
    if (entries.empty() && !lastError.empty()) {
        return "";
    }

    if (config.format == "csv") {
        return exportToCSV(entries, config);
    } else if (config.format == "json") {
        return exportToJSON(entries, config);
    } else if (config.format == "html") {
        return exportToHTML(entries, config);
    } else {
        return formatAsTable(entries, config);
    }
}

std::string EphemerisTable::generateMonthlyTable(int year, int month, const std::vector<Planet>& planets) const {
    EphemerisConfig config;
    config.startDate = {year, month, 1, 0, 0, 0, 0.0, 0.0, 0.0};

    // Calculate last day of month
    int daysInMonth = 31;
    if (month == 2) {
        daysInMonth = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        daysInMonth = 30;
    }

    config.endDate = {year, month, daysInMonth, 23, 59, 59, 0.0, 0.0, 0.0};
    config.planets = planets;
    config.intervalDays = 1;

    return generateTable(config);
}

std::string EphemerisTable::generateYearlyTable(int year, const std::vector<Planet>& planets) const {
    EphemerisConfig config;
    config.startDate = {year, 1, 1, 0, 0, 0, 0.0, 0.0, 0.0};
    config.endDate = {year, 12, 31, 23, 59, 59, 0.0, 0.0, 0.0};
    config.planets = planets;
    config.intervalDays = 30; // Monthly entries for yearly table

    return generateTable(config);
}

std::string EphemerisTable::generateTransitTable(const BirthData& natalData, const BirthData& fromDate,
                                               const BirthData& toDate, int intervalDays) const {
    if (!isInitialized) {
        lastError = "Ephemeris table not initialized";
        return "";
    }

    std::stringstream ss;
    ss << "\n=== TRANSIT EPHEMERIS TABLE ===\n";
    ss << "Natal Date: " << natalData.getDateTimeString() << "\n";
    ss << "Transit Period: " << fromDate.getDateTimeString() << " to " << toDate.getDateTimeString() << "\n\n";

    // Calculate natal positions
    EphemerisManager ephMgr;
    if (!ephMgr.initialize()) {
        lastError = "Failed to initialize ephemeris manager";
        return "";
    }

    PlanetCalculator calc(ephMgr);
    std::vector<PlanetPosition> natalPositions;
    calc.calculateAllPlanets(natalData, natalPositions);

    // Generate transit table
    EphemerisConfig config;
    config.startDate = fromDate;
    config.endDate = toDate;
    config.intervalDays = intervalDays;
    config.planets = getDefaultPlanets();

    std::vector<EphemerisEntry> entries = generateEntries(config);

    // Format with natal comparisons
    ss << "Date        ";
    for (Planet planet : config.planets) {
        ss << std::left << std::setw(12) << planetToString(planet);
    }
    ss << "\n";
    ss << "────────────";
    for (size_t i = 0; i < config.planets.size(); i++) {
        ss << "────────────";
    }
    ss << "\n";

    for (const auto& entry : entries) {
        ss << entry.getDateString() << " ";

        for (Planet planet : config.planets) {
            auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                                 [planet](const PlanetPosition& pos) { return pos.planet == planet; });

            if (it != entry.positions.end()) {
                // Find natal position for comparison
                auto natalIt = std::find_if(natalPositions.begin(), natalPositions.end(),
                                          [planet](const PlanetPosition& pos) { return pos.planet == planet; });

                if (natalIt != natalPositions.end()) {
                    double aspect = std::abs(it->longitude - natalIt->longitude);
                    if (aspect > 180.0) aspect = 360.0 - aspect;

                    ss << std::left << std::setw(12) << formatDegreeWithSign(it->longitude);
                } else {
                    ss << std::left << std::setw(12) << formatDegreeWithSign(it->longitude);
                }
            } else {
                ss << std::left << std::setw(12) << "---";
            }
        }
        ss << "\n";
    }

    return ss.str();
}

std::vector<EphemerisEntry> EphemerisTable::generateEntries(const EphemerisConfig& config) const {
    std::vector<EphemerisEntry> entries;

    double startJD = config.startDate.getJulianDay();
    double endJD = config.endDate.getJulianDay();

    EphemerisManager ephMgr;
    if (!ephMgr.initialize()) {
        return entries;
    }

    PlanetCalculator calc(ephMgr);

    for (double currentJD = startJD; currentJD <= endJD; currentJD += config.intervalDays) {
        EphemerisEntry entry;
        entry.julianDay = currentJD;

        // Convert Julian Day to calendar date
        int year, month, day, hour, minute;
        double second;
        swe_jdet_to_utc(currentJD, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);
        entry.year = year;
        entry.month = month;
        entry.day = day;

        // Calculate sidereal time for ephemeris table
        // Testing different sidereal time calculation methods to match reference
        double jd_0h_ut = floor(currentJD) + 0.5; // JD for 0h UT of the date

        // Try using swe_sidtime0() which calculates mean sidereal time at Greenwich for 0h UT
        // This might be what the reference ephemeris is using
        double mean_sidereal_time = swe_sidtime0(jd_0h_ut, 0.0, 0.0); // mean sidereal time, no nutation, no longitude

        // Ensure it's in the 0-24 hour range
        while (mean_sidereal_time >= 24.0) mean_sidereal_time -= 24.0;
        while (mean_sidereal_time < 0.0) mean_sidereal_time += 24.0;

        entry.siderealTime = mean_sidereal_time;

        // Calculate planet positions for this date with zodiac mode and ayanamsa
        BirthData entryDate = {year, month, day, hour, minute, static_cast<int>(second), 0.0, 0.0, 0.0};

        // Configure the calculator with zodiac mode and ayanamsa
        calc.setZodiacMode(config.zodiacMode);
        if (config.zodiacMode == ZodiacMode::SIDEREAL) {
            calc.setAyanamsa(config.ayanamsa);
        }

        calc.calculateAllPlanets(entryDate, entry.positions);

        entries.push_back(entry);
    }

    return entries;
}

std::string EphemerisTable::formatAsTable(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const {
    if (entries.empty()) {
        return "No entries to display.\n";
    }

    if (config.compactFormat) {
        return formatAsCompactTable(entries, config);
    }

    std::stringstream ss;
    ss << "\n=== EPHEMERIS TABLE ===\n";
    ss << "Period: " << config.startDate.getDateTimeString() << " to " << config.endDate.getDateTimeString() << "\n";
    ss << "Interval: " << config.intervalDays << " day(s)\n";

    // Show zodiac mode and ayanamsa info
    ss << "Zodiac: " << zodiacModeToString(config.zodiacMode);
    if (config.zodiacMode == ZodiacMode::SIDEREAL) {
        ss << " (" << ayanamsaTypeToString(config.ayanamsa) << " ayanamsa)";
    }
    ss << "\n\n";

    // Calculate column widths
    std::vector<int> widths = calculateColumnWidths(entries, config);

    // Generate header
    std::vector<std::string> headers = getColumnHeaders(config);
    ss << formatTableHeader(headers, widths);
    ss << formatTableSeparator(widths);

    // Generate table rows
    for (const auto& entry : entries) {
        ss << formatTableRow(entry, config, widths);
    }

    ss << formatTableSeparator(widths);

    // Add legend
    if (config.show3LineCoordinates) {
        ss << "\n3-Line Format:\n";
        ss << "Line 1: Longitude (ecliptic position)\n";
        ss << "Line 2: Latitude (ecliptic north/south of ecliptic plane)\n";
        ss << "Line 3: Declination (celestial north/south of celestial equator)\n";
        if (config.showRetrograde) {
            ss << "R = Retrograde motion\n";
        }
        if (config.showSign) {
            ss << "Signs: ♈Ari ♉Tau ♊Gem ♋Can ♌Leo ♍Vir ♎Lib ♏Sco ♐Sag ♑Cap ♒Aqu ♓Pis\n";
        }
    } else {
        if (config.showRetrograde) {
            ss << "\nR = Retrograde motion\n";
        }
        if (config.showLatitudeOnly) {
            ss << "Lat = Ecliptic latitude (north/south of ecliptic plane, -90° to +90°)\n";
        } else if (config.showDistance) {
            ss << "Dist = Distance from Earth (in Astronomical Units, AU)\n";
        } else if (config.showRightAscension) {
            ss << "RA = Right Ascension (celestial longitude, 0h to 24h)\n";
        } else if (config.showDeclination && !config.showSign) {
            ss << "Dec = Declination (celestial latitude, -90° to +90°)\n";
        } else if (config.showDeclination && config.showSign) {
            ss << "Lon/Dec = Longitude (ecliptic) / Declination (celestial)\n";
            ss << "Signs: ♈Ari ♉Tau ♊Gem ♋Can ♌Leo ♍Vir ♎Lib ♏Sco ♐Sag ♑Cap ♒Aqu ♓Pis\n";
        } else if (config.showSign) {
            ss << "Signs: ♈Ari ♉Tau ♊Gem ♋Can ♌Leo ♍Vir ♎Lib ♏Sco ♐Sag ♑Cap ♒Aqu ♓Pis\n";
        }
    }

    // Add ayanamsa information for sidereal zodiac mode
    if (config.zodiacMode == ZodiacMode::SIDEREAL && !entries.empty()) {
        double ayanamsaValue = entries[0].getAyanamsaValue(config.ayanamsa);
        ss << "Ayanamsa: " << ayanamsaTypeToString(config.ayanamsa)
           << " = " << std::fixed << std::setprecision(4) << ayanamsaValue << "°"
           << " (at " << entries[0].getDateString() << ")\n";
    }

    return ss.str();
}

std::vector<std::string> EphemerisTable::getColumnHeaders(const EphemerisConfig& config) const {
    std::vector<std::string> headers;

    // Date header - construct based on calendar mode configuration
    std::string dateHeader = "Date";
    if (config.showDayNames) {
        dateHeader = "Day Date";
    }

    if (config.calendarMode == "jul") {
        dateHeader = config.showDayNames ? "Day Julian Date" : "Julian Date";
    } else if (config.calendarMode == "gregorian") {
        dateHeader = config.showDayNames ? "Day Date" : "Date";
    } else if (config.calendarMode == "auto") {
        dateHeader = config.showDayNames ? "Day Date" : "Date";
    } else if (config.calendarMode == "both") {
        dateHeader = config.showDayNames ? "Day Date (Julian)" : "Date (Julian)";
    }

    headers.push_back(dateHeader);    // Add sidereal time column if enabled
    if (config.showSiderealTime) {
        headers.push_back("Sidereal Time");
    }

    for (Planet planet : config.planets) {
        std::string header = planetToSymbol(planet);

        // Add coordinate type suffix to clarify what's being shown
        if (config.show3LineCoordinates) {
            header += " (L/L/D)"; // Longitude/Latitude/Declination
        } else if (config.showLatitudeOnly) {
            header += " (Lat)"; // Latitude only
        } else if (config.showDistance) {
            header += " (Dist)"; // Distance only
        } else if (config.showRightAscension) {
            header += " (RA)"; // Right Ascension only
        } else if (config.showDeclination && !config.showSign) {
            header += " (Dec)";
        } else if (config.showDeclination && config.showSign) {
            header += " (Lon/Dec)";
        }
        // For longitude-only mode, no suffix needed (default)

        headers.push_back(header);
    }

    return headers;
}

std::vector<int> EphemerisTable::calculateColumnWidths(const std::vector<EphemerisEntry>& entries,
                                                      const EphemerisConfig& config) const {
    std::vector<std::string> headers = getColumnHeaders(config);
    std::vector<int> widths;

    // Date column width - calculate based on configuration
    int dateWidth = headers[0].length(); // Start with header width

    // Calculate maximum date string width from actual entries
    for (const auto& entry : entries) {
        std::string dateStr;
        if (config.showDayNames) {
            dateStr = entry.getDayName() + " ";
        }

        if (config.calendarMode == "jul") {
            dateStr += entry.getJulianDateString();
        } else if (config.calendarMode == "gregorian") {
            dateStr += entry.getDateString();
        } else if (config.calendarMode == "auto") {
            if (entry.shouldUseJulianCalendar()) {
                dateStr += entry.getJulianDateString() + " (Jul)";
            } else {
                dateStr += entry.getDateString();
            }
        } else if (config.calendarMode == "both") {
            dateStr += entry.getDateString() + " (" + entry.getJulianDateString() + ")";
        } else {
            dateStr += entry.getDateString(); // Default fallback
        }

        dateWidth = std::max(dateWidth, static_cast<int>(dateStr.length()));
    }

    widths.push_back(dateWidth);

    // Sidereal time column width if enabled
    if (config.showSiderealTime) {
        int siderealWidth = headers[widths.size()].length(); // Start with header width

        // Calculate width based on actual sidereal time data
        for (const auto& entry : entries) {
            std::string siderealStr = formatSiderealTime(entry.siderealTime);
            siderealWidth = std::max(siderealWidth, static_cast<int>(siderealStr.length()));
        }

        widths.push_back(siderealWidth);
    }

    // Planet column widths - calculate based on actual data content
    for (size_t i = 0; i < config.planets.size(); i++) {
        Planet planet = config.planets[i];
        int headerIndex = widths.size();
        int width = headers[headerIndex].length(); // Start with header width

        // Calculate maximum width needed for this planet's data across all entries
        for (const auto& entry : entries) {
            // Find position for this planet
            auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                                 [planet](const PlanetPosition& pos) { return pos.planet == planet; });

            if (it != entry.positions.end()) {
                std::string positionStr = formatPlanetPosition(*it, config);

                if (config.show3LineCoordinates) {
                    // For 3-line format, check each line separately and take the maximum
                    std::istringstream iss(positionStr);
                    std::string line;
                    while (std::getline(iss, line)) {
                        width = std::max(width, static_cast<int>(getVisualWidth(line)));
                    }
                } else {
                    // For single-line formats, use the full string width
                    width = std::max(width, static_cast<int>(getVisualWidth(positionStr)));
                }
            }
        }

        // Add a small padding (2 characters) for better readability
        width += 2;

        widths.push_back(width);
    }

    return widths;
}

std::string EphemerisTable::formatTableHeader(const std::vector<std::string>& headers,
                                            const std::vector<int>& widths) const {
    std::stringstream ss;
    for (size_t i = 0; i < headers.size() && i < widths.size(); i++) {
        // All columns - left aligned using consistent padding
        if (i > 0) ss << " ";
        ss << padStringToWidth(headers[i], widths[i], true);
    }
    ss << "\n";
    return ss.str();
}

std::string EphemerisTable::formatTableSeparator(const std::vector<int>& widths) const {
    std::stringstream ss;
    for (size_t i = 0; i < widths.size(); i++) {
        ss << std::string(widths[i], '-');
        if (i < widths.size() - 1) ss << " ";
    }
    ss << "\n";
    return ss.str();
}

std::string EphemerisTable::formatTableRow(const EphemerisEntry& entry, const EphemerisConfig& config,
                                         const std::vector<int>& widths) const {
    // Handle 3-line coordinate format differently
    if (config.show3LineCoordinates) {
        return format3LineTableRow(entry, config, widths);
    }

    std::stringstream ss;

    // Date column - construct based on calendar mode configuration
    std::string dateStr;
    if (config.showDayNames) {
        dateStr = entry.getDayName() + " ";
    }

    if (config.calendarMode == "jul") {
        // Julian calendar only
        dateStr += entry.getJulianDateString();
    } else if (config.calendarMode == "gregorian") {
        // Gregorian calendar only
        dateStr += entry.getDateString();
    } else if (config.calendarMode == "auto") {
        // Automatic: use Julian before Oct 15, 1582, Gregorian after
        if (entry.shouldUseJulianCalendar()) {
            dateStr += entry.getJulianDateString() + " (Jul)";
        } else {
            dateStr += entry.getDateString();
        }
    } else if (config.calendarMode == "both") {
        // Show both calendars
        dateStr += entry.getDateString() + " (" + entry.getJulianDateString() + ")";
    } else {
        // Default fallback to Gregorian
        dateStr += entry.getDateString();
    }

    ss << padStringToWidth(dateStr, widths[0], true);    size_t columnIndex = 1;

    // Sidereal time column if enabled
    if (config.showSiderealTime) {
        ss << " " << padStringToWidth(formatSiderealTime(entry.siderealTime), widths[columnIndex], true);
        columnIndex++;
    }

    // Planet columns - left aligned
    for (size_t i = 0; i < config.planets.size() && columnIndex < widths.size(); i++) {
        Planet planet = config.planets[i];

        // Find position for this planet
        auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                             [planet](const PlanetPosition& pos) { return pos.planet == planet; });

        std::string planetStr;
        if (it != entry.positions.end()) {
            planetStr = formatColoredPosition(*it, config);
        } else {
            planetStr = "---";
        }

        // Left-align the planet position data using proper padding for colored text
        ss << " " << padStringToWidth(planetStr, widths[columnIndex], true);
        columnIndex++;
    }
    ss << "\n";
    return ss.str();
}

std::string EphemerisTable::format3LineTableRow(const EphemerisEntry& entry, const EphemerisConfig& config,
                                               const std::vector<int>& widths) const {
    std::stringstream ss;

    // Date column - construct based on calendar mode configuration
    std::string dateStr;
    if (config.showDayNames) {
        dateStr = entry.getDayName() + " ";
    }

    if (config.calendarMode == "jul") {
        dateStr += entry.getJulianDateString();
    } else if (config.calendarMode == "gregorian") {
        dateStr += entry.getDateString();
    } else if (config.calendarMode == "auto") {
        if (entry.shouldUseJulianCalendar()) {
            dateStr += entry.getJulianDateString() + " (Jul)";
        } else {
            dateStr += entry.getDateString();
        }
    } else if (config.calendarMode == "both") {
        dateStr += entry.getDateString() + " (" + entry.getJulianDateString() + ")";
    } else {
        dateStr += entry.getDateString();
    }

    size_t columnIndex = 1;

    // Collect planet position data for each line
    std::vector<std::string> lonLines, latLines, decLines;

    for (size_t i = 0; i < config.planets.size(); i++) {
        Planet planet = config.planets[i];

        auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                             [planet](const PlanetPosition& pos) { return pos.planet == planet; });

        if (it != entry.positions.end()) {
            // Format longitude
            std::string lonStr;
            if (config.showDegreeMinutes) {
                lonStr = formatDegreeWithSign(it->longitude);
            } else if (config.showSign) {
                lonStr = zodiacSignToString(it->sign);
            } else {
                lonStr = std::to_string(static_cast<int>(it->longitude)) + "°";
            }
            if (config.showRetrograde && isRetrograde(*it)) {
                lonStr += "R";
            }
            lonLines.push_back(lonStr);
            latLines.push_back(formatLatitude(it->latitude));
            decLines.push_back(formatDeclination(it->declination));
        } else {
            lonLines.push_back("---");
            latLines.push_back("---");
            decLines.push_back("---");
        }
    }

    // Generate three lines
    // Line 1: Date + Longitude values
    ss << padStringToWidth(dateStr, widths[0], true);
    if (config.showSiderealTime) {
        ss << " " << padStringToWidth(formatSiderealTime(entry.siderealTime), widths[columnIndex], true);
        columnIndex++;
    }
    for (size_t i = 0; i < lonLines.size() && columnIndex < widths.size(); i++) {
        ss << " " << padStringToWidth(lonLines[i], widths[columnIndex], true);
        columnIndex++;
    }
    ss << "\n";

    // Line 2: Empty date column + Latitude values
    ss << padStringToWidth("", widths[0], true);
    columnIndex = 1;
    if (config.showSiderealTime) {
        ss << " " << padStringToWidth("", widths[columnIndex], true);
        columnIndex++;
    }
    for (size_t i = 0; i < latLines.size() && columnIndex < widths.size(); i++) {
        ss << " " << padStringToWidth(latLines[i], widths[columnIndex], true);
        columnIndex++;
    }
    ss << "\n";

    // Line 3: Empty date column + Declination values
    ss << padStringToWidth("", widths[0], true);
    columnIndex = 1;
    if (config.showSiderealTime) {
        ss << " " << padStringToWidth("", widths[columnIndex], true);
        columnIndex++;
    }
    for (size_t i = 0; i < decLines.size() && columnIndex < widths.size(); i++) {
        ss << " " << padStringToWidth(decLines[i], widths[columnIndex], true);
        columnIndex++;
    }
    ss << "\n";

    return ss.str();
}

std::string EphemerisTable::formatPlanetPosition(const PlanetPosition& position, const EphemerisConfig& config) const {
    std::stringstream ss;

    // Handle 3-line coordinate format
    if (config.show3LineCoordinates) {
        return format3LinePosition(position, config);
    }

    // Handle different coordinate types based on configuration
    if (config.showLatitudeOnly) {
        // Latitude only mode
        ss << formatLatitude(position.latitude);
    } else if (config.showDistance) {
        // Distance only mode
        ss << formatDistance(position.distance);
    } else if (config.showRightAscension) {
        // Right ascension only mode
        ss << formatRightAscension(position.rightAscension);
    } else if (config.showDeclination && !config.showSign) {
        // Declination only mode
        ss << formatDeclination(position.declination);
    } else if (config.showDeclination && config.showSign) {
        // Both longitude and declination mode
        if (config.showDegreeMinutes) {
            ss << formatDegreeWithSign(position.longitude) << "/" << formatDeclination(position.declination);
        } else {
            ss << zodiacSignToString(position.sign) << "/" << formatDeclination(position.declination);
        }
    } else {
        // Default longitude mode
        if (config.showDegreeMinutes) {
            ss << formatDegreeWithSign(position.longitude);
        } else if (config.showSign) {
            ss << zodiacSignToString(position.sign);
        } else {
            ss << std::fixed << std::setprecision(2) << position.longitude;
        }
    }

    if (config.showRetrograde && isRetrograde(position)) {
        ss << "R";
    }

    if (config.showSpeed) {
        ss << " (" << std::fixed << std::setprecision(2) << position.speed << ")";
    }

    return ss.str();
}

std::string EphemerisTable::format3LinePosition(const PlanetPosition& position, const EphemerisConfig& config) const {
    std::stringstream ss;

    // Format longitude (line 1)
    std::string lonStr;
    if (config.showDegreeMinutes) {
        lonStr = "Lon: " + formatDegreeWithSign(position.longitude);
    } else if (config.showSign) {
        lonStr = "Lon: " + zodiacSignToString(position.sign);
    } else {
        lonStr = "Lon: " + std::to_string(static_cast<int>(position.longitude)) + "°";
    }

    // Format latitude (line 2)
    std::string latStr = "Lat: " + formatLatitude(position.latitude);

    // Format declination (line 3)
    std::string decStr = "Dec: " + formatDeclination(position.declination);

    // Add retrograde indicator if applicable
    if (config.showRetrograde && isRetrograde(position)) {
        lonStr += "R";
    }

    // Combine all three lines with newlines
    ss << lonStr << "\n" << latStr << "\n" << decStr;

    return ss.str();
}

std::string EphemerisTable::exportToCSV(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const {
    std::stringstream ss;

    // CSV Header with metadata comments
    ss << "# Ephemeris Table - Zodiac: " << (config.zodiacMode == ZodiacMode::TROPICAL ? "Tropical" : "Sidereal");
    if (config.zodiacMode == ZodiacMode::SIDEREAL && !entries.empty()) {
        double ayanamsaValue = entries[0].getAyanamsaValue(config.ayanamsa);
        ss << " (" << ayanamsaTypeToString(config.ayanamsa) << " ayanamsa = "
           << std::fixed << std::setprecision(4) << ayanamsaValue << "°)";
    }
    ss << "\n";

    // CSV Header
    ss << "Date";
    for (Planet planet : config.planets) {
        ss << "," << planetToString(planet);
        if (config.showSign) ss << " Sign";
        if (config.showSpeed) ss << " Speed";
        if (config.showRetrograde) ss << " Retrograde";
    }
    ss << "\n";

    // CSV Data
    for (const auto& entry : entries) {
        ss << entry.getDateString();

        for (Planet planet : config.planets) {
            auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                                 [planet](const PlanetPosition& pos) { return pos.planet == planet; });

            if (it != entry.positions.end()) {
                ss << "," << std::fixed << std::setprecision(6) << it->longitude;
                if (config.showSign) ss << "," << zodiacSignToString(it->sign);
                if (config.showSpeed) ss << "," << std::fixed << std::setprecision(6) << it->speed;
                if (config.showRetrograde) ss << "," << (isRetrograde(*it) ? "Yes" : "No");
            } else {
                ss << ",";
                if (config.showSign) ss << ",";
                if (config.showSpeed) ss << ",";
                if (config.showRetrograde) ss << ",";
            }
        }
        ss << "\n";
    }

    return ss.str();
}

std::string EphemerisTable::exportToJSON(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"ephemeris\": {\n";
    ss << "    \"period\": {\n";
    ss << "      \"start\": \"" << config.startDate.getDateTimeString() << "\",\n";
    ss << "      \"end\": \"" << config.endDate.getDateTimeString() << "\",\n";
    ss << "      \"interval_days\": " << config.intervalDays << "\n";
    ss << "    },\n";
    ss << "    \"zodiac_mode\": \"" << (config.zodiacMode == ZodiacMode::TROPICAL ? "tropical" : "sidereal") << "\",\n";
    if (config.zodiacMode == ZodiacMode::SIDEREAL && !entries.empty()) {
        double ayanamsaValue = entries[0].getAyanamsaValue(config.ayanamsa);
        ss << "    \"ayanamsa\": {\n";
        ss << "      \"name\": \"" << ayanamsaTypeToString(config.ayanamsa) << "\",\n";
        ss << "      \"value\": " << std::fixed << std::setprecision(4) << ayanamsaValue << ",\n";
        ss << "      \"date\": \"" << entries[0].getDateString() << "\"\n";
        ss << "    },\n";
    }
    ss << "    \"entries\": [\n";

    for (size_t i = 0; i < entries.size(); i++) {
        const auto& entry = entries[i];
        ss << "      {\n";
        ss << "        \"date\": \"" << entry.getDateString() << "\",\n";
        ss << "        \"julian_day\": " << std::fixed << std::setprecision(6) << entry.julianDay << ",\n";
        ss << "        \"planets\": {\n";

        for (size_t j = 0; j < config.planets.size(); j++) {
            Planet planet = config.planets[j];
            auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                                 [planet](const PlanetPosition& pos) { return pos.planet == planet; });

            if (it != entry.positions.end()) {
                ss << "          \"" << planetToString(planet) << "\": {\n";
                ss << "            \"longitude\": " << std::fixed << std::setprecision(6) << it->longitude << ",\n";
                ss << "            \"latitude\": " << std::fixed << std::setprecision(6) << it->latitude << ",\n";
                ss << "            \"speed\": " << std::fixed << std::setprecision(6) << it->speed << ",\n";
                ss << "            \"sign\": \"" << zodiacSignToString(it->sign) << "\",\n";
                ss << "            \"retrograde\": " << (isRetrograde(*it) ? "true" : "false") << "\n";
                ss << "          }";
                if (j < config.planets.size() - 1) ss << ",";
                ss << "\n";
            }
        }

        ss << "        }\n";
        ss << "      }";
        if (i < entries.size() - 1) ss << ",";
        ss << "\n";
    }

    ss << "    ]\n";
    ss << "  }\n";
    ss << "}\n";

    return ss.str();
}

// Helper functions

std::string EphemerisEntry::getDateString() const {
    std::stringstream ss;
    ss << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setw(2) << day;
    return ss.str();
}

std::string EphemerisEntry::getJulianDateString() const {
    // Convert Julian Day to Julian calendar date using Swiss Ephemeris
    int julYear, julMonth, julDay;
    double julHour;
    swe_revjul(julianDay, SE_JUL_CAL, &julYear, &julMonth, &julDay, &julHour);

    std::stringstream ss;
    ss << julYear << "-" << std::setfill('0') << std::setw(2) << julMonth << "-" << std::setw(2) << julDay;
    return ss.str();
}

std::string EphemerisEntry::getDayName() const {
    // Calculate day of week from Julian Day
    // Sunday = 0, Monday = 1, ..., Saturday = 6
    int dayOfWeek = ((int)(julianDay + 1.5)) % 7;

    static const std::string dayNames[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    return dayNames[dayOfWeek];
}

bool EphemerisEntry::shouldUseJulianCalendar() const {
    // Julian Day for October 15, 1582 (Gregorian calendar adoption date)
    // October 4, 1582 (Julian) = October 15, 1582 (Gregorian)
    const double gregorianAdoptionJD = 2299161.0; // October 15, 1582
    return julianDay < gregorianAdoptionJD;
}

double EphemerisEntry::getAyanamsaValue(AyanamsaType ayanamsa) const {
    // Set the ayanamsa mode in Swiss Ephemeris
    swe_set_sid_mode(ayanamsaTypeToSwissEphId(ayanamsa), 0, 0);

    // Get the ayanamsa value for this Julian Day
    return swe_get_ayanamsa_ut(julianDay);
}bool EphemerisTable::isRetrograde(const PlanetPosition& position) const {
    return position.speed < 0.0;
}

MotionType EphemerisTable::getMotionType(const PlanetPosition& position) const {
    double speed = std::abs(position.speed);

    // Retrograde motion
    if (position.speed < 0.0) {
        return MotionType::RETROGRADE;
    }

    // Define speed thresholds based on planet type
    double fastThreshold, normalThreshold, slowThreshold;

    switch (position.planet) {
        case Planet::SUN:
            fastThreshold = 1.1;   // Faster than normal solar motion
            normalThreshold = 0.9; // Normal solar motion (~1°/day)
            slowThreshold = 0.5;   // Slow solar motion
            break;
        case Planet::MOON:
            fastThreshold = 14.0;  // Fast lunar motion
            normalThreshold = 12.0; // Normal lunar motion (~13°/day)
            slowThreshold = 10.0;  // Slow lunar motion
            break;
        case Planet::MERCURY:
            fastThreshold = 2.5;
            normalThreshold = 1.0;
            slowThreshold = 0.3;
            break;
        case Planet::VENUS:
            fastThreshold = 1.5;
            normalThreshold = 0.8;
            slowThreshold = 0.2;
            break;
        case Planet::MARS:
            fastThreshold = 0.8;
            normalThreshold = 0.4;
            slowThreshold = 0.1;
            break;
        case Planet::JUPITER:
            fastThreshold = 0.3;
            normalThreshold = 0.1;
            slowThreshold = 0.03;
            break;
        case Planet::SATURN:
            fastThreshold = 0.15;
            normalThreshold = 0.08;
            slowThreshold = 0.02;
            break;
        default: // Outer planets
            fastThreshold = 0.1;
            normalThreshold = 0.05;
            slowThreshold = 0.01;
            break;
    }

    // Determine motion type based on speed
    if (speed < slowThreshold) {
        return MotionType::STATIONARY;
    } else if (speed > fastThreshold) {
        return MotionType::DIRECT_FAST;
    } else if (speed > normalThreshold) {
        return MotionType::DIRECT_NORMAL;
    } else {
        return MotionType::DIRECT_SLOW;
    }
}

std::string EphemerisTable::applyMotionColor(const std::string& text, MotionType motionType, bool useColors) const {
    if (!useColors) {
        return text;
    }

    std::stringstream ss;

    switch (motionType) {
        case MotionType::RETROGRADE:
            ss << AnsiColors::RED << text << AnsiColors::RESET;
            break;
        case MotionType::DIRECT_FAST:
            ss << AnsiColors::GREEN << text << AnsiColors::RESET;
            break;
        case MotionType::DIRECT_SLOW:
            ss << AnsiColors::YELLOW << text << AnsiColors::RESET;
            break;
        case MotionType::STATIONARY:
            ss << AnsiColors::BLUE << text << AnsiColors::RESET;
            break;
        case MotionType::DIRECT_NORMAL:
        default:
            return text; // No color for normal motion
    }

    return ss.str();
}

std::string EphemerisTable::formatColoredPosition(const PlanetPosition& position, const EphemerisConfig& config) const {
    // Get the basic formatted position
    std::string positionStr = formatPlanetPosition(position, config);

    // Apply motion-based coloring if enabled
    if (config.useColors) {
        MotionType motionType = getMotionType(position);
        positionStr = applyMotionColor(positionStr, motionType, true);
    }

    return positionStr;
}

std::string EphemerisTable::formatDegreeWithSign(double longitude) const {
    int degrees = static_cast<int>(longitude) % 30;
    int minutes = static_cast<int>((longitude - static_cast<int>(longitude)) * 60) % 60;
    ZodiacSign sign = longitudeToSign(longitude);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << degrees << "°" << std::setw(2) << minutes << "'";

    // Add sign symbol
    switch (sign) {
        case ZodiacSign::ARIES: oss << "♈"; break;
        case ZodiacSign::TAURUS: oss << "♉"; break;
        case ZodiacSign::GEMINI: oss << "♊"; break;
        case ZodiacSign::CANCER: oss << "♋"; break;
        case ZodiacSign::LEO: oss << "♌"; break;
        case ZodiacSign::VIRGO: oss << "♍"; break;
        case ZodiacSign::LIBRA: oss << "♎"; break;
        case ZodiacSign::SCORPIO: oss << "♏"; break;
        case ZodiacSign::SAGITTARIUS: oss << "♐"; break;
        case ZodiacSign::CAPRICORN: oss << "♑"; break;
        case ZodiacSign::AQUARIUS: oss << "♒"; break;
        case ZodiacSign::PISCES: oss << "♓"; break;
    }

    return oss.str();
}

std::string EphemerisTable::formatDeclination(double declination) const {
    bool isNegative = declination < 0.0;
    declination = std::abs(declination);

    int degrees = static_cast<int>(declination);
    int minutes = static_cast<int>((declination - degrees) * 60);

    std::ostringstream oss;
    oss << (isNegative ? "-" : "+") << std::setfill('0') << std::setw(2) << degrees
        << "°" << std::setw(2) << minutes << "'";

    return oss.str();
}

std::string EphemerisTable::formatLatitude(double latitude) const {
    bool isNegative = latitude < 0.0;
    latitude = std::abs(latitude);

    int degrees = static_cast<int>(latitude);
    int minutes = static_cast<int>((latitude - degrees) * 60);

    std::ostringstream oss;
    oss << (isNegative ? "-" : "+") << std::setfill('0') << std::setw(2) << degrees
        << "°" << std::setw(2) << minutes << "'";

    return oss.str();
}

std::string EphemerisTable::formatDistance(double distance) const {
    std::ostringstream oss;

    if (distance < 10.0) {
        // Show 4 decimal places for objects closer than 10 AU
        oss << std::fixed << std::setprecision(4) << distance << " AU";
    } else {
        // Show 2 decimal places for more distant objects
        oss << std::fixed << std::setprecision(2) << distance << " AU";
    }

    return oss.str();
}

std::string EphemerisTable::formatRightAscension(double rightAscension) const {
    // Convert from degrees to hours (1 hour = 15 degrees)
    double hours = rightAscension / 15.0;

    // Ensure value is in 0-24 hour range
    while (hours < 0.0) hours += 24.0;
    while (hours >= 24.0) hours -= 24.0;

    int h = static_cast<int>(hours);
    double minutesFloat = (hours - h) * 60.0;
    int m = static_cast<int>(minutesFloat);
    int s = static_cast<int>((minutesFloat - m) * 60.0);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << h
        << "h" << std::setw(2) << m
        << "m" << std::setw(2) << s << "s";

    return oss.str();
}

std::string EphemerisTable::formatSiderealTime(double siderealHours) const {
    int hours = static_cast<int>(siderealHours);
    double minutesFloat = (siderealHours - hours) * 60;
    int minutes = static_cast<int>(minutesFloat);
    int seconds = static_cast<int>((minutesFloat - minutes) * 60);

    // Keep within 24 hour range
    hours = hours % 24;
    if (hours < 0) hours += 24;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours
        << ":" << std::setw(2) << minutes
        << ":" << std::setw(2) << seconds;

    return oss.str();
}

std::vector<Planet> EphemerisTable::getDefaultPlanets() const {
    return {Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS, Planet::MARS,
            Planet::JUPITER, Planet::SATURN, Planet::URANUS, Planet::NEPTUNE, Planet::PLUTO};
}

std::string EphemerisTable::generateTable(const std::string& fromDate, const std::string& toDate, int intervalDays) const {
    EphemerisConfig config;

    // Initialize BirthData structures with default values
    config.startDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};
    config.endDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};

    // Parse date strings with BC era support
    parseBCDate(fromDate, config.startDate.year, config.startDate.month, config.startDate.day);
    parseBCDate(toDate, config.endDate.year, config.endDate.month, config.endDate.day);

    config.intervalDays = intervalDays;
    config.format = "table";

    return generateTable(config);
}

std::string EphemerisTable::generateCSVTable(const std::string& fromDate, const std::string& toDate, int intervalDays) const {
    EphemerisConfig config;

    // Initialize BirthData structures with default values
    config.startDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};
    config.endDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};

    // Parse date strings with BC era support
    parseBCDate(fromDate, config.startDate.year, config.startDate.month, config.startDate.day);
    parseBCDate(toDate, config.endDate.year, config.endDate.month, config.endDate.day);

    config.intervalDays = intervalDays;
    config.format = "csv";

    return exportToCSV(generateEntries(config), config);
}

std::string EphemerisTable::generateJSONTable(const std::string& fromDate, const std::string& toDate, int intervalDays) const {
    EphemerisConfig config;

    // Initialize BirthData structures with default values
    config.startDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};
    config.endDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};

    // Parse date strings with BC era support
    parseBCDate(fromDate, config.startDate.year, config.startDate.month, config.startDate.day);
    parseBCDate(toDate, config.endDate.year, config.endDate.month, config.endDate.day);

    config.intervalDays = intervalDays;
    config.format = "json";

    return exportToJSON(generateEntries(config), config);
}

std::string EphemerisTable::exportToHTML(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const {
    std::ostringstream html;

    html << "<!DOCTYPE html>\n";
    html << "<html><head><title>Ephemeris Table</title></head><body>\n";
    html << "<h1>Ephemeris Table</h1>\n";
    html << "<table border='1'>\n";

    // Header row
    html << "<tr><th>Date</th>";
    for (const auto& planet : config.planets) {
        html << "<th>" << planetToString(planet) << "</th>";
    }
    html << "</tr>\n";

    // Data rows
    for (const auto& entry : entries) {
        html << "<tr>";
        html << "<td>" << entry.getDateString() << "</td>";

        for (const auto& pos : entry.positions) {
            html << "<td>" << formatPlanetPosition(pos, config) << "</td>";
        }
        html << "</tr>\n";
    }

    html << "</table>\n</body></html>";
    return html.str();
}

std::string EphemerisTable::formatAsCompactTable(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const {
    if (entries.empty()) {
        return "No entries to display.\n";
    }

    std::stringstream ss;

    // Get year and determine title
    int year = entries[0].year;
    ss << "ASTRODIENST EPHEMERIS for the year " << year << "\n";

    // Show zodiac mode and ayanamsa info
    if (config.zodiacMode == ZodiacMode::SIDEREAL) {
        ss << "sidereal (" << ayanamsaTypeToString(config.ayanamsa) << " ayanamsa)\n\n";
    } else {
        ss << "geocentric\n\n";
    }

    // Group entries by month
    std::map<int, std::vector<EphemerisEntry>> monthlyEntries;
    for (const auto& entry : entries) {
        monthlyEntries[entry.month].push_back(entry);
    }

    for (const auto& monthPair : monthlyEntries) {
        int month = monthPair.first;
        const auto& monthEntries = monthPair.second;

        // Month header
        const char* monthNames[] = {"", "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
                                   "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"};
        ss << monthNames[month] << " " << year << std::string(50, ' ') << "00:00 UT\n";

        // Planet abbreviations header
        ss << "Day  ";
        for (Planet planet : config.planets) {
            ss << getPlanetAbbreviation(planet) << "     ";
        }
        if (config.showSiderealTime) {
            ss << "ST";
        }
        ss << "\n";

        // Days
        for (const auto& entry : monthEntries) {
            ss << std::setw(2) << std::setfill(' ') << entry.day << "   ";

            // Planet positions
            for (Planet planet : config.planets) {
                auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                                     [planet](const PlanetPosition& pos) { return pos.planet == planet; });
                if (it != entry.positions.end()) {
                    ss << formatColoredCompactPosition(*it, config) << " ";
                } else {
                    ss << "-----  ";
                }
            }

            // Sidereal time
            if (config.showSiderealTime) {
                ss << formatSiderealTime(entry.siderealTime);
            }
            ss << "\n";
        }
        ss << "\n";
    }

    return ss.str();
}

std::string EphemerisTable::getPlanetAbbreviation(Planet planet) const {
    switch (planet) {
        case Planet::SUN: return "Sun";
        case Planet::MOON: return "Moon";
        case Planet::MERCURY: return "Merc";
        case Planet::VENUS: return "Ven";
        case Planet::MARS: return "Mars";
        case Planet::JUPITER: return "Jup";
        case Planet::SATURN: return "Sat";
        case Planet::URANUS: return "Ura";
        case Planet::NEPTUNE: return "Nep";
        case Planet::PLUTO: return "Plu";
        default: return "???";
    }
}

std::string EphemerisTable::formatCompactPosition(const PlanetPosition& position) const {
    int degrees = static_cast<int>(position.longitude) % 30;
    int minutes = static_cast<int>((position.longitude - static_cast<int>(position.longitude)) * 60) % 60;

    // Get sign symbol
    char signChar = getSignCharacter(position.sign);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << degrees
        << std::setw(2) << minutes << signChar;

    return oss.str();
}

std::string EphemerisTable::formatColoredCompactPosition(const PlanetPosition& position, const EphemerisConfig& config) const {
    // Get the basic compact position
    std::string positionStr = formatCompactPosition(position);

    // Apply motion-based coloring if enabled
    if (config.useColors) {
        MotionType motionType = getMotionType(position);
        positionStr = applyMotionColor(positionStr, motionType, true);
    }

    return positionStr;
}

char EphemerisTable::getSignCharacter(ZodiacSign sign) const {
    switch (sign) {
        case ZodiacSign::ARIES: return 'a';
        case ZodiacSign::TAURUS: return 'b';
        case ZodiacSign::GEMINI: return 'c';
        case ZodiacSign::CANCER: return 'd';
        case ZodiacSign::LEO: return 'e';
        case ZodiacSign::VIRGO: return 'f';
        case ZodiacSign::LIBRA: return 'g';
        case ZodiacSign::SCORPIO: return 'h';
        case ZodiacSign::SAGITTARIUS: return 'i';
        case ZodiacSign::CAPRICORN: return 'j';
        case ZodiacSign::AQUARIUS: return 'k';
        case ZodiacSign::PISCES: return 'l';
        default: return '?';
    }
}

// Static helper methods for configuration
EphemerisConfig EphemerisTable::createSiderealConfig(AyanamsaType ayanamsa) {
    EphemerisConfig config;
    config.zodiacMode = ZodiacMode::SIDEREAL;
    config.ayanamsa = ayanamsa;
    return config;
}

EphemerisConfig EphemerisTable::createTropicalConfig() {
    EphemerisConfig config;
    config.zodiacMode = ZodiacMode::TROPICAL;
    // ayanamsa is not used for tropical calculations
    return config;
}

// Helper function to calculate visual width excluding ANSI codes
size_t EphemerisTable::getVisualWidth(const std::string& text) const {
    size_t visualWidth = 0;
    bool inEscapeSequence = false;

    for (size_t i = 0; i < text.length(); i++) {
        if (text[i] == '\033') {
            inEscapeSequence = true;
        } else if (inEscapeSequence && text[i] == 'm') {
            inEscapeSequence = false;
        } else if (!inEscapeSequence) {
            visualWidth++;
        }
    }

    return visualWidth;
}

// Helper function to pad string to specific width accounting for ANSI codes
std::string EphemerisTable::padStringToWidth(const std::string& text, int width, bool leftAlign) const {
    size_t visualWidth = getVisualWidth(text);

    if (static_cast<int>(visualWidth) >= width) {
        return text; // Already wide enough or too wide
    }

    int paddingNeeded = width - static_cast<int>(visualWidth);
    std::string padding(paddingNeeded, ' ');

    if (leftAlign) {
        return text + padding;
    } else {
        return padding + text;
    }
}

} // namespace Astro
