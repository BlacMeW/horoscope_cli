#include "horoscope_calculator.h"
#include "eastern_chart_drawer.h"
#include "solar_system_drawer.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>

using namespace Astro;

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
    bool showHelp = false;
    bool showVersion = false;
    bool showSolarSystemOnly = false;
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
    std::cout << "Optional:\n";
    std::cout << "  --house-system SYS House system: P=Placidus, K=Koch, E=Equal, W=Whole Sign,\n";
    std::cout << "                     C=Campanus, R=Regiomontanus (default: P)\n";
    std::cout << "  --output FORMAT    Output format: text or json (default: text)\n";
    std::cout << "  --chart-style STY  Chart style: western, north-indian, south-indian, east-indian, solar-system (default: western)\n";
    std::cout << "  --ephe-path PATH   Path to Swiss Ephemeris data files\n";
    std::cout << "  --solar-system     Show just the solar system orbital paths (no birth data needed)\n";
    std::cout << "  --help, -h         Show this help message\n";
    std::cout << "  --version, -v      Show version information\n\n";
    std::cout << "Examples:\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5\n";
    std::cout << "  horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --house-system K\n";
    std::cout << "  horoscope_cli --date 1975-12-03 --time 22:45:00 --lat -33.8688 --lon 151.2093 --timezone 10 --output json\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style north-indian\n";
    std::cout << "  horoscope_cli --date 1985-06-20 --time 09:15:30 --lat 51.5074 --lon -0.1278 --timezone 1 --chart-style south-indian\n";
    std::cout << "  horoscope_cli --date 1869-10-02 --time 07:45:00 --lat 21.6416 --lon 69.6293 --timezone 5.5 --chart-style east-indian\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system\n";
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
        } else if (arg == "--solar-system") {
            args.showSolarSystemOnly = true;
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
            std::cout << solarDrawer.drawSolarSystem(chart) << std::endl;
        } else {
            // Display traditional Western chart first
            std::cout << chart.getFormattedChart() << std::endl;

            // Add Eastern chart if requested
            if (args.chartStyle == "north-indian" || args.chartStyle == "south-indian" || args.chartStyle == "east-indian") {
                EasternChartDrawer chartDrawer;
                chartDrawer.setChartStyle(args.chartStyle);
                std::cout << chartDrawer.drawEasternChart(chart) << std::endl;
            }
        }
    }

    return 0;
}
