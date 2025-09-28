#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include "third_party/swisseph/swephexp.h"

struct PlanetTiming {
    std::string name;
    int planetId;
    double riseTime;
    double setTime;
    double meridianTime;
    double antiMeridianTime;
    bool riseValid;
    bool setValid;
    bool meridianValid;
    bool antiMeridianValid;
    std::string error;
};

std::string formatTime(double timeInHours) {
    if (timeInHours < 0 || timeInHours >= 24) {
        // Normalize to 0-24 range
        timeInHours = fmod(timeInHours, 24.0);
        if (timeInHours < 0) timeInHours += 24.0;
    }
    
    int hours = static_cast<int>(timeInHours);
    int minutes = static_cast<int>((timeInHours - hours) * 60);
    int seconds = static_cast<int>(((timeInHours - hours) * 60 - minutes) * 60);
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setfill('0') << std::setw(2) << minutes << ":"
       << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

// Simple atmospheric model to match Hindu Panchanga
struct AtmosphericModel {
    double pressure;
    double temperature;
};

AtmosphericModel getSeasonalAtmosphere(double julianDay, double latitude, double longitude) {
    // EXACT COPY of Hindu Panchanga's atmospheric calculation
    AtmosphericModel atmosphere;
    
    // Extract month from Julian Day
    int year, month, day, hour, minute;
    double second;
    swe_jdet_to_utc(julianDay, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);
    
    // Bangkok/Thailand tropical climate parameters (same as Hindu Panchanga)
    if (latitude >= 10.0 && latitude <= 20.0 && longitude >= 97.0 && longitude <= 106.0) {
        if (month >= 3 && month <= 5) {
            // Hot season (March-May): lower pressure, higher temperature
            atmosphere.pressure = 1010.0;
            atmosphere.temperature = 32.0;
        } else if (month >= 6 && month <= 10) {
            // Rainy season (June-October): lowest pressure, moderate temperature
            atmosphere.pressure = 1008.0;  // EXACTLY what Hindu Panchanga uses for Sept
            atmosphere.temperature = 28.0;  // EXACTLY what Hindu Panchanga uses for Sept
        } else {
            // Cool season (November-February): higher pressure, cooler temperature
            atmosphere.pressure = 1015.0;
            atmosphere.temperature = 24.0;
        }
    } else {
        // Default fallback
        atmosphere.pressure = 1013.25;
        atmosphere.temperature = 15.0;
    }
    
    return atmosphere;
}

PlanetTiming calculatePlanetTiming(int planetId, const std::string& planetName, 
                                  double julianDay, double latitude, double longitude, 
                                  double timezone) {
    PlanetTiming timing;
    timing.name = planetName;
    timing.planetId = planetId;
    timing.riseValid = false;
    timing.setValid = false;
    timing.meridianValid = false;
    timing.antiMeridianValid = false;
    
    // Use EXACT same setup as Hindu Panchanga including atmospheric model
    AtmosphericModel atmosphere = getSeasonalAtmosphere(julianDay, latitude, longitude);
    double geopos[3] = {longitude, latitude, 0.0}; // Sea level  
    double tret[10];  // Swiss Ephemeris result array
    char serr[AS_MAXCH]; // Error string
    int flags = SEFLG_SWIEPH | SEFLG_TOPOCTR; // Same flags as Drik
    int result;
    
    // Calculate rise time
    result = swe_rise_trans(julianDay, planetId, nullptr, flags, SE_CALC_RISE,
                           geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
    if (result != ERR) {
        // Use CORRECT birthfinder approach: swe_jdut1_to_utc() then apply timezone
        int year, month, day, hour, min;
        double sec;
        swe_jdut1_to_utc(tret[0], 1, &year, &month, &day, &hour, &min, &sec);
        
        // Apply timezone offset (longitude-based)
        double hour_offset = longitude / 15.0;
        int hour_offset_int = (int)hour_offset;
        double minute_offset = (hour_offset - hour_offset_int) * 60.0;
        
        hour += hour_offset_int;
        min += (int)minute_offset;
        
        // Handle minute overflow
        if (min >= 60) {
            hour += 1;
            min -= 60;
        } else if (min < 0) {
            hour -= 1;
            min += 60;
        }
        
        // Handle hour overflow/underflow
        if (hour >= 24) {
            hour -= 24;
        } else if (hour < 0) {
            hour += 24;
        }
        
        // Convert back to decimal time
        timing.riseTime = hour + min / 60.0 + sec / 3600.0;
        timing.riseValid = true;
    } else {
        timing.error += "Rise: " + std::string(serr) + "; ";
    }
    
    // Calculate set time
    result = swe_rise_trans(julianDay, planetId, nullptr, flags, SE_CALC_SET,
                           geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
    if (result != ERR) {
        // Use CORRECT birthfinder approach: swe_jdut1_to_utc() then apply timezone
        int year, month, day, hour, min;
        double sec;
        swe_jdut1_to_utc(tret[0], 1, &year, &month, &day, &hour, &min, &sec);
        
        // Apply timezone offset (longitude-based)
        double hour_offset = longitude / 15.0;
        int hour_offset_int = (int)hour_offset;
        double minute_offset = (hour_offset - hour_offset_int) * 60.0;
        
        hour += hour_offset_int;
        min += (int)minute_offset;
        
        // Handle minute overflow
        if (min >= 60) {
            hour += 1;
            min -= 60;
        } else if (min < 0) {
            hour -= 1;
            min += 60;
        }
        
        // Handle hour overflow/underflow
        if (hour >= 24) {
            hour -= 24;
        } else if (hour < 0) {
            hour += 24;
        }
        
        // Convert back to decimal time
        timing.setTime = hour + min / 60.0 + sec / 3600.0;
        timing.setValid = true;
    } else {
        timing.error += "Set: " + std::string(serr) + "; ";
    }
    
    // Calculate meridian transit (upper culmination)
    result = swe_rise_trans(julianDay, planetId, nullptr, flags, SE_CALC_MTRANSIT,
                           geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
    if (result != ERR) {
        // Use CORRECT birthfinder approach: swe_jdut1_to_utc() then apply timezone
        int year, month, day, hour, min;
        double sec;
        swe_jdut1_to_utc(tret[0], 1, &year, &month, &day, &hour, &min, &sec);
        
        // Apply timezone offset (longitude-based)
        double hour_offset = longitude / 15.0;
        int hour_offset_int = (int)hour_offset;
        double minute_offset = (hour_offset - hour_offset_int) * 60.0;
        
        hour += hour_offset_int;
        min += (int)minute_offset;
        
        // Handle minute overflow
        if (min >= 60) {
            hour += 1;
            min -= 60;
        } else if (min < 0) {
            hour -= 1;
            min += 60;
        }
        
        // Handle hour overflow/underflow
        if (hour >= 24) {
            hour -= 24;
        } else if (hour < 0) {
            hour += 24;
        }
        
        // Convert back to decimal time
        timing.meridianTime = hour + min / 60.0 + sec / 3600.0;
        timing.meridianValid = true;
    } else {
        timing.error += "Meridian: " + std::string(serr) + "; ";
    }
    
    // Calculate anti-meridian transit (lower culmination)
    result = swe_rise_trans(julianDay, planetId, nullptr, flags, SE_CALC_ITRANSIT,
                           geopos, atmosphere.pressure, atmosphere.temperature, tret, serr);
    if (result != ERR) {
        // Use CORRECT birthfinder approach: swe_jdut1_to_utc() then apply timezone
        int year, month, day, hour, min;
        double sec;
        swe_jdut1_to_utc(tret[0], 1, &year, &month, &day, &hour, &min, &sec);
        
        // Apply timezone offset (longitude-based)
        double hour_offset = longitude / 15.0;
        int hour_offset_int = (int)hour_offset;
        double minute_offset = (hour_offset - hour_offset_int) * 60.0;
        
        hour += hour_offset_int;
        min += (int)minute_offset;
        
        // Handle minute overflow
        if (min >= 60) {
            hour += 1;
            min -= 60;
        } else if (min < 0) {
            hour -= 1;
            min += 60;
        }
        
        // Handle hour overflow/underflow
        if (hour >= 24) {
            hour -= 24;
        } else if (hour < 0) {
            hour += 24;
        }
        
        // Convert back to decimal time
        timing.antiMeridianTime = hour + min / 60.0 + sec / 3600.0;
        timing.antiMeridianValid = true;
    } else {
        timing.error += "Anti-Meridian: " + std::string(serr) + "; ";
    }
    
    return timing;
}

int main() {
    std::cout << "=== COMPREHENSIVE PLANETARY RISE/SET/MERIDIAN TIMING TEST ===" << std::endl;
    std::cout << "Using Swiss Ephemeris swe_rise_trans function directly" << std::endl;
    std::cout << "=============================================================" << std::endl << std::endl;
    
    // Test parameters - Bangkok coordinates
    double latitude = 13.7563;
    double longitude = 100.5018;
    double timezone = 7.0;
    // Use the EXACT same Julian Day as Hindu Panchanga (2460581.5)
    // This corresponds to Sept 27, 2024 12:00 UTC (which becomes Sept 28 local time in Bangkok)
    double julianDay = 2460581.5;
    double pressure = 1013.25; // Standard atmospheric pressure
    double temperature = 15.0;  // Standard temperature
    
    std::cout << "Location: Bangkok, Thailand (13.7563°N, 100.5018°E)" << std::endl;
    std::cout << "Date: 2024-09-28 (JD: " << julianDay << ")" << std::endl;
    std::cout << "Timezone: +7 hours (ICT)" << std::endl;
    std::cout << "Atmospheric: " << pressure << " mbar, " << temperature << "°C" << std::endl << std::endl;
    
    // Initialize Swiss Ephemeris
    swe_set_ephe_path("/DATA/FlutterCodes/githubCodes/horoscope_cli/data");
    
    // Define planets to test
    std::vector<std::pair<int, std::string>> planets = {
        {SE_SUN, "Sun"},
        {SE_MOON, "Moon"},
        {SE_MERCURY, "Mercury"},
        {SE_VENUS, "Venus"},
        {SE_MARS, "Mars"},
        {SE_JUPITER, "Jupiter"},
        {SE_SATURN, "Saturn"},
        {SE_URANUS, "Uranus"},
        {SE_NEPTUNE, "Neptune"},
        {SE_PLUTO, "Pluto"}
    };
    
    std::vector<PlanetTiming> results;
    
    // Calculate timing for each planet
    for (const auto& planet : planets) {
        PlanetTiming timing = calculatePlanetTiming(planet.first, planet.second, 
                                                   julianDay, latitude, longitude, 
                                                   timezone);
        results.push_back(timing);
    }
    
    // Display results in tabular format
    std::cout << std::left;
    std::cout << std::setw(10) << "Planet" 
              << std::setw(12) << "Rise" 
              << std::setw(12) << "Meridian" 
              << std::setw(12) << "Set" 
              << std::setw(12) << "Anti-Mer" 
              << "Errors" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& timing : results) {
        std::cout << std::setw(10) << timing.name;
        
        if (timing.riseValid) {
            std::cout << std::setw(12) << formatTime(timing.riseTime);
        } else {
            std::cout << std::setw(12) << "---";
        }
        
        if (timing.meridianValid) {
            std::cout << std::setw(12) << formatTime(timing.meridianTime);
        } else {
            std::cout << std::setw(12) << "---";
        }
        
        if (timing.setValid) {
            std::cout << std::setw(12) << formatTime(timing.setTime);
        } else {
            std::cout << std::setw(12) << "---";
        }
        
        if (timing.antiMeridianValid) {
            std::cout << std::setw(12) << formatTime(timing.antiMeridianTime);
        } else {
            std::cout << std::setw(12) << "---";
        }
        
        if (!timing.error.empty()) {
            std::cout << timing.error;
        }
        
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    
    // Special focus on Sun and Moon
    std::cout << "=== DETAILED SUN AND MOON ANALYSIS ===" << std::endl;
    for (const auto& timing : results) {
        if (timing.name == "Sun" || timing.name == "Moon") {
            std::cout << "\n" << timing.name << ":" << std::endl;
            if (timing.riseValid) {
                std::cout << "  Rise:         " << formatTime(timing.riseTime) << std::endl;
            } else {
                std::cout << "  Rise:         FAILED" << std::endl;
            }
            if (timing.meridianValid) {
                std::cout << "  Meridian:     " << formatTime(timing.meridianTime) << std::endl;
            }
            if (timing.setValid) {
                std::cout << "  Set:          " << formatTime(timing.setTime) << std::endl;
            } else {
                std::cout << "  Set:          FAILED" << std::endl;
            }
            if (timing.antiMeridianValid) {
                std::cout << "  Anti-Mer:     " << formatTime(timing.antiMeridianTime) << std::endl;
            }
            if (!timing.error.empty()) {
                std::cout << "  Errors:       " << timing.error << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
    
    // Test different atmospheric conditions
    std::cout << "=== ATMOSPHERIC CONDITION COMPARISON ===" << std::endl;
    std::cout << "Testing Sun rise/set with different atmospheric parameters:" << std::endl;
    
    std::vector<std::pair<double, double>> atmospheres = {
        {1013.25, 15.0},   // Standard
        {1010.0, 32.0},    // Hot season Bangkok
        {1008.0, 28.0},    // Rainy season Bangkok  
        {1015.0, 24.0},    // Cool season Bangkok
        {0.0, 0.0}         // No atmospheric correction
    };
    
    std::string conditions[] = {"Standard", "Hot Season", "Rainy Season", "Cool Season", "No Atmosphere"};
    
    for (size_t i = 0; i < atmospheres.size(); i++) {
        PlanetTiming sunTiming = calculatePlanetTiming(SE_SUN, "Sun", julianDay, latitude, longitude, 
                                                      timezone);
        std::cout << std::setw(15) << conditions[i] << ": ";
        if (sunTiming.riseValid && sunTiming.setValid) {
            std::cout << "Rise " << formatTime(sunTiming.riseTime) 
                      << " | Set " << formatTime(sunTiming.setTime);
        } else {
            std::cout << "CALCULATION FAILED";
        }
        std::cout << std::endl;
    }
    
    // Test timezone effect
    std::cout << "\n=== TIMEZONE COMPARISON ===" << std::endl;
    std::cout << "Testing Sun rise/set with different timezone values:" << std::endl;
    
    std::vector<double> timezones = {0.0, 7.0, round(longitude / 15.0)};
    std::string tzLabels[] = {"UTC+0", "ICT+7", "Auto-calc"};
    
    for (size_t i = 0; i < timezones.size(); i++) {
        PlanetTiming sunTiming = calculatePlanetTiming(SE_SUN, "Sun", julianDay, latitude, longitude, 
                                                      timezones[i]);
        std::cout << std::setw(10) << tzLabels[i] << ": ";
        if (sunTiming.riseValid && sunTiming.setValid) {
            std::cout << "Rise " << formatTime(sunTiming.riseTime) 
                      << " | Set " << formatTime(sunTiming.setTime);
        } else {
            std::cout << "CALCULATION FAILED";
        }
        std::cout << std::endl;
    }
    
    // Close Swiss Ephemeris
    swe_close();
    
    std::cout << "\n=== TEST COMPLETE ===" << std::endl;
    return 0;
}