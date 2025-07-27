#pragma once

#include "astro_types.h"
#include <string>
#include <vector>
#include <map>

namespace Astro {

// Hindu calendar elements enumeration
enum class Tithi {
    PRATIPAD = 1, DWITIYA, TRITIYA, CHATURTHI, PANCHAMI,
    SHASHTHI, SAPTAMI, ASHTAMI, NAVAMI, DASHAMI,
    EKADASHI, DWADASHI, TRAYODASHI, CHATURDASHI,
    PURNIMA = 15,  // Full moon
    PRATIPAD_K = 16, DWITIYA_K, TRITIYA_K, CHATURTHI_K, PANCHAMI_K,
    SHASHTHI_K, SAPTAMI_K, ASHTAMI_K, NAVAMI_K, DASHAMI_K,
    EKADASHI_K, DWADASHI_K, TRAYODASHI_K, CHATURDASHI_K,
    AMAVASYA = 30  // New moon
};

enum class Vara {
    SUNDAY = 0, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY
};

enum class HinduNakshatra {
    ASHWINI = 1, BHARANI, KRITTIKA, ROHINI, MRIGASHIRA, ARDRA,
    PUNARVASU, PUSHYA, ASHLESHA, MAGHA, PURVA_PHALGUNI, UTTARA_PHALGUNI,
    HASTA, CHITRA, SWATI, VISHAKHA, ANURADHA, JYESHTHA,
    MULA, PURVA_ASHADHA, UTTARA_ASHADHA, SHRAVANA, DHANISHTA,
    SHATABHISHA, PURVA_BHADRAPADA, UTTARA_BHADRAPADA, REVATI
};

enum class Yoga {
    VISHKUMBHA = 1, PRITI, AYUSHMAN, SAUBHAGYA, SHOBHANA, ATIGANDA,
    SUKARMAN, DHRITI, SHULA, GANDA, VRIDDHI, DHRUVA,
    VYAGHATA, HARSHANA, VAJRA, SIDDHI, VYATIPATA, VARIYAN,
    PARIGHA, SHIVA, SIDDHA, SADHYA, SHUBHA, SHUKLA,
    BRAHMA, INDRA, VAIDHRITI
};

enum class Karana {
    BAVA = 1, BALAVA, KAULAVA, TAITILA, GARA, VANIJA, VISHTI,
    SHAKUNI, CHATUSHPADA, NAGA, KIMSTUGHNA
};

enum class HinduMonth {
    CHAITRA = 1, VAISHAKHA, JYESHTHA, ASHADHA, SHRAVANA, BHADRAPADA,
    ASHWINI_MONTH, KARTIKA, MARGASHIRSHA, PAUSHA, MAGHA, PHALGUNA
};

enum class Rashi {
    MESHA = 1, VRISHABHA, MITHUNA, KARKA, SIMHA, KANYA,
    TULA, VRISHCHIKA, DHANU, MAKARA, KUMBHA, MEENA
};

// Calendar calculation methods
enum class CalculationMethod {
    DRIK_SIDDHANTA = 1,  // Accurate astronomical calculations
    SURYA_SIDDHANTA = 2, // Traditional Surya Siddhanta
    ARYA_SIDDHANTA = 3,  // Arya Siddhanta
    BRAHMA_SIDDHANTA = 4, // Brahma Siddhanta
    MODERN_MIXED = 5     // Modern mixed approach
};

// Calendar system types
enum class CalendarSystem {
    LUNAR_BASED = 1,     // Pure lunar calendar
    SOLAR_BASED = 2,     // Solar calendar with lunar tithis
    LUNI_SOLAR = 3       // Combined system (most common)
};

// Pancanga data structure
struct PanchangaData {
    // Basic five elements
    Tithi tithi;
    Vara vara;
    HinduNakshatra nakshatra;
    Yoga yoga;
    Karana karana;

    // Extended information
    HinduMonth month;
    int day;                    // Day of Hindu month
    int year;                   // Hindu year (Vikram Samvat, Shaka, etc.)
    bool isKrishna;            // Krishna paksha (dark fortnight)
    bool isShukla;             // Shukla paksha (bright fortnight)

    // Timing information
    double tithiEndTime;       // When current tithi ends (in hours from midnight)
    double nakshatraEndTime;   // When current nakshatra ends
    double yogaEndTime;        // When current yoga ends
    double karanaEndTime;      // When current karana ends

    // Solar information
    Rashi sunRashi;            // Sun's zodiac sign
    double sunLongitude;       // Sun's longitude

    // Lunar information
    Rashi moonRashi;           // Moon's zodiac sign
    double moonLongitude;      // Moon's longitude
    double lunarPhase;         // 0-360 degrees from new moon

    // Sun/Moon rise and set times (in decimal hours from midnight)
    double sunriseTime;        // Sunrise time
    double sunsetTime;         // Sunset time
    double moonriseTime;       // Moonrise time
    double moonsetTime;        // Moonset time

    // Day/night duration
    double dayLength;          // Day length in hours
    double nightLength;        // Night length in hours

    // Muhurta timings (all in decimal hours from midnight)
    double brahmaMuhurtaStart; // Brahma Muhurta start
    double brahmaMuhurtaEnd;   // Brahma Muhurta end
    double abhijitStart;       // Abhijit Muhurta start
    double abhijitEnd;         // Abhijit Muhurta end
    double godhuliBelStart;    // Godhuli Bela start
    double godhuliBelEnd;      // Godhuli Bela end
    double nishitaMuhurtaStart;// Nishita Muhurta start
    double nishitaMuhurtaEnd;  // Nishita Muhurta end

    // Rahu Kaal timing
    double rahuKaalStart;      // Rahu Kaal start
    double rahuKaalEnd;        // Rahu Kaal end

    // Yamaganda timing
    double yamagandaStart;     // Yamaganda start
    double yamagandaEnd;       // Yamaganda end

    // Gulikai timing
    double gulikaiStart;       // Gulikai start
    double gulikaiEnd;         // Gulikai end

    // Dur Muhurtam timing
    double durMuhurtamStart;   // Dur Muhurtam start
    double durMuhurtamEnd;     // Dur Muhurtam end

    // Varjyam timing
    std::vector<std::pair<double, double>> varjyamTimes; // Multiple Varjyam periods

    // Ayanamsa and astronomical data
    double ayanamsaValue;      // Current Ayanamsa value
    double julianDay;          // Julian Day Number
    int kaliyugaYear;          // Kali Yuga year
    int shakaYear;             // Shaka Samvat year
    int vikramYear;            // Vikram Samvat year

    // Season and direction information
    std::string ritu;          // Current Ritu (season)
    std::string ayana;         // Uttarayana or Dakshinayana
    std::string dishaShool;    // Direction of Shool
    std::string nakshatraShool;// Nakshatra Shool direction

    // Varna (Savarna) information
    std::string varnaDay;      // Varna of the day (Brahmin, Kshatriya, Vaishya, Shudra)
    std::string varnaTithi;    // Varna based on Tithi
    std::string varnaNakshatra;// Varna based on Nakshatra

    // Nakshatra Pada information
    int nakshatraPada;         // Current Nakshatra Pada (1-4)
    double nakshatraPadaEndTime; // When current pada ends

    // Chandra Balam and Tara Balam
    std::vector<Rashi> goodChandraBalam;    // Good Chandra Balam for these rashis
    std::vector<HinduNakshatra> goodTaraBalam; // Good Tara Balam for these nakshatras

    // Special events and festivals
    std::vector<std::string> festivals;
    std::vector<std::string> specialEvents;
    std::vector<std::string> ekadashiNames;  // Specific Ekadashi names
    bool isEkadashi;
    bool isPurnima;
    bool isAmavasya;
    bool isSankranti;          // Solar transition
    bool isNavratri;           // Navratri period
    bool isGandaMool;          // Ganda Mool Nakshatra

    // Enhanced Sankranti information
    std::string sankrantiName; // Name of the current Sankranti
    double sankrantiTime;      // Exact time of Sankranti (hours from midnight)
    bool isSankrantiToday;     // If Sankranti occurs today
    std::string nextSankranti; // Next upcoming Sankranti name
    double daysToNextSankranti;// Days until next Sankranti
    bool isPanchak;            // Panchak period
    bool isBhadra;             // Bhadra period

    // Additional astronomical yogas
    bool isSarvarthaSiddhi;    // Sarvartha Siddhi Yoga
    bool isAmritaSiddhi;       // Amrita Siddhi Yoga
    bool isDwipushkar;         // Dwipushkar Yoga
    bool isTripushkar;         // Tripushkar Yoga
    bool isRaviPushya;         // Ravi Pushya Yoga
    bool isGuruPushya;         // Guru Pushya Yoga

    // Quality assessments
    bool isShubhaMuhurta;      // Auspicious time
    bool isAshubhaMuhurta;     // Inauspicious time
    std::string muhurtaDescription;

    // Vrata and Upavas
    std::vector<std::string> vrataList;     // List of applicable vratas
    bool isFastingDay;         // Whether it's a fasting day

    // Formatting methods
    std::string getFormattedTithi() const;
    std::string getFormattedNakshatra() const;
    std::string getFormattedYoga() const;
    std::string getFormattedKarana() const;
    std::string getFullDescription() const;
    std::string getSummary() const;
    std::string getTimeString(double hours) const;  // Convert decimal hours to HH:MM format
};

// Main Hindu Calendar System
class HinduCalendar {
private:
    // Calculation parameters
    static constexpr double SIDEREAL_YEAR = 365.25636; // days
    static constexpr double LUNAR_MONTH = 29.53058868; // days
    static constexpr double NAKSHATRA_SPAN = 13.333333; // degrees (360/27)
    static constexpr double YOGA_SPAN = 13.333333; // degrees

    // Reference epochs
    static constexpr double KALI_EPOCH_JD = 588465.5; // Feb 18, 3102 BC
    static constexpr double SHAKA_EPOCH_JD = 1749994.5; // Mar 22, 79 AD
    static constexpr double VIKRAM_EPOCH_JD = 1593829.5; // Chaitra 1, 57 BC

    // Nakshatra and star data
    struct NakshatraInfo {
        std::string name;
        std::string sanskrit;
        Planet lord;
        std::string symbol;
        std::string deity;
        double startDegree;
        double endDegree;
        std::string nature; // Dev, Manush, Rakshasa
        std::string gana;
        std::string quality;
    };

    struct TithiInfo {
        std::string name;
        std::string sanskrit;
        Planet lord;
        std::string nature;
        bool isShubha; // auspicious
        std::string significance;
    };

    struct YogaInfo {
        std::string name;
        std::string sanskrit;
        std::string nature;
        std::string effect;
    };

    struct KaranaInfo {
        std::string name;
        std::string sanskrit;
        std::string nature;
        bool isMovable;
    };

    // Data tables
    std::vector<NakshatraInfo> nakshatraData;
    std::vector<TithiInfo> tithiData;
    std::vector<YogaInfo> yogaData;
    std::vector<KaranaInfo> karanaData;

    // Festival and special event data
    std::map<std::string, std::vector<std::string>> festivalMap;

    // Calculation configuration
    AyanamsaType ayanamsa;
    CalculationMethod calculationMethod;
    CalendarSystem calendarSystem;
    bool useModernCalculations;

    // Initialization
    void initializeNakshatraData();
    void initializeTithiData();
    void initializeYogaData();
    void initializeKaranaData();
    void initializeFestivalData();

    // Calculation methods
    double calculateLunarPhase(double sunLong, double moonLong) const;
    Tithi calculateTithi(double lunarPhase) const;
    HinduNakshatra calculateNakshatra(double moonLongitude) const;
    Yoga calculateYoga(double sunLong, double moonLong) const;
    Karana calculateKarana(double lunarPhase, bool isFirstHalf) const;
    Vara calculateVara(double julianDay) const;
    HinduMonth calculateHinduMonth(double sunLongitude) const;
    Rashi calculateRashi(double longitude) const;

    // Timing calculations
    double calculateTithiEndTime(double currentPhase, double sunSpeed, double moonSpeed) const;
    double calculateNakshatraEndTime(double moonLong, double moonSpeed) const;
    double calculateYogaEndTime(double sunLong, double moonLong, double sunSpeed, double moonSpeed) const;
    double calculateKaranaEndTime(double currentPhase, double sunSpeed, double moonSpeed) const;

    // Year calculations
    int calculateVikramYear(double julianDay) const;
    int calculateShakaYear(double julianDay) const;
    int calculateKaliYear(double julianDay) const;

    // Date conversion utilities
    double hinduDateToJulianDay(int year, int month, int day, bool isKrishna = false) const;
    void julianDayToHinduDate(double jd, int& year, int& month, int& day, bool& isKrishna) const;
    void julianDayToGregorianDate(double jd, int& year, int& month, int& day) const;

    // Ayanamsa and calculation setup
    void setAyanamsa(AyanamsaType type);
    void setCalculationMethod(CalculationMethod method);
    void setCalendarSystem(CalendarSystem system);
    double getAyanamsaValue(double julianDay) const;
    int getSweAyanamsaId() const;

    // Festival identification
    void identifyFestivals(PanchangaData& panchanga) const;
    void identifySpecialEvents(PanchangaData& panchanga) const;

    // Muhurta calculations
    void calculateMuhurta(PanchangaData& panchanga) const;
    void calculateSunMoonTimes(PanchangaData& panchanga, double latitude, double longitude) const;
    void calculateRahuKaal(PanchangaData& panchanga) const;
    void calculateYamaganda(PanchangaData& panchanga) const;
    void calculateGulikai(PanchangaData& panchanga) const;
    void calculateDurMuhurtam(PanchangaData& panchanga) const;
    void calculateVarjyam(PanchangaData& panchanga) const;
    void calculateSpecialYogas(PanchangaData& panchanga) const;
    void calculateNakshatraPada(PanchangaData& panchanga) const;
    void calculateChandraTaraBalam(PanchangaData& panchanga) const;
    void calculateRituAyana(PanchangaData& panchanga) const;
    void calculateShoolDirections(PanchangaData& panchanga) const;
    void calculateVarnaInformation(PanchangaData& panchanga) const;  // New method for Savarna calculation
    void identifyVrataUpavas(PanchangaData& panchanga) const;

    // Swiss Ephemeris enhanced calculation support functions
    void getSeasonalAtmosphericParams(int month, double latitude, double longitude,
                                     double* pressure, double* temperature) const;
    void handleCalculationError(int result, const char* errorString, PanchangaData& panchanga,
                               const std::string& calculation, double latitude) const;
    void handlePolarConditions(PanchangaData& panchanga, const std::string& calculation,
                              double latitude) const;
    void setReasonableFallback(PanchangaData& panchanga, const std::string& calculation) const;
    void handleException(const std::exception& e, PanchangaData& panchanga) const;

    // Helper methods for time calculations
    double calculateBrahmaMuhurta(double sunriseTime, bool isStart) const;
    double calculateAbhijitMuhurta(double sunriseTime, double sunsetTime, bool isStart) const;
    double calculateGodhuliBela(double sunsetTime, bool isStart) const;
    double calculateNishitaMuhurta(double sunsetTime, double nextSunriseTime, bool isStart) const;

    bool initialized;
    mutable std::string lastError;

public:
    HinduCalendar();
    HinduCalendar(AyanamsaType ayanamsa, CalculationMethod method = CalculationMethod::DRIK_SIDDHANTA,
                  CalendarSystem system = CalendarSystem::LUNI_SOLAR);
    ~HinduCalendar();

    // Initialize the calendar system
    bool initialize();
    bool initialize(AyanamsaType ayanamsa, CalculationMethod method = CalculationMethod::DRIK_SIDDHANTA);

    // Main calculation method
    PanchangaData calculatePanchanga(const BirthData& birthData) const;
    PanchangaData calculatePanchanga(double julianDay, double latitude, double longitude) const;

    // Bulk calculations
    std::vector<PanchangaData> calculatePanchangaRange(const std::string& fromDate,
                                                       const std::string& toDate,
                                                       double latitude, double longitude) const;

    // Specific element calculations
    Tithi getTithi(double julianDay) const;
    HinduNakshatra getNakshatra(double julianDay) const;
    Yoga getYoga(double julianDay) const;
    Karana getKarana(double julianDay) const;

    // Festival and event queries
    std::vector<std::string> getFestivalsForDate(const PanchangaData& panchanga) const;
    bool isEkadashi(const PanchangaData& panchanga) const;
    bool isPurnima(const PanchangaData& panchanga) const;
    bool isAmavasya(const PanchangaData& panchanga) const;

    // Muhurta calculations
    bool isShubhaMuhurta(const PanchangaData& panchanga) const;
    std::string getMuhurtaDescription(const PanchangaData& panchanga) const;

    // Output formatting
    std::string generatePanchangaTable(const PanchangaData& panchanga) const;
    std::string generatePanchangaTable(const std::vector<PanchangaData>& panchangaList) const;
    std::string generatePanchangaTableFormat(const std::vector<PanchangaData>& panchangaList,
                                           const std::vector<double>& julianDays) const;
    std::string generateJSON(const PanchangaData& panchanga) const;
    std::string generateCSV(const std::vector<PanchangaData>& panchangaList) const;

    // Utility methods
    std::string getNakshatraName(HinduNakshatra nak) const;
    std::string getTithiName(Tithi tithi) const;
    std::string getYogaName(Yoga yoga) const;
    std::string getKaranaName(Karana karana) const;
    std::string getVaraName(Vara vara) const;
    std::string getHinduMonthName(HinduMonth month) const;
    std::string getRashiName(Rashi rashi) const;

    // Date conversion functions
    std::string hinduDateToGregorian(int hinduYear, int hinduMonth, int hinduDay, bool isKrishna = false) const;
    std::string gregorianDateToHindu(int gregYear, int gregMonth, int gregDay) const;
    double gregorianDateToJulianDay(int year, int month, int day, double hour = 12.0) const;

    // Configuration getters/setters
    AyanamsaType getAyanamsa() const { return ayanamsa; }
    CalculationMethod getCalculationMethod() const { return calculationMethod; }
    CalendarSystem getCalendarSystem() const { return calendarSystem; }
    std::string getAyanamsaName() const;
    std::string getCalculationMethodName() const;

    // Error handling
    std::string getLastError() const { return lastError; }
    bool isInitialized() const { return initialized; }

    // Logic mode for combining search criteria
    enum class LogicMode {
        AND = 0,    // All criteria must match
        OR = 1      // Any criteria can match
    };

    // Search functionality
    struct SearchCriteria {
        // Year criteria
        int exactYear = -1;           // Search for specific year (-1 = ignore)
        int yearRangeStart = -1;      // Year range start (-1 = ignore)
        int yearRangeEnd = -1;        // Year range end (-1 = ignore)

        // Month criteria
        int exactMonth = -1;          // Search for specific month (1-12, -1 = ignore)
        int monthRangeStart = -1;     // Month range start (1-12, -1 = ignore)
        int monthRangeEnd = -1;       // Month range end (1-12, -1 = ignore)

        // Tithi criteria
        int exactTithi = -1;          // Search for specific tithi (1-30, -1 = ignore)
        int tithiRangeStart = -1;     // Tithi range start (1-30, -1 = ignore)
        int tithiRangeEnd = -1;       // Tithi range end (1-30, -1 = ignore)

        // Weekday criteria
        int exactWeekday = -1;        // Search for specific weekday (0=Sun, 6=Sat, -1 = ignore)

        // Nakshatra criteria
        int exactNakshatra = -1;      // Search for specific nakshatra (1-27, -1 = ignore)
        int nakshatraRangeStart = -1; // Nakshatra range start (1-27, -1 = ignore)
        int nakshatraRangeEnd = -1;   // Nakshatra range end (1-27, -1 = ignore)

        // Yoga criteria
        int exactYoga = -1;           // Search for specific yoga (1-27, -1 = ignore)
        int yogaRangeStart = -1;      // Yoga range start (1-27, -1 = ignore)
        int yogaRangeEnd = -1;        // Yoga range end (1-27, -1 = ignore)

        // Special day criteria
        bool searchEkadashi = false;  // Search for Ekadashi days
        bool searchPurnima = false;   // Search for Purnima (full moon) days
        bool searchAmavasya = false;  // Search for Amavasya (new moon) days
        bool searchSankranti = false; // Search for Sankranti days

        // Julian Day criteria
        double exactJulianDay = -1.0;      // Search for specific Julian Day (-1 = ignore)
        double julianDayRangeStart = -1.0; // Julian Day range start (-1 = ignore)
        double julianDayRangeEnd = -1.0;   // Julian Day range end (-1 = ignore)
        double julianDayTolerance = 0.5;   // Tolerance for Julian Day matching (default 0.5 days)

        // Varna (Savarna) criteria
        std::string exactVarnaDay = "";        // Search for specific day Varna ("Brahmin", "Kshatriya", "Vaishya", "Shudra", "" = ignore)
        std::string exactVarnaTithi = "";      // Search for specific tithi Varna ("" = ignore)
        std::string exactVarnaNakshatra = "";  // Search for specific nakshatra Varna ("" = ignore)
        bool searchBrahminDays = false;        // Search for Brahmin Varna days
        bool searchKshatriyaDays = false;      // Search for Kshatriya Varna days
        bool searchVaishyaDays = false;        // Search for Vaishya Varna days
        bool searchShudradays = false;         // Search for Shudra Varna days

        // Logical combination mode
        LogicMode logicMode = LogicMode::AND; // How to combine multiple criteria

        // Search options
        bool exactMatch = true;       // true = exact match, false = near match
        int nearMatchTolerance = 1;   // For near match, tolerance in days

        // Date range for search
        std::string searchStartDate;  // Start date for search (YYYY-MM-DD)
        std::string searchEndDate;    // End date for search (YYYY-MM-DD)
    };

    struct SearchResult {
        std::string gregorianDate;    // Gregorian date (YYYY-MM-DD)
        PanchangaData panchangaData;  // Full panchanga data for this date
        double julianDay;             // Julian day number
        int weekday;                  // Weekday (0=Sunday, 6=Saturday)
        double matchScore;            // Match score (1.0 = perfect match, 0.0 = no match)
        std::string matchDescription; // Description of what matched
    };

    // Search methods
    std::vector<SearchResult> searchHinduCalendar(const SearchCriteria& criteria, double latitude, double longitude) const;
    std::vector<SearchResult> searchByTithi(int tithi, const std::string& startDate, const std::string& endDate, double latitude, double longitude, bool exactMatch = true) const;
    std::vector<SearchResult> searchByWeekday(int weekday, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchByMonth(int month, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchByYear(int year, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;

    // Multi-criteria search helper methods
    std::vector<SearchResult> searchEkadashiAndPurnima(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchPurnimaOrAmavasya(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchByNakshatra(int nakshatra, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchMultiCriteria(const SearchCriteria& criteria, LogicMode logicMode, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;

    // Julian Day search methods
    std::vector<SearchResult> searchByJulianDay(double julianDay, const std::string& startDate, const std::string& endDate, double latitude, double longitude, double tolerance = 0.5) const;
    std::vector<SearchResult> searchByJulianDayRange(double jdStart, double jdEnd, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;

    // Simplified JD search methods (no date range required)
    SearchResult searchJulianDayOnly(double julianDay, double latitude = 0.0, double longitude = 0.0) const;
    PanchangaData calculatePanchangaFromJD(double julianDay, double latitude = 0.0, double longitude = 0.0) const;

    // Varna (Savarna) search methods
    std::vector<SearchResult> searchByVarnaDay(const std::string& varnaType, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchByVarnaTithi(const std::string& varnaType, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchByVarnaNakshatra(const std::string& varnaType, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchBrahminDays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchKshatriyaDays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchVaishyaDays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;
    std::vector<SearchResult> searchShudradays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const;

    // Enhanced Sankranti calculation methods
    double calculateSankrantiTime(double julianDay, Rashi currentRashi, Rashi nextRashi) const;
    std::string calculateNextSankranti(double julianDay, double& daysUntil) const;
    bool isSankrantiOccurringToday(double julianDay, double tolerance = 0.5) const;
    std::vector<std::pair<double, std::string>> getSankrantiTimesForMonth(int year, int month) const;
    std::vector<std::pair<double, std::string>> getSankrantiTimesForYear(int year) const;

    // Sankranti search and analysis methods
    std::vector<SearchResult> searchSankranti(const std::string& startDate, const std::string& endDate,
                                             double latitude = 0.0, double longitude = 0.0) const;
    std::vector<SearchResult> searchSpecificSankranti(Rashi rashi, const std::string& startDate,
                                                     const std::string& endDate, double latitude = 0.0,
                                                     double longitude = 0.0) const;

    // Advanced astronomical calculation methods
    struct CoordinateSet {
        struct {
            double rightAscension;     // RA in degrees
            double declination;        // Dec in degrees
            double distance;           // Distance in AU
        } astrometric;

        struct {
            double rightAscension;     // Apparent RA
            double declination;        // Apparent Dec
            double eclipticLongitude;  // Ecliptic longitude
            double eclipticLatitude;   // Ecliptic latitude
        } apparent;

        struct {
            double rightAscension;     // Topocentric RA
            double declination;        // Topocentric Dec
            double azimuth;            // Azimuth (0°=N, 90°=E)
            double elevation;          // Elevation above horizon
            double hourAngle;          // Local hour angle
        } topocentric;
    };

    struct AtmosphericModel {
        double pressure;           // Atmospheric pressure (mbar)
        double temperature;        // Temperature (Celsius)
        double humidity;           // Relative humidity (0-1)
        double wavelength;         // Light wavelength (micrometers)
        double lapseRate;          // Temperature lapse rate
    };

    struct PolarConditions {
        bool isPolarNight;         // Sun never rises
        bool isPolarDay;           // Sun never sets
        bool isExtendedTwilight;   // Extended twilight period
        double continuousDays;     // Days of continuous condition
        std::string description;   // Human-readable description
    };

    struct RiseSetEvent {
        std::string objectName;
        std::string eventType;    // "rise", "set", "culmination"
        double julianDay;
        double localTime;
        double azimuth;
        double elevation;
        bool isValid;
        CoordinateSet coordinates;
        std::string notes;
    };

    struct HorizonData {
        double geometricHorizon;   // Pure geometric horizon
        double apparentHorizon;    // With refraction
        double nauticalHorizon;    // Nautical definition
        double astronomicalHorizon; // Astronomical definition
    };

    // Enhanced calculation methods
    CoordinateSet calculateAllCoordinates(int body, double julianDay,
                                        double latitude, double longitude,
                                        double elevation = 0.0) const;

    AtmosphericModel getSeasonalAtmosphere(double julianDay, double latitude, double longitude) const;

    PolarConditions detectPolarConditions(double latitude, double declination, double julianDay) const;

    std::vector<RiseSetEvent> calculateAllEvents(double jdStart, double latitude,
                                               double longitude, double timezone,
                                               double elevation = 0.0) const;

    HorizonData calculateHorizon(double observerHeight, double temperature = 15.0,
                               double pressure = 1013.25) const;

    double calculateCustomHorizon(double observerElevation, double targetElevation = 0.0) const;

    double calculateRefraction(double elevation, const AtmosphericModel& atm) const;

    RiseSetEvent calculatePreciseRiseSet(int body, double jdStart,
                                       double latitude, double longitude,
                                       double elevation, double timezone) const;

    double calculateCulminationTime(int body, double julianDay, double latitude) const;

    void handlePolarRiseSet(PanchangaData& panchanga, double latitude,
                          const PolarConditions& polar) const;

    double getDeltaT(double julianDay) const;
    int getLeapSeconds(double jdUtc) const;
    double utcToTdb(double jdUtc) const;

private:
    // Utility method for parsing dates
    bool parseDate(const std::string& dateStr, int& year, int& month, int& day) const;

    // Advanced calculation helper methods
    void addSolarEvents(std::vector<RiseSetEvent>& events, double jdStart,
                       double latitude, double longitude, double timezone, double elevation) const;
    void addLunarEvents(std::vector<RiseSetEvent>& events, double jdStart,
                       double latitude, double longitude, double timezone, double elevation) const;
    RiseSetEvent findRiseEvent(int body, double jdStart, double latitude,
                              double longitude, double timezone, double elevation) const;
    RiseSetEvent findSetEvent(int body, double jdStart, double latitude,
                             double longitude, double timezone, double elevation) const;
    RiseSetEvent findCulminationEvent(int body, double jdStart, double latitude,
                                     double longitude, double timezone) const;

    double calculateGeometricHorizon(double height) const;
    double calculateRefractionCorrection(double temperature, double pressure) const;
    double calculateLunarParallax(double latitude, double elevation) const;
    double getCurrentDeclination(int body, double julianDay) const;
    double calculatePolarDuration(double latitude, double declination, double julianDay) const;
    void calculateWithExtendedSearch(PanchangaData& panchanga, double latitude, double searchHours) const;
};

// Utility functions
std::string formatHinduDate(int day, HinduMonth month, int year, bool isKrishna);
double julianDayToHinduDate(double jd, int& year, int& month, int& day);
double hinduDateToJulianDay(int year, int month, int day);
bool isLeapYear(int hinduYear);

} // namespace Astro
