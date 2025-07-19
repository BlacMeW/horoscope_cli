#include "horoscope_calculator.h"
#include "birth_chart.h"
#include "eastern_chart_drawer.h"
#include "western_chart_drawer.h"
#include "solar_system_drawer.h"
#include "eclipse_calculator.h"
#include "conjunction_calculator.h"
#include "ephemeris_table.h"
#include "kp_system.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <algorithm>

using namespace Astro;

SolarSystemPerspective stringToPerspective(const std::string& perspectiveStr) {
    if (perspectiveStr == "geocentric") return SolarSystemPerspective::GEOCENTRIC;
    else if (perspectiveStr == "mars-centric") return SolarSystemPerspective::MARS_CENTRIC;
    else if (perspectiveStr == "jupiter-centric") return SolarSystemPerspective::JUPITER_CENTRIC;
    else if (perspectiveStr == "saturn-centric") return SolarSystemPerspective::SATURN_CENTRIC;
    else return SolarSystemPerspective::HELIOCENTRIC; // default
}

Planet stringToPlanet(const std::string& planetStr) {
    std::string lower = planetStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "sun") return Planet::SUN;
    else if (lower == "moon") return Planet::MOON;
    else if (lower == "mercury") return Planet::MERCURY;
    else if (lower == "venus") return Planet::VENUS;
    else if (lower == "mars") return Planet::MARS;
    else if (lower == "jupiter") return Planet::JUPITER;
    else if (lower == "saturn") return Planet::SATURN;
    else if (lower == "uranus") return Planet::URANUS;
    else if (lower == "neptune") return Planet::NEPTUNE;
    else if (lower == "pluto") return Planet::PLUTO;
    else if (lower == "north_node" || lower == "rahu") return Planet::NORTH_NODE;
    else if (lower == "south_node" || lower == "ketu") return Planet::SOUTH_NODE;
    else if (lower == "chiron") return Planet::CHIRON;
    else if (lower == "lilith") return Planet::LILITH;
    else return Planet::SUN; // default
}

struct CommandLineArgs {
    std::string date;
    std::string time;
    double latitude = 0.0;
    double longitude = 0.0;
    double timezone = 0.0;
    HouseSystem houseSystem = HouseSystem::PLACIDUS;
    std::string outputFormat = "text";
    std::string chartStyle = "";
    std::string ephemerisPath;
    std::string solarSystemPerspective = "heliocentric";
    bool showHelp = false;
    bool showVersion = false;
    bool showFeatures = false;
    bool showSolarSystemOnly = false;
    bool noDrawing = false;

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

    // KP System options
    bool showKPTable = false;
    bool showKPTransitions = false;
    std::string kpTransitionFromDate;
    std::string kpTransitionToDate;
    std::string kpTransitionPlanet = "all";
    std::string kpTransitionLevel = "all";
    std::string kpOutputFormat = "table";
};

void printHelp() {
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "             🌟 HOROSCOPE CLI - Professional Astrology Tool ⭐\n";
    std::cout << "                   Swiss Ephemeris • High Precision 🔮\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n\n";

    std::cout << "SYNOPSIS\n";
    std::cout << "    horoscope_cli [REQUIRED OPTIONS] [CHART OPTIONS] [FEATURE OPTIONS]\n\n";

    std::cout << "DESCRIPTION 📖\n";
    std::cout << "    A professional-grade astronomical calculation tool that generates birth charts,\n";
    std::cout << "    calculates planetary positions, finds eclipses and conjunctions, and provides\n";
    std::cout << "    comprehensive astrological analysis using the Swiss Ephemeris library. ✨\n\n";

    std::cout << "REQUIRED OPTIONS 📅\n";
    std::cout << "    --date DATE        Birth date in YYYY-MM-DD format\n";
    std::cout << "                       • Use -YYYY-MM-DD for BC dates (e.g., -0044-03-15)\n";
    std::cout << "                       • Supports dates from 6000 BC to 7000 AD\n";
    std::cout << "                       • Examples: 1990-01-15, -0500-12-25\n\n";

    std::cout << "    --time TIME        Birth time in HH:MM:SS format (24-hour)\n";
    std::cout << "                       • Use local time at birth location\n";
    std::cout << "                       • Examples: 14:30:00, 09:15:30, 23:45:12\n\n";

    std::cout << "    --lat LATITUDE     Latitude in decimal degrees\n";
    std::cout << "                       • Range: -90.0 (South Pole) to +90.0 (North Pole)\n";
    std::cout << "                       • Positive = North, Negative = South\n";
    std::cout << "                       • Examples: 40.7128 (NYC), -33.8688 (Sydney)\n\n";

    std::cout << "    --lon LONGITUDE    Longitude in decimal degrees\n";
    std::cout << "                       • Range: -180.0 (West) to +180.0 (East)\n";
    std::cout << "                       • Positive = East, Negative = West\n";
    std::cout << "                       • Examples: -74.0060 (NYC), 151.2093 (Sydney)\n\n";

    std::cout << "    --timezone HOURS   Timezone offset from UTC in hours\n";
    std::cout << "                       • Range: -12.0 to +14.0\n";
    std::cout << "                       • Use time zone at birth time (consider DST)\n";
    std::cout << "                       • Examples: -5 (EST), 1 (CET), 9 (JST)\n\n";

    std::cout << "CHART GENERATION OPTIONS 🎨\n";
    std::cout << "    --house-system SYS House system to use (default: P)\n";
    std::cout << "                       P = Placidus (most common)\n";
    std::cout << "                       K = Koch\n";
    std::cout << "                       E = Equal House\n";
    std::cout << "                       W = Whole Sign\n";
    std::cout << "                       C = Campanus\n";
    std::cout << "                       R = Regiomontanus\n\n";

    std::cout << "    --chart-style STY  Chart display style (optional)\n";
    std::cout << "                       western      = Western wheel & rectangular\n";
    std::cout << "                       north-indian = North Indian Vedic style\n";
    std::cout << "                       south-indian = South Indian Vedic style\n";
    std::cout << "                       east-indian  = East Indian/Bengali style\n";
    std::cout << "                       solar-system = Orbital view with perspectives\n";
    std::cout << "                       (If not specified, shows basic chart data only)\n\n";

    std::cout << "    --perspective PER  Solar system perspective (for solar-system style)\n";
    std::cout << "                       heliocentric   = Sun-centered (default)\n";
    std::cout << "                       geocentric     = Earth-centered\n";
    std::cout << "                       mars-centric   = Mars-centered\n";
    std::cout << "                       jupiter-centric = Jupiter-centered\n";
    std::cout << "                       saturn-centric = Saturn-centered\n\n";

    std::cout << "    --output FORMAT    Output format (default: text)\n";
    std::cout << "                       text = Human-readable charts with ASCII art\n";
    std::cout << "                       json = Complete data structure for APIs\n\n";

    std::cout << "ECLIPSE ANALYSIS OPTIONS 🌚🌞\n";
    std::cout << "    --eclipses         Show eclipses near birth date (±1 year)\n\n";

    std::cout << "    --eclipse-range FROM TO\n";
    std::cout << "                       Find eclipses in specific date range\n";
    std::cout << "                       • Format: YYYY-MM-DD YYYY-MM-DD\n";
    std::cout << "                       • Supports BC dates: -YYYY-MM-DD\n";
    std::cout << "                       • Requires --lat and --lon for visibility\n\n";

    std::cout << "    --eclipse-years-before N\n";
    std::cout << "                       Years before birth to search (default: 1)\n\n";

    std::cout << "    --eclipse-years-after N\n";
    std::cout << "                       Years after birth to search (default: 1)\n\n";

    std::cout << "CONJUNCTION ANALYSIS OPTIONS 🪐✨\n";
    std::cout << "    --conjunctions     Show planetary conjunctions near birth\n\n";

    std::cout << "    --conjunction-range FROM TO\n";
    std::cout << "                       Find conjunctions in date range\n";
    std::cout << "                       • Requires --lat and --lon for calculations\n\n";

    std::cout << "    --conjunction-orb DEGREES\n";
    std::cout << "                       Maximum orb for conjunctions (default: 3.0)\n";
    std::cout << "                       • Smaller values = tighter conjunctions\n";
    std::cout << "                       • Range: 0.1 to 15.0 degrees\n\n";

    std::cout << "EPHEMERIS TABLE OPTIONS 📊📈\n";
    std::cout << "    --ephemeris        Generate ephemeris table\n\n";

    std::cout << "    --ephemeris-range FROM TO\n";
    std::cout << "                       Date range for ephemeris table\n\n";

    std::cout << "    --ephemeris-interval DAYS\n";
    std::cout << "                       Days between entries (default: 1)\n";
    std::cout << "                       • 1 = daily, 7 = weekly, 30 = monthly\n\n";

    std::cout << "    --ephemeris-format FORMAT\n";
    std::cout << "                       table = ASCII table (default)\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n\n";

    std::cout << "KP SYSTEM OPTIONS (Krishnamurti Paddhati) 🇮🇳🔢\n";
    std::cout << "    --kp-table         Show complete KP Sub Lord 5 Levels analysis\n";
    std::cout << "                       • Includes all planets with sub-lords\n";
    std::cout << "                       • Shows KP notation (Sign-Star-Sub-Sub-Sub)\n\n";

    std::cout << "    --kp-transitions   Show KP planetary transitions\n\n";

    std::cout << "    --kp-transition-range FROM TO\n";
    std::cout << "                       KP transitions in date range\n\n";

    std::cout << "    --kp-transition-planet PLANET\n";
    std::cout << "                       Specific planet for transitions\n";
    std::cout << "                       • Options: SUN, MOON, MERCURY, VENUS, MARS,\n";
    std::cout << "                         JUPITER, SATURN, RAHU, KETU (default: all)\n\n";

    std::cout << "    --kp-transition-level LEVEL\n";
    std::cout << "                       Transition level to track\n";
    std::cout << "                       sign    = Sign changes only\n";
    std::cout << "                       star    = Nakshatra changes\n";
    std::cout << "                       sub     = Sub-lord changes\n";
    std::cout << "                       sub-sub = Sub-sub lord changes\n";
    std::cout << "                       sub³    = Sub³ lord changes\n";
    std::cout << "                       (default: all levels)\n\n";

    std::cout << "    --kp-format FORMAT KP output format: table, csv, json (default: table)\n\n";

    std::cout << "UTILITY OPTIONS ⚙️🛠️\n";
    std::cout << "    --solar-system     Show solar system orbital paths only\n";
    std::cout << "                       • No birth data required for this option\n";
    std::cout << "                       • Displays planetary orbits around Sun\n\n";

    std::cout << "    --no-drawing       Disable all chart and ASCII art drawing\n";
    std::cout << "                       • Shows only numerical data and calculations\n";
    std::cout << "                       • Useful for data-only output or scripting\n\n";

    std::cout << "    --ephe-path PATH   Custom path to Swiss Ephemeris data files\n";
    std::cout << "                       • Default: ./data/\n";
    std::cout << "                       • Required files: seas_18.se1, semo_18.se1, etc.\n\n";

    std::cout << "    --help, -h         Show this comprehensive help message\n";
    std::cout << "    --features, -f     Show colorful feature showcase\n";
    std::cout << "    --version, -v      Show version and build information\n\n";

    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "                                 EXAMPLES 💡\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n\n";

    std::cout << "BASIC BIRTH CHARTS 📍\n";
    std::cout << "  # Standard Western chart for New York birth\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5\n\n";

    std::cout << "  # London birth with Koch houses\n";
    std::cout << "  horoscope_cli --date 1985-06-20 --time 09:15:30 \\\n";
    std::cout << "                --lat 51.5074 --lon -0.1278 --timezone 1 \\\n";
    std::cout << "                --house-system K\n\n";

    std::cout << "  # North Indian Vedic chart\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --chart-style north-indian\n\n";

    std::cout << "  # JSON output for API integration\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --output json\n\n";

    std::cout << "  # Data-only output without charts or ASCII art\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --no-drawing\n\n";

    std::cout << "HISTORICAL CHARTS (BC Era) 🏛️\n";
    std::cout << "  # Julius Caesar's assassination (44 BC)\n";
    std::cout << "  horoscope_cli --date -0044-03-15 --time 12:00:00 \\\n";
    std::cout << "                --lat 41.9028 --lon 12.4964 --timezone 1\n\n";

    std::cout << "  # Ancient Athens (500 BC)\n";
    std::cout << "  horoscope_cli --date -0500-03-15 --time 12:00:00 \\\n";
    std::cout << "                --lat 37.9755 --lon 23.7348 --timezone 2\n\n";

    std::cout << "ECLIPSE ANALYSIS 🌑\n";
    std::cout << "  # Eclipses around birth date\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --eclipses\n\n";

    std::cout << "  # All eclipses in 2024\n";
    std::cout << "  horoscope_cli --eclipse-range 2024-01-01 2024-12-31 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060\n\n";

    std::cout << "  # Historical eclipses in ancient Greece\n";
    std::cout << "  horoscope_cli --eclipse-range -0500-01-01 -0499-12-31 \\\n";
    std::cout << "                --lat 37.9755 --lon 23.7348\n\n";

    std::cout << "CONJUNCTION ANALYSIS 🔗\n";
    std::cout << "  # Tight conjunctions (2° orb) around birth\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --conjunctions --conjunction-orb 2.0\n\n";

    std::cout << "  # Conjunctions in first quarter 2024\n";
    std::cout << "  horoscope_cli --conjunction-range 2024-01-01 2024-03-31 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 \\\n";
    std::cout << "                --conjunction-orb 5.0\n\n";

    std::cout << "KP SYSTEM ANALYSIS 🧮\n";
    std::cout << "  # Complete KP Sub Lord table\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --kp-table\n\n";

    std::cout << "  # Sun's sub-lord transitions for one week\n";
    std::cout << "  horoscope_cli --kp-transitions \\\n";
    std::cout << "                --kp-transition-range 2025-01-01 2025-01-07 \\\n";
    std::cout << "                --kp-transition-planet SUN \\\n";
    std::cout << "                --kp-transition-level sub\n\n";

    std::cout << "  # All KP transitions in CSV format\n";
    std::cout << "  horoscope_cli --kp-transitions \\\n";
    std::cout << "                --kp-transition-range 2025-01-01 2025-01-31 \\\n";
    std::cout << "                --kp-format csv\n\n";

    std::cout << "EPHEMERIS TABLES 🗓️\n";
    std::cout << "  # Daily ephemeris for January 2025\n";
    std::cout << "  horoscope_cli --ephemeris \\\n";
    std::cout << "                --ephemeris-range 2025-01-01 2025-01-31\n\n";

    std::cout << "  # Weekly ephemeris in CSV format\n";
    std::cout << "  horoscope_cli --ephemeris \\\n";
    std::cout << "                --ephemeris-range 2025-01-01 2025-12-31 \\\n";
    std::cout << "                --ephemeris-interval 7 \\\n";
    std::cout << "                --ephemeris-format csv\n\n";

    std::cout << "SOLAR SYSTEM VIEWS 🌌\n";
    std::cout << "  # Standalone solar system orbital display\n";
    std::cout << "  horoscope_cli --solar-system\n\n";

    std::cout << "  # Solar system with birth chart (geocentric view)\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --chart-style solar-system --perspective geocentric\n\n";

    std::cout << "COORDINATE EXAMPLES 🌍\n";
    std::cout << "  # Major cities coordinates for reference:\n";
    std::cout << "  # New York:    --lat  40.7128 --lon  -74.0060 --timezone -5\n";
    std::cout << "  # London:      --lat  51.5074 --lon   -0.1278 --timezone  0\n";
    std::cout << "  # Paris:       --lat  48.8566 --lon    2.3522 --timezone  1\n";
    std::cout << "  # Tokyo:       --lat  35.6762 --lon  139.6503 --timezone  9\n";
    std::cout << "  # Sydney:      --lat -33.8688 --lon  151.2093 --timezone 10\n";
    std::cout << "  # Cairo:       --lat  30.0444 --lon   31.2357 --timezone  2\n";
    std::cout << "  # Mumbai:      --lat  19.0760 --lon   72.8777 --timezone  5.5\n";
    std::cout << "  # Los Angeles: --lat  34.0522 --lon -118.2437 --timezone -8\n\n";

    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "                             TECHNICAL NOTES ⚡\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n\n";

    std::cout << "PRECISION & ACCURACY 🎯\n";
    std::cout << "  • Swiss Ephemeris provides arc-second precision\n";
    std::cout << "  • Date range: approximately 6000 BC to 7000 AD\n";
    std::cout << "  • All calculations use UTC internally\n";
    std::cout << "  • Eclipse calculations use real astronomical functions\n\n";

    std::cout << "COORDINATE SYSTEMS 🧭\n";
    std::cout << "  • Tropical zodiac (season-based) used by default\n";
    std::cout << "  • Geocentric calculations (Earth-centered)\n";
    std::cout << "  • True lunar nodes calculated\n";
    std::cout << "  • Modern planetary positions including Pluto\n\n";

    std::cout << "DATA REQUIREMENTS 💾\n";
    std::cout << "  • Swiss Ephemeris data files required in ./data/ directory\n";
    std::cout << "  • Minimum files: seas_18.se1, semo_18.se1, sepl_18.se1\n";
    std::cout << "  • For historical dates, additional files may be needed\n\n";

    std::cout << "OUTPUT FORMATS 📄\n";
    std::cout << "  • Text: Human-readable with Unicode astrological symbols\n";
    std::cout << "  • JSON: Complete data structure for programming interfaces\n";
    std::cout << "  • CSV: Tabular data for spreadsheet analysis\n\n";

    std::cout << "For more information and documentation, see MANUAL.md\n";
    std::cout << "Report bugs and feature requests to the project repository.\n\n";
}

void printVersion() {
    std::cout << "🌟 Horoscope CLI v1.0.0 ⭐\n";
    std::cout << "✨ Built with Swiss Ephemeris 🔮\n";
    std::cout << "📅 Copyright (c) 2025 🏛️\n";
    std::cout << "🚀 Professional Astrology Tool 🌌\n";
}

void printFeatures() {
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "              🌟 HOROSCOPE CLI - FEATURE SHOWCASE ⭐\n";
    std::cout << "                      Your Professional Astrology Toolkit\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n\n";

    std::cout << "🔮 BIRTH CHART GENERATION\n";
    std::cout << "   ✨ Western Wheel Charts with ASCII Art\n";
    std::cout << "   🕉️ Vedic Charts (North/South/East Indian styles)\n";
    std::cout << "   🌌 Solar System Orbital Views\n";
    std::cout << "   🏛️ 6 House Systems: Placidus, Koch, Equal, Whole Sign, Campanus, Regiomontanus\n\n";

    std::cout << "🌞 ECLIPSE CALCULATIONS\n";
    std::cout << "   🌚 Solar & Lunar Eclipse Predictions\n";
    std::cout << "   🔍 Historical Eclipse Research (6000 BC - 7000 AD)\n";
    std::cout << "   🌍 Global Visibility Calculations\n";
    std::cout << "   📅 Precise Timing with Swiss Ephemeris\n\n";

    std::cout << "✨ PLANETARY CONJUNCTIONS\n";
    std::cout << "   🪐 Multi-Planet Alignment Detection\n";
    std::cout << "   🎯 Customizable Orb Settings (0.1° - 15°)\n";
    std::cout << "   📈 Historical Conjunction Analysis\n";
    std::cout << "   🔗 Aspect Relationship Mapping\n\n";

    std::cout << "🇮🇳 KP SYSTEM (Krishnamurti Paddhati)\n";
    std::cout << "   🧮 5-Level Sub-Lord Calculations\n";
    std::cout << "   🌟 27 Nakshatra System Integration\n";
    std::cout << "   📊 Planetary Transition Tracking\n";
    std::cout << "   📋 Complete KP Tables & Analysis\n\n";

    std::cout << "📊 EPHEMERIS TABLES\n";
    std::cout << "   🗓️ Daily/Weekly/Monthly Planetary Positions\n";
    std::cout << "   💾 CSV/JSON Export for Data Analysis\n";
    std::cout << "   📈 Custom Date Range Calculations\n";
    std::cout << "   🎯 Arc-Second Precision Positioning\n\n";

    std::cout << "⚙️ TECHNICAL EXCELLENCE\n";
    std::cout << "   🎯 Swiss Ephemeris Integration (NASA-Quality)\n";
    std::cout << "   🌍 Global Coordinate System Support\n";
    std::cout << "   🏛️ Ancient Date Support (BC Era)\n";
    std::cout << "   💾 Multiple Output Formats (Text/JSON/CSV)\n\n";

    std::cout << "🚀 COMMAND LINE POWER\n";
    std::cout << "   ⚡ Lightning-Fast Calculations\n";
    std::cout << "   🔄 Batch Processing Capabilities\n";
    std::cout << "   🛠️ Developer-Friendly API Output\n";
    std::cout << "   📖 Comprehensive Documentation\n\n";

    std::cout << "════════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "                Ready to explore the cosmos? Try --help for examples!\n";
    std::cout << "════════════════════════════════════════════════════════════════════════════════\n\n";
}

bool parseDate(const std::string& dateStr, int& year, int& month, int& day) {
    return Astro::parseBCDate(dateStr, year, month, day);
}

// Helper function to parse dates with BC support for other modules
bool parseBCDate(const std::string& dateStr, int& year, int& month, int& day) {
    return Astro::parseBCDate(dateStr, year, month, day);
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
        } else if (arg == "--features" || arg == "-f") {
            args.showFeatures = true;
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
        } else if (arg == "--no-drawing") {
            args.noDrawing = true;
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
        } else if (arg == "--kp-table") {
            args.showKPTable = true;
        } else if (arg == "--kp-transitions") {
            args.showKPTransitions = true;
        } else if (arg == "--kp-transition-range" && i + 2 < argc) {
            args.kpTransitionFromDate = argv[++i];
            args.kpTransitionToDate = argv[++i];
            args.showKPTransitions = true;
        } else if (arg == "--kp-transition-planet" && i + 1 < argc) {
            args.kpTransitionPlanet = argv[++i];
        } else if (arg == "--kp-transition-level" && i + 1 < argc) {
            args.kpTransitionLevel = argv[++i];
        } else if (arg == "--kp-format" && i + 1 < argc) {
            args.kpOutputFormat = argv[++i];
            if (args.kpOutputFormat != "table" && args.kpOutputFormat != "csv" && args.kpOutputFormat != "json") {
                std::cerr << "Error: KP format must be 'table', 'csv', or 'json'\n";
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
    if (args.showHelp || args.showVersion || args.showFeatures || args.showSolarSystemOnly) {
        return true;
    }

    // Eclipse and ephemeris features can work without full birth data
    if (args.showEclipses || args.showConjunctions || args.showEphemerisTable || args.showKPTransitions) {
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

    if (args.showFeatures) {
        printFeatures();
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
        if (args.showEclipses || !args.eclipseFromDate.empty()) {
            EclipseCalculator eclipseCalc;
            if (!eclipseCalc.initialize(args.ephemerisPath)) {
                std::cerr << "Failed to initialize eclipse calculator" << std::endl;
                return 1;
            }

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
        if (args.showConjunctions || !args.conjunctionFromDate.empty()) {
            ConjunctionCalculator conjCalc;
            if (!conjCalc.initialize(args.ephemerisPath)) {
                std::cerr << "Failed to initialize conjunction calculator" << std::endl;
                return 1;
            }
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

        // Handle KP system calculations
        if (args.showKPTransitions) {
            KPSystem kpSystem;
            if (!kpSystem.initialize()) {
                std::cerr << "Error: Failed to initialize KP system" << std::endl;
                return 1;
            }

            std::string fromDate = args.kpTransitionFromDate;
            std::string toDate = args.kpTransitionToDate;

            if (fromDate.empty() || toDate.empty()) {
                std::cerr << "Error: KP transitions require --kp-transition-range FROM TO dates" << std::endl;
                return 1;
            }

            // Parse planet parameter
            Planet targetPlanet = Planet::SUN; // Default
            bool allPlanets = (args.kpTransitionPlanet == "all");

            if (!allPlanets) {
                targetPlanet = stringToPlanet(args.kpTransitionPlanet);
                if (targetPlanet == Planet::SUN && args.kpTransitionPlanet != "SUN" && args.kpTransitionPlanet != "sun") {
                    std::cerr << "Error: Invalid planet '" << args.kpTransitionPlanet << "'" << std::endl;
                    return 1;
                }
            }

            // Parse level parameter
            KPLevel targetLevel = KPLevel::SUB; // Default
            bool allLevels = (args.kpTransitionLevel == "all");

            if (!allLevels) {
                targetLevel = stringToKPLevel(args.kpTransitionLevel);
            }

            // Generate transitions
            std::vector<KPTransition> allTransitions;

            if (allPlanets) {
                // Calculate for all planets
                std::vector<Planet> planets = {Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS,
                                             Planet::MARS, Planet::JUPITER, Planet::SATURN};
                for (Planet planet : planets) {
                    if (allLevels) {
                        // Calculate for all levels
                        std::vector<KPLevel> levels = {KPLevel::SIGN, KPLevel::STAR, KPLevel::SUB,
                                                     KPLevel::SUB_SUB, KPLevel::SUB_SUB_SUB};
                        for (KPLevel level : levels) {
                            auto transitions = kpSystem.findTransitions(fromDate, toDate, planet, level);
                            allTransitions.insert(allTransitions.end(), transitions.begin(), transitions.end());
                        }
                    } else {
                        auto transitions = kpSystem.findTransitions(fromDate, toDate, planet, targetLevel);
                        allTransitions.insert(allTransitions.end(), transitions.begin(), transitions.end());
                    }
                }
            } else {
                if (allLevels) {
                    // Calculate for all levels of target planet
                    std::vector<KPLevel> levels = {KPLevel::SIGN, KPLevel::STAR, KPLevel::SUB,
                                                 KPLevel::SUB_SUB, KPLevel::SUB_SUB_SUB};
                    for (KPLevel level : levels) {
                        auto transitions = kpSystem.findTransitions(fromDate, toDate, targetPlanet, level);
                        allTransitions.insert(allTransitions.end(), transitions.begin(), transitions.end());
                    }
                } else {
                    allTransitions = kpSystem.findTransitions(fromDate, toDate, targetPlanet, targetLevel);
                }
            }

            // Output results
            if (!allTransitions.empty()) {
                std::string result = kpSystem.generateTransitionTable(allTransitions);
                std::cout << result << std::endl;
            } else {
                std::cout << "No KP transitions found in the specified period." << std::endl;
            }
        }

        // Return early if only special features were requested
        if (args.showEclipses || args.showConjunctions || args.showEphemerisTable || args.showKPTransitions) {
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

    // Handle KP Table if requested
    if (args.showKPTable) {
        KPSystem kpSystem;
        if (!kpSystem.initialize()) {
            std::cerr << "Error: Failed to initialize KP system" << std::endl;
            return 1;
        }

        std::string kpTable = kpSystem.generateKPTable(chart.getPlanetPositions());
        std::cout << kpTable << std::endl;

        // If only KP table was requested, return
        if (!args.showKPTransitions && args.outputFormat == "text" && args.chartStyle.empty()) {
            return 0;
        }
    }

    // Output results
    if (args.outputFormat == "json") {
        std::cout << chart.exportToJson() << std::endl;
    } else if (args.noDrawing) {
        // No-drawing mode: show only basic text output without charts or ASCII art
        std::cout << chart.getFormattedChart() << std::endl;
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
        } else if (args.chartStyle == "western") {
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
        } else if (args.chartStyle.empty()) {
            // No chart style specified: show only basic formatted chart data
            std::cout << chart.getFormattedChart() << std::endl;
        } else {
            // Default to basic text output
            std::cout << chart.getFormattedChart() << std::endl;
        }
    }

    return 0;
}
