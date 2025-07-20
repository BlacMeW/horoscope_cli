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
#include "professional_table.h"
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
#include <sstream>
#include <iomanip>

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
    ZodiacMode zodiacMode = ZodiacMode::TROPICAL;
    AyanamsaType ayanamsa = AyanamsaType::LAHIRI;
    std::vector<CalculationFlag> calculationFlags;
    std::string outputFormat = "text";
    std::string chartStyle = "";
    std::string ephemerisPath;
    std::string solarSystemPerspective = "heliocentric";
    bool showHelp = false;
    bool showVersion = false;
    bool showFeatures = false;
    bool showSolarSystemOnly = false;
    bool noDrawing = false;
    bool showAstronomicalCoordinates = false;

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
    double conjunctionMinLatitude = -90.0;
    double conjunctionMaxLatitude = 90.0;
    bool showGrahaYuddha = false;
    double grahaYuddhaMaxOrb = 1.0;
    std::string grahaYuddhaFromDate;
    std::string grahaYuddhaToDate;
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

    // Hindu Calendar Search options
    bool showHinduSearch = false;
    int searchYear = -1;
    int searchYearStart = -1;
    int searchYearEnd = -1;
    int searchMonth = -1;
    int searchMonthStart = -1;
    int searchMonthEnd = -1;
    int searchTithi = -1;
    int searchTithiStart = -1;
    int searchTithiEnd = -1;
    int searchWeekday = -1;
    bool searchExactMatch = true;
    int searchNearTolerance = 1;
    std::string searchStartDate;
    std::string searchEndDate;
    std::string hinduSearchFormat = "table";

    // Myanmar Calendar options
    bool showMyanmarCalendar = false;
    bool showMyanmarCalendarRange = false;
    std::string myanmarCalendarFromDate;
    std::string myanmarCalendarToDate;
    std::string myanmarCalendarFormat = "table";
    bool showAstrologicalDaysOnly = false;

    // Myanmar Calendar Search options
    bool showMyanmarSearch = false;
    int myanmarSearchYear = -1;
    int myanmarSearchYearStart = -1;
    int myanmarSearchYearEnd = -1;
    int myanmarSearchMonth = -1;
    int myanmarSearchMonthStart = -1;
    int myanmarSearchMonthEnd = -1;
    int myanmarSearchMoonPhase = -1;
    int myanmarSearchMoonPhaseStart = -1;
    int myanmarSearchMoonPhaseEnd = -1;
    int myanmarSearchWeekday = -1;
    int myanmarSearchFortnightDay = -1;
    int myanmarSearchFortnightDayStart = -1;
    int myanmarSearchFortnightDayEnd = -1;
    bool myanmarSearchSabbath = false;
    bool myanmarSearchSabbathEve = false;
    bool myanmarSearchYatyaza = false;
    bool myanmarSearchPyathada = false;
    bool myanmarSearchThamanyo = false;
    bool myanmarSearchExactMatch = true;
    int myanmarSearchNearTolerance = 1;
    std::string myanmarSearchStartDate;
    std::string myanmarSearchEndDate;
    std::string myanmarSearchFormat = "table";

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

    std::cout << "    --zodiac-mode MODE Zodiac calculation mode (default: tropical)\n";
    std::cout << "                       tropical  = Tropical zodiac (season-based, Western)\n";
    std::cout << "                       sidereal  = Sidereal zodiac (star-based, Vedic)\n\n";

    std::cout << "    --ayanamsa TYPE    Ayanamsa for sidereal calculations (default: lahiri)\n";
    std::cout << "                       lahiri           = Lahiri/Chitrapaksha (most common)\n";
    std::cout << "                       fagan-bradley    = Fagan-Bradley\n";
    std::cout << "                       raman            = B.V. Raman\n";
    std::cout << "                       krishnamurti     = K.S. Krishnamurti (KP System)\n";
    std::cout << "                       yukteshwar       = Sri Yukteshwar\n";
    std::cout << "                       jn_bhasin        = J.N. Bhasin\n";
    std::cout << "                       sassanian        = Sassanian\n";
    std::cout << "                       galactic_center  = Galactic Center at 0° Sagittarius\n";
    std::cout << "                       (See full list with --help-ayanamsa)\n\n";

    std::cout << "    --calculation-flags FLAGS  Calculation flags (comma-separated)\n";
    std::cout << "                               Coordinate system:\n";
    std::cout << "                               geocentric, heliocentric, barycentric, topocentric\n";
    std::cout << "                               Position type:\n";
    std::cout << "                               apparent, true_geometric, astrometric\n";
    std::cout << "                               Precision:\n";
    std::cout << "                               high_precision_speed, j2000_equinox\n";
    std::cout << "                               Example: --calculation-flags geocentric,apparent\n\n";

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

    std::cout << "    --conjunction-latitude-range MIN MAX\n";
    std::cout << "                       Filter conjunctions by planetary latitude range\n";
    std::cout << "                       • MIN/MAX in degrees (-90.0 to +90.0)\n";
    std::cout << "                       • Example: --conjunction-latitude-range -5.0 5.0\n\n";

    std::cout << "    --graha-yuddha     Find Graha Yuddha (Planetary Wars)\n";
    std::cout << "                       • Very close conjunctions (< 1°) between visible planets\n";
    std::cout << "                       • Determines winner based on Vedic rules\n\n";

    std::cout << "    --graha-yuddha-range FROM TO\n";
    std::cout << "                       Find planetary wars in specific date range\n";
    std::cout << "                       • Shows winner and astrological effects\n\n";

    std::cout << "    --graha-yuddha-orb DEGREES\n";
    std::cout << "                       Maximum orb for planetary wars (default: 1.0)\n";
    std::cout << "                       • Range: 0.1 to 2.0 degrees\n\n";

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

    std::cout << "HINDU CALENDAR SEARCH OPTIONS 🔍🕉️\n";
    std::cout << "    --hindu-search FROM TO  Search Hindu calendar by criteria\n";
    std::cout << "                            • Format: YYYY-MM-DD YYYY-MM-DD (date range)\n";
    std::cout << "                            • Combine with search criteria below\n";
    std::cout << "                            • Results sorted by match score\n\n";

    std::cout << "    --search-year YEAR      Search for specific year\n";
    std::cout << "                            • Example: --search-year 2025\n\n";

    std::cout << "    --search-year-range START END  Search for year range\n";
    std::cout << "                                    • Example: --search-year-range 2025 2027\n\n";

    std::cout << "    --search-month MONTH    Search for specific month (1-12)\n";
    std::cout << "                            • Example: --search-month 5 (May)\n\n";

    std::cout << "    --search-month-range START END  Search for month range (1-12)\n";
    std::cout << "                                     • Example: --search-month-range 4 6\n\n";

    std::cout << "    --search-tithi TITHI    Search for specific tithi (1-30)\n";
    std::cout << "                            • 1-15: Shukla Paksha, 16-30: Krishna Paksha\n";
    std::cout << "                            • Example: --search-tithi 15 (Purnima)\n\n";

    std::cout << "    --search-tithi-range START END  Search for tithi range (1-30)\n";
    std::cout << "                                     • Example: --search-tithi-range 14 16\n\n";

    std::cout << "    --search-weekday DAY    Search for specific weekday (0-6)\n";
    std::cout << "                            • 0=Sunday, 1=Monday, ..., 6=Saturday\n";
    std::cout << "                            • Example: --search-weekday 0 (Sundays)\n\n";

    std::cout << "    --search-exact          Use exact matching (default)\n";
    std::cout << "    --search-near TOL       Use near matching with tolerance\n";
    std::cout << "                            • Example: --search-near 2\n\n";

    std::cout << "    --hindu-search-format FORMAT\n";
    std::cout << "                            Hindu search results output format\n";
    std::cout << "                            table = Detailed ASCII table (default)\n";
    std::cout << "                            csv   = Comma-separated values\n";
    std::cout << "                            json  = JSON structure for integration\n";
    std::cout << "                            list  = Simple date list for scripts\n\n";

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

    std::cout << "MYANMAR CALENDAR SEARCH OPTIONS 🔍🇲🇲\n";
    std::cout << "    --myanmar-search FROM TO  Search Myanmar calendar by criteria\n";
    std::cout << "                              • Format: YYYY-MM-DD YYYY-MM-DD (date range)\n";
    std::cout << "                              • Combine with search criteria below\n";
    std::cout << "                              • Results sorted by match score\n\n";

    std::cout << "    --myanmar-search-year YEAR      Search for specific Myanmar year\n";
    std::cout << "                                     • Example: --myanmar-search-year 1385\n\n";

    std::cout << "    --myanmar-search-year-range START END  Search for year range\n";
    std::cout << "                                            • Example: --myanmar-search-year-range 1385 1387\n\n";

    std::cout << "    --myanmar-search-month MONTH    Search for specific month (0-14)\n";
    std::cout << "                                     • 0=First Waso, 1=Tagu, 2=Kason, etc.\n";
    std::cout << "                                     • Example: --myanmar-search-month 4 (Waso)\n\n";

    std::cout << "    --myanmar-search-month-range START END  Search for month range (0-14)\n";
    std::cout << "                                             • Example: --myanmar-search-month-range 3 5\n\n";

    std::cout << "    --myanmar-search-moon-phase PHASE    Search for specific moon phase (0-3)\n";
    std::cout << "                                          • 0=Waxing, 1=Full, 2=Waning, 3=New\n";
    std::cout << "                                          • Example: --myanmar-search-moon-phase 1 (Full Moon)\n\n";

    std::cout << "    --myanmar-search-moon-phase-range START END  Search for moon phase range (0-3)\n";
    std::cout << "                                                  • Example: --myanmar-search-moon-phase-range 0 1\n\n";

    std::cout << "    --myanmar-search-weekday DAY    Search for specific weekday (0-6)\n";
    std::cout << "                                     • 0=Saturday, 1=Sunday, ..., 6=Friday\n";
    std::cout << "                                     • Example: --myanmar-search-weekday 1 (Sundays)\n\n";

    std::cout << "    --myanmar-search-fortnight-day DAY    Search for specific fortnight day (1-15)\n";
    std::cout << "                                           • Example: --myanmar-search-fortnight-day 15\n\n";

    std::cout << "    --myanmar-search-fortnight-day-range START END  Search for fortnight day range (1-15)\n";
    std::cout << "                                                     • Example: --myanmar-search-fortnight-day-range 14 15\n\n";

    std::cout << "    --myanmar-search-sabbath        Search for Buddhist sabbath days\n";
    std::cout << "    --myanmar-search-sabbath-eve    Search for sabbath eve days\n";
    std::cout << "    --myanmar-search-yatyaza        Search for yatyaza (inauspicious) days\n";
    std::cout << "    --myanmar-search-pyathada       Search for pyathada (inauspicious) days\n";
    std::cout << "    --myanmar-search-thamanyo       Search for thamanyo (auspicious) days\n\n";

    std::cout << "    --myanmar-search-exact          Use exact matching (default)\n";
    std::cout << "    --myanmar-search-near TOL       Use near matching with tolerance\n";
    std::cout << "                                     • Example: --myanmar-search-near 2\n\n";

    std::cout << "    --myanmar-search-format FORMAT  Myanmar search results output format\n";
    std::cout << "                                     table = Detailed ASCII table (default)\n";
    std::cout << "                                     csv   = Comma-separated values\n";
    std::cout << "                                     json  = JSON structure for integration\n";
    std::cout << "                                     list  = Simple date list for scripts\n\n";

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
    std::cout << "    --astronomical     Show astronomical coordinates (declination, RA, inclination)\n";
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

    std::cout << "  # Sidereal chart with Lahiri ayanamsa\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --zodiac-mode sidereal --ayanamsa lahiri\n\n";

    std::cout << "  # KP System chart with Krishnamurti ayanamsa\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --zodiac-mode sidereal --ayanamsa krishnamurti \\\n";
    std::cout << "                --chart-style north-indian\n\n";

    std::cout << "  # High precision sidereal chart\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --zodiac-mode sidereal --ayanamsa lahiri \\\n";
    std::cout << "                --calculation-flags high_precision_speed,true_geometric\n\n";

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

    std::cout << "  # Conjunctions with latitude filtering (planets near ecliptic)\n";
    std::cout << "  horoscope_cli --conjunction-range 2024-01-01 2024-12-31 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 \\\n";
    std::cout << "                --conjunction-latitude-range -2.0 2.0\n\n";

    std::cout << "  # Find Graha Yuddha (Planetary Wars) in 2024\n";
    std::cout << "  horoscope_cli --graha-yuddha-range 2024-01-01 2024-12-31 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060\n\n";

    std::cout << "  # Planetary wars around birth with custom orb\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --graha-yuddha --graha-yuddha-orb 0.5\n\n";

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

    std::cout << "CALENDAR SEARCH EXAMPLES 🔍📅\n";
    std::cout << "  # Hindu calendar search for Purnima (Full Moon) days in 2025\n";
    std::cout << "  horoscope_cli --hindu-search 2025-01-01 2025-12-31 \\\n";
    std::cout << "                --search-tithi 15 \\\n";
    std::cout << "                --lat 28.6139 --lon 77.2090 \\\n";
    std::cout << "                --hindu-search-format table\n\n";

    std::cout << "  # Hindu calendar search results in CSV format\n";
    std::cout << "  horoscope_cli --hindu-search 2025-01-01 2025-03-31 \\\n";
    std::cout << "                --search-month 2 --search-tithi-range 14 16 \\\n";
    std::cout << "                --lat 19.0760 --lon 72.8777 \\\n";
    std::cout << "                --hindu-search-format csv\n\n";

    std::cout << "  # Myanmar calendar search for Full Moon Sabbath days\n";
    std::cout << "  horoscope_cli --myanmar-search 2025-01-01 2025-12-31 \\\n";
    std::cout << "                --myanmar-search-moon-phase 1 \\\n";
    std::cout << "                --myanmar-search-sabbath \\\n";
    std::cout << "                --lat 16.8661 --lon 96.1951 \\\n";
    std::cout << "                --myanmar-search-format table\n\n";

    std::cout << "  # Myanmar calendar search in JSON format\n";
    std::cout << "  horoscope_cli --myanmar-search 2025-06-01 2025-08-31 \\\n";
    std::cout << "                --myanmar-search-month 4 \\\n";
    std::cout << "                --myanmar-search-thamanyo \\\n";
    std::cout << "                --lat 16.8661 --lon 96.1951 \\\n";
    std::cout << "                --myanmar-search-format json\n\n";

    std::cout << "  # Hindu calendar search - simple date list format for scripts\n";
    std::cout << "  horoscope_cli --hindu-search 2025-01-01 2025-03-31 \\\n";
    std::cout << "                --search-tithi 15 \\\n";
    std::cout << "                --hindu-search-format list\n\n";

    std::cout << "  # Myanmar calendar search - simple date list format\n";
    std::cout << "  horoscope_cli --myanmar-search 2025-01-01 2025-12-31 \\\n";
    std::cout << "                --myanmar-search-moon-phase 1 \\\n";
    std::cout << "                --myanmar-search-format list\n\n";

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
    std::cout << "🌟 Horoscope CLI v1.1.0 ⭐\n";
    std::cout << "✨ Built with Swiss Ephemeris 🔮\n";
    std::cout << "📅 Copyright (c) 2025 🏛️\n";
    std::cout << "🚀 Professional Astrology Tool 🌌\n";
    std::cout << "🆕 Sidereal Zodiac & 21 Ayanamsa Types 🕉️\n";
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
        } else if (arg == "--zodiac-mode" && i + 1 < argc) {
            args.zodiacMode = Astro::stringToZodiacMode(argv[++i]);
        } else if (arg == "--ayanamsa" && i + 1 < argc) {
            args.ayanamsa = Astro::stringToAyanamsaType(argv[++i]);
        } else if (arg == "--calculation-flags" && i + 1 < argc) {
            std::string flagsStr = argv[++i];
            std::string flag;
            std::stringstream ss(flagsStr);
            args.calculationFlags.clear();
            while (std::getline(ss, flag, ',')) {
                // Trim whitespace
                flag.erase(0, flag.find_first_not_of(" \t"));
                flag.erase(flag.find_last_not_of(" \t") + 1);
                if (!flag.empty()) {
                    args.calculationFlags.push_back(Astro::stringToCalculationFlag(flag));
                }
            }
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
        } else if (arg == "--astronomical") {
            args.showAstronomicalCoordinates = true;
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
        } else if (arg == "--conjunction-latitude-range" && i + 2 < argc) {
            try {
                args.conjunctionMinLatitude = std::stod(argv[++i]);
                args.conjunctionMaxLatitude = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid latitude range values\n";
                return false;
            }
        } else if (arg == "--graha-yuddha") {
            args.showGrahaYuddha = true;
        } else if (arg == "--graha-yuddha-range" && i + 2 < argc) {
            args.grahaYuddhaFromDate = argv[++i];
            args.grahaYuddhaToDate = argv[++i];
            args.showGrahaYuddha = true;
        } else if (arg == "--graha-yuddha-orb" && i + 1 < argc) {
            try {
                args.grahaYuddhaMaxOrb = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid graha yuddha orb value\n";
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

        // Hindu Calendar Search options
        } else if (arg == "--hindu-search" && i + 2 < argc) {
            args.showHinduSearch = true;
            args.searchStartDate = argv[++i];
            args.searchEndDate = argv[++i];
        } else if (arg == "--search-year" && i + 1 < argc) {
            args.searchYear = std::stoi(argv[++i]);
        } else if (arg == "--search-year-range" && i + 2 < argc) {
            args.searchYearStart = std::stoi(argv[++i]);
            args.searchYearEnd = std::stoi(argv[++i]);
        } else if (arg == "--search-month" && i + 1 < argc) {
            args.searchMonth = std::stoi(argv[++i]);
        } else if (arg == "--search-month-range" && i + 2 < argc) {
            args.searchMonthStart = std::stoi(argv[++i]);
            args.searchMonthEnd = std::stoi(argv[++i]);
        } else if (arg == "--search-tithi" && i + 1 < argc) {
            args.searchTithi = std::stoi(argv[++i]);
        } else if (arg == "--search-tithi-range" && i + 2 < argc) {
            args.searchTithiStart = std::stoi(argv[++i]);
            args.searchTithiEnd = std::stoi(argv[++i]);
        } else if (arg == "--search-weekday" && i + 1 < argc) {
            args.searchWeekday = std::stoi(argv[++i]);
        } else if (arg == "--search-exact") {
            args.searchExactMatch = true;
        } else if (arg == "--search-near" && i + 1 < argc) {
            args.searchExactMatch = false;
            args.searchNearTolerance = std::stoi(argv[++i]);
        } else if (arg == "--hindu-search-format" && i + 1 < argc) {
            args.hinduSearchFormat = argv[++i];
            if (args.hinduSearchFormat != "table" && args.hinduSearchFormat != "csv" &&
                args.hinduSearchFormat != "json" && args.hinduSearchFormat != "list") {
                std::cerr << "Error: Invalid Hindu search format. Must be 'table', 'csv', 'json', or 'list'\n";
                return false;
            }

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

        // Myanmar Calendar Search options
        } else if (arg == "--myanmar-search" && i + 2 < argc) {
            args.showMyanmarSearch = true;
            args.myanmarSearchStartDate = argv[++i];
            args.myanmarSearchEndDate = argv[++i];
        } else if (arg == "--myanmar-search-year" && i + 1 < argc) {
            args.myanmarSearchYear = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-year-range" && i + 2 < argc) {
            args.myanmarSearchYearStart = std::stoi(argv[++i]);
            args.myanmarSearchYearEnd = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-month" && i + 1 < argc) {
            args.myanmarSearchMonth = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-month-range" && i + 2 < argc) {
            args.myanmarSearchMonthStart = std::stoi(argv[++i]);
            args.myanmarSearchMonthEnd = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-moon-phase" && i + 1 < argc) {
            args.myanmarSearchMoonPhase = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-moon-phase-range" && i + 2 < argc) {
            args.myanmarSearchMoonPhaseStart = std::stoi(argv[++i]);
            args.myanmarSearchMoonPhaseEnd = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-weekday" && i + 1 < argc) {
            args.myanmarSearchWeekday = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-fortnight-day" && i + 1 < argc) {
            args.myanmarSearchFortnightDay = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-fortnight-day-range" && i + 2 < argc) {
            args.myanmarSearchFortnightDayStart = std::stoi(argv[++i]);
            args.myanmarSearchFortnightDayEnd = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-sabbath") {
            args.myanmarSearchSabbath = true;
        } else if (arg == "--myanmar-search-sabbath-eve") {
            args.myanmarSearchSabbathEve = true;
        } else if (arg == "--myanmar-search-yatyaza") {
            args.myanmarSearchYatyaza = true;
        } else if (arg == "--myanmar-search-pyathada") {
            args.myanmarSearchPyathada = true;
        } else if (arg == "--myanmar-search-thamanyo") {
            args.myanmarSearchThamanyo = true;
        } else if (arg == "--myanmar-search-exact") {
            args.myanmarSearchExactMatch = true;
        } else if (arg == "--myanmar-search-near" && i + 1 < argc) {
            args.myanmarSearchExactMatch = false;
            args.myanmarSearchNearTolerance = std::stoi(argv[++i]);
        } else if (arg == "--myanmar-search-format" && i + 1 < argc) {
            args.myanmarSearchFormat = argv[++i];
            if (args.myanmarSearchFormat != "table" && args.myanmarSearchFormat != "csv" &&
                args.myanmarSearchFormat != "json" && args.myanmarSearchFormat != "list") {
                std::cerr << "Error: Invalid Myanmar search format. Must be 'table', 'csv', 'json', or 'list'\n";
                return false;
            }
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

    // Eclipse, ephemeris, panchanga, Myanmar calendar, and Hindu/Myanmar search features can work without full birth data
    if (args.showEclipses || args.showConjunctions || args.showEphemerisTable || args.showKPTransitions ||
        args.showPanchangaRange || args.showMyanmarCalendarRange || args.showHinduSearch || args.showMyanmarSearch ||
        args.showGrahaYuddha) {
        // For eclipse and conjunction range queries, we need coordinates (can come from location)
        if ((!args.eclipseFromDate.empty() || !args.conjunctionFromDate.empty() || !args.panchangaFromDate.empty() ||
             !args.myanmarCalendarFromDate.empty() || !args.searchStartDate.empty() || !args.myanmarSearchStartDate.empty() ||
             !args.grahaYuddhaFromDate.empty()) &&
            args.locationName.empty() &&
            (args.latitude < -90.0 || args.latitude > 90.0 ||
             args.longitude < -180.0 || args.longitude > 180.0)) {
            std::cerr << "Error: Valid coordinates (--lat/--lon) or location (--location) required for eclipse/conjunction/panchanga/Myanmar calendar/Hindu/Myanmar search/Graha Yuddha\n";
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

// Helper function to parse date string into BirthData for conjunction calculations
BirthData parseDateStringToBirthData(const std::string& dateStr) {
    BirthData birthData;

    // Parse the date string
    if (parseDate(dateStr, birthData.year, birthData.month, birthData.day)) {
        // Set default values for time and location
        birthData.hour = 12;      // Noon UTC
        birthData.minute = 0;
        birthData.second = 0;
        birthData.timezone = 0.0; // UTC
        birthData.latitude = 0.0; // Equator
        birthData.longitude = 0.0; // Prime Meridian
    } else {
        // If parsing fails, set to epoch
        birthData.year = 1970;
        birthData.month = 1;
        birthData.day = 1;
        birthData.hour = 0;
        birthData.minute = 0;
        birthData.second = 0;
        birthData.timezone = 0.0;
        birthData.latitude = 0.0;
        birthData.longitude = 0.0;
    }

    return birthData;
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

            std::vector<ConjunctionEvent> conjunctions;

            // Check if latitude range filtering is requested
            if (args.conjunctionMinLatitude != -90.0 || args.conjunctionMaxLatitude != 90.0) {
                conjunctions = conjCalc.findConjunctionsWithLatitudeRange(
                    parseDateStringToBirthData(fromDate),
                    parseDateStringToBirthData(toDate),
                    args.conjunctionMaxOrb,
                    args.conjunctionMinLatitude,
                    args.conjunctionMaxLatitude);
            } else {
                conjunctions = conjCalc.findConjunctions(fromDate, toDate, args.conjunctionMaxOrb);
            }

            std::cout << "\nPlanetary Conjunctions (" << fromDate << " to " << toDate << "):\n";
            std::cout << "Orb: " << args.conjunctionMaxOrb << "°";
            if (args.conjunctionMinLatitude != -90.0 || args.conjunctionMaxLatitude != 90.0) {
                std::cout << " | Latitude Range: " << args.conjunctionMinLatitude
                          << "° to " << args.conjunctionMaxLatitude << "°";
            }
            std::cout << "\n" << std::string(80, '=') << std::endl;

            for (const auto& conjunction : conjunctions) {
                conjCalc.printConjunctionEvent(conjunction);
                std::cout << std::string(80, '-') << std::endl;
            }

            if (conjunctions.empty()) {
                std::cout << "No conjunctions found in the specified period.\n";
            }
        }

        // Handle Graha Yuddha (Planetary Wars) analysis
        if (args.showGrahaYuddha || !args.grahaYuddhaFromDate.empty()) {
            ConjunctionCalculator conjCalc;
            if (!conjCalc.initialize(args.ephemerisPath)) {
                std::cerr << "Failed to initialize conjunction calculator for Graha Yuddha" << std::endl;
                return 1;
            }

            std::string fromDate = args.grahaYuddhaFromDate;
            std::string toDate = args.grahaYuddhaToDate;

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

            std::vector<ConjunctionEvent> wars = conjCalc.findGrahaYuddha(
                parseDateStringToBirthData(fromDate),
                parseDateStringToBirthData(toDate),
                args.grahaYuddhaMaxOrb);

            std::cout << conjCalc.generateGrahaYuddhaReport(wars);
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

        // Handle Hindu Calendar Search
        if (args.showHinduSearch) {
            HinduCalendar hinduCalendar;
            if (!hinduCalendar.initialize()) {
                std::cerr << "Error: Failed to initialize Hindu Calendar system: " << hinduCalendar.getLastError() << std::endl;
                return 1;
            }

            std::string fromDate = args.searchStartDate;
            std::string toDate = args.searchEndDate;

            if (fromDate.empty() || toDate.empty()) {
                std::cerr << "Error: Hindu calendar search requires --hindu-search FROM TO dates" << std::endl;
                return 1;
            }

            // Build search criteria
            HinduCalendar::SearchCriteria criteria;
            criteria.searchStartDate = fromDate;
            criteria.searchEndDate = toDate;
            criteria.exactMatch = args.searchExactMatch;
            criteria.nearMatchTolerance = args.searchNearTolerance;

            // Set search parameters
            if (args.searchYear > 0) {
                criteria.exactYear = args.searchYear;
            } else if (args.searchYearStart > 0) {
                criteria.yearRangeStart = args.searchYearStart;
                criteria.yearRangeEnd = args.searchYearEnd;
            }

            if (args.searchMonth > 0) {
                criteria.exactMonth = args.searchMonth;
            } else if (args.searchMonthStart > 0) {
                criteria.monthRangeStart = args.searchMonthStart;
                criteria.monthRangeEnd = args.searchMonthEnd;
            }

            if (args.searchTithi > 0) {
                criteria.exactTithi = args.searchTithi;
            } else if (args.searchTithiStart > 0) {
                criteria.tithiRangeStart = args.searchTithiStart;
                criteria.tithiRangeEnd = args.searchTithiEnd;
            }

            if (args.searchWeekday >= 0) {
                criteria.exactWeekday = args.searchWeekday;
            }

            // Perform search
            std::vector<HinduCalendar::SearchResult> searchResults = hinduCalendar.searchHinduCalendar(criteria, args.latitude, args.longitude);

            if (!searchResults.empty()) {
                // Generate output based on format
                if (args.hinduSearchFormat == "json") {
                    // Generate JSON output manually
                    std::cout << "{\n";
                    std::cout << "  \"search_criteria\": {\n";
                    std::cout << "    \"date_range\": \"" << criteria.searchStartDate << " to " << criteria.searchEndDate << "\",\n";
                    std::cout << "    \"exact_match\": " << (criteria.exactMatch ? "true" : "false") << ",\n";
                    std::cout << "    \"tolerance\": " << criteria.nearMatchTolerance << "\n";
                    std::cout << "  },\n";
                    std::cout << "  \"results_count\": " << searchResults.size() << ",\n";
                    std::cout << "  \"results\": [\n";
                    for (size_t i = 0; i < searchResults.size(); ++i) {
                        const auto& result = searchResults[i];
                        std::cout << "    {\n";
                        std::cout << "      \"gregorian_date\": \"" << result.gregorianDate << "\",\n";
                        std::cout << "      \"match_score\": " << result.matchScore << ",\n";
                        std::cout << "      \"panchanga\": " << hinduCalendar.generateJSON(result.panchangaData) << "\n";
                        std::cout << "    }" << (i < searchResults.size() - 1 ? "," : "") << "\n";
                    }
                    std::cout << "  ]\n";
                    std::cout << "}\n";
                } else if (args.hinduSearchFormat == "csv") {
                    // Generate CSV output manually
                    std::cout << "Date,Score,HinduYear,Month,Tithi,Vara,Nakshatra,Yoga,Karana\n";
                    for (const auto& result : searchResults) {
                        std::cout << result.gregorianDate << ","
                                  << result.matchScore << ","
                                  << result.panchangaData.year << ","
                                  << static_cast<int>(result.panchangaData.month) << ","
                                  << static_cast<int>(result.panchangaData.tithi) << ","
                                  << static_cast<int>(result.panchangaData.vara) << ","
                                  << static_cast<int>(result.panchangaData.nakshatra) << ","
                                  << static_cast<int>(result.panchangaData.yoga) << ","
                                  << static_cast<int>(result.panchangaData.karana) << "\n";
                    }
                } else if (args.hinduSearchFormat == "list") {
                    // Use Professional Table System inspired by p-ranav/tabulate
                    Astro::ProfessionalTable table;

                    // Apply Hindu calendar styling
                    table.applyHinduCalendarStyle();

                    // Set table title and subtitle with metadata
                    table.setTitle("Hindu Calendar Search Results - Professional View");

                    std::stringstream subtitleStream;
                    subtitleStream << searchResults.size() << " Results | Location: "
                                  << std::fixed << std::setprecision(2) << args.latitude << "°N, "
                                  << args.longitude << "°E | Period: " << args.searchStartDate
                                  << " ↔ " << args.searchEndDate;
                    table.setSubtitle(subtitleStream.str());

                    // Define headers with astrological context
                    std::vector<std::string> headers = {
                        "DATE", "WEEKDAY", "TITHI", "NAKSHATRA", "YOGA",
                        "KARANA", "H.MONTH", "PAKSHA", "YEAR", "MUH", "SPECIAL EVENTS", "SCORE"
                    };
                    table.addRow(headers);

                    // Add data rows with cultural formatting
                    for (const auto& result : searchResults) {
                        std::vector<std::string> row;

                        // Date column
                        row.push_back(result.gregorianDate);

                        // Weekday with auspiciousness indicator
                        std::string weekday = hinduCalendar.getVaraName(result.panchangaData.vara);
                        if (weekday == "Sunday" || weekday == "Monday" || weekday == "Thursday") {
                            weekday = "*" + weekday;  // Auspicious days
                        }
                        row.push_back(weekday);

                        // Tithi with lunar phase indicators
                        std::string tithi = hinduCalendar.getTithiName(result.panchangaData.tithi);
                        if (tithi.find("Purnima") != std::string::npos) tithi = "O" + tithi;
                        else if (tithi.find("Amavasya") != std::string::npos) tithi = "@" + tithi;
                        else if (tithi.find("Ekadashi") != std::string::npos) tithi = "!" + tithi;
                        row.push_back(tithi);

                        // Nakshatra with constellation grouping
                        std::string nakshatra = hinduCalendar.getNakshatraName(result.panchangaData.nakshatra);
                        std::vector<std::string> devaGanas = {"Ashwini", "Mrigashirsha", "Punarvasu", "Pushya", "Hasta", "Swati", "Anuradha", "Shravana", "Revati"};
                        for (const auto& deva : devaGanas) {
                            if (nakshatra.find(deva.substr(0, 4)) != std::string::npos) {
                                nakshatra = "+" + nakshatra;
                                break;
                            }
                        }
                        row.push_back(nakshatra);

                        // Yoga with benefic indicators
                        std::string yoga = hinduCalendar.getYogaName(result.panchangaData.yoga);
                        if (yoga == "Siddhi" || yoga == "Shubha" || yoga == "Amrita") {
                            yoga = "#" + yoga;
                        }
                        row.push_back(yoga);

                        // Karana
                        row.push_back(hinduCalendar.getKaranaName(result.panchangaData.karana));

                        // Hindu Month with seasonal indicators
                        std::string hmonth = hinduCalendar.getHinduMonthName(result.panchangaData.month);
                        std::vector<std::string> springMonths = {"Chaitra", "Vaishakha"};
                        for (const auto& spring : springMonths) {
                            if (hmonth.find(spring.substr(0, 4)) != std::string::npos) {
                                hmonth = "~" + hmonth;
                                break;
                            }
                        }
                        row.push_back(hmonth);

                        // Paksha with moon phase visual
                        std::string paksha = result.panchangaData.isShukla ? ">Shukla" : "<Krishna";
                        row.push_back(paksha);

                        // Year
                        row.push_back(std::to_string(result.panchangaData.year));

                        // Muhurta with indicators
                        std::string muhurta;
                        if (result.panchangaData.isShubhaMuhurta) muhurta = "Good";
                        else if (result.panchangaData.isAshubhaMuhurta) muhurta = "Warn";
                        else muhurta = "Neut";
                        row.push_back(muhurta);

                        // Special Events with priority display
                        std::string specialEvents;
                        if (result.panchangaData.isEkadashi) specialEvents += "Eka ";
                        if (result.panchangaData.isPurnima) specialEvents += "Pur ";
                        if (result.panchangaData.isAmavasya) specialEvents += "Ama ";
                        if (result.panchangaData.isSankranti) specialEvents += "San ";

                        if (!result.panchangaData.festivals.empty()) {
                            std::string fest = result.panchangaData.festivals[0];
                            if (fest.length() > 8) fest = fest.substr(0, 6) + "..";
                            specialEvents += fest;
                        }

                        if (specialEvents.empty()) specialEvents = "-";
                        row.push_back(specialEvents);

                        // Score with quality indicator
                        std::string scoreDisplay = std::to_string(result.matchScore);
                        if (scoreDisplay.length() > 5) scoreDisplay = scoreDisplay.substr(0, 5);
                        if (result.matchScore >= 0.90) scoreDisplay = "*" + scoreDisplay;
                        else if (result.matchScore >= 0.75) scoreDisplay = "#" + scoreDisplay;
                        row.push_back(scoreDisplay);

                        table.addRow(row);
                    }

                    // Output the professional table
                    std::cout << "\n" << table.toString() << std::endl;

                    // Add legend
                    std::cout << "\n========================================= LEGEND & SYMBOLS ==========================================\n";
                    std::cout << "MUHURTA: Good=Shubha(Auspicious) Warn=Ashubha(Inauspicious) Neut=Samanya(Neutral)\n";
                    std::cout << "WEEKDAY: *=Highly Auspicious (Sun/Mon/Thu) • Others=Normal\n";
                    std::cout << "TITHI: O=Purnima(Full Moon) @=Amavasya(New Moon) !=Ekadashi(Sacred)\n";
                    std::cout << "NAKSHATRA: +=Deva Gana(Divine) • Others=Manushya/Rakshasa Gana\n";
                    std::cout << "YOGA: #=Highly Beneficial • Others=Regular Combinations\n";
                    std::cout << "MONTH: ~=Spring Season • Others=Summer/Monsoon/Autumn/Winter\n";
                    std::cout << "PAKSHA: >=Shukla(Bright Fortnight) <=Krishna(Dark Fortnight)\n";
                    std::cout << "EVENTS: Eka=Ekadashi Pur=Purnima Ama=Amavasya San=Sankranti + Festival names\n";
                    std::cout << "SCORE: *=Excellent(>=0.9) #=Good(>=0.75) •=Standard(<0.75)\n";
                    std::cout << "========================================================================================================\n";

                } else {
                    // Default table format
                    std::cout << "\n🔍 HINDU CALENDAR SEARCH RESULTS 🕉️\n";
                    std::cout << "═══════════════════════════════════════════════\n\n";
                    std::cout << "Found " << searchResults.size() << " matching dates\n";
                    std::cout << "Search criteria: ";

                    bool first = true;
                    if (criteria.exactYear > 0) {
                        if (!first) std::cout << ", ";
                        std::cout << "Year=" << criteria.exactYear;
                        first = false;
                    } else if (criteria.yearRangeStart > 0) {
                        if (!first) std::cout << ", ";
                        std::cout << "Year=" << criteria.yearRangeStart << "-" << criteria.yearRangeEnd;
                        first = false;
                    }
                    if (criteria.exactMonth > 0) {
                        if (!first) std::cout << ", ";
                        std::cout << "Month=" << criteria.exactMonth;
                        first = false;
                    } else if (criteria.monthRangeStart > 0) {
                        if (!first) std::cout << ", ";
                        std::cout << "Month=" << criteria.monthRangeStart << "-" << criteria.monthRangeEnd;
                        first = false;
                    }
                    if (criteria.exactTithi > 0) {
                        if (!first) std::cout << ", ";
                        std::cout << "Tithi=" << criteria.exactTithi;
                        first = false;
                    } else if (criteria.tithiRangeStart > 0) {
                        if (!first) std::cout << ", ";
                        std::cout << "Tithi=" << criteria.tithiRangeStart << "-" << criteria.tithiRangeEnd;
                        first = false;
                    }
                    if (criteria.exactWeekday >= 0) {
                        if (!first) std::cout << ", ";
                        std::string weekdayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
                        std::cout << "Weekday=" << weekdayNames[criteria.exactWeekday];
                        first = false;
                    }
                    std::cout << "\n";
                    std::cout << "Match type: " << (criteria.exactMatch ? "Exact" : "Near (tolerance=" + std::to_string(criteria.nearMatchTolerance) + ")") << "\n\n";

                    for (size_t i = 0; i < searchResults.size(); ++i) {
                        const auto& result = searchResults[i];
                        std::cout << "Result #" << (i + 1) << " (Score: " << result.matchScore << ")\n";
                        std::cout << "Date: " << result.gregorianDate << "\n";
                        std::cout << hinduCalendar.generatePanchangaTable(result.panchangaData) << "\n";
                        if (i < searchResults.size() - 1) {
                            std::cout << "-----------------------------------------------\n\n";
                        }
                    }
                }
            } else {
                std::cout << "\n🔍 HINDU CALENDAR SEARCH RESULTS 🕉️\n";
                std::cout << "═══════════════════════════════════════════════\n\n";
                std::cout << "No matching dates found for the specified criteria.\n";
                std::cout << "Try adjusting your search parameters or using --search-near with a higher tolerance.\n";
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

        // Handle Myanmar Calendar Search
        if (args.showMyanmarSearch) {
            MyanmarCalendar myanmarCalendar;

            // Initialize the Myanmar calendar
            if (!myanmarCalendar.initialize()) {
                std::cerr << "Error: Failed to initialize Myanmar calendar" << std::endl;
                return 1;
            }

            if (args.myanmarSearchStartDate.empty() || args.myanmarSearchEndDate.empty()) {
                std::cerr << "Error: Myanmar calendar search requires --myanmar-search FROM TO dates" << std::endl;
                return 1;
            }

            MyanmarCalendar::SearchCriteria criteria;
            criteria.searchStartDate = args.myanmarSearchStartDate;
            criteria.searchEndDate = args.myanmarSearchEndDate;

            // Set search parameters
            if (args.myanmarSearchYear != -1) {
                criteria.exactYear = args.myanmarSearchYear;
            }
            if (args.myanmarSearchYearStart != -1 && args.myanmarSearchYearEnd != -1) {
                criteria.yearRangeStart = args.myanmarSearchYearStart;
                criteria.yearRangeEnd = args.myanmarSearchYearEnd;
            }
            if (args.myanmarSearchMonth != -1) {
                criteria.exactMonth = args.myanmarSearchMonth;
            }
            if (args.myanmarSearchMonthStart != -1 && args.myanmarSearchMonthEnd != -1) {
                criteria.monthRangeStart = args.myanmarSearchMonthStart;
                criteria.monthRangeEnd = args.myanmarSearchMonthEnd;
            }
            if (args.myanmarSearchMoonPhase != -1) {
                criteria.exactMoonPhase = args.myanmarSearchMoonPhase;
            }
            if (args.myanmarSearchMoonPhaseStart != -1 && args.myanmarSearchMoonPhaseEnd != -1) {
                criteria.moonPhaseRangeStart = args.myanmarSearchMoonPhaseStart;
                criteria.moonPhaseRangeEnd = args.myanmarSearchMoonPhaseEnd;
            }
            if (args.myanmarSearchWeekday != -1) {
                criteria.exactWeekday = args.myanmarSearchWeekday;
            }
            if (args.myanmarSearchFortnightDay != -1) {
                criteria.exactFortnightDay = args.myanmarSearchFortnightDay;
            }
            if (args.myanmarSearchFortnightDayStart != -1 && args.myanmarSearchFortnightDayEnd != -1) {
                criteria.fortnightDayRangeStart = args.myanmarSearchFortnightDayStart;
                criteria.fortnightDayRangeEnd = args.myanmarSearchFortnightDayEnd;
            }

            // Set astrological criteria
            criteria.searchSabbath = args.myanmarSearchSabbath;
            criteria.searchSabbathEve = args.myanmarSearchSabbathEve;
            criteria.searchYatyaza = args.myanmarSearchYatyaza;
            criteria.searchPyathada = args.myanmarSearchPyathada;
            criteria.searchThamanyo = args.myanmarSearchThamanyo;

            // Set matching mode
            criteria.exactMatch = args.myanmarSearchExactMatch;
            criteria.nearMatchTolerance = args.myanmarSearchNearTolerance;

            std::vector<MyanmarCalendar::SearchResult> searchResults = myanmarCalendar.searchMyanmarCalendar(criteria, args.latitude, args.longitude);

            if (!searchResults.empty()) {
                // Generate output based on format
                if (args.myanmarSearchFormat == "json") {
                    // Generate JSON output manually
                    std::cout << "{\n";
                    std::cout << "  \"search_criteria\": {\n";
                    std::cout << "    \"date_range\": \"" << criteria.searchStartDate << " to " << criteria.searchEndDate << "\",\n";
                    std::cout << "    \"exact_match\": " << (criteria.exactMatch ? "true" : "false") << ",\n";
                    std::cout << "    \"tolerance\": " << criteria.nearMatchTolerance << "\n";
                    std::cout << "  },\n";
                    std::cout << "  \"results_count\": " << searchResults.size() << ",\n";
                    std::cout << "  \"results\": [\n";
                    for (size_t i = 0; i < searchResults.size(); ++i) {
                        const auto& result = searchResults[i];
                        std::cout << "    {\n";
                        std::cout << "      \"gregorian_date\": \"" << result.gregorianDate << "\",\n";
                        std::cout << "      \"match_score\": " << result.matchScore << ",\n";
                        std::cout << "      \"match_description\": \"" << result.matchDescription << "\",\n";
                        std::cout << "      \"myanmar_data\": " << myanmarCalendar.generateJSON(result.myanmarData) << "\n";
                        std::cout << "    }" << (i < searchResults.size() - 1 ? "," : "") << "\n";
                    }
                    std::cout << "  ]\n";
                    std::cout << "}\n";
                } else if (args.myanmarSearchFormat == "csv") {
                    // Generate CSV output manually
                    std::cout << "Date,Score,Description,MyanmarYear,Month,MoonPhase,FortnightDay,Weekday\n";
                    for (const auto& result : searchResults) {
                        std::cout << result.gregorianDate << ","
                                  << result.matchScore << ","
                                  << "\"" << result.matchDescription << "\","
                                  << result.myanmarData.myanmarYear << ","
                                  << static_cast<int>(result.myanmarData.month) << ","
                                  << static_cast<int>(result.myanmarData.moonPhase) << ","
                                  << result.myanmarData.fortnightDay << ","
                                  << static_cast<int>(result.myanmarData.weekday) << "\n";
                    }
                } else if (args.myanmarSearchFormat == "list") {
                    // Generate professional-grade Myanmar tabular output with enhanced design and ASCII borders
                    std::cout << "\n";
                    std::cout << "+==================================================================================================================================================================================================+\n";
                    std::cout << "|                                             🇲🇲  MYANMAR CALENDAR SEARCH RESULTS - PROFESSIONAL VIEW  🇲🇲                                                                            |\n";
                    std::cout << "+==================================================================================================================================================================================================+\n";
                    std::cout << "| " << std::setw(3) << std::right << searchResults.size() << " Results Found"
                              << " | Location: " << std::fixed << std::setprecision(2) << args.latitude << "°N, " << args.longitude << "°E"
                              << " | Period: " << args.myanmarSearchStartDate << " -> " << args.myanmarSearchEndDate;

                    // Calculate padding for right alignment
                    size_t contentLength = std::to_string(searchResults.size()).length() + 15 +
                                         (args.latitude >= 0 ? std::to_string((int)args.latitude).length() + 3 : std::to_string((int)args.latitude).length() + 3) +
                                         (args.longitude >= 0 ? std::to_string((int)args.longitude).length() + 3 : std::to_string((int)args.longitude).length() + 3) +
                                         args.myanmarSearchStartDate.length() + args.myanmarSearchEndDate.length() + 19;
                    size_t padding = (180 > contentLength) ? (180 - contentLength) : 0;
                    std::cout << std::string(padding, ' ') << " |\n";

                    std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+--------+----------+----------------+------+\n";
                    std::cout << "|    DATE     |  WEEKDAY   |MY.YR |    MONTH    | DAY |MOON.PHASE |  MAHABOTE  | NAKHAT  | NAGAHLE| RELIGIOUS|  ASTRO.DAYS    |SCORE |\n";
                    std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+--------+----------+----------------+------+";

                    // Professional Myanmar table rows with enhanced formatting
                    size_t rowCount = 0;
                    for (const auto& result : searchResults) {
                        rowCount++;

                        // Date column with center alignment
                        std::cout << "| " << std::setw(11) << std::left << result.gregorianDate << " | ";

                        // Weekday column with Myanmar cultural indicators - fixed 10 chars
                        std::string weekday = myanmarCalendar.getMyanmarWeekdayName(result.myanmarData.weekday);
                        std::string weekdayDisplay = weekday;

                        // Create consistent weekday abbreviations
                        if (weekday == "Sunday") weekdayDisplay = "⚡Sunday";
                        else if (weekday == "Monday") weekdayDisplay = "☾Monday";
                        else if (weekday == "Tuesday") weekdayDisplay = "⚡Tuesday";
                        else if (weekday == "Wednesday") weekdayDisplay = "Wednesday";
                        else if (weekday == "Thursday") weekdayDisplay = "☾Thursday";
                        else if (weekday == "Friday") weekdayDisplay = "Friday";
                        else if (weekday == "Saturday") weekdayDisplay = "Saturday";

                        // Ensure fixed width of exactly 10 characters
                        if (weekdayDisplay.length() > 10) weekdayDisplay = weekdayDisplay.substr(0, 10);
                        std::cout << std::setw(10) << std::left << weekdayDisplay << " | ";

                        // Myanmar Year column - right aligned
                        std::cout << std::setw(4) << std::right << result.myanmarData.myanmarYear << " | ";

                        // Month column with seasonal indicators
                        std::string month = myanmarCalendar.getMyanmarMonthName(result.myanmarData.month);
                        if (month.length() > 11) month = month.substr(0, 9) + "..";
                        // Add seasonal context for Myanmar months
                        std::vector<std::string> hotMonths = {"Tagu", "Kason", "Nayon"};
                        std::vector<std::string> rainyMonths = {"Waso", "Wagaung", "Tawthalin"};
                        std::vector<std::string> coolMonths = {"Thadingyut", "Tazaungmon", "Nadaw", "Pyatho", "Tabodwe", "Tabaung"};

                        std::string monthDisplay = month;
                        for (const auto& hot : hotMonths) {
                            if (month.find(hot) != std::string::npos) {
                                monthDisplay = "☀" + month.substr(0, 10);
                                break;
                            }
                        }
                        for (const auto& rainy : rainyMonths) {
                            if (month.find(rainy) != std::string::npos) {
                                monthDisplay = "🌧" + month.substr(0, 10);
                                break;
                            }
                        }
                        for (const auto& cool : coolMonths) {
                            if (month.find(cool) != std::string::npos) {
                                monthDisplay = "❄" + month.substr(0, 10);
                                break;
                            }
                        }
                        std::cout << std::setw(11) << std::left << monthDisplay << " | ";

                        // Day column - center aligned
                        std::cout << std::setw(3) << std::right << result.myanmarData.dayOfMonth << " | ";

                        // Moon Phase column with visual indicators
                        std::string moonPhase = myanmarCalendar.getMoonPhaseName(result.myanmarData.moonPhase);
                        if (moonPhase.length() > 9) moonPhase = moonPhase.substr(0, 7) + "..";
                        // Add moon phase visual symbols
                        if (moonPhase.find("Full") != std::string::npos) moonPhase = "🌕" + moonPhase.substr(0, 8);
                        else if (moonPhase.find("New") != std::string::npos) moonPhase = "🌑" + moonPhase.substr(0, 8);
                        else if (moonPhase.find("Waxing") != std::string::npos) moonPhase = "🌔" + moonPhase.substr(0, 8);
                        else if (moonPhase.find("Waning") != std::string::npos) moonPhase = "🌖" + moonPhase.substr(0, 8);
                        std::cout << std::setw(9) << std::left << moonPhase << " | ";

                        // Mahabote column with quality indicators - fixed 10 chars
                        std::string mahabote;
                        switch(result.myanmarData.mahabote) {
                            case Mahabote::BINGA: mahabote = "♦Binga"; break;      // Benefic
                            case Mahabote::ATUN: mahabote = "◇Atun"; break;       // Neutral
                            case Mahabote::YAZA: mahabote = "⚠Yaza"; break;       // Malefic
                            case Mahabote::ADIPATI: mahabote = "♦Adipati"; break;  // Benefic
                            case Mahabote::MARANA: mahabote = "⚠Marana"; break;   // Malefic
                            case Mahabote::THIKE: mahabote = "◇Thike"; break;     // Neutral
                            case Mahabote::PUTI: mahabote = "♦Puti"; break;       // Benefic
                        }

                        // Ensure fixed width of exactly 10 characters
                        if (mahabote.length() > 10) mahabote = mahabote.substr(0, 10);
                        std::cout << std::setw(10) << std::left << mahabote << " | ";

                        // Nakhat column with cycle indicators
                        std::string nakhat;
                        switch(result.myanmarData.nakhat) {
                            case Nakhat::ORC: nakhat = "🔴Orc"; break;      // Active/Aggressive
                            case Nakhat::ELF: nakhat = "🟢Elf"; break;      // Peaceful/Wise
                            case Nakhat::HUMAN: nakhat = "🟡Human"; break;  // Balanced
                        }
                        std::cout << std::setw(7) << std::left << nakhat << " | ";

                        // Nagahle column with directional symbols
                        std::string nagahle;
                        switch(result.myanmarData.nagahle) {
                            case NagahleDirection::WEST: nagahle = "⬅West"; break;
                            case NagahleDirection::NORTH: nagahle = "⬆North"; break;
                            case NagahleDirection::EAST: nagahle = "➡East"; break;
                            case NagahleDirection::SOUTH: nagahle = "⬇South"; break;
                        }
                        std::cout << std::setw(6) << std::left << nagahle << " | ";

                        // Religious days column with Buddhist symbols
                        std::string religious;
                        if (result.myanmarData.isSabbath) religious = "☸Sabbath";
                        else if (result.myanmarData.isSabbathEve) religious = "☸Eve";
                        else religious = "   -   ";
                        std::cout << std::setw(8) << std::left << religious << " | ";

                        // Astrological days column with enhanced indicators
                        std::vector<std::string> astro;
                        if (result.myanmarData.isYatyaza) astro.push_back("⚠Yat");
                        if (result.myanmarData.isPyathada) astro.push_back("⚠Pya");
                        if (result.myanmarData.isThamanyo) astro.push_back("✅Tha");
                        if (result.myanmarData.isAmyeittasote) astro.push_back("◇Amy");
                        if (result.myanmarData.isWarameittugyi) astro.push_back("◇War");

                        std::string astroStr;
                        for (size_t i = 0; i < astro.size() && astroStr.length() < 12; ++i) {
                            if (i > 0) astroStr += " ";
                            astroStr += astro[i];
                        }
                        if (astroStr.empty()) astroStr = "      -      ";
                        if (astroStr.length() > 14) astroStr = astroStr.substr(0, 11) + "...";
                        std::cout << std::setw(14) << std::left << astroStr << " | ";

                        // Score column with quality indicator
                        std::string scoreDisplay = std::to_string(result.matchScore);
                        if (scoreDisplay.length() > 4) scoreDisplay = scoreDisplay.substr(0, 4);
                        if (result.matchScore >= 0.90) scoreDisplay = "★" + scoreDisplay;
                        else if (result.matchScore >= 0.75) scoreDisplay = "◆" + scoreDisplay;
                        std::cout << std::setw(4) << std::right << scoreDisplay << " |";

                        std::cout << "\n";

                        // Add subtle row separators every 5 rows
                        if (rowCount % 5 == 0 && rowCount < searchResults.size()) {
                            std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+--------+----------+----------------+------+\n";
                        }
                    }

                    std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+--------+----------+----------------+------+\n";

                    // Enhanced professional legend for Myanmar calendar
                    std::cout << "+---------------------------------------------- MYANMAR LEGEND & SYMBOLS ----------------------------------------------+\n";
                    std::cout << "| WEEKDAY: ⚡=Power Days(Sun/Tue) ☾=Moon Days(Mon/Thu) •=Regular Days                                                |\n";
                    std::cout << "| MONTH: ☀=Hot Season(Tagu-Nayon) 🌧=Rainy Season(Waso-Tawthalin) ❄=Cool Season(Thadingyut-Tabaung)                |\n";
                    std::cout << "| MOON: 🌕=Full Moon 🌑=New Moon 🌔=Waxing 🌖=Waning                                                                |\n";
                    std::cout << "| MAHABOTE: ♦=Benefic(Binga/Adipati/Puti) ⚠=Malefic(Yaza/Marana) ◇=Neutral(Atun/Thike)                            |\n";
                    std::cout << "| NAKHAT: 🔴=Orc(Active) 🟢=Elf(Peaceful) 🟡=Human(Balanced) • 3-Year Cycle                                       |\n";
                    std::cout << "| NAGAHLE: ⬅=West ⬆=North ➡=East ⬇=South • Snake Head Direction                                                   |\n";
                    std::cout << "| RELIGIOUS: ☸=Buddhist Sabbath/Eve • Important for Merit-making                                                   |\n";
                    std::cout << "| ASTRO.DAYS: ⚠Yat=Yatyaza(Avoid) ⚠Pya=Pyathada(Caution) ✅Tha=Thamanyo(Auspicious) ◇=Others                     |\n";
                    std::cout << "| SCORE: ★=Excellent(≥0.9) ◆=Good(≥0.75) •=Standard(<0.75)                                                        |\n";
                    std::cout << "+-------------------------------------------------------------------------------------------------------------------+\n";
                    std::cout << "| YEAR TYPE: " << (searchResults.empty() ? "N/A" :
                                  (searchResults[0].myanmarData.yearType == MyanmarYearType::COMMON ? "📅 Common Year" :
                                   searchResults[0].myanmarData.yearType == MyanmarYearType::LITTLE_WATAT ? "📅 Little Watat (13 months)" :
                                   "📅 Big Watat (13 months + extra day)")) << "                                |\n";
                    std::cout << "| Full detailed analysis available using 'table' format • Buddhist Era: Myanmar Calendar System                     |\n";
                    std::cout << "+-------------------------------------------------------------------------------------------------------------------+\n";
                } else {
                    // Default table format
                    std::cout << "\n🔍 MYANMAR CALENDAR SEARCH RESULTS 🇲🇲\n";
                    std::cout << "═══════════════════════════════════════════════════════\n";
                    std::cout << "Found " << searchResults.size() << " matching days:\n\n";

                    for (const auto& result : searchResults) {
                        std::cout << "📅 Date: " << result.gregorianDate << " (Score: " << result.matchScore << ")\n";
                        std::cout << "   Myanmar Year: " << result.myanmarData.myanmarYear << ", Month: " << myanmarCalendar.getMyanmarMonthName(result.myanmarData.month);
                        std::cout << ", Day: " << result.myanmarData.fortnightDay << " (" << myanmarCalendar.getMoonPhaseName(result.myanmarData.moonPhase) << ")\n";
                        std::cout << "   Weekday: " << myanmarCalendar.getMyanmarWeekdayName(result.myanmarData.weekday) << "\n";

                        if (!result.matchDescription.empty()) {
                            std::cout << "   🔮 " << result.matchDescription << "\n";
                        }
                        std::cout << "\n";
                    }
                }
            } else {
                std::cout << "\n🔍 MYANMAR CALENDAR SEARCH RESULTS 🇲🇲\n";
                std::cout << "═══════════════════════════════════════════════════════\n";
                std::cout << "No matching days found for the specified criteria.\n\n";

                std::cout << "Search Criteria Summary:\n";
                std::cout << "• Date Range: " << criteria.searchStartDate << " to " << criteria.searchEndDate << "\n";
                if (criteria.exactYear != -1) std::cout << "• Myanmar Year: " << criteria.exactYear << "\n";
                if (criteria.yearRangeStart != -1 && criteria.yearRangeEnd != -1) {
                    std::cout << "• Myanmar Year Range: " << criteria.yearRangeStart << " to " << criteria.yearRangeEnd << "\n";
                }
                if (criteria.exactMonth != -1) std::cout << "• Month: " << criteria.exactMonth << "\n";
                if (criteria.monthRangeStart != -1 && criteria.monthRangeEnd != -1) {
                    std::cout << "• Month Range: " << criteria.monthRangeStart << " to " << criteria.monthRangeEnd << "\n";
                }
                if (criteria.exactMoonPhase != -1) std::cout << "• Moon Phase: " << criteria.exactMoonPhase << "\n";
                if (criteria.moonPhaseRangeStart != -1 && criteria.moonPhaseRangeEnd != -1) {
                    std::cout << "• Moon Phase Range: " << criteria.moonPhaseRangeStart << " to " << criteria.moonPhaseRangeEnd << "\n";
                }
                if (criteria.exactWeekday != -1) std::cout << "• Weekday: " << criteria.exactWeekday << "\n";
                if (criteria.exactFortnightDay != -1) std::cout << "• Fortnight Day: " << criteria.exactFortnightDay << "\n";
                if (criteria.fortnightDayRangeStart != -1 && criteria.fortnightDayRangeEnd != -1) {
                    std::cout << "• Fortnight Day Range: " << criteria.fortnightDayRangeStart << " to " << criteria.fortnightDayRangeEnd << "\n";
                }
                if (criteria.searchSabbath) std::cout << "• Looking for Sabbath days\n";
                if (criteria.searchSabbathEve) std::cout << "• Looking for Sabbath eve days\n";
                if (criteria.searchYatyaza) std::cout << "• Looking for Yatyaza days\n";
                if (criteria.searchPyathada) std::cout << "• Looking for Pyathada days\n";
                if (criteria.searchThamanyo) std::cout << "• Looking for Thamanyo days\n";

                std::cout << "\nTry adjusting your search criteria or expanding the date range.\n";
            }
        }

        // Return early if only special features were requested
        if (args.showEclipses || args.showConjunctions || args.showEphemerisTable ||
            args.showKPTransitions || args.showPanchangaRange || args.showMyanmarCalendarRange ||
            args.showHinduSearch || args.showMyanmarSearch || args.showGrahaYuddha) {
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

    // Set calculation parameters
    calculator.setZodiacMode(args.zodiacMode);
    calculator.setAyanamsa(args.ayanamsa);
    if (!args.calculationFlags.empty()) {
        calculator.setCalculationFlags(args.calculationFlags);
    }

    // Calculate birth chart
    BirthChart chart;
    if (!calculator.calculateBirthChart(birthData, args.houseSystem, args.zodiacMode, args.ayanamsa, chart)) {
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
        std::cout << chart.getFormattedChart(args.showAstronomicalCoordinates) << std::endl;
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
            std::cout << chart.getFormattedChart(args.showAstronomicalCoordinates) << std::endl;

            // Add Eastern chart
            EasternChartDrawer chartDrawer;
            chartDrawer.setChartStyle(args.chartStyle);
            std::cout << chartDrawer.drawEasternChart(chart) << std::endl;
        } else if (args.chartStyle.empty()) {
            // No chart style specified: show only basic formatted chart data
            std::cout << chart.getFormattedChart(args.showAstronomicalCoordinates) << std::endl;
        } else {
            // Default to basic text output
            std::cout << chart.getFormattedChart(args.showAstronomicalCoordinates) << std::endl;
        }
    }

    return 0;
}
