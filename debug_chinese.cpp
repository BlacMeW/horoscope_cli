#include <iostream>
#include <iomanip>
#include "chinese_calendar.h"

using namespace Astro;

int main() {
    std::cout << "Chinese Calendar Debug Test\n";
    std::cout << "===========================\n\n";
    
    ChineseCalendar calendar;
    if (!calendar.initialize()) {
        std::cerr << "Failed to initialize Chinese calendar\n";
        return 1;
    }
    
    // Test the Year of Dragon in 2024
    int year = 2024, month = 2, day = 12;
    double jd = calendar.gregorianToJulian(year, month, day);
    
    std::cout << "Gregorian date: " << year << "/" << month << "/" << day << "\n";
    std::cout << "Julian Day: " << std::fixed << std::setprecision(1) << jd << "\n";
    
    int chineseYear = calendar.calculateChineseYear(jd);
    std::cout << "Chinese Year: " << chineseYear << "\n";
    
    // Debug the sexagenary calculation
    int sexYear = calendar.getSexagenaryYear(chineseYear);
    std::cout << "Sexagenary cycle position: " << sexYear << "\n";
    
    HeavenlyStem stem = calendar.calculateStem(sexYear);
    EarthlyBranch branch = calendar.calculateBranch(sexYear);
    
    std::cout << "Stem: " << static_cast<int>(stem) << " (" << calendar.getHeavenlyStemName(stem, false) << ")\n";
    std::cout << "Branch: " << static_cast<int>(branch) << " (" << calendar.getEarthlyBranchName(branch, false) << ")\n";
    
    ChineseZodiac animal = calendar.getZodiacAnimal(chineseYear);
    std::cout << "Zodiac Animal: " << static_cast<int>(animal) << " (" << calendar.getZodiacAnimalName(animal, false) << ")\n";
    
    // Expected: 2024 = Year of Dragon
    // Dragon = 5th animal in cycle
    std::cout << "\nExpected for 2024: Dragon (5)\n";
    std::cout << "Dragon years: 2024, 2012, 2000, 1988, 1976...\n";
    
    // Let's test a few known years
    std::cout << "\nTesting known Dragon years:\n";
    for (int testYear : {2012, 2000, 1988, 1976}) {
        double testJD = calendar.gregorianToJulian(testYear, 2, 15);
        int testChineseYear = calendar.calculateChineseYear(testJD);
        ChineseZodiac testAnimal = calendar.getZodiacAnimal(testChineseYear);
        std::cout << testYear << " -> Chinese " << testChineseYear << " -> " 
                  << calendar.getZodiacAnimalName(testAnimal, false) << "\n";
    }
    
    return 0;
}