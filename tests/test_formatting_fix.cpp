#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

// Test the current formatting function vs accurate formatting
std::string formatPositionCurrent(double longitude) {
    int degrees = static_cast<int>(longitude) % 30;
    int minutes = static_cast<int>((longitude - static_cast<int>(longitude)) * 60) % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << degrees << "°"
        << std::setw(2) << minutes << "'";
    return oss.str();
}

std::string formatPositionFixed(double longitude) {
    // Calculate position within the sign (0-30 degrees)
    double signPosition = fmod(longitude, 30.0);
    if (signPosition < 0) signPosition += 30.0;

    int degrees = static_cast<int>(signPosition);
    double fractionalMinutes = (signPosition - degrees) * 60.0;
    int minutes = static_cast<int>(round(fractionalMinutes));
    
    // Handle rounding overflow
    if (minutes >= 60) {
        degrees += 1;
        minutes = 0;
    }
    if (degrees >= 30) {
        degrees = 29;
        minutes = 59;
    }

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << degrees << "°"
        << std::setw(2) << minutes << "'";
    return oss.str();
}

int main() {
    // Test with the problematic Sun longitude from our validation test
    double sunLongitude = 294.839091;
    
    std::cout << "Sun longitude: " << std::fixed << std::setprecision(6) << sunLongitude << "°" << std::endl;
    
    // Calculate sign position manually
    double signPosition = fmod(sunLongitude, 30.0);
    std::cout << "Position in sign: " << std::fixed << std::setprecision(6) << signPosition << "°" << std::endl;
    
    // Show the exact conversion to degrees and minutes
    int deg = static_cast<int>(signPosition);
    double fractionalMinutes = (signPosition - deg) * 60.0;
    std::cout << "Degrees: " << deg << std::endl;
    std::cout << "Fractional minutes: " << std::fixed << std::setprecision(6) << fractionalMinutes << std::endl;
    std::cout << "Minutes (truncated): " << static_cast<int>(fractionalMinutes) << std::endl;
    std::cout << "Minutes (rounded): " << static_cast<int>(round(fractionalMinutes)) << std::endl;
    
    std::cout << "\nCurrent formatting: " << formatPositionCurrent(sunLongitude) << std::endl;
    std::cout << "Fixed formatting:   " << formatPositionFixed(sunLongitude) << std::endl;
    
    // Test with Moon longitude as well
    double moonLongitude = 350.182537;
    std::cout << "\nMoon longitude: " << std::fixed << std::setprecision(6) << moonLongitude << "°" << std::endl;
    std::cout << "Current formatting: " << formatPositionCurrent(moonLongitude) << std::endl;
    std::cout << "Fixed formatting:   " << formatPositionFixed(moonLongitude) << std::endl;
    
    return 0;
}