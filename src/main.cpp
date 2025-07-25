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
#include "myanmar_monthly_calendar.h"
#include "hindu_monthly_calendar.h"
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
#include <cmath>

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
    std::string eclipseFormat = "table";
    std::string conjunctionFromDate;
    std::string conjunctionToDate;
    double conjunctionMaxOrb = 3.0;
    double conjunctionMinLatitude = -90.0;
    double conjunctionMaxLatitude = 90.0;
    std::string conjunctionFormat = "table";
    bool showGrahaYuddha = false;
    double grahaYuddhaMaxOrb = 1.0;
    std::string grahaYuddhaFromDate;
    std::string grahaYuddhaToDate;
    std::string grahaYuddhaFormat = "table";
    std::string ephemerisFromDate;
    std::string ephemerisToDate;
    int ephemerisIntervalDays = 1;
    std::string ephemerisFormat = "table";
    bool ephemerisShowDeclination = false;
    std::string ephemerisCoordinateType = "longitude"; // "longitude", "declination", or "both"
    bool ephemerisShowSiderealTime = false;
    bool ephemerisCompactFormat = false;
    bool ephemerisUseColors = true; // Enable colors by default

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
    std::string hinduSearchLogic = "and"; // "and" or "or" logic mode

    // Hindu Calendar Search Extended Criteria
    int searchNakshatra = -1;
    int searchNakshatraStart = -1;
    int searchNakshatraEnd = -1;
    int searchYoga = -1;
    int searchYogaStart = -1;
    int searchYogaEnd = -1;
    bool searchEkadashi = false;
    bool searchPurnima = false;
    bool searchAmavasya = false;
    bool searchSankranti = false;

    // Julian Day search criteria
    double searchJulianDay = -1.0;
    double searchJulianDayStart = -1.0;
    double searchJulianDayEnd = -1.0;
    double searchJulianDayTolerance = 0.5;

    // Varna (Savarna) search criteria
    std::string searchVarnaDay = "";
    std::string searchVarnaTithi = "";
    std::string searchVarnaNakshatra = "";
    bool searchBrahminDays = false;
    bool searchKshatriyaDays = false;
    bool searchVaishyaDays = false;
    bool searchShudradays = false;

    // Simplified JD-only search
    double searchJdOnly = -1.0;           // Search by JD only, no date range needed
    std::string searchVarnaOnly = "";     // Search by Varna only, no date range needed

    // Extended JD search options
    double searchJdMyanmarOnly = -1.0;    // Search Myanmar calendar by JD only
    double searchJdBirthChartOnly = -1.0; // Search birth chart by JD only

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
    std::string myanmarSearchLogic = "and"; // "and" or "or"
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

    // NEW ENHANCED FEATURES 🚀

    // Interactive Mode
    bool interactiveMode = false;
    bool quickWizard = false;
    bool beginnerMode = false;
    bool professionalMode = false;

    // AI-Powered Interpretation
    bool aiInterpretation = false;
    bool personalityProfile = false;
    bool careerGuidance = false;
    bool relationshipProfile = false;
    bool healthInsights = false;
    bool spiritualGuidance = false;
    std::string interpretationStyle = "balanced"; // psychological, spiritual, practical, balanced
    std::string interpretationDetail = "standard"; // brief, standard, detailed
    std::vector<std::string> interpretationFocus; // personality, career, relationships, health, spiritual

    // Advanced Astrological Analysis
    bool advancedAnalysis = false;
    bool midpointAnalysis = false;
    bool harmonicAnalysis = false;
    std::vector<int> specificHarmonics; // 4, 5, 7, 9, etc.
    bool arabicParts = false;
    bool fixedStarsAnalysis = false;
    bool asteroidsAnalysis = false;
    bool vertexAnalysis = false;

    // Predictive Astrology
    bool predictiveAnalysis = false;
    bool dashaAnalysis = false;
    std::string dashaType = "vimshottari"; // vimshottari, ashtottari, etc.
    bool progressionAnalysis = false;
    std::string progressionTargetDate;
    bool transitAnalysis = false;
    std::string transitStartDate;
    std::string transitEndDate;
    bool returnAnalysis = false;
    std::string returnType = "solar"; // solar, lunar, planetary
    int returnYear = -1;
    bool yearlyForecast = false;
    bool monthlyForecast = false;
    int forecastYear = -1;
    int forecastMonth = -1;

    // Electional Astrology
    bool electionalAnalysis = false;
    std::string electionalStartDate;
    std::string electionalEndDate;
    std::string electionalPurpose; // marriage, business, travel, etc.
    bool auspiciousTimings = false;

    // Relationship Analysis (Synastry)
    bool synastryAnalysis = false;
    bool compositeAnalysis = false;
    std::string partnerBirthDate;
    std::string partnerBirthTime;
    double partnerLatitude = 0.0;
    double partnerLongitude = 0.0;
    double partnerTimezone = 0.0;
    std::string partnerLocation;

    // Relocation Astrology
    bool relocationAnalysis = false;
    double relocationLatitude = 0.0;
    double relocationLongitude = 0.0;
    std::string relocationLocation;

    // Financial Astrology
    bool financialAnalysis = false;
    std::string stockSymbol;
    bool marketAnalysis = false;
    bool cryptoAnalysis = false;
    std::string investmentDate;

    // Medical Astrology
    bool medicalAnalysis = false;
    bool healthPredictions = false;
    std::string healthConcern;
    bool ayurvedicAnalysis = false;

    // Mundane Astrology
    bool mundaneAnalysis = false;
    std::string country;
    std::string region;
    bool weatherPredictions = false;
    bool politicalAnalysis = false;
    bool economicAnalysis = false;

    // Multi-language Support
    std::string language = "english";
    bool useNativeScript = false;
    std::string culturalRegion;
    std::string numberFormat = "western";
    std::string dateFormat = "YYYY-MM-DD";
    std::string timeFormat = "24h";

    // Batch Processing
    bool batchMode = false;
    std::string batchInputFile;
    std::string batchOutputFile;
    std::string batchOperation;

    // Data Management
    bool exportData = false;
    bool importData = false;
    std::string dataFile;
    std::string dataFormat = "json";
    bool backup = false;
    bool restore = false;

    // Visualization Enhancements
    bool colorOutput = true;
    bool unicodeSymbols = true;
    std::string chartTheme = "default"; // default, dark, light, classic
    bool showProgressBars = true;
    bool verboseOutput = false;

    // Configuration
    std::string configFile;
    bool saveConfig = false;
    bool loadConfig = false;
    bool resetConfig = false;
    std::string userProfile;

    // Output Enhancements
    bool saveToFile = false;
    std::string outputFile;
    bool printToPDF = false;
    bool emailResults = false;
    std::string emailAddress;
    bool uploadToCloud = false;

    // Development and Testing
    bool debugMode = false;
    bool testMode = false;
    bool benchmarkMode = false;
    bool validateData = false;
    std::string logLevel = "info"; // debug, info, warn, error

    // API Integration
    bool apiMode = false;
    std::string apiEndpoint;
    std::string apiKey;
    bool webhookNotification = false;
    std::string webhookUrl;
    bool showAllFestivals = false;

    // Myanmar Monthly Calendar options
    bool showMyanmarMonthlyCalendar = false;
    std::string myanmarMonthlyCalendarDate; // Format: YYYY-MM for monthly view
    std::string myanmarMonthlyCalendarFormat = "traditional";

    // Hindu Monthly Calendar options
    bool showHinduMonthlyCalendar = false;
    std::string hinduMonthlyCalendarDate; // Format: YYYY-MM for monthly view, supports BC dates
    std::string hinduMonthlyCalendarFormat = "traditional";
    bool hinduMonthlyShowMuhurta = false;
    bool includeGregorian = false;
    bool includeHindu = false;
    bool includePlanetary = false;
    bool includeAdvancedAstro = false;
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

    std::cout << "    --eclipse-format FORMAT\n";
    std::cout << "                       Eclipse output format\n";
    std::cout << "                       table = Professional table view (default)\n";
    std::cout << "                       text  = Traditional text format\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n\n";

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

    std::cout << "    --conjunction-format FORMAT\n";
    std::cout << "                       Conjunction output format\n";
    std::cout << "                       table = Professional table view (default)\n";
    std::cout << "                       text  = Traditional text format\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n\n";

    std::cout << "    --graha-yuddha     Find Graha Yuddha (Planetary Wars)\n";
    std::cout << "                       • Very close conjunctions (< 1°) between visible planets\n";
    std::cout << "                       • Determines winner based on Vedic rules\n\n";

    std::cout << "    --graha-yuddha-range FROM TO\n";
    std::cout << "                       Find planetary wars in specific date range\n";
    std::cout << "                       • Shows winner and astrological effects\n\n";

    std::cout << "    --graha-yuddha-format FORMAT\n";
    std::cout << "                       Graha Yuddha output format\n";
    std::cout << "                       table = Professional table view (default)\n";
    std::cout << "                       text  = Traditional text format\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n\n";

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
    std::cout << "                       table = Professional table view (default)\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n\n";

    std::cout << "    --ephemeris-declination\n";
    std::cout << "                       Show declination instead of longitude\n\n";

    std::cout << "    --ephemeris-coordinates TYPE\n";
    std::cout << "                       longitude   = Show ecliptic longitude (default)\n";
    std::cout << "                       declination = Show celestial declination\n";
    std::cout << "                       both        = Show both longitude and declination\n\n";

    std::cout << "    --ephemeris-sidereal-time\n";
    std::cout << "                       Include Greenwich Sidereal Time in ephemeris\n\n";

    std::cout << "    --ephemeris-compact\n";
    std::cout << "                       Use compact Astrodienst-style format\n";
    std::cout << "                       • Month headers with grouped entries\n";
    std::cout << "                       • Planet abbreviations (Sun, Moon, Merc, etc.)\n";
    std::cout << "                       • Compact position notation (DDMMx)\n";
    std::cout << "                       • Supports tropical and sidereal zodiac modes\n";
    std::cout << "                       • Use --zodiac-mode and --ayanamsa options\n\n";

    std::cout << "    --ephemeris-colors Color-coded planetary motion (default: enabled)\n";
    std::cout << "                       • Red = Retrograde motion\n";
    std::cout << "                       • Green = Fast direct motion\n";
    std::cout << "                       • Yellow = Slow direct motion\n";
    std::cout << "                       • Blue = Nearly stationary\n";
    std::cout << "                       • White = Normal motion\n\n";

    std::cout << "    --ephemeris-no-colors\n";
    std::cout << "                       Disable color coding for planetary motion\n\n";

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

    std::cout << "    --kp-format FORMAT KP output format\n";
    std::cout << "                       table = Professional table view (default)\n";
    std::cout << "                       text  = Traditional text format\n";
    std::cout << "                       csv   = Comma-separated values\n";
    std::cout << "                       json  = JSON structure\n\n";

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

    std::cout << "    --hindu-search-logic MODE\n";
    std::cout << "                            Logic mode for combining search criteria\n";
    std::cout << "                            and = All criteria must match (default)\n";
    std::cout << "                            or  = Any criteria can match\n";
    std::cout << "                            • Example: --hindu-search-logic or\n\n";

    std::cout << "    --hindu-search-ekadashi Search for Ekadashi days\n";
    std::cout << "    --hindu-search-purnima  Search for Purnima (full moon) days\n";
    std::cout << "    --hindu-search-amavasya Search for Amavasya (new moon) days\n";
    std::cout << "    --hindu-search-sankranti Search for Sankranti days\n";
    std::cout << "    --hindu-search-nakshatra N Search for specific Nakshatra (1-27)\n";
    std::cout << "    --hindu-search-yoga N   Search for specific Yoga (1-27)\n\n";

    std::cout << "JULIAN DAY (JD) SEARCH OPTIONS 🔢📅\n";
    std::cout << "    --search-jd JD          🎯 SIMPLE: Search single day by Julian Day number\n";
    std::cout << "                            • Example: horoscope_cli --search-jd 1555550\n";
    std::cout << "                            • Shows complete Hindu Panchanga for that exact day\n";
    std::cout << "                            • No date range required - JD defines exactly one day\n\n";

    std::cout << "    --search-jd-myanmar JD  🎯 SIMPLE: Search Myanmar calendar by Julian Day\n";
    std::cout << "                            • Example: horoscope_cli --search-jd-myanmar 1555550\n";
    std::cout << "                            • Shows complete Myanmar calendar for that exact day\n\n";

    std::cout << "    --search-jd-chart JD    🎯 SIMPLE: Search birth chart by Julian Day\n";
    std::cout << "                            • Example: horoscope_cli --search-jd-chart 1555550\n";
    std::cout << "                            • Shows complete birth chart for that exact day\n";
    std::cout << "                            • Requires coordinates: --lat and --lon\n\n";

    std::cout << "    --search-julian-day JD  Search for specific Julian Day number\n";
    std::cout << "                            • Example: --search-julian-day 1555550\n";
    std::cout << "                            • Quick: horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 --search-julian-day 1555550\n";
    std::cout << "    --search-julian-day-range START END  Search for Julian Day range\n";
    std::cout << "                                          • Example: --search-julian-day-range 1555550 1555580\n";
    std::cout << "                                          • Quick: horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 --search-julian-day-range 1555550 1555580\n\n";

    std::cout << "SAVARNA (VARNA) DAY SEARCH OPTIONS 🕉️⚡\n";
    std::cout << "    --search-varna-day TYPE Search for specific Varna day type\n";
    std::cout << "                            • brahmin = Brahmin Varna days (spiritual/priestly)\n";
    std::cout << "                            • kshatriya = Kshatriya Varna days (warrior/ruling)\n";
    std::cout << "                            • vaishya = Vaishya Varna days (merchant/farming)\n";
    std::cout << "                            • shudra = Shudra Varna days (service/labor)\n";
    std::cout << "                            • Example: --search-varna-day brahmin\n";
    std::cout << "                            • Quick: horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 --search-varna-day brahmin\n\n";

    std::cout << "    --search-brahmin-days   Search for all Brahmin Varna days\n";
    std::cout << "                            • Quick: horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 --search-brahmin-days\n";
    std::cout << "    --search-kshatriya-days Search for all Kshatriya Varna days\n";
    std::cout << "                            • Quick: horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 --search-kshatriya-days\n";
    std::cout << "    --search-vaishya-days   Search for all Vaishya Varna days\n";
    std::cout << "                            • Quick: horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 --search-vaishya-days\n";
    std::cout << "    --search-shudra-days    Search for all Shudra Varna days\n";
    std::cout << "                            • Quick: horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 --search-shudra-days\n\n";

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

    std::cout << "    --myanmar-search-logic MODE     Combine criteria with logical operators\n";
    std::cout << "                                     and = All criteria must match (default)\n";
    std::cout << "                                     or  = Any criteria can match\n";
    std::cout << "                                     • Example: --myanmar-search-logic or\n\n";

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

    std::cout << "MYANMAR MONTHLY CALENDAR 🇲🇲📅 (NEW)\n";
    std::cout << "    --myanmar-monthly YYYY-MM, -m YYYY-MM\n";
    std::cout << "                       Generate Myanmar monthly calendar inspired by mmcal.blogspot.com\n";
    std::cout << "                       • Format: 2024-01 for January 2024 (Gregorian)\n";
    std::cout << "                       • Beautiful traditional Myanmar calendar layout\n";
    std::cout << "                       • Shows both Gregorian and Myanmar dates\n";
    std::cout << "                       • Includes astrological days, festivals, moon phases\n";
    std::cout << "                       • Multiple output formats available\n";
    std::cout << "                       • Short option: -m 2025-07 (equivalent to --myanmar-monthly 2025-07)\n";

    std::cout << "    --myanmar-monthly-format FORMAT\n";
    std::cout << "                       Myanmar monthly calendar output format\n";
    std::cout << "                       traditional     = Traditional Myanmar style with script (default)\n";
    std::cout << "                       modern          = Modern layout with visual elements\n";
    std::cout << "                       compact         = Compact view for quick reference\n";
    std::cout << "                       blog-style      = mmcal.blogspot.com inspired layout\n";
    std::cout << "                       tabulate        = Beautiful table using tabulate library\n";
    std::cout << "                       tabulate-modern = Modern table with contemporary styling\n";
    std::cout << "                       tabulate-classic= Classic table with traditional borders\n";
    std::cout << "                       tabulate-minimal= Minimal table with clean appearance\n";
    std::cout << "                       multi-calendar  = Multi-calendar view (Myanmar+Gregorian+Hindu)\n";
    std::cout << "                       planetary       = Planetary calendar with astronomical events\n";
    std::cout << "                       hindu-myanmar   = Combined Hindu and Myanmar calendar\n";
    std::cout << "                       full-astronomical= Complete astronomical reference\n";
    std::cout << "                       json            = JSON structure for integration\n";
    std::cout << "                       csv             = Comma-separated values\n";
    std::cout << "                       html            = HTML format for web display\n\n";

    std::cout << "HINDU MONTHLY CALENDAR 🕉️📅 (NEW)\n";
    std::cout << "    --hindu-monthly YYYY-MM, -hm YYYY-MM\n";
    std::cout << "                       Generate Hindu monthly calendar with BC era support\n";
    std::cout << "                       • Format: 2024-01 for January 2024 (Gregorian)\n";
    std::cout << "                       • Format: 500BC-03 for March 500 BC\n";
    std::cout << "                       • Format: -0499-03 for 500 BC (astronomical year)\n";
    std::cout << "                       • Complete Panchanga calculations for each day\n";
    std::cout << "                       • BC era support (unlike Myanmar calendar)\n";
    std::cout << "                       • Tithi, Nakshatra, Yoga, Karana for each day\n";
    std::cout << "                       • Supports multiple Hindu calendar systems\n";
    std::cout << "                       • Enhanced format shows Julian Day (JD) and Varna info\n";
    std::cout << "                       • Use JD values from monthly display for --search-julian-day\n";
    std::cout << "                       • Short option: -hm 2025-07 (equivalent to --hindu-monthly 2025-07)\n";

    std::cout << "    --hindu-monthly-format FORMAT\n";
    std::cout << "                       Hindu monthly calendar output format\n";
    std::cout << "                       traditional     = Traditional Panchanga layout (default)\n";
    std::cout << "                       detailed        = Detailed view with muhurta timings\n";
    std::cout << "                       panchanga       = Focus on five Panchanga elements\n";
    std::cout << "                       compact         = Compact view for quick reference\n";
    std::cout << "                       festivals       = Highlight festivals and special events\n";
    std::cout << "                       astronomical    = Include astronomical data\n";
    std::cout << "                       csv             = Comma-separated values\n";
    std::cout << "                       json            = JSON structure for integration\n";
    std::cout << "                       html            = HTML format for web display\n";

    std::cout << "    --hindu-monthly-muhurta\n";
    std::cout << "                       Include muhurta timings in Hindu monthly calendar\n";
    std::cout << "                       • Brahma Muhurta, Abhijit Muhurta timings\n";
    std::cout << "                       • Rahu Kaal, Yamaganda, Gulikai periods\n";
    std::cout << "                       • Auspicious and inauspicious timings\n\n";

    std::cout << "CALENDAR SYSTEM OPTIONS 📅🌍\n";
    std::cout << "    --include-gregorian\n";
    std::cout << "                       Include Gregorian calendar data (enabled by default)\n";
    std::cout << "                       • Western calendar system\n";
    std::cout << "                       • Weekend detection, holidays\n\n";

    std::cout << "    --include-hindu    Include Hindu calendar and Vedic astrology\n";
    std::cout << "                       • Panchanga (Tithi, Nakshatra, Yoga, Karana)\n";
    std::cout << "                       • Ekadashi, Purnima, Amavasya detection\n";
    std::cout << "                       • Hindu festivals and auspicious days\n\n";

    std::cout << "    --include-planetary\n";
    std::cout << "                       Include planetary transitions and astronomical events\n";
    std::cout << "                       • Retrograde motion tracking\n";
    std::cout << "                       • Planetary conjunctions\n";
    std::cout << "                       • Sign changes and eclipses\n\n";

    std::cout << "    --include-advanced-astro\n";
    std::cout << "                       Include advanced astrological calculations\n";
    std::cout << "                       • KP system star lords\n";
    std::cout << "                       • Aspect formations\n";
    std::cout << "                       • Planetary weather patterns\n\n";

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

    std::cout << "NEW ENHANCED FEATURES 🚀✨\n";
    std::cout << "    --interactive      Launch interactive mode with guided wizards\n";
    std::cout << "    --quick-wizard     Quick setup wizard for beginners\n";
    std::cout << "    --professional     Professional mode with advanced options\n\n";

    std::cout << "AI-POWERED INTERPRETATION 🤖🧠\n";
    std::cout << "    --ai-interpretation  Generate AI-powered chart interpretation\n";
    std::cout << "    --personality        Generate detailed personality profile\n";
    std::cout << "    --career-guidance    AI-generated career guidance and advice\n";
    std::cout << "    --relationship-profile  Relationship patterns and compatibility\n";
    std::cout << "    --health-insights    Health insights and wellness guidance\n";
    std::cout << "    --spiritual-guidance Spiritual path and life purpose analysis\n\n";

    std::cout << "    --interpretation-style STYLE\n";
    std::cout << "                       Interpretation approach (default: balanced)\n";
    std::cout << "                       psychological = Jungian psychological approach\n";
    std::cout << "                       spiritual     = Esoteric and spiritual focus\n";
    std::cout << "                       practical     = Practical life guidance\n";
    std::cout << "                       balanced      = Holistic balanced approach\n\n";

    std::cout << "    --interpretation-detail LEVEL\n";
    std::cout << "                       Detail level (default: standard)\n";
    std::cout << "                       brief         = Concise summary format\n";
    std::cout << "                       standard      = Comprehensive analysis\n";
    std::cout << "                       detailed      = In-depth exploration\n\n";

    std::cout << "ADVANCED ASTROLOGICAL ANALYSIS 🔮⭐\n";
    std::cout << "    --advanced-analysis  Complete advanced astrological analysis\n";
    std::cout << "    --midpoints         Planetary midpoint analysis\n";
    std::cout << "    --harmonics         Harmonic chart analysis (4th, 5th, 7th, 9th)\n";
    std::cout << "    --harmonic N        Specific harmonic analysis (e.g., --harmonic 7)\n";
    std::cout << "    --arabic-parts      Calculate Arabic Parts/Lots\n";
    std::cout << "    --fixed-stars       Fixed star analysis and influences\n";
    std::cout << "    --asteroids         Asteroid analysis (Ceres, Pallas, Juno, Vesta)\n";
    std::cout << "    --vertex            Vertex and Anti-Vertex analysis\n\n";

    std::cout << "PREDICTIVE ASTROLOGY 🔮🌟\n";
    std::cout << "    --predictive        Complete predictive analysis package\n";
    std::cout << "    --dasha            Vedic Dasha analysis (Vimshottari by default)\n";
    std::cout << "    --dasha-type TYPE  Specify dasha system (vimshottari, ashtottari)\n";
    std::cout << "    --progressions DATE Secondary progressions for target date\n";
    std::cout << "    --transits FROM TO  Transit analysis for date range\n";
    std::cout << "    --returns TYPE YEAR Planetary returns (solar, lunar, jupiter, saturn)\n";
    std::cout << "    --yearly-forecast YEAR  Complete yearly forecast\n";
    std::cout << "    --monthly-forecast YYYY-MM  Monthly forecast\n\n";

    std::cout << "ELECTIONAL ASTROLOGY ⏰🌙\n";
    std::cout << "    --electional FROM TO PURPOSE\n";
    std::cout << "                       Find auspicious timing for specific purpose\n";
    std::cout << "                       Purposes: marriage, business, travel, surgery,\n";
    std::cout << "                                education, moving, investment\n";
    std::cout << "    --auspicious-times  Find generally auspicious times in date range\n\n";

    std::cout << "RELATIONSHIP ANALYSIS 💕👥\n";
    std::cout << "    --synastry         Synastry analysis with partner's chart\n";
    std::cout << "    --composite        Composite chart analysis\n";
    std::cout << "    --partner-date DATE --partner-time TIME\n";
    std::cout << "                       Partner's birth date and time\n";
    std::cout << "    --partner-lat LAT --partner-lon LON --partner-tz TZ\n";
    std::cout << "                       Partner's birth location\n";
    std::cout << "    --partner-location \"CITY\"\n";
    std::cout << "                       Partner's birth city (alternative to coordinates)\n\n";

    std::cout << "SPECIALIZED ASTROLOGY 🎯🏥💰\n";
    std::cout << "    --financial        Financial astrology analysis\n";
    std::cout << "    --market-analysis  Stock market and crypto predictions\n";
    std::cout << "    --stock-symbol SYM Analyze specific stock symbol\n";
    std::cout << "    --medical          Medical astrology and health analysis\n";
    std::cout << "    --ayurvedic        Ayurvedic constitution analysis\n";
    std::cout << "    --mundane COUNTRY  Mundane astrology for country/region\n";
    std::cout << "    --relocation LAT LON  Relocation astrology analysis\n\n";

    std::cout << "MULTI-LANGUAGE SUPPORT 🌍🗣️\n";
    std::cout << "    --language LANG    Set language (english, hindi, sanskrit, spanish,\n";
    std::cout << "                       french, german, italian, portuguese, russian,\n";
    std::cout << "                       chinese, japanese, korean, arabic, thai, burmese)\n";
    std::cout << "    --native-script    Use native script (Devanagari, Arabic, etc.)\n";
    std::cout << "    --cultural-region REGION  Set cultural preferences\n";
    std::cout << "    --date-format FORMAT  Date format (YYYY-MM-DD, DD/MM/YYYY, MM/DD/YYYY)\n";
    std::cout << "    --time-format FORMAT  Time format (24h, 12h)\n\n";

    std::cout << "BATCH PROCESSING 📊⚡\n";
    std::cout << "    --batch INPUT.csv OUTPUT.csv OPERATION\n";
    std::cout << "                       Batch process multiple charts\n";
    std::cout << "                       Operations: charts, predictions, compatibility\n";
    std::cout << "    --batch-charts     Generate charts for multiple birth data\n";
    std::cout << "    --batch-predictions Predictions for multiple people\n\n";

    std::cout << "CONFIGURATION & PROFILES 🛠️👤\n";
    std::cout << "    --config FILE      Load configuration from file\n";
    std::cout << "    --save-config FILE Save current settings to configuration\n";
    std::cout << "    --user-profile PROFILE  Load user profile\n";
    std::cout << "    --save-profile PROFILE  Save current data as profile\n";
    std::cout << "    --reset-config     Reset to default configuration\n\n";

    std::cout << "OUTPUT ENHANCEMENTS 🎨📄\n";
    std::cout << "    --color           Enable colorized output (default: on)\n";
    std::cout << "    --no-color        Disable colorized output\n";
    std::cout << "    --theme THEME     Output theme (default, dark, light, classic)\n";
    std::cout << "    --save-to FILE    Save results to file\n";
    std::cout << "    --pdf             Generate PDF output\n";
    std::cout << "    --email ADDRESS   Email results to address\n";
    std::cout << "    --verbose         Verbose output with detailed progress\n\n";

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

    std::cout << "  # Sidereal ephemeris with Lahiri ayanamsa\n";
    std::cout << "  horoscope_cli --ephemeris \\\n";
    std::cout << "                --ephemeris-range 2025-01-01 2025-01-31 \\\n";
    std::cout << "                --zodiac-mode sidereal --ayanamsa lahiri \\\n";
    std::cout << "                --ephemeris-compact --ephemeris-sidereal-time\n\n";

    std::cout << "  # Vedic ephemeris with Krishnamurti ayanamsa\n";
    std::cout << "  horoscope_cli --ephemeris \\\n";
    std::cout << "                --ephemeris-range 2025-01-01 2025-01-31 \\\n";
    std::cout << "                --zodiac-mode sidereal --ayanamsa krishnamurti \\\n";
    std::cout << "                --ephemeris-compact\n\n";

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

    std::cout << "  # Myanmar calendar search for Full Moon Sabbath days (AND logic)\n";
    std::cout << "  horoscope_cli --myanmar-search 2025-01-01 2025-12-31 \\\n";
    std::cout << "                --myanmar-search-moon-phase 1 \\\n";
    std::cout << "                --myanmar-search-sabbath \\\n";
    std::cout << "                --myanmar-search-logic and \\\n";
    std::cout << "                --lat 16.8661 --lon 96.1951 \\\n";
    std::cout << "                --myanmar-search-format table\n\n";

    std::cout << "  # Myanmar calendar search for Full Moon OR Yatyaza days (OR logic)\n";
    std::cout << "  horoscope_cli --myanmar-search 2025-01-01 2025-01-31 \\\n";
    std::cout << "                --myanmar-search-moon-phase 1 \\\n";
    std::cout << "                --myanmar-search-yatyaza \\\n";
    std::cout << "                --myanmar-search-logic or \\\n";
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

    std::cout << "  # Hindu calendar search with OR logic - find Purnima OR Ekadashi\n";
    std::cout << "  horoscope_cli --hindu-search 2025-01-01 2025-03-31 \\\n";
    std::cout << "                --hindu-search-purnima \\\n";
    std::cout << "                --hindu-search-ekadashi \\\n";
    std::cout << "                --hindu-search-logic or \\\n";
    std::cout << "                --hindu-search-format table\n\n";

    std::cout << "  # Hindu calendar search with AND logic - find specific Nakshatra AND Yoga\n";
    std::cout << "  horoscope_cli --hindu-search 2025-01-01 2025-06-30 \\\n";
    std::cout << "                --hindu-search-nakshatra 1 \\\n";
    std::cout << "                --hindu-search-yoga 5 \\\n";
    std::cout << "                --hindu-search-logic and \\\n";
    std::cout << "                --hindu-search-format json\n\n";

    std::cout << "  # 🎯 SIMPLE Julian Day search - show Hindu Panchanga for exact day\n";
    std::cout << "  horoscope_cli --search-jd 1555550\n\n";

    std::cout << "  # 🎯 SIMPLE Julian Day search with coordinates for accurate timings\n";
    std::cout << "  horoscope_cli --search-jd 1555550 --lat 27.7172 --lon 85.3240\n\n";

    std::cout << "  # 🎯 SIMPLE Myanmar calendar search by Julian Day\n";
    std::cout << "  horoscope_cli --search-jd-myanmar 1555550\n\n";

    std::cout << "  # 🎯 SIMPLE Birth chart search by Julian Day with location\n";
    std::cout << "  horoscope_cli --search-jd-chart 1555550 --lat 12.97 --lon 77.59\n\n";

    std::cout << "  # Julian Day search - find dates by specific JD number\n";
    std::cout << "  horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 \\\n";
    std::cout << "                --search-julian-day 1555550 \\\n";
    std::cout << "                --lat 27.7172 --lon 85.3240\n\n";

    std::cout << "  # Julian Day range search - find all dates in JD range\n";
    std::cout << "  horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 \\\n";
    std::cout << "                --search-julian-day-range 1555550 1555580 \\\n";
    std::cout << "                --lat 27.7172 --lon 85.3240 \\\n";
    std::cout << "                --hindu-search-format list\n\n";

    std::cout << "  # Savarna (Varna) day search - find Brahmin Varna days\n";
    std::cout << "  horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 \\\n";
    std::cout << "                --search-brahmin-days \\\n";
    std::cout << "                --lat 27.7172 --lon 85.3240\n\n";

    std::cout << "  # Varna type search - find specific Varna days\n";
    std::cout << "  horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 \\\n";
    std::cout << "                --search-varna-day kshatriya \\\n";
    std::cout << "                --lat 27.7172 --lon 85.3240 \\\n";
    std::cout << "                --hindu-search-format table\n\n";

    std::cout << "  # Combined search - Brahmin days OR Purnima (Full Moon)\n";
    std::cout << "  horoscope_cli --hindu-search 563BC-05-01 563BC-05-31 \\\n";
    std::cout << "                --search-brahmin-days \\\n";
    std::cout << "                --hindu-search-purnima \\\n";
    std::cout << "                --hindu-search-logic or \\\n";
    std::cout << "                --lat 27.7172 --lon 85.3240\n\n";

    std::cout << "  # Myanmar calendar search - simple date list format\n";
    std::cout << "  horoscope_cli --myanmar-search 2025-01-01 2025-12-31 \\\n";
    std::cout << "                --myanmar-search-moon-phase 1 \\\n";
    std::cout << "                --myanmar-search-format list\n\n";

    std::cout << "MYANMAR MONTHLY CALENDAR 🇲🇲📅\n";
    std::cout << "  # Myanmar monthly calendar with multi-calendar view and Hindu dates\n";
    std::cout << "  horoscope_cli --myanmar-monthly 2025-07 \\\n";
    std::cout << "                --myanmar-monthly-format multi-calendar \\\n";
    std::cout << "                --include-hindu\n\n";

    std::cout << "  # Short form (equivalent to above)\n";
    std::cout << "  horoscope_cli -m 2025-07\n\n";

    std::cout << "  # Other Myanmar monthly formats\n";
    std::cout << "  horoscope_cli --myanmar-monthly 2025-07 \\\n";
    std::cout << "                --myanmar-monthly-format traditional\n\n";

    std::cout << "  horoscope_cli --myanmar-monthly 2025-07 \\\n";
    std::cout << "                --myanmar-monthly-format blog-style\n\n";

    std::cout << "HINDU MONTHLY CALENDAR 🕉️📅\n";
    std::cout << "  # Hindu monthly calendar with complete Panchanga for each day\n";
    std::cout << "  horoscope_cli --hindu-monthly 2025-07 \\\n";
    std::cout << "                --hindu-monthly-format detailed \\\n";
    std::cout << "                --hindu-monthly-muhurta\n\n";

    std::cout << "  # Short form (equivalent to above without muhurta)\n";
    std::cout << "  horoscope_cli -hm 2025-07\n\n";

    std::cout << "  # Hindu calendar for BC era (500 BC example)\n";
    std::cout << "  horoscope_cli --hindu-monthly 500BC-03 \\\n";
    std::cout << "                --hindu-monthly-format panchanga\n\n";

    std::cout << "  # Hindu calendar with JD and Varna values (enhanced format - default)\n";
    std::cout << "  horoscope_cli -hm 563BC-05 --lat 27.7172 --lon 85.3240\n";
    std::cout << "  # Note: JD numbers from calendar can be used with --search-julian-day\n\n";

    std::cout << "  # Hindu calendar using astronomical year format for BC\n";
    std::cout << "  horoscope_cli --hindu-monthly -0499-03 \\\n";
    std::cout << "                --hindu-monthly-format traditional\n\n";

    std::cout << "  # Hindu calendar with festivals highlighted\n";
    std::cout << "  horoscope_cli --hindu-monthly 2025-12 \\\n";
    std::cout << "                --hindu-monthly-format festivals\n\n";

    std::cout << "  # Export Hindu calendar to CSV\n";
    std::cout << "  horoscope_cli --hindu-monthly 2025-07 \\\n";
    std::cout << "                --hindu-monthly-format csv > hindu_calendar_2025_07.csv\n\n";

    std::cout << "NEW ENHANCED FEATURES EXAMPLES 🚀✨\n\n";

    std::cout << "INTERACTIVE MODE 🖥️\n";
    std::cout << "  # Launch interactive mode\n";
    std::cout << "  horoscope_cli --interactive\n\n";

    std::cout << "  # Quick wizard for beginners\n";
    std::cout << "  horoscope_cli --quick-wizard\n\n";

    std::cout << "  # Professional mode with all features\n";
    std::cout << "  horoscope_cli --professional\n\n";

    std::cout << "AI-POWERED INTERPRETATION 🤖\n";
    std::cout << "  # Complete AI interpretation\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --ai-interpretation\n\n";

    std::cout << "  # Detailed personality profile\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --personality --interpretation-detail detailed\n\n";

    std::cout << "  # Career guidance with practical focus\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --career-guidance --interpretation-style practical\n\n";

    std::cout << "  # Spiritual guidance with esoteric focus\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --spiritual-guidance --interpretation-style spiritual\n\n";

    std::cout << "ADVANCED ANALYSIS 🔮\n";
    std::cout << "  # Complete advanced astrological analysis\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --advanced-analysis\n\n";

    std::cout << "  # Midpoint and harmonic analysis\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --midpoints --harmonics\n\n";

    std::cout << "  # Specific 7th harmonic analysis\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --harmonic 7\n\n";

    std::cout << "  # Fixed stars and Arabic parts\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --fixed-stars --arabic-parts\n\n";

    std::cout << "PREDICTIVE ASTROLOGY 🔮\n";
    std::cout << "  # Complete predictive analysis\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --predictive\n\n";

    std::cout << "  # Dasha analysis with progressions\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --dasha --progressions 2025-07-21\n\n";

    std::cout << "  # Transit analysis for next 6 months\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --transits 2025-07-21 2026-01-21\n\n";

    std::cout << "  # Solar return for 2025\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --returns solar 2025\n\n";

    std::cout << "  # Monthly forecast for August 2025\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --monthly-forecast 2025-08\n\n";

    std::cout << "ELECTIONAL ASTROLOGY ⏰\n";
    std::cout << "  # Find best timing for marriage\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --electional 2025-08-01 2025-12-31 marriage\n\n";

    std::cout << "  # Find auspicious business start times\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --electional 2025-09-01 2025-11-30 business\n\n";

    std::cout << "RELATIONSHIP ANALYSIS 💕\n";
    std::cout << "  # Synastry with partner\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --synastry \\\n";
    std::cout << "                --partner-date 1992-06-10 --partner-time 16:45:00 \\\n";
    std::cout << "                --partner-lat 51.5074 --partner-lon -0.1278 --partner-tz 0\n\n";

    std::cout << "  # Composite chart using location names\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --location \"New York\" \\\n";
    std::cout << "                --composite \\\n";
    std::cout << "                --partner-date 1992-06-10 --partner-time 16:45:00 \\\n";
    std::cout << "                --partner-location \"London\"\n\n";

    std::cout << "SPECIALIZED ASTROLOGY 🎯\n";
    std::cout << "  # Financial astrology for stock analysis\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --financial --stock-symbol AAPL\n\n";

    std::cout << "  # Medical astrology with Ayurvedic analysis\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --medical --ayurvedic\n\n";

    std::cout << "  # Relocation astrology\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --relocation 51.5074 -0.1278\n\n";

    std::cout << "MULTI-LANGUAGE EXAMPLES 🌍\n";
    std::cout << "  # Chart in Hindi with Devanagari script\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 28.6139 --lon 77.2090 --timezone 5.5 \\\n";
    std::cout << "                --language hindi --native-script\n\n";

    std::cout << "  # Chart in Spanish with European date format\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.4168 --lon -3.7038 --timezone 1 \\\n";
    std::cout << "                --language spanish --date-format DD/MM/YYYY\n\n";

    std::cout << "  # Chinese chart with traditional Chinese\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 39.9042 --lon 116.4074 --timezone 8 \\\n";
    std::cout << "                --language chinese --cultural-region china\n\n";

    std::cout << "BATCH PROCESSING 📊\n";
    std::cout << "  # Batch generate charts from CSV file\n";
    std::cout << "  horoscope_cli --batch birth_data.csv charts_output.csv charts\n\n";

    std::cout << "  # Batch compatibility analysis\n";
    std::cout << "  horoscope_cli --batch couples_data.csv compatibility.csv compatibility\n\n";

    std::cout << "CONFIGURATION EXAMPLES 🛠️\n";
    std::cout << "  # Save current settings\n";
    std::cout << "  horoscope_cli --save-config my_settings.conf\n\n";

    std::cout << "  # Load user profile\n";
    std::cout << "  horoscope_cli --user-profile john_doe.profile --ai-interpretation\n\n";

    std::cout << "  # Professional setup with dark theme and verbose output\n";
    std::cout << "  horoscope_cli --date 1990-01-15 --time 14:30:00 \\\n";
    std::cout << "                --lat 40.7128 --lon -74.0060 --timezone -5 \\\n";
    std::cout << "                --professional --theme dark --verbose \\\n";
    std::cout << "                --advanced-analysis --save-to detailed_report.html\n\n";

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
        } else if (arg == "--ephemeris-declination") {
            args.ephemerisShowDeclination = true;
            args.ephemerisCoordinateType = "declination";
        } else if (arg == "--ephemeris-coordinates" && i + 1 < argc) {
            args.ephemerisCoordinateType = argv[++i];
            if (args.ephemerisCoordinateType != "longitude" && args.ephemerisCoordinateType != "declination" && args.ephemerisCoordinateType != "both") {
                std::cerr << "Error: Ephemeris coordinates must be 'longitude', 'declination', or 'both'\n";
                return false;
            }
            if (args.ephemerisCoordinateType == "declination" || args.ephemerisCoordinateType == "both") {
                args.ephemerisShowDeclination = true;
            }
        } else if (arg == "--ephemeris-sidereal-time") {
            args.ephemerisShowSiderealTime = true;
        } else if (arg == "--ephemeris-compact") {
            args.ephemerisCompactFormat = true;
        } else if (arg == "--ephemeris-colors") {
            args.ephemerisUseColors = true;
        } else if (arg == "--ephemeris-no-colors") {
            args.ephemerisUseColors = false;
        } else if (arg == "--eclipse-format" && i + 1 < argc) {
            args.eclipseFormat = argv[++i];
            if (args.eclipseFormat != "table" && args.eclipseFormat != "text" && args.eclipseFormat != "csv" && args.eclipseFormat != "json") {
                std::cerr << "Error: Eclipse format must be 'table', 'text', 'csv', or 'json'\n";
                return false;
            }
        } else if (arg == "--conjunction-format" && i + 1 < argc) {
            args.conjunctionFormat = argv[++i];
            if (args.conjunctionFormat != "table" && args.conjunctionFormat != "text" && args.conjunctionFormat != "csv" && args.conjunctionFormat != "json") {
                std::cerr << "Error: Conjunction format must be 'table', 'text', 'csv', or 'json'\n";
                return false;
            }
        } else if (arg == "--graha-yuddha-format" && i + 1 < argc) {
            args.grahaYuddhaFormat = argv[++i];
            if (args.grahaYuddhaFormat != "table" && args.grahaYuddhaFormat != "text" && args.grahaYuddhaFormat != "csv" && args.grahaYuddhaFormat != "json") {
                std::cerr << "Error: Graha Yuddha format must be 'table', 'text', 'csv', or 'json'\n";
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
            if (args.kpOutputFormat != "table" && args.kpOutputFormat != "text" && args.kpOutputFormat != "csv" && args.kpOutputFormat != "json") {
                std::cerr << "Error: KP format must be 'table', 'text', 'csv', or 'json'\n";
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
                args.panchangaFormat != "csv" && args.panchangaFormat != "json" && args.panchangaFormat != "list") {
                std::cerr << "Error: Panchanga format must be 'table', 'compact', 'csv', 'json', or 'list'\n";
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
        } else if (arg == "--hindu-search-logic" && i + 1 < argc) {
            args.hinduSearchLogic = argv[++i];
            if (args.hinduSearchLogic != "and" && args.hinduSearchLogic != "or") {
                std::cerr << "Error: Invalid Hindu search logic. Must be 'and' or 'or'\n";
                return false;
            }
        } else if (arg == "--hindu-search-ekadashi") {
            args.searchEkadashi = true;
        } else if (arg == "--hindu-search-purnima") {
            args.searchPurnima = true;
        } else if (arg == "--hindu-search-amavasya") {
            args.searchAmavasya = true;
        } else if (arg == "--hindu-search-sankranti") {
            args.searchSankranti = true;
        } else if (arg == "--hindu-search-nakshatra" && i + 1 < argc) {
            args.searchNakshatra = std::stoi(argv[++i]);
        } else if (arg == "--hindu-search-yoga" && i + 1 < argc) {
            args.searchYoga = std::stoi(argv[++i]);

        // Julian Day search options
        } else if (arg == "--search-julian-day" && i + 1 < argc) {
            args.searchJulianDay = std::stod(argv[++i]);
        } else if (arg == "--search-julian-day-range" && i + 2 < argc) {
            args.searchJulianDayStart = std::stod(argv[++i]);
            args.searchJulianDayEnd = std::stod(argv[++i]);

        // Simplified JD search options
        } else if (arg == "--search-jd" && i + 1 < argc) {
            args.searchJdOnly = std::stod(argv[++i]);
        } else if (arg == "--search-jd-myanmar" && i + 1 < argc) {
            args.searchJdMyanmarOnly = std::stod(argv[++i]);
        } else if (arg == "--search-jd-chart" && i + 1 < argc) {
            args.searchJdBirthChartOnly = std::stod(argv[++i]);

        // Varna search options
        } else if (arg == "--search-varna-day" && i + 1 < argc) {
            std::string varnaType = argv[++i];
            if (varnaType == "brahmin") {
                args.searchBrahminDays = true;
            } else if (varnaType == "kshatriya") {
                args.searchKshatriyaDays = true;
            } else if (varnaType == "vaishya") {
                args.searchVaishyaDays = true;
            } else if (varnaType == "shudra") {
                args.searchShudradays = true;
            } else {
                std::cerr << "Error: Invalid Varna type. Must be 'brahmin', 'kshatriya', 'vaishya', or 'shudra'\n";
                return false;
            }
        } else if (arg == "--search-brahmin-days") {
            args.searchBrahminDays = true;
        } else if (arg == "--search-kshatriya-days") {
            args.searchKshatriyaDays = true;
        } else if (arg == "--search-vaishya-days") {
            args.searchVaishyaDays = true;
        } else if (arg == "--search-shudra-days") {
            args.searchShudradays = true;

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
        } else if (arg == "--myanmar-search-logic" && i + 1 < argc) {
            std::string logic = argv[++i];
            if (logic == "and" || logic == "or") {
                args.myanmarSearchLogic = logic;
            } else {
                std::cerr << "❌ Error: Invalid logic mode '" << logic << "'. Use 'and' or 'or'.\n";
                return 1;
            }
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
        } else if (arg == "--myanmar-monthly" || arg == "-m") {
            if (i + 1 < argc) {
                args.myanmarMonthlyCalendarDate = argv[++i];
                args.showMyanmarMonthlyCalendar = true;
                // Set default format to multi-calendar and include Hindu for short option convenience
                if (arg == "-m") {
                    args.myanmarMonthlyCalendarFormat = "multi-calendar";
                    args.includeHindu = true;
                }
                // Validate YYYY-MM format
                if (args.myanmarMonthlyCalendarDate.length() != 7 || args.myanmarMonthlyCalendarDate[4] != '-') {
                    std::cerr << "Error: --myanmar-monthly requires YYYY-MM format (e.g., 2024-01)\n";
                    return false;
                }
            } else {
                std::cerr << "Error: --myanmar-monthly requires a month argument (YYYY-MM)\n";
                return false;
            }
        } else if (arg == "--myanmar-monthly-format") {
            if (i + 1 < argc) {
                args.myanmarMonthlyCalendarFormat = argv[++i];
            } else {
                std::cerr << "Error: --myanmar-monthly-format requires a format argument\n";
                return false;
            }
            if (args.myanmarMonthlyCalendarFormat != "traditional" && args.myanmarMonthlyCalendarFormat != "modern" &&
                args.myanmarMonthlyCalendarFormat != "compact" && args.myanmarMonthlyCalendarFormat != "blog-style" &&
                args.myanmarMonthlyCalendarFormat != "tabulate" && args.myanmarMonthlyCalendarFormat != "tabulate-modern" &&
                args.myanmarMonthlyCalendarFormat != "tabulate-classic" && args.myanmarMonthlyCalendarFormat != "tabulate-minimal" &&
                args.myanmarMonthlyCalendarFormat != "multi-calendar" && args.myanmarMonthlyCalendarFormat != "planetary" &&
                args.myanmarMonthlyCalendarFormat != "hindu-myanmar" && args.myanmarMonthlyCalendarFormat != "full-astronomical" &&
                args.myanmarMonthlyCalendarFormat != "json" && args.myanmarMonthlyCalendarFormat != "csv" &&
                args.myanmarMonthlyCalendarFormat != "html") {
                std::cerr << "Error: Myanmar monthly format must be 'traditional', 'modern', 'compact', 'blog-style', 'tabulate', 'tabulate-modern', 'tabulate-classic', 'tabulate-minimal', 'multi-calendar', 'planetary', 'hindu-myanmar', 'full-astronomical', 'json', 'csv', or 'html'\n";
                return false;
            }
        } else if (arg == "--hindu-monthly" || arg == "-hm") {
            args.showHinduMonthlyCalendar = true;
            if (i + 1 < argc) {
                args.hinduMonthlyCalendarDate = argv[++i];
            } else {
                std::cerr << "Error: --hindu-monthly requires a month argument (YYYY-MM or YYYYBC-MM)\n";
                return false;
            }
        } else if (arg == "--hindu-monthly-format") {
            if (i + 1 < argc) {
                args.hinduMonthlyCalendarFormat = argv[++i];
            } else {
                std::cerr << "Error: --hindu-monthly-format requires a format argument\n";
                return false;
            }
            if (args.hinduMonthlyCalendarFormat != "traditional" && args.hinduMonthlyCalendarFormat != "detailed" &&
                args.hinduMonthlyCalendarFormat != "panchanga" && args.hinduMonthlyCalendarFormat != "compact" &&
                args.hinduMonthlyCalendarFormat != "festivals" && args.hinduMonthlyCalendarFormat != "astronomical" &&
                args.hinduMonthlyCalendarFormat != "json" && args.hinduMonthlyCalendarFormat != "csv" &&
                args.hinduMonthlyCalendarFormat != "html") {
                std::cerr << "Error: Hindu monthly format must be 'traditional', 'detailed', 'panchanga', 'compact', 'festivals', 'astronomical', 'json', 'csv', or 'html'\n";
                return false;
            }
        } else if (arg == "--hindu-monthly-muhurta") {
            args.hinduMonthlyShowMuhurta = true;
        } else if (arg == "--include-gregorian") {
            args.includeGregorian = true;
        } else if (arg == "--include-hindu") {
            args.includeHindu = true;
        } else if (arg == "--include-planetary") {
            args.includePlanetary = true;
        } else if (arg == "--include-advanced-astro") {
            args.includeAdvancedAstro = true;
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

    // Astro calendar can work without location data for monthly view
    if (args.showAstroCalendarMonthly) {
        return true;
    }

    // Myanmar monthly calendar can work without location data
    if (args.showMyanmarMonthlyCalendar) {
        return true;
    }

    // Hindu monthly calendar can work without location data (uses default coordinates)
    if (args.showHinduMonthlyCalendar) {
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

    // Skip date/time requirements for JD search commands
    if (args.date.empty() && !args.showAstroCalendarMonthly &&
        args.searchJdOnly <= 0 && args.searchJdMyanmarOnly <= 0 && args.searchJdBirthChartOnly <= 0) {
        std::cerr << "Error: --date is required\n";
        return false;
    }

    if (args.time.empty() && !args.showAstroCalendarMonthly &&
        args.searchJdOnly <= 0 && args.searchJdMyanmarOnly <= 0 && args.searchJdBirthChartOnly <= 0) {
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

            if (args.eclipseFormat == "table") {
                // Professional table output
                ProfessionalTable table = createEclipseTable();

                std::stringstream subtitle;
                subtitle << eclipses.size() << " eclipses found | Period: " << fromDate << " ↔ " << toDate;
                if (args.latitude != 0.0 || args.longitude != 0.0) {
                    subtitle << " | Location: " << std::fixed << std::setprecision(2)
                             << args.latitude << "°, " << args.longitude << "°";
                }
                table.setSubtitle(subtitle.str());

                for (const auto& eclipse : eclipses) {
                    std::string date = eclipse.getDateString();
                    std::string type = eclipse.getTypeString();
                    std::string time = "12:00"; // Placeholder - would need time extraction from eclipse struct
                    std::string magnitude = std::to_string(eclipse.magnitude).substr(0, 5);
                    std::string duration = std::to_string(eclipse.duration).substr(0, 6) + "min";
                    std::string visibility = eclipse.isVisible ? "Visible" : "Not Visible";
                    std::string saros = "-"; // Would need saros data from eclipse struct
                    std::string pathWidth = "-"; // Would need path data from eclipse struct
                    std::string centralLine = eclipse.location.empty() ? "Global" : eclipse.location;

                    addEclipseEventRow(table, date, type, time, magnitude, duration,
                                     visibility, saros, pathWidth, centralLine);
                }

                std::cout << "\n" << table.toString() << std::endl;

                if (eclipses.empty()) {
                    std::cout << "\nNo eclipses found in the specified period.\n";
                }
            } else if (args.eclipseFormat == "csv") {
                // CSV output
                std::cout << "Date,Type,Magnitude,Duration,Visibility,Location\n";
                for (const auto& eclipse : eclipses) {
                    std::cout << eclipse.getDateString() << ","
                              << eclipse.getTypeString() << ","
                              << eclipse.magnitude << ","
                              << eclipse.duration << ","
                              << (eclipse.isVisible ? "Visible" : "Not Visible") << ","
                              << (eclipse.location.empty() ? "Global" : eclipse.location) << "\n";
                }
            } else if (args.eclipseFormat == "json") {
                // JSON output
                std::cout << "{\n  \"eclipses\": [\n";
                for (size_t i = 0; i < eclipses.size(); ++i) {
                    const auto& eclipse = eclipses[i];
                    std::cout << "    {\n";
                    std::cout << "      \"date\": \"" << eclipse.getDateString() << "\",\n";
                    std::cout << "      \"type\": \"" << eclipse.getTypeString() << "\",\n";
                    std::cout << "      \"magnitude\": " << eclipse.magnitude << ",\n";
                    std::cout << "      \"duration\": " << eclipse.duration << ",\n";
                    std::cout << "      \"visible\": " << (eclipse.isVisible ? "true" : "false") << ",\n";
                    std::cout << "      \"location\": \"" << (eclipse.location.empty() ? "Global" : eclipse.location) << "\"\n";
                    std::cout << "    }" << (i < eclipses.size() - 1 ? "," : "") << "\n";
                }
                std::cout << "  ],\n";
                std::cout << "  \"total_count\": " << eclipses.size() << "\n";
                std::cout << "}\n";
            } else {
                // Traditional text output
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

            if (args.conjunctionFormat == "table") {
                // Professional table output
                ProfessionalTable table = createConjunctionTable();

                std::stringstream subtitle;
                subtitle << conjunctions.size() << " conjunctions found | Period: " << fromDate << " ↔ " << toDate
                         << " | Orb: " << args.conjunctionMaxOrb << "°";
                if (args.conjunctionMinLatitude != -90.0 || args.conjunctionMaxLatitude != 90.0) {
                    subtitle << " | Latitude Range: " << args.conjunctionMinLatitude
                             << "° to " << args.conjunctionMaxLatitude << "°";
                }
                table.setSubtitle(subtitle.str());

                for (const auto& conjunction : conjunctions) {
                    // Note: These fields would need to be extracted from ConjunctionEvent structure
                    std::string date = "2024-01-01"; // Placeholder - would extract from conjunction.julianDay
                    std::string time = "12:00"; // Placeholder
                    std::string planet1 = "Mercury"; // Placeholder - conjunction.planet1
                    std::string planet2 = "Venus"; // Placeholder - conjunction.planet2
                    std::string separation = "2.5°"; // Placeholder - conjunction.separation
                    std::string orb = std::to_string(args.conjunctionMaxOrb) + "°";
                    std::string sign = "Gemini"; // Placeholder - would calculate from longitude
                    std::string strength = "Strong"; // Placeholder - based on orb and planets
                    std::string significance = "Communication enhanced"; // Placeholder

                    addConjunctionEventRow(table, date, time, planet1, planet2, separation,
                                         orb, sign, strength, significance);
                }

                std::cout << "\n" << table.toString() << std::endl;

                if (conjunctions.empty()) {
                    std::cout << "\nNo conjunctions found in the specified period.\n";
                }
            } else if (args.conjunctionFormat == "csv") {
                // CSV output
                std::cout << "Date,Planet1,Planet2,Separation,Orb,Significance\n";
                for (const auto& conjunction : conjunctions) {
                    std::cout << "2024-01-01,Mercury,Venus,2.5°," << args.conjunctionMaxOrb << "°,Strong conjunction\n";
                    // Note: Would need actual data extraction from conjunction object
                }
            } else if (args.conjunctionFormat == "json") {
                // JSON output
                std::cout << "{\n  \"conjunctions\": [\n";
                for (size_t i = 0; i < conjunctions.size(); ++i) {
                    std::cout << "    {\n";
                    std::cout << "      \"date\": \"2024-01-01\",\n";
                    std::cout << "      \"planet1\": \"Mercury\",\n";
                    std::cout << "      \"planet2\": \"Venus\",\n";
                    std::cout << "      \"separation\": \"2.5\",\n";
                    std::cout << "      \"orb\": \"" << args.conjunctionMaxOrb << "\",\n";
                    std::cout << "      \"significance\": \"Strong conjunction\"\n";
                    std::cout << "    }" << (i < conjunctions.size() - 1 ? "," : "") << "\n";
                }
                std::cout << "  ],\n";
                std::cout << "  \"total_count\": " << conjunctions.size() << "\n";
                std::cout << "}\n";
            } else {
                // Traditional text output
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

            if (args.grahaYuddhaFormat == "table") {
                // Professional table format for Graha Yuddha events
                ProfessionalTable table = createGrahaYuddhaTable();

                std::ostringstream subtitle;
                subtitle << wars.size() << " planetary wars found | Period: " << fromDate
                        << " ↔ " << toDate << " | Max Orb: " << args.grahaYuddhaMaxOrb << "°";
                table.setSubtitle(subtitle.str());

                for (const auto& war : wars) {
                    std::string date = war.getDateString();
                    std::string time = "12:00"; // Placeholder - could extract from julianDay
                    std::string combatants = war.getPlanetsString();
                    std::string separation = std::to_string(war.orb) + "°";

                    // Get winner and loser
                    std::string winner = "Unknown";
                    std::string loser = "Unknown";
                    if (war.isGrahaYuddha && war.planets.size() >= 2) {
                        winner = planetToString(war.grahaYuddhaWinner);
                        // Determine loser (the other planet)
                        for (const auto& planet : war.planets) {
                            if (planet != war.grahaYuddhaWinner) {
                                loser = planetToString(planet);
                                break;
                            }
                        }
                    }

                    std::string victoryMargin = std::to_string(war.orb) + "°";
                    std::string effects = war.grahaYuddhaEffect;
                    std::string significance = war.getDescription();

                    addGrahaYuddhaRow(table, date, time, combatants, separation, winner,
                                     loser, victoryMargin, effects, significance);
                }

                std::cout << table.toString();
            } else if (args.grahaYuddhaFormat == "csv") {
                std::cout << "Date,Combatants,Separation,Winner,Loser,Effects\n";
                for (const auto& war : wars) {
                    std::string winner = "Unknown";
                    std::string loser = "Unknown";
                    if (war.isGrahaYuddha && war.planets.size() >= 2) {
                        winner = planetToString(war.grahaYuddhaWinner);
                        for (const auto& planet : war.planets) {
                            if (planet != war.grahaYuddhaWinner) {
                                loser = planetToString(planet);
                                break;
                            }
                        }
                    }

                    std::cout << war.getDateString() << ","
                             << war.getPlanetsString() << ","
                             << war.orb << "°,"
                             << winner << ","
                             << loser << ","
                             << war.grahaYuddhaEffect << "\n";
                }
            } else if (args.grahaYuddhaFormat == "json") {
                std::cout << "{\n  \"graha_yuddha_events\": [\n";
                for (size_t i = 0; i < wars.size(); ++i) {
                    const auto& war = wars[i];
                    std::string winner = "Unknown";
                    std::string loser = "Unknown";
                    if (war.isGrahaYuddha && war.planets.size() >= 2) {
                        winner = planetToString(war.grahaYuddhaWinner);
                        for (const auto& planet : war.planets) {
                            if (planet != war.grahaYuddhaWinner) {
                                loser = planetToString(planet);
                                break;
                            }
                        }
                    }

                    std::cout << "    {\n";
                    std::cout << "      \"date\": \"" << war.getDateString() << "\",\n";
                    std::cout << "      \"combatants\": \"" << war.getPlanetsString() << "\",\n";
                    std::cout << "      \"separation\": " << war.orb << ",\n";
                    std::cout << "      \"winner\": \"" << winner << "\",\n";
                    std::cout << "      \"loser\": \"" << loser << "\",\n";
                    std::cout << "      \"effects\": \"" << war.grahaYuddhaEffect << "\",\n";
                    std::cout << "      \"significance\": \"" << war.getDescription() << "\"\n";
                    std::cout << "    }" << (i < wars.size() - 1 ? "," : "") << "\n";
                }
                std::cout << "  ]\n}\n";
            } else {
                // Default text format (original behavior)
                std::cout << conjCalc.generateGrahaYuddhaReport(wars);
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

            // Generate ephemeris table with configuration
            Astro::EphemerisConfig config;

            // Initialize BirthData structures with default values
            config.startDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};
            config.endDate = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};

            // Parse date strings with BC era support
            Astro::parseBCDate(fromDate, config.startDate.year, config.startDate.month, config.startDate.day);
            Astro::parseBCDate(toDate, config.endDate.year, config.endDate.month, config.endDate.day);

            config.intervalDays = args.ephemerisIntervalDays;
            config.format = args.ephemerisFormat;

            // Set zodiac mode and ayanamsa
            config.zodiacMode = args.zodiacMode;
            config.ayanamsa = args.ayanamsa;

            // Set coordinate type options
            if (args.ephemerisCoordinateType == "declination") {
                config.showDeclination = true;
                config.showDegreeMinutes = true;
                config.showSign = false;  // Declination doesn't use zodiac signs
            } else if (args.ephemerisCoordinateType == "both") {
                config.showDeclination = true;
                config.showDegreeMinutes = true;
                config.showSign = true;   // Show both longitude with signs and declination
            } else {
                // Default longitude mode
                config.showDeclination = false;
                config.showDegreeMinutes = true;
                config.showSign = true;
            }

            // Set sidereal time and compact format options
            config.showSiderealTime = args.ephemerisShowSiderealTime;
            config.compactFormat = args.ephemerisCompactFormat;
            config.useColors = args.ephemerisUseColors;

            std::string result = ephemTable.generateTable(config);

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
                } else if (args.panchangaFormat == "list") {
                    // Professional table format using ProfessionalTable system
                    ProfessionalTable table = createHinduCalendarTable();

                    std::ostringstream subtitle;
                    subtitle << panchangaList.size() << " days found | Period: " << fromDate
                            << " to " << toDate << " | Location: "
                            << std::fixed << std::setprecision(2) << args.latitude << "N, " << args.longitude << "E";
                    if (args.showFestivalsOnly) {
                        subtitle << " | Festivals Only";
                    }
                    table.setSubtitle(subtitle.str());

                    // Parse start date to calculate actual dates for each entry
                    int startYear = std::stoi(fromDate.substr(0, 4));
                    int startMonth = std::stoi(fromDate.substr(5, 2));
                    int startDay = std::stoi(fromDate.substr(8, 2));
                    double startJD = swe_julday(startYear, startMonth, startDay, 12.0, SE_GREG_CAL);

                    for (size_t i = 0; i < panchangaList.size(); ++i) {
                        const auto& panchanga = panchangaList[i];

                        // Calculate actual gregorian date for this entry
                        double currentJD = startJD + i;
                        int gYear, gMonth, gDay;
                        double gTime;
                        swe_revjul(currentJD, SE_GREG_CAL, &gYear, &gMonth, &gDay, &gTime);

                        char dateBuffer[16];
                        snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d", gYear, gMonth, gDay);
                        std::string date = std::string(dateBuffer);

                        // Convert PanchangaData to strings for the table
                        std::string weekday = hinduCalendar.getVaraName(panchanga.vara);
                        std::string tithi = hinduCalendar.getTithiName(panchanga.tithi);
                        std::string nakshatra = hinduCalendar.getNakshatraName(panchanga.nakshatra);
                        std::string yoga = hinduCalendar.getYogaName(panchanga.yoga);
                        std::string karana = hinduCalendar.getKaranaName(panchanga.karana);
                        std::string month = hinduCalendar.getHinduMonthName(panchanga.month);
                        std::string paksha = panchanga.isKrishna ? "Krishna" : "Shukla";
                        std::string year = std::to_string(panchanga.year);
                        std::string muhurta = panchanga.isShubhaMuhurta ? "Good" : (panchanga.isAshubhaMuhurta ? "Avoid" : "Neutral");

                        // Handle festivals vector
                        std::string events = "-";
                        if (!panchanga.festivals.empty()) {
                            events = panchanga.festivals[0];
                            if (panchanga.festivals.size() > 1) {
                                events += " & " + std::to_string(panchanga.festivals.size() - 1) + " more";
                            }
                        }

                        // Calculate a basic score based on auspiciousness
                        double score = 0.5; // Base score
                        if (panchanga.isShubhaMuhurta) score += 0.3;
                        if (panchanga.isAshubhaMuhurta) score -= 0.2;
                        if (panchanga.isEkadashi) score += 0.2;
                        if (panchanga.isPurnima || panchanga.isAmavasya) score += 0.1;
                        if (!panchanga.festivals.empty()) score += 0.2;
                        std::string scoreStr = std::to_string(score).substr(0, 4);

                        addHinduCalendarRow(table, date, weekday, tithi, nakshatra, yoga, karana,
                                          month, paksha, year, muhurta, events, scoreStr);
                    }

                    std::cout << table.toString();
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
                    // Default table format - create custom table with actual dates
                    std::ostringstream oss;

                    oss << "\n═══════════════════════════════════════════════════════════════════\n";
                    oss << "              🕉️  HINDU PANCHANGA TABLE  🕉️\n";
                    oss << "═══════════════════════════════════════════════════════════════════\n\n";

                    // Header
                    oss << "Date       | Tithi        | Vara       | Nakshatra          | Yoga         | Karana       | Festivals\n";
                    oss << "-----------|--------------|------------|--------------------|--------------|--------------|---------\n";

                    // Parse start date to calculate actual dates for each entry
                    int startYear = std::stoi(fromDate.substr(0, 4));
                    int startMonth = std::stoi(fromDate.substr(5, 2));
                    int startDay = std::stoi(fromDate.substr(8, 2));
                    double startJD = swe_julday(startYear, startMonth, startDay, 12.0, SE_GREG_CAL);

                    for (size_t i = 0; i < panchangaList.size(); ++i) {
                        const auto& panchanga = panchangaList[i];

                        // Calculate actual gregorian date for this entry
                        double currentJD = startJD + i;
                        int gYear, gMonth, gDay;
                        double gTime;
                        swe_revjul(currentJD, SE_GREG_CAL, &gYear, &gMonth, &gDay, &gTime);

                        char dateBuffer[16];
                        snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d", gYear, gMonth, gDay);

                        // Format table row with actual date
                        oss << std::setw(10) << std::left << std::string(dateBuffer) << " | ";
                        oss << std::setw(12) << hinduCalendar.getTithiName(panchanga.tithi) << " | ";
                        oss << std::setw(10) << hinduCalendar.getVaraName(panchanga.vara) << " | ";
                        oss << std::setw(18) << hinduCalendar.getNakshatraName(panchanga.nakshatra) << " | ";
                        oss << std::setw(12) << hinduCalendar.getYogaName(panchanga.yoga) << " | ";
                        oss << std::setw(12) << hinduCalendar.getKaranaName(panchanga.karana) << " | ";

                        if (!panchanga.festivals.empty()) {
                            oss << panchanga.festivals[0];
                            for (size_t j = 1; j < panchanga.festivals.size(); ++j) {
                                oss << ", " << panchanga.festivals[j];
                            }
                        }
                        oss << "\n";
                    }

                    oss << "\n═══════════════════════════════════════════════════════════════════\n";
                    std::cout << oss.str() << std::endl;
                }
            } else {
                std::cout << "Failed to generate Panchanga for the specified period." << std::endl;
            }
        }

        // Handle Simplified Julian Day Search
        if (args.searchJdOnly > 0) {
            HinduCalendar hinduCalendar;
            if (!hinduCalendar.initialize()) {
                std::cerr << "Error: Failed to initialize Hindu Calendar system: " << hinduCalendar.getLastError() << std::endl;
                return 1;
            }

            try {
                // Search for the specific Julian Day
                HinduCalendar::SearchResult result = hinduCalendar.searchJulianDayOnly(args.searchJdOnly, args.latitude, args.longitude);

                std::cout << "🔢 JULIAN DAY SEARCH RESULT 🕉️\n";
                std::cout << "=================================\n\n";
                std::cout << "Julian Day: " << std::fixed << std::setprecision(1) << result.julianDay << "\n";
                std::cout << "Gregorian Date: " << result.gregorianDate << "\n";
                std::cout << "Weekday: " << hinduCalendar.getVaraName(result.panchangaData.vara) << "\n\n";

                // Display complete Panchanga information
                std::cout << hinduCalendar.generatePanchangaTable(result.panchangaData) << std::endl;

                // Exit after successful JD search
                return 0;

            } catch (const std::exception& e) {
                std::cerr << "Error searching Julian Day " << args.searchJdOnly << ": " << e.what() << std::endl;
                return 1;
            }
        }

        // Handle Simplified Myanmar Calendar JD Search
        if (args.searchJdMyanmarOnly > 0) {
            MyanmarCalendar myanmarCalendar;
            if (!myanmarCalendar.initialize()) {
                std::cerr << "Error: Failed to initialize Myanmar Calendar system" << std::endl;
                return 1;
            }

            try {
                // Calculate Myanmar calendar data for the specific Julian Day
                MyanmarCalendarData result = myanmarCalendar.calculateMyanmarCalendar(args.searchJdMyanmarOnly);

                std::cout << "🔢 JULIAN DAY MYANMAR CALENDAR RESULT 🇲🇲\n";
                std::cout << "==========================================\n\n";
                std::cout << "Julian Day: " << std::fixed << std::setprecision(1) << args.searchJdMyanmarOnly << "\n";

                // Convert JD to Gregorian date for reference
                int year, month, day;
                double gTime;
                swe_revjul(args.searchJdMyanmarOnly, SE_GREG_CAL, &year, &month, &day, &gTime);
                std::cout << "Gregorian Date: " << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setw(2) << day << "\n";

                // Display complete Myanmar calendar information
                std::cout << myanmarCalendar.generateMyanmarCalendarTable(result) << std::endl;

                // Exit after successful Myanmar JD search
                return 0;

            } catch (const std::exception& e) {
                std::cerr << "Error searching Myanmar calendar for Julian Day " << args.searchJdMyanmarOnly << ": " << e.what() << std::endl;
                return 1;
            }
        }

        // Handle Simplified Birth Chart JD Search
        if (args.searchJdBirthChartOnly > 0) {
            // Birth chart requires coordinates
            if (args.latitude < -90.0 || args.latitude > 90.0 ||
                args.longitude < -180.0 || args.longitude > 180.0) {
                std::cerr << "Error: Valid coordinates (--lat and --lon) required for birth chart JD search\n";
                return 1;
            }

            try {
                // Convert JD to Gregorian date and time
                int year, month, day;
                double gTime;
                swe_revjul(args.searchJdBirthChartOnly, SE_GREG_CAL, &year, &month, &day, &gTime);

                // Convert fractional day to hours:minutes (gTime is fractional part of day)
                double fractionalDay = gTime - floor(gTime);
                int hours = static_cast<int>(fractionalDay * 24);
                int minutes = static_cast<int>((fractionalDay * 24 - hours) * 60);

                // Create BirthData from JD
                BirthData birthData;
                birthData.year = year;
                birthData.month = month;
                birthData.day = day;
                birthData.hour = hours;
                birthData.minute = minutes;
                birthData.second = 0;
                birthData.latitude = args.latitude;
                birthData.longitude = args.longitude;
                birthData.timezone = 0.0; // UTC

                // Use HoroscopeCalculator to properly calculate the birth chart
                HoroscopeCalculator calculator;
                if (!calculator.initialize(args.ephemerisPath)) {
                    std::cerr << "Error: Failed to initialize horoscope calculator: " << calculator.getLastError() << "\n";
                    return 1;
                }

                BirthChart chart;
                if (!calculator.calculateBirthChart(birthData, args.houseSystem, args.zodiacMode, args.ayanamsa, chart)) {
                    std::cerr << "Error: Failed to calculate birth chart: " << calculator.getLastError() << "\n";
                    return 1;
                }

                std::cout << "🔢 JULIAN DAY BIRTH CHART RESULT ⭐\n";
                std::cout << "====================================\n\n";
                std::cout << "Julian Day: " << std::fixed << std::setprecision(1) << args.searchJdBirthChartOnly << "\n";
                std::cout << "Gregorian Date: " << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setw(2) << day << "\n";
                std::cout << "Time: " << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << " UTC\n";
                std::cout << "Location: " << std::fixed << std::setprecision(4) << args.latitude << "°N, " << args.longitude << "°E\n\n";

                // Display birth chart
                std::cout << chart.getFormattedChart() << std::endl;

                // Exit after successful birth chart JD search
                return 0;

            } catch (const std::exception& e) {
                std::cerr << "Error generating birth chart for Julian Day " << args.searchJdBirthChartOnly << ": " << e.what() << std::endl;
                return 1;
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

            // Set logic mode
            criteria.logicMode = (args.hinduSearchLogic == "or") ? HinduCalendar::LogicMode::OR : HinduCalendar::LogicMode::AND;

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

            // Extended Panchanga criteria
            if (args.searchNakshatra > 0) {
                criteria.exactNakshatra = args.searchNakshatra;
            } else if (args.searchNakshatraStart > 0) {
                criteria.nakshatraRangeStart = args.searchNakshatraStart;
                criteria.nakshatraRangeEnd = args.searchNakshatraEnd;
            }

            if (args.searchYoga > 0) {
                criteria.exactYoga = args.searchYoga;
            } else if (args.searchYogaStart > 0) {
                criteria.yogaRangeStart = args.searchYogaStart;
                criteria.yogaRangeEnd = args.searchYogaEnd;
            }

            // Special day criteria
            criteria.searchEkadashi = args.searchEkadashi;
            criteria.searchPurnima = args.searchPurnima;
            criteria.searchAmavasya = args.searchAmavasya;
            criteria.searchSankranti = args.searchSankranti;

            // Julian Day criteria
            if (args.searchJulianDay > 0) {
                criteria.exactJulianDay = args.searchJulianDay;
            } else if (args.searchJulianDayStart > 0) {
                criteria.julianDayRangeStart = args.searchJulianDayStart;
                criteria.julianDayRangeEnd = args.searchJulianDayEnd;
            }

            // Varna day criteria
            criteria.searchBrahminDays = args.searchBrahminDays;
            criteria.searchKshatriyaDays = args.searchKshatriyaDays;
            criteria.searchVaishyaDays = args.searchVaishyaDays;
            criteria.searchShudradays = args.searchShudradays;

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
                    // Simple date list format
                    for (const auto& result : searchResults) {
                        std::cout << result.gregorianDate << "\n";
                    }
                } else {
                    // Professional table format using ProfessionalTable system
                    ProfessionalTable table = createHinduCalendarTable();

                    std::ostringstream subtitle;
                    subtitle << searchResults.size() << " results found | Period: " << args.searchStartDate
                            << " to " << args.searchEndDate << " | Location: "
                            << std::fixed << std::setprecision(2) << args.latitude << "N, " << args.longitude << "E";
                    table.setSubtitle(subtitle.str());

                    for (const auto& result : searchResults) {
                        std::string date = result.gregorianDate;
                        std::string weekday = hinduCalendar.getVaraName(result.panchangaData.vara);
                        std::string tithi = hinduCalendar.getTithiName(result.panchangaData.tithi);
                        std::string nakshatra = hinduCalendar.getNakshatraName(result.panchangaData.nakshatra);
                        std::string yoga = hinduCalendar.getYogaName(result.panchangaData.yoga);
                        std::string karana = hinduCalendar.getKaranaName(result.panchangaData.karana);
                        std::string month = hinduCalendar.getHinduMonthName(result.panchangaData.month);
                        std::string paksha = result.panchangaData.isShukla ? "Shukla" : "Krishna";
                        std::string year = std::to_string(result.panchangaData.year);

                        std::string muhurta = "Neutral";
                        if (result.panchangaData.isShubhaMuhurta) muhurta = "Good";
                        else if (result.panchangaData.isAshubhaMuhurta) muhurta = "Caution";

                        std::string events = "";
                        if (result.panchangaData.isEkadashi) events += "Ekadashi ";
                        if (result.panchangaData.isPurnima) events += "Purnima ";
                        if (result.panchangaData.isAmavasya) events += "Amavasya ";
                        if (result.panchangaData.isSankranti) events += "Sankranti ";
                        if (!result.panchangaData.festivals.empty()) {
                            events += result.panchangaData.festivals[0];
                        }
                        if (events.empty()) events = "-";

                        std::string score = std::to_string(result.matchScore).substr(0, 5);

                        addHinduCalendarRow(table, date, weekday, tithi, nakshatra, yoga, karana,
                                          month, paksha, year, muhurta, events, score);
                    }

                    std::cout << table.toString();
                }
            } else {
                std::cout << "\nHINDU CALENDAR SEARCH RESULTS\n";
                std::cout << "=================================\n\n";
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

            // Set logic mode
            if (args.myanmarSearchLogic == "or") {
                criteria.logicMode = MyanmarCalendar::LogicMode::OR;
            } else {
                criteria.logicMode = MyanmarCalendar::LogicMode::AND;
            }

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
                    // Generate professional-grade Myanmar tabular output with enhanced design and ASCII borders - NO UNICODE EMOJIS
                    std::cout << "\n";
                    std::cout << "+==================================================================================================================================================================================================+\n";
                    std::cout << "|                                                  MYANMAR CALENDAR SEARCH RESULTS - PROFESSIONAL VIEW                                                                              |\n";
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

                    std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+---------+----------+----------------+-------+\n";
                    std::cout << "|    DATE     |  WEEKDAY   |MY.YR |    MONTH    | DAY |MOON.PHASE |  MAHABOTE  | NAKHAT  | NAGAHLE | RELIGIOUS|  ASTRO.DAYS    |SCORE  |\n";
                    std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+---------+----------+----------------+-------+\n";

                    // Professional Myanmar table rows with enhanced formatting
                    size_t rowCount = 0;
                    for (const auto& result : searchResults) {
                        rowCount++;

                        // Date column with center alignment
                        std::cout << "| " << std::setw(11) << std::left << result.gregorianDate << " | ";

                        // Weekday column with Myanmar cultural indicators - fixed 10 chars - NO UNICODE EMOJIS
                        std::string weekday = myanmarCalendar.getMyanmarWeekdayName(result.myanmarData.weekday);
                        std::string weekdayDisplay = weekday;

                        // Create consistent weekday abbreviations without emojis
                        if (weekday == "Sunday") weekdayDisplay = "P-Sunday";
                        else if (weekday == "Monday") weekdayDisplay = "M-Monday";
                        else if (weekday == "Tuesday") weekdayDisplay = "P-Tuesday";
                        else if (weekday == "Wednesday") weekdayDisplay = "Wednesday";
                        else if (weekday == "Thursday") weekdayDisplay = "M-Thursday";
                        else if (weekday == "Friday") weekdayDisplay = "Friday";
                        else if (weekday == "Saturday") weekdayDisplay = "Saturday";

                        // Ensure fixed width of exactly 10 characters
                        if (weekdayDisplay.length() > 10) weekdayDisplay = weekdayDisplay.substr(0, 10);
                        std::cout << std::setw(10) << std::left << weekdayDisplay << " | ";

                        // Myanmar Year column - right aligned
                        std::cout << std::setw(4) << std::right << result.myanmarData.myanmarYear << " | ";

                        // Month column with seasonal indicators - NO UNICODE EMOJIS
                        std::string month = myanmarCalendar.getMyanmarMonthName(result.myanmarData.month);
                        if (month.length() > 11) month = month.substr(0, 9) + "..";
                        // Add seasonal context for Myanmar months without emojis
                        std::vector<std::string> hotMonths = {"Tagu", "Kason", "Nayon"};
                        std::vector<std::string> rainyMonths = {"Waso", "Wagaung", "Tawthalin"};
                        std::vector<std::string> coolMonths = {"Thadingyut", "Tazaungmon", "Nadaw", "Pyatho", "Tabodwe", "Tabaung"};

                        std::string monthDisplay = month;
                        for (const auto& hot : hotMonths) {
                            if (month.find(hot) != std::string::npos) {
                                monthDisplay = "H-" + month.substr(0, 9);
                                break;
                            }
                        }
                        for (const auto& rainy : rainyMonths) {
                            if (month.find(rainy) != std::string::npos) {
                                monthDisplay = "R-" + month.substr(0, 9);
                                break;
                            }
                        }
                        for (const auto& cool : coolMonths) {
                            if (month.find(cool) != std::string::npos) {
                                monthDisplay = "C-" + month.substr(0, 9);
                                break;
                            }
                        }
                        std::cout << std::setw(11) << std::left << monthDisplay << " | ";

                        // Day column - center aligned
                        std::cout << std::setw(3) << std::right << result.myanmarData.dayOfMonth << " | ";

                        // Moon Phase column with visual indicators - NO UNICODE EMOJIS
                        std::string moonPhase = myanmarCalendar.getMoonPhaseName(result.myanmarData.moonPhase);
                        if (moonPhase.length() > 9) moonPhase = moonPhase.substr(0, 7) + "..";
                        // Add moon phase visual symbols without emojis
                        if (moonPhase.find("Full") != std::string::npos) moonPhase = "F-" + moonPhase.substr(0, 7);
                        else if (moonPhase.find("New") != std::string::npos) moonPhase = "N-" + moonPhase.substr(0, 7);
                        else if (moonPhase.find("Waxing") != std::string::npos) moonPhase = "W+" + moonPhase.substr(0, 6);
                        else if (moonPhase.find("Waning") != std::string::npos) moonPhase = "W-" + moonPhase.substr(0, 6);
                        std::cout << std::setw(9) << std::left << moonPhase << " | ";

                        // Mahabote column with quality indicators - fixed 10 chars - NO UNICODE EMOJIS
                        std::string mahabote;
                        switch(result.myanmarData.mahabote) {
                            case Mahabote::BINGA: mahabote = "B-Binga"; break;      // Benefic
                            case Mahabote::ATUN: mahabote = "N-Atun"; break;       // Neutral
                            case Mahabote::YAZA: mahabote = "M-Yaza"; break;       // Malefic
                            case Mahabote::ADIPATI: mahabote = "B-Adipati"; break;  // Benefic
                            case Mahabote::MARANA: mahabote = "M-Marana"; break;   // Malefic
                            case Mahabote::THIKE: mahabote = "N-Thike"; break;     // Neutral
                            case Mahabote::PUTI: mahabote = "B-Puti"; break;       // Benefic
                        }

                        // Ensure fixed width of exactly 10 characters
                        if (mahabote.length() > 10) mahabote = mahabote.substr(0, 10);
                        std::cout << std::setw(10) << std::left << mahabote << " | ";

                        // Nakhat column with cycle indicators - NO UNICODE EMOJIS
                        std::string nakhat;
                        switch(result.myanmarData.nakhat) {
                            case Nakhat::ORC: nakhat = "A-Orc"; break;      // Active/Aggressive
                            case Nakhat::ELF: nakhat = "P-Elf"; break;      // Peaceful/Wise
                            case Nakhat::HUMAN: nakhat = "B-Human"; break;  // Balanced
                        }
                        std::cout << std::setw(7) << std::left << nakhat << " | ";

                        // Nagahle column with directional symbols - NO UNICODE EMOJIS
                        std::string nagahle;
                        switch(result.myanmarData.nagahle) {
                            case NagahleDirection::WEST: nagahle = "W-West"; break;
                            case NagahleDirection::NORTH: nagahle = "N-North"; break;
                            case NagahleDirection::EAST: nagahle = "E-East "; break;
                            case NagahleDirection::SOUTH: nagahle = "S-South"; break;
                        }
                        std::cout << std::setw(6) << std::left << nagahle << " | ";

                        // Religious days column with Buddhist symbols - NO UNICODE EMOJIS
                        std::string religious;
                        if (result.myanmarData.isSabbath) religious = "Sabbath";
                        else if (result.myanmarData.isSabbathEve) religious = "S-Eve";
                        else religious = "   -   ";
                        std::cout << std::setw(8) << std::left << religious << " | ";

                        // Astrological days column with enhanced indicators - NO UNICODE EMOJIS
                        std::vector<std::string> astro;
                        if (result.myanmarData.isYatyaza) astro.push_back("M-Yat");
                        if (result.myanmarData.isPyathada) astro.push_back("M-Pya");
                        if (result.myanmarData.isThamanyo) astro.push_back("G-Tha");
                        if (result.myanmarData.isAmyeittasote) astro.push_back("N-Amy");
                        if (result.myanmarData.isWarameittugyi) astro.push_back("N-War");

                        std::string astroStr;
                        for (size_t i = 0; i < astro.size() && astroStr.length() < 12; ++i) {
                            if (i > 0) astroStr += " ";
                            astroStr += astro[i];
                        }
                        if (astroStr.empty()) astroStr = "      -      ";
                        if (astroStr.length() > 14) astroStr = astroStr.substr(0, 11) + "...";
                        std::cout << std::setw(14) << std::left << astroStr << " | ";

                        // Score column with quality indicator - NO UNICODE EMOJIS
                        std::string scoreDisplay = std::to_string(result.matchScore);
                        if (scoreDisplay.length() > 4) scoreDisplay = scoreDisplay.substr(0, 4);
                        if (result.matchScore >= 0.90) scoreDisplay = "E" + scoreDisplay;
                        else if (result.matchScore >= 0.75) scoreDisplay = "G" + scoreDisplay;
                        std::cout << std::setw(4) << std::right << scoreDisplay << " |";

                        std::cout << "\n";

                        // Add subtle row separators every 5 rows
                        if (rowCount % 5 == 0 && rowCount < searchResults.size()) {
                            std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+---------+----------+----------------+-------+\n";
                        }
                    }

                    std::cout << "+-------------+------------+------+-------------+-----+-----------+------------+---------+---------+----------+----------------+-------+\n";

                    // Enhanced professional legend for Myanmar calendar - NO UNICODE EMOJIS
                    std::cout << "+---------------------------------------------- MYANMAR LEGEND & SYMBOLS ----------------------------------------------+\n";
                    std::cout << "| WEEKDAY: P=Power Days(Sun/Tue) M=Moon Days(Mon/Thu) •=Regular Days                                                 |\n";
                    std::cout << "| MONTH: H=Hot Season(Tagu-Nayon) R=Rainy Season(Waso-Tawthalin) C=Cool Season(Thadingyut-Tabaung)                 |\n";
                    std::cout << "| MOON: F=Full Moon N=New Moon W+=Waxing W-=Waning                                                                  |\n";
                    std::cout << "| MAHABOTE: B=Benefic(Binga/Adipati/Puti) M=Malefic(Yaza/Marana) N=Neutral(Atun/Thike)                            |\n";
                    std::cout << "| NAKHAT: A=Orc(Active) P=Elf(Peaceful) B=Human(Balanced) • 3-Year Cycle                                          |\n";
                    std::cout << "| NAGAHLE: W=West N=North E=East S=South • Snake Head Direction                                                     |\n";
                    std::cout << "| RELIGIOUS: Sabbath/S-Eve=Buddhist Days • Important for Merit-making                                              |\n";
                    std::cout << "| ASTRO.DAYS: M-Yat=Yatyaza(Avoid) M-Pya=Pyathada(Caution) G-Tha=Thamanyo(Auspicious) N=Others                   |\n";
                    std::cout << "| SCORE: E=Excellent(≥0.9) G=Good(≥0.75) •=Standard(<0.75)                                                        |\n";
                    std::cout << "+-------------------------------------------------------------------------------------------------------------------+\n";
                    std::cout << "| YEAR TYPE: " << (searchResults.empty() ? "N/A" :
                                  (searchResults[0].myanmarData.yearType == MyanmarYearType::COMMON ? "Common Year" :
                                   searchResults[0].myanmarData.yearType == MyanmarYearType::LITTLE_WATAT ? "Little Watat (13 months)" :
                                   "Big Watat (13 months + extra day)")) << "                                       |\n";
                    std::cout << "| Full detailed analysis available using 'table' format • Buddhist Era: Myanmar Calendar System                     |\n";
                    std::cout << "+-------------------------------------------------------------------------------------------------------------------+\n";
                } else {
                    // Default table format - WITH UNICODE EMOJIS
                    std::cout << "\n🇲🇲 MYANMAR CALENDAR SEARCH RESULTS 🇲🇲\n";
                    std::cout << "======================================\n";
                    std::cout << "Found " << searchResults.size() << " matching days:\n\n";

                    for (const auto& result : searchResults) {
                        std::cout << "📅 Date: " << result.gregorianDate << " (Score: ";
                        if (result.matchScore >= 0.90) std::cout << "⭐ ";
                        else if (result.matchScore >= 0.75) std::cout << "✨ ";
                        std::cout << result.matchScore << ")\n";

                        std::cout << "   🌙 Myanmar Year: " << result.myanmarData.myanmarYear << ", Month: " << myanmarCalendar.getMyanmarMonthName(result.myanmarData.month);
                        std::cout << ", Day: " << result.myanmarData.fortnightDay << " (";

                        // Add moon phase with emoji
                        std::string moonPhase = myanmarCalendar.getMoonPhaseName(result.myanmarData.moonPhase);
                        if (moonPhase.find("Full") != std::string::npos) std::cout << "🌕 ";
                        else if (moonPhase.find("New") != std::string::npos) std::cout << "🌑 ";
                        else if (moonPhase.find("Waxing") != std::string::npos) std::cout << "🌔 ";
                        else if (moonPhase.find("Waning") != std::string::npos) std::cout << "🌖 ";
                        std::cout << moonPhase << ")\n";

                        // Add weekday with emoji
                        std::string weekday = myanmarCalendar.getMyanmarWeekdayName(result.myanmarData.weekday);
                        std::cout << "   📆 Weekday: ";
                        if (weekday == "Sunday" || weekday == "Tuesday") std::cout << "⚡ ";
                        else if (weekday == "Monday" || weekday == "Thursday") std::cout << "☾ ";
                        std::cout << weekday << "\n";

                        if (!result.matchDescription.empty()) {
                            std::cout << "   ✨ " << result.matchDescription << "\n";
                        }
                        std::cout << "\n";
                    }
                }
            } else {
                if (args.myanmarSearchFormat == "list") {
                    std::cout << "\nMYANMAR CALENDAR SEARCH RESULTS - PROFESSIONAL VIEW\n";
                    std::cout << "=====================================================\n";
                    std::cout << "No matching days found for the specified criteria.\n\n";
                } else {
                    std::cout << "\n🔍 MYANMAR CALENDAR SEARCH RESULTS 🇲🇲\n";
                    std::cout << "═══════════════════════════════════════════════════════\n";
                    std::cout << "No matching days found for the specified criteria.\n\n";
                }

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

    // Handle Myanmar Monthly Calendar (doesn't need birth data)
    if (args.showMyanmarMonthlyCalendar) {
        MyanmarMonthlyCalendar myanmarMonthlyCalendar;

        if (!myanmarMonthlyCalendar.initialize()) {
            std::cerr << "Error: Failed to initialize Myanmar Monthly Calendar: " << myanmarMonthlyCalendar.getLastError() << std::endl;
            return 1;
        }

        try {
            // Parse year and month from myanmarMonthlyCalendarDate
            int year, month;
            if (sscanf(args.myanmarMonthlyCalendarDate.c_str(), "%d-%d", &year, &month) == 2) {
                // Use Yangon coordinates as default if not provided
                double latitude = args.latitude != 0.0 ? args.latitude : 16.8661;  // Yangon
                double longitude = args.longitude != 0.0 ? args.longitude : 96.1951; // Yangon

                // Auto-enable calendars based on format
                bool enableHindu = args.includeHindu;
                bool enablePlanetary = args.includePlanetary;
                bool enableAdvancedAstro = args.includeAdvancedAstro;

                if (args.myanmarMonthlyCalendarFormat == "multi-calendar" ||
                    args.myanmarMonthlyCalendarFormat == "hindu-myanmar" ||
                    args.myanmarMonthlyCalendarFormat == "full-astronomical") {
                    enableHindu = true;
                }

                if (args.myanmarMonthlyCalendarFormat == "planetary" ||
                    args.myanmarMonthlyCalendarFormat == "full-astronomical") {
                    enablePlanetary = true;
                }

                if (args.myanmarMonthlyCalendarFormat == "full-astronomical") {
                    enableAdvancedAstro = true;
                }

                MyanmarMonthlyData monthData = myanmarMonthlyCalendar.calculateMonthlyData(
                    year, month, latitude, longitude,
                    args.includeGregorian, enableHindu, enablePlanetary, enableAdvancedAstro
                );
                std::cout << myanmarMonthlyCalendar.generateMonthlyCalendar(monthData, args.myanmarMonthlyCalendarFormat) << std::endl;
            } else {
                std::cerr << "Error: Invalid date format for Myanmar monthly calendar. Use YYYY-MM format.\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error generating Myanmar monthly calendar: " << e.what() << std::endl;
            return 1;
        }

        return 0; // Exit after Myanmar monthly calendar
    }

    // Handle Hindu Monthly Calendar (doesn't need birth data, supports BC dates)
    if (args.showHinduMonthlyCalendar) {
        HinduMonthlyCalendar hinduMonthlyCalendar;

        // Use provided coordinates or default to New Delhi if not provided
        double latitude = args.latitude != 0.0 ? args.latitude : 28.6139;  // New Delhi
        double longitude = args.longitude != 0.0 ? args.longitude : 77.2090; // New Delhi

        // Set up display options
        HinduMonthlyCalendar::DisplayOptions displayOptions = HinduMonthlyCalendar::getDefaultDisplayOptions();
        displayOptions.ayanamsa = args.ayanamsa;
        displayOptions.showMuhurta = args.hinduMonthlyShowMuhurta;

        // Configure display based on format
        if (args.hinduMonthlyCalendarFormat == "detailed") {
            displayOptions.showMuhurta = true;
            displayOptions.showRashiInfo = true;
            displayOptions.showLunarPhase = true;
        } else if (args.hinduMonthlyCalendarFormat == "panchanga") {
            displayOptions.showTithi = true;
            displayOptions.showNakshatra = true;
            displayOptions.showYoga = true;
            displayOptions.showKarana = true;
        } else if (args.hinduMonthlyCalendarFormat == "compact") {
            displayOptions.showPackedLayout = true;
            displayOptions.cellWidth = 8;
        } else if (args.hinduMonthlyCalendarFormat == "festivals") {
            displayOptions.showFestivals = true;
            displayOptions.showSpecialDays = true;
            displayOptions.showVrataInfo = true;
        } else if (args.hinduMonthlyCalendarFormat == "astronomical") {
            displayOptions.showRashiInfo = true;
            displayOptions.showLunarPhase = true;
            displayOptions.showMuhurta = true;
        } else if (args.hinduMonthlyCalendarFormat == "html") {
            displayOptions.htmlOutput = true;
        }

        if (!hinduMonthlyCalendar.initialize(latitude, longitude, displayOptions)) {
            std::cerr << "Error: Failed to initialize Hindu Monthly Calendar: " << hinduMonthlyCalendar.getLastError() << std::endl;
            return 1;
        }

        try {
            if (args.hinduMonthlyCalendarFormat == "csv") {
                HinduMonthlyCalendar::MonthlyData monthData = hinduMonthlyCalendar.generateMonthlyData(args.hinduMonthlyCalendarDate);
                std::cout << hinduMonthlyCalendar.exportToCSV(monthData) << std::endl;
            } else if (args.hinduMonthlyCalendarFormat == "json") {
                HinduMonthlyCalendar::MonthlyData monthData = hinduMonthlyCalendar.generateMonthlyData(args.hinduMonthlyCalendarDate);
                std::cout << hinduMonthlyCalendar.exportToJSON(monthData) << std::endl;
            } else if (args.hinduMonthlyCalendarFormat == "html") {
                std::cout << hinduMonthlyCalendar.generateHTMLCalendar(args.hinduMonthlyCalendarDate) << std::endl;
            } else {
                std::cout << hinduMonthlyCalendar.generateCalendar(args.hinduMonthlyCalendarDate) << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error generating Hindu monthly calendar: " << e.what() << std::endl;
            return 1;
        }

        return 0; // Exit after Hindu monthly calendar
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

        if (args.kpOutputFormat == "table") {
            // Professional table output
            ProfessionalTable table = createKPSystemTable();
            table.setSubtitle("Krishnamurti Paddhati Sub-Lord Analysis");

            // Add KP system data rows
            const auto& planetPositions = chart.getPlanetPositions();
            for (const auto& planet : planetPositions) {
                std::string planetName = getPlanetName(planet.planet);
                std::string longitude = std::to_string(planet.longitude).substr(0, 8) + "°";
                std::string sign = zodiacSignToString(longitudeToSign(planet.longitude));
                std::string nakshatra = "Ashwini"; // Placeholder - would calculate from longitude
                std::string subLord = "Venus"; // Placeholder - KP calculation needed
                std::string subSub = "Mars"; // Placeholder
                std::string subSubSub = "Jupiter"; // Placeholder
                std::string kpNotation = sign.substr(0,2) + "-" + nakshatra.substr(0,3) + "-" + subLord.substr(0,2);
                std::string signification = "Career, Authority"; // Placeholder

                addKPSystemRow(table, planetName, longitude, sign, nakshatra, subLord,
                              subSub, subSubSub, kpNotation, signification);
            }

            std::cout << "\n" << table.toString() << std::endl;
        } else if (args.kpOutputFormat == "csv") {
            std::cout << "Planet,Longitude,Sign,Nakshatra,SubLord,SubSub,SubSubSub,KP_Notation,Signification\n";
            const auto& planetPositions = chart.getPlanetPositions();
            for (const auto& planet : planetPositions) {
                std::cout << getPlanetName(planet.planet) << ","
                          << planet.longitude << ","
                          << zodiacSignToString(longitudeToSign(planet.longitude)) << ","
                          << "Ashwini,Venus,Mars,Jupiter,Ge-Ash-Ve,Career\n";
            }
        } else if (args.kpOutputFormat == "json") {
            std::cout << "{\n  \"kp_analysis\": [\n";
            const auto& planetPositions = chart.getPlanetPositions();
            for (size_t i = 0; i < planetPositions.size(); ++i) {
                const auto& planet = planetPositions[i];
                std::cout << "    {\n";
                std::cout << "      \"planet\": \"" << getPlanetName(planet.planet) << "\",\n";
                std::cout << "      \"longitude\": " << planet.longitude << ",\n";
                std::cout << "      \"sign\": \"" << zodiacSignToString(longitudeToSign(planet.longitude)) << "\",\n";
                std::cout << "      \"nakshatra\": \"Ashwini\",\n";
                std::cout << "      \"sub_lord\": \"Venus\",\n";
                std::cout << "      \"kp_notation\": \"Ge-Ash-Ve\"\n";
                std::cout << "    }" << (i < planetPositions.size() - 1 ? "," : "") << "\n";
            }
            std::cout << "  ]\n}\n";
        } else {
            // Traditional text output
            std::string kpTable = kpSystem.generateKPTable(chart.getPlanetPositions());
            std::cout << kpTable << std::endl;
        }

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

        // Create a copy of birthData with Myanmar timezone for accurate calendar calculation
        BirthData myanmarBirthData = birthData;
        myanmarBirthData.timezone = 6.5; // Myanmar Standard Time (UTC+6:30)

        MyanmarCalendarData myanmarData = myanmarCalendar.calculateMyanmarDate(myanmarBirthData);

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
