#include "myanmar_calendar.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <ctime>
#include <array>

extern "C" {
#include "swephexp.h"
}

namespace Astro {

MyanmarCalendar::MyanmarCalendar() : initialized(false) {
}

MyanmarCalendar::~MyanmarCalendar() {
}

bool MyanmarCalendar::initialize() {
    try {
        initializeMonthData();
        initializeAstrologicalDays();
        initializeFestivalData();
        initializeExceptionData();

        initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError = "Failed to initialize Myanmar Calendar: " + std::string(e.what());
        return false;
    }
}

void MyanmarCalendar::initializeMonthData() {
    monthData = {
        {"1st Waso", "ပထမ ဝါဆို", "Intercalary month in watat years", {}, true},
        {"Tagu", "တန်ခူး", "First month of Myanmar year", {"Thingyan Water Festival", "Myanmar New Year"}, true},
        {"Kason", "ကဆုန်", "Month of Vesak", {"Vesak Full Moon", "Buddha's Birthday"}, true},
        {"Nayon", "နယုန်", "Hot season month", {"Pre-monsoon festivals"}, false},
        {"Waso", "ဝါဆို", "Beginning of Buddhist Lent", {"Beginning of Buddhist Lent", "Waso Full Moon"}, true},
        {"Wagaung", "ဝါခေါင်", "Monsoon month", {"Nat festivals"}, false},
        {"Tawthalin", "တော်သလင်း", "End of monsoon", {"End of monsoon ceremonies"}, false},
        {"Thadingyut", "သီတင်းကျွတ်", "End of Buddhist Lent", {"Thadingyut Festival", "Festival of Lights"}, true},
        {"Tazaungmon", "တန်ဆောင်မုန်း", "Cool season begins", {"Tazaungmon Festival", "Kathina ceremonies"}, true},
        {"Nadaw", "နတ်တော်", "Cool season", {"Nadaw festivals"}, false},
        {"Pyatho", "ပြာသို", "Cold season", {"Ananda Pagoda Festival"}, false},
        {"Tabodwe", "တပေါင်းတွေ", "End of cold season", {"Htamane festivals"}, false},
        {"Tabaung", "တပေါင်း", "Hot season begins", {"Shwedagon Pagoda Festival"}, true},
        {"Late Tagu", "နှောင်းတန်ခူး", "Late first month", {}, false},
        {"Late Kason", "နှောင်းကဆုန်", "Late second month", {}, false}
    };
}

void MyanmarCalendar::initializeAstrologicalDays() {
    astrologicalDays = {
        {"sabbath", {"Sabbath", "Buddhist sabbath day for religious observances", false, "Observe eight precepts, visit pagodas"}},
        {"sabbath_eve", {"Sabbath Eve", "Day before sabbath", false, "Prepare for religious observances"}},
        {"yatyaza", {"Yatyaza", "Inauspicious day for starting new ventures", false, "Avoid new beginnings, postpone important work"}},
        {"pyathada", {"Pyathada", "Very inauspicious day", false, "Avoid all important activities"}},
        {"afternoon_pyathada", {"Afternoon Pyathada", "Afternoon is inauspicious", false, "Avoid afternoon activities"}},
        {"thamanyo", {"Thamanyo", "Auspicious day", true, "Good for starting new ventures"}},
        {"amyeittasote", {"Amyeittasote", "Moderately auspicious", true, "Good for routine activities"}},
        {"warameittugyi", {"Warameittugyi", "Great auspicious day", true, "Excellent for all activities"}},
        {"warameittunge", {"Warameittunge", "Lesser auspicious day", true, "Good for most activities"}},
        {"yatpote", {"Yatpote", "Neutral day", true, "Normal activities permitted"}},
        {"thamaphyu", {"Thamaphyu", "Protective day", true, "Good for defensive activities"}},
        {"nagapor", {"Nagapor", "Dragon day", true, "Special protective qualities"}},
        {"yatyotema", {"Yatyotema", "Day requiring caution", false, "Be careful in all activities"}},
        {"mahayatkyan", {"Mahayatkyan", "Great caution day", false, "Extra care needed"}},
        {"shanyat", {"Shanyat", "Day of conflict", false, "Avoid confrontations"}}
    };
}

void MyanmarCalendar::initializeFestivalData() {
    festivalMap = {
        {"tagu_festivals", {"Thingyan Water Festival", "Myanmar New Year", "Bo Tree Watering"}},
        {"kason_festivals", {"Vesak Full Moon", "Buddha's Birthday", "Enlightenment Day", "Parinirvana Day"}},
        {"waso_festivals", {"Beginning of Buddhist Lent", "Dhammacakka Day", "Waso Robe Offering"}},
        {"thadingyut_festivals", {"End of Buddhist Lent", "Thadingyut Festival", "Festival of Lights", "Pavarana Day"}},
        {"tazaungmon_festivals", {"Tazaungmon Festival", "Kathina Robe Offering", "Hot Air Balloon Festival"}},
        {"tabaung_festivals", {"Shwedagon Pagoda Festival", "Magha Puja Day"}},
        {"nat_festivals", {"37 Nats Festival", "Mount Popa Festival", "Spirit Festivals"}}
    };
}

void MyanmarCalendar::initializeExceptionData() {
    // Initialize exception arrays based on historical Myanmar calendar data
    // This is simplified - the full data would require extensive historical records

    // Full moon offset exceptions (year, offset)
    fullMoonOffsetExceptions = {
        {{1100, 1}}, {{1120, -1}}, {{1140, 1}}, {{1160, -1}}, {{1180, 1}},
        {{1200, -1}}, {{1220, 1}}, {{1240, -1}}, {{1260, 1}}, {{1280, -1}}
        // This would continue with historical data
    };

    // Watat exceptions
    watatExceptions = {
        1201, 1202, 1207, 1209, 1212, 1215, 1218, 1221, 1222, 1223
        // This would continue with historical watat exception years
    };
}

void MyanmarCalendar::getMyanmarConstants(long myanmarYear, double& eraId, double& watatOffset,
                                        double& monthNumber, long& exceptionWatat) const {
    exceptionWatat = 0;

    // The third era (the era after Independence 1312 ME and after)
    if (myanmarYear >= 1312) {
        eraId = 3.0;
        watatOffset = -0.5;
        monthNumber = 4.0;
    }
    // The second era (the era under British colony: 1217 ME to 1311 ME)
    else if (myanmarYear >= 1217) {
        eraId = 2.0;
        watatOffset = -1.0;
        monthNumber = 4.0;
    }
    // The first era (the era of Myanmar kings: 0 ME to 1216 ME)
    else {
        eraId = 1.0;
        watatOffset = -2.0;
        monthNumber = 4.0;
    }

    // Check for watat exceptions
    if (binarySearch1D(myanmarYear, watatExceptions) >= 0) {
        exceptionWatat = 1;
    }
}

void MyanmarCalendar::calculateWatat(long myanmarYear, long& watat, long& fullMoonDay) const {
    double eraId, watatOffset, monthNumber;
    long exceptionWatat;
    getMyanmarConstants(myanmarYear, eraId, watatOffset, monthNumber, exceptionWatat);

    double thresholdAdjust = (SOLAR_YEAR / 12.0 - LUNAR_MONTH) * (12.0 - monthNumber);
    double excessDay = fmod(SOLAR_YEAR * (myanmarYear + 3739), LUNAR_MONTH);

    if (excessDay < thresholdAdjust) {
        excessDay += LUNAR_MONTH;
    }

    fullMoonDay = long(round(SOLAR_YEAR * myanmarYear + MYANMAR_EPOCH - excessDay + 4.5 * LUNAR_MONTH + watatOffset));

    double thresholdWatat = 0.0;
    watat = 0;

    if (eraId >= 2.0) {
        double adjustment = (SOLAR_YEAR * (myanmarYear + 3739) + 1.5 * LUNAR_MONTH) / LUNAR_MONTH;
        adjustment = adjustment - floor(adjustment);
        if (adjustment >= (15.0 / 692.0)) watat = 1;
        thresholdWatat = 0.5;
    } else {
        if (excessDay < 207.0 / 692.0) watat = 1;
        thresholdWatat = (207.0 / 692.0);
    }

    watat ^= exceptionWatat; // Apply watat exceptions
}

void MyanmarCalendar::calculateMyanmarYear(long myanmarYear, long& yearType, long& tagu1,
                                         long& fullMoonDay, long& watatError) const {
    long yearDiff = 0, nearestDiff = 0, y1watat, y1fm, y2watat, y2fm;
    watatError = 0;

    calculateWatat(myanmarYear, y2watat, y2fm);
    yearType = y2watat;

    do {
        yearDiff++;
        calculateWatat(myanmarYear - yearDiff, y1watat, y1fm);
    } while (y1watat == 0 && yearDiff < 3);

    if (yearType) {
        nearestDiff = yearDiff;
        fullMoonDay = y2fm;
        if (y1watat == 0) {
            if (yearDiff > 1) {
                yearType = 2; // Big watat
            } else {
                yearType = 1; // Little watat
            }
        }
    } else {
        fullMoonDay = y1fm;
    }

    tagu1 = y1fm + 354 * yearDiff - 102;
}

void MyanmarCalendar::julianToMyanmarDate(double julianDay, long& yearType, long& myanmarYear,
                                        long& month, long& dayOfMonth) const {
    long julianDayInt = long(round(julianDay));
    long dayDiff, yearLength, monthType, a, b, c, e, f;
    long tagu1, fullMoonDay, watatError;

    myanmarYear = long(floor((julianDayInt - 0.5 - MYANMAR_EPOCH) / SOLAR_YEAR));
    calculateMyanmarYear(myanmarYear, yearType, tagu1, fullMoonDay, watatError);

    dayDiff = julianDayInt - tagu1 + 1;
    b = long(floor(yearType / 2.0));
    c = long(floor(1.0 / (yearType + 1.0)));
    yearLength = 354 + (1 - c) * 30 + b;
    monthType = long(floor((dayDiff - 1) / yearLength));
    dayDiff -= monthType * yearLength;

    a = long(floor((dayDiff + 423) / 512.0));
    month = long(floor((dayDiff - b * a + c * a * 30 + 29.26) / 29.544));
    e = long(floor((month + 12) / 16.0));
    f = long(floor((month + 11) / 16.0));
    dayOfMonth = dayDiff - long(floor(29.544 * month - 29.26)) - b * e + c * f * 30;
    month += f * 3 - e * 4 + 12 * monthType;
}

long MyanmarCalendar::myanmarToJulianDay(long myanmarYear, long month, long dayOfMonth) const {
    long b, c, dayDiff, yearLength, monthType;
    long yearType, tagu1, fullMoonDay, watatError;

    calculateMyanmarYear(myanmarYear, yearType, tagu1, fullMoonDay, watatError);

    monthType = long(floor(month / 13.0));
    month = month % 13 + monthType;

    b = long(floor(yearType / 2.0));
    c = 1 - long(floor((yearType + 1) / 2.0));

    month += 4 - long(floor((month + 15) / 16.0)) * 4 + long(floor((month + 12) / 16.0));

    dayDiff = dayOfMonth + long(floor(29.544 * month - 29.26)) - c * long(floor((month + 11) / 16.0)) * 30
              + b * long(floor((month + 12) / 16.0));

    yearLength = 354 + (1 - c) * 30 + b;
    dayDiff += monthType * yearLength;

    return dayDiff + tagu1 - 1;
}

long MyanmarCalendar::calculateMoonPhase(long dayOfMonth, long month, long yearType) const {
    long monthLength = calculateMonthLength(month, yearType);
    return long(floor((dayOfMonth + 1) / 16.0) + floor(dayOfMonth / 16.0) + floor(dayOfMonth / monthLength));
}

long MyanmarCalendar::calculateMonthLength(long month, long yearType) const {
    long monthLength = 30 - month % 2;
    if (month == 3) { // Nayon in big watat
        monthLength -= long(floor(yearType / 2.0));
    }
    return monthLength;
}

long MyanmarCalendar::calculateYearLength(long yearType) const {
    return 354 + (1 - long(floor(1.0 / (yearType + 1.0)))) * 30 + long(floor(yearType / 2.0));
}

long MyanmarCalendar::calculateFortnightDay(long dayOfMonth) const {
    return dayOfMonth - 15 * long(floor(dayOfMonth / 16.0));
}

long MyanmarCalendar::calculateDayOfMonth(long fortnightDay, long moonPhase, long month, long yearType) const {
    long monthLength = calculateMonthLength(month, yearType);
    long m1 = moonPhase % 2;
    long m2 = long(floor(moonPhase / 2.0));
    return m1 * (15 + m2 * (monthLength - 15)) + (1 - m1) * (fortnightDay + 15 * m2);
}

long MyanmarCalendar::calculateSabbath(long dayOfMonth, long month, long yearType) const {
    long monthLength = calculateMonthLength(month, yearType);
    long sabbath = 0;

    if ((dayOfMonth == 8) || (dayOfMonth == 15) || (dayOfMonth == 23) || (dayOfMonth == monthLength)) {
        sabbath = 1; // Sabbath day
    }
    if ((dayOfMonth == 7) || (dayOfMonth == 14) || (dayOfMonth == 22) || (dayOfMonth == (monthLength - 1))) {
        sabbath = 2; // Sabbath eve
    }

    return sabbath;
}

long MyanmarCalendar::calculateYatyaza(long month, long weekday) const {
    // First waso is considered waso
    if (month <= 0) month = 4;

    long m1 = month % 4;
    long yatyaza = 0;
    long wd1 = long(floor(m1 / 2.0)) + 4;
    long wd2 = ((1 - long(floor(m1 / 2.0))) + m1 % 2) * (1 + 2 * (m1 % 2));

    if ((weekday == wd1) || (weekday == wd2)) {
        yatyaza = 1;
    }

    return yatyaza;
}

long MyanmarCalendar::calculatePyathada(long month, long weekday) const {
    // First waso is considered waso
    if (month <= 0) month = 4;

    long m1 = month % 4;
    long pyathada = 0;
    long wda[] = {1, 3, 3, 0, 2, 1, 2};

    if ((m1 == 0) && (weekday == 4)) {
        pyathada = 2; // Afternoon pyathada
    } else if (m1 == wda[weekday]) {
        pyathada = 1; // Pyathada
    }

    return pyathada;
}

long MyanmarCalendar::calculateNagahle(long month) const {
    if (month <= 0) month = 4; // First waso is considered waso
    return long(floor((month % 12) / 3.0));
}

long MyanmarCalendar::calculateMahabote(long myanmarYear, long weekday) const {
    return (myanmarYear - weekday) % 7;
}

long MyanmarCalendar::calculateNakhat(long myanmarYear) const {
    return myanmarYear % 3;
}

long MyanmarCalendar::calculateThamanyo(long month, long weekday) const {
    long monthType = long(floor(month / 13.0));
    month = month % 13 + monthType;

    if (month <= 0) month = 4;

    long thamanyo = 0;
    long m1 = month - 1 - long(floor(month / 9.0));
    long wd1 = (m1 * 2 - long(floor(m1 / 8.0))) % 7;
    long wd2 = (weekday + 7 - wd1) % 7;

    if (wd2 <= 1) {
        thamanyo = 1;
    }

    return thamanyo;
}

long MyanmarCalendar::calculateAmyeittasote(long dayOfMonth, long weekday) const {
    long fortnightDay = calculateFortnightDay(dayOfMonth);
    long amyeittasote = 0;
    long wda[] = {5, 8, 3, 7, 2, 4, 1};

    if (fortnightDay == wda[weekday]) {
        amyeittasote = 1;
    }

    return amyeittasote;
}

long MyanmarCalendar::calculateWarameittugyi(long dayOfMonth, long weekday) const {
    long fortnightDay = calculateFortnightDay(dayOfMonth);
    long warameittugyi = 0;
    long wda[] = {7, 1, 4, 8, 9, 6, 3};

    if (fortnightDay == wda[weekday]) {
        warameittugyi = 1;
    }

    return warameittugyi;
}

long MyanmarCalendar::calculateWarameittunge(long dayOfMonth, long weekday) const {
    long fortnightDay = calculateFortnightDay(dayOfMonth);
    long warameittunge = 0;
    long weekdayNext = (weekday + 6) % 7;

    if ((12 - fortnightDay) == weekdayNext) {
        warameittunge = 1;
    }

    return warameittunge;
}

long MyanmarCalendar::calculateYatpote(long dayOfMonth, long weekday) const {
    long fortnightDay = calculateFortnightDay(dayOfMonth);
    long yatpote = 0;
    long wda[] = {8, 1, 4, 6, 9, 8, 7};

    if (fortnightDay == wda[weekday]) {
        yatpote = 1;
    }

    return yatpote;
}

long MyanmarCalendar::calculateThamaphyu(long dayOfMonth, long weekday) const {
    long fortnightDay = calculateFortnightDay(dayOfMonth);
    long thamaphyu = 0;
    long wda[] = {1, 2, 6, 6, 5, 6, 7};

    if (fortnightDay == wda[weekday]) {
        thamaphyu = 1;
    }

    return thamaphyu;
}

long MyanmarCalendar::calculateNagapor(long dayOfMonth, long weekday) const {
    long fortnightDay = calculateFortnightDay(dayOfMonth);
    long nagapor = 0;
    long wda[] = {26, 21, 2, 10, 18, 2, 21};

    if (fortnightDay == (wda[weekday] % 15)) {
        nagapor = 1;
    }

    return nagapor;
}

long MyanmarCalendar::calculateYatyotema(long month, long dayOfMonth) const {
    long yatyotema = 0;
    long mwd[][2] = {{2, 7}, {3, 1}, {6, 3}, {1, 7}, {5, 1}};

    if (month >= 1 && month <= 5) {
        if ((dayOfMonth == mwd[month-1][0]) || (dayOfMonth == mwd[month-1][1])) {
            yatyotema = 1;
        }
    }

    return yatyotema;
}

long MyanmarCalendar::calculateMahayatkyan(long month, long dayOfMonth) const {
    long mahayatkyan = 0;
    long mwd[][2] = {{3, 1}, {1, 2}, {6, 1}, {2, 1}, {7, 1}};

    if (month >= 1 && month <= 5) {
        if ((dayOfMonth == mwd[month-1][0]) || (dayOfMonth == mwd[month-1][1])) {
            mahayatkyan = 1;
        }
    }

    return mahayatkyan;
}

long MyanmarCalendar::calculateShanyat(long month, long dayOfMonth) const {
    long shanyat = 0;
    long mwd[][2] = {{8, 9}, {6, 8}, {2, 3}, {1, 3}, {4, 5}};

    if (month >= 1 && month <= 5) {
        if ((dayOfMonth == mwd[month-1][0]) || (dayOfMonth == mwd[month-1][1])) {
            shanyat = 1;
        }
    }

    return shanyat;
}

long MyanmarCalendar::binarySearch2D(long key, const std::vector<std::array<long, 2>>& array) const {
    long low = 0;
    long high = array.size() - 1;

    while (high >= low) {
        long mid = (low + high) / 2;
        if (key == array[mid][0]) return mid;
        if (key < array[mid][0]) high = mid - 1;
        else low = mid + 1;
    }

    return -1;
}

long MyanmarCalendar::binarySearch1D(long key, const std::vector<long>& array) const {
    long low = 0;
    long high = array.size() - 1;

    while (high >= low) {
        long mid = (low + high) / 2;
        if (key == array[mid]) return mid;
        if (key < array[mid]) high = mid - 1;
        else low = mid + 1;
    }

    return -1;
}

MyanmarCalendarData MyanmarCalendar::calculateMyanmarDate(const BirthData& birthData) const {
    if (!initialized) {
        throw std::runtime_error("Myanmar Calendar not initialized");
    }

    // Calculate Julian Day
    double julianDay = birthData.getJulianDay();
    return calculateMyanmarDate(julianDay);
}

MyanmarCalendarData MyanmarCalendar::calculateMyanmarDate(double julianDay) const {
    if (!initialized) {
        throw std::runtime_error("Myanmar Calendar not initialized");
    }

    MyanmarCalendarData data;
    data.julianDay = julianDay;

    // Convert Julian Day to Myanmar date
    long yearType, myanmarYear, month, dayOfMonth;
    julianToMyanmarDate(julianDay, yearType, myanmarYear, month, dayOfMonth);

    data.myanmarYear = myanmarYear;
    data.sasanaYear = myanmarYear + 1182;
    data.month = static_cast<MyanmarMonth>(month);
    data.dayOfMonth = dayOfMonth;
    data.yearType = static_cast<MyanmarYearType>(yearType);

    // Calculate moon phase and related information
    long moonPhase = calculateMoonPhase(dayOfMonth, month, yearType);
    data.moonPhase = static_cast<MyanmarMoonPhase>(moonPhase);
    data.fortnightDay = calculateFortnightDay(dayOfMonth);
    data.monthLength = calculateMonthLength(month, yearType);

    // Calculate weekday
    long weekday = long(julianDay + 1.5) % 7; // Adjust for Myanmar weekday calculation
    data.weekday = static_cast<MyanmarWeekday>(weekday);

    // Calculate astrological information
    data.mahabote = static_cast<Mahabote>(calculateMahabote(myanmarYear, weekday));
    data.nakhat = static_cast<Nakhat>(calculateNakhat(myanmarYear));
    data.nagahle = static_cast<NagahleDirection>(calculateNagahle(month));

    // Calculate astrological days
    long sabbath = calculateSabbath(dayOfMonth, month, yearType);
    data.isSabbath = (sabbath == 1);
    data.isSabbathEve = (sabbath == 2);

    data.isYatyaza = (calculateYatyaza(month, weekday) == 1);

    long pyathada = calculatePyathada(month, weekday);
    data.isPyathada = (pyathada == 1);
    data.isAfternoonPyathada = (pyathada == 2);

    data.isThamanyo = (calculateThamanyo(month, weekday) == 1);
    data.isAmyeittasote = (calculateAmyeittasote(dayOfMonth, weekday) == 1);
    data.isWarameittugyi = (calculateWarameittugyi(dayOfMonth, weekday) == 1);
    data.isWarameittunge = (calculateWarameittunge(dayOfMonth, weekday) == 1);
    data.isYatpote = (calculateYatpote(dayOfMonth, weekday) == 1);
    data.isThamaphyu = (calculateThamaphyu(dayOfMonth, weekday) == 1);
    data.isNagapor = (calculateNagapor(dayOfMonth, weekday) == 1);
    data.isYatyotema = (calculateYatyotema(month, dayOfMonth) == 1);
    data.isMahayatkyan = (calculateMahayatkyan(month, dayOfMonth) == 1);
    data.isShanyat = (calculateShanyat(month, dayOfMonth) == 1);

    // Identify festivals and events
    identifyFestivals(data);
    identifyHolidays(data);
    identifyAstrologicalEvents(data);

    return data;
}

void MyanmarCalendar::identifyFestivals(MyanmarCalendarData& data) const {
    // Check for month-specific festivals
    switch (data.month) {
        case MyanmarMonth::TAGU:
            if (data.dayOfMonth >= 13 && data.dayOfMonth <= 16) {
                data.festivals.push_back("Thingyan Water Festival");
            }
            if (data.dayOfMonth == 1) {
                data.festivals.push_back("Myanmar New Year");
            }
            break;

        case MyanmarMonth::KASON:
            if (data.moonPhase == MyanmarMoonPhase::FULL_MOON) {
                data.festivals.push_back("Vesak Full Moon");
                data.festivals.push_back("Buddha's Birthday");
            }
            break;

        case MyanmarMonth::WASO:
            if (data.moonPhase == MyanmarMoonPhase::FULL_MOON) {
                data.festivals.push_back("Beginning of Buddhist Lent");
                data.festivals.push_back("Waso Full Moon");
            }
            break;

        case MyanmarMonth::THADINGYUT:
            if (data.moonPhase == MyanmarMoonPhase::FULL_MOON) {
                data.festivals.push_back("End of Buddhist Lent");
                data.festivals.push_back("Thadingyut Festival");
                data.festivals.push_back("Festival of Lights");
            }
            break;

        case MyanmarMonth::TAZAUNGMON:
            if (data.moonPhase == MyanmarMoonPhase::FULL_MOON) {
                data.festivals.push_back("Tazaungmon Festival");
                data.festivals.push_back("Kathina Robe Offering");
            }
            break;

        case MyanmarMonth::TABAUNG:
            if (data.moonPhase == MyanmarMoonPhase::FULL_MOON) {
                data.festivals.push_back("Shwedagon Pagoda Festival");
            }
            break;

        default:
            break;
    }

    // Add sabbath-related observances
    if (data.isSabbath) {
        data.festivals.push_back("Buddhist Sabbath Day");
    }
}

void MyanmarCalendar::identifyHolidays(MyanmarCalendarData& data) const {
    // Government holidays and public observances
    if (data.month == MyanmarMonth::TAGU && data.dayOfMonth >= 13 && data.dayOfMonth <= 16) {
        data.holidays.push_back("Thingyan Public Holiday");
    }

    if (data.month == MyanmarMonth::TAGU && data.dayOfMonth == 1) {
        data.holidays.push_back("Myanmar New Year Holiday");
    }

    if (data.isSabbath && (data.month == MyanmarMonth::KASON ||
                          data.month == MyanmarMonth::WASO ||
                          data.month == MyanmarMonth::THADINGYUT ||
                          data.month == MyanmarMonth::TAZAUNGMON)) {
        data.holidays.push_back("Religious Holiday");
    }
}

void MyanmarCalendar::identifyAstrologicalEvents(MyanmarCalendarData& data) const {
    // Add astrological events based on calculated days
    if (data.isYatyaza) {
        data.astrologicalEvents.push_back("Yatyaza - Avoid new ventures");
    }

    if (data.isPyathada) {
        data.astrologicalEvents.push_back("Pyathada - Very inauspicious");
    }

    if (data.isAfternoonPyathada) {
        data.astrologicalEvents.push_back("Afternoon Pyathada - Afternoon inauspicious");
    }

    if (data.isThamanyo) {
        data.astrologicalEvents.push_back("Thamanyo - Auspicious day");
    }

    if (data.isWarameittugyi) {
        data.astrologicalEvents.push_back("Warameittugyi - Great auspicious day");
    }

    if (data.isAmyeittasote) {
        data.astrologicalEvents.push_back("Amyeittasote - Moderately auspicious");
    }

    if (data.isSabbath) {
        data.astrologicalEvents.push_back("Buddhist Sabbath - Religious observance");
    }
}

std::string MyanmarCalendarData::getFormattedDate() const {
    std::stringstream ss;
    ss << dayOfMonth << " " << static_cast<int>(month) << " " << myanmarYear;
    return ss.str();
}

std::string MyanmarCalendarData::getFormattedMoonPhase() const {
    switch (moonPhase) {
        case MyanmarMoonPhase::WAXING: return "Waxing";
        case MyanmarMoonPhase::FULL_MOON: return "Full Moon";
        case MyanmarMoonPhase::WANING: return "Waning";
        case MyanmarMoonPhase::NEW_MOON: return "New Moon";
        default: return "Unknown";
    }
}

std::string MyanmarCalendarData::getAstrologicalSummary() const {
    std::stringstream ss;

    if (isSabbath) ss << "Sabbath ";
    if (isThamanyo) ss << "Thamanyo ";
    if (isWarameittugyi) ss << "Warameittugyi ";
    if (isYatyaza) ss << "Yatyaza ";
    if (isPyathada) ss << "Pyathada ";

    return ss.str();
}

std::string MyanmarCalendarData::getFullDescription() const {
    std::stringstream ss;
    ss << "Myanmar Date: " << getFormattedDate() << "\n";
    ss << "Moon Phase: " << getFormattedMoonPhase() << "\n";
    ss << "Astrological: " << getAstrologicalSummary() << "\n";

    if (!festivals.empty()) {
        ss << "Festivals: ";
        for (size_t i = 0; i < festivals.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << festivals[i];
        }
        ss << "\n";
    }

    return ss.str();
}

std::string MyanmarCalendar::generateMyanmarCalendarTable(const MyanmarCalendarData& data) const {
    std::stringstream ss;

    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "                      🇲🇲  MYANMAR CALENDAR  🇲🇲\n";
    ss << "═══════════════════════════════════════════════════════════════════\n\n";

    ss << "📅 DATE INFORMATION:\n";
    ss << "   Myanmar Year: " << data.myanmarYear << " ME\n";
    ss << "   Sasana Year: " << data.sasanaYear << " SE\n";
    ss << "   Month: " << getMyanmarMonthName(data.month) << "\n";
    ss << "   Day: " << data.dayOfMonth << "\n";
    ss << "   Year Type: " << getYearTypeName(data.yearType) << "\n\n";

    ss << "🌙 LUNAR INFORMATION:\n";
    ss << "   Moon Phase: " << getMoonPhaseName(data.moonPhase) << "\n";
    ss << "   Fortnight Day: " << data.fortnightDay << "\n";
    ss << "   Month Length: " << data.monthLength << " days\n\n";

    ss << "📊 ASTROLOGICAL INFORMATION:\n";
    ss << "   Weekday: " << getMyanmarWeekdayName(data.weekday) << "\n";
    ss << "   Mahabote: " << getMahaboteName(data.mahabote) << "\n";
    ss << "   Nakhat: " << getNakhatName(data.nakhat) << "\n";
    ss << "   Nagahle: " << getNagahleDirectionName(data.nagahle) << "\n\n";

    ss << "🌟 ASTROLOGICAL DAYS:\n";
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
    ss << "    \"type\": " << static_cast<int>(data.yearType) << ",\n";
    ss << "    \"name\": \"" << getYearTypeName(data.yearType) << "\"\n";
    ss << "  },\n";
    ss << "  \"moonPhase\": {\n";
    ss << "    \"phase\": " << static_cast<int>(data.moonPhase) << ",\n";
    ss << "    \"name\": \"" << getMoonPhaseName(data.moonPhase) << "\"\n";
    ss << "  },\n";
    ss << "  \"fortnightDay\": " << data.fortnightDay << ",\n";
    ss << "  \"monthLength\": " << data.monthLength << ",\n";
    ss << "  \"weekday\": {\n";
    ss << "    \"number\": " << static_cast<int>(data.weekday) << ",\n";
    ss << "    \"name\": \"" << getMyanmarWeekdayName(data.weekday) << "\"\n";
    ss << "  },\n";
    ss << "  \"astrology\": {\n";
    ss << "    \"mahabote\": \"" << getMahaboteName(data.mahabote) << "\",\n";
    ss << "    \"nakhat\": \"" << getNakhatName(data.nakhat) << "\",\n";
    ss << "    \"nagahle\": \"" << getNagahleDirectionName(data.nagahle) << "\"\n";
    ss << "  },\n";
    ss << "  \"astrologicalDays\": {\n";
    ss << "    \"isSabbath\": " << (data.isSabbath ? "true" : "false") << ",\n";
    ss << "    \"isSabbathEve\": " << (data.isSabbathEve ? "true" : "false") << ",\n";
    ss << "    \"isThamanyo\": " << (data.isThamanyo ? "true" : "false") << ",\n";
    ss << "    \"isWarameittugyi\": " << (data.isWarameittugyi ? "true" : "false") << ",\n";
    ss << "    \"isYatyaza\": " << (data.isYatyaza ? "true" : "false") << ",\n";
    ss << "    \"isPyathada\": " << (data.isPyathada ? "true" : "false") << "\n";
    ss << "  },\n";
    ss << "  \"festivals\": [";
    for (size_t i = 0; i < data.festivals.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << data.festivals[i] << "\"";
    }
    ss << "],\n";
    ss << "  \"julianDay\": " << std::fixed << std::setprecision(6) << data.julianDay << "\n";
    ss << "}";

    return ss.str();
}

std::vector<MyanmarCalendarData> MyanmarCalendar::calculateMyanmarDateRange(const std::string& fromDate,
                                                                          const std::string& toDate) const {
    std::vector<MyanmarCalendarData> results;

    if (!initialized) {
        throw std::runtime_error("Myanmar Calendar not initialized");
    }

    // Parse from date
    int fromYear, fromMonth, fromDay;
    if (!parseDate(fromDate, fromYear, fromMonth, fromDay)) {
        throw std::runtime_error("Invalid from date format: " + fromDate);
    }

    // Parse to date
    int toYear, toMonth, toDay;
    if (!parseDate(toDate, toYear, toMonth, toDay)) {
        throw std::runtime_error("Invalid to date format: " + toDate);
    }

    // Calculate Julian days using Swiss Ephemeris (consistent with single date calculation)
    double fromJD = swe_julday(fromYear, fromMonth, fromDay, 12.0, SE_GREG_CAL); // Noon
    double toJD = swe_julday(toYear, toMonth, toDay, 12.0, SE_GREG_CAL);         // Noon

    // Calculate Myanmar calendar data for each day
    for (double jd = fromJD; jd <= toJD; jd += 1.0) {
        try {
            MyanmarCalendarData data = calculateMyanmarDate(jd);
            results.push_back(data);
        } catch (const std::exception& e) {
            // Log error but continue with other dates
            lastError = "Error calculating Myanmar date for JD " + std::to_string(jd) + ": " + e.what();
        }
    }

    return results;
}

std::string MyanmarCalendar::generateMyanmarCalendarTable(const std::vector<MyanmarCalendarData>& dataList) const {
    if (dataList.empty()) {
        return "No Myanmar calendar data available.\n";
    }

    std::stringstream ss;

    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "              🇲🇲  MYANMAR CALENDAR TABLE  🇲🇲\n";
    ss << "═══════════════════════════════════════════════════════════════════\n\n";

    // Table header
    ss << std::left << std::setw(12) << "Date"
       << std::setw(8) << "ME Year"
       << std::setw(12) << "Month"
       << std::setw(4) << "Day"
       << std::setw(12) << "Moon Phase"
       << std::setw(10) << "Weekday"
       << std::setw(15) << "Events" << "\n";
    ss << std::string(85, '-') << "\n";

    for (const auto& data : dataList) {
        // Convert Julian day to Gregorian date for display using Swiss Ephemeris
        double jd = data.julianDay;
        int year, month, day, hour, min;
        double sec;
        swe_jdut1_to_utc(jd, SE_GREG_CAL, &year, &month, &day, &hour, &min, &sec);

        std::string dateStr = std::to_string(year) + "-" +
                             (month < 10 ? "0" : "") + std::to_string(month) + "-" +
                             (day < 10 ? "0" : "") + std::to_string(day);

        std::string events;
        if (data.isSabbath) events += "Sabbath ";
        if (data.isThamanyo) events += "Thamanyo ";
        if (data.isYatyaza) events += "Yatyaza ";
        if (!data.festivals.empty()) events += data.festivals[0];

        ss << std::left << std::setw(12) << dateStr
           << std::setw(8) << data.myanmarYear
           << std::setw(12) << getMyanmarMonthName(data.month)
           << std::setw(4) << data.dayOfMonth
           << std::setw(12) << getMoonPhaseName(data.moonPhase)
           << std::setw(10) << getMyanmarWeekdayName(data.weekday)
           << std::setw(15) << events << "\n";
    }

    ss << "\n═══════════════════════════════════════════════════════════════════\n";

    return ss.str();
}

std::string MyanmarCalendar::generateCSV(const std::vector<MyanmarCalendarData>& dataList) const {
    if (dataList.empty()) {
        return "No Myanmar calendar data available.\n";
    }

    std::stringstream ss;

    // CSV header
    ss << "Date,ME_Year,SE_Year,Month,Day,Month_Length,Year_Type,Moon_Phase,Fortnight_Day,Weekday,";
    ss << "Mahabote,Nakhat,Nagahle,Is_Sabbath,Is_Thamanyo,Is_Yatyaza,Is_Pyathada,";
    ss << "Is_Warameittugyi,Is_Amyeittasote,Festivals,Astrological_Events,Julian_Day\n";

    for (const auto& data : dataList) {
        // Convert Julian day to Gregorian date using Swiss Ephemeris
        double jd = data.julianDay;
        int year, month, day, hour, min;
        double sec;
        swe_jdut1_to_utc(jd, SE_GREG_CAL, &year, &month, &day, &hour, &min, &sec);

        std::string dateStr = std::to_string(year) + "-" +
                             (month < 10 ? "0" : "") + std::to_string(month) + "-" +
                             (day < 10 ? "0" : "") + std::to_string(day);

        // Festivals as comma-separated values within quotes
        std::string festivals = "\"";
        for (size_t i = 0; i < data.festivals.size(); ++i) {
            if (i > 0) festivals += "; ";
            festivals += data.festivals[i];
        }
        festivals += "\"";

        // Astrological events
        std::string astroEvents = "\"";
        for (size_t i = 0; i < data.astrologicalEvents.size(); ++i) {
            if (i > 0) astroEvents += "; ";
            astroEvents += data.astrologicalEvents[i];
        }
        astroEvents += "\"";

        ss << dateStr << ","
           << data.myanmarYear << ","
           << data.sasanaYear << ","
           << getMyanmarMonthName(data.month) << ","
           << data.dayOfMonth << ","
           << data.monthLength << ","
           << getYearTypeName(data.yearType) << ","
           << getMoonPhaseName(data.moonPhase) << ","
           << data.fortnightDay << ","
           << getMyanmarWeekdayName(data.weekday) << ","
           << getMahaboteName(data.mahabote) << ","
           << getNakhatName(data.nakhat) << ","
           << getNagahleDirectionName(data.nagahle) << ","
           << (data.isSabbath ? "Yes" : "No") << ","
           << (data.isThamanyo ? "Yes" : "No") << ","
           << (data.isYatyaza ? "Yes" : "No") << ","
           << (data.isPyathada ? "Yes" : "No") << ","
           << (data.isWarameittugyi ? "Yes" : "No") << ","
           << (data.isAmyeittasote ? "Yes" : "No") << ","
           << festivals << ","
           << astroEvents << ","
           << std::fixed << std::setprecision(6) << data.julianDay << "\n";
    }

    return ss.str();
}

bool MyanmarCalendar::parseDate(const std::string& dateStr, int& year, int& month, int& day) const {
    // Parse YYYY-MM-DD format
    if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return false;
    }

    try {
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

// Utility method implementations
std::string MyanmarCalendar::getMyanmarMonthName(MyanmarMonth month) const {
    switch (month) {
        case MyanmarMonth::FIRST_WASO: return "1st Waso";
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

// Julian Day Number conversion utilities (from reference implementation)
double MyanmarCalendar::gregorianToJdn(int year, int month, int day) const {
    if (month < 3) {
        year = year - 1;
        month = month + 12;
    }
    double julianDay = (365.25 * year) + (30.59 * (month - 2)) + day + 1721086.5;
    if (year < 0) {
        julianDay = julianDay - 1;
        if ((year % 4) == 0 && 3 <= month) {
            julianDay = julianDay + 1;
        }
    }
    if (2299160 < julianDay) {
        julianDay = julianDay + (year / 400) - (year / 100) + 2;
    }
    return julianDay;
}

std::vector<int> MyanmarCalendar::jdnToGregorian(double jd) const {
    int J = static_cast<int>(jd) + 32044;
    int g = J / 146097;
    int dg = J % 146097;
    int c = ((dg / 36524) + 1) * 3 / 4;
    int dc = dg - c * 36524;
    int b = dc / 1461;
    int db = dc % 1461;
    int a = ((db / 365) + 1) * 3 / 4;
    int da = db - a * 365;
    int y = g * 400 + c * 100 + b * 4 + a;
    int m = (da * 5 + 308) / 153 - 2;
    int d = da - ((m + 4) * 153 / 5) + 122;
    int year = y - 4800 + ((m + 2) / 12);
    int month = (m + 2) % 12 + 1;
    int dayResult = d + 1;
    return {year, month, dayResult};
}

} // namespace Astro
