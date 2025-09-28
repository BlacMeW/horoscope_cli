#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

// Minimal structures needed for testing
struct HouseCusps {
    double cusps[13];       // Houses 1-12 (index 0 unused)
    
    int getHouseForLongitude(double longitude) const;
};

// Normalize angle to 0-360 range
double normalizeAngle(double angle) {
    return angle - floor(angle / 360.0) * 360.0;
}

// Implement the house finding logic from astro_types.cpp
int HouseCusps::getHouseForLongitude(double longitude) const {
    longitude = normalizeAngle(longitude);

    for (int house = 1; house <= 12; house++) {
        double currentCusp = normalizeAngle(cusps[house]);
        double nextCusp = normalizeAngle(cusps[house == 12 ? 1 : house + 1]);

        if (nextCusp > currentCusp) {
            // Normal case: cusp doesn't cross 0°
            if (longitude >= currentCusp && longitude < nextCusp) {
                return house;
            }
        } else {
            // Cusp crosses 0° Aries
            if (longitude >= currentCusp || longitude < nextCusp) {
                return house;
            }
        }
    }

    return 1; // Default to first house
}

// Implement the house position calculation from house_calculator.cpp
double getHousePosition(const HouseCusps& cusps, double longitude, int house) {
    if (house < 1 || house > 12) return 0.0;

    double currentCusp = normalizeAngle(cusps.cusps[house]);
    double nextCusp = normalizeAngle(cusps.cusps[house == 12 ? 1 : house + 1]);
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

void testHouseCrossingZeroAries() {
    std::cout << "=== Testing House Position Calculation (0° Aries Crossing) ===" << std::endl;
    
    // Test case: House 12 cusp at 330°, House 1 cusp at 30°
    // This simulates a house crossing 0° Aries
    HouseCusps cusps;
    for (int i = 0; i <= 12; i++) cusps.cusps[i] = 0.0;
    
    cusps.cusps[12] = 330.0;  // House 12 starts at 330°
    cusps.cusps[1] = 30.0;    // House 1 starts at 30°
    cusps.cusps[2] = 60.0;    // House 2 starts at 60°
    
    // Test planets in house 12 (between 330° and 30°)
    std::vector<double> testLongitudes = {330.0, 350.0, 0.0, 10.0, 29.9};
    
    for (double longitude : testLongitudes) {
        int house = cusps.getHouseForLongitude(longitude);
        double position = HouseCalculator::getHousePosition(cusps, longitude, house);
        
        std::cout << "Longitude " << std::fixed << std::setprecision(1) << longitude 
                  << "° -> House " << house << ", Position " << std::setprecision(2) 
                  << position << "°" << std::endl;
    }
    
    std::cout << std::endl;
}

void testNormalHousePositions() {
    std::cout << "=== Testing Normal House Position Calculation ===" << std::endl;
    
    // Test case: House 5 cusp at 120°, House 6 cusp at 150°
    HouseCusps cusps;
    for (int i = 0; i <= 12; i++) cusps.cusps[i] = 0.0;
    
    cusps.cusps[5] = 120.0;  // House 5 starts at 120°
    cusps.cusps[6] = 150.0;  // House 6 starts at 150°
    
    // Test planets in house 5 (between 120° and 150°)
    std::vector<double> testLongitudes = {120.0, 125.0, 135.0, 145.0, 149.9};
    
    for (double longitude : testLongitudes) {
        int house = cusps.getHouseForLongitude(longitude);
        double position = HouseCalculator::getHousePosition(cusps, longitude, house);
        
        std::cout << "Longitude " << std::fixed << std::setprecision(1) << longitude 
                  << "° -> House " << house << ", Position " << std::setprecision(2) 
                  << position << "°" << std::endl;
        
        // Verify that position is within 0-30° range
        if (position < 0.0 || position > 30.0) {
            std::cout << "  ERROR: Position outside valid range 0-30°!" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testHouseEdgeCases() {
    std::cout << "=== Testing House Calculation Edge Cases ===" << std::endl;
    
    HouseCusps cusps;
    
    // Test edge case: Very small house
    cusps.cusps[1] = 0.0;
    cusps.cusps[2] = 1.0;  // Only 1° house size
    
    double longitude = 0.5;  // Middle of tiny house
    int house = cusps.getHouseForLongitude(longitude);
    double position = HouseCalculator::getHousePosition(cusps, longitude, house);
    
    std::cout << "Tiny house test: " << longitude << "° -> House " << house 
              << ", Position " << position << "°" << std::endl;
    
    // Test edge case: Large house crossing 0°
    cusps.cusps[12] = 300.0;
    cusps.cusps[1] = 100.0;   // 160° house size (300° to 100°)
    
    longitude = 0.0;  // Exactly at 0°
    house = cusps.getHouseForLongitude(longitude);
    position = HouseCalculator::getHousePosition(cusps, longitude, house);
    
    std::cout << "Large crossing house: " << longitude << "° -> House " << house 
              << ", Position " << position << "°" << std::endl;
    
    std::cout << std::endl;
}

// Test the mathematical logic of house position calculation manually
void analyzeHousePositionMath() {
    std::cout << "=== Analyzing House Position Mathematical Logic ===" << std::endl;
    
    // Manually calculate what should happen for house crossing 0°
    double currentCusp = 330.0;  // House 12 cusp
    double nextCusp = 30.0;      // House 1 cusp (next house)
    double longitude = 10.0;     // Planet at 10°
    
    std::cout << "Manual calculation for crossing case:" << std::endl;
    std::cout << "Current cusp: " << currentCusp << "°" << std::endl;
    std::cout << "Next cusp: " << nextCusp << "°" << std::endl;
    std::cout << "Planet longitude: " << longitude << "°" << std::endl;
    
    // Calculate house size (crossing case)
    double houseSize = (360.0 - currentCusp) + nextCusp;
    std::cout << "House size: (360.0 - " << currentCusp << ") + " << nextCusp << " = " << houseSize << "°" << std::endl;
    
    // Calculate position in house
    double positionInHouse;
    if (longitude >= currentCusp) {
        positionInHouse = longitude - currentCusp;
        std::cout << "Position (>=cusp): " << longitude << " - " << currentCusp << " = " << positionInHouse << "°" << std::endl;
    } else {
        positionInHouse = (360.0 - currentCusp) + longitude;
        std::cout << "Position (<cusp): (360.0 - " << currentCusp << ") + " << longitude << " = " << positionInHouse << "°" << std::endl;
    }
    
    // Convert to 0-30° scale
    double normalizedPosition = (positionInHouse / houseSize) * 30.0;
    std::cout << "Normalized position: (" << positionInHouse << " / " << houseSize << ") * 30.0 = " << normalizedPosition << "°" << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "House Calculation Analysis and Testing" << std::endl;
    std::cout << "======================================" << std::endl << std::endl;
    
    analyzeHousePositionMath();
    testNormalHousePositions();
    testHouseCrossingZeroAries();
    testHouseEdgeCases();
    
    return 0;
}