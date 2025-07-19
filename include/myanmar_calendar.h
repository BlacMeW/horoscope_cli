#pragma once

#include "astro_types.h"
#include <string>
#include <vector>
#include <map>

namespace Astro {

// Myanmar calendar elements enumeration
enum class MyanmarYearType {
    COMMON = 0,      // Common year (354 days)
    LITTLE_WATAT = 1, // Little watat (384 days)
    BIG_WATAT = 2     // Big watat (385 days)
};

enum class MyanmarMonth {
    FIRST_WASO = 0,   // 1st Waso (intercalary month)
    TAGU = 1,         // Tagu
    KASON = 2,        // Kason
    NAYON = 3,        // Nayon
    WASO = 4,         // (2nd) Waso
    WAGAUNG = 5,      // Wagaung
    TAWTHALIN = 6,    // Tawthalin
    THADINGYUT = 7,   // Thadingyut
    TAZAUNGMON = 8,   // Tazaungmon
    NADAW = 9,        // Nadaw
    PYATHO = 10,      // Pyatho
    TABODWE = 11,     // Tabodwe
    TABAUNG = 12,     // Tabaung
    LATE_TAGU = 13,   // Late Tagu
    LATE_KASON = 14   // Late Kason
};

enum class MyanmarMoonPhase {
    WAXING = 0,      // Waxing moon
    FULL_MOON = 1,   // Full moon
    WANING = 2,      // Waning moon
    NEW_MOON = 3     // New moon
};

enum class MyanmarWeekday {
    SATURDAY = 0,
    SUNDAY = 1,
    MONDAY = 2,
    TUESDAY = 3,
    WEDNESDAY = 4,
    THURSDAY = 5,
    FRIDAY = 6
};

enum class Mahabote {
    BINGA = 0,     // Binga
    ATUN = 1,      // Atun
    YAZA = 2,      // Yaza
    ADIPATI = 3,   // Adipati
    MARANA = 4,    // Marana
    THIKE = 5,     // Thike
    PUTI = 6       // Puti
};

enum class Nakhat {
    ORC = 0,       // Orc
    ELF = 1,       // Elf
    HUMAN = 2      // Human
};

enum class NagahleDirection {
    WEST = 0,      // West
    NORTH = 1,     // North
    EAST = 2,      // East
    SOUTH = 3      // South
};

// Myanmar calendar data structure
struct MyanmarCalendarData {
    // Basic date information
    long myanmarYear;              // Myanmar Era year
    long sasanaYear;               // Buddhist Era year (Myanmar Year + 1182)
    MyanmarMonth month;            // Myanmar month
    long dayOfMonth;               // Day of month [1-30]
    MyanmarYearType yearType;      // Year type (common, little watat, big watat)

    // Moon phase information
    MyanmarMoonPhase moonPhase;    // Current moon phase
    long fortnightDay;             // Day within fortnight [1-15]
    long monthLength;              // Length of current month [29 or 30]

    // Weekday and astrological information
    MyanmarWeekday weekday;        // Day of week
    Mahabote mahabote;             // Mahabote (7-day cycle)
    Nakhat nakhat;                 // Nakhat (3-year cycle)
    NagahleDirection nagahle;      // Nagahle direction

    // Astrological days and events
    bool isSabbath;                // Buddhist sabbath day
    bool isSabbathEve;            // Sabbath eve
    bool isYatyaza;               // Yatyaza (inauspicious)
    bool isPyathada;              // Pyathada (inauspicious)
    bool isAfternoonPyathada;     // Afternoon pyathada
    bool isThamanyo;              // Thamanyo (auspicious)
    bool isAmyeittasote;          // Amyeittasote
    bool isWarameittugyi;         // Warameittugyi
    bool isWarameittunge;         // Warameittunge
    bool isYatpote;               // Yatpote
    bool isThamaphyu;             // Thamaphyu
    bool isNagapor;               // Nagapor
    bool isYatyotema;             // Yatyotema
    bool isMahayatkyan;           // Mahayatkyan
    bool isShanyat;               // Shanyat

    // Festival and holiday information
    std::vector<std::string> festivals;
    std::vector<std::string> holidays;
    std::vector<std::string> astrologicalEvents;

    // Julian day information
    double julianDay;              // Julian day number
    long fullMoonJulianDay;       // Full moon day for watat calculation

    // Formatting methods
    std::string getFormattedDate() const;
    std::string getFormattedMoonPhase() const;
    std::string getAstrologicalSummary() const;
    std::string getFullDescription() const;
};

// Main Myanmar Calendar System
class MyanmarCalendar {
private:
    // Constants for calculations
    static constexpr double SOLAR_YEAR = 365.2587565;        // Solar year in days
    static constexpr double LUNAR_MONTH = 29.53058795;       // Lunar month in days
    static constexpr double MYANMAR_EPOCH = 1954168.050623;  // Beginning of 0 ME for MMT

    // Era data structures
    struct EraData {
        double eraId;          // Calendar era ID [1-3]
        double watatOffset;    // Watat offset
        double monthNumber;    // Number of months for excess days
        long exceptionWatat;   // Exception in watat year
    };

    // Month information structure
    struct MonthInfo {
        std::string englishName;
        std::string myanmarName;
        std::string description;
        std::vector<std::string> festivals;
        bool hasSpecialEvents;
    };

    // Astrological day information
    struct AstrologicalDayInfo {
        std::string name;
        std::string description;
        bool isAuspicious;
        std::string recommendation;
    };

    // Data tables and maps
    std::vector<MonthInfo> monthData;
    std::map<std::string, AstrologicalDayInfo> astrologicalDays;
    std::map<std::string, std::vector<std::string>> festivalMap;

    // Exception arrays (from original Myanmar calendar implementation)
    std::vector<std::array<long, 2>> fullMoonOffsetExceptions;
    std::vector<long> watatExceptions;

    // Initialization methods
    void initializeMonthData();
    void initializeAstrologicalDays();
    void initializeFestivalData();
    void initializeExceptionData();

    // Core calculation methods (adapted from cemmdatetime)
    void getMyanmarConstants(long myanmarYear, double& eraId, double& watatOffset, 
                           double& monthNumber, long& exceptionWatat) const;
    void calculateWatat(long myanmarYear, long& watat, long& fullMoonDay) const;
    void calculateMyanmarYear(long myanmarYear, long& yearType, long& tagu1, 
                            long& fullMoonDay, long& watatError) const;
    void julianToMyanmarDate(double julianDay, long& yearType, long& myanmarYear,
                           long& month, long& dayOfMonth) const;
    long myanmarToJulianDay(long myanmarYear, long month, long dayOfMonth) const;

    // Julian Day Number conversion utilities (from reference implementation)
    double gregorianToJdn(int year, int month, int day) const;
    std::vector<int> jdnToGregorian(double jd) const;

    // Utility calculation methods
    long calculateMoonPhase(long dayOfMonth, long month, long yearType) const;
    long calculateMonthLength(long month, long yearType) const;
    long calculateYearLength(long yearType) const;
    long calculateFortnightDay(long dayOfMonth) const;
    long calculateDayOfMonth(long fortnightDay, long moonPhase, long month, long yearType) const;

    // Astrological calculations
    long calculateSabbath(long dayOfMonth, long month, long yearType) const;
    long calculateYatyaza(long month, long weekday) const;
    long calculatePyathada(long month, long weekday) const;
    long calculateNagahle(long month) const;
    long calculateMahabote(long myanmarYear, long weekday) const;
    long calculateNakhat(long myanmarYear) const;
    long calculateThamanyo(long month, long weekday) const;
    long calculateAmyeittasote(long dayOfMonth, long weekday) const;
    long calculateWarameittugyi(long dayOfMonth, long weekday) const;
    long calculateWarameittunge(long dayOfMonth, long weekday) const;
    long calculateYatpote(long dayOfMonth, long weekday) const;
    long calculateThamaphyu(long dayOfMonth, long weekday) const;
    long calculateNagapor(long dayOfMonth, long weekday) const;
    long calculateYatyotema(long month, long dayOfMonth) const;
    long calculateMahayatkyan(long month, long dayOfMonth) const;
    long calculateShanyat(long month, long dayOfMonth) const;

    // Binary search methods (from original implementation)
    long binarySearch2D(long key, const std::vector<std::array<long, 2>>& array) const;
    long binarySearch1D(long key, const std::vector<long>& array) const;

    // Festival and holiday identification
    void identifyFestivals(MyanmarCalendarData& data) const;
    void identifyHolidays(MyanmarCalendarData& data) const;
    void identifyAstrologicalEvents(MyanmarCalendarData& data) const;

    // Date of Easter calculation (for Christian holidays in Myanmar)
    long calculateEaster(long year) const;

    bool initialized;
    mutable std::string lastError;

public:
    MyanmarCalendar();
    ~MyanmarCalendar();

    // Initialize the calendar system
    bool initialize();

    // Main calculation methods
    MyanmarCalendarData calculateMyanmarDate(const BirthData& birthData) const;
    MyanmarCalendarData calculateMyanmarDate(double julianDay) const;

    // Bulk calculations
    std::vector<MyanmarCalendarData> calculateMyanmarDateRange(const std::string& fromDate,
                                                              const std::string& toDate) const;

    // Specific element calculations
    MyanmarYearType getYearType(long myanmarYear) const;
    MyanmarMonth getCurrentMonth(double julianDay) const;
    MyanmarMoonPhase getMoonPhase(double julianDay) const;
    long getSasanaYear(long myanmarYear) const { return myanmarYear + 1182; }

    // Astrological queries
    bool isSabbathDay(const MyanmarCalendarData& data) const;
    bool isAuspiciousDay(const MyanmarCalendarData& data) const;
    bool isInauspiciousDay(const MyanmarCalendarData& data) const;
    std::vector<std::string> getAstrologicalEvents(const MyanmarCalendarData& data) const;

    // Festival and holiday queries
    std::vector<std::string> getFestivalsForDate(const MyanmarCalendarData& data) const;
    std::vector<std::string> getHolidaysForDate(const MyanmarCalendarData& data) const;

    // Output formatting
    std::string generateMyanmarCalendarTable(const MyanmarCalendarData& data) const;
    std::string generateMyanmarCalendarTable(const std::vector<MyanmarCalendarData>& dataList) const;
    std::string generateJSON(const MyanmarCalendarData& data) const;
    std::string generateCSV(const std::vector<MyanmarCalendarData>& dataList) const;

    // Utility methods
    std::string getMyanmarMonthName(MyanmarMonth month) const;
    std::string getMyanmarWeekdayName(MyanmarWeekday weekday) const;
    std::string getMahaboteName(Mahabote mahabote) const;
    std::string getNakhatName(Nakhat nakhat) const;
    std::string getNagahleDirectionName(NagahleDirection direction) const;
    std::string getMoonPhaseName(MyanmarMoonPhase phase) const;
    std::string getYearTypeName(MyanmarYearType type) const;

    // Date conversion utilities
    std::string formatMyanmarDate(const MyanmarCalendarData& data, const std::string& format = "&y &M &P &ff") const;
    bool parseDate(const std::string& dateStr, int& year, int& month, int& day) const;

    // Error handling
    std::string getLastError() const { return lastError; }
    bool isInitialized() const { return initialized; }
};

// Utility functions
std::string formatMyanmarDateString(long year, MyanmarMonth month, long day, MyanmarMoonPhase phase);
double julianDayToMyanmarDate(double jd, long& year, long& month, long& day);
double myanmarDateToJulianDay(long year, long month, long day);
bool isMyanmarLeapYear(long myanmarYear);

} // namespace Astro
