#include <iostream>
#include <iomanip>

// Simple reverse Julian Day function to check what date a JD represents
void reverseJulianDay(double jd) {
    // Simplified reverse calculation (approximation)
    double a = jd + 32044;
    double b = (4 * a + 3) / 146097;
    double c = a - (146097 * b) / 4;
    double d = (4 * c + 3) / 1461;
    double e = c - (1461 * d) / 4;
    double m = (5 * e + 2) / 153;
    
    int day = (int)(e - (153 * m + 2) / 5 + 1);
    int month = (int)(m + 3 - 12 * (m / 10));
    int year = (int)(100 * b + d - 4800 + m / 10);
    
    double fraction = jd - (int)jd;
    double hours = (fraction - 0.5) * 24.0;
    if (hours < 0) hours += 24.0;
    
    std::cout << "JD " << std::fixed << std::setprecision(6) << jd 
              << " = " << year << "-" << std::setfill('0') << std::setw(2) << month 
              << "-" << std::setw(2) << day 
              << " " << std::setw(2) << (int)hours << ":" 
              << std::setw(2) << (int)((hours - (int)hours) * 60) 
              << " UTC" << std::endl;
}

int main() {
    std::cout << "=== JULIAN DAY ANALYSIS ===" << std::endl;
    
    double calculatedJD = 2460676.708333; // What our CLI calculates
    double expectedJD = 2460677.708333;   // What it should be
    
    std::cout << "CLI calculated JD: ";
    reverseJulianDay(calculatedJD);
    
    std::cout << "Expected JD: ";
    reverseJulianDay(expectedJD);
    
    std::cout << "\nThe CLI is calculating JD for December 31, 2024 instead of January 1, 2025!" << std::endl;
    std::cout << "This suggests the timezone conversion is causing a day rollback error." << std::endl;
    
    return 0;
}