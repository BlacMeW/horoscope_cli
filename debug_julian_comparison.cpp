#include <iostream>
#include <iomanip>
#include "third_party/swisseph/swephexp.h"

int main() {
    // Test program approach (simple)
    double testJD = swe_julday(2024, 9, 28, 12.0, SE_GREG_CAL);
    
    // Hindu Panchanga approach (complex UTC conversion)
    // Simulating what BirthData::getJulianDay() does for 2024-09-28 00:00:00 +7 timezone
    double localTime = 0.0; // midnight local time
    double timezone = 7.0;
    double utcTime = localTime - timezone; // -7.0 hours = previous day 17:00
    
    int utcYear = 2024;
    int utcMonth = 9;
    int utcDay = 28;
    
    // Handle negative UTC time (previous day)
    if (utcTime < 0.0) {
        utcTime += 24.0; // 17.0 hours
        utcDay -= 1; // 27th
    }
    
    double hinduJD = swe_julday(utcYear, utcMonth, utcDay, utcTime, SE_GREG_CAL);
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "=== JULIAN DAY COMPARISON ===" << std::endl;
    std::cout << "Test Program JD:    " << testJD << std::endl;
    std::cout << "Hindu Panchanga JD: " << hinduJD << std::endl;
    std::cout << "Difference:         " << (testJD - hinduJD) << " days" << std::endl;
    std::cout << "Difference:         " << (testJD - hinduJD) * 24 << " hours" << std::endl;
    
    return 0;
}