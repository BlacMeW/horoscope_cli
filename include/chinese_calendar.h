#pragma once

#include "astro_types.h"
#include <string>
#include <vector>
#include <array>
#include <map>
#include <utility>

namespace Astro {

// 10 Heavenly Stems (天干)
enum class HeavenlyStem {
    JIA = 1,    // 甲 Wood Yang
    YI = 2,     // 乙 Wood Yin  
    BING = 3,   // 丙 Fire Yang
    DING = 4,   // 丁 Fire Yin
    WU = 5,     // 戊 Earth Yang
    JI = 6,     // 己 Earth Yin
    GENG = 7,   // 庚 Metal Yang
    XIN = 8,    // 辛 Metal Yin
    REN = 9,    // 壬 Water Yang
    GUI = 10    // 癸 Water Yin
};

// 12 Earthly Branches (地支)
enum class EarthlyBranch {
    ZI = 1,     // 子 Rat    (23:00-01:00)
    CHOU = 2,   // 丑 Ox     (01:00-03:00)
    YIN = 3,    // 寅 Tiger  (03:00-05:00)
    MAO = 4,    // 卯 Rabbit (05:00-07:00)
    CHEN = 5,   // 辰 Dragon (07:00-09:00)
    SI = 6,     // 巳 Snake  (09:00-11:00)
    WU = 7,     // 午 Horse  (11:00-13:00)
    WEI = 8,    // 未 Goat   (13:00-15:00)
    SHEN = 9,   // 申 Monkey (15:00-17:00)
    YOU = 10,   // 酉 Rooster(17:00-19:00)
    XU = 11,    // 戌 Dog    (19:00-21:00)
    HAI = 12    // 亥 Pig    (21:00-23:00)
};

// 12 Zodiac Animals (十二生肖)
enum class ChineseZodiac {
    RAT = 1, OX, TIGER, RABBIT, DRAGON, SNAKE,
    HORSE, GOAT, MONKEY, ROOSTER, DOG, PIG
};

// 5 Elements (五行)
enum class ChineseElement {
    WOOD = 1,   // 木
    FIRE = 2,   // 火
    EARTH = 3,  // 土
    METAL = 4,  // 金
    WATER = 5   // 水
};

// Yin-Yang (陰陽)
enum class YinYang {
    YIN = 0,    // 陰 (Even numbers)
    YANG = 1    // 陽 (Odd numbers)
};

// 24 Solar Terms (二十四節氣)
enum class SolarTerm {
    // Spring (春)
    LICHUN = 1,      // 立春 Beginning of Spring      (315°)
    YUSHUI = 2,      // 雨水 Rain Water              (330°)
    JINGZHE = 3,     // 驚蟄 Awakening of Insects     (345°)
    CHUNFEN = 4,     // 春分 Vernal Equinox          (0°)
    QINGMING = 5,    // 清明 Clear and Bright         (15°)
    GUYU = 6,        // 穀雨 Grain Rain              (30°)
    
    // Summer (夏)
    LIXIA = 7,       // 立夏 Beginning of Summer      (45°)
    XIAOMAN = 8,     // 小滿 Grain Buds              (60°)
    MANGZHONG = 9,   // 芒種 Grain in Ear            (75°)
    XIAZHI = 10,     // 夏至 Summer Solstice          (90°)
    XIAOSHU = 11,    // 小暑 Slight Heat             (105°)
    DASHU = 12,      // 大暑 Great Heat              (120°)
    
    // Autumn (秋)
    LIQIU = 13,      // 立秋 Beginning of Autumn      (135°)
    CHUSHU = 14,     // 處暑 Stopping the Heat       (150°)
    BAILU = 15,      // 白露 White Dew               (165°)
    QIUFEN = 16,     // 秋分 Autumnal Equinox        (180°)
    HANLU = 17,      // 寒露 Cold Dew                (195°)
    SHUANGJIANG = 18,// 霜降 Frost's Descent          (210°)
    
    // Winter (冬)
    LIDONG = 19,     // 立冬 Beginning of Winter      (225°)
    XIAOXUE = 20,    // 小雪 Slight Snow             (240°)
    DAXUE = 21,      // 大雪 Great Snow              (255°)
    DONGZHI = 22,    // 冬至 Winter Solstice          (270°)
    XIAOHAN = 23,    // 小寒 Slight Cold             (285°)
    DAHAN = 24       // 大寒 Great Cold              (300°)
};

// Chinese Calendar Data Structure
struct ChineseCalendarData {
    // Basic date information
    int chineseYear;           // Chinese year number (from Huangdi era)
    int chineseMonth;          // Chinese month (1-12, with possible leap)
    int chineseDay;            // Day of Chinese month (1-29/30)
    bool isLeapMonth;          // Is this a leap month?
    int monthLength;           // Days in this month (29 or 30)
    
    // 60-year cycle information (Sexagenary cycle)
    HeavenlyStem yearStem;     // Year's heavenly stem
    EarthlyBranch yearBranch;  // Year's earthly branch
    HeavenlyStem monthStem;    // Month's heavenly stem
    EarthlyBranch monthBranch; // Month's earthly branch
    HeavenlyStem dayStem;      // Day's heavenly stem
    EarthlyBranch dayBranch;   // Day's earthly branch
    
    // Zodiac and elements
    ChineseZodiac yearAnimal;  // Year's zodiac animal
    ChineseElement yearElement;// Year's primary element
    YinYang yearYinYang;       // Year's yin-yang polarity
    
    // Solar terms and seasons
    SolarTerm currentSolarTerm;// Current solar term
    SolarTerm nextSolarTerm;   // Next solar term
    double nextSolarTermJD;    // Julian day of next solar term
    int daysSinceNewMoon;      // Days since new moon (for moon phase)
    
    // Festival and cultural information
    std::vector<std::string> festivals;     // Traditional festivals on this day
    std::vector<std::string> traditions;    // Cultural traditions/customs
    std::vector<std::string> taboos;        // Things to avoid on this day
    bool isAuspicious;         // General fortune indication
    
    // Integration with other calendars
    double julianDay;          // Julian day for cross-reference
    
    // Formatted strings for display
    std::string ganZhi;        // Combined stem-branch (干支) name
    std::string cycleName;     // 60-year cycle name
    std::string moonPhaseName; // Traditional moon phase name
};

// BaZi (Four Pillars of Destiny 八字) Structure
struct BaZi {
    // Four Pillars (年月日時)
    std::pair<HeavenlyStem, EarthlyBranch> yearPillar;   // 年柱
    std::pair<HeavenlyStem, EarthlyBranch> monthPillar;  // 月柱
    std::pair<HeavenlyStem, EarthlyBranch> dayPillar;    // 日柱
    std::pair<HeavenlyStem, EarthlyBranch> hourPillar;   // 時柱
    
    // Formatted pillar names
    std::string yearPillarName;    // 年柱名
    std::string monthPillarName;   // 月柱名
    std::string dayPillarName;     // 日柱名
    std::string hourPillarName;    // 時柱名
    
    // Five Elements analysis
    std::map<ChineseElement, int> elementCount;  // Count of each element
    ChineseElement dominantElement;              // Most prominent element
    ChineseElement lackingElement;               // Missing or weak element
    
    // Fortune analysis
    std::string lifePalace;        // 命宮 Life Palace
    std::string personalityType;   // Basic personality indication
    std::vector<std::string> strengths;    // Personal strengths
    std::vector<std::string> challenges;   // Areas for improvement
};

// Main Chinese Calendar System
class ChineseCalendar {
public:
    // Astronomical constants
    static constexpr double TROPICAL_YEAR = 365.2421896698;  // days
    static constexpr double SYNODIC_MONTH = 29.5305888544;   // days
    static constexpr double SIDEREAL_YEAR = 365.25636;       // days
    
    // Reference epochs for different eras
    static constexpr double HUANGDI_EPOCH = 1724220.5;       // Yellow Emperor (Feb 15, -2636)
    static constexpr double GREGORIAN_EPOCH = 1721425.5;     // Jan 1, 1 AD
    static constexpr double UNIX_EPOCH = 2440587.5;          // Jan 1, 1970
    
    // Solar term reference (degrees from vernal equinox)
    static constexpr std::array<double, 24> SOLAR_TERM_DEGREES = {
        315, 330, 345, 0, 15, 30,        // Spring
        45, 60, 75, 90, 105, 120,        // Summer  
        135, 150, 165, 180, 195, 210,    // Autumn
        225, 240, 255, 270, 285, 300     // Winter
    };
    
    // Core calculation methods
    int calculateChineseYear(double julianDay) const;
    double calculateSunLongitude(double julianDay) const;
    double calculateSolarTermJD(int year, SolarTerm term) const;
    bool isChineseLeapYear(int chineseYear);
    int determineLeapMonth(int chineseYear);
    double calculateNewMoonJD(int lunarMonth, int year) const;
    double calculateNewMoonIterative(double approximateJD) const;
    
    // 60-year cycle calculations
    static HeavenlyStem calculateStem(int cyclePosition);
    static EarthlyBranch calculateBranch(int cyclePosition);
    static int getSexagenaryYear(int chineseYear);
    static int getSexagenaryMonth(int chineseYear, int chineseMonth);
    static int getSexagenaryDay(double julianDay);
    static int getSexagenaryHour(double julianDay, double hour);
    
    // Zodiac and element calculations
    static ChineseZodiac getZodiacAnimal(int chineseYear);
    static ChineseElement getYearElement(int chineseYear);
    static YinYang getYinYang(int value);
    static ChineseElement getStemElement(HeavenlyStem stem);
    static ChineseElement getBranchElement(EarthlyBranch branch);
    
    // Festival and cultural calculations
    static std::vector<std::string> calculateFestivals(int year, int month, int day, bool isLeapMonth);
    static std::vector<std::string> calculateTraditions(const ChineseCalendarData& data);
    static std::vector<std::string> calculateTaboos(const ChineseCalendarData& data);
    static bool calculateAuspiciousness(const ChineseCalendarData& data);
    
    // Internal state
    bool initialized;
    mutable std::string lastError;
    
    // Precomputed data for performance
    mutable std::map<int, std::vector<double>> solarTermCache;  // year -> solar terms JD
    mutable std::map<int, int> leapMonthCache;                  // year -> leap month (0 if none)

public:
    ChineseCalendar();
    ~ChineseCalendar();
    
    // Initialize the calendar system
    bool initialize();
    
    // Main calculation methods
    ChineseCalendarData calculateChineseCalendar(double julianDay) const;
    // ChineseCalendarData calculateChineseCalendar(const BirthData& birthData) const;
    
    // Bulk calculations
    std::vector<ChineseCalendarData> calculateChineseCalendarRange(
        const std::string& fromDate, const std::string& toDate) const;
    
    // Solar term calculations
    SolarTerm getCurrentSolarTerm(double julianDay) const;
    SolarTerm getNextSolarTerm(double julianDay) const;
    double getNextSolarTermJD(double julianDay) const;
    std::vector<std::pair<SolarTerm, double>> getSolarTermsForYear(int year) const;
    
    // Utility methods
    double gregorianToJulian(int year, int month, int day, double hour = 12.0) const;
    void julianToGregorian(double jd, int& year, int& month, int& day) const;
    
    // Conversion methods
    double chineseToJulian(int year, int month, int day, bool isLeapMonth = false) const;
    void julianToChinese(double jd, int& year, int& month, int& day, bool& isLeapMonth) const;
    
    // String formatting methods - Traditional Chinese
    std::string getHeavenlyStemName(HeavenlyStem stem, bool traditional = true) const;
    std::string getEarthlyBranchName(EarthlyBranch branch, bool traditional = true) const;
    std::string getZodiacAnimalName(ChineseZodiac animal, bool traditional = true) const;
    std::string getElementName(ChineseElement element, bool traditional = true) const;
    std::string getSolarTermName(SolarTerm term, bool traditional = true) const;
    std::string getGanZhiName(HeavenlyStem stem, EarthlyBranch branch, bool traditional = true) const;
    std::string getYinYangName(YinYang yinyang, bool traditional = true) const;
    
    // String formatting methods - English
    std::string getHeavenlyStemNameEn(HeavenlyStem stem) const;
    std::string getEarthlyBranchNameEn(EarthlyBranch branch) const;
    std::string getZodiacAnimalNameEn(ChineseZodiac animal) const;
    std::string getElementNameEn(ChineseElement element) const;
    std::string getSolarTermNameEn(SolarTerm term) const;
    
    // String formatting methods - Pinyin
    std::string getHeavenlyStemPinyin(HeavenlyStem stem) const;
    std::string getEarthlyBranchPinyin(EarthlyBranch branch) const;
    std::string getZodiacAnimalPinyin(ChineseZodiac animal) const;
    std::string getElementPinyin(ChineseElement element) const;
    std::string getSolarTermPinyin(SolarTerm term) const;
    
    // BaZi (Four Pillars) calculations
    BaZi calculateBaZi(double julianDay, double hourOfDay = 12.0) const;
    BaZi calculateBaZi(const BirthData& birthData) const;
    std::string analyzeBaZi(const BaZi& bazi) const;
    
    // Fortune and auspiciousness
    std::string getDailyFortune(const ChineseCalendarData& data) const;
    std::vector<std::string> getAuspiciousActivities(const ChineseCalendarData& data) const;
    std::vector<std::string> getInauspiciousActivities(const ChineseCalendarData& data) const;
    
    // Festival and cultural information
    std::vector<std::string> getMajorFestivals(int year) const;
    std::string getFestivalDescription(const std::string& festivalName) const;
    std::vector<std::string> getSeasonalTraditions(SolarTerm solarTerm) const;
    
    // Output formatting
    std::string generateTable(const ChineseCalendarData& data) const;
    std::string generateDetailedReport(const ChineseCalendarData& data) const;
    std::string generateTraditionalFormat(const ChineseCalendarData& data) const;
    std::string generateModernFormat(const ChineseCalendarData& data) const;
    
    // Multi-language support
    enum class Language { TRADITIONAL_CHINESE, SIMPLIFIED_CHINESE, PINYIN, ENGLISH };
    std::string formatDate(const ChineseCalendarData& data, Language lang = Language::TRADITIONAL_CHINESE) const;
    
    // Utility methods
    bool isValidChineseDate(int year, int month, int day, bool isLeapMonth = false) const;
    int getDaysInChineseMonth(int year, int month, bool isLeapMonth = false) const;
    int getDaysInChineseYear(int year) const;
    
    // Error handling
    std::string getLastError() const { return lastError; }
    
    // Integration with existing systems (to be implemented after adding type dependencies)
    // std::string generateCompatibilityReport(const ChineseCalendarData& chinese,
    //                                       const PanchangaData& hindu,
    //                                       const MyanmarCalendarData& myanmar) const;
};

// Chinese Monthly Calendar (similar to existing monthly calendar classes)
class ChineseMonthlyCalendar {
public:
    struct ChineseMonthlyData {
        // Month information
        int gregorianYear;
        int gregorianMonth;
        std::string gregorianMonthName;
        
        int chineseYear;
        int chineseMonth;
        bool isLeapMonth;
        std::string chineseMonthName;
        std::string monthGanZhi;
        
        // Days data
        struct DayData {
            // Gregorian info
            int gregorianDay;
            double julianDay;
            
            // Chinese info
            int chineseDay;
            std::string dayGanZhi;
            ChineseZodiac dayAnimal;
            ChineseElement dayElement;
            
            // Cultural info
            std::vector<std::string> festivals;
            SolarTerm solarTerm;
            std::string solarTermName;
            bool isAuspicious;
            std::string fortune;
            
            // Multi-calendar integration (if enabled)
            struct HinduCalendarData {
                std::string tithiName;
                std::string nakshatraName;
                std::string paksha;
                bool isEkadashi;
                bool isPurnima;
            } hindu;
            
            struct MyanmarCalendarData {
                int myanmarDay;
                std::string monthName;
                std::string weekdayName;
                bool isSabbath;
                bool isYatyaza;
            } myanmar;
        };
        
        std::vector<DayData> days;
        
        // Monthly statistics and info
        int totalFestivals;
        std::vector<SolarTerm> solarTermsInMonth;
        std::vector<std::pair<SolarTerm, int>> solarTermDays;  // solar term -> day of month
        std::string monthFortune;
        
        // Integration flags
        bool includeHindu = false;
        bool includeMyanmar = false;
        bool includeFortune = true;
        bool includeCulturalInfo = true;
    };
    
private:
    ChineseCalendar chineseCalendar;
    
public:
    ChineseMonthlyCalendar();
    ~ChineseMonthlyCalendar();
    
    // Initialize calendar
    bool initialize();
    
    // Main calculation method
    ChineseMonthlyData calculateMonthlyData(int year, int month, 
                                          bool includeHindu = false,
                                          bool includeMyanmar = false,
                                          bool includeFortune = true,
                                          bool includeCulturalInfo = true) const;
    
    // Output format methods
    std::string generateMonthlyCalendar(const ChineseMonthlyData& monthData, 
                                      const std::string& format = "traditional") const;
    std::string generateTraditionalLayout(const ChineseMonthlyData& monthData) const;
    std::string generateModernLayout(const ChineseMonthlyData& monthData) const;
    std::string generateCompactLayout(const ChineseMonthlyData& monthData) const;
    std::string generateMultiCalendarView(const ChineseMonthlyData& monthData) const;
    
    // Export formats
    std::string exportToJSON(const ChineseMonthlyData& monthData) const;
    std::string exportToXML(const ChineseMonthlyData& monthData) const;
    std::string exportToCSV(const ChineseMonthlyData& monthData) const;
    std::string exportToHTML(const ChineseMonthlyData& monthData) const;
};

} // namespace Astro