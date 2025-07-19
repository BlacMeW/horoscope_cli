#include "horoscope_calculator.h"
#include "birth_chart.h"
#include "eastern_chart_drawer.h"
#include "western_chart_drawer.h"
#include "solar_system_drawer.h"
#include "eclipse_calculator.h"
#include "conjunction_calculator.h"
#include "ephemeris_table.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <chrono>
#include <ctime>

using namespace Astro;

SolarSystemPerspective stringToPerspective(const std::string& perspectiveStr) {
    if (perspectiveStr == "geocentric") return SolarSystemPerspective::GEOCENTRIC;
    else if (perspectiveStr == "mars-centric") return SolarSystemPerspective::MARS_CENTRIC;
    else if (perspectiveStr == "jupiter-centric") return SolarSystemPerspective::JUPITER_CENTRIC;
    else if (perspectiveStr == "saturn-centric") return SolarSystemPerspective::SATURN_CENTRIC;
    else return SolarSystemPerspective::HELIOCENTRIC; // default
}

struct CommandLineArgs {
    std::string date;
    std::string time;
    double latitude = 0.0;
    double longitude = 0.0;
    double timezone = 0.0;
    HouseSystem houseSystem = HouseSystem::PLACIDUS;
    std::string outputFormat = "text";
    std::string chartStyle = "western";
    std::string ephemerisPath;
    std::string solarSystemPerspective = "heliocentric";
    bool showHelp = false;
    bool showVersion = false;
    bool showSolarSystemOnly = false;

    // New eclipse and conjunction features
    bool showEclipses = false;
    bool showConjunctions = false;
    bool showEphemerisTable = false;
    std::string eclipseFromDate;
    std::string eclipseToDate;
    int eclipseYearsBefore = 1;
    int eclipseYearsAfter = 1;
    std::string conjunctionFromDate;
    std::string conjunctionToDate;
    double conjunctionMaxOrb = 3.0;
    std::string ephemerisFromDate;
    std::string ephemerisToDate;
    int ephemerisIntervalDays = 1;
    std::string ephemerisFormat = "table";
};

void printHelp() {
    std::cout << "Horoscope CLI - Birth Chart Calculator using Swiss Ephemeris\n\n";
    std::cout << "Usage: horoscope_cli [OPTIONS]\n\n";
    std::cout << "Required Options:\n";
    std::cout << "  --date DATE        Birth date in YYYY-MM-DD format\n";
    std::cout << "  --time TIME        Birth time in HH:MM:SS format (24-hour)\n";
    std::cout << "  --lat LATITUDE     Latitude in decimal degrees (-90 to 90)\n";
    std::cout << "  --lon LONGITUDE    Longitude in decimal degrees (-180 to 180)\n";
    std::cout << "  --timezone HOURS   Timezone offset from UTC in hours\n\n";
    std::cout << "Chart Options:\n";
    std::cout << "  --house-system SYS House system: P=Placidus, K=Koch, E=Equal, W=Whole Sign,\n";
    std::cout << "                     C=Campanus, R=Regiomontanus (default: P)\n";
    std::cout << "  --output FORMAT    Output format: text or json (default: text)\n";
    std::cout << "  --chart-style STY  Chart style: western, north-indian, south-indian, east-indian, solar-system (default: western)\n";
    std::cout << "  --perspective PER  Solar system perspective: heliocentric, geocentric, mars-centric, jupiter-centric, saturn-centric (default: heliocentric)\n\n";
    std::cout << "Eclipse Options:\n";
    std::cout << "  --eclipses         Show eclipses near birth date\n";
    std::cout << "  --eclipse-range FROM TO  Show eclipses in date range (YYYY-MM-DD format)\n";
    std::cout << "  --eclipse-years-before N Years before birth to search (default: 1)\n";
    std::cout << "  --eclipse-years-after N  Years after birth to search (default: 1)\n\n";
    std::cout << "Conjunction Options:\n";
    std::cout << "  --conjunctions     Show planetary conjunctions near birth date\n";
    std::cout << "  --conjunction-range FROM TO  Show conjunctions in date range (YYYY-MM-DD format)\n";
    std::cout << "  --conjunction-orb DEGREES    Maximum orb for conjunctions (default: 3.0)\n\n";
    std::cout << "Ephemeris Table Options:\n";
    std::cout << "  --ephemeris        Generate ephemeris table\n";
    std::cout << "  --ephemeris-range FROM TO    Date range for ephemeris (YYYY-MM-DD format)\n";
    std::cout << "  --ephemeris-interval DAYS    Interval between entries (default: 1)\n";
    std::cout << "  --ephemeris-format FORMAT    Format: table, csv, json (default: table)\n\n";
    std::cout << "General Options:\n";
    std::cout << "  --ephe-path PATH   Path to Swiss Ephemeris data files\n";
    std::cout << "  --solar-system     Show just the solar system orbital paths (no birth data needed)\n";
    std::cout << "  --help, -h         Show this help message\n";
    std::cout << "  --version, -v      Show version information\n\n";
    std::cout << "Examples:\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5\n";
    std::cout << "  horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --house-system K\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --eclipses\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --conjunctions --conjunction-orb 2.0\n";
    std::cout << "  horoscope_cli --ephemeris --ephemeris-range 2025-01-01 2025-01-31 --ephemeris-format csv\n";
    std::cout << "  horoscope_cli --eclipse-range 2024-01-01 2024-12-31 --lat 40.7128 --lon -74.0060\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective geocentric\n";
    std::cout << "  horoscope_cli --solar-system\n";
}

void printVersion() {
    std::cout << "Horoscope CLI v1.0.0\n";
    std::cout << "Built with Swiss Ephemeris\n";
    std::cout << "Copyright (c) 2025\n";
}

bool parseDate(const std::string& dateStr, int& year, int& month, int& day) {
    if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return false;
    }

    try {
        year = std::stoi(dateStr.substr(0, 4));
        month = std::stoi(dateStr.substr(5, 2));
        day = std::stoi(dateStr.substr(8, 2));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool parseTime(const std::string& timeStr, int& hour, int& minute, int& second) {
    if (timeStr.length() != 8 || timeStr[2] != ':' || timeStr[5] != ':') {
        return false;
    }

    try {
        hour = std::stoi(timeStr.substr(0, 2));
        minute = std::stoi(timeStr.substr(3, 2));
        second = std::stoi(timeStr.substr(6, 2));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

HouseSystem parseHouseSystem(const std::string& hsStr) {
    if (hsStr.empty()) return HouseSystem::PLACIDUS;

    char hs = std::toupper(hsStr[0]);
    switch (hs) {
        case 'P': return HouseSystem::PLACIDUS;
        case 'K': return HouseSystem::KOCH;
        case 'E': return HouseSystem::EQUAL;
        case 'W': return HouseSystem::WHOLE_SIGN;
        case 'C': return HouseSystem::CAMPANUS;
        case 'R': return HouseSystem::REGIOMONTANUS;
        default: return HouseSystem::PLACIDUS;
    }
}

bool parseCommandLine(int argc, char* argv[], CommandLineArgs& args) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
            return true;
        } else if (arg == "--version" || arg == "-v") {
            args.showVersion = true;
            return true;
        } else if (arg == "--date" && i + 1 < argc) {
            args.date = argv[++i];
        } else if (arg == "--time" && i + 1 < argc) {
            args.time = argv[++i];
        } else if (arg == "--lat" && i + 1 < argc) {
            try {
                args.latitude = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid latitude value\n";
                return false;
            }
        } else if (arg == "--lon" && i + 1 < argc) {
            try {
                args.longitude = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid longitude value\n";
                return false;
            }
        } else if (arg == "--timezone" && i + 1 < argc) {
            try {
                args.timezone = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid timezone value\n";
                return false;
            }
        } else if (arg == "--house-system" && i + 1 < argc) {
            args.houseSystem = parseHouseSystem(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            args.outputFormat = argv[++i];
            if (args.outputFormat != "text" && args.outputFormat != "json") {
                std::cerr << "Error: Output format must be 'text' or 'json'\n";
                return false;
            }
        } else if (arg == "--chart-style" && i + 1 < argc) {
            args.chartStyle = argv[++i];
            if (args.chartStyle != "western" && args.chartStyle != "north-indian" &&
                args.chartStyle != "south-indian" && args.chartStyle != "east-indian" &&
                args.chartStyle != "solar-system") {
                std::cerr << "Error: Chart style must be 'western', 'north-indian', 'south-indian', 'east-indian', or 'solar-system'\n";
                return false;
            }
        } else if (arg == "--ephe-path" && i + 1 < argc) {
            args.ephemerisPath = argv[++i];
        } else if (arg == "--perspective" && i + 1 < argc) {
            args.solarSystemPerspective = argv[++i];
            if (args.solarSystemPerspective != "heliocentric" && args.solarSystemPerspective != "geocentric" &&
                args.solarSystemPerspective != "mars-centric" && args.solarSystemPerspective != "jupiter-centric" &&
                args.solarSystemPerspective != "saturn-centric") {
                std::cerr << "Error: Perspective must be 'heliocentric', 'geocentric', 'mars-centric', 'jupiter-centric', or 'saturn-centric'\n";
                return false;
            }
        } else if (arg == "--solar-system") {
            args.showSolarSystemOnly = true;
        } else if (arg == "--eclipses") {
            args.showEclipses = true;
        } else if (arg == "--eclipse-range" && i + 2 < argc) {
            args.eclipseFromDate = argv[++i];
            args.eclipseToDate = argv[++i];
            args.showEclipses = true;
        } else if (arg == "--eclipse-years-before" && i + 1 < argc) {
            try {
                args.eclipseYearsBefore = std::stoi(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid eclipse years before value\n";
                return false;
            }
        } else if (arg == "--eclipse-years-after" && i + 1 < argc) {
            try {
                args.eclipseYearsAfter = std::stoi(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid eclipse years after value\n";
                return false;
            }
        } else if (arg == "--conjunctions") {
            args.showConjunctions = true;
        } else if (arg == "--conjunction-range" && i + 2 < argc) {
            args.conjunctionFromDate = argv[++i];
            args.conjunctionToDate = argv[++i];
            args.showConjunctions = true;
        } else if (arg == "--conjunction-orb" && i + 1 < argc) {
            try {
                args.conjunctionMaxOrb = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid conjunction orb value\n";
                return false;
            }
        } else if (arg == "--ephemeris") {
            args.showEphemerisTable = true;
        } else if (arg == "--ephemeris-range" && i + 2 < argc) {
            args.ephemerisFromDate = argv[++i];
            args.ephemerisToDate = argv[++i];
            args.showEphemerisTable = true;
        } else if (arg == "--ephemeris-interval" && i + 1 < argc) {
            try {
                args.ephemerisIntervalDays = std::stoi(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid ephemeris interval value\n";
                return false;
            }
        } else if (arg == "--ephemeris-format" && i + 1 < argc) {
            args.ephemerisFormat = argv[++i];
            if (args.ephemerisFormat != "table" && args.ephemerisFormat != "csv" && args.ephemerisFormat != "json") {
                std::cerr << "Error: Ephemeris format must be 'table', 'csv', or 'json'\n";
                return false;
            }
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            return false;
        }
    }

    return true;
}

bool validateArgs(const CommandLineArgs& args) {
    if (args.showHelp || args.showVersion || args.showSolarSystemOnly) {
        return true;
    }

    // Eclipse and ephemeris features can work without full birth data
    if (args.showEclipses || args.showConjunctions || args.showEphemerisTable) {
        // For eclipse and conjunction range queries, we need coordinates
        if ((!args.eclipseFromDate.empty() || !args.conjunctionFromDate.empty()) &&
            (args.latitude < -90.0 || args.latitude > 90.0 ||
             args.longitude < -180.0 || args.longitude > 180.0)) {
            std::cerr << "Error: Valid latitude (-90 to 90) and longitude (-180 to 180) required for eclipse/conjunction searches\n";
            return false;
        }

        // For ephemeris tables, coordinates are optional
        // For birth-related eclipse searches, we need birth data
        if (args.showEclipses && args.eclipseFromDate.empty() &&
            (args.date.empty() || args.time.empty())) {
            std::cerr << "Error: Birth date and time required for eclipse searches around birth\n";
            return false;
        }

        return true;
    }

    if (args.date.empty()) {
        std::cerr << "Error: --date is required\n";
        return false;
    }

    if (args.time.empty()) {
        std::cerr << "Error: --time is required\n";
        return false;
    }

    if (args.latitude < -90.0 || args.latitude > 90.0) {
        std::cerr << "Error: Latitude must be between -90 and 90 degrees\n";
        return false;
    }

    if (args.longitude < -180.0 || args.longitude > 180.0) {
        std::cerr << "Error: Longitude must be between -180 and 180 degrees\n";
        return false;
    }

    if (args.timezone < -12.0 || args.timezone > 14.0) {
        std::cerr << "Error: Timezone must be between -12 and +14 hours\n";
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    CommandLineArgs args;

    if (!parseCommandLine(argc, argv, args)) {
        return 1;
    }

    if (args.showHelp) {
        printHelp();
        return 0;
    }

    if (args.showVersion) {
        printVersion();
        return 0;
    }

    if (!validateArgs(args)) {
        std::cerr << "Use --help for usage information\n";
        return 1;
    }

    try {
        // Initialize ephemeris for special features
        EphemerisManager ephemerisManager;
        ephemerisManager.initialize(args.ephemerisPath);

        // Handle eclipse calculations
        if (args.showEclipses) {
            EclipseCalculator eclipseCalc;

            std::string fromDate = args.eclipseFromDate;
            std::string toDate = args.eclipseToDate;

            // If no range specified, use birth date
            if (fromDate.empty()) {
                fromDate = args.date;
                // Default to 1 year after birth if no end date
                if (toDate.empty()) {
                    struct tm tm = {};
                    if (strptime(fromDate.c_str(), "%Y-%m-%d", &tm)) {
                        tm.tm_year += 1;  // Add 1 year
                        char buffer[16];
                        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
                        toDate = std::string(buffer);
                    }
                }
            }

            if (toDate.empty()) {
                // Default to 1 year from start date
                struct tm tm = {};
                if (strptime(fromDate.c_str(), "%Y-%m-%d", &tm)) {
                    tm.tm_year += 1;
                    char buffer[16];
                    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
                    toDate = std::string(buffer);
                }
            }

            std::vector<EclipseEvent> eclipses = eclipseCalc.findEclipses(fromDate, toDate, args.latitude, args.longitude);

            std::cout << "\nEclipse Events (" << fromDate << " to " << toDate << "):\n";
            std::cout << std::string(80, '=') << std::endl;

            for (const auto& eclipse : eclipses) {
                eclipseCalc.printEclipseEvent(eclipse);
                std::cout << std::string(80, '-') << std::endl;
            }

            if (eclipses.empty()) {
                std::cout << "No eclipses found in the specified period.\n";
            }
        }

        // Handle conjunction calculations
        if (args.showConjunctions) {
            ConjunctionCalculator conjCalc;
            conjCalc.setMaximumOrb(args.conjunctionMaxOrb);

            std::string fromDate = args.conjunctionFromDate;
            std::string toDate = args.conjunctionToDate;

            // If no range specified, use birth date
            if (fromDate.empty()) {
                fromDate = args.date;
                if (toDate.empty()) {
                    struct tm tm = {};
                    if (strptime(fromDate.c_str(), "%Y-%m-%d", &tm)) {
                        tm.tm_year += 1;  // Add 1 year
                        char buffer[16];
                        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
                        toDate = std::string(buffer);
                    }
                }
            }

            if (toDate.empty()) {
                struct tm tm = {};
                if (strptime(fromDate.c_str(), "%Y-%m-%d", &tm)) {
                    tm.tm_year += 1;
                    char buffer[16];
                    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
                    toDate = std::string(buffer);
                }
            }

            std::vector<ConjunctionEvent> conjunctions = conjCalc.findConjunctions(fromDate, toDate);

            std::cout << "\nPlanetary Conjunctions (" << fromDate << " to " << toDate << "):\n";
            std::cout << "Orb: " << args.conjunctionMaxOrb << "°\n";
            std::cout << std::string(80, '=') << std::endl;

            for (const auto& conjunction : conjunctions) {
                conjCalc.printConjunctionEvent(conjunction);
                std::cout << std::string(80, '-') << std::endl;
            }

            if (conjunctions.empty()) {
                std::cout << "No conjunctions found in the specified period.\n";
            }
        }

        // Handle ephemeris table generation
        if (args.showEphemerisTable) {
            EphemerisTable ephemTable;

            // Initialize ephemeris table
            if (!ephemTable.initialize(args.ephemerisPath.empty() ? "data" : args.ephemerisPath)) {
                std::cerr << "Error: Failed to initialize ephemeris table" << std::endl;
                return 1;
            }

            std::string fromDate = args.ephemerisFromDate;
            std::string toDate = args.ephemerisToDate;

            // Default to current month if no dates specified
            if (fromDate.empty()) {
                auto now = std::chrono::system_clock::now();
                auto time_t = std::chrono::system_clock::to_time_t(now);
                struct tm* tm = gmtime(&time_t);

                char buffer[16];
                strftime(buffer, sizeof(buffer), "%Y-%m-01", tm);
                fromDate = std::string(buffer);

                if (toDate.empty()) {
                    tm->tm_mon += 1;
                    if (tm->tm_mon > 11) {
                        tm->tm_mon = 0;
                        tm->tm_year++;
                    }
                    strftime(buffer, sizeof(buffer), "%Y-%m-01", tm);
                    toDate = std::string(buffer);
                }
            }

            // Generate ephemeris table
            std::string result;
            if (args.ephemerisFormat == "csv") {
                result = ephemTable.generateCSVTable(fromDate, toDate, args.ephemerisIntervalDays);
            } else if (args.ephemerisFormat == "json") {
                result = ephemTable.generateJSONTable(fromDate, toDate, args.ephemerisIntervalDays);
            } else {
                result = ephemTable.generateTable(fromDate, toDate, args.ephemerisIntervalDays);
            }

            if (!result.empty()) {
                std::cout << result << std::endl;
            } else {
                std::cout << "Failed to generate ephemeris table" << std::endl;
            }
        }

        // Return early if only special features were requested
        if (args.showEclipses || args.showConjunctions || args.showEphemerisTable) {
            return 0;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in astronomical calculations: " << e.what() << std::endl;
        return 1;
    }

    // Handle solar system only display
    if (args.showSolarSystemOnly) {
        SolarSystemDrawer solarDrawer;
        solarDrawer.setShowOrbits(true);
        solarDrawer.setShowPlanetNames(true);
        std::cout << solarDrawer.drawOrbitalPaths() << std::endl;
        return 0;
    }

    // Parse date and time
    BirthData birthData;
    if (!parseDate(args.date, birthData.year, birthData.month, birthData.day)) {
        std::cerr << "Error: Invalid date format. Use YYYY-MM-DD\n";
        return 1;
    }

    if (!parseTime(args.time, birthData.hour, birthData.minute, birthData.second)) {
        std::cerr << "Error: Invalid time format. Use HH:MM:SS\n";
        return 1;
    }

    birthData.latitude = args.latitude;
    birthData.longitude = args.longitude;
    birthData.timezone = args.timezone;

    // Initialize horoscope calculator
    HoroscopeCalculator calculator;
    if (!calculator.initialize(args.ephemerisPath)) {
        std::cerr << "Error: Failed to initialize calculator: " << calculator.getLastError() << "\n";
        std::cerr << "Note: You may need to install Swiss Ephemeris data files or specify the path with --ephe-path\n";
        return 1;
    }

    // Calculate birth chart
    BirthChart chart;
    if (!calculator.calculateBirthChart(birthData, args.houseSystem, chart)) {
        std::cerr << "Error: Failed to calculate birth chart: " << calculator.getLastError() << "\n";
        return 1;
    }

    // Output results
    if (args.outputFormat == "json") {
        std::cout << chart.exportToJson() << std::endl;
    } else {
        // Handle different chart styles
        if (args.chartStyle == "solar-system") {
            // Display solar system view
            SolarSystemDrawer solarDrawer;
            solarDrawer.setShowOrbits(true);
            solarDrawer.setShowPlanetNames(true);
            solarDrawer.setShowDistances(true);
            solarDrawer.setPerspective(stringToPerspective(args.solarSystemPerspective));
            std::cout << solarDrawer.drawSolarSystem(chart) << std::endl;
        } else {
            // Display appropriate chart style
            if (args.chartStyle == "western") {
                WesternChartDrawer westernDrawer;
                std::cout << westernDrawer.drawChartWheel(chart) << std::endl;
                std::cout << westernDrawer.drawRectangularChart(chart) << std::endl;
                std::cout << westernDrawer.drawAspectGrid(chart) << std::endl;
            } else if (args.chartStyle == "north-indian" || args.chartStyle == "south-indian" || args.chartStyle == "east-indian") {
                // Display traditional Western chart first
                std::cout << chart.getFormattedChart() << std::endl;

                // Add Eastern chart
                EasternChartDrawer chartDrawer;
                chartDrawer.setChartStyle(args.chartStyle);
                std::cout << chartDrawer.drawEasternChart(chart) << std::endl;
            } else {
                // Default to basic text output
                std::cout << chart.getFormattedChart() << std::endl;
            }
        }
    }

    return 0;
}
