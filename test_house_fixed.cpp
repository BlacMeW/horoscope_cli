#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

// Test the fixed house calculation algorithm
double normalizeAngle(double angle) {
    return angle - floor(angle / 360.0) * 360.0;
}

struct HouseCusps {
    double cusps[13];       // Houses 1-12 (index 0 unused)
    
    // Use the FIXED algorithm from astro_types.cpp
    int getHouseForLongitude(double longitude) const {
        // Normalize longitude to 0-360 range
        longitude = normalizeAngle(longitude);

        // Check each house to see if the longitude falls within its bounds
        for (int house = 1; house <= 12; house++) {
            double currentCusp = normalizeAngle(cusps[house]);
            double nextCusp = normalizeAngle(cusps[house == 12 ? 1 : house + 1]);

            bool inThisHouse = false;
            
            if (nextCusp > currentCusp) {
                // Normal case: house doesn't cross 0° Aries
                // House spans from currentCusp to nextCusp
                inThisHouse = (longitude >= currentCusp && longitude < nextCusp);
            } else {
                // House crosses 0° Aries
                // House spans from currentCusp to 360° AND from 0° to nextCusp
                inThisHouse = (longitude >= currentCusp) || (longitude < nextCusp);
            }

            if (inThisHouse) {
                return house;
            }
        }

        return 1; // Default to first house if no match found
    }
};

void testFixedZeroCrossing() {
    std::cout << "=== Testing FIXED Zero-Crossing Algorithm ===" << std::endl;
    
    HouseCusps cusps;
    // Initialize all cusps to 0 first
    for (int i = 0; i <= 12; i++) cusps.cusps[i] = 0.0;
    
    // Set up a complete house system with house 12 crossing 0°
    cusps.cusps[1] = 30.0;    // House 1 starts at 30°
    cusps.cusps[2] = 60.0;    // House 2 starts at 60°
    cusps.cusps[3] = 90.0;    // House 3 starts at 90°
    cusps.cusps[4] = 120.0;   // House 4 starts at 120°
    cusps.cusps[5] = 150.0;   // House 5 starts at 150°
    cusps.cusps[6] = 180.0;   // House 6 starts at 180°
    cusps.cusps[7] = 210.0;   // House 7 starts at 210°
    cusps.cusps[8] = 240.0;   // House 8 starts at 240°
    cusps.cusps[9] = 270.0;   // House 9 starts at 270°
    cusps.cusps[10] = 300.0;  // House 10 starts at 300°
    cusps.cusps[11] = 330.0;  // House 11 starts at 330°
    cusps.cusps[12] = 350.0;  // House 12 starts at 350°
    
    std::vector<std::pair<double, int>> tests = {
        // Test house 11 (330° to 350°)
        {330.0, 11}, {335.0, 11}, {340.0, 11}, {349.9, 11},
        
        // Test house 12 (350° to 30°, crossing 0°)
        {350.0, 12}, {355.0, 12}, {359.9, 12},
        {0.0, 12}, {10.0, 12}, {20.0, 12}, {29.9, 12},
        
        // Test house 1 (30° to 60°)
        {30.0, 1}, {40.0, 1}, {50.0, 1}, {59.9, 1},
        
        // Test house 2 (60° to 90°)  
        {60.0, 2}, {70.0, 2}, {80.0, 2}, {89.9, 2},
        
        // Test house 10 (300° to 330°)
        {300.0, 10}, {310.0, 10}, {320.0, 10}, {329.9, 10}
    };
    
    std::cout << "House System:" << std::endl;
    std::cout << "  House 10: 300° to 330°" << std::endl;
    std::cout << "  House 11: 330° to 350°" << std::endl;
    std::cout << "  House 12: 350° to 30° (crosses 0°)" << std::endl;
    std::cout << "  House 1:  30° to 60°" << std::endl;
    std::cout << "  House 2:  60° to 90°" << std::endl;
    std::cout << std::endl;
    
    int errors = 0;
    for (auto& test : tests) {
        double longitude = test.first;
        int expectedHouse = test.second;
        int actualHouse = cusps.getHouseForLongitude(longitude);
        
        std::cout << "Longitude " << std::fixed << std::setprecision(1) << longitude 
                  << "° -> Expected House " << expectedHouse 
                  << ", Got House " << actualHouse;
        
        if (actualHouse != expectedHouse) {
            std::cout << " *** ERROR ***";
            errors++;
        } else {
            std::cout << " ✓";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Result: " << (errors == 0 ? "ALL TESTS PASSED!" : 
                              std::to_string(errors) + " ERRORS FOUND") << std::endl;
    std::cout << std::endl;
}

void testRegularHouses() {
    std::cout << "=== Testing Regular House Calculations ===" << std::endl;
    
    HouseCusps cusps;
    // Set up regular 30° houses (no zero crossing)
    for (int i = 1; i <= 12; i++) {
        cusps.cusps[i] = (i - 1) * 30.0; // Houses start at 0°, 30°, 60°, etc.
    }
    
    std::vector<std::pair<double, int>> tests = {
        {0.0, 1}, {15.0, 1}, {29.9, 1},      // House 1: 0° to 30°
        {30.0, 2}, {45.0, 2}, {59.9, 2},    // House 2: 30° to 60°
        {120.0, 5}, {135.0, 5}, {149.9, 5}, // House 5: 120° to 150°
        {330.0, 12}, {345.0, 12}, {359.9, 12} // House 12: 330° to 360°
    };
    
    int errors = 0;
    for (auto& test : tests) {
        double longitude = test.first;
        int expectedHouse = test.second;
        int actualHouse = cusps.getHouseForLongitude(longitude);
        
        std::cout << "Longitude " << std::fixed << std::setprecision(1) << longitude 
                  << "° -> Expected House " << expectedHouse 
                  << ", Got House " << actualHouse;
        
        if (actualHouse != expectedHouse) {
            std::cout << " *** ERROR ***";
            errors++;
        } else {
            std::cout << " ✓";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Result: " << (errors == 0 ? "ALL TESTS PASSED!" : 
                              std::to_string(errors) + " ERRORS FOUND") << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "FIXED House Calculation Algorithm Verification" << std::endl;
    std::cout << "==============================================" << std::endl << std::endl;
    
    testRegularHouses();
    testFixedZeroCrossing();
    
    return 0;
}