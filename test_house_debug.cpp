#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

// Fix the house finding algorithm
double normalizeAngle(double angle) {
    return angle - floor(angle / 360.0) * 360.0;
}

struct HouseCusps {
    double cusps[13];       // Houses 1-12 (index 0 unused)
    
    int getHouseForLongitude(double longitude) const {
        longitude = normalizeAngle(longitude);
        
        // Iterate through houses to find which one contains the longitude
        for (int house = 1; house <= 12; house++) {
            double currentCusp = normalizeAngle(cusps[house]);
            double nextCusp = normalizeAngle(cusps[house == 12 ? 1 : house + 1]);
            
            bool inHouse = false;
            
            if (nextCusp > currentCusp) {
                // Normal case: cusp doesn't cross 0°
                inHouse = (longitude >= currentCusp && longitude < nextCusp);
            } else {
                // Cusp crosses 0° Aries
                inHouse = (longitude >= currentCusp || longitude < nextCusp);
            }
            
            if (inHouse) {
                return house;
            }
        }
        
        return 1; // Default fallback
    }
};

void testCompleteHouseSystem() {
    std::cout << "=== Testing Complete House System ===" << std::endl;
    
    HouseCusps cusps;
    // Set up realistic house cusps (each house roughly 30°)
    cusps.cusps[1] = 0.0;    // ASC
    cusps.cusps[2] = 30.0;
    cusps.cusps[3] = 60.0;   // IC region
    cusps.cusps[4] = 90.0;
    cusps.cusps[5] = 120.0;
    cusps.cusps[6] = 150.0;
    cusps.cusps[7] = 180.0;  // DESC
    cusps.cusps[8] = 210.0;
    cusps.cusps[9] = 240.0;  // MC region
    cusps.cusps[10] = 270.0;
    cusps.cusps[11] = 300.0;
    cusps.cusps[12] = 330.0;
    
    std::cout << "House Cusps:" << std::endl;
    for (int h = 1; h <= 12; h++) {
        std::cout << "  House " << h << ": " << cusps.cusps[h] << "°" << std::endl;
    }
    std::cout << std::endl;
    
    // Test various longitudes
    std::vector<std::pair<double, int>> tests = {
        {0.0, 1}, {15.0, 1}, {29.9, 1},      // House 1
        {30.0, 2}, {45.0, 2}, {59.9, 2},    // House 2  
        {120.0, 5}, {135.0, 5}, {149.9, 5}, // House 5
        {180.0, 7}, {195.0, 7}, {209.9, 7}, // House 7
        {330.0, 12}, {345.0, 12}, {359.9, 12} // House 12
    };
    
    std::cout << "Testing longitude -> house assignments:" << std::endl;
    for (auto& test : tests) {
        double longitude = test.first;
        int expectedHouse = test.second;
        int actualHouse = cusps.getHouseForLongitude(longitude);
        
        std::cout << "Longitude " << std::fixed << std::setprecision(1) << longitude 
                  << "° -> Expected House " << expectedHouse 
                  << ", Got House " << actualHouse;
        
        if (actualHouse != expectedHouse) {
            std::cout << " *** ERROR ***";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void testZeroCrossingHouse() {
    std::cout << "=== Testing Zero-Crossing House (House 12->1) ===" << std::endl;
    
    HouseCusps cusps;
    // Set up a house that crosses 0° Aries
    cusps.cusps[12] = 330.0;  // House 12 starts at 330°
    cusps.cusps[1] = 30.0;    // House 1 starts at 30° (so house 12 goes 330°->30°)
    cusps.cusps[2] = 60.0;    // House 2 starts at 60°
    
    std::vector<std::pair<double, int>> tests = {
        {330.0, 12}, {340.0, 12}, {350.0, 12}, // Should be house 12
        {0.0, 12}, {10.0, 12}, {20.0, 12}, {29.9, 12}, // Also should be house 12
        {30.0, 1}, {45.0, 1}, {59.9, 1}  // Should be house 1
    };
    
    std::cout << "House 12 spans: 330° to 30° (crossing 0°)" << std::endl;
    std::cout << "House 1 spans: 30° to 60°" << std::endl;
    std::cout << std::endl;
    
    for (auto& test : tests) {
        double longitude = test.first;
        int expectedHouse = test.second;
        int actualHouse = cusps.getHouseForLongitude(longitude);
        
        std::cout << "Longitude " << std::fixed << std::setprecision(1) << longitude 
                  << "° -> Expected House " << expectedHouse 
                  << ", Got House " << actualHouse;
        
        if (actualHouse != expectedHouse) {
            std::cout << " *** ERROR ***";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "House Calculation Algorithm Verification" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    testCompleteHouseSystem();
    testZeroCrossingHouse();
    
    return 0;
}