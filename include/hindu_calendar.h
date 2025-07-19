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

// Ayanamsa types for sidereal calculations
enum class AyanamsaType {
    LAHIRI = 1,        // Lahiri/Chitrapaksha (most common)
    RAMAN = 2,         // B.V. Raman
    KRISHNAMURTI = 3,  // K.S. Krishnamurti
    YUKTESHWAR = 4,    // Sri Yukteshwar
    JN_BHASIN = 5,     // J.N. Bhasin
    SASSANIAN = 6,     // Sassanian
    GALACTIC_CENTER = 7, // Galactic Center
    TRUE_CHITRA = 8,   // True Chitra
    TRUE_REVATI = 9,   // True Revati
    TRUE_PUSHYA = 10   // True Pushya
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

    // Special events and festivals
    std::vector<std::string> festivals;
    std::vector<std::string> specialEvents;
    bool isEkadashi;
    bool isPurnima;
    bool isAmavasya;
    bool isSankranti;          // Solar transition

    // Quality assessments
    bool isShubhaMuhurta;      // Auspicious time
    bool isAshubhaMuhurta;     // Inauspicious time
    std::string muhurtaDescription;

    // Formatting methods
    std::string getFormattedTithi() const;
    std::string getFormattedNakshatra() const;
    std::string getFormattedYoga() const;
    std::string getFormattedKarana() const;
    std::string getFullDescription() const;
    std::string getSummary() const;
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

private:
    // Utility method for parsing dates
    bool parseDate(const std::string& dateStr, int& year, int& month, int& day) const;
};

// Utility functions
std::string formatHinduDate(int day, HinduMonth month, int year, bool isKrishna);
double julianDayToHinduDate(double jd, int& year, int& month, int& day);
double hinduDateToJulianDay(int year, int month, int day);
bool isLeapYear(int hinduYear);

} // namespace Astro
