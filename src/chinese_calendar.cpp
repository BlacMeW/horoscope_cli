#include "chinese_calendar.h"
#include "../third_party/swisseph/swephexp.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace Astro {

// Static arrays for string conversion
static const std::array<const char*, 11> HEAVENLY_STEMS_ZH = {
    "", "甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"
};

static const std::array<const char*, 11> HEAVENLY_STEMS_EN = {
    "", "Jia", "Yi", "Bing", "Ding", "Wu", "Ji", "Geng", "Xin", "Ren", "Gui"
};

static const std::array<const char*, 11> HEAVENLY_STEMS_PINYIN = {
    "", "jiǎ", "yǐ", "bǐng", "dīng", "wù", "jǐ", "gēng", "xīn", "rén", "guǐ"
};

static const std::array<const char*, 13> EARTHLY_BRANCHES_ZH = {
    "", "子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"
};

static const std::array<const char*, 13> EARTHLY_BRANCHES_EN = {
    "", "Zi", "Chou", "Yin", "Mao", "Chen", "Si", "Wu", "Wei", "Shen", "You", "Xu", "Hai"
};

static const std::array<const char*, 13> EARTHLY_BRANCHES_PINYIN = {
    "", "zǐ", "chǒu", "yín", "mǎo", "chén", "sì", "wǔ", "wèi", "shēn", "yǒu", "xū", "hài"
};

static const std::array<const char*, 13> ZODIAC_ANIMALS_ZH = {
    "", "鼠", "牛", "虎", "兔", "龍", "蛇", "馬", "羊", "猴", "雞", "狗", "豬"
};

static const std::array<const char*, 13> ZODIAC_ANIMALS_EN = {
    "", "Rat", "Ox", "Tiger", "Rabbit", "Dragon", "Snake", "Horse", "Goat", "Monkey", "Rooster", "Dog", "Pig"
};

static const std::array<const char*, 13> ZODIAC_ANIMALS_PINYIN = {
    "", "shǔ", "niú", "hǔ", "tù", "lóng", "shé", "mǎ", "yáng", "hóu", "jī", "gǒu", "zhū"
};

static const std::array<const char*, 6> ELEMENTS_ZH = {
    "", "木", "火", "土", "金", "水"
};

static const std::array<const char*, 6> ELEMENTS_EN = {
    "", "Wood", "Fire", "Earth", "Metal", "Water"
};

static const std::array<const char*, 6> ELEMENTS_PINYIN = {
    "", "mù", "huǒ", "tǔ", "jīn", "shuǐ"
};

static const std::array<const char*, 25> SOLAR_TERMS_ZH = {
    "",
    "立春", "雨水", "驚蟄", "春分", "清明", "穀雨",      // Spring
    "立夏", "小滿", "芒種", "夏至", "小暑", "大暑",      // Summer
    "立秋", "處暑", "白露", "秋分", "寒露", "霜降",      // Autumn
    "立冬", "小雪", "大雪", "冬至", "小寒", "大寒"       // Winter
};

static const std::array<const char*, 25> SOLAR_TERMS_EN = {
    "",
    "Beginning of Spring", "Rain Water", "Awakening of Insects", "Vernal Equinox", "Clear and Bright", "Grain Rain",
    "Beginning of Summer", "Grain Buds", "Grain in Ear", "Summer Solstice", "Slight Heat", "Great Heat",
    "Beginning of Autumn", "Stopping the Heat", "White Dew", "Autumnal Equinox", "Cold Dew", "Frost's Descent",
    "Beginning of Winter", "Slight Snow", "Great Snow", "Winter Solstice", "Slight Cold", "Great Cold"
};

// Constructor
ChineseCalendar::ChineseCalendar() : initialized(false) {
    // Initialize Swiss Ephemeris path if needed
    swe_set_ephe_path(const_cast<char*>("./data"));
}

// Destructor
ChineseCalendar::~ChineseCalendar() {
    if (initialized) {
        swe_close();
    }
}

// Initialize the calendar system
bool ChineseCalendar::initialize() {
    if (initialized) {
        return true;
    }
    
    try {
        // Set Swiss Ephemeris to sidereal mode (needed for precise calculations)
        swe_set_sid_mode(SE_SIDM_LAHIRI, 0, 0);
        
        initialized = true;
        lastError.clear();
        return true;
    } catch (const std::exception& e) {
        lastError = "Failed to initialize Chinese calendar: " + std::string(e.what());
        return false;
    }
}

// Core calculation: Chinese year from Julian Day
int ChineseCalendar::calculateChineseYear(double julianDay) const {
    // Simple approach: convert JD to Gregorian year and add Chinese era offset
    // More precise calculation would check Chinese New Year dates
    
    int gregYear, gregMonth, gregDay;
    julianToGregorian(julianDay, gregYear, gregMonth, gregDay);
    
    // Chinese New Year typically falls in Jan-Feb
    // If before Feb 15 (rough estimate), might be previous Chinese year
    if (gregMonth == 1 || (gregMonth == 2 && gregDay < 15)) {
        // Check if this is before Chinese New Year
        // For simplicity, assume Chinese New Year is around Feb 5-15
        if (gregMonth == 1 || gregDay < 10) {
            gregYear--;  // Previous Chinese year
        }
    }
    
    // Convert to Chinese era (Huangdi calendar starts from 2697 BC)
    return gregYear + 2697;
}

// Calculate Sun's longitude for solar term determination
double ChineseCalendar::calculateSunLongitude(double julianDay) const {
    double sunPos[6];
    char errorString[256];
    
    int result = swe_calc(julianDay, SE_SUN, SEFLG_SIDEREAL | SEFLG_SPEED, sunPos, errorString);
    if (result < 0) {
        return 0.0;  // Error fallback
    }
    
    return sunPos[0];  // Longitude in degrees
}

// Calculate when a specific solar term occurs
double ChineseCalendar::calculateSolarTermJD(int year, SolarTerm term) const {
    // Check cache first
    if (solarTermCache.find(year) != solarTermCache.end()) {
        return solarTermCache[year][static_cast<int>(term) - 1];
    }
    
    // Calculate all solar terms for the year
    std::vector<double> solarTerms(24);
    
    // Start from approximate date
    double startJD = GREGORIAN_EPOCH + (year - 1) * 365.25;
    
    for (int i = 0; i < 24; i++) {
        double targetLongitude = SOLAR_TERM_DEGREES[i];
        
        // Use iterative method to find when sun reaches target longitude
        double jd = startJD + i * 15.2;  // Approximate 15.2 days between solar terms
        
        for (int iter = 0; iter < 10; iter++) {  // Newton-Raphson iteration
            double sunLong = calculateSunLongitude(jd);
            double difference = targetLongitude - sunLong;
            
            // Handle longitude wraparound
            if (difference > 180) difference -= 360;
            if (difference < -180) difference += 360;
            
            if (std::abs(difference) < 0.0001) break;  // Converged
            
            // Sun moves ~1 degree per day
            jd += difference;
        }
        
        solarTerms[i] = jd;
    }
    
    // Cache the results
    solarTermCache[year] = solarTerms;
    
    return solarTerms[static_cast<int>(term) - 1];
}

// Determine if a Chinese year is a leap year (has 13 months)
bool ChineseCalendar::isChineseLeapYear(int chineseYear) {
    // Use the traditional Metonic cycle: 7 leap years in 19 years
    // This approximates the 235 lunar months = 19 solar years relationship
    
    // Calculate position in 19-year Metonic cycle
    int metonicYear = ((chineseYear - 1) % 19) + 1;
    
    // Traditional Chinese leap year pattern in 19-year cycle
    // Years 3, 6, 8, 11, 14, 17, 19 are leap years
    std::array<int, 7> leapYears = {3, 6, 8, 11, 14, 17, 19};
    
    bool isLeap = std::find(leapYears.begin(), leapYears.end(), metonicYear) != leapYears.end();
    
    // For more accuracy, we could also check astronomical conditions
    // but the Metonic cycle is the traditional method used historically
    
    return isLeap;
}

// Determine which month is the leap month in a leap year
int ChineseCalendar::determineLeapMonth(int chineseYear) {
    if (!isChineseLeapYear(chineseYear)) {
        return 0;  // No leap month
    }
    
    // Check cache first
    if (leapMonthCache.find(chineseYear) != leapMonthCache.end()) {
        return leapMonthCache[chineseYear];
    }
    
    int gregorianYear = chineseYear - 2697;  // Convert to Gregorian year
    
    // Find all 13 new moons in the Chinese year
    std::vector<double> newMoons;
    double startJD = GREGORIAN_EPOCH + (gregorianYear - 1) * 365.25;
    
    // Chinese year starts around winter solstice of previous year
    double winterSolstice = calculateSolarTermJD(gregorianYear - 1, SolarTerm::DONGZHI);
    double currentJD = winterSolstice;
    
    // Find 13 consecutive new moons
    for (int i = 0; i < 13; i++) {
        double newMoon = calculateNewMoonJD(i + 1, gregorianYear);
        if (newMoon >= currentJD) {
            newMoons.push_back(newMoon);
            currentJD = newMoon + 1;  // Move to next day to find next new moon
        }
    }
    
    // Find which lunar month contains no major solar term (中氣)
    // Major solar terms occur every 30 degrees (12 per year)
    std::vector<SolarTerm> majorTerms = {
        SolarTerm::YUSHUI,    // 330°  Rain Water
        SolarTerm::CHUNFEN,   // 0°    Spring Equinox  
        SolarTerm::GUYU,      // 30°   Grain Rain
        SolarTerm::XIAOMAN,   // 60°   Grain Buds
        SolarTerm::XIAZHI,    // 90°   Summer Solstice
        SolarTerm::DASHU,     // 120°  Great Heat
        SolarTerm::CHUSHU,    // 150°  Stopping Heat
        SolarTerm::QIUFEN,    // 180°  Autumn Equinox
        SolarTerm::SHUANGJIANG, // 210° Frost's Descent
        SolarTerm::XIAOXUE,   // 240°  Slight Snow
        SolarTerm::DONGZHI,   // 270°  Winter Solstice
        SolarTerm::DAHAN      // 300°  Great Cold
    };
    
    // Check each lunar month for major solar terms
    for (int month = 1; month < newMoons.size(); month++) {
        double monthStart = newMoons[month - 1];
        double monthEnd = newMoons[month];
        
        bool hasMajorTerm = false;
        
        // Check if any major solar term falls within this lunar month
        for (SolarTerm term : majorTerms) {
            double termJD = calculateSolarTermJD(gregorianYear, term);
            if (termJD >= monthStart && termJD < monthEnd) {
                hasMajorTerm = true;
                break;
            }
        }
        
        // The first month without a major solar term is the leap month
        if (!hasMajorTerm) {
            leapMonthCache[chineseYear] = month;
            return month;
        }
    }
    
    // Fallback if no clear leap month found
    int leapMonth = (chineseYear * 7) % 12 + 1;
    leapMonthCache[chineseYear] = leapMonth;
    return leapMonth;
}

// Calculate new moon JD for a given lunar month
double ChineseCalendar::calculateNewMoonJD(int lunarMonth, int year) const {
    // Use Swiss Ephemeris to find precise new moon timing
    double startJD = GREGORIAN_EPOCH + (year - 1) * 365.25;
    double searchJD = startJD + (lunarMonth - 1) * SYNODIC_MONTH;
    
    // Find next new moon after searchJD using Swiss Ephemeris
    char errorString[256];
    
    // Use swe_solcross_ut to find lunar phase (corrected API call)
    double newMoonJD = swe_solcross_ut(0.0, searchJD, 0, errorString);
    if (newMoonJD > 0) {
        return newMoonJD;
    }
    
    // Alternative method: manual calculation of new moon
    return calculateNewMoonIterative(searchJD);
}

// Iterative method to find precise new moon timing
double ChineseCalendar::calculateNewMoonIterative(double approximateJD) const {
    double jd = approximateJD;
    
    for (int iteration = 0; iteration < 10; iteration++) {
        double moonPos[6], sunPos[6];
        char errorString[256];
        
        // Calculate Moon and Sun positions
        int moonResult = swe_calc(jd, SE_MOON, SEFLG_SIDEREAL, moonPos, errorString);
        int sunResult = swe_calc(jd, SE_SUN, SEFLG_SIDEREAL, sunPos, errorString);
        
        if (moonResult < 0 || sunResult < 0) {
            break; // Error in calculation
        }
        
        // Calculate lunar elongation (angular separation)
        double elongation = moonPos[0] - sunPos[0];
        
        // Normalize to 0-360 range
        while (elongation < 0) elongation += 360;
        while (elongation > 360) elongation -= 360;
        
        // New moon occurs when elongation is closest to 0 or 360
        double target = (elongation < 180) ? 0 : 360;
        double difference = target - elongation;
        
        if (std::abs(difference) < 0.01) {
            break; // Converged (within 0.01 degrees)
        }
        
        // Moon moves ~13.2 degrees per day relative to sun
        jd += difference / 13.2;
    }
    
    return jd;
}// 60-year cycle calculations
HeavenlyStem ChineseCalendar::calculateStem(int cyclePosition) {
    int stem = ((cyclePosition - 1) % 10) + 1;
    return static_cast<HeavenlyStem>(stem);
}

EarthlyBranch ChineseCalendar::calculateBranch(int cyclePosition) {
    int branch = ((cyclePosition - 1) % 12) + 1;
    return static_cast<EarthlyBranch>(branch);
}

int ChineseCalendar::getSexagenaryYear(int chineseYear) {
    // The sexagenary cycle started in year 2697 BC (Chinese year 1)
    return ((chineseYear - 1) % 60) + 1;
}

int ChineseCalendar::getSexagenaryMonth(int chineseYear, int chineseMonth) {
    // Month stems follow a pattern based on year stem
    int yearStem = ((chineseYear - 1) % 10) + 1;
    int monthStem = ((yearStem - 1) * 2 + chineseMonth) % 10;
    if (monthStem == 0) monthStem = 10;
    
    return ((monthStem - 1) * 12 + (chineseMonth - 1)) % 60 + 1;
}

int ChineseCalendar::getSexagenaryDay(double julianDay) {
    // Day cycle: the sexagenary cycle for days is continuous
    // Reference: JD 1842713 (Feb 9, 1899) = 甲子 (Jia-Zi, cycle day 1)
    const double REFERENCE_JD = 1842713.0;
    const int REFERENCE_CYCLE_DAY = 1;
    
    int daysSinceReference = static_cast<int>(julianDay - REFERENCE_JD);
    int cycleDay = ((daysSinceReference + REFERENCE_CYCLE_DAY - 1) % 60) + 1;
    if (cycleDay <= 0) cycleDay += 60;
    
    return cycleDay;
}

int ChineseCalendar::getSexagenaryHour(double julianDay, double hour) {
    // Each day is divided into 12 double-hours (時辰)
    // Hour stems follow a pattern based on day stem
    int dayCyclePos = getSexagenaryDay(julianDay);
    int dayStem = ((dayCyclePos - 1) % 10) + 1;
    
    int hourIndex = static_cast<int>(hour / 2) % 12;  // 0-11 for 12 double-hours
    int hourStem = ((dayStem - 1) * 2 + hourIndex) % 10 + 1;
    
    return ((hourStem - 1) * 12 + hourIndex) % 60 + 1;
}

// Zodiac and element calculations
ChineseZodiac ChineseCalendar::getZodiacAnimal(int chineseYear) {
    // The zodiac cycle is based on the Gregorian year modulo 12
    // 2024 = Dragon, so we need to calculate from a known Dragon year
    // Dragon years: ..., 2012, 2024, 2036, ...
    // 2024 = 2012 + 12, so 2024 mod 12 should give us Dragon position
    
    int gregorianYear = chineseYear - 2697;  // Convert back to approximate Gregorian
    
    // Known reference: 2024 is Year of Dragon (position 5)
    // So (2024 - 2024) % 12 = 0, but we want position 5
    // This means: ((year - 2024) % 12 + 5 - 1) % 12 + 1
    int animalPos = ((gregorianYear - 2024) % 12 + 12) % 12;  // Normalize to 0-11
    int dragonOffset = 5;  // Dragon is 5th animal
    int animal = (animalPos + dragonOffset - 1) % 12 + 1;  // Convert to 1-12
    
    return static_cast<ChineseZodiac>(animal);
}

ChineseElement ChineseCalendar::getYearElement(int chineseYear) {
    // Each animal has an element cycle: Wood, Fire, Earth, Metal, Water
    // Each element lasts 2 years (Yin and Yang)
    // 60-year cycle = 12 animals × 5 elements
    int elementCycle = ((chineseYear - 1) % 10) / 2 + 1;
    return static_cast<ChineseElement>(elementCycle);
}

YinYang ChineseCalendar::getYinYang(int value) {
    return (value % 2 == 0) ? YinYang::YIN : YinYang::YANG;
}

ChineseElement ChineseCalendar::getStemElement(HeavenlyStem stem) {
    int stemValue = static_cast<int>(stem);
    return static_cast<ChineseElement>((stemValue - 1) / 2 + 1);
}

ChineseElement ChineseCalendar::getBranchElement(EarthlyBranch branch) {
    // Each branch has an associated element
    static const std::array<ChineseElement, 13> branchElements = {
        ChineseElement::WATER,  // Placeholder for index 0
        ChineseElement::WATER,  // Zi (Rat)
        ChineseElement::EARTH,  // Chou (Ox)
        ChineseElement::WOOD,   // Yin (Tiger)
        ChineseElement::WOOD,   // Mao (Rabbit)
        ChineseElement::EARTH,  // Chen (Dragon)
        ChineseElement::FIRE,   // Si (Snake)
        ChineseElement::FIRE,   // Wu (Horse)
        ChineseElement::EARTH,  // Wei (Goat)
        ChineseElement::METAL,  // Shen (Monkey)
        ChineseElement::METAL,  // You (Rooster)
        ChineseElement::EARTH,  // Xu (Dog)
        ChineseElement::WATER   // Hai (Pig)
    };
    
    return branchElements[static_cast<int>(branch)];
}

// Main calculation method
ChineseCalendarData ChineseCalendar::calculateChineseCalendar(double julianDay) const {
    if (!initialized) {
        throw std::runtime_error("Chinese calendar not initialized");
    }
    
    ChineseCalendarData data;
    data.julianDay = julianDay;
    
    // Calculate Chinese date
    int chineseYear, chineseMonth, chineseDay;
    bool isLeapMonth;
    julianToChinese(julianDay, chineseYear, chineseMonth, chineseDay, isLeapMonth);
    
    data.chineseYear = chineseYear;
    data.chineseMonth = chineseMonth;
    data.chineseDay = chineseDay;
    data.isLeapMonth = isLeapMonth;
    
    // Calculate 60-year cycle positions
    int yearCycle = getSexagenaryYear(chineseYear);
    int monthCycle = getSexagenaryMonth(chineseYear, chineseMonth);
    int dayCycle = getSexagenaryDay(julianDay);
    
    data.yearStem = calculateStem(yearCycle);
    data.yearBranch = calculateBranch(yearCycle);
    data.monthStem = calculateStem(monthCycle);
    data.monthBranch = calculateBranch(monthCycle);
    data.dayStem = calculateStem(dayCycle);
    data.dayBranch = calculateBranch(dayCycle);
    
    // Calculate zodiac and elements
    data.yearAnimal = getZodiacAnimal(chineseYear);
    data.yearElement = getYearElement(chineseYear);
    data.yearYinYang = getYinYang(chineseYear);
    
    // Calculate solar terms
    data.currentSolarTerm = getCurrentSolarTerm(julianDay);
    data.nextSolarTerm = getNextSolarTerm(julianDay);
    data.nextSolarTermJD = getNextSolarTermJD(julianDay);
    
    // Calculate cultural information
    data.festivals = calculateFestivals(chineseYear, chineseMonth, chineseDay, isLeapMonth);
    data.traditions = calculateTraditions(data);
    data.taboos = calculateTaboos(data);
    data.isAuspicious = calculateAuspiciousness(data);
    
    // Generate formatted strings
    data.ganZhi = getGanZhiName(data.yearStem, data.yearBranch, true);
    data.cycleName = data.ganZhi + "年";
    
    return data;
}

// Note: BirthData overload removed to avoid dependency issues

// String formatting methods - Traditional Chinese
std::string ChineseCalendar::getHeavenlyStemName(HeavenlyStem stem, bool traditional) const {
    int index = static_cast<int>(stem);
    return traditional ? HEAVENLY_STEMS_ZH[index] : HEAVENLY_STEMS_EN[index];
}

std::string ChineseCalendar::getEarthlyBranchName(EarthlyBranch branch, bool traditional) const {
    int index = static_cast<int>(branch);
    return traditional ? EARTHLY_BRANCHES_ZH[index] : EARTHLY_BRANCHES_EN[index];
}

std::string ChineseCalendar::getZodiacAnimalName(ChineseZodiac animal, bool traditional) const {
    int index = static_cast<int>(animal);
    return traditional ? ZODIAC_ANIMALS_ZH[index] : ZODIAC_ANIMALS_EN[index];
}

std::string ChineseCalendar::getElementName(ChineseElement element, bool traditional) const {
    int index = static_cast<int>(element);
    return traditional ? ELEMENTS_ZH[index] : ELEMENTS_EN[index];
}

std::string ChineseCalendar::getSolarTermName(SolarTerm term, bool traditional) const {
    int index = static_cast<int>(term);
    return traditional ? SOLAR_TERMS_ZH[index] : SOLAR_TERMS_EN[index];
}

std::string ChineseCalendar::getGanZhiName(HeavenlyStem stem, EarthlyBranch branch, bool traditional) const {
    return getHeavenlyStemName(stem, traditional) + getEarthlyBranchName(branch, traditional);
}

// Current solar term determination
SolarTerm ChineseCalendar::getCurrentSolarTerm(double julianDay) const {
    double sunLong = calculateSunLongitude(julianDay);
    
    // Find which solar term range the sun longitude falls into
    for (int i = 0; i < 24; i++) {
        double termLong = SOLAR_TERM_DEGREES[i];
        double nextTermLong = SOLAR_TERM_DEGREES[(i + 1) % 24];
        
        // Handle year boundary
        if (nextTermLong < termLong) {
            if (sunLong >= termLong || sunLong < nextTermLong) {
                return static_cast<SolarTerm>(i + 1);
            }
        } else {
            if (sunLong >= termLong && sunLong < nextTermLong) {
                return static_cast<SolarTerm>(i + 1);
            }
        }
    }
    
    return SolarTerm::LICHUN;  // Default fallback
}

SolarTerm ChineseCalendar::getNextSolarTerm(double julianDay) const {
    SolarTerm current = getCurrentSolarTerm(julianDay);
    int nextIndex = static_cast<int>(current) % 24 + 1;
    return static_cast<SolarTerm>(nextIndex);
}

double ChineseCalendar::getNextSolarTermJD(double julianDay) const {
    // Convert JD to approximate year
    int year = static_cast<int>((julianDay - GREGORIAN_EPOCH) / 365.25) + 1;
    
    SolarTerm nextTerm = getNextSolarTerm(julianDay);
    double nextTermJD = calculateSolarTermJD(year, nextTerm);
    
    // If next term JD is before current JD, it's in the next year
    if (nextTermJD < julianDay) {
        nextTermJD = calculateSolarTermJD(year + 1, nextTerm);
    }
    
    return nextTermJD;
}

// Simplified Julian to Chinese conversion (to be refined)
void ChineseCalendar::julianToChinese(double jd, int& year, int& month, int& day, bool& isLeapMonth) const {
    // This is a simplified implementation - needs refinement
    year = calculateChineseYear(jd);
    
    // Find the start of the Chinese year (first new moon after winter solstice)
    double yearStartJD = calculateNewMoonJD(1, year - 2697);  // Convert to Gregorian year
    
    // Calculate which lunar month we're in
    double daysSinceYearStart = jd - yearStartJD;
    month = static_cast<int>(daysSinceYearStart / SYNODIC_MONTH) + 1;
    
    if (month > 12) {
        month = 12;
        isLeapMonth = true;  // Simplified - needs proper leap month detection
    } else {
        isLeapMonth = false;
    }
    
    // Calculate day within month
    double monthStartJD = calculateNewMoonJD(month, year - 2697);
    day = static_cast<int>(jd - monthStartJD) + 1;
    
    if (day < 1) day = 1;
    if (day > 30) day = 30;  // Chinese months are 29 or 30 days
}

// Placeholder implementations for cultural features
std::vector<std::string> ChineseCalendar::calculateFestivals(int year, int month, int day, bool isLeapMonth) {
    std::vector<std::string> festivals;
    
    if (!isLeapMonth) {
        if (month == 1 && day == 1) festivals.push_back("春節 Spring Festival");
        if (month == 1 && day == 15) festivals.push_back("元宵節 Lantern Festival");
        if (month == 5 && day == 5) festivals.push_back("端午節 Dragon Boat Festival");
        if (month == 7 && day == 7) festivals.push_back("七夕 Qixi Festival");
        if (month == 8 && day == 15) festivals.push_back("中秋節 Mid-Autumn Festival");
        if (month == 9 && day == 9) festivals.push_back("重陽節 Double Ninth Festival");
    }
    
    return festivals;
}

std::vector<std::string> ChineseCalendar::calculateTraditions(const ChineseCalendarData& data) {
    std::vector<std::string> traditions;
    // Add seasonal traditions based on solar terms
    // This is a placeholder - to be expanded
    return traditions;
}

std::vector<std::string> ChineseCalendar::calculateTaboos(const ChineseCalendarData& data) {
    std::vector<std::string> taboos;
    // Add traditional taboos based on stems, branches, etc.
    // This is a placeholder - to be expanded
    return taboos;
}

bool ChineseCalendar::calculateAuspiciousness(const ChineseCalendarData& data) {
    // Simplified calculation based on stem-branch combinations
    // In traditional Chinese almanac, this is much more complex
    int stemValue = static_cast<int>(data.dayStem);
    int branchValue = static_cast<int>(data.dayBranch);
    
    // Even numbers are generally considered more auspicious
    return (stemValue + branchValue) % 2 == 0;
}

// Utility methods for date conversion
double ChineseCalendar::gregorianToJulian(int year, int month, int day, double hour) const {
    // Use Swiss Ephemeris Julian day conversion
    return swe_julday(year, month, day, hour, SE_GREG_CAL);
}

void ChineseCalendar::julianToGregorian(double jd, int& year, int& month, int& day) const {
    // Use Swiss Ephemeris conversion
    int y, m, d, h, min;
    double sec;
    swe_jdut1_to_utc(jd, SE_GREG_CAL, &y, &m, &d, &h, &min, &sec);
    year = y;
    month = m;
    day = d;
}

// Generate formatted output
std::string ChineseCalendar::generateTable(const ChineseCalendarData& data) const {
    std::ostringstream oss;
    oss << "Chinese Calendar Data\n";
    oss << "====================\n";
    oss << "Chinese Date: " << data.chineseYear << "/" << data.chineseMonth;
    if (data.isLeapMonth) oss << "(leap)";
    oss << "/" << data.chineseDay << "\n";
    oss << "Gan-Zhi: " << data.ganZhi << "\n";
    oss << "Zodiac Animal: " << getZodiacAnimalName(data.yearAnimal, false) << "\n";
    oss << "Year Element: " << getElementName(data.yearElement, false) << "\n";
    oss << "Solar Term: " << getSolarTermName(data.currentSolarTerm, false) << "\n";
    oss << "Auspicious: " << (data.isAuspicious ? "Yes" : "No") << "\n";
    
    if (!data.festivals.empty()) {
        oss << "Festivals: ";
        for (size_t i = 0; i < data.festivals.size(); i++) {
            if (i > 0) oss << ", ";
            oss << data.festivals[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

} // namespace Astro