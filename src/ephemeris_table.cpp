#include "ephemeris_table.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "astro_types.h"
#include <swephexp.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace Astro {

EphemerisConfig::EphemerisConfig()
    : intervalDays(1), showDegreeMinutes(true), showSign(true), showRetrograde(true),
      showSpeed(false), showDistance(false), showLatitude(false),
      showDeclination(false), showRightAscension(false), format("table") {

    // Default planets
    planets = {Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS, Planet::MARS,
               Planet::JUPITER, Planet::SATURN, Planet::URANUS, Planet::NEPTUNE, Planet::PLUTO};
}

EphemerisTable::EphemerisTable() : isInitialized(false) {
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

        // Calculate planet positions for this date
        BirthData entryDate = {year, month, day, hour, minute, static_cast<int>(second), 0.0, 0.0, 0.0};
        calc.calculateAllPlanets(entryDate, entry.positions);

        entries.push_back(entry);
    }

    return entries;
}

std::string EphemerisTable::formatAsTable(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const {
    if (entries.empty()) {
        return "No entries to display.\n";
    }

    std::stringstream ss;
    ss << "\n=== EPHEMERIS TABLE ===\n";
    ss << "Period: " << config.startDate.getDateTimeString() << " to " << config.endDate.getDateTimeString() << "\n";
    ss << "Interval: " << config.intervalDays << " day(s)\n\n";

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
    if (config.showRetrograde) {
        ss << "\nR = Retrograde motion\n";
    }
    if (config.showSign) {
        ss << "Signs: ♈Ari ♉Tau ♊Gem ♋Can ♌Leo ♍Vir ♎Lib ♏Sco ♐Sag ♑Cap ♒Aqu ♓Pis\n";
    }

    return ss.str();
}

std::vector<std::string> EphemerisTable::getColumnHeaders(const EphemerisConfig& config) const {
    std::vector<std::string> headers;
    headers.push_back("Date");

    for (Planet planet : config.planets) {
        std::string header = planetToString(planet);
        if (config.showSign && config.showDegreeMinutes) {
            header += " (°Sign)";
        } else if (config.showSign) {
            header += " Sign";
        } else if (config.showDegreeMinutes) {
            header += " (°)";
        }
        headers.push_back(header);
    }

    return headers;
}

std::vector<int> EphemerisTable::calculateColumnWidths(const std::vector<EphemerisEntry>& entries,
                                                      const EphemerisConfig& config) const {
    std::vector<int> widths;

    // Date column width
    widths.push_back(12);

    // Planet column widths
    for (size_t i = 0; i < config.planets.size(); i++) {
        int width = 8; // Minimum width
        if (config.showSign && config.showDegreeMinutes) width = 12;
        else if (config.showSign) width = 10;
        else if (config.showDegreeMinutes) width = 8;

        if (config.showRetrograde) width += 1;
        if (config.showSpeed) width += 4;

        widths.push_back(width);
    }

    return widths;
}

std::string EphemerisTable::formatTableHeader(const std::vector<std::string>& headers,
                                            const std::vector<int>& widths) const {
    std::stringstream ss;
    for (size_t i = 0; i < headers.size() && i < widths.size(); i++) {
        ss << std::left << std::setw(widths[i]) << headers[i];
        if (i < headers.size() - 1) ss << " ";
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
    std::stringstream ss;

    // Date column
    ss << std::left << std::setw(widths[0]) << entry.getDateString();
    if (widths.size() > 1) ss << " ";

    // Planet columns
    for (size_t i = 0; i < config.planets.size() && i + 1 < widths.size(); i++) {
        Planet planet = config.planets[i];

        // Find position for this planet
        auto it = std::find_if(entry.positions.begin(), entry.positions.end(),
                             [planet](const PlanetPosition& pos) { return pos.planet == planet; });

        std::string planetStr;
        if (it != entry.positions.end()) {
            planetStr = formatPlanetPosition(*it, config);
        } else {
            planetStr = "---";
        }

        ss << std::left << std::setw(widths[i + 1]) << planetStr;
        if (i < config.planets.size() - 1) ss << " ";
    }
    ss << "\n";
    return ss.str();
}

std::string EphemerisTable::formatPlanetPosition(const PlanetPosition& position, const EphemerisConfig& config) const {
    std::stringstream ss;

    if (config.showDegreeMinutes) {
        ss << formatDegreeWithSign(position.longitude);
    } else if (config.showSign) {
        ss << zodiacSignToString(position.sign);
    } else {
        ss << std::fixed << std::setprecision(2) << position.longitude;
    }

    if (config.showRetrograde && isRetrograde(position)) {
        ss << "R";
    }

    if (config.showSpeed) {
        ss << " (" << std::fixed << std::setprecision(2) << position.speed << ")";
    }

    return ss.str();
}

std::string EphemerisTable::exportToCSV(const std::vector<EphemerisEntry>& entries, const EphemerisConfig& config) const {
    std::stringstream ss;

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

bool EphemerisTable::isRetrograde(const PlanetPosition& position) const {
    return position.speed < 0.0;
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

} // namespace Astro
