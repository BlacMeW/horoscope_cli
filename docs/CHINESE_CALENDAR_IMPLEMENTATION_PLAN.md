# Chinese Calendar Implementation Plan
## Full-Featured Chinese Lunisolar Calendar System

### 1. OVERVIEW
This document outlines the complete implementation of a Chinese Calendar system for the horoscope_cli repository, following the existing patterns of Hindu and Myanmar calendar systems.

### 2. CORE CHINESE CALENDAR CONCEPTS

#### 2.1 Fundamental Systems
- **60-Year Cycle (甲子 Jiǎzǐ)**: 10 Heavenly Stems × 12 Earthly Branches
- **12 Zodiac Animals**: Rat, Ox, Tiger, Rabbit, Dragon, Snake, Horse, Goat, Monkey, Rooster, Dog, Pig
- **5 Elements (Wu Xing 五行)**: Wood, Fire, Earth, Metal, Water
- **Yin-Yang (陰陽)**: Duality principle applied to years, months, days, hours
- **24 Solar Terms (二十四節氣)**: Seasonal markers based on sun's position

#### 2.2 Calendar Structure
- **Lunar Months**: 29 or 30 days, following moon phases
- **Leap Months**: Inserted ~7 times per 19 years to sync with solar year
- **Solar Terms**: Determine seasons and agricultural timing
- **Traditional Hours**: 12 double-hours (each = 2 modern hours)

### 3. DETAILED IMPLEMENTATION PLAN

#### 3.1 Header File Structure (`include/chinese_calendar.h`)

```cpp
#pragma once
#include "astro_types.h"
#include <string>
#include <vector>
#include <array>
#include <map>

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

// 24 Solar Terms (二十四節氣)
enum class SolarTerm {
    // Spring (春)
    LICHUN = 1,      // 立春 Beginning of Spring
    YUSHUI = 2,      // 雨水 Rain Water
    JINGZHE = 3,     // 驚蟄 Awakening of Insects
    CHUNFEN = 4,     // 春分 Vernal Equinox
    QINGMING = 5,    // 清明 Clear and Bright
    GUYU = 6,        // 穀雨 Grain Rain
    
    // Summer (夏)
    LIXIA = 7,       // 立夏 Beginning of Summer
    XIAOMAN = 8,     // 小滿 Grain Buds
    MANGZHONG = 9,   // 芒種 Grain in Ear
    XIAZHI = 10,     // 夏至 Summer Solstice
    XIAOSHU = 11,    // 小暑 Slight Heat
    DASHU = 12,      // 大暑 Great Heat
    
    // Autumn (秋)
    LIQIU = 13,      // 立秋 Beginning of Autumn
    CHUSHU = 14,     // 處暑 Stopping the Heat
    BAILU = 15,      // 白露 White Dew
    QIUFEN = 16,     // 秋分 Autumnal Equinox
    HANLU = 17,      // 寒露 Cold Dew
    SHUANGJIANG = 18,// 霜降 Frost's Descent
    
    // Winter (冬)
    LIDONG = 19,     // 立冬 Beginning of Winter
    XIAOXUE = 20,    // 小雪 Slight Snow
    DAXUE = 21,      // 大雪 Great Snow
    DONGZHI = 22,    // 冬至 Winter Solstice
    XIAOHAN = 23,    // 小寒 Slight Cold
    DAHAN = 24       // 大寒 Great Cold
};

// Chinese Calendar Data Structure
struct ChineseCalendarData {
    // Basic date information
    int chineseYear;           // Chinese year number
    int chineseMonth;          // Chinese month (1-12, with possible leap)
    int chineseDay;            // Day of Chinese month (1-29/30)
    bool isLeapMonth;          // Is this a leap month?
    
    // 60-year cycle information
    HeavenlyStem yearStem;     // Year's heavenly stem
    EarthlyBranch yearBranch;  // Year's earthly branch
    HeavenlyStem monthStem;    // Month's heavenly stem
    EarthlyBranch monthBranch; // Month's earthly branch
    HeavenlyStem dayStem;      // Day's heavenly stem
    EarthlyBranch dayBranch;   // Day's earthly branch
    
    // Zodiac and elements
    ChineseZodiac yearAnimal;  // Year's zodiac animal
    ChineseElement yearElement;// Year's element
    bool isYinYear;            // Is year Yin or Yang?
    
    // Solar terms
    SolarTerm currentSolarTerm;// Current solar term
    SolarTerm nextSolarTerm;   // Next solar term
    double solarTermJD;        // Julian day of next solar term
    
    // Festival and cultural information
    std::vector<std::string> festivals;  // Traditional festivals
    std::vector<std::string> traditions; // Cultural traditions
    bool isAuspicious;         // General fortune indication
    
    // Integration with other calendars
    double julianDay;          // Julian day for cross-reference
    std::string ganZhi;        // Combined stem-branch (干支) name
    std::string cycleName;     // 60-year cycle name
};

class ChineseCalendar {
private:
    // Astronomical constants
    static constexpr double TROPICAL_YEAR = 365.2421896698; // days
    static constexpr double SYNODIC_MONTH = 29.5305888544;  // days
    static constexpr double CHINESE_EPOCH = 1724220.5;     // Feb 15, -2636 (Huangdi era)
    
    // Reference epochs for different eras
    static constexpr double HUANGDI_EPOCH = 1724220.5;     // Yellow Emperor
    static constexpr double CONFUCIUS_EPOCH = 1566583.5;   // Confucian era
    static constexpr double REPUBLICAN_EPOCH = 2419403.5;  // Republic of China (1912)
    
    // Core calculation methods
    static int calculateChineseYear(double julianDay);
    static bool isChineseLeapYear(int chineseYear);
    static int getLeapMonth(int chineseYear);
    static double calculateNewMoonJD(int year, int month);
    static double calculateSolarTermJD(int year, SolarTerm term);
    
    // 60-year cycle calculations
    static HeavenlyStem calculateStem(int value);
    static EarthlyBranch calculateBranch(int value);
    static ChineseZodiac getZodiacAnimal(int year);
    static ChineseElement getYearElement(int year);
    
    // Festival and cultural calculations
    static std::vector<std::string> getFestivals(int year, int month, int day, bool isLeapMonth);
    static bool isAuspiciousDay(const ChineseCalendarData& data);
    
    bool initialized;
    mutable std::string lastError;

public:
    ChineseCalendar();
    ~ChineseCalendar();
    
    // Initialize the calendar system
    bool initialize();
    
    // Main calculation methods
    ChineseCalendarData calculateChineseCalendar(double julianDay) const;
    ChineseCalendarData calculateChineseCalendar(const BirthData& birthData) const;
    
    // Bulk calculations
    std::vector<ChineseCalendarData> calculateChineseCalendarRange(
        const std::string& fromDate, const std::string& toDate) const;
    
    // Solar term calculations
    double getNextSolarTerm(double julianDay) const;
    std::vector<std::pair<SolarTerm, double>> getSolarTermsForYear(int year) const;
    
    // Conversion methods
    double chineseToJulian(int year, int month, int day, bool isLeapMonth = false) const;
    static void julianToChinese(double jd, int& year, int& month, int& day, bool& isLeapMonth);
    
    // String formatting methods
    std::string getHeavenlyStemName(HeavenlyStem stem, bool traditional = true) const;
    std::string getEarthlyBranchName(EarthlyBranch branch, bool traditional = true) const;
    std::string getZodiacAnimalName(ChineseZodiac animal, bool traditional = true) const;
    std::string getElementName(ChineseElement element, bool traditional = true) const;
    std::string getSolarTermName(SolarTerm term, bool traditional = true) const;
    std::string getGanZhiName(HeavenlyStem stem, EarthlyBranch branch, bool traditional = true) const;
    
    // BaZi (Four Pillars) calculations
    struct BaZi {
        std::pair<HeavenlyStem, EarthlyBranch> year;
        std::pair<HeavenlyStem, EarthlyBranch> month;
        std::pair<HeavenlyStem, EarthlyBranch> day;
        std::pair<HeavenlyStem, EarthlyBranch> hour;
        
        std::string yearPillar;
        std::string monthPillar;
        std::string dayPillar;
        std::string hourPillar;
    };
    
    BaZi calculateBaZi(double julianDay, double hourOfDay = 12.0) const;
    
    // Output formatting
    std::string generateTable(const ChineseCalendarData& data) const;
    std::string generateDetailedReport(const ChineseCalendarData& data) const;
    
    // Error handling
    std::string getLastError() const { return lastError; }
};

} // namespace Astro
```

#### 3.2 Implementation File (`src/chinese_calendar.cpp`)

**Key Implementation Sections:**

##### 3.2.1 Core Algorithms
```cpp
// Lunar month calculation with leap month detection
int ChineseCalendar::getLeapMonth(int chineseYear) {
    // Calculate 13 new moons starting from winter solstice of previous year
    // Find month without major solar term (中氣) - that's the leap month
}

// Solar term calculation using Swiss Ephemeris
double ChineseCalendar::calculateSolarTermJD(int year, SolarTerm term) {
    // Use sun's longitude to determine precise solar term timing
    // Each solar term = 15° increment starting from spring equinox
}

// 60-year cycle calculations
HeavenlyStem ChineseCalendar::calculateStem(int value) {
    return static_cast<HeavenlyStem>((value - 1) % 10 + 1);
}
```

##### 3.2.2 Festival Calculations
```cpp
std::vector<std::string> ChineseCalendar::getFestivals(int year, int month, int day, bool isLeapMonth) {
    std::vector<std::string> festivals;
    
    // Major festivals
    if (month == 1 && day == 1) festivals.push_back("春節 Spring Festival");
    if (month == 1 && day == 15) festivals.push_back("元宵節 Lantern Festival");
    if (month == 5 && day == 5) festivals.push_back("端午節 Dragon Boat Festival");
    if (month == 7 && day == 7) festivals.push_back("七夕 Qixi Festival");
    if (month == 8 && day == 15) festivals.push_back("中秋節 Mid-Autumn Festival");
    // ... more festivals
    
    return festivals;
}
```

#### 3.3 Monthly Calendar Implementation (`include/chinese_monthly_calendar.h`)

```cpp
class ChineseMonthlyCalendar {
public:
    struct ChineseMonthlyData {
        // Month information
        int chineseYear;
        int chineseMonth;
        bool isLeapMonth;
        std::string monthName;
        std::string monthGanZhi;
        
        // Days data
        struct DayData {
            int gregorianDay;
            int chineseDay;
            std::string dayGanZhi;
            ChineseZodiac dayAnimal;
            std::vector<std::string> festivals;
            SolarTerm solarTerm;
            bool isAuspicious;
            
            // Multi-calendar integration
            HinduCalendarData hindu;    // From existing system
            MyanmarCalendarData myanmar; // From existing system
        };
        
        std::vector<DayData> days;
        
        // Monthly statistics
        int totalFestivals;
        std::vector<SolarTerm> solarTermsInMonth;
    };
    
    // Generate various output formats
    std::string generateTraditionalLayout(const ChineseMonthlyData& monthData) const;
    std::string generateModernLayout(const ChineseMonthlyData& monthData) const;
    std::string generateMultiCalendarView(const ChineseMonthlyData& monthData) const;
};
```

#### 3.4 Integration Points

##### 3.4.1 Multi-Calendar Data Structure
```cpp
// Extend MyanmarMonthlyData to include Chinese calendar
struct MyanmarMonthlyData::DayData {
    // ... existing fields ...
    
    struct ChineseCalendarData {
        int chineseDay;
        std::string dayGanZhi;
        ChineseZodiac dayAnimal;
        ChineseElement dayElement;
        std::vector<std::string> festivals;
        SolarTerm currentSolarTerm;
        bool isAuspicious;
    } chinese;
};
```

##### 3.4.2 Main Program Integration
```cpp
// Add Chinese calendar option to main.cpp
if (mode == "chinese-calendar") {
    ChineseCalendar chineseCalendar;
    ChineseCalendarData result = chineseCalendar.calculateChineseCalendar(birthData);
    
    std::cout << "Chinese Calendar: " << result.chineseYear << "/" 
              << result.chineseMonth << "/" << result.chineseDay << std::endl;
    std::cout << "Gan-Zhi: " << result.ganZhi << std::endl;
    std::cout << "Zodiac: " << getZodiacAnimalName(result.yearAnimal) << std::endl;
    // ... more output
}
```

### 4. TESTING STRATEGY

#### 4.1 Test Cases (`test_chinese_calendar.cpp`)
```cpp
// Test known historical dates
struct ChineseTestDate {
    double julianDay;
    int expectedYear;
    int expectedMonth;
    int expectedDay;
    bool expectedLeapMonth;
    std::string expectedGanZhi;
    ChineseZodiac expectedAnimal;
};

ChineseTestDate testDates[] = {
    {2451545.0, 4697, 11, 7, false, "己卯", ChineseZodiac::RABBIT},  // 2000-01-01
    {2299161.0, 4280, 9, 2, false, "壬戌", ChineseZodiac::DOG},      // 1582-10-15
    // ... more test cases
};
```

#### 4.2 Solar Term Verification
```cpp
// Verify solar terms against astronomical data
void testSolarTerms() {
    ChineseCalendar calendar;
    
    // Test 2024 winter solstice
    double dongzhi2024 = calendar.calculateSolarTermJD(2024, SolarTerm::DONGZHI);
    // Should be around December 21, 2024
    
    // Verify all 24 solar terms for a year
    auto solarTerms = calendar.getSolarTermsForYear(2024);
    assert(solarTerms.size() == 24);
}
```

### 5. CULTURAL FEATURES

#### 5.1 Traditional Festivals Database
- **Major Festivals**: Spring Festival, Lantern, Dragon Boat, Mid-Autumn, etc.
- **Seasonal Celebrations**: Based on 24 solar terms
- **Regional Variations**: Different traditions across China

#### 5.2 Fortune and Astrology
- **Daily Fortune**: Based on stem-branch combinations
- **Auspicious/Inauspicious Days**: Traditional calendar wisdom
- **BaZi Analysis**: Four Pillars fortune telling system

#### 5.3 Multiple Language Support
- **Traditional Chinese**: 繁體中文
- **Simplified Chinese**: 简体中文  
- **Pinyin**: Romanized pronunciation
- **English**: Full translations

### 6. ADVANCED FEATURES

#### 6.1 Historical Eras Support
- **Huangdi Era**: Yellow Emperor calendar (traditional)
- **Confucian Era**: Confucius-based counting
- **Republican Era**: Republic of China (1912-)
- **People's Republic**: PRC era (1949-)

#### 6.2 Regional Variations
- **Mainland China**: Standard implementation
- **Taiwan**: Traditional practices
- **Hong Kong**: Cantonese traditions
- **Overseas Chinese**: Diaspora variations

### 7. OUTPUT FORMATS

#### 7.1 Traditional Format
```
農曆二○二四年十一月初七日 (甲辰年 丙子月 己卯日)
生肖：龍年
五行：木龍
節氣：大雪
吉凶：宜
```

#### 7.2 Modern Format
```
Chinese Calendar: 2024/11/7
Gan-Zhi: JiaChén Year, BingZi Month, JiMao Day  
Zodiac: Dragon Year
Element: Wood Dragon
Solar Term: Great Snow
Fortune: Auspicious
```

### 8. IMPLEMENTATION TIMELINE

1. **Week 1**: Core data structures and enums
2. **Week 2**: Basic Julian Day conversion algorithms  
3. **Week 3**: 60-year cycle and zodiac calculations
4. **Week 4**: Solar terms and leap month logic
5. **Week 5**: Festival and cultural feature database
6. **Week 6**: Monthly calendar integration
7. **Week 7**: Multi-calendar view implementation
8. **Week 8**: Testing and documentation

This comprehensive plan ensures a full-featured Chinese Calendar system that integrates seamlessly with your existing Hindu and Myanmar calendar implementations while providing rich cultural and astronomical features.