#pragma once

#include "astro_types.h"
#include <string>
#include <vector>

namespace Astro {

// KP Sub Lord Levels
enum class KPLevel {
    SIGN = 1,      // Rashi Level
    STAR = 2,      // Nakshatra Level  
    SUB = 3,       // Sub Level
    SUB_SUB = 4,   // Sub-Sub Level
    SUB_SUB_SUB = 5 // Sub-Sub-Sub Level
};

// Nakshatra (Star) information
struct Nakshatra {
    int number;         // 1-27
    std::string name;
    Planet lord;        // Ruling planet
    double startDegree; // Starting degree in zodiac
    double endDegree;   // Ending degree in zodiac
};

// KP Sub Division
struct KPSubDivision {
    Planet lord;        // Sub lord
    double startDegree; // Starting degree
    double endDegree;   // Ending degree
    int level;          // Sub level (1-5)
};

// Complete KP Position with 5 levels
struct KPPosition {
    // Level 1: Sign
    ZodiacSign sign;
    Planet signLord;
    
    // Level 2: Nakshatra (Star)
    Nakshatra nakshatra;
    
    // Level 3: Sub
    Planet subLord;
    
    // Level 4: Sub-Sub
    Planet subSubLord;
    
    // Level 5: Sub-Sub-Sub
    Planet subSubSubLord;
    
    // Original longitude
    double longitude;
    
    // Formatted position string
    std::string getFormattedPosition() const;
    std::string getKPNotation() const;
};

// KP Transition information
struct KPTransition {
    double julianDay;
    Planet planet;
    KPLevel level;
    Planet fromLord;
    Planet toLord;
    std::string description;
    
    std::string getDateString() const;
    std::string getTransitionDescription() const;
};

// Main KP System Calculator
class KPSystem {
private:
    std::vector<Nakshatra> nakshatras;
    std::vector<std::vector<KPSubDivision>> subDivisions;
    bool isInitialized;
    mutable std::string lastError;
    
    void initializeNakshatras();
    void initializeSubDivisions();
    
    // Calculate sub divisions for a nakshatra
    std::vector<KPSubDivision> calculateSubDivisions(const Nakshatra& nakshatra, int level) const;
    
    // Find nakshatra for given longitude
    Nakshatra findNakshatra(double longitude) const;
    
    // Find sub lord at specific level
    Planet findSubLord(double longitude, const Nakshatra& nakshatra, int level) const;
    
    // Calculate planet longitude for Julian Day (helper function)
    double calculatePlanetLongitudeForJD(double julianDay, Planet planet) const;

public:
    KPSystem();
    ~KPSystem();
    
    // Initialize the KP system
    bool initialize();
    
    // Calculate complete KP position for a longitude
    KPPosition calculateKPPosition(double longitude) const;
    
    // Calculate KP positions for all planets
    std::vector<KPPosition> calculateAllKPPositions(const std::vector<PlanetPosition>& planets) const;
    
    // Generate KP table for planets
    std::string generateKPTable(const std::vector<PlanetPosition>& planets) const;
    std::string generateKPTableCSV(const std::vector<PlanetPosition>& planets) const;
    std::string generateKPTableJSON(const std::vector<PlanetPosition>& planets) const;
    
    // Find transitions in date range
    std::vector<KPTransition> findTransitions(const BirthData& fromDate, const BirthData& toDate, 
                                             Planet planet, KPLevel level) const;
    std::vector<KPTransition> findTransitions(const std::string& fromDate, const std::string& toDate,
                                             Planet planet, KPLevel level) const;
    
    // Generate transition table
    std::string generateTransitionTable(const std::vector<KPTransition>& transitions) const;
    std::string generateTransitionTableCSV(const std::vector<KPTransition>& transitions) const;
    std::string generateTransitionTableJSON(const std::vector<KPTransition>& transitions) const;
    
    // Utility functions
    std::vector<Nakshatra> getNakshatras() const { return nakshatras; }
    std::string getNakshatraName(int number) const;
    Planet getNakshatraLord(int number) const;
    
    // Error handling
    std::string getLastError() const { return lastError; }
    bool isSystemInitialized() const { return isInitialized; }
};

// Utility functions for KP calculations
double normalizeKPLongitude(double longitude);
std::string formatKPDegree(double degree);
std::string kpLevelToString(KPLevel level);
KPLevel stringToKPLevel(const std::string& levelStr);

} // namespace Astro
