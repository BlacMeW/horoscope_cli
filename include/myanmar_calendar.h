#pragma once

#include "astro_types.h"
#include <string>
#include <vector>
#include <array>
#include <map>

namespace Astro {

// Myanmar calendar elements enumeration (based on yan9a/mmcal)
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
    SATURDAY = 0,    // yan9a/mmcal: 0=sat
    SUNDAY = 1,      // yan9a/mmcal: 1=sun
    MONDAY = 2,      // yan9a/mmcal: 2=mon
    TUESDAY = 3,     // yan9a/mmcal: 3=tue
    WEDNESDAY = 4,   // yan9a/mmcal: 4=wed
    THURSDAY = 5,    // yan9a/mmcal: 5=thu
    FRIDAY = 6       // yan9a/mmcal: 6=fri
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

// Main Myanmar Calendar System (Rewritten based on yan9a/mmcal)
class MyanmarCalendar {
private:
    // Astronomical constants from yan9a/mmcal reference implementation
    static constexpr double SOLAR_YEAR = 1577917828.0 / 4320000.0;        // 365.2587565 days
    static constexpr double LUNAR_MONTH = 1577917828.0 / 53433336.0;       // 29.53058795 days
    static constexpr double MYANMAR_EPOCH = 1954168.050623;                // Beginning of 0 ME for MMT

    // Core calculation methods (yan9a/mmcal implementation)
    static void getMyanmarConstants(long my, double& EI, double& WO, double& NM, long& EW);
    static void calculateWatat(long my, long& watat, long& fm);
    static void calculateMyanmarYear(long my, long& myt, long& tg1, long& fm, long& werr);
    static void julianToMyanmar(double jd, long& myt, long& my, long& mm, long& md);
    static long myanmarToJulian(long my, long mm, long md);

    // Calendar property calculations
    static long calculateYearLength(long myt);
    static long calculateMonthLength(long mm, long myt);
    static long calculateFortnightDay(long md);
    static long calculateMoonPhase(long md, long mm, long myt);

    // Weekday and astrological calculations
    static long calculateSabbath(long md, long mm, long myt);
    static long calculateYatyaza(long mm, long wd);
    static long calculatePyathada(long mm, long wd);
    static long calculateThamanyo(long mm, long wd);
    static long calculateAmyeittasote(long md, long wd);
    static long calculateWarameittugyi(long md, long wd);
    static long calculateWarameittunge(long md, long wd);
    static long calculateYatpote(long md, long wd);
    static long calculateThamaphyu(long md, long wd);
    static long calculateNagapor(long md, long wd);
    static long calculateYatyotema(long mm, long md);
    static long calculateMahayatkyan(long mm, long md);
    static long calculateShanyat(long mm, long md);

    // Derived astrological calculations
    static long calculateMahabote(long my, long wd);
    static long calculateNakhat(long my);
    static long calculateNagahle(long mm);

    // Festival and holiday identification
    static void identifyFestivals(MyanmarCalendarData& data);
    static void identifyHolidays(MyanmarCalendarData& data);
    static void identifyAstrologicalEvents(MyanmarCalendarData& data);

    // Internal utility methods
    static long sasanaYear(long my, long mm, long md, long k = 0);
    static long binarySearch1(long key, const std::vector<long>& array);
    static long binarySearch2(long key, const std::vector<std::pair<long, long>>& array);

    bool initialized;
    mutable std::string lastError;

public:
    MyanmarCalendar();
    ~MyanmarCalendar();

    // Initialize the calendar system
    bool initialize();

    // Main calculation method (yan9a/mmcal interface)
    MyanmarCalendarData calculateMyanmarCalendar(const BirthData& birthData) const;
    MyanmarCalendarData calculateMyanmarCalendar(double julianDay) const;

    // Methods for main.cpp compatibility
    MyanmarCalendarData calculateMyanmarDate(const BirthData& birthData) const;
    std::vector<MyanmarCalendarData> calculateMyanmarDateRange(const std::string& fromDate,
                                                              const std::string& toDate) const;
    std::string generateMyanmarCalendarTable(const MyanmarCalendarData& data) const;
    std::string generateMyanmarCalendarTable(const std::vector<MyanmarCalendarData>& dataList) const;

    // Bulk calculations
    std::vector<MyanmarCalendarData> calculateMyanmarCalendarRange(const std::string& fromDate,
                                                                  const std::string& toDate) const;

    // Utility calculations
    MyanmarYearType getYearType(long myanmarYear) const;
    long getSasanaYear(long myanmarYear, long month = 1, long day = 1) const;

    // Output formatting (yan9a/mmcal style)
    std::string generateTable(const MyanmarCalendarData& data) const;
    std::string generateTable(const std::vector<MyanmarCalendarData>& dataList) const;
    std::string generateJSON(const MyanmarCalendarData& data) const;
    std::string generateCSV(const std::vector<MyanmarCalendarData>& dataList) const;
    std::string generateCalendarView(long myanmarYear, long month) const;

    // Myanmar date string formatting (yan9a/mmcal interface)
    std::string formatMyanmarDate(double jd, const std::string& format = "&y &M &P &ff", double tz = 0) const;

    // Name conversion utilities
    std::string getMyanmarMonthName(MyanmarMonth month) const;
    std::string getMyanmarWeekdayName(MyanmarWeekday weekday) const;
    std::string getMahaboteName(Mahabote mahabote) const;
    std::string getNakhatName(Nakhat nakhat) const;
    std::string getNagahleDirectionName(NagahleDirection direction) const;
    std::string getMoonPhaseName(MyanmarMoonPhase phase) const;
    std::string getYearTypeName(MyanmarYearType type) const;

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
