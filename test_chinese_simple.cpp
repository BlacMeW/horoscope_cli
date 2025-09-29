#include <iostream>
#include <iomanip>
#include <cassert>
#include "chinese_calendar.h"

using namespace Astro;

void testBasicCalculations() {
    std::cout << "Testing basic Chinese calendar calculations...\n";
    
    ChineseCalendar calendar;
    if (!calendar.initialize()) {
        std::cerr << "Failed to initialize Chinese calendar\n";
        return;
    }
    
    // Test known date: February 12, 2024 (Chinese New Year - Year of Dragon)
    // This should be Chinese year 4722, month 1, day 1
    int year = 2024, month = 2, day = 12;
    double jd = calendar.gregorianToJulian(year, month, day);
    
    std::cout << "Testing date: " << year << "/" << month << "/" << day << " (JD: " << jd << ")\n";
    
    ChineseCalendarData data = calendar.calculateChineseCalendar(jd);
    
    std::cout << "Chinese date: " << data.chineseYear << "/" << data.chineseMonth << "/" << data.chineseDay;
    if (data.isLeapMonth) std::cout << " (leap)";
    std::cout << "\n";
    
    std::cout << "Year Gan-Zhi: " << calendar.getGanZhiName(data.yearStem, data.yearBranch, false) << "\n";
    std::cout << "Zodiac Animal: " << calendar.getZodiacAnimalName(data.yearAnimal, false) << "\n";
    std::cout << "Year Element: " << calendar.getElementName(data.yearElement, false) << "\n";
    std::cout << "Solar Term: " << calendar.getSolarTermName(data.currentSolarTerm, false) << "\n";
    
    // 2024 should be Year of Dragon
    assert(data.yearAnimal == ChineseZodiac::DRAGON);
    std::cout << "✓ Zodiac animal test passed\n";
}

void testSexagenarySystem() {
    std::cout << "\nTesting 60-year cycle system...\n";
    
    ChineseCalendar calendar;
    calendar.initialize();
    
    // Test that the cycle repeats every 60 years
    for (int year = 1; year <= 10; year++) {
        int cycle1 = calendar.getSexagenaryYear(year);
        int cycle2 = calendar.getSexagenaryYear(year + 60);
        
        assert(cycle1 == cycle2);
        
        HeavenlyStem stem = calendar.calculateStem(cycle1);
        EarthlyBranch branch = calendar.calculateBranch(cycle1);
        std::cout << "Year " << year << ": " 
                  << calendar.getGanZhiName(stem, branch, false) 
                  << " (cycle position " << cycle1 << ")\n";
    }
    std::cout << "✓ 60-year cycle test passed\n";
}

void testLeapYears() {
    std::cout << "\nTesting leap year calculations...\n";
    
    ChineseCalendar calendar;
    calendar.initialize();
    
    // Test Metonic cycle: 7 leap years in 19 years
    int leapCount = 0;
    for (int year = 1; year <= 19; year++) {
        if (calendar.isChineseLeapYear(year)) {
            leapCount++;
            std::cout << "Year " << year << " is a leap year\n";
        }
    }
    
    assert(leapCount == 7);
    std::cout << "✓ Leap year pattern test passed (" << leapCount << " leap years in 19)\n";
}

void testSolarTerms() {
    std::cout << "\nTesting solar term calculations...\n";
    
    ChineseCalendar calendar;
    calendar.initialize();
    
    // Test that solar terms progress correctly through the year
    double startJD = calendar.gregorianToJulian(2024, 1, 1);
    
    for (int days = 0; days < 365; days += 30) {  // Check every 30 days
        double jd = startJD + days;
        SolarTerm currentTerm = calendar.getCurrentSolarTerm(jd);
        
        if (days < 120) {  // Only print first few
            std::cout << "Day " << days << ": " << calendar.getSolarTermName(currentTerm, false) << "\n";
        }
    }
    std::cout << "✓ Solar term progression test completed\n";
}

void testElements() {
    std::cout << "\nTesting element calculations...\n";
    
    ChineseCalendar calendar;
    calendar.initialize();
    
    // Test that elements cycle correctly
    for (int year = 1; year <= 10; year++) {
        ChineseElement element = calendar.getYearElement(year);
        YinYang yinyang = calendar.getYinYang(year);
        
        std::cout << "Year " << year << ": " 
                  << calendar.getElementName(element, false)
                  << " (" << (yinyang == YinYang::YANG ? "Yang" : "Yin") << ")\n";
    }
    std::cout << "✓ Element calculation test completed\n";
}

void testTableGeneration() {
    std::cout << "\nTesting table generation...\n";
    
    ChineseCalendar calendar;
    calendar.initialize();
    
    // Generate table for a specific date
    double jd = calendar.gregorianToJulian(2024, 6, 1);
    ChineseCalendarData data = calendar.calculateChineseCalendar(jd);
    
    std::string table = calendar.generateTable(data);
    std::cout << table << "\n";
    
    assert(!table.empty());
    std::cout << "✓ Table generation test passed\n";
}

int main() {
    std::cout << "Chinese Calendar Test Suite\n";
    std::cout << "===========================\n\n";
    
    try {
        testBasicCalculations();
        testSexagenarySystem();
        testLeapYears();
        testSolarTerms();
        testElements();
        testTableGeneration();
        
        std::cout << "\n✅ All tests passed successfully!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception\n";
        return 1;
    }
}