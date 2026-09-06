/////////////////////////////////////////////////////////////////////////////
// File: myanmar_calendar.cpp
// Description: Myanmar Calendar Implementation based on yan9a/mmcal
// Reference: https://github.com/yan9a/mmcal
// Rewritten to match the authoritative yan9a/mmcal algorithms
/////////////////////////////////////////////////////////////////////////////

#include "myanmar_calendar.h"
#include "ephemeris_manager.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <algorithm>

extern "C" {
#include "swephexp.h"
}

namespace Astro {

/////////////////////////////////////////////////////////////////////////////
// Myanmar Calendar Constants and Exception Data (from yan9a/mmcal)
/////////////////////////////////////////////////////////////////////////////

// Full moon day offset exceptions [my, offset]
static const std::vector<std::pair<long, long>> fullMoonOffsetExceptions = {
    {1120, -1}, {1126, -1}, {1150, 1}, {1152, -1}, {1161, -1}, {1162, -1},
    {1172, -1}, {1181, -1}, {1190, 1}, {1191, -1}, {1194, -1}, {1195, -1},
    {1198, -1}, {1201, 1}, {1202, 1}, {1208, 1}, {1215, -1}, {1217, -1},
    {1218, -1}, {1221, 1}, {1234, 1}, {1235, -1}, {1236, -1}, {1237, -1},
    {1238, -1}, {1239, -1}, {1241, -1}, {1242, -1}, {1244, 1}, {1245, 1},
    {1253, -1}, {1258, 1}, {1264, 1}, {1291, -1}, {1292, -1}, {1298, 1},
    {1309, 1}, {1310, 1}, {1315, 1}, {1324, 1}, {1344, 1}, {1345, 1}
};

// Watat exceptions (years to flip watat calculation)
static const std::vector<long> watatExceptions = {
    1201, 1202, 1263, 1264, 1344, 1345
};

/////////////////////////////////////////////////////////////////////////////
// Constructor and Initialization
/////////////////////////////////////////////////////////////////////////////

MyanmarCalendar::MyanmarCalendar() : initialized(false) {
    // Default constructor
}

MyanmarCalendar::~MyanmarCalendar() {
    // Destructor
}

bool MyanmarCalendar::initialize() {
    initialized = true;
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Core Myanmar Calendar Calculations (yan9a/mmcal implementation)
/////////////////////////////////////////////////////////////////////////////

//// Get Myanmar year constants depending on era
// Based on geolib / yan9a/mmcal GetMyConst function
void MyanmarCalendar::getMyanmarConstants(long my, double& EI, double& WO, double& NM, long& EW) {
    EI = 3.0;
    WO = 0.0;
    NM = 0.0;
    EW = 0;
    std::vector<std::pair<long, long>> fme;
    std::vector<long> wte;

    if (my >= 1312) {
        // Third era (Independence 1312 ME and after)
        EI = 3.0;
        WO = -0.5;
        NM = 8;
        fme = {{1377, 1}};
        wte = {1344, 1345};
    } else if (my >= 1217) {
        // Second era (British colonial period: 1217 ME - 1311 ME)
        EI = 2.0;
        WO = -1.0;
        NM = 4;
        fme = {{1234, 1}, {1261, -1}};
        wte = {1263, 1264};
    } else if (my >= 1100) {
        // First era: Thandeikta (1100 ME - 1216 ME)
        EI = 1.3;
        WO = -0.85;
        NM = -1;
        fme = {{1120, 1}, {1126, -1}, {1150, 1}, {1172, -1}, {1207, 1}};
        wte = {1201, 1202};
    } else if (my >= 798) {
        // First era: Makaranta 2 (798 ME - 1099 ME)
        EI = 1.2;
        WO = -1.1;
        NM = -1;
        fme = {{813, -1}, {849, -1}, {851, -1}, {854, -1}, {927, -1}, {933, -1},
               {936, -1}, {938, -1}, {949, -1}, {952, -1}, {963, -1}, {968, -1}, {1039, -1}};
    } else {
        // First era: Makaranta 1 (0 ME - 797 ME)
        EI = 1.1;
        WO = -1.1;
        NM = -1;
        fme = {{205, 1}, {246, 1}, {471, 1}, {572, -1}, {651, 1}, {653, 2},
               {656, 1}, {672, 1}, {729, 1}, {767, -1}};
    }

    // Apply full moon offset exceptions
    for (const auto& e : fme) {
        if (e.first == my) {
            WO += e.second;
            break;
        }
    }

    // Apply watat exceptions
    for (long exceptionYear : wte) {
        if (exceptionYear == my) {
            EW = 1;
            break;
        }
    }
}

// Check watat (intercalary month) - geolib / yan9a/mmcal algorithm
void MyanmarCalendar::calculateWatat(long my, long& watat, long& fm) {
    double SY = SOLAR_YEAR;          // 365.2587565
    double LM = LUNAR_MONTH;         // 29.53058795
    double MO = MYANMAR_EPOCH;       // 1954168.050623

    double EI, WO, NM;
    long EW;
    getMyanmarConstants(my, EI, WO, NM, EW);

    double TA = (SY / 12.0 - LM) * (12.0 - NM); // threshold to adjust
    double ed = std::fmod(SY * (my + 3739), LM); // excess day
    if (ed < 0.0) ed += LM;
    if (ed < TA) ed += LM; // adjust excess days

    fm = static_cast<long>(std::round(SY * my + MO - ed + 4.5 * LM + WO)); // full moon day of 2nd Waso
    watat = 0; // find watat

    if (EI >= 2.0) { // if 2nd era or later find watat based on excess days
        double TW = LM - (SY / 12.0 - LM) * NM;
        if (ed >= TW) watat = 1;
    } else { // if 1st era, find watat by 19 years metonic cycle
        long w = (my * 7 + 2) % 19;
        if (w < 0) w += 19;
        watat = static_cast<long>(std::floor(w / 12.0));
    }
    watat ^= EW; // correct watat exceptions
}

// Check Myanmar Year - geolib / yan9a/mmcal algorithm
void MyanmarCalendar::calculateMyanmarYear(long my, long& myt, long& tg1, long& fm, long& werr) {
    long yd = 0, nd = 0, y1watat, y1fm, y2watat, y2fm;
    werr = 0;

    calculateWatat(my, y2watat, y2fm);
    myt = y2watat;

    do {
        yd++;
        calculateWatat(my - yd, y1watat, y1fm);
    } while (y1watat == 0 && yd < 3);

    if (myt) {
        nd = (y2fm - y1fm) % 354;
        myt = static_cast<long>(std::floor(nd / 31.0) + 1);
        fm = y2fm;
        if (nd != 30 && nd != 31) {
            werr = 1;
        }
    } else {
        fm = y1fm + 354 * yd;
    }
    tg1 = y1fm + 354 * yd - 102;
}

// Julian day number to Myanmar date - geolib / yan9a/mmcal algorithm
void MyanmarCalendar::julianToMyanmar(double jd, long& myt, long& my, long& mm, long& md) {
    long jdn = static_cast<long>(std::floor(jd + 0.5));
    double SY = SOLAR_YEAR;
    double MO = MYANMAR_EPOCH;

    my = static_cast<long>(std::floor((jdn - 0.5 - MO) / SY));
    long tg1, fm, werr;
    calculateMyanmarYear(my, myt, tg1, fm, werr);

    long dd = jdn - tg1 + 1;
    long b = static_cast<long>(std::floor(myt / 2.0));
    long c = static_cast<long>(std::floor(1.0 / (myt + 1)));
    long myl = 354 + (1 - c) * 30 + b;

    long mmt = static_cast<long>(std::floor((dd - 1.0) / myl));
    dd -= mmt * myl;

    long a = static_cast<long>(std::floor((dd + 423.0) / 512.0));
    mm = static_cast<long>(std::floor((dd - b * a + c * a * 30.0 + 29.26) / 29.544));
    long e = static_cast<long>(std::floor((mm + 12.0) / 16.0));
    long f = static_cast<long>(std::floor((mm + 11.0) / 16.0));
    md = dd - static_cast<long>(std::floor(29.544 * mm - 29.26)) - b * e + c * f * 30;
    mm += f * 3 - e * 4 + 12 * mmt;
}

// Myanmar date to Julian day number - geolib / yan9a/mmcal algorithm
long MyanmarCalendar::myanmarToJulian(long my, long mm, long md) {
    long myt, tg1, fm, werr;
    calculateMyanmarYear(my, myt, tg1, fm, werr);

    long mmt = static_cast<long>(std::floor(mm / 13.0));
    long adjMm = mm % 13 + mmt;
    long b = static_cast<long>(std::floor(myt / 2.0));
    long c = 1 - static_cast<long>(std::floor((myt + 1.0) / 2.0));

    adjMm += 4 - static_cast<long>(std::floor((adjMm + 15.0) / 16.0)) * 4 + static_cast<long>(std::floor((adjMm + 12.0) / 16.0));
    long dd = md + static_cast<long>(std::floor(29.544 * adjMm - 29.26)) -
              c * static_cast<long>(std::floor((adjMm + 11.0) / 16.0)) * 30 +
              b * static_cast<long>(std::floor((adjMm + 12.0) / 16.0));
    long myl = 354 + (1 - c) * 30 + b;
    dd += mmt * myl;

    return (dd + tg1 - 1);
}

/////////////////////////////////////////////////////////////////////////////
// Calendar Property Calculations (yan9a/mmcal algorithms)
/////////////////////////////////////////////////////////////////////////////

// Get the apparent length of the year from year type
long MyanmarCalendar::calculateYearLength(long myt) {
    return (354 + (1 - static_cast<long>(floor(1 / (myt + 1)))) * 30 + static_cast<long>(floor(myt / 2)));
}

// Get length of month from month and year type
long MyanmarCalendar::calculateMonthLength(long mm, long myt) {
    long mml = 30 - mm % 2; // month length
    if (mm == 3) mml += static_cast<long>(floor(myt / 2)); // adjust if Nayon in big watat
    return mml;
}

// Get fortnight day from month day
long MyanmarCalendar::calculateFortnightDay(long md) {
    return (md - 15 * static_cast<long>(floor(md / 16)));
}

// Get day of month from fortnight day (1..15) and moon phase (0..3) - yan9a/mmcal & geolib
long MyanmarCalendar::calculateDayOfMonth(long mf, long mp, long mm, long myt) {
    long mml = calculateMonthLength(mm, myt);
    long m1 = (mp % 2);
    long m2 = static_cast<long>(floor(mp / 2.0));
    return m1 * (15 + m2 * (mml - 15)) + (1 - m1) * (mf + 15 * m2);
}

// Get moon phase from day, month, and year type
long MyanmarCalendar::calculateMoonPhase(long md, long mm, long myt) {
    long mml = calculateMonthLength(mm, myt);
    return (static_cast<long>(floor((md + 1) / 16)) + static_cast<long>(floor(md / 16)) + static_cast<long>(floor(md / mml)));
}

/////////////////////////////////////////////////////////////////////////////
// Astrological Day Calculations (yan9a/mmcal algorithms)
/////////////////////////////////////////////////////////////////////////////

// Get sabbath day and sabbath eve from day of the month, month, and year type
long MyanmarCalendar::calculateSabbath(long md, long mm, long myt) {
    long mml = calculateMonthLength(mm, myt);
    long s = 0;
    if ((md == 8) || (md == 15) || (md == 23) || (md == mml)) s = 1;
    if ((md == 7) || (md == 14) || (md == 22) || (md == (mml - 1))) s = 2;
    return s;
}

// Get yatyaza from month and weekday
long MyanmarCalendar::calculateYatyaza(long mm, long wd) {
    // first waso is considered waso
    long m1 = mm % 4;
    long yatyaza = 0;
    long wd1 = static_cast<long>(floor(m1 / 2)) + 4;
    long wd2 = ((1 - static_cast<long>(floor(m1 / 2))) + m1 % 2) * (1 + 2 * (m1 % 2));
    if ((wd == wd1) || (wd == wd2)) yatyaza = 1;
    return yatyaza;
}

// Get pyathada from month and weekday
long MyanmarCalendar::calculatePyathada(long mm, long wd) {
    // first waso is considered waso
    long m1 = mm % 4;
    long pyathada = 0;
    std::vector<long> wda = {1, 3, 3, 0, 2, 1, 2};
    if ((m1 == 0) && (wd == 4)) {
        pyathada = 2; // afternoon pyathada
    } else if (m1 == wda[wd]) {
        pyathada = 1;
    }
    return pyathada;
}

// Get nagahle direction from month
long MyanmarCalendar::calculateNagahle(long mm) {
    if (mm <= 0) mm = 4; // first waso is considered waso
    return ((mm % 12) / 3);
}

// Get mahabote from year and weekday
long MyanmarCalendar::calculateMahabote(long my, long wd) {
    return ((my - wd) % 7);
}

// Get nakhat from year
long MyanmarCalendar::calculateNakhat(long my) {
    return (my % 3);
}

// Get thamanyo from month and weekday
long MyanmarCalendar::calculateThamanyo(long mm, long wd) {
    long mmt = static_cast<long>(floor(mm / 13));
    mm = mm % 13 + mmt; // to 1-12 with month type
    if (mm <= 0) mm = 4; // first waso is considered waso
    long thamanyo = 0;
    long wd1 = (mm - 4) % 8;
    wd1 = static_cast<long>(floor(wd1 / 2)) + 4;
    long wd2 = (wd + 7 - wd1) % 7;
    if (wd2 <= 1) thamanyo = 1;
    return thamanyo;
}

// Get amyeittasote from day and weekday
long MyanmarCalendar::calculateAmyeittasote(long md, long wd) {
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long amyeittasote = 0;
    std::vector<long> wda = {5, 8, 3, 7, 2, 4, 1};
    if (mf == wda[wd]) amyeittasote = 1;
    return amyeittasote;
}

// Get warameittugyi from day and weekday
long MyanmarCalendar::calculateWarameittugyi(long md, long wd) {
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long warameittugyi = 0;
    std::vector<long> wda = {7, 1, 4, 8, 9, 6, 3};
    if (mf == wda[wd]) warameittugyi = 1;
    return warameittugyi;
}

// Get warameittunge from day and weekday
long MyanmarCalendar::calculateWarameittunge(long md, long wd) {
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long warameittunge = 0;
    long wn = (wd + 6) % 7;
    if ((12 - mf) == wn) warameittunge = 1;
    return warameittunge;
}

// Get yatpote from day and weekday
long MyanmarCalendar::calculateYatpote(long md, long wd) {
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long yatpote = 0;
    std::vector<long> wda = {8, 1, 4, 6, 9, 8, 7};
    if (mf == wda[wd]) yatpote = 1;
    return yatpote;
}

// Get thamaphyu from day and weekday
long MyanmarCalendar::calculateThamaphyu(long md, long wd) {
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long thamaphyu = 0;
    std::vector<long> wda = {1, 2, 6, 6, 5, 6, 7};
    if (mf == wda[wd]) thamaphyu = 1;
    return thamaphyu;
}

// Get nagapor from day and weekday
long MyanmarCalendar::calculateNagapor(long md, long wd) {
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long nagapor = 0;
    std::vector<long> wda = {26, 21, 2, 10, 18, 2, 21};
    if (mf == wda[wd]) nagapor = 1;
    return nagapor;
}

// Get yatyotema from month and day
long MyanmarCalendar::calculateYatyotema(long mm, long md) {
    long mmt = static_cast<long>(floor(mm / 13));
    mm = mm % 13 + mmt; // to 1-12 with month type
    if (mm <= 0) mm = 4; // first waso is considered waso
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long yatyotema = 0;
    long m1 = (mm % 2) ? mm : ((mm + 9) % 12);
    m1 = (m1 + 4) % 12 + 1;
    if (mf == m1) yatyotema = 1;
    return yatyotema;
}

// Get mahayatkyan from month and day
long MyanmarCalendar::calculateMahayatkyan(long mm, long md) {
    long mmt = static_cast<long>(floor(mm / 13));
    mm = mm % 13 + mmt; // to 1-12 with month type
    if (mm <= 0) mm = 4; // first waso is considered waso
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long mahayatkyan = 0;
    long m1 = (static_cast<long>(floor((mm % 12) / 2)) + 4) % 6 + 1;
    if (mf == m1) mahayatkyan = 1;
    return mahayatkyan;
}

// Get shanyat from month and day
long MyanmarCalendar::calculateShanyat(long mm, long md) {
    long mmt = static_cast<long>(floor(mm / 13));
    mm = mm % 13 + mmt; // to 1-12 with month type
    if (mm <= 0) mm = 4; // first waso is considered waso
    long mf = md - 15 * static_cast<long>(floor(md / 16)); // get fortnight day [0-15]
    long shanyat = 0;
    std::vector<long> sya = {8, 8, 2, 2, 9, 3, 3, 5, 1, 4, 7, 4};
    if (mf == sya[mm - 1]) shanyat = 1;
    return shanyat;
}

// Calculate Sasana year
long MyanmarCalendar::sasanaYear(long my, long mm, long md, long k) {
    // Basic calculation: Myanmar year + 1182
    long sy = my + 1182;

    // Adjust for Kason full moon day if k=1 (yan9a/mmcal feature)
    if (k == 1) {
        if (mm == 2) { // Kason
            long mp = calculateMoonPhase(md, mm, 0); // assume common year for moon phase calc
            if (mp == 1) sy++; // if Kason full moon, increment Sasana year
        }
    }

    return sy;
}

/////////////////////////////////////////////////////////////////////////////
// Main Calculation Methods
/////////////////////////////////////////////////////////////////////////////

MyanmarCalendarData MyanmarCalendar::calculateMyanmarCalendar(const BirthData& birthData) const {
    if (!initialized) {
        throw std::runtime_error("Myanmar calendar not initialized");
    }

    // Convert birth data to Julian Day (in UTC)
    double jd = birthData.getJulianDay();

    return calculateMyanmarCalendar(jd);
}

MyanmarCalendarData MyanmarCalendar::calculateMyanmarCalendar(double julianDay, MyanmarAstronomicalTradition tradition) const {
    if (!initialized) {
        throw std::runtime_error("Myanmar calendar not initialized");
    }

    MyanmarCalendarData data;

    // Core conversion using yan9a/mmcal algorithm
    long myt, my, mm, md;
    julianToMyanmar(julianDay, myt, my, mm, md);

    // Fill basic information
    data.myanmarYear = my;
    data.month = static_cast<MyanmarMonth>(mm);
    data.dayOfMonth = md;
    data.yearType = static_cast<MyanmarYearType>(myt);
    data.sasanaYear = sasanaYear(my, mm, md, 0);

    // Calculate additional properties
    data.moonPhase = static_cast<MyanmarMoonPhase>(calculateMoonPhase(md, mm, myt));
    data.fortnightDay = calculateFortnightDay(md);
    data.monthLength = calculateMonthLength(mm, myt);

    // Calculate weekday using yan9a/mmcal formula: (jd+2)%7
    long jdn = static_cast<long>(round(julianDay)); // Convert to Julian Day Number
    long weekday = (jdn + 2) % 7; // 0=sat, 1=sun, ..., 6=fri
    data.weekday = static_cast<MyanmarWeekday>(weekday);

    // Calculate astrological information
    data.mahabote = static_cast<Mahabote>(calculateMahabote(my, weekday));
    data.nakhat = static_cast<Nakhat>(calculateNakhat(my));
    data.nagahle = static_cast<NagahleDirection>(calculateNagahle(mm));

    // Calculate astrological days
    long sabbath = calculateSabbath(md, mm, myt);
    data.isSabbath = (sabbath == 1);
    data.isSabbathEve = (sabbath == 2);

    data.isYatyaza = (calculateYatyaza(mm, weekday) == 1);

    long pyathada = calculatePyathada(mm, weekday);
    data.isPyathada = (pyathada == 1);
    data.isAfternoonPyathada = (pyathada == 2);

    data.isThamanyo = (calculateThamanyo(mm, weekday) == 1);
    data.isAmyeittasote = (calculateAmyeittasote(md, weekday) == 1);
    data.isWarameittugyi = (calculateWarameittugyi(md, weekday) == 1);
    data.isWarameittunge = (calculateWarameittunge(md, weekday) == 1);
    data.isYatpote = (calculateYatpote(md, weekday) == 1);
    data.isThamaphyu = (calculateThamaphyu(md, weekday) == 1);
    data.isNagapor = (calculateNagapor(md, weekday) == 1);
    data.isYatyotema = (calculateYatyotema(mm, md) == 1);
    data.isMahayatkyan = (calculateMahayatkyan(mm, md) == 1);
    data.isShanyat = (calculateShanyat(mm, md) == 1);

    data.julianDay = julianDay;

    // Calculate traditional Myanmar astronomical coordinates (from geolib / Makaranta / Thandeikta)
    const double meEpochJd = 1954167.5;
    double elapsedDays = julianDay - meEpochJd;
    data.aharganaDays = static_cast<long>(std::floor(elapsedDays));
    data.sunMeanLonDeg = std::fmod(std::fmod(elapsedDays * (360.0 / SOLAR_YEAR), 360.0) + 360.0, 360.0);
    const double sunMandocca = 78.0;
    double sunAnomaly = (data.sunMeanLonDeg - sunMandocca) * (M_PI / 180.0);
    const double maxMandaArcmin = (tradition == MyanmarAstronomicalTradition::THANDEIKTA) ? 130.0 : 130.5;
    double sunMandaCorrection = (maxMandaArcmin / 60.0) * std::sin(sunAnomaly);
    data.sunTrueLonDeg = std::fmod(std::fmod(data.sunMeanLonDeg - sunMandaCorrection, 360.0) + 360.0, 360.0);
    double lunarDays = std::fmod(std::fmod(elapsedDays, LUNAR_MONTH) + LUNAR_MONTH, LUNAR_MONTH);
    data.moonTrueLonDeg = std::fmod(std::fmod(data.sunTrueLonDeg + lunarDays * (360.0 / LUNAR_MONTH), 360.0) + 360.0, 360.0);
    data.thitjaDeg = std::fmod((sunAnomaly * (180.0 / M_PI)) + 360.0, 360.0);
    data.avamanna = std::fmod(std::fmod(data.aharganaDays * 11.0 + 650.0, 692.0) + 692.0, 692.0);

    // Calculate Thingyan details
    data.thingyan = calculateThingyan(data.myanmarYear);

    // Identify festivals, holidays and astrological events
    identifyFestivals(data);
    identifyHolidays(data);
    identifyAstrologicalEvents(data);

    // Add holidays from comprehensive geolib holiday calculation
    auto extraHols = calculateHolidays(data.myanmarYear, static_cast<long>(data.month), data.dayOfMonth, static_cast<long>(data.moonPhase), jdn);
    for (const auto& hol : extraHols) {
        if (std::find(data.holidays.begin(), data.holidays.end(), hol) == data.holidays.end()) {
            data.holidays.push_back(hol);
        }
    }

    return data;
}

/////////////////////////////////////////////////////////////////////////////
// Festival and Holiday Identification (yan9a/mmcal style)
/////////////////////////////////////////////////////////////////////////////

MyanmarThingyanDetails MyanmarCalendar::calculateThingyan(long my) {
    MyanmarThingyanDetails details;
    const double solarYear = 1577917828.0 / 4320000.0;
    const double myanmarEpochMO = 1954168.050623;

    double ja = solarYear * my + myanmarEpochMO;
    double jk = (my >= 1312) ? (ja - 2.169918982) : (ja - 2.1675);

    details.atetJd = ja;
    details.akyaJd = jk;

    long da = static_cast<long>(round(ja));
    long dk = static_cast<long>(round(jk));

    auto jdnToYMD = [](long jdn, int& y, int& m, int& d) {
        long l = jdn + 68569;
        long n = (4 * l / 146097);
        long l2 = l - ((146097 * n + 3) / 4);
        long i = ((4000 * (l2 + 1)) / 1461001);
        long l3 = l2 - ((1461 * i) / 4) + 31;
        long j = ((80 * l3) / 2447);
        d = l3 - ((2447 * j) / 80);
        long l4 = (j / 11);
        m = j + 2 - 12 * l4;
        y = 100 * (n - 49) + i + l4;
    };

    jdnToYMD(dk - 1, details.akyoYear, details.akyoMonth, details.akyoDay);
    jdnToYMD(dk, details.akyaYear, details.akyaMonth, details.akyaDay);
    jdnToYMD(da, details.atetYear, details.atetMonth, details.atetDay);
    jdnToYMD(da + 1, details.newYearYear, details.newYearMonth, details.newYearDay);

    for (long j = dk + 1; j < da; ++j) {
        int y = 0, m = 0, d = 0;
        jdnToYMD(j, y, m, d);
        details.akyatDates.push_back(std::make_tuple(y, m, d));
    }

    return details;
}

std::vector<std::string> MyanmarCalendar::calculateHolidays(long my, long mm, long md, long mp, long jdn) {
    std::vector<std::string> h;

    // Myanmar Traditional & Religious Holidays
    if (mm == 2 && mp == 1) {
        h.push_back("Buddha Day (Kason Full Moon)");
    } else if (mm == 4 && mp == 1) {
        h.push_back("Dhammacakka Day (Start of Buddhist Lent / Waso Full Moon)");
    } else if (mm == 7 && mp == 1) {
        h.push_back("Abhidhamma Day (End of Buddhist Lent / Thadingyut Full Moon)");
    } else if (mm == 8 && mp == 1) {
        h.push_back("Tazaungdaing Festival (Tazaungmon Full Moon)");
    } else if (mm == 12 && mp == 1) {
        h.push_back("Tabaung Full Moon Day");
    } else if (mm == 8 && mp == 2 && (md - 15) == 10) {
        h.push_back("National Day");
    } else if (mm == 10 && md == 1) {
        h.push_back("Kayin New Year");
    }

    // Mon National Day
    if (my >= 1309 && mm == 11 && md == 16) {
        h.push_back("Mon National Day");
    }

    // Additional Myanmar Calendar holidays
    if (my >= 1379 && mm == 7 && (md == 14 || md == 16)) {
        h.push_back("Thadingyut Holiday");
    }
    if (my >= 1379 && mm == 8 && md == 14) {
        h.push_back("Tazaungdaing Holiday");
    }
    if (my >= 1306 && mm == 9 && md == 1) {
        h.push_back("Authors Day");
    }

    // Thingyan checks
    MyanmarThingyanDetails thingyan = calculateThingyan(my);
    long da = static_cast<long>(round(thingyan.atetJd));
    long dk = static_cast<long>(round(thingyan.akyaJd));

    if (jdn == dk - 1) {
        h.push_back("Thingyan Akyo (Eve of Water Festival)");
    } else if (jdn == dk) {
        h.push_back("Thingyan Akya");
    } else if (jdn > dk && jdn < da) {
        h.push_back("Thingyan Akyat");
    } else if (jdn == da) {
        h.push_back("Thingyan Atet");
    } else if (jdn == da + 1) {
        h.push_back("Myanmar New Year Day");
    }

    // Gregorian based Myanmar public holidays
    long l = jdn + 68569;
    long n = (4 * l / 146097);
    long l2 = l - ((146097 * n + 3) / 4);
    long i = ((4000 * (l2 + 1)) / 1461001);
    long l3 = l2 - ((1461 * i) / 4) + 31;
    long j = ((80 * l3) / 2447);
    int gd = l3 - ((2447 * j) / 80);
    long l4 = (j / 11);
    int gm = j + 2 - 12 * l4;

    if (gm == 1 && gd == 4) {
        h.push_back("Independence Day");
    } else if (gm == 2 && gd == 12) {
        h.push_back("Union Day");
    } else if (gm == 3 && gd == 2) {
        h.push_back("Peasants' Day");
    } else if (gm == 3 && gd == 27) {
        h.push_back("Armed Forces Day");
    } else if (gm == 5 && gd == 1) {
        h.push_back("World Workers' Day (Labor Day)");
    } else if (gm == 7 && gd == 19) {
        h.push_back("Martyrs' Day");
    } else if (gm == 12 && gd == 25) {
        h.push_back("Christmas Day");
    }

    return h;
}

void MyanmarCalendar::identifyFestivals(MyanmarCalendarData& data) {
    long mm = static_cast<long>(data.month);
    long md = data.dayOfMonth;
    long mp = static_cast<long>(data.moonPhase);

    // Major Buddhist festivals based on lunar calendar
    if ((mm == 2) && (mp == 1)) data.festivals.push_back("Buddha Day"); // Vesak day
    if ((mm == 4) && (mp == 1)) data.festivals.push_back("Start of Buddhist Lent"); // Warso day
    if ((mm == 7) && (mp == 1)) data.festivals.push_back("End of Buddhist Lent");
    if ((mm == 8) && (mp == 1)) data.festivals.push_back("Tazaungdaing");
    if ((mm == 12) && (mp == 1)) data.festivals.push_back("Tabaung Pwe");

    // Other traditional festivals
    if ((mm == 5) && (mp == 1)) data.festivals.push_back("Metta Day"); // Wagaung full moon
    if ((mm == 9) && (md == 1)) data.festivals.push_back("Shan New Year Day"); // Nadaw waxing moon 1
    if ((mm == 10) && (md == 1)) data.festivals.push_back("Karen New Year Day");

    // Special observances
    if ((mm == 5) && (md == 10)) data.festivals.push_back("Taungpyone Pwe");
    if ((mm == 5) && (md == 23)) data.festivals.push_back("Yadanagu Pwe");
}

void MyanmarCalendar::identifyHolidays(MyanmarCalendarData& data) {
    long my = data.myanmarYear;
    long mm = static_cast<long>(data.month);
    long md = data.dayOfMonth;

    // Myanmar calendar holidays
    if ((my >= 1309) && (mm == 11) && (md == 16)) {
        data.holidays.push_back("Mon National Day");
    }
    if ((my >= 1379) && (mm == 7) && (md == 14 || md == 16)) {
        data.holidays.push_back("Holiday");
    }
    if ((my >= 1379) && (mm == 8) && (md == 14)) {
        data.holidays.push_back("Holiday");
    }
    if ((my >= 1282) && (mm == 8) && (md == 25)) {
        data.holidays.push_back("National Day");
    }
    if ((my >= 1306) && (mm == 9) && (md == 1)) {
        data.holidays.push_back("Authors Day");
    }
}

void MyanmarCalendar::identifyAstrologicalEvents(MyanmarCalendarData& data) {
    // Identify significant astrological events
    if (data.isYatyaza || data.isAmyeittasote) {
        data.astrologicalEvents.push_back("Auspicious Day (Yatyaza / Amyeittasote)");
    }
    if (data.isPyathada || data.isAfternoonPyathada || data.isThamanyo || data.isWarameittugyi || data.isWarameittunge) {
        data.astrologicalEvents.push_back("Inauspicious Day (Pyathada / Thamanyo / Warameittu)");
    }
    if (data.isSabbath) {
        data.astrologicalEvents.push_back("Buddhist Sabbath Day");
    }
    if (data.moonPhase == MyanmarMoonPhase::FULL_MOON) {
        data.astrologicalEvents.push_back("Full Moon Day");
    }
    if (data.moonPhase == MyanmarMoonPhase::NEW_MOON) {
        data.astrologicalEvents.push_back("New Moon Day");
    }
}

/////////////////////////////////////////////////////////////////////////////
// Utility Calculations
/////////////////////////////////////////////////////////////////////////////

MyanmarYearType MyanmarCalendar::getYearType(long myanmarYear) const {
    long watat, fm;
    calculateWatat(myanmarYear, watat, fm);
    return static_cast<MyanmarYearType>(watat);
}

long MyanmarCalendar::getSasanaYear(long myanmarYear, long month, long day) const {
    return sasanaYear(myanmarYear, month, day, 0);
}

/////////////////////////////////////////////////////////////////////////////
// Binary Search Utilities (yan9a/mmcal style)
/////////////////////////////////////////////////////////////////////////////

long MyanmarCalendar::binarySearch1(long key, const std::vector<long>& array) {
    long i = 0, u = array.size() - 1, m;
    while (u >= i) {
        m = (i + u) / 2;
        if (array[m] == key) return m;
        else if (array[m] > key) u = m - 1;
        else i = m + 1;
    }
    return -1;
}

long MyanmarCalendar::binarySearch2(long key, const std::vector<std::pair<long, long>>& array) {
    long i = 0, u = array.size() - 1, m;
    while (u >= i) {
        m = (i + u) / 2;
        if (array[m].first == key) return m;
        else if (array[m].first > key) u = m - 1;
        else i = m + 1;
    }
    return -1;
}

/////////////////////////////////////////////////////////////////////////////
// Output Formatting Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarCalendar::generateTable(const MyanmarCalendarData& data) const {
    std::stringstream ss;

    ss << "===================================================================\n";
    ss << "MYANMAR CALENDAR (GEOLIB ARYABHATA_01 TRADITIONAL SYSTEM)\n";
    ss << "===================================================================\n";

    // Basic date information
    ss << "\n📅 DATE INFORMATION:\n";
    ss << "   Myanmar Year: " << data.myanmarYear << " ME\n";
    ss << "   Sasana Year: " << data.sasanaYear << " SE\n";
    ss << "   Month: " << getMyanmarMonthName(data.month) << "\n";
    ss << "   Day: " << data.dayOfMonth << "\n";
    ss << "   Year Type: " << getYearTypeName(data.yearType) << "\n";

    // Lunar information
    ss << "\n🌙 LUNAR INFORMATION:\n";
    ss << "   Moon Phase: " << getMoonPhaseName(data.moonPhase) << "\n";
    ss << "   Fortnight Day: " << data.fortnightDay << "\n";
    ss << "   Month Length: " << data.monthLength << " days\n";

    // Astrological information
    ss << "\n🔮 ASTROLOGICAL INFORMATION:\n";
    ss << "   Weekday: " << getMyanmarWeekdayName(data.weekday) << "\n";
    ss << "   Mahabote: " << getMahaboteName(data.mahabote) << "\n";
    ss << "   Nakhat: " << getNakhatName(data.nakhat) << "\n";
    ss << "   Nagahle: " << getNagahleDirectionName(data.nagahle) << "\n";

    // Astrological days
    ss << "\n🌟 ASTROLOGICAL DAYS:\n";
    if (data.isSabbath) ss << "   • Buddhist Sabbath Day\n";
    if (data.isSabbathEve) ss << "   • Sabbath Eve\n";
    if (data.isYatyaza) ss << "   • Yatyaza (Auspicious Day)\n";
    if (data.isPyathada) ss << "   • Pyathada (Inauspicious Day)\n";
    if (data.isAfternoonPyathada) ss << "   • Afternoon Pyathada (Inauspicious Afternoon)\n";
    if (data.isThamanyo) ss << "   • Thamanyo (Inauspicious Day)\n";
    if (data.isWarameittugyi) ss << "   • Warameittugyi (Inauspicious Day)\n";
    if (data.isWarameittunge) ss << "   • Warameittunge (Inauspicious Day)\n";
    if (data.isAmyeittasote) ss << "   • Amyeittasote (Auspicious Day)\n";

    if (!data.festivals.empty()) {
        ss << "\n🎉 FESTIVALS & OBSERVANCES:\n";
        for (const auto& festival : data.festivals) {
            ss << "   • " << festival << "\n";
        }
    }

    if (!data.astrologicalEvents.empty()) {
        ss << "\n⭐ ASTROLOGICAL EVENTS:\n";
        for (const auto& event : data.astrologicalEvents) {
            ss << "   • " << event << "\n";
        }
    }

    // Traditional astronomical coordinates
    ss << "\nTRADITIONAL ASTRONOMICAL COORDINATES:\n";
    ss << "   Ahargana Days: " << data.aharganaDays << " days\n";
    ss << "   Madhyama Ravi: " << std::fixed << std::setprecision(3) << data.sunMeanLonDeg << " deg (Mean Sun)\n";
    ss << "   Spashta Ravi : " << data.sunTrueLonDeg << " deg (True Sun)\n";
    ss << "   Spashta Moon : " << data.moonTrueLonDeg << " deg (True Moon)\n";
    ss << "   Thitja Deg   : " << data.thitjaDeg << " deg (Solar Anomaly)\n";
    ss << "   Avamanna     : " << data.avamanna << " (Avama Remainder)\n";

    // Recommendations
    ss << "\n💡 RECOMMENDATIONS:\n";
    if (data.isSabbath) {
        ss << "   Status: [Religious Observance] Buddhist Sabbath Day\n";
        ss << "   Note: Observe Buddhist precepts, visit pagodas, practice meditation\n";
    } else if (data.isYatyaza || data.isAmyeittasote) {
        ss << "   Status: [Auspicious Day] Yatyaza / Amyeittasote\n";
        ss << "   Note: Good day for starting new ventures, business, weddings, and auspicious ceremonies\n";
    } else if (data.isPyathada || data.isAfternoonPyathada || data.isThamanyo || data.isWarameittugyi || data.isWarameittunge) {
        ss << "   Status: [Inauspicious Day] Pyathada / Thamanyo / Warameittu\n";
        ss << "   Note: Avoid important activities, postpone new beginnings\n";
    } else {
        ss << "   Status: [Neutral Day]\n";
        ss << "   Note: Normal activities permitted\n";
    }

    ss << "\n═══════════════════════════════════════════════════════════════════\n";

    return ss.str();
}

std::string MyanmarCalendar::generateJSON(const MyanmarCalendarData& data) const {
    std::stringstream ss;

    ss << "{\n";
    ss << "  \"myanmarYear\": " << data.myanmarYear << ",\n";
    ss << "  \"sasanaYear\": " << data.sasanaYear << ",\n";
    ss << "  \"month\": {\n";
    ss << "    \"number\": " << static_cast<int>(data.month) << ",\n";
    ss << "    \"name\": \"" << getMyanmarMonthName(data.month) << "\"\n";
    ss << "  },\n";
    ss << "  \"dayOfMonth\": " << data.dayOfMonth << ",\n";
    ss << "  \"yearType\": {\n";
    ss << "    \"number\": " << static_cast<int>(data.yearType) << ",\n";
    ss << "    \"name\": \"" << getYearTypeName(data.yearType) << "\"\n";
    ss << "  },\n";
    ss << "  \"weekday\": {\n";
    ss << "    \"number\": " << static_cast<int>(data.weekday) << ",\n";
    ss << "    \"name\": \"" << getMyanmarWeekdayName(data.weekday) << "\"\n";
    ss << "  },\n";
    ss << "  \"moonPhase\": {\n";
    ss << "    \"number\": " << static_cast<int>(data.moonPhase) << ",\n";
    ss << "    \"name\": \"" << getMoonPhaseName(data.moonPhase) << "\"\n";
    ss << "  },\n";
    ss << "  \"fortnightDay\": " << data.fortnightDay << ",\n";
    ss << "  \"monthLength\": " << data.monthLength << ",\n";
    ss << "  \"astrological\": {\n";
    ss << "    \"mahabote\": \"" << getMahaboteName(data.mahabote) << "\",\n";
    ss << "    \"nakhat\": \"" << getNakhatName(data.nakhat) << "\",\n";
    ss << "    \"nagahle\": \"" << getNagahleDirectionName(data.nagahle) << "\",\n";
    ss << "    \"sabbath\": " << (data.isSabbath ? "true" : "false") << ",\n";
    ss << "    \"yatyaza\": " << (data.isYatyaza ? "true" : "false") << ",\n";
    ss << "    \"pyathada\": " << (data.isPyathada ? "true" : "false") << ",\n";
    ss << "    \"thamanyo\": " << (data.isThamanyo ? "true" : "false") << "\n";
    ss << "  },\n";
    ss << "  \"festivals\": [";
    for (size_t i = 0; i < data.festivals.size(); ++i) {
        ss << "\"" << data.festivals[i] << "\"";
        if (i < data.festivals.size() - 1) ss << ", ";
    }
    ss << "],\n";
    ss << "  \"astrologicalEvents\": [";
    for (size_t i = 0; i < data.astrologicalEvents.size(); ++i) {
        ss << "\"" << data.astrologicalEvents[i] << "\"";
        if (i < data.astrologicalEvents.size() - 1) ss << ", ";
    }
    ss << "]\n";
    ss << "}\n";

    return ss.str();
}

/////////////////////////////////////////////////////////////////////////////
// Name Conversion Utilities
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarCalendar::getMyanmarMonthName(MyanmarMonth month) {
    switch (month) {
        case MyanmarMonth::FIRST_WASO: return "First Waso";
        case MyanmarMonth::TAGU: return "Tagu";
        case MyanmarMonth::KASON: return "Kason";
        case MyanmarMonth::NAYON: return "Nayon";
        case MyanmarMonth::WASO: return "Waso";
        case MyanmarMonth::WAGAUNG: return "Wagaung";
        case MyanmarMonth::TAWTHALIN: return "Tawthalin";
        case MyanmarMonth::THADINGYUT: return "Thadingyut";
        case MyanmarMonth::TAZAUNGMON: return "Tazaungmon";
        case MyanmarMonth::NADAW: return "Nadaw";
        case MyanmarMonth::PYATHO: return "Pyatho";
        case MyanmarMonth::TABODWE: return "Tabodwe";
        case MyanmarMonth::TABAUNG: return "Tabaung";
        case MyanmarMonth::LATE_TAGU: return "Late Tagu";
        case MyanmarMonth::LATE_KASON: return "Late Kason";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getMyanmarWeekdayName(MyanmarWeekday weekday) {
    switch (weekday) {
        case MyanmarWeekday::SATURDAY: return "Saturday";
        case MyanmarWeekday::SUNDAY: return "Sunday";
        case MyanmarWeekday::MONDAY: return "Monday";
        case MyanmarWeekday::TUESDAY: return "Tuesday";
        case MyanmarWeekday::WEDNESDAY: return "Wednesday";
        case MyanmarWeekday::THURSDAY: return "Thursday";
        case MyanmarWeekday::FRIDAY: return "Friday";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getMahaboteName(Mahabote mahabote) {
    switch (mahabote) {
        case Mahabote::BINGA: return "Binga";
        case Mahabote::ATUN: return "Atun";
        case Mahabote::YAZA: return "Yaza";
        case Mahabote::ADIPATI: return "Adipati";
        case Mahabote::MARANA: return "Marana";
        case Mahabote::THIKE: return "Thike";
        case Mahabote::PUTI: return "Puti";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getNakhatName(Nakhat nakhat) {
    switch (nakhat) {
        case Nakhat::ORC: return "Balu (Ogre)";
        case Nakhat::ELF: return "Nat (Elf)";
        case Nakhat::HUMAN: return "Lu (Human)";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getNagahleDirectionName(NagahleDirection direction) {
    switch (direction) {
        case NagahleDirection::WEST: return "West";
        case NagahleDirection::NORTH: return "North";
        case NagahleDirection::EAST: return "East";
        case NagahleDirection::SOUTH: return "South";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getMoonPhaseName(MyanmarMoonPhase phase) {
    switch (phase) {
        case MyanmarMoonPhase::WAXING: return "Waxing";
        case MyanmarMoonPhase::FULL_MOON: return "Full Moon";
        case MyanmarMoonPhase::WANING: return "Waning";
        case MyanmarMoonPhase::NEW_MOON: return "New Moon";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getYearTypeName(MyanmarYearType type) {
    switch (type) {
        case MyanmarYearType::COMMON: return "Common Year";
        case MyanmarYearType::LITTLE_WATAT: return "Little Watat";
        case MyanmarYearType::BIG_WATAT: return "Big Watat";
        default: return "Unknown";
    }
}

/////////////////////////////////////////////////////////////////////////////
// Additional Methods (placeholder implementations)
/////////////////////////////////////////////////////////////////////////////

std::vector<MyanmarCalendarData> MyanmarCalendar::calculateMyanmarCalendarRange(const std::string& fromDate, const std::string& toDate) const {
    // Placeholder implementation
    std::vector<MyanmarCalendarData> result;
    return result;
}

std::string MyanmarCalendar::generateTable(const std::vector<MyanmarCalendarData>& dataList) const {
    std::stringstream ss;
    for (const auto& data : dataList) {
        ss << generateTable(data) << "\n";
    }
    return ss.str();
}

std::string MyanmarCalendar::generateCSV(const std::vector<MyanmarCalendarData>& dataList) const {
    std::stringstream ss;
    ss << "Myanmar Year,Month,Day,Year Type,Weekday,Moon Phase,Sabbath,Yatyaza,Pyathada,Thamanyo\n";

    for (const auto& data : dataList) {
        ss << data.myanmarYear << ","
           << getMyanmarMonthName(data.month) << ","
           << data.dayOfMonth << ","
           << getYearTypeName(data.yearType) << ","
           << getMyanmarWeekdayName(data.weekday) << ","
           << getMoonPhaseName(data.moonPhase) << ","
           << (data.isSabbath ? "Yes" : "No") << ","
           << (data.isYatyaza ? "Yes" : "No") << ","
           << (data.isPyathada ? "Yes" : "No") << ","
           << (data.isThamanyo ? "Yes" : "No") << "\n";
    }

    return ss.str();
}

std::string MyanmarCalendar::generateCalendarView(long myanmarYear, long month) const {
    // Placeholder implementation
    return "Calendar view for " + std::to_string(myanmarYear) + "/" + std::to_string(month);
}

std::string MyanmarCalendar::formatMyanmarDate(double jd, const std::string& format, double tz) const {
    MyanmarCalendarData data = calculateMyanmarCalendar(jd + tz / 24.0);

    std::string result = format;

    // Replace format codes (yan9a/mmcal style)
    std::regex yyyyRegex("&yyyy");
    result = std::regex_replace(result, yyyyRegex, std::to_string(data.myanmarYear));

    std::regex yRegex("&y");
    result = std::regex_replace(result, yRegex, std::to_string(data.myanmarYear));

    std::regex MRegex("&M");
    result = std::regex_replace(result, MRegex, getMyanmarMonthName(data.month));

    std::regex PRegex("&P");
    result = std::regex_replace(result, PRegex, getMoonPhaseName(data.moonPhase));

    std::regex ffRegex("&ff");
    result = std::regex_replace(result, ffRegex, (data.fortnightDay < 10 ? "0" : "") + std::to_string(data.fortnightDay));

    return result;
}

// Wrapper methods for main.cpp compatibility
MyanmarCalendarData MyanmarCalendar::calculateMyanmarDate(const BirthData& birthData) const {
    return calculateMyanmarCalendar(birthData);
}

std::vector<MyanmarCalendarData> MyanmarCalendar::calculateMyanmarDateRange(const std::string& fromDate, const std::string& toDate) const {
    return calculateMyanmarCalendarRange(fromDate, toDate);
}

std::string MyanmarCalendar::generateMyanmarCalendarTable(const MyanmarCalendarData& data) const {
    return generateTable(data);
}

std::string MyanmarCalendar::generateMyanmarCalendarTable(const std::vector<MyanmarCalendarData>& dataList) const {
    return generateTable(dataList);
}

/////////////////////////////////////////////////////////////////////////////
// Search functionality implementation
/////////////////////////////////////////////////////////////////////////////

// Utility method for parsing dates
bool MyanmarCalendar::parseDate(const std::string& dateStr, int& year, int& month, int& day) const {
    try {
        if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
            return false;
        }

        year = std::stoi(dateStr.substr(0, 4));
        month = std::stoi(dateStr.substr(5, 2));
        day = std::stoi(dateStr.substr(8, 2));

        // Basic validation
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Convert Gregorian date to Julian day
double MyanmarCalendar::gregorianDateToJulianDay(int year, int month, int day, double hour) const {
    // Use standard Julian day calculation
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;

    double jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    jd += hour / 24.0;

    return jd;
}

// Main search function - comprehensive Myanmar calendar search
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchMyanmarCalendar(const SearchCriteria& criteria, double latitude, double longitude) const {
    std::vector<SearchResult> results;

    if (!initialized) {
        return results;
    }

    // Parse search date range
    int startYear, startMonth, startDay;
    int endYear, endMonth, endDay;

    if (!parseDate(criteria.searchStartDate, startYear, startMonth, startDay) ||
        !parseDate(criteria.searchEndDate, endYear, endMonth, endDay)) {
        return results; // Invalid date range
    }

    // Calculate Julian day range
    double startJD = gregorianDateToJulianDay(startYear, startMonth, startDay, 0.0);
    double endJD = gregorianDateToJulianDay(endYear, endMonth, endDay, 0.0);

    // Search each day in the range
    for (double jd = startJD; jd <= endJD; jd += 1.0) {
        try {
            // Calculate Myanmar calendar for this day
            MyanmarCalendarData myanmarData = calculateMyanmarCalendar(jd);

            // Use accurately calculated Myanmar weekday (0=Saturday, 1=Sunday, ..., 6=Friday)
            int weekday = static_cast<int>(myanmarData.weekday);

            // Convert to Gregorian date for result using swe_revjul (no deltaT day shift)
            int gregYear, gregMonth, gregDay;
            double gTime;
            swe_revjul(jd, SE_GREG_CAL, &gregYear, &gregMonth, &gregDay, &gTime);

            char dateBuffer[32];
            snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d", gregYear, gregMonth, gregDay);

            // Check all criteria and calculate match score
            double matchScore = 0.0;
            std::string matchDescription;
            int matchCount = 0;
            int totalCriteria = 0;
            bool isMatch = (criteria.logicMode == LogicMode::AND);

            // Year criteria
            if (criteria.exactYear > 0) {
                totalCriteria++;
                bool match = (myanmarData.myanmarYear == criteria.exactYear);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Year=" + std::to_string(criteria.exactYear);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.yearRangeStart > 0) {
                totalCriteria++;
                bool match = (myanmarData.myanmarYear >= criteria.yearRangeStart && myanmarData.myanmarYear <= criteria.yearRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Year=" + std::to_string(criteria.yearRangeStart) + "-" + std::to_string(criteria.yearRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Month criteria
            if (criteria.exactMonth >= 0) {
                totalCriteria++;
                bool match = (static_cast<int>(myanmarData.month) == criteria.exactMonth);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Month=" + getMyanmarMonthName(myanmarData.month);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.monthRangeStart >= 0) {
                totalCriteria++;
                int monthValue = static_cast<int>(myanmarData.month);
                bool match = (monthValue >= criteria.monthRangeStart && monthValue <= criteria.monthRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Month=" + std::to_string(criteria.monthRangeStart) + "-" + std::to_string(criteria.monthRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Moon phase criteria
            if (criteria.exactMoonPhase >= 0) {
                totalCriteria++;
                bool match = (static_cast<int>(myanmarData.moonPhase) == criteria.exactMoonPhase);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "MoonPhase=" + getMoonPhaseName(myanmarData.moonPhase);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.moonPhaseRangeStart >= 0) {
                totalCriteria++;
                int moonPhaseValue = static_cast<int>(myanmarData.moonPhase);
                bool match = (moonPhaseValue >= criteria.moonPhaseRangeStart && moonPhaseValue <= criteria.moonPhaseRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "MoonPhase=" + std::to_string(criteria.moonPhaseRangeStart) + "-" + std::to_string(criteria.moonPhaseRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Weekday criteria
            if (criteria.exactWeekday >= 0) {
                totalCriteria++;
                bool match = (weekday == criteria.exactWeekday);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Weekday=" + getMyanmarWeekdayName(static_cast<MyanmarWeekday>(weekday));
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Fortnight day criteria
            if (criteria.exactFortnightDay > 0) {
                totalCriteria++;
                bool match = (myanmarData.fortnightDay == criteria.exactFortnightDay);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "FortnightDay=" + std::to_string(criteria.exactFortnightDay);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.fortnightDayRangeStart > 0) {
                totalCriteria++;
                bool match = (myanmarData.fortnightDay >= criteria.fortnightDayRangeStart &&
                    myanmarData.fortnightDay <= criteria.fortnightDayRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "FortnightDay=" + std::to_string(criteria.fortnightDayRangeStart) + "-" + std::to_string(criteria.fortnightDayRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Astrological criteria
            if (criteria.searchSabbath) {
                totalCriteria++;
                bool match = myanmarData.isSabbath;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Sabbath";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchSabbathEve) {
                totalCriteria++;
                bool match = myanmarData.isSabbathEve;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "SabbathEve";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchYatyaza) {
                totalCriteria++;
                bool match = myanmarData.isYatyaza;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Yatyaza";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchPyathada) {
                totalCriteria++;
                bool match = myanmarData.isPyathada;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Pyathada";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchThamanyo) {
                totalCriteria++;
                bool match = myanmarData.isThamanyo;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Thamanyo";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Calculate match score
            if (totalCriteria > 0) {
                matchScore = static_cast<double>(matchCount) / static_cast<double>(totalCriteria);
            } else {
                // If no specific criteria are provided, return all days with base score
                matchScore = 0.5;
                matchDescription = "All days (no specific criteria)";
            }

            // Add to results if there's a match (or if no criteria specified)
            if (isMatch && matchScore > 0.0) {
                SearchResult result;
                result.gregorianDate = dateBuffer;
                result.myanmarData = myanmarData;
                result.julianDay = jd;
                result.weekday = weekday;
                result.matchScore = matchScore;
                result.matchDescription = matchDescription;

                results.push_back(result);
            }

        } catch (const std::exception& e) {
            // Skip this day on error
            continue;
        }
    }

    // Sort results by match score (descending) then by date (ascending)
    std::sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        if (a.matchScore != b.matchScore) {
            return a.matchScore > b.matchScore;
        }
        return a.julianDay < b.julianDay;
    });

    return results;
}

// Search by specific moon phase
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchByMoonPhase(int moonPhase, const std::string& startDate, const std::string& endDate, double latitude, double longitude, bool exactMatch) const {
    SearchCriteria criteria;
    criteria.exactMoonPhase = moonPhase;
    criteria.exactMatch = exactMatch;
    criteria.nearMatchTolerance = 1;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchMyanmarCalendar(criteria, latitude, longitude);
}

// Search by weekday
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchByWeekday(int weekday, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactWeekday = weekday;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchMyanmarCalendar(criteria, latitude, longitude);
}

// Search by month
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchByMonth(int month, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactMonth = month;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchMyanmarCalendar(criteria, latitude, longitude);
}

// Search by year
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchByYear(int year, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactYear = year;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchMyanmarCalendar(criteria, latitude, longitude);
}

// Search by sabbath days
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchBySabbath(bool includeSabbathEve, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchSabbath = true;
    criteria.searchSabbathEve = includeSabbathEve;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchMyanmarCalendar(criteria, latitude, longitude);
}

// Multi-criteria search: Full Moon OR Yatyaza
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchFullMoonOrYatyaza(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactMoonPhase = static_cast<int>(MyanmarMoonPhase::FULL_MOON);
    criteria.searchYatyaza = true;
    criteria.logicMode = LogicMode::OR;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchMyanmarCalendar(criteria, latitude, longitude);
}

// Multi-criteria search: Sabbath AND Full Moon
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchSabbathAndFullMoon(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactMoonPhase = static_cast<int>(MyanmarMoonPhase::FULL_MOON);
    criteria.searchSabbath = true;
    criteria.logicMode = LogicMode::AND;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchMyanmarCalendar(criteria, latitude, longitude);
}

// Generic multi-criteria search with specified logic mode
std::vector<MyanmarCalendar::SearchResult> MyanmarCalendar::searchMultiCriteria(const SearchCriteria& criteria, LogicMode logicMode, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria modifiedCriteria = criteria;
    modifiedCriteria.logicMode = logicMode;
    modifiedCriteria.searchStartDate = startDate;
    modifiedCriteria.searchEndDate = endDate;

    return searchMyanmarCalendar(modifiedCriteria, latitude, longitude);
}

bool MyanmarCalendar::gregorianToMyanmar(int gy, int gm, int gd, MyanmarCalendarData& result) const {
    double jd = swe_julday(gy, gm, gd, 12.0, SE_GREG_CAL);
    result = calculateMyanmarCalendar(jd);
    return true;
}

bool MyanmarCalendar::myanmarToGregorian(int my, int mm, int moonPhase, int fortnightDay, int& gy, int& gm, int& gd, MyanmarCalendarData* resultData) const {
    long myt, tg1, fm, werr;
    calculateMyanmarYear(my, myt, tg1, fm, werr);
    long md = calculateDayOfMonth(fortnightDay, moonPhase, mm, myt);
    return myanmarToGregorianByDay(my, mm, md, gy, gm, gd, resultData);
}

bool MyanmarCalendar::myanmarToGregorianByDay(int my, int mm, int md, int& gy, int& gm, int& gd, MyanmarCalendarData* resultData) const {
    long myt, tg1, fm, werr;
    calculateMyanmarYear(my, myt, tg1, fm, werr);
    long mml = calculateMonthLength(mm, myt);
    if (md < 1) md = 1;
    if (md > mml) md = mml;
    long jdn = myanmarToJulian(my, mm, md);
    double hour = 12.0;
    swe_revjul(static_cast<double>(jdn), SE_GREG_CAL, &gy, &gm, &gd, &hour);
    if (resultData) {
        *resultData = calculateMyanmarCalendar(static_cast<double>(jdn));
    }
    return true;
}

std::string MyanmarCalendar::formatBidirectionalConversion(const MyanmarCalendarData& data, int gy, int gm, int gd) {
    std::ostringstream oss;
    oss << "===============================================================================\n";
    oss << "                 MYANMAR <-> ENGLISH DATE CONVERSION RESULT                    \n";
    oss << "===============================================================================\n\n";

    oss << " [GREGORIAN CALENDAR DATE]\n";
    oss << "   Western Date : " << gy << "-" 
        << std::setw(2) << std::setfill('0') << gm << "-"
        << std::setw(2) << std::setfill('0') << gd << "\n";
    oss << "   Julian Day   : " << std::fixed << std::setprecision(1) << data.julianDay << "\n\n";

    oss << " [MYANMAR LUNISOLAR CALENDAR DATE]\n";
    oss << "   Myanmar Year : " << data.myanmarYear << " ME (" << data.sasanaYear << " SE)\n";
    oss << "   Year Type    : ";
    switch (data.yearType) {
        case MyanmarYearType::COMMON: oss << "Common Year (354 days)\n"; break;
        case MyanmarYearType::LITTLE_WATAT: oss << "Little Watat (384 days)\n"; break;
        case MyanmarYearType::BIG_WATAT: oss << "Big Watat (385 days)\n"; break;
    }
    oss << "   Month & Day  : " << data.getFormattedDate() << "\n";
    oss << "   Moon Phase   : " << data.getFormattedMoonPhase() 
        << " (Fortnight Day " << data.fortnightDay << ", Month Day " << data.dayOfMonth << "/" << data.monthLength << ")\n";
    oss << "   Weekday      : ";
    switch (data.weekday) {
        case MyanmarWeekday::SATURDAY: oss << "Saturday\n"; break;
        case MyanmarWeekday::SUNDAY: oss << "Sunday\n"; break;
        case MyanmarWeekday::MONDAY: oss << "Monday\n"; break;
        case MyanmarWeekday::TUESDAY: oss << "Tuesday\n"; break;
        case MyanmarWeekday::WEDNESDAY: oss << "Wednesday\n"; break;
        case MyanmarWeekday::THURSDAY: oss << "Thursday\n"; break;
        case MyanmarWeekday::FRIDAY: oss << "Friday\n"; break;
        default: oss << "Unknown\n"; break;
    }

    oss << "\n [CROSS-CALENDAR EQUIVALENTS (GEOLIB INTEGRATION)]\n";
    // Thai (B.E.) Calendar
    int thaiBeYear = gy + 543;
    const char* mNames[] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    long mmIndex = static_cast<long>(data.month);
    bool isLeapMonth = (mmIndex == 0);
    int thaiLunarMonth = isLeapMonth ? 8 : (((mmIndex + 3) % 12 == 0) ? 12 : ((mmIndex + 3) % 12));
    std::string thaiPhase = (data.moonPhase == MyanmarMoonPhase::FULL_MOON) ? "Full Moon (Wan Phra)" :
                            (data.moonPhase == MyanmarMoonPhase::NEW_MOON) ? "New Moon (Wan Phra)" :
                            (data.moonPhase == MyanmarMoonPhase::WAXING) ? ("Waxing " + std::to_string(data.fortnightDay)) :
                            ("Waning " + std::to_string(data.fortnightDay));
    if (data.fortnightDay == 8) thaiPhase += " (Wan Phra)";
    oss << "   Thai (B.E.)  : " << gd << " " << ((gm >= 1 && gm <= 12) ? mNames[gm] : "") 
        << " B.E. " << thaiBeYear << " (" << thaiPhase << ", Month " << thaiLunarMonth << (isLeapMonth ? "-8" : "") << ")\n";

    // Chinese Lunar Calendar
    if (gy >= 1900 && gy <= 2100) {
        static const int cYearInfo[] = {
            0x04bd8, 0x04ae0, 0x0a570, 0x054d5, 0x0d260, 0x0d950, 0x16554, 0x056a0, 0x09ad0, 0x055d2,
            0x04ae0, 0x0a5b6, 0x0a4d0, 0x0d250, 0x1d255, 0x0b540, 0x0d6a0, 0x0ada2, 0x095b0, 0x14977,
            0x04970, 0x0a4b0, 0x0b4b5, 0x06a50, 0x06d40, 0x1ab54, 0x02b60, 0x09570, 0x052f2, 0x04970,
            0x06566, 0x0d4a0, 0x0ea50, 0x06e95, 0x05ad0, 0x02b60, 0x186e3, 0x092e0, 0x1c8d7, 0x0c950,
            0x0d4a0, 0x1d8a6, 0x0b550, 0x056a0, 0x1a5b4, 0x025d0, 0x092d0, 0x0d2b2, 0x0a950, 0x0b557,
            0x06ca0, 0x0b550, 0x15355, 0x04da0, 0x0a5b0, 0x14573, 0x052b0, 0x0a9a8, 0x0e950, 0x06aa0,
            0x0aea6, 0x0ab50, 0x04b60, 0x0aae4, 0x0a570, 0x05260, 0x0f263, 0x0d950, 0x05b57, 0x056a0,
            0x096d0, 0x04dd5, 0x04ad0, 0x0a4d0, 0x0d4d4, 0x0d250, 0x0d558, 0x0b540, 0x0b5a0, 0x195a6,
            0x095b0, 0x049b0, 0x0a974, 0x0a4b0, 0x0b27a, 0x06a50, 0x06d40, 0x0af46, 0x0ab60, 0x09570,
            0x04af5, 0x04970, 0x064b0, 0x074a3, 0x0ea50, 0x06b58, 0x05ac0, 0x0ab60, 0x096d5, 0x092e0,
            0x0c960, 0x0d954, 0x0d4a0, 0x0da50, 0x07552, 0x056a0, 0x0abb7, 0x025d0, 0x092d0, 0x0cab5,
            0x0a950, 0x0b4a0, 0x0baa4, 0x0ad50, 0x055d9, 0x04ba0, 0x0a5b0, 0x15176, 0x052b0, 0x0a930,
            0x07954, 0x06aa0, 0x0ad50, 0x05b52, 0x04b60, 0x0a6e6, 0x0a4e0, 0x0d260, 0x0ea65, 0x0d530,
            0x05aa0, 0x076a3, 0x096d0, 0x04bd7, 0x04ad0, 0x0a4d0, 0x1d0b6, 0x0d250, 0x0d520, 0x0dd45,
            0x0b5a0, 0x056d0, 0x055b2, 0x049b0, 0x0a577, 0x0a4b0, 0x0aa50, 0x1b255, 0x06d20, 0x0ada0,
            0x14b63, 0x09370, 0x049f8, 0x04970, 0x064b0, 0x168a6, 0x0ea50, 0x06b20, 0x1a6c4, 0x0aae0,
            0x0a2e0, 0x0d2e3, 0x0c960, 0x0d557, 0x0d4a0, 0x0da50, 0x05d55, 0x056a0, 0x0a6d0, 0x055d4,
            0x052d0, 0x0a9b8, 0x0a950, 0x0b4a0, 0x0b6a6, 0x0ad50, 0x055a0, 0x0aba4, 0x0a5b0, 0x052b0,
            0x0b273, 0x06930, 0x07337, 0x06aa0, 0x0ad50, 0x14b55, 0x04b60, 0x0a570, 0x054e4, 0x0d160,
            0x0e968, 0x0d520, 0x0daa0, 0x16aa6, 0x056d0, 0x04ae0, 0x0a9d4, 0x0a2d0, 0x0d150, 0x0f252,
            0x0d520
        };
        auto toJdnCivil = [](int y, int m, int d) {
            int a = (14 - m) / 12;
            int yP = y + 4800 - a;
            int mP = m + 12 * a - 3;
            return d + (153 * mP + 2) / 5 + 365 * yP + yP / 4 - yP / 100 + yP / 400 - 32045;
        };
        long targetJdn = toJdnCivil(gy, gm, gd);
        long baseJdn = toJdnCivil(1900, 1, 31);
        long offset = targetJdn - baseJdn;
        if (offset >= 0) {
            int cYear = 1900;
            while (cYear <= 2100) {
                int info = cYearInfo[cYear - 1900];
                int days = 348;
                for (int mask = 0x8000; mask > 0x8; mask >>= 1) {
                    if (info & mask) days++;
                }
                int lm = info & 0xf;
                if (lm != 0) days += ((info & 0x10000) ? 30 : 29);
                if (offset < days) break;
                offset -= days;
                cYear++;
            }
            if (cYear <= 2100) {
                int info = cYearInfo[cYear - 1900];
                int lm = info & 0xf;
                int cMonth = 1;
                bool cLeap = false;
                while (cMonth <= 12) {
                    int mDays = (cLeap) ? ((info & 0x10000) ? 30 : 29) : ((info & (0x10000 >> cMonth)) ? 30 : 29);
                    if (offset < mDays) break;
                    offset -= mDays;
                    if (lm == cMonth && !cLeap) {
                        cLeap = true;
                    } else {
                        if (cLeap) cLeap = false;
                        cMonth++;
                    }
                }
                long cDay = offset + 1;
                const char* stems[] = {"Jia", "Yi", "Bing", "Ding", "Wu", "Ji", "Geng", "Xin", "Ren", "Gui"};
                const char* branches[] = {"Zi", "Chou", "Yin", "Mao", "Chen", "Si", "Wu", "Wei", "Shen", "You", "Xu", "Hai"};
                const char* animals[] = {"Rat", "Ox", "Tiger", "Rabbit", "Dragon", "Snake", "Horse", "Goat", "Monkey", "Rooster", "Dog", "Pig"};
                int cOff = cYear - 4;
                std::string stemBranch = std::string(stems[(cOff % 10 + 10) % 10]) + "-" + branches[(cOff % 12 + 12) % 12];
                std::string animal = animals[(cOff % 12 + 12) % 12];

                oss << "   Chinese Lunar: Year " << cYear << " (" << stemBranch << " " << animal << " Year), "
                    << (cLeap ? "Leap Month " : "Month ") << cMonth << ", Day " << cDay << "\n";
            }
        }
    }

    oss << "\n [ASTROLOGICAL ATTRIBUTES]\n";
    oss << "   Mahabote     : ";
    switch (data.mahabote) {
        case Mahabote::BINGA: oss << "Binga\n"; break;
        case Mahabote::ATUN: oss << "Atun\n"; break;
        case Mahabote::YAZA: oss << "Yaza\n"; break;
        case Mahabote::ADIPATI: oss << "Adipati\n"; break;
        case Mahabote::MARANA: oss << "Marana\n"; break;
        case Mahabote::THIKE: oss << "Thike\n"; break;
        case Mahabote::PUTI: oss << "Puti\n"; break;
        default: oss << "Unknown\n"; break;
    }
    oss << "   Nakhat       : ";
    switch (data.nakhat) {
        case Nakhat::ORC: oss << "Balu (Ogre)\n"; break;
        case Nakhat::ELF: oss << "Nat (Elf)\n"; break;
        case Nakhat::HUMAN: oss << "Lu (Human)\n"; break;
        default: oss << "Unknown\n"; break;
    }
    oss << "   Nagahle Head : ";
    switch (data.nagahle) {
        case NagahleDirection::WEST: oss << "West\n"; break;
        case NagahleDirection::NORTH: oss << "North\n"; break;
        case NagahleDirection::EAST: oss << "East\n"; break;
        case NagahleDirection::SOUTH: oss << "South\n"; break;
        default: oss << "Unknown\n"; break;
    }

    std::vector<std::string> astroFlags;
    if (data.isSabbath) astroFlags.push_back("Buddhist Sabbath Day");
    if (data.isSabbathEve) astroFlags.push_back("Sabbath Eve");
    if (data.isYatyaza) astroFlags.push_back("Yatyaza (Auspicious)");
    if (data.isPyathada) astroFlags.push_back("Pyathada (Inauspicious)");
    if (data.isAfternoonPyathada) astroFlags.push_back("Afternoon Pyathada (Inauspicious)");
    if (data.isThamanyo) astroFlags.push_back("Thamanyo (Inauspicious)");
    if (data.isAmyeittasote) astroFlags.push_back("Amyeittasote (Auspicious)");
    if (data.isWarameittugyi) astroFlags.push_back("Warameittugyi");
    if (data.isWarameittunge) astroFlags.push_back("Warameittunge");

    oss << "   Astro Days   : ";
    if (astroFlags.empty()) {
        oss << "Regular day (No special restrictions)\n";
    } else {
        for (size_t i = 0; i < astroFlags.size(); ++i) {
            oss << astroFlags[i] << (i + 1 < astroFlags.size() ? ", " : "\n");
        }
    }

    oss << "\n [TRADITIONAL ASTRONOMICAL COORDINATES]\n";
    oss << "   Ahargana Days: " << data.aharganaDays << " days since ME 0 epoch\n";
    oss << "   Madhyama Ravi: " << std::fixed << std::setprecision(3) << data.sunMeanLonDeg << " deg (Mean Sun)\n";
    oss << "   Spashta Ravi : " << data.sunTrueLonDeg << " deg (True Sun)\n";
    oss << "   Spashta Moon : " << data.moonTrueLonDeg << " deg (True Moon)\n";
    oss << "   Thitja Deg   : " << data.thitjaDeg << " deg (Solar Anomaly)\n";
    oss << "   Avamanna     : " << data.avamanna << " (Avama Remainder)\n";

    if (!data.festivals.empty()) {
        oss << "\n [FESTIVALS & OBSERVANCES]\n";
        for (const auto& f : data.festivals) {
            oss << "   • " << f << "\n";
        }
    }
    if (!data.holidays.empty()) {
        oss << "\n [PUBLIC & CULTURAL HOLIDAYS]\n";
        for (const auto& h : data.holidays) {
            oss << "   • " << h << "\n";
        }
    }
    oss << "===============================================================================\n";
    return oss.str();
}

std::string MyanmarCalendarData::getFormattedDate() const {
    std::string mName = MyanmarCalendar::getMyanmarMonthName(month);
    std::string pName = MyanmarCalendar::getMoonPhaseName(moonPhase);
    if (moonPhase == MyanmarMoonPhase::FULL_MOON) {
        return mName + " Full Moon Day";
    } else if (moonPhase == MyanmarMoonPhase::NEW_MOON) {
        return mName + " New Moon Day";
    }
    return mName + " " + pName + " " + std::to_string(fortnightDay);
}

std::string MyanmarCalendarData::getFormattedMoonPhase() const {
    return MyanmarCalendar::getMoonPhaseName(moonPhase);
}

std::string MyanmarCalendarData::getAstrologicalSummary() const {
    std::vector<std::string> flags;
    if (isSabbath) flags.push_back("Buddhist Sabbath Day");
    if (isSabbathEve) flags.push_back("Sabbath Eve");
    if (isYatyaza) flags.push_back("Yatyaza");
    if (isPyathada) flags.push_back("Pyathada");
    if (isAfternoonPyathada) flags.push_back("Afternoon Pyathada");
    if (isThamanyo) flags.push_back("Thamanyo");
    if (isAmyeittasote) flags.push_back("Amyeittasote");
    if (isWarameittugyi) flags.push_back("Warameittugyi");
    if (isWarameittunge) flags.push_back("Warameittunge");
    if (isYatpote) flags.push_back("Yatpote");
    if (isThamaphyu) flags.push_back("Thamaphyu");
    if (flags.empty()) return "Regular Day";
    std::string res;
    for (size_t i = 0; i < flags.size(); ++i) {
        if (i > 0) res += ", ";
        res += flags[i];
    }
    return res;
}

std::string MyanmarCalendarData::getFullDescription() const {
    return std::to_string(myanmarYear) + " ME, " + getFormattedDate() + " (" + getAstrologicalSummary() + ")";
}

} // namespace Astro
