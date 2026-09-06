#include <iostream>
#include <iomanip>

int main() {
    // Julian Day calculation for verification
    // JD for 2000-01-01 12:00:00 UTC (J2000.0) = 2451545.0
    
    // Calculate days from J2000.0 to 2025-01-01
    // From 2000-01-01 to 2025-01-01 = 25 years
    // 25 years * 365.25 days/year = 9131.25 days (accounting for leap years)
    
    double j2000 = 2451545.0; // J2000.0 epoch (noon UTC)
    int yearsFrom2000 = 25;
    double daysFrom2000 = yearsFrom2000 * 365.25; // Approximate
    
    // More precise: count actual days including leap years
    // Leap years from 2000-2024: 2000, 2004, 2008, 2012, 2016, 2020, 2024 = 7 leap years
    int regularYears = 25 - 7; // 18 regular years  
    int leapYears = 7;
    double exactDays = regularYears * 365 + leapYears * 366;
    
    double jd2025Jan01Noon = j2000 + exactDays;
    double jd2025Jan01Midnight = jd2025Jan01Noon - 0.5; // Midnight is JD.5, noon is JD.0
    double jd2025Jan01_05UTC = jd2025Jan01Midnight + 5.0/24.0; // Add 5 hours for 05:00 UTC
    
    std::cout << "=== JULIAN DAY VERIFICATION ===" << std::endl;
    std::cout << "J2000.0 (2000-01-01 12:00 UTC): " << std::fixed << std::setprecision(1) << j2000 << std::endl;
    std::cout << "Years from J2000 to 2025: " << yearsFrom2000 << std::endl;
    std::cout << "Exact days (accounting for leap years): " << exactDays << std::endl;
    std::cout << "2025-01-01 12:00 UTC: " << std::fixed << std::setprecision(1) << jd2025Jan01Noon << std::endl;
    std::cout << "2025-01-01 00:00 UTC: " << std::fixed << std::setprecision(1) << jd2025Jan01Midnight << std::endl;  
    std::cout << "2025-01-01 05:00 UTC: " << std::fixed << std::setprecision(6) << jd2025Jan01_05UTC << std::endl;
    
    std::cout << "\nCLI calculates: 2460676.708333" << std::endl;
    std::cout << "Expected should be: " << std::fixed << std::setprecision(6) << jd2025Jan01_05UTC << std::endl;
    std::cout << "Difference: " << (2460676.708333 - jd2025Jan01_05UTC) << " days" << std::endl;
    
    if (std::abs(2460676.708333 - jd2025Jan01_05UTC) < 0.1) {
        std::cout << "✅ CLI Julian Day calculation is CORRECT!" << std::endl;
    } else {
        std::cout << "❌ CLI Julian Day calculation has an error" << std::endl;
    }
    
    return 0;
}