#include "conjunction_calculator.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include <swephexp.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace Astro {

ConjunctionCalculator::ConjunctionCalculator()
    : isInitialized(false), maximumOrb(3.0), includeMinorPlanets(false), includeNodes(true) {
}

ConjunctionCalculator::~ConjunctionCalculator() {
}

bool ConjunctionCalculator::initialize(const std::string& ephemerisPath) {
    EphemerisManager ephMgr;
    if (!ephMgr.initialize(ephemerisPath)) {
        lastError = ephMgr.getLastError();
        return false;
    }

    isInitialized = true;
    this->ephemerisPath = ephemerisPath;
    return true;
}

std::vector<ConjunctionEvent> ConjunctionCalculator::findConjunctions(const BirthData& fromDate, const BirthData& toDate,
                                                                     double maxOrb) const {
    if (!isInitialized) {
        lastError = "Conjunction calculator not initialized";
        return {};
    }

    std::vector<ConjunctionEvent> conjunctions;
    double startJD = fromDate.getJulianDay();
    double endJD = toDate.getJulianDay();

    std::vector<Planet> planets = getCalculationPlanets();

    // Search through time range in daily steps
    for (double currentJD = startJD; currentJD <= endJD; currentJD += 1.0) {
        // Check all planet pairs for conjunctions
        for (size_t i = 0; i < planets.size(); i++) {
            for (size_t j = i + 1; j < planets.size(); j++) {
                if (areInConjunction(planets[i], planets[j], currentJD, maxOrb)) {
                    // Find exact conjunction time
                    double exactJD = findExactConjunctionTime(planets[i], planets[j],
                                                            currentJD - 0.5, currentJD + 0.5);

                    // Check if we already found this conjunction
                    bool duplicate = false;
                    for (const auto& existing : conjunctions) {
                        if (std::abs(existing.julianDay - exactJD) < 0.5 &&
                            ((existing.planets[0] == planets[i] && existing.planets[1] == planets[j]) ||
                             (existing.planets[0] == planets[j] && existing.planets[1] == planets[i]))) {
                            duplicate = true;
                            break;
                        }
                    }

                    if (!duplicate) {
                        ConjunctionEvent conjunction = calculateConjunction(planets[i], planets[j], exactJD);
                        if (conjunction.orb <= maxOrb) {
                            conjunctions.push_back(conjunction);
                        }
                    }
                }
            }
        }

        // Check for multiple planet conjunctions (3 or more)
        auto multiConjunctions = findMultiplePlanetConjunctions(currentJD, maxOrb);
        for (const auto& conjunction : multiConjunctions) {
            conjunctions.push_back(conjunction);
        }
    }

    // Sort by date and remove duplicates
    sortConjunctionsByDate(conjunctions);

    return conjunctions;
}

std::vector<ConjunctionEvent> ConjunctionCalculator::findConjunctions(const std::string& fromDate, const std::string& toDate,
                                                                      double maxOrb) const {
    // Convert string dates to BirthData
    BirthData fromBirth, toBirth;

    // Parse date strings (assumes YYYY-MM-DD format)
    std::sscanf(fromDate.c_str(), "%d-%d-%d", &fromBirth.year, &fromBirth.month, &fromBirth.day);
    std::sscanf(toDate.c_str(), "%d-%d-%d", &toBirth.year, &toBirth.month, &toBirth.day);

    // Set default time values
    fromBirth.hour = 0;
    fromBirth.minute = 0;
    fromBirth.second = 0;
    fromBirth.timezone = 0.0;
    fromBirth.latitude = 0.0;
    fromBirth.longitude = 0.0;

    toBirth.hour = 23;
    toBirth.minute = 59;
    toBirth.second = 59;
    toBirth.timezone = 0.0;
    toBirth.latitude = 0.0;
    toBirth.longitude = 0.0;

    return findConjunctions(fromBirth, toBirth, maxOrb);
}

ConjunctionEvent ConjunctionCalculator::findNextConjunction(const BirthData& afterDate, double maxOrb) const {
    if (!isInitialized) {
        lastError = "Conjunction calculator not initialized";
        return {};
    }

    double startJD = afterDate.getJulianDay();
    std::vector<Planet> planets = getCalculationPlanets();

    // Search forward from the given date
    for (double currentJD = startJD; currentJD <= startJD + 365 * 2; currentJD += 1.0) {
        for (size_t i = 0; i < planets.size(); i++) {
            for (size_t j = i + 1; j < planets.size(); j++) {
                if (areInConjunction(planets[i], planets[j], currentJD, maxOrb)) {
                    double exactJD = findExactConjunctionTime(planets[i], planets[j],
                                                            currentJD - 0.5, currentJD + 0.5);
                    return calculateConjunction(planets[i], planets[j], exactJD);
                }
            }
        }
    }

    lastError = "No conjunction found within 2 years";
    return {};
}

std::vector<ConjunctionEvent> ConjunctionCalculator::findMajorConjunctions(const BirthData& fromDate, const BirthData& toDate) const {
    // Major conjunctions involve outer planets (Jupiter, Saturn, Uranus, Neptune, Pluto)
    std::vector<Planet> majorPlanets = {Planet::JUPITER, Planet::SATURN, Planet::URANUS, Planet::NEPTUNE, Planet::PLUTO};

    std::vector<ConjunctionEvent> conjunctions;
    double startJD = fromDate.getJulianDay();
    double endJD = toDate.getJulianDay();

    for (double currentJD = startJD; currentJD <= endJD; currentJD += 7.0) { // Weekly steps for major conjunctions
        for (size_t i = 0; i < majorPlanets.size(); i++) {
            for (size_t j = i + 1; j < majorPlanets.size(); j++) {
                if (areInConjunction(majorPlanets[i], majorPlanets[j], currentJD, 5.0)) { // Wider orb for major conjunctions
                    double exactJD = findExactConjunctionTime(majorPlanets[i], majorPlanets[j],
                                                            currentJD - 3.5, currentJD + 3.5);

                    ConjunctionEvent conjunction = calculateConjunction(majorPlanets[i], majorPlanets[j], exactJD);
                    if (conjunction.orb <= 5.0) {
                        conjunctions.push_back(conjunction);
                    }
                }
            }
        }
    }

    sortConjunctionsByDate(conjunctions);
    return conjunctions;
}

std::string ConjunctionCalculator::generateConjunctionReport(const std::vector<ConjunctionEvent>& conjunctions) const {
    std::stringstream ss;

    ss << "\n=== PLANETARY CONJUNCTION REPORT ===\n\n";
    ss << "Total conjunctions found: " << conjunctions.size() << "\n\n";

    if (conjunctions.empty()) {
        ss << "No conjunctions found in the specified time range.\n";
        return ss.str();
    }

    // Count by type
    int planetary = 0, solar = 0, lunar = 0, nodal = 0, triple = 0, grand = 0;
    for (const auto& conj : conjunctions) {
        switch (conj.type) {
            case ConjunctionType::PLANETARY: planetary++; break;
            case ConjunctionType::SOLAR: solar++; break;
            case ConjunctionType::LUNAR: lunar++; break;
            case ConjunctionType::NODAL: nodal++; break;
            case ConjunctionType::TRIPLE: triple++; break;
            case ConjunctionType::GRAND: grand++; break;
        }
    }

    ss << "Conjunction Summary:\n";
    ss << "───────────────────\n";
    if (planetary > 0) ss << "Planet-Planet Conjunctions: " << planetary << "\n";
    if (solar > 0) ss << "Planet-Sun Conjunctions: " << solar << "\n";
    if (lunar > 0) ss << "Planet-Moon Conjunctions: " << lunar << "\n";
    if (nodal > 0) ss << "Planet-Node Conjunctions: " << nodal << "\n";
    if (triple > 0) ss << "Triple Conjunctions: " << triple << "\n";
    if (grand > 0) ss << "Grand Conjunctions: " << grand << "\n";

    ss << "\nDetailed Conjunction List:\n";
    ss << "──────────────────────────\n";

    for (const auto& conjunction : conjunctions) {
        ss << conjunction.getDateString() << " - " << conjunction.getPlanetsString()
           << " in " << zodiacSignToString(conjunction.sign);

        ss << " (Orb: " << std::fixed << std::setprecision(2) << conjunction.orb << "°)";

        if (conjunction.isApplying) {
            ss << " [Applying]";
        } else {
            ss << " [Separating]";
        }

        ss << "\n";
        ss << "  " << conjunction.getDescription() << "\n";
        ss << "  Significance: " << getConjunctionSignificance(conjunction) << "\n\n";
    }

    return ss.str();
}

// Helper function implementations

std::vector<Planet> ConjunctionCalculator::getCalculationPlanets() const {
    std::vector<Planet> planets = {
        Planet::SUN, Planet::MOON, Planet::MERCURY, Planet::VENUS, Planet::MARS,
        Planet::JUPITER, Planet::SATURN, Planet::URANUS, Planet::NEPTUNE, Planet::PLUTO
    };

    if (includeNodes) {
        planets.push_back(Planet::NORTH_NODE);
    }

    if (includeMinorPlanets) {
        planets.push_back(Planet::CHIRON);
    }

    return planets;
}

bool ConjunctionCalculator::areInConjunction(Planet planet1, Planet planet2, double julianDay, double maxOrb) const {
    double pos1[6], pos2[6];
    char serr[256];

    int sweBody1 = static_cast<int>(planet1);
    int sweBody2 = static_cast<int>(planet2);

    // Handle special cases for nodes
    if (planet1 == Planet::NORTH_NODE) sweBody1 = SE_MEAN_NODE;
    if (planet2 == Planet::NORTH_NODE) sweBody2 = SE_MEAN_NODE;

    int result1 = swe_calc(julianDay, sweBody1, SEFLG_SWIEPH, pos1, serr);
    int result2 = swe_calc(julianDay, sweBody2, SEFLG_SWIEPH, pos2, serr);

    if (result1 < 0 || result2 < 0) {
        return false;
    }

    double diff = std::abs(pos1[0] - pos2[0]);
    if (diff > 180.0) diff = 360.0 - diff;

    return diff <= maxOrb;
}

double ConjunctionCalculator::findExactConjunctionTime(Planet planet1, Planet planet2, double startJD, double endJD) const {
    double bestJD = startJD;
    double minSeparation = 360.0;

    // Use binary search to find exact conjunction time
    for (int iterations = 0; iterations < 20; iterations++) {
        double testJD = (startJD + endJD) / 2.0;

        double pos1[6], pos2[6];
        char serr[256];

        int sweBody1 = static_cast<int>(planet1);
        int sweBody2 = static_cast<int>(planet2);

        if (planet1 == Planet::NORTH_NODE) sweBody1 = SE_MEAN_NODE;
        if (planet2 == Planet::NORTH_NODE) sweBody2 = SE_MEAN_NODE;

        swe_calc(testJD, sweBody1, SEFLG_SWIEPH, pos1, serr);
        swe_calc(testJD, sweBody2, SEFLG_SWIEPH, pos2, serr);

        double separation = std::abs(pos1[0] - pos2[0]);
        if (separation > 180.0) separation = 360.0 - separation;

        if (separation < minSeparation) {
            minSeparation = separation;
            bestJD = testJD;
        }

        // Determine which half to search next
        double pos1a[6], pos2a[6], pos1b[6], pos2b[6];
        swe_calc(startJD, sweBody1, SEFLG_SWIEPH, pos1a, serr);
        swe_calc(startJD, sweBody2, SEFLG_SWIEPH, pos2a, serr);
        swe_calc(endJD, sweBody1, SEFLG_SWIEPH, pos1b, serr);
        swe_calc(endJD, sweBody2, SEFLG_SWIEPH, pos2b, serr);

        double sepA = std::abs(pos1a[0] - pos2a[0]);
        if (sepA > 180.0) sepA = 360.0 - sepA;
        double sepB = std::abs(pos1b[0] - pos2b[0]);
        if (sepB > 180.0) sepB = 360.0 - sepB;

        if (sepA < sepB) {
            endJD = testJD;
        } else {
            startJD = testJD;
        }

        if (std::abs(endJD - startJD) < 0.0001) break; // Sufficient precision
    }

    return bestJD;
}

ConjunctionEvent ConjunctionCalculator::calculateConjunction(Planet planet1, Planet planet2, double julianDay) const {
    ConjunctionEvent conjunction;
    conjunction.planets = {planet1, planet2};
    conjunction.julianDay = julianDay;
    conjunction.type = determineConjunctionType({planet1, planet2});

    double pos1[6], pos2[6];
    char serr[256];

    int sweBody1 = static_cast<int>(planet1);
    int sweBody2 = static_cast<int>(planet2);

    if (planet1 == Planet::NORTH_NODE) sweBody1 = SE_MEAN_NODE;
    if (planet2 == Planet::NORTH_NODE) sweBody2 = SE_MEAN_NODE;

    swe_calc(julianDay, sweBody1, SEFLG_SWIEPH, pos1, serr);
    swe_calc(julianDay, sweBody2, SEFLG_SWIEPH, pos2, serr);

    conjunction.longitude = (pos1[0] + pos2[0]) / 2.0;
    conjunction.sign = longitudeToSign(conjunction.longitude);
    conjunction.orb = calculateConjunctionOrb(planet1, planet2, julianDay);

    // Determine if applying or separating
    swe_calc(julianDay - 0.1, sweBody1, SEFLG_SWIEPH, pos1, serr);
    swe_calc(julianDay - 0.1, sweBody2, SEFLG_SWIEPH, pos2, serr);
    double previousOrb = std::abs(pos1[0] - pos2[0]);
    if (previousOrb > 180.0) previousOrb = 360.0 - previousOrb;

    conjunction.isApplying = conjunction.orb < previousOrb;
    conjunction.duration = 3.0; // Approximate duration for orb < 3°

    return conjunction;
}

ConjunctionType ConjunctionCalculator::determineConjunctionType(const std::vector<Planet>& planets) const {
    if (planets.size() > 2) {
        if (planets.size() >= 4) return ConjunctionType::GRAND;
        return ConjunctionType::TRIPLE;
    }

    for (Planet planet : planets) {
        if (planet == Planet::SUN) return ConjunctionType::SOLAR;
        if (planet == Planet::MOON) return ConjunctionType::LUNAR;
        if (planet == Planet::NORTH_NODE || planet == Planet::SOUTH_NODE) return ConjunctionType::NODAL;
    }

    return ConjunctionType::PLANETARY;
}

// String formatting functions

std::string ConjunctionEvent::getDateString() const {
    int year, month, day, hour, minute;
    double second;
    swe_jdet_to_utc(julianDay, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);

    std::stringstream ss;
    ss << year << "-" << std::setfill('0') << std::setw(2) << month << "-"
       << std::setw(2) << day << " " << std::setw(2) << hour << ":"
       << std::setw(2) << minute << " UTC";
    return ss.str();
}

std::string ConjunctionEvent::getPlanetsString() const {
    std::string result;
    for (size_t i = 0; i < planets.size(); ++i) {
        if (i > 0) result += " - ";
        result += planetToString(planets[i]);
    }
    return result;
}

std::string ConjunctionEvent::getTypeString() const {
    switch (type) {
        case ConjunctionType::PLANETARY: return "Planetary";
        case ConjunctionType::SOLAR: return "Solar";
        case ConjunctionType::LUNAR: return "Lunar";
        case ConjunctionType::NODAL: return "Nodal";
        case ConjunctionType::GRAND: return "Grand";
        default: return "Unknown";
    }
}

std::string ConjunctionEvent::getDescription() const {
    std::stringstream ss;
    ss << "Conjunction of " << getPlanetsString() << " at "
       << std::fixed << std::setprecision(2) << longitude << "° "
       << zodiacSignToString(sign) << ".";
    return ss.str();
}

std::string ConjunctionCalculator::getConjunctionSignificance(const ConjunctionEvent& conjunction) const {
    std::stringstream ss;

    // Analyze significance based on planets involved
    bool hasOuter = false;
    bool hasInner = false;
    bool hasLuminaries = false;

    for (Planet planet : conjunction.planets) {
        if (planet == Planet::SUN || planet == Planet::MOON) hasLuminaries = true;
        if (planet == Planet::MERCURY || planet == Planet::VENUS || planet == Planet::MARS) hasInner = true;
        if (planet == Planet::JUPITER || planet == Planet::SATURN ||
            planet == Planet::URANUS || planet == Planet::NEPTUNE || planet == Planet::PLUTO) hasOuter = true;
    }

    if (hasOuter && hasLuminaries) {
        ss << "High significance - involves outer planets and luminaries";
    } else if (hasOuter) {
        ss << "Significant - involves outer planets with generational influence";
    } else if (hasLuminaries) {
        ss << "Moderate significance - involves Sun or Moon";
    } else {
        ss << "Personal significance - involves inner planets";
    }

    if (conjunction.orb < 1.0) {
        ss << ". Very tight orb increases potency.";
    } else if (conjunction.orb > 2.5) {
        ss << ". Wide orb reduces immediate impact.";
    }

    return ss.str();
}

void ConjunctionCalculator::sortConjunctionsByDate(std::vector<ConjunctionEvent>& conjunctions) const {
    std::sort(conjunctions.begin(), conjunctions.end(),
              [](const ConjunctionEvent& a, const ConjunctionEvent& b) {
                  return a.julianDay < b.julianDay;
              });
}

std::vector<ConjunctionEvent> ConjunctionCalculator::findMultiplePlanetConjunctions(double julianDay, double maxOrb) const {
    // Implementation for finding triple+ conjunctions would go here
    // This is a simplified version - full implementation would check all combinations
    return {};
}

double ConjunctionCalculator::calculateConjunctionOrb(Planet planet1, Planet planet2, double julianDay) const {
    double pos1[6], pos2[6];
    char serr[256];

    int sweBody1 = static_cast<int>(planet1);
    int sweBody2 = static_cast<int>(planet2);

    if (planet1 == Planet::NORTH_NODE) sweBody1 = SE_MEAN_NODE;
    if (planet2 == Planet::NORTH_NODE) sweBody2 = SE_MEAN_NODE;

    swe_calc(julianDay, sweBody1, SEFLG_SWIEPH, pos1, serr);
    swe_calc(julianDay, sweBody2, SEFLG_SWIEPH, pos2, serr);

    double diff = std::abs(pos1[0] - pos2[0]);
    if (diff > 180.0) diff = 360.0 - diff;

    return diff;
}

void ConjunctionCalculator::printConjunctionEvent(const ConjunctionEvent& conjunction) const {
    std::cout << conjunction.getDateString() << " - " << conjunction.getPlanetsString()
              << " (" << conjunction.getTypeString() << ")\n";
    std::cout << "  Orb: " << std::fixed << std::setprecision(2) << conjunction.orb << "°";
    std::cout << "  Description: " << conjunction.getDescription() << "\n";
}

} // namespace Astro
