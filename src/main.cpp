#include "horoscope_calculator.h"
#include "birth_chart.h"
#include "eastern_chart_drawer.h"
#include "western_chart_drawer.h"
#include "solar_system_drawer.h"
#include "eclipse_calculator.h"
#include "conjunction_calculator.h"
#include "ephemeris_table.h"
#include "kp_system.h"
#include "location_manager.h"
#include "hindu_calendar.h"
#include "myanmar_calendar.h"
#include "astro_calendar.h"
#include "swephexp.h"
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

    // Location search options
    std::string locationName = "";
    std::string searchLocation = "";
    bool listLocations = false;

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

    // Hindu Calendar (Panchanga) options
    bool showPanchanga = false;
    bool showPanchangaRange = false;
    std::string panchangaFromDate;
    std::string panchangaToDate;
    std::string panchangaFormat = "table";
    bool showFestivalsOnly = false;

    // Myanmar Calendar options
    bool showMyanmarCalendar = false;
    bool showMyanmarCalendarRange = false;
    std::string myanmarCalendarFromDate;
    std::string myanmarCalendarToDate;
    std::string myanmarCalendarFormat = "table";
    bool showAstrologicalDaysOnly = false;

    // Astro Calendar options (Combined calendar system)
    bool showAstroCalendar = false;
    std::string astroCalendarDate;
    std::string astroCalendarMonth; // Format: YYYY-MM for monthly view
    bool showAstroCalendarMonthly = false;
    std::string astroCalendarFormat = "calendar";
    bool showPlanetaryTransitions = false;
    bool showAllFestivals = false;
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
    std::cout << "                       • Standard: 1990-01-15 (for 1990 AD)\n";
    std::cout << "                       • BC dates: -0044-03-15 or 44BC-03-15 (for 44 BC)\n";
    std::cout << "                       • AD dates: 1990AD-01-15 (optional AD suffix)\n";
    std::cout << "                       • Supports dates from 6000 BC to 7000 AD\n";
    std::cout << "                       • Examples: 1990-01-15, -0500-12-25, 44BC-03-15\n\n";

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

    std::cout << "HINDU CALENDAR OPTIONS (Panchanga) 🕉️📅\n";
    std::cout << "    --panchanga        Show Hindu calendar (Panchanga) for birth date\n";
    std::cout << "                       • Displays Tithi, Vara, Nakshatra, Yoga, Karana\n";
    std::cout << "                       • Shows Hindu month, year (Vikram Samvat)\n";
    std::cout << "                       • Includes festivals and special events\n\n";

    std::cout << "    --panchanga-range FROM TO\n";
    std::cout << "                       Generate Panchanga for date range\n";
    std::cout << "                       • Format: YYYY-MM-DD YYYY-MM-DD\n";
    std::cout << "                       • Shows daily Panchanga elements\n";
    std::cout << "                       • Requires --lat and --lon for calculations\n\n";

    std::cout << "    --panchanga-format FORMAT\n";
    std::cout << "                       Panchanga output format\n";
    std::cout << "                       table = Detailed ASCII table (default)\n";
    std::cout << "                       compact = Traditional tabular format (like Pancanga3.14.pl)\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n\n";

    std::cout << "    --festivals-only   Show only festivals and special events\n";
    std::cout << "                       • Filters output to show religious observances\n";
    std::cout << "                       • Includes Ekadashi, Purnima, Amavasya\n";
    std::cout << "                       • Shows major Hindu festivals\n\n";

    std::cout << "MYANMAR CALENDAR OPTIONS 🇲🇲📅\n";
    std::cout << "    --myanmar-calendar Show Myanmar calendar for birth date\n";
    std::cout << "                       • Displays Myanmar Era (ME) and Sasana Era (SE) years\n";
    std::cout << "                       • Shows Myanmar month, day, year type (watat)\n";
    std::cout << "                       • Includes moon phases and fortnight days\n";
    std::cout << "                       • Displays astrological days and observances\n";

    std::cout << "    --myanmar-calendar-range FROM TO\n";
    std::cout << "                       Generate Myanmar calendar for date range\n";
    std::cout << "                       • Format: YYYY-MM-DD YYYY-MM-DD\n";
    std::cout << "                       • Shows daily Myanmar calendar elements\n";

    std::cout << "    --myanmar-calendar-format FORMAT\n";
    std::cout << "                       Myanmar calendar output format\n";
    std::cout << "                       table = Detailed display (default)\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n";

    std::cout << "    --astrological-days-only\n";
    std::cout << "                       Show only astrological days and events\n";
    std::cout << "                       • Filters output to show Sabbath, Yatyaza, etc.\n";
    std::cout << "                       • Shows Myanmar astrological observances\n\n";

    std::cout << "ASTRO CALENDAR OPTIONS (Combined Calendar System) 📅🌟🇮🇳🇲🇲\n";
    std::cout << "    --astro-calendar   Show comprehensive astro-calendar for birth date\n";
    std::cout << "                       • Combines Gregorian, Hindu (Panchanga), Myanmar calendars\n";
    std::cout << "                       • Shows festivals, events, and planetary positions\n";
    std::cout << "                       • Displays astrological days and recommendations\n";
    std::cout << "                       • Beautiful unified calendar layout\n";

    std::cout << "    --astro-calendar-monthly YYYY-MM\n";
    std::cout << "                       Generate monthly astro-calendar view\n";
    std::cout << "                       • Format: 2024-01 for January 2024\n";
    std::cout << "                       • Shows all three calendar systems for entire month\n";
    std::cout << "                       • Includes daily festivals, events, planetary transits\n";
    std::cout << "                       • Comprehensive monthly astrological overview\n";

    std::cout << "    --astro-calendar-format FORMAT\n";
    std::cout << "                       Astro-calendar output format\n";
    std::cout << "                       calendar     = Beautiful calendar layout (default)\n";
    std::cout << "                       table        = Detailed tabular format\n";
    std::cout << "                       professional = Saint John Astronomy Club style with KP\n";
    std::cout << "                       json         = JSON structure for integration\n";
    std::cout << "                       csv          = Comma-separated values\n";

    std::cout << "    --planetary-transitions\n";
    std::cout << "                       Include planetary transitions in astro-calendar\n";
    std::cout << "                       • Shows planet sign changes, retrogrades\n";
    std::cout << "                       • Displays conjunction aspects\n";
    std::cout << "                       • Indicates astrologically significant movements\n";

    std::cout << "    --all-festivals    Show all festivals from all calendar systems\n";
    std::cout << "                       • Hindu festivals, Myanmar observances\n";
    std::cout << "                       • Religious holidays, special events\n";
    std::cout << "                       • Cultural celebrations and fasting days\n\n";

    std::cout << "UTILITY OPTIONS ⚙️🛠️\n";
    std::cout << "    --solar-system     Show solar system orbital paths only\n";
    std::cout << "                       • No birth data required for this option\n";
    std::cout << "                       • Displays planetary orbits around Sun\n\n";

    std::cout << "    --no-drawing       Disable all chart and ASCII art drawing\n";
    std::cout << "                       • Shows only numerical data and calculations\n";
    std::cout << "                       • Useful for data-only output or scripting\n\n";

    std::cout << "    --location NAME    Search for location by name instead of coordinates\n";
    std::cout << "                       • Use city name, e.g., \"New York\", \"London\", \"Tokyo\"\n";
    std::cout << "                       • Automatically sets lat, lon, and timezone\n";
    std::cout << "                       • Supports major world cities\n\n";

    std::cout << "    --search-location NAME  Search and list matching locations\n";
    std::cout << "                       • Shows all locations matching the search term\n";
    std::cout << "                       • Displays coordinates and timezone info\n";
    std::cout << "                       • No calculations performed\n\n";

    std::cout << "    --list-locations   Show all available predefined locations\n";
    std::cout << "                       • Lists major cities with coordinates\n";
    std::cout << "                       • Useful for finding exact spelling\n";
    std::cout << "                       • No calculations performed\n\n";

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

    std::cout << "LOCATION-BASED CHARTS 🌍\n";
    std::cout << "  # Use city name instead of coordinates\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --location \"New York\"\n\n";

    std::cout << "  # Birth in London using location name\n";
    std::cout << "  horoscope_cli --date 1985-06-20 --time 09:15:30 \\\n";
    std::cout << "                --location \"London\" --chart-style north-indian\n\n";

    std::cout << "  # Search for available locations\n";
    std::cout << "  horoscope_cli --search-location \"Paris\"\n\n";

    std::cout << "  # List all available cities\n";
    std::cout << "  horoscope_cli --list-locations\n\n";

    std::cout << "HISTORICAL CHARTS (BC Era) 🏛️\n";
    std::cout << "  # Julius Caesar's assassination (44 BC)\n";
    std::cout << "  horoscope_cli --date 44BC-03-15 --time 12:00:00 \\\n";
    std::cout << "                --lat 41.9028 --lon 12.4964 --timezone 1\n\n";

    std::cout << "  # Ancient Athens (500 BC) - using minus format\n";
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

    std::cout << "  # Historical eclipses in ancient Greece (500 BC)\n";
    std::cout << "  horoscope_cli --eclipse-range 500BC-01-01 500BC-12-31 \\\n";
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
        } else if (arg == "--location" && i + 1 < argc) {
            args.locationName = argv[++i];
        } else if (arg == "--search-location" && i + 1 < argc) {
            args.searchLocation = argv[++i];
        } else if (arg == "--list-locations") {
            args.listLocations = true;
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
        } else if (arg == "--panchanga") {
            args.showPanchanga = true;
        } else if (arg == "--panchanga-range" && i + 2 < argc) {
            args.panchangaFromDate = argv[++i];
            args.panchangaToDate = argv[++i];
            args.showPanchangaRange = true;
        } else if (arg == "--panchanga-format" && i + 1 < argc) {
            args.panchangaFormat = argv[++i];
            if (args.panchangaFormat != "table" && args.panchangaFormat != "compact" &&
                args.panchangaFormat != "csv" && args.panchangaFormat != "json") {
                std::cerr << "Error: Panchanga format must be 'table', 'compact', 'csv', or 'json'\n";
                return false;
            }
        } else if (arg == "--festivals-only") {
            args.showFestivalsOnly = true;

        // Myanmar Calendar options
        } else if (arg == "--myanmar-calendar") {
            args.showMyanmarCalendar = true;
        } else if (arg == "--myanmar-calendar-range") {
            args.showMyanmarCalendarRange = true;
            if (i + 2 < argc) {
                args.myanmarCalendarFromDate = argv[++i];
                args.myanmarCalendarToDate = argv[++i];
            } else {
                std::cerr << "Error: --myanmar-calendar-range requires two date arguments (FROM TO)\n";
                return false;
            }
        } else if (arg == "--myanmar-calendar-format") {
            if (i + 1 < argc) {
                args.myanmarCalendarFormat = argv[++i];
            } else {
                std::cerr << "Error: --myanmar-calendar-format requires a format argument\n";
                return false;
            }
            if (args.myanmarCalendarFormat != "table" && args.myanmarCalendarFormat != "csv" && args.myanmarCalendarFormat != "json") {
                std::cerr << "Error: Myanmar calendar format must be 'table', 'csv', or 'json'\n";
                return false;
            }
        } else if (arg == "--astrological-days-only") {
            args.showAstrologicalDaysOnly = true;
        } else if (arg == "--astro-calendar") {
            args.showAstroCalendar = true;
        } else if (arg == "--astro-calendar-monthly") {
            if (i + 1 < argc) {
                args.astroCalendarMonth = argv[++i];
                args.showAstroCalendarMonthly = true;
                // Validate YYYY-MM format
                if (args.astroCalendarMonth.length() != 7 || args.astroCalendarMonth[4] != '-') {
                    std::cerr << "Error: --astro-calendar-monthly requires YYYY-MM format (e.g., 2024-01)\n";
                    return false;
                }
            } else {
                std::cerr << "Error: --astro-calendar-monthly requires a month argument (YYYY-MM)\n";
                return false;
            }
        } else if (arg == "--astro-calendar-format") {
            if (i + 1 < argc) {
                args.astroCalendarFormat = argv[++i];
            } else {
                std::cerr << "Error: --astro-calendar-format requires a format argument\n";
                return false;
            }
            if (args.astroCalendarFormat != "calendar" && args.astroCalendarFormat != "table" &&
                args.astroCalendarFormat != "csv" && args.astroCalendarFormat != "json" &&
                args.astroCalendarFormat != "professional") {
                std::cerr << "Error: Astro-calendar format must be 'calendar', 'table', 'csv', 'json', or 'professional'\n";
                return false;
            }
        } else if (arg == "--planetary-transitions") {
            args.showPlanetaryTransitions = true;
        } else if (arg == "--all-festivals") {
            args.showAllFestivals = true;
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            return false;
        }
    }

    return true;
}

bool validateArgs(const CommandLineArgs& args) {
    if (args.showHelp || args.showVersion || args.showFeatures || args.showSolarSystemOnly ||
        args.listLocations || !args.searchLocation.empty()) {
        return true;
    }

    // Eclipse, ephemeris, panchanga, and Myanmar calendar features can work without full birth data
    if (args.showEclipses || args.showConjunctions || args.showEphemerisTable || args.showKPTransitions ||
        args.showPanchangaRange || args.showMyanmarCalendarRange) {
        // For eclipse and conjunction range queries, we need coordinates (can come from location)
        if ((!args.eclipseFromDate.empty() || !args.conjunctionFromDate.empty() || !args.panchangaFromDate.empty() ||
             !args.myanmarCalendarFromDate.empty()) &&
            args.locationName.empty() &&
            (args.latitude < -90.0 || args.latitude > 90.0 ||
             args.longitude < -180.0 || args.longitude > 180.0)) {
            std::cerr << "Error: Valid coordinates (--lat/--lon) or location (--location) required for eclipse/conjunction/panchanga/Myanmar calendar searches\n";
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

    if (args.date.empty() && !args.showAstroCalendarMonthly) {
        std::cerr << "Error: --date is required\n";
        return false;
    }

    if (args.time.empty() && !args.showAstroCalendarMonthly) {
        std::cerr << "Error: --time is required\n";
        return false;
    }

    // Coordinates can come from location or direct input
    if (args.locationName.empty() &&
        (args.latitude < -90.0 || args.latitude > 90.0)) {
        std::cerr << "Error: Valid latitude (-90 to 90) or location (--location) required\n";
        return false;
    }

    if (args.locationName.empty() &&
        (args.longitude < -180.0 || args.longitude > 180.0)) {
        std::cerr << "Error: Valid longitude (-180 to 180) or location (--location) required\n";
        return false;
    }

    if (args.locationName.empty() &&
        (args.timezone < -12.0 || args.timezone > 14.0)) {
        std::cerr << "Error: Valid timezone (-12 to +14) or location (--location) required\n";
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

    // Handle location search operations
    LocationManager locationManager;

    if (args.listLocations) {
        std::cout << "Available Predefined Locations:\n";
        std::cout << std::string(80, '=') << std::endl;
        auto locations = locationManager.getAllLocations();
        for (const auto& location : locations) {
            std::cout << "📍 " << location.name << " (" << location.country << ")\n";
            std::cout << "   Coordinates: " << location.latitude << "°, " << location.longitude << "°\n";
            std::cout << "   Timezone: UTC" << (location.timezone >= 0 ? "+" : "") << location.timezone << "\n\n";
        }
        return 0;
    }

    if (!args.searchLocation.empty()) {
        std::cout << "Searching for locations matching: \"" << args.searchLocation << "\"\n";
        std::cout << std::string(80, '=') << std::endl;
        auto matches = locationManager.searchLocations(args.searchLocation);
        if (matches.empty()) {
            std::cout << "No locations found matching \"" << args.searchLocation << "\"\n";
            std::cout << "Try using --list-locations to see all available locations.\n";
        } else {
            for (const auto& location : matches) {
                std::cout << "📍 " << location.name << " (" << location.country << ")\n";
                std::cout << "   Coordinates: " << location.latitude << "°, " << location.longitude << "°\n";
                std::cout << "   Timezone: UTC" << (location.timezone >= 0 ? "+" : "") << location.timezone << "\n\n";
            }
        }
        return 0;
    }

    // Resolve location if specified
    if (!args.locationName.empty()) {
        auto location = locationManager.getLocationByName(args.locationName);
        if (!location.name.empty()) {
            args.latitude = location.latitude;
            args.longitude = location.longitude;
            args.timezone = location.timezone;
            std::cout << "🌍 Using location: " << location.name << " (" << location.country << ")\n";
            std::cout << "   Coordinates: " << location.latitude << "°, " << location.longitude << "°\n";
            std::cout << "   Timezone: UTC" << (location.timezone >= 0 ? "+" : "") << location.timezone << "\n\n";
        } else {
            std::cerr << "Error: Location \"" << args.locationName << "\" not found\n";
            std::cerr << "Use --search-location \"" << args.locationName << "\" to find similar locations\n";
            std::cerr << "Or use --list-locations to see all available locations\n";
            return 1;
        }
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

        // Handle Hindu Calendar (Panchanga) calculations
        if (args.showPanchangaRange) {
            HinduCalendar hinduCalendar;
            if (!hinduCalendar.initialize()) {
                std::cerr << "Error: Failed to initialize Hindu Calendar system: " << hinduCalendar.getLastError() << std::endl;
                return 1;
            }

            std::string fromDate = args.panchangaFromDate;
            std::string toDate = args.panchangaToDate;

            if (fromDate.empty() || toDate.empty()) {
                std::cerr << "Error: Panchanga range requires --panchanga-range FROM TO dates" << std::endl;
                return 1;
            }

            // Generate panchanga for date range
            std::vector<PanchangaData> panchangaList = hinduCalendar.calculatePanchangaRange(
                fromDate, toDate, args.latitude, args.longitude);

            if (!panchangaList.empty()) {
                if (args.panchangaFormat == "csv") {
                    std::string result = hinduCalendar.generateCSV(panchangaList);
                    std::cout << result << std::endl;
                } else if (args.panchangaFormat == "json") {
                    std::cout << "[\n";
                    for (size_t i = 0; i < panchangaList.size(); ++i) {
                        std::cout << hinduCalendar.generateJSON(panchangaList[i]);
                        if (i < panchangaList.size() - 1) std::cout << ",";
                        std::cout << "\n";
                    }
                    std::cout << "]\n";
                } else if (args.panchangaFormat == "compact") {
                    // Generate Julian days for compact format - simple calculation
                    std::vector<double> julianDays;

                    // Parse start date
                    int startYear = std::stoi(fromDate.substr(0, 4));
                    int startMonth = std::stoi(fromDate.substr(5, 2));
                    int startDay = std::stoi(fromDate.substr(8, 2));

                    // Use proper Swiss Ephemeris Julian Day calculation
                    double startJD = swe_julday(startYear, startMonth, startDay, 12.0, SE_GREG_CAL);

                    for (size_t i = 0; i < panchangaList.size(); ++i) {
                        julianDays.push_back(startJD + i);
                    }

                    std::string result = hinduCalendar.generatePanchangaTableFormat(panchangaList, julianDays);
                    std::cout << result << std::endl;
                } else {
                    std::string result = hinduCalendar.generatePanchangaTable(panchangaList);
                    std::cout << result << std::endl;
                }
            } else {
                std::cout << "Failed to generate Panchanga for the specified period." << std::endl;
            }
        }

        // Handle Myanmar Calendar calculations
        if (args.showMyanmarCalendarRange) {
            MyanmarCalendar myanmarCalendar;
            if (!myanmarCalendar.initialize()) {
                std::cerr << "Error: Failed to initialize Myanmar Calendar system: " << myanmarCalendar.getLastError() << std::endl;
                return 1;
            }

            std::string fromDate = args.myanmarCalendarFromDate;
            std::string toDate = args.myanmarCalendarToDate;

            if (fromDate.empty() || toDate.empty()) {
                std::cerr << "Error: Myanmar calendar range requires --myanmar-calendar-range FROM TO dates" << std::endl;
                return 1;
            }

            // Generate Myanmar calendar for date range
            std::vector<MyanmarCalendarData> myanmarList = myanmarCalendar.calculateMyanmarDateRange(fromDate, toDate);

            if (!myanmarList.empty()) {
                if (args.myanmarCalendarFormat == "csv") {
                    std::string result = myanmarCalendar.generateCSV(myanmarList);
                    std::cout << result << std::endl;
                } else if (args.myanmarCalendarFormat == "json") {
                    std::cout << "[\n";
                    for (size_t i = 0; i < myanmarList.size(); ++i) {
                        std::cout << myanmarCalendar.generateJSON(myanmarList[i]);
                        if (i < myanmarList.size() - 1) std::cout << ",";
                        std::cout << "\n";
                    }
                    std::cout << "]\n";
                } else {
                    std::string result = myanmarCalendar.generateMyanmarCalendarTable(myanmarList);
                    std::cout << result << std::endl;
                }
            } else {
                std::cout << "Failed to generate Myanmar Calendar for the specified period." << std::endl;
            }
        }

        // Return early if only special features were requested
        if (args.showEclipses || args.showConjunctions || args.showEphemerisTable ||
            args.showKPTransitions || args.showPanchangaRange || args.showMyanmarCalendarRange) {
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

    // Handle AstroCalendar monthly calculations early (doesn't need birth data)
    if (args.showAstroCalendarMonthly) {
        AstroCalendar astroCalendar;

        if (!astroCalendar.initialize(args.latitude, args.longitude)) {
            std::cerr << "Error: Failed to initialize AstroCalendar system: " << astroCalendar.getLastError() << std::endl;
            return 1;
        }

        astroCalendar.setIncludePlanetaryTransitions(args.showPlanetaryTransitions);

        try {
            // Parse year and month from astroCalendarMonth
            int year, month;
            if (sscanf(args.astroCalendarMonth.c_str(), "%d-%d", &year, &month) == 2) {
                AstroCalendarMonth monthData = astroCalendar.calculateAstroCalendarMonth(year, month);
                std::cout << astroCalendar.generateMonthlyCalendar(monthData, args.astroCalendarFormat) << std::endl;
            } else {
                std::cerr << "Error: Invalid date format for monthly calendar. Use YYYY-MM format.\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error generating monthly astro-calendar: " << e.what() << std::endl;
            return 1;
        }

        return 0; // Exit after monthly astro-calendar
    }

    // Parse date and time (required for all other functions)
    BirthData birthData;
    if (!parseDate(args.date, birthData.year, birthData.month, birthData.day)) {
        std::cerr << "Error: Invalid date format. Supported formats:\n";
        std::cerr << "  • YYYY-MM-DD (e.g., 1990-01-15)\n";
        std::cerr << "  • -YYYY-MM-DD (e.g., -0044-03-15 for BC)\n";
        std::cerr << "  • YYYYBC-MM-DD (e.g., 44BC-03-15)\n";
        std::cerr << "  • YYYYAD-MM-DD (e.g., 1990AD-01-15)\n";
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
        if (!args.showKPTransitions && !args.showPanchanga && args.outputFormat == "text" && args.chartStyle.empty()) {
            return 0;
        }
    }

    // Handle Panchanga (Hindu Calendar) for birth date
    if (args.showPanchanga) {
        HinduCalendar hinduCalendar;
        if (!hinduCalendar.initialize()) {
            std::cerr << "Error: Failed to initialize Hindu Calendar system: " << hinduCalendar.getLastError() << std::endl;
            return 1;
        }

        PanchangaData panchanga = hinduCalendar.calculatePanchanga(birthData);

        if (args.panchangaFormat == "json") {
            std::cout << hinduCalendar.generateJSON(panchanga) << std::endl;
        } else {
            std::cout << hinduCalendar.generatePanchangaTable(panchanga) << std::endl;
        }

        // If only Panchanga was requested, return
        if (!args.showKPTable && args.outputFormat == "text" && args.chartStyle.empty()) {
            return 0;
        }
    }

    // Handle Myanmar Calendar for birth date
    if (args.showMyanmarCalendar) {
        MyanmarCalendar myanmarCalendar;
        if (!myanmarCalendar.initialize()) {
            std::cerr << "Error: Failed to initialize Myanmar Calendar system: " << myanmarCalendar.getLastError() << std::endl;
            return 1;
        }

        MyanmarCalendarData myanmarData = myanmarCalendar.calculateMyanmarDate(birthData);

        if (args.myanmarCalendarFormat == "json") {
            std::cout << myanmarCalendar.generateJSON(myanmarData) << std::endl;
        } else {
            std::cout << myanmarCalendar.generateMyanmarCalendarTable(myanmarData) << std::endl;
        }

        // If only Myanmar Calendar was requested, return
        if (!args.showKPTable && args.outputFormat == "text" && args.chartStyle.empty()) {
            return 0;
        }
    }

    // Handle Myanmar Calendar Range calculations
    if (args.showMyanmarCalendarRange) {
        MyanmarCalendar myanmarCalendar;
        if (!myanmarCalendar.initialize()) {
            std::cerr << "Error: Failed to initialize Myanmar Calendar system: " << myanmarCalendar.getLastError() << std::endl;
            return 1;
        }

        std::string fromDate = args.myanmarCalendarFromDate;
        std::string toDate = args.myanmarCalendarToDate;

        if (fromDate.empty() || toDate.empty()) {
            std::cerr << "Error: Myanmar calendar range requires both FROM and TO dates" << std::endl;
            return 1;
        }

        try {
            std::vector<MyanmarCalendarData> myanmarDataList = myanmarCalendar.calculateMyanmarDateRange(fromDate, toDate);

            if (myanmarDataList.empty()) {
                std::cout << "No Myanmar calendar data found for the specified period." << std::endl;
            } else {
                if (args.myanmarCalendarFormat == "json") {
                    std::cout << "[" << std::endl;
                    for (size_t i = 0; i < myanmarDataList.size(); ++i) {
                        if (i > 0) std::cout << "," << std::endl;
                        std::cout << myanmarCalendar.generateJSON(myanmarDataList[i]);
                    }
                    std::cout << std::endl << "]" << std::endl;
                } else if (args.myanmarCalendarFormat == "csv") {
                    std::cout << myanmarCalendar.generateCSV(myanmarDataList) << std::endl;
                } else {
                    std::cout << myanmarCalendar.generateMyanmarCalendarTable(myanmarDataList) << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error calculating Myanmar calendar range: " << e.what() << std::endl;
            return 1;
        }

        return 0; // Exit after Myanmar calendar range
    }

    // Handle AstroCalendar single-day calculations
    if (args.showAstroCalendar) {
        AstroCalendar astroCalendar;

        if (!astroCalendar.initialize(birthData.latitude, birthData.longitude)) {
            std::cerr << "Error: Failed to initialize AstroCalendar system: " << astroCalendar.getLastError() << std::endl;
            return 1;
        }

        // Set configuration options
        astroCalendar.setIncludePlanetaryTransitions(args.showPlanetaryTransitions);

        try {
            // Single day AstroCalendar
            AstroCalendarDay dayData = astroCalendar.calculateAstroCalendarDay(birthData);
            std::cout << astroCalendar.generateDayCalendar(dayData, args.astroCalendarFormat) << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error generating astro-calendar: " << e.what() << std::endl;
            return 1;
        }

        return 0; // Exit after single-day astro-calendar
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
