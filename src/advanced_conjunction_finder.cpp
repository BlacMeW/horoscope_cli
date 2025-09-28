#include "advanced_conjunction_finder.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "astro_types.h"
#include <swephexp.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <map>

namespace Astro {

AdvancedConjunctionFinder::AdvancedConjunctionFinder()
    : isInitialized(false) {
}

AdvancedConjunctionFinder::~AdvancedConjunctionFinder() {
}

bool AdvancedConjunctionFinder::initialize(const std::string& ephePath) {
    EphemerisManager ephMgr;
    if (!ephMgr.initialize(ephePath)) {
        lastError = ephMgr.getLastError();
        return false;
    }

    isInitialized = true;
    ephemerisPath = ephePath;
    clearCache();
    return true;
}

std::vector<ConjunctionEvent> AdvancedConjunctionFinder::findConjunctionsAdvanced(
    const BirthData& fromDate, 
    const BirthData& toDate,
    double maxOrb,
    bool useNewtonRaphson,
    bool useProgressiveTolerance) const {

    if (!isInitialized) {
        setError("Advanced conjunction finder not initialized");
        return {};
    }

    std::vector<ConjunctionEvent> conjunctions;

    if (useProgressiveTolerance) {
        // Use progressive tolerance levels for high accuracy
        conjunctions = findConjunctionsProgressive(fromDate, toDate);
    } else {
        // Use single-pass search with specified refinement method
        double startJD = fromDate.getJulianDay();
        double endJD = toDate.getJulianDay();

        std::vector<Planet> planets = {
            Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS, 
            Planet::MARS, Planet::JUPITER, Planet::SATURN, Planet::URANUS,
            Planet::NEPTUNE, Planet::PLUTO, Planet::NORTH_NODE
        };

        // Search all planet pairs
        for (size_t i = 0; i < planets.size(); i++) {
            for (size_t j = i + 1; j < planets.size(); j++) {
                auto conjunction = findConjunctionRefined(
                    planets[i], planets[j], startJD, endJD, 0.001, useNewtonRaphson);
                
                if (isValidConjunction(conjunction, maxOrb)) {
                    conjunctions.push_back(conjunction);
                }
            }
        }
    }

    // Find multi-planet conjunctions
    auto multiConjunctions = findMultiPlanetConjunctions(fromDate, toDate, 3, maxOrb);
    conjunctions.insert(conjunctions.end(), multiConjunctions.begin(), multiConjunctions.end());

    // Remove duplicates and sort
    removeDuplicates(conjunctions);
    sortByAccuracy(conjunctions);

    return conjunctions;
}

std::vector<ConjunctionEvent> AdvancedConjunctionFinder::findConjunctionsProgressive(
    const BirthData& fromDate,
    const BirthData& toDate,
    const std::vector<double>& toleranceLevels) const {

    if (!isInitialized) {
        setError("Advanced conjunction finder not initialized");
        return {};
    }

    std::vector<ConjunctionEvent> allConjunctions;

    // Progressive search with increasing accuracy
    for (double tolerance : toleranceLevels) {
        auto conjunctions = searchWithTolerance(fromDate, toDate, tolerance);
        
        // Merge results, keeping the most accurate versions
        for (const auto& newConj : conjunctions) {
            bool found = false;
            for (auto& existing : allConjunctions) {
                // Check if this is the same conjunction with better accuracy
                if (std::abs(existing.julianDay - newConj.julianDay) < 1.0 &&
                    existing.planets == newConj.planets) {
                    // Keep the more accurate one
                    if (newConj.orb < existing.orb) {
                        existing = newConj;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                allConjunctions.push_back(newConj);
            }
        }
    }

    removeDuplicates(allConjunctions);
    sortByAccuracy(allConjunctions);
    return allConjunctions;
}

std::vector<ConjunctionEvent> AdvancedConjunctionFinder::findMultiPlanetConjunctions(
    const BirthData& fromDate,
    const BirthData& toDate,
    int minPlanets,
    double maxOrb) const {

    if (!isInitialized) {
        setError("Advanced conjunction finder not initialized");
        return {};
    }

    std::vector<ConjunctionEvent> multiConjunctions;
    double startJD = fromDate.getJulianDay();
    double endJD = toDate.getJulianDay();

    // Search through time range
    for (double currentJD = startJD; currentJD <= endJD; currentJD += 0.5) {
        std::vector<Planet> allPlanets = {
            Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS,
            Planet::MARS, Planet::JUPITER, Planet::SATURN, Planet::URANUS,
            Planet::NEPTUNE, Planet::PLUTO
        };

        // Find all planets within maxOrb of each other
        std::vector<std::vector<Planet>> groups;
        
        for (Planet planet : allPlanets) {
            auto pos = getPlanetPosition(planet, currentJD);
            double longitude = pos.first;
            
            // Find planets within orb
            auto nearbyPlanets = findPlanetsInRange(longitude, maxOrb, currentJD);
            
            if (nearbyPlanets.size() >= minPlanets) {
                groups.push_back(nearbyPlanets);
            }
        }

        // Process groups to create conjunction events
        for (const auto& group : groups) {
            if (group.size() >= minPlanets) {
                ConjunctionEvent conjunction;
                conjunction.planets = group;
                conjunction.julianDay = currentJD;
                conjunction.type = (group.size() >= 4) ? ConjunctionType::GRAND : ConjunctionType::TRIPLE;
                
                // Calculate average longitude and determine sign
                double totalLongitude = 0.0;
                for (Planet planet : group) {
                    auto pos = getPlanetPosition(planet, currentJD);
                    totalLongitude += pos.first;
                }
                conjunction.longitude = totalLongitude / group.size();
                conjunction.sign = longitudeToSign(conjunction.longitude);
                
                // Calculate maximum orb within group
                double maxSeparation = 0.0;
                for (size_t i = 0; i < group.size(); i++) {
                    for (size_t j = i + 1; j < group.size(); j++) {
                        double sep = calculateSeparation(group[i], group[j], currentJD);
                        maxSeparation = std::max(maxSeparation, sep);
                    }
                }
                conjunction.orb = maxSeparation;
                
                if (conjunction.orb <= maxOrb) {
                    multiConjunctions.push_back(conjunction);
                }
            }
        }
    }

    removeDuplicates(multiConjunctions);
    return multiConjunctions;
}

ConjunctionEvent AdvancedConjunctionFinder::findConjunctionRefined(
    Planet planet1,
    Planet planet2,
    double startJD,
    double endJD,
    double tolerance,
    bool useNewtonRaphson) const {

    ConjunctionEvent conjunction;
    conjunction.planets = {planet1, planet2};
    conjunction.type = ConjunctionType::PLANETARY;

    // Initial search for approximate conjunction time
    double minSeparation = 360.0;
    double approximateJD = startJD;
    
    for (double jd = startJD; jd <= endJD; jd += 1.0) {
        double separation = calculateSeparation(planet1, planet2, jd);
        if (separation < minSeparation) {
            minSeparation = separation;
            approximateJD = jd;
        }
    }

    // Refine using selected method
    NewtonRaphsonResult refinement;
    if (useNewtonRaphson) {
        refinement = refineNewtonRaphson(planet1, planet2, approximateJD, tolerance);
        if (!refinement.converged) {
            // Fallback to bisection if Newton-Raphson fails
            refinement = refineBisection(planet1, planet2, approximateJD - 0.5, approximateJD + 0.5, tolerance);
        }
    } else {
        refinement = refineBisection(planet1, planet2, approximateJD - 0.5, approximateJD + 0.5, tolerance);
    }

    // Populate conjunction event
    conjunction.julianDay = refinement.time;
    conjunction.orb = refinement.separation;
    
    auto pos = getPlanetPosition(planet1, refinement.time);
    conjunction.longitude = pos.first;
    conjunction.sign = longitudeToSign(conjunction.longitude);
    
    return conjunction;
}

std::future<std::vector<ConjunctionEvent>> AdvancedConjunctionFinder::findConjunctionsAsync(
    const BirthData& fromDate,
    const BirthData& toDate,
    double maxOrb) const {

    return std::async(std::launch::async, [this, fromDate, toDate, maxOrb]() {
        return this->findConjunctionsAdvanced(fromDate, toDate, maxOrb, true, true);
    });
}

std::vector<ConjunctionEvent> AdvancedConjunctionFinder::findGrahaYuddha(
    const BirthData& fromDate,
    const BirthData& toDate,
    double maxOrb) const {

    if (!isInitialized) {
        setError("Advanced conjunction finder not initialized");
        return {};
    }

    // Graha Yuddha occurs when planets are extremely close (typically < 1°)
    // Use very high precision with Newton-Raphson refinement
    auto conjunctions = findConjunctionsAdvanced(fromDate, toDate, maxOrb, true, true);
    
    std::vector<ConjunctionEvent> wars;
    
    for (auto& conjunction : conjunctions) {
        // Check if this is a planetary war (very close conjunction between planets)
        if (conjunction.orb <= maxOrb && conjunction.planets.size() >= 2) {
            // Exclude Sun and Moon from planetary wars as per Vedic astrology
            bool validWar = true;
            for (Planet planet : conjunction.planets) {
                if (planet == Planet::SUN || planet == Planet::MOON) {
                    validWar = false;
                    break;
                }
            }
            
            if (validWar) {
                conjunction.isGrahaYuddha = true;
                
                // Determine winner based on planetary strengths and speeds
                if (conjunction.planets.size() >= 2) {
                    Planet winner = determineGrahaYuddhaWinner(conjunction.planets, conjunction.julianDay);
                    conjunction.grahaYuddhaWinner = winner;
                    conjunction.grahaYuddhaEffect = getGrahaYuddhaEffect(conjunction.planets, winner);
                }
                
                wars.push_back(conjunction);
            }
        }
    }
    
    return wars;
}

// Private method implementations

NewtonRaphsonResult AdvancedConjunctionFinder::searchForward(
    Planet planet1, 
    Planet planet2,
    double startJD,
    double stepSize,
    double tolerance) const {

    NewtonRaphsonResult result;
    result.method = "forward_search";
    result.converged = false;
    
    double currentJD = startJD;
    double previousSep = calculateSeparation(planet1, planet2, currentJD);
    
    for (int step = 0; step < 1000; step++) {
        currentJD += stepSize;
        double currentSep = calculateSeparation(planet1, planet2, currentJD);
        
        // Check if we passed minimum separation
        if (currentSep > previousSep) {
            // Refine the exact time
            result = refineNewtonRaphson(planet1, planet2, currentJD - stepSize, tolerance);
            break;
        }
        
        previousSep = currentSep;
    }
    
    return result;
}

NewtonRaphsonResult AdvancedConjunctionFinder::refineBisection(
    Planet planet1,
    Planet planet2,
    double startJD,
    double endJD,
    double tolerance) const {

    NewtonRaphsonResult result;
    result.method = "bisection";
    result.iterations = 0;
    result.tolerance = tolerance;
    
    double left = startJD;
    double right = endJD;
    
    while ((right - left) > tolerance && result.iterations < 50) {
        double mid = (left + right) / 2.0;
        double leftSep = calculateSeparation(planet1, planet2, left);
        double rightSep = calculateSeparation(planet1, planet2, right);
        
        if (leftSep < rightSep) {
            right = mid;
        } else {
            left = mid;
        }
        
        result.iterations++;
    }
    
    result.time = (left + right) / 2.0;
    result.separation = calculateSeparation(planet1, planet2, result.time);
    result.converged = (right - left) <= tolerance;
    
    return result;
}

NewtonRaphsonResult AdvancedConjunctionFinder::refineNewtonRaphson(
    Planet planet1,
    Planet planet2,
    double approximateJD,
    double tolerance,
    int maxIterations) const {

    NewtonRaphsonResult result;
    result.method = "newton_raphson";
    result.iterations = 0;
    result.tolerance = tolerance;
    result.time = approximateJD;
    
    for (int i = 0; i < maxIterations; i++) {
        double separation = calculateSeparation(planet1, planet2, result.time);
        double derivative = calculateSeparationDerivative(planet1, planet2, result.time);
        
        if (std::abs(derivative) < 1e-12) {
            // Derivative too small, fallback to bisection
            result.converged = false;
            return result;
        }
        
        double delta = separation / derivative;
        result.time -= delta;
        
        result.iterations++;
        
        if (std::abs(delta) < tolerance) {
            result.separation = calculateSeparation(planet1, planet2, result.time);
            result.converged = true;
            return result;
        }
    }
    
    result.separation = calculateSeparation(planet1, planet2, result.time);
    result.converged = false;
    return result;
}

double AdvancedConjunctionFinder::calculateSeparation(Planet planet1, Planet planet2, double jd) const {
    auto pos1 = getPlanetPosition(planet1, jd);
    auto pos2 = getPlanetPosition(planet2, jd);
    
    double diff = std::abs(pos1.first - pos2.first);
    if (diff > 180.0) {
        diff = 360.0 - diff;
    }
    
    return diff;
}

double AdvancedConjunctionFinder::calculateSeparationDerivative(Planet planet1, Planet planet2, double jd) const {
    const double h = 0.001; // Small step for numerical derivative
    
    double sep1 = calculateSeparation(planet1, planet2, jd - h);
    double sep2 = calculateSeparation(planet1, planet2, jd + h);
    
    return (sep2 - sep1) / (2.0 * h);
}

std::pair<double, double> AdvancedConjunctionFinder::getPlanetPosition(Planet planet, double jd) const {
    // Check cache first
    auto cacheKey = std::make_pair(planet, jd);
    auto it = positionCache.find(cacheKey);
    if (it != positionCache.end()) {
        return it->second;
    }
    
    // Calculate position using Swiss Ephemeris
    double xx[6];
    char serr[256];
    int32 iflag = SEFLG_SWIEPH | SEFLG_SPEED;
    
    int planetNum = static_cast<int>(planet);
    if (planet == Planet::NORTH_NODE) planetNum = SE_TRUE_NODE;
    if (planet == Planet::SOUTH_NODE) planetNum = SE_TRUE_NODE; // Will negate later
    
    int ret = swe_calc(jd, planetNum, iflag, xx, serr);
    if (ret < 0) {
        setError("Swiss Ephemeris calculation failed: " + std::string(serr));
        return {0.0, 0.0};
    }
    
    double longitude = xx[0];
    double latitude = xx[1];
    
    if (planet == Planet::SOUTH_NODE) {
        longitude = fmod(longitude + 180.0, 360.0);
    }
    
    // Cache the result
    positionCache[cacheKey] = {longitude, latitude};
    
    return {longitude, latitude};
}

std::vector<Planet> AdvancedConjunctionFinder::findPlanetsInRange(double longitude, double orb, double jd) const {
    std::vector<Planet> planetsInRange;
    std::vector<Planet> allPlanets = {
        Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS,
        Planet::MARS, Planet::JUPITER, Planet::SATURN, Planet::URANUS,
        Planet::NEPTUNE, Planet::PLUTO
    };
    
    for (Planet planet : allPlanets) {
        auto pos = getPlanetPosition(planet, jd);
        double planetLong = pos.first;
        
        double diff = std::abs(longitude - planetLong);
        if (diff > 180.0) {
            diff = 360.0 - diff;
        }
        
        if (diff <= orb) {
            planetsInRange.push_back(planet);
        }
    }
    
    return planetsInRange;
}

std::vector<ConjunctionEvent> AdvancedConjunctionFinder::searchWithTolerance(
    const BirthData& fromDate,
    const BirthData& toDate,
    double tolerance) const {

    std::vector<ConjunctionEvent> conjunctions;
    double startJD = fromDate.getJulianDay();
    double endJD = toDate.getJulianDay();
    
    std::vector<Planet> planets = {
        Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS,
        Planet::MARS, Planet::JUPITER, Planet::SATURN, Planet::URANUS,
        Planet::NEPTUNE, Planet::PLUTO
    };
    
    // Search all planet pairs with given tolerance
    for (size_t i = 0; i < planets.size(); i++) {
        for (size_t j = i + 1; j < planets.size(); j++) {
            auto conjunction = findConjunctionRefined(
                planets[i], planets[j], startJD, endJD, tolerance, true);
            
            if (conjunction.orb <= tolerance && conjunction.orb > 0.0) {
                conjunctions.push_back(conjunction);
            }
        }
    }
    
    return conjunctions;
}

bool AdvancedConjunctionFinder::isValidConjunction(const ConjunctionEvent& conjunction, double maxOrb) const {
    return conjunction.orb > 0.0 && conjunction.orb <= maxOrb && conjunction.julianDay > 0.0;
}

void AdvancedConjunctionFinder::removeDuplicates(std::vector<ConjunctionEvent>& conjunctions) const {
    std::sort(conjunctions.begin(), conjunctions.end(), 
        [](const ConjunctionEvent& a, const ConjunctionEvent& b) {
            return a.julianDay < b.julianDay;
        });
    
    auto it = std::unique(conjunctions.begin(), conjunctions.end(),
        [](const ConjunctionEvent& a, const ConjunctionEvent& b) {
            return std::abs(a.julianDay - b.julianDay) < 0.1 && a.planets == b.planets;
        });
    
    conjunctions.erase(it, conjunctions.end());
}

void AdvancedConjunctionFinder::sortByAccuracy(std::vector<ConjunctionEvent>& conjunctions) const {
    std::sort(conjunctions.begin(), conjunctions.end(),
        [](const ConjunctionEvent& a, const ConjunctionEvent& b) {
            return a.julianDay < b.julianDay;
        });
}

Planet AdvancedConjunctionFinder::determineGrahaYuddhaWinner(const std::vector<Planet>& planets, double jd) const {
    // In Vedic astrology, the winner is usually the one with higher longitude (ahead in the zodiac)
    // or based on planetary strengths and speeds
    
    Planet winner = planets[0];
    double highestLongitude = -1.0;
    
    for (Planet planet : planets) {
        auto pos = getPlanetPosition(planet, jd);
        double longitude = pos.first;
        
        if (longitude > highestLongitude) {
            highestLongitude = longitude;
            winner = planet;
        }
    }
    
    return winner;
}

std::string AdvancedConjunctionFinder::getGrahaYuddhaEffect(const std::vector<Planet>& planets, Planet winner) const {
    std::stringstream ss;
    ss << "Planetary war between ";
    
    for (size_t i = 0; i < planets.size(); ++i) {
        if (i > 0) {
            if (i == planets.size() - 1) {
                ss << " and ";
            } else {
                ss << ", ";
            }
        }
        ss << planetToString(planets[i]);
    }
    
    ss << ". Winner: " << planetToString(winner) << ". ";
    ss << "The winning planet gains strength while the losing planet(s) become weakened. ";
    ss << "This affects the significations ruled by these planets during this period.";
    
    return ss.str();
}

// Factory function
std::unique_ptr<AdvancedConjunctionFinder> createConjunctionFinder(bool useAdvanced) {
    if (useAdvanced) {
        return std::make_unique<AdvancedConjunctionFinder>();
    }
    // Could return basic finder here if needed
    return std::make_unique<AdvancedConjunctionFinder>();
}

} // namespace Astro