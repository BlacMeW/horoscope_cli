#pragma once

#include "astro_types.h"
#include <string>
#include <vector>

namespace Astro {

// Conjunction types
enum class ConjunctionType {
    PLANETARY,      // Planet-Planet conjunction
    SOLAR,          // Planet-Sun conjunction (superior/inferior)
    LUNAR,          // Planet-Moon conjunction (occultation possible)
    NODAL,          // Planet-Node conjunction
    TRIPLE,         // Three planets in conjunction
    GRAND           // Multiple planets in conjunction
};

// Conjunction event structure
struct ConjunctionEvent {
    ConjunctionType type;
    std::vector<Planet> planets;    // Planets involved in conjunction
    double julianDay;               // Julian day of exact conjunction
    double longitude;               // Ecliptic longitude of conjunction
    ZodiacSign sign;                // Zodiac sign of conjunction
    double orb;                     // Maximum separation in degrees
    double duration;                // Duration of conjunction (orb < 3°) in days
    bool isApplying;                // True if planets are approaching
    std::string house;              // House location if birth chart provided
    bool isGrahaYuddha;             // True if this is a planetary war
    Planet grahaYuddhaWinner;       // Winner in planetary war (if applicable)
    std::string grahaYuddhaEffect;  // Effect description for planetary war

    // Get formatted date string
    std::string getDateString() const;
    std::string getTypeString() const;
    std::string getDescription() const;
    std::string getPlanetsString() const;
};

class ConjunctionCalculator {
public:
    ConjunctionCalculator();
    ~ConjunctionCalculator();

    // Initialize with ephemeris data
    bool initialize(const std::string& ephePath = "");

    // Find conjunctions in a given time range
    std::vector<ConjunctionEvent> findConjunctions(const BirthData& fromDate, const BirthData& toDate,
                                                  double maxOrb = 3.0) const;

    // Find conjunctions by date strings (convenience method)
    std::vector<ConjunctionEvent> findConjunctions(const std::string& fromDate, const std::string& toDate,
                                                  double maxOrb = 3.0) const;

    // Find next conjunction after given date
    ConjunctionEvent findNextConjunction(const BirthData& afterDate, double maxOrb = 3.0) const;

    // Find conjunctions for specific planets
    std::vector<ConjunctionEvent> findPlanetConjunctions(Planet planet1, Planet planet2,
                                                        const BirthData& fromDate, const BirthData& toDate,
                                                        double maxOrb = 3.0) const;

    // Find conjunctions near birth date
    std::vector<ConjunctionEvent> findConjunctionsNearBirth(const BirthData& birthData,
                                                           int daysBefore = 30, int daysAfter = 30,
                                                           double maxOrb = 3.0) const;

    // Find major conjunctions (outer planets)
    std::vector<ConjunctionEvent> findMajorConjunctions(const BirthData& fromDate, const BirthData& toDate) const;

    // Find rare conjunctions (multiple planets)
    std::vector<ConjunctionEvent> findRareConjunctions(const BirthData& fromDate, const BirthData& toDate) const;

    // Check for conjunctions on specific date
    std::vector<ConjunctionEvent> checkConjunctionsOnDate(const BirthData& date, double maxOrb = 3.0) const;

    // Find conjunctions with latitude range filtering
    std::vector<ConjunctionEvent> findConjunctionsWithLatitudeRange(const BirthData& fromDate, const BirthData& toDate,
                                                                   double maxOrb = 3.0, double minLatitude = -90.0,
                                                                   double maxLatitude = 90.0) const;

    // Find Graha Yuddha (Planetary Wars) - very close conjunctions with specific conditions
    std::vector<ConjunctionEvent> findGrahaYuddha(const BirthData& fromDate, const BirthData& toDate,
                                                  double maxOrb = 1.0) const;

    // Check if conjunction qualifies as Graha Yuddha
    bool isGrahaYuddha(const ConjunctionEvent& conjunction) const;

    // Get winner planet in Graha Yuddha based on Vedic rules
    Planet getGrahaYuddhaWinner(Planet planet1, Planet planet2, double julianDay) const;

    // Generate formatted conjunction report
    std::string generateConjunctionReport(const std::vector<ConjunctionEvent>& conjunctions) const;

    // Generate Graha Yuddha report with winners and effects
    std::string generateGrahaYuddhaReport(const std::vector<ConjunctionEvent>& wars) const;

    // Print conjunction event to console
    void printConjunctionEvent(const ConjunctionEvent& conjunction) const;

    // Get error message
    std::string getLastError() const { return lastError; }

    // Configuration
    void setMaximumOrb(double orb) { maximumOrb = orb; }
    void setIncludeMinorPlanets(bool include) { includeMinorPlanets = include; }
    void setIncludeNodes(bool include) { includeNodes = include; }

private:
    bool isInitialized;
    mutable std::string lastError;
    std::string ephemerisPath;
    double maximumOrb;
    bool includeMinorPlanets;
    bool includeNodes;

    // Helper functions
    std::vector<Planet> getCalculationPlanets() const;
    ConjunctionEvent calculateConjunction(Planet planet1, Planet planet2, double julianDay) const;
    ConjunctionType determineConjunctionType(const std::vector<Planet>& planets) const;

    // Check if planets are in conjunction at given time
    bool areInConjunction(Planet planet1, Planet planet2, double julianDay, double maxOrb) const;

    // Find exact conjunction time between two planets
    double findExactConjunctionTime(Planet planet1, Planet planet2, double startJD, double endJD) const;

    // Calculate conjunction orb
    double calculateConjunctionOrb(Planet planet1, Planet planet2, double julianDay) const;

    // Get planet positions at given time
    std::vector<PlanetPosition> getPlanetPositions(double julianDay) const;

    // Check for triple/grand conjunctions
    std::vector<ConjunctionEvent> findMultiplePlanetConjunctions(double julianDay, double maxOrb) const;

    // Format conjunction significance
    std::string getConjunctionSignificance(const ConjunctionEvent& conjunction) const;

    // Sort conjunctions by date
    void sortConjunctionsByDate(std::vector<ConjunctionEvent>& conjunctions) const;

    // Helper methods for Graha Yuddha
    Planet getStrongerPlanetByHierarchy(Planet planet1, Planet planet2) const;
    std::string getGrahaYuddhaEffectDescription(Planet winner, Planet planet1, Planet planet2) const;
};

} // namespace Astro
