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

namespace Astro {

/////////////////////////////////////////////////////////////////////////////
// Myanmar Calendar Constants and Exception Data (from yan9a/mmcal)
/////////////////////////////////////////////////////////////////////////////

// Full moon day offset exceptions [my, offset]
static const std::vector<std::array<long, 2>> fullMoonOffsetExceptions = {
    {1120, -1}, {1126, -1}, {1150, 1}, {1152, -1}, {1161, -1}, {1162, -1}, {1172, -1},
    {1181, -1}, {1190, 1}, {1191, -1}, {1194, -1}, {1195, -1}, {1198, -1}, {1201, 1},
    {1202, 1}, {1208, 1}, {1215, -1}, {1217, -1}, {1218, -1}, {1221, 1}, {1234, 1},
    {1235, -1}, {1236, -1}, {1237, -1}, {1238, -1}, {1239, -1}, {1241, -1}, {1242, -1},
    {1244, 1}, {1245, 1}, {1253, -1}, {1258, 1}, {1264, 1}, {1291, -1}, {1292, -1},
    {1298, 1}, {1309, 1}, {1310, 1}, {1315, 1}, {1324, 1}, {1344, 1}, {1345, 1}
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

// Get Myanmar year constants depending on era
// Based on yan9a/mmcal GetMyConst function
void MyanmarCalendar::getMyanmarConstants(long my, double& EI, double& WO, double& NM, long& EW) {
    // Default values
    EI = 3; WO = 0; NM = 0; EW = 0;

    // Era determination
    if (my >= 1312) {
        EI = 3; NM = 0;  // 3rd era
    } else if (my >= 1217) {
        EI = 2; NM = 4;  // 2nd era
    } else {
        EI = 1; NM = 0;  // 1st era
    }

    // Apply full moon offset exceptions
    for (const auto& exception : fullMoonOffsetExceptions) {
        if (exception[0] == my) {
            WO += exception[1];
            break;
        }
    }

    // Apply watat exceptions
    for (long exceptionYear : watatExceptions) {
        if (exceptionYear == my) {
            EW = 1;
            break;
        }
    }
}

// Check watat (intercalary month) - yan9a/mmcal algorithm
void MyanmarCalendar::calculateWatat(long my, long& watat, long& fm) {
    double SY = SOLAR_YEAR;          // 365.2587565
    double LM = LUNAR_MONTH;         // 29.53058795
    double MO = MYANMAR_EPOCH;       // 1954168.050623

    double EI, WO, NM;
    long EW;
    getMyanmarConstants(my, EI, WO, NM, EW);

    double TA = (SY/12 - LM) * (12 - NM); // threshold to adjust
    double ed = fmod(SY * (my + 3739), LM); // excess day
    if (ed < TA) ed += LM; // adjust excess days

    fm = static_cast<long>(round(SY * my + MO - ed + 4.5 * LM + WO)); // full moon day of 2nd Waso
    double TW = 0;
    watat = 0; // find watat

    if (EI >= 2) { // if 2nd era or later find watat based on excess days
        TW = LM - (SY/12 - LM) * NM;
        if (ed >= TW) watat = 1;
    } else { // if 1st era, find watat by 19 years metonic cycle
        watat = (my * 7 + 2) % 19;
        if (watat < 0) watat += 19;
        watat = static_cast<long>(floor(watat / 12));
    }
    watat ^= EW; // correct watat exceptions
}

// Check Myanmar Year - yan9a/mmcal algorithm
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
        myt = static_cast<long>(floor(nd / 31) + 1);
        fm = y2fm;
        if (nd != 30 && nd != 31) {
            werr = 1;
        }
    } else {
        fm = y1fm + 354 * yd;
    }
    tg1 = y1fm + 354 * yd - 102;
}

// Julian day number to Myanmar date - yan9a/mmcal algorithm
void MyanmarCalendar::julianToMyanmar(double jd, long& myt, long& my, long& mm, long& md) {
    long jdn = static_cast<long>(round(jd)); // convert jdn to integer
    double SY = SOLAR_YEAR; // solar year (365.2587565)
    double MO = MYANMAR_EPOCH; // beginning of 0 ME
    long dd, myl, mmt, a, b, c, e, f;
    long tg1, fm, werr;

    my = static_cast<long>(floor((jdn - 0.5 - MO) / SY)); // Myanmar year
    calculateMyanmarYear(my, myt, tg1, fm, werr); // check year
    dd = jdn - tg1 + 1; // day count
    b = static_cast<long>(floor(myt / 2));
    c = static_cast<long>(floor(1 / (myt + 1))); // big wa and common yr
    myl = 354 + (1 - c) * 30 + b; // year length

    mmt = static_cast<long>(floor((dd - 1) / myl)); // month type: late =1 or early = 0
    dd -= mmt * myl;
    a = static_cast<long>(floor((dd + 423) / 512)); // adjust day count and threshold
    mm = static_cast<long>(floor((dd - b * a + c * a * 30 + 29.26) / 29.544)); // month
    e = static_cast<long>(floor((mm + 12) / 16));
    f = static_cast<long>(floor((mm + 11) / 16));
    md = dd - static_cast<long>(floor(29.544 * mm - 29.26)) - b * e + c * f * 30; // day
    mm += f * 3 - e * 4 + 12 * mmt; // adjust month numbers for late months
}

// Myanmar date to Julian day number - yan9a/mmcal algorithm
long MyanmarCalendar::myanmarToJulian(long my, long mm, long md) {
    long b, c, dd, myl, mmt;
    long myt, tg1, fm, werr;

    calculateMyanmarYear(my, myt, tg1, fm, werr); // check year
    mmt = static_cast<long>(floor(mm / 13));
    mm = mm % 13 + mmt; // to 1-12 with month type
    b = static_cast<long>(floor(myt / 2));
    c = 1 - static_cast<long>(floor((myt + 1) / 2)); // if big watat and common year
    mm += 4 - static_cast<long>(floor((mm + 15) / 16)) * 4 + static_cast<long>(floor((mm + 12) / 16)); // adjust month
    dd = md + static_cast<long>(floor(29.544 * mm - 29.26)) - c * static_cast<long>(floor((mm + 11) / 16)) * 30 + b * static_cast<long>(floor((mm + 12) / 16));
    myl = 354 + (1 - c) * 30 + b;
    dd += mmt * myl; // adjust day count with year length

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

    // Convert birth data to Julian Day
    double jd = 0.0; // This should be calculated from birthData
    // For now, use a placeholder calculation

    return calculateMyanmarCalendar(jd);
}

MyanmarCalendarData MyanmarCalendar::calculateMyanmarCalendar(double julianDay) const {
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
    long weekday = (static_cast<long>(julianDay) + 2) % 7; // 0=sat, 1=sun, ..., 6=fri
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

    // Identify festivals and events
    identifyFestivals(data);
    identifyHolidays(data);
    identifyAstrologicalEvents(data);

    return data;
}

/////////////////////////////////////////////////////////////////////////////
// Festival and Holiday Identification (yan9a/mmcal style)
/////////////////////////////////////////////////////////////////////////////

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
    if (data.isThamanyo || data.isWarameittugyi) {
        data.astrologicalEvents.push_back("Auspicious Day");
    }
    if (data.isPyathada || data.isYatyaza) {
        data.astrologicalEvents.push_back("Inauspicious Day");
    }
    if (data.isSabbath) {
        data.astrologicalEvents.push_back("Buddhist Sabbath");
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

long MyanmarCalendar::binarySearch2(long key, const std::vector<std::array<long, 2>>& array) {
    long i = 0, u = array.size() - 1, m;
    while (u >= i) {
        m = (i + u) / 2;
        if (array[m][0] == key) return m;
        else if (array[m][0] > key) u = m - 1;
        else i = m + 1;
    }
    return -1;
}

/////////////////////////////////////////////////////////////////////////////
// Output Formatting Methods
/////////////////////////////////////////////////////////////////////////////

std::string MyanmarCalendar::generateTable(const MyanmarCalendarData& data) const {
    std::stringstream ss;

    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "🇲🇲 MYANMAR CALENDAR (yan9a/mmcal Implementation) 🇲🇲\n";
    ss << "═══════════════════════════════════════════════════════════════════\n";

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
    if (data.isThamanyo) ss << "   • Thamanyo (Auspicious)\n";
    if (data.isWarameittugyi) ss << "   • Warameittugyi (Great Auspicious)\n";
    if (data.isWarameittunge) ss << "   • Warameittunge (Lesser Auspicious)\n";
    if (data.isAmyeittasote) ss << "   • Amyeittasote (Moderately Auspicious)\n";
    if (data.isYatyaza) ss << "   • Yatyaza (Inauspicious)\n";
    if (data.isPyathada) ss << "   • Pyathada (Very Inauspicious)\n";
    if (data.isAfternoonPyathada) ss << "   • Afternoon Pyathada\n";

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

    // Recommendations
    ss << "\n💡 RECOMMENDATIONS:\n";
    if (data.isSabbath) {
        ss << "   Status: 🙏 Religious Observance\n";
        ss << "   Note: Observe Buddhist precepts, visit pagodas, practice meditation\n";
    } else if (data.isWarameittugyi || data.isThamanyo) {
        ss << "   Status: ✅ Auspicious Day\n";
        ss << "   Note: Good day for starting new ventures and important activities\n";
    } else if (data.isPyathada || data.isYatyaza) {
        ss << "   Status: ⚠️  Inauspicious Day\n";
        ss << "   Note: Avoid important activities, postpone new beginnings\n";
    } else {
        ss << "   Status: ⚪ Neutral Day\n";
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

std::string MyanmarCalendar::getMyanmarMonthName(MyanmarMonth month) const {
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

std::string MyanmarCalendar::getMyanmarWeekdayName(MyanmarWeekday weekday) const {
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

std::string MyanmarCalendar::getMahaboteName(Mahabote mahabote) const {
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

std::string MyanmarCalendar::getNakhatName(Nakhat nakhat) const {
    switch (nakhat) {
        case Nakhat::ORC: return "Orc";
        case Nakhat::ELF: return "Elf";
        case Nakhat::HUMAN: return "Human";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getNagahleDirectionName(NagahleDirection direction) const {
    switch (direction) {
        case NagahleDirection::WEST: return "West";
        case NagahleDirection::NORTH: return "North";
        case NagahleDirection::EAST: return "East";
        case NagahleDirection::SOUTH: return "South";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getMoonPhaseName(MyanmarMoonPhase phase) const {
    switch (phase) {
        case MyanmarMoonPhase::WAXING: return "Waxing";
        case MyanmarMoonPhase::FULL_MOON: return "Full Moon";
        case MyanmarMoonPhase::WANING: return "Waning";
        case MyanmarMoonPhase::NEW_MOON: return "New Moon";
        default: return "Unknown";
    }
}

std::string MyanmarCalendar::getYearTypeName(MyanmarYearType type) const {
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

} // namespace Astro
