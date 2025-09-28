#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

// Test the house position calculation
double normalizeAngle(double angle) {
    return angle - floor(angle / 360.0) * 360.0;
}

double getHousePosition(double currentCusp, double nextCusp, double longitude) {
    currentCusp = normalizeAngle(currentCusp);
    nextCusp = normalizeAngle(nextCusp);
    longitude = normalizeAngle(longitude);

    double houseSize;
    double positionInHouse;

    if (nextCusp > currentCusp) {
        // Normal case: cusp doesn't cross 0°
        houseSize = nextCusp - currentCusp;
        positionInHouse = longitude - currentCusp;
    } else {
        // Cusp crosses 0° Aries
        houseSize = (360.0 - currentCusp) + nextCusp;
        if (longitude >= currentCusp) {
            positionInHouse = longitude - currentCusp;
        } else {
            positionInHouse = (360.0 - currentCusp) + longitude;
        }
    }

    // Convert to 0-30 degree scale (as if each house were 30 degrees)
    return (positionInHouse / houseSize) * 30.0;
}

void testHousePositions() {
    std::cout << "=== Testing House Position Calculations ===" << std::endl;
    
    // Test normal house (no crossing)
    std::cout << "Normal House (120° to 150°):" << std::endl;
    double currentCusp = 120.0;
    double nextCusp = 150.0;
    
    std::vector<double> testLongitudes = {120.0, 125.0, 135.0, 145.0, 149.9};
    
    for (double longitude : testLongitudes) {
        double position = getHousePosition(currentCusp, nextCusp, longitude);
        double expectedPos = ((longitude - currentCusp) / 30.0) * 30.0; // Should be linear
        
        std::cout << "  " << longitude << "° -> " << std::fixed << std::setprecision(2) 
                  << position << "° (expected ~" << expectedPos << "°)" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test crossing house
    std::cout << "Crossing House (350° to 30°, size 40°):" << std::endl;
    currentCusp = 350.0;
    nextCusp = 30.0;
    double houseSize = (360.0 - 350.0) + 30.0; // = 40°
    
    testLongitudes = {350.0, 355.0, 0.0, 10.0, 20.0, 29.9};
    
    for (double longitude : testLongitudes) {
        double position = getHousePosition(currentCusp, nextCusp, longitude);
        
        // Calculate expected position manually
        double posInHouse;
        if (longitude >= 350.0) {
            posInHouse = longitude - 350.0;
        } else {
            posInHouse = (360.0 - 350.0) + longitude;
        }
        double expectedPos = (posInHouse / houseSize) * 30.0;
        
        std::cout << "  " << longitude << "° -> " << std::fixed << std::setprecision(2) 
                  << position << "° (expected " << expectedPos << "°)";
        
        if (std::abs(position - expectedPos) < 0.01) {
            std::cout << " ✓";
        } else {
            std::cout << " *** ERROR ***";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
}

void testEdgeCases() {
    std::cout << "=== Testing Edge Cases ===" << std::endl;
    
    // Test very small house
    std::cout << "Tiny House (359° to 1°, size 2°):" << std::endl;
    double currentCusp = 359.0;
    double nextCusp = 1.0;
    
    std::vector<double> testLongitudes = {359.0, 359.5, 0.0, 0.5, 0.9};
    
    for (double longitude : testLongitudes) {
        double position = getHousePosition(currentCusp, nextCusp, longitude);
        std::cout << "  " << longitude << "° -> " << std::fixed << std::setprecision(2) 
                  << position << "°" << std::endl;
        
        // Verify position is within valid range
        if (position < 0.0 || position > 30.0) {
            std::cout << "    *** ERROR: Position outside 0-30° range! ***" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "House Position Calculation Testing" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;
    
    testHousePositions();
    testEdgeCases();
    
    return 0;
}