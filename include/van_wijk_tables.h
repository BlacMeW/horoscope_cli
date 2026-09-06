#pragma once

#include <string>
#include <vector>
#include <map>
#include <cmath>

namespace Astro {

/// Single entry in Table I (Century Constants - Page 47 of Van Wijk 1938).
struct VanWijkCenturyEntry {
    int centuryKY;       // Expired years of Kali Yuga (0000, 1000..5800)
    int adYear;          // Julian AD/BC Year equivalent
    double base;         // B: Julian date of true Mina-Samkranti
    double elongationC;  // C: Distance of first mean New Moon from base
    double sunAnomalyD;  // D: Mean anomaly of the Sun (days)
    double moonAnomalyE; // E: Mean anomaly of the Moon without bija (days)
    double bija;         // F: Bija additive correction (days)
};

/// Single entry in Table II (Single Years 00 to 99 - Page 49 of Van Wijk 1938).
struct VanWijkYearEntry {
    int yearOffset;          // 00 to 99
    double baseDelta;        // B: Base addition
    double elongationDelta;  // C: Distance of first mean New Moon addition
    double sunAnomalyDelta;  // D: Sun Anomaly addition (0.0)
    double moonAnomalyDelta; // E: Moon Anomaly addition
};

/// Single entry in Table III (12 Lunations / Masas - Page 50 of Van Wijk 1938).
struct VanWijkLunationEntry {
    int masaIndex;           // 1 = Caitra .. 12 = Phalguna
    std::string sanskritName;
    std::string myanmarName; // English transliterated equivalent (e.g. Tagu, Kason)
    double trueSamkranti;    // Section A: True Samkranti distance from Base
    double meanSamkranti;    // Section A: Mean Samkranti distance from Base
    double distanceG;        // Section G: Multiples of synodic period
    double sunAnomalyG;
    double moonAnomalyG;     // Section H: Multiples of anomalistic period
};

/// Single entry in Table IV Section B (30 Tithis & 60 Karanas - Page 51 of Van Wijk 1938).
struct VanWijkTithiEntry {
    int tithiNumber;         // 1 to 30 (1..15 Sukla, 16..30 Krishna)
    std::string paksha;      // Sukla or Krishna
    int pakshaDay;           // 1 to 15
    std::string tithiName;
    double distanceT;        // Distance in days from New Moon
    double sunAnomalyT;
    double moonAnomalyT;
    std::string karana1;
    double karana1Distance;
    std::string karana2;
    double karana2Distance;
};

/// Historical record from Table A (First Auxiliary Table Section A - Page 46 of Van Wijk 1938).
struct VanWijkTableAEntry {
    int kaliYugaExpired;
    int sakaExpired;
    int adYear;
    std::string intercalaryMonth1; // First Adhika
    std::string suppressedMonth;   // Kshayamasa
    std::string intercalaryMonth2; // Second Adhika (if any)
    std::string historicalNote;
};

/// Comprehensive result of reducing a Hindu Date to a Western Date.
struct VanWijkReductionResult {
    int kaliYugaYear = 0;
    int sakaYear = 0;
    int vikramaYear = 0;
    int masaIndex = 1;
    std::string masaName;
    std::string paksha;
    int tithiInPaksha = 1;
    int absoluteTithi = 1;
    std::string weekdayName;

    double tableIBase = 0.0;
    double tableIIBase = 0.0;
    double julianBase = 0.0;
    double tableIElongation = 0.0;
    double tableIIElongation = 0.0;
    double meanElongationC = 0.0;
    double sunAnomalyBaseD = 0.0;
    double moonAnomalyBaseE = 0.0;

    double lunationDistanceG = 0.0;
    double lunationSunAnomaly = 0.0;
    double lunationMoonAnomaly = 0.0;
    double tithiDistanceT = 0.0;
    double tithiSunAnomaly = 0.0;
    double tithiMoonAnomaly = 0.0;

    double meanBeginningDaysFromBase = 0.0;
    double totalSunAnomaly = 0.0;
    double totalMoonAnomaly = 0.0;
    double sunEquationDays = 0.0;
    double moonEquationDays = 0.0;
    double totalEquationCorrectionDeltaT = 0.0;

    double trueBeginningDaysFromBase = 0.0;
    double julianDayEquivalent = 0.0;

    int adYear = 0;
    bool isLeapYear = false;
    int adMonth = 1;
    std::string adMonthName;
    int adDay = 1;
    double dayFraction = 0.0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    std::string weekdayCalculated;
    bool isWeekdayMatching = false;
    std::string calendarType;
};

/// Comprehensive result of converting a Western Date to a Hindu Date.
struct VanWijkWesternToHinduResult {
    int adYear = 0;
    int adMonth = 1;
    int adDay = 1;
    double dayFraction = 0.0;
    double julianDayNumber = 0.0;
    double kaliAhargana = 0.0;
    int kaliYugaExpired = 0;
    int sakaYear = 0;
    int vikramaYear = 0;
    int lunarMasaIndex = 1;
    std::string lunarMasaName;
    std::string paksha;
    int tithiInPaksha = 1;
    std::string tithiName;
    double tithiProgressFrac = 0.0;
    std::string nakshatraName;
    double nakshatraDeg = 0.0;
    std::string yogaName;
    std::string karanaName;
    std::string weekdayName;
    bool isAdhimasa = false;
    bool isKshayamasa = false;
};

/// Master Repository of Van Wijk's Decimal Tables and Surya Siddhanta Constants.
class VanWijkDecimalTables {
public:
    // Fundamental astronomical constants (Surya Siddhanta)
    static constexpr double siderealYearDays = 365.2587564;
    static constexpr double synodicMonthDays = 29.5305879;
    static constexpr double anomalisticMonthDays = 27.554551;
    static constexpr double kaliYugaEpochJD = 588465.5; // Feb 18, 3102 BCE (Midnight Lanka)

    // Table Accessors
    static const std::vector<VanWijkCenturyEntry>& getTableI();
    static VanWijkCenturyEntry getCenturyEntry(int centuryKY);

    static const std::vector<VanWijkYearEntry>& getTableII();
    static VanWijkYearEntry getTableIIEntry(int yearOffset);

    static const std::vector<VanWijkLunationEntry>& getTableIII();
    static const std::vector<VanWijkTithiEntry>& getTableIV();
    static const std::vector<VanWijkTableAEntry>& getTableA();

    // Equation of the Centre Interpolators
    static double getSunEquationDays(double anom);
    static double getMoonEquationDays(double anom);

    // Standard Names
    static const std::vector<std::string>& getWesternMonthNames();
    static const std::vector<std::string>& getWeekdayNames();
    static const std::vector<std::string>& getNakshatraNames();
    static const std::vector<std::string>& getYogaNames();
    static const std::vector<std::string>& getKaranaNames();
};

/// Core Calculation Engine for Date Reductions and Conversions.
class VanWijkReductionEngine {
public:
    /// Reduces a Hindu Luni-Solar Date to a Western Calendar Date using Van Wijk's 1938 Algorithm.
    static VanWijkReductionResult reduceHinduDate(
        int kaliYugaExpired,
        int masaIndex,      // 1 = Caitra .. 12 = Phalguna
        int pakshaIndex,    // 0 = Sukla (1..15), 1 = Krishna (16..30)
        int tithiInPaksha,  // 1..15
        int expectedWeekday = -1 // 0 = Monday .. 6 = Sunday (-1 for ignore)
    );

    /// Converts a Western Gregorian / Julian Calendar Date to full Hindu Panchanga & Ahargana.
    static VanWijkWesternToHinduResult convertWesternToHindu(
        int year,
        int month,
        int day,
        double hour = 6.0 // Default 6:00 AM Lanka Sunrise
    );

    /// Formats the reduction result into a clean, formatted ASCII text report.
    static std::string formatReductionReport(const VanWijkReductionResult& res);

    /// Formats the Western -> Hindu result into a clean, formatted ASCII text report.
    static std::string formatWesternToHinduReport(const VanWijkWesternToHinduResult& res);
};

} // namespace Astro
