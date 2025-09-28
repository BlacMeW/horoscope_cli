#include <iostream>
#include <iomanip>
#include "kp_system.h"
#include "astro_types.h"

int main() {
    std::cout << "=== SIMPLE KP SYSTEM TEST ===" << std::endl;
    
    // Test KP System initialization
    Astro::KPSystem kpSystem;
    std::cout << "1. Creating KP System..." << std::endl;
    
    if (!kpSystem.initialize()) {
        std::cerr << "Failed to initialize KP system: " << kpSystem.getLastError() << std::endl;
        return 1;
    }
    std::cout << "2. KP System initialized successfully!" << std::endl;
    
    // Test simple longitude calculation
    double testLongitude = 256.915661; // Sun position from previous test
    std::cout << "3. Testing longitude: " << std::fixed << std::setprecision(6) << testLongitude << "°" << std::endl;
    
    try {
        // Test zodiac sign calculation first
        Astro::ZodiacSign sign = Astro::longitudeToSign(testLongitude);
        std::cout << "4. Sign: " << Astro::zodiacSignToString(sign) << std::endl;
        
        // Test sign lord
        Astro::Planet signLord = Astro::getSignLord(sign);
        std::cout << "5. Sign Lord: " << Astro::planetToString(signLord) << std::endl;
        
        // Now test KP position calculation
        std::cout << "6. Calculating KP position..." << std::endl;
        Astro::KPPosition kpPos = kpSystem.calculateKPPosition(testLongitude);
        
        std::cout << "7. KP Position calculated successfully!" << std::endl;
        std::cout << "   Sign: " << Astro::zodiacSignToString(kpPos.sign) << std::endl;
        std::cout << "   Nakshatra: " << kpPos.nakshatra.name << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught!" << std::endl;
        return 1;
    }
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}