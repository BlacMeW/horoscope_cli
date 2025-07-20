#include "conjunction_calculator.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "astro_types.h"
#include <swephexp.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <map>

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

    // Parse date strings with BC era support
    parseBCDate(fromDate, fromBirth.year, fromBirth.month, fromBirth.day);
    parseBCDate(toDate, toBirth.year, toBirth.month, toBirth.day);

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

    // Initialize Graha Yuddha fields
    conjunction.isGrahaYuddha = false;
    conjunction.grahaYuddhaWinner = Planet::SUN; // Default, will be set if it's actually a war
    conjunction.grahaYuddhaEffect = "";

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
    std::cout << "  Description: " << conjunction.getDescription();

    if (conjunction.isGrahaYuddha) {
        std::cout << " [GRAHA YUDDHA - Winner: " << planetToString(conjunction.grahaYuddhaWinner) << "]";
    }
    std::cout << "\n";
}

std::vector<ConjunctionEvent> ConjunctionCalculator::findConjunctionsWithLatitudeRange(
    const BirthData& fromDate, const BirthData& toDate,
    double maxOrb, double minLatitude, double maxLatitude) const {

    if (!isInitialized) {
        lastError = "Conjunction calculator not initialized";
        return {};
    }

    std::vector<ConjunctionEvent> allConjunctions = findConjunctions(fromDate, toDate, maxOrb);
    std::vector<ConjunctionEvent> filteredConjunctions;

    EphemerisManager ephMgr;
    ephMgr.initialize(ephemerisPath);

    for (const auto& conjunction : allConjunctions) {
        bool withinLatitudeRange = true;

        // Check each planet's latitude at the conjunction time
        for (Planet planet : conjunction.planets) {
            PlanetPosition position;
            if (ephMgr.calculatePlanetPosition(conjunction.julianDay, planet, position)) {
                double latitude = position.latitude;
                if (latitude < minLatitude || latitude > maxLatitude) {
                    withinLatitudeRange = false;
                    break;
                }
            }
        }

        if (withinLatitudeRange) {
            filteredConjunctions.push_back(conjunction);
        }
    }

    return filteredConjunctions;
}

std::vector<ConjunctionEvent> ConjunctionCalculator::findGrahaYuddha(
    const BirthData& fromDate, const BirthData& toDate, double maxOrb) const {

    if (!isInitialized) {
        lastError = "Conjunction calculator not initialized";
        return {};
    }

    std::vector<ConjunctionEvent> allConjunctions = findConjunctions(fromDate, toDate, maxOrb);
    std::vector<ConjunctionEvent> grahaYuddhaEvents;

    for (auto conjunction : allConjunctions) {
        if (isGrahaYuddha(conjunction)) {
            conjunction.isGrahaYuddha = true;

            // Determine winner if it's a two-planet war
            if (conjunction.planets.size() == 2) {
                conjunction.grahaYuddhaWinner = getGrahaYuddhaWinner(
                    conjunction.planets[0], conjunction.planets[1], conjunction.julianDay);

                // Add effect description
                conjunction.grahaYuddhaEffect = getGrahaYuddhaEffectDescription(
                    conjunction.grahaYuddhaWinner, conjunction.planets[0], conjunction.planets[1]);
            }

            grahaYuddhaEvents.push_back(conjunction);
        }
    }

    return grahaYuddhaEvents;
}

bool ConjunctionCalculator::isGrahaYuddha(const ConjunctionEvent& conjunction) const {
    // Graha Yuddha conditions:
    // 1. Very tight orb (usually < 1°)
    // 2. Involves visible planets (not nodes, Chiron, or outer planets in traditional Vedic)
    // 3. Planets should be in the same sign
    // 4. Not involving Sun or Moon (they don't participate in wars in traditional sense)

    if (conjunction.orb > 1.0) return false;
    if (conjunction.planets.size() != 2) return false; // Only two-planet wars for now

    std::vector<Planet> traditionalPlanets = {
        Planet::MARS, Planet::MERCURY, Planet::JUPITER, Planet::VENUS, Planet::SATURN
    };

    int validPlanets = 0;
    for (Planet p : conjunction.planets) {
        if (std::find(traditionalPlanets.begin(), traditionalPlanets.end(), p) != traditionalPlanets.end()) {
            validPlanets++;
        }
    }

    return validPlanets == 2;
}

Planet ConjunctionCalculator::getGrahaYuddhaWinner(Planet planet1, Planet planet2, double julianDay) const {
    // Graha Yuddha winner determination based on Vedic astrology rules:
    // 1. The planet that is further south (higher latitude) wins
    // 2. If latitudes are very close, the brighter planet wins
    // 3. Hierarchical rules: Jupiter > Saturn > Mars > Venus > Mercury

    EphemerisManager ephMgr;
    ephMgr.initialize(ephemerisPath);

    PlanetPosition pos1, pos2;
    if (!ephMgr.calculatePlanetPosition(julianDay, planet1, pos1) ||
        !ephMgr.calculatePlanetPosition(julianDay, planet2, pos2)) {
        // Fallback to hierarchy if calculation fails
        return getStrongerPlanetByHierarchy(planet1, planet2);
    }

    // Primary rule: Southern planet (higher latitude) wins
    if (std::abs(pos1.latitude - pos2.latitude) > 0.1) {
        return (pos1.latitude > pos2.latitude) ? planet1 : planet2;
    }

    // Secondary rule: Brighter planet wins (approximated by distance)
    if (std::abs(pos1.distance - pos2.distance) > 0.01) {
        return (pos1.distance < pos2.distance) ? planet1 : planet2;
    }

    // Tertiary rule: Hierarchical strength
    return getStrongerPlanetByHierarchy(planet1, planet2);
}

Planet ConjunctionCalculator::getStrongerPlanetByHierarchy(Planet planet1, Planet planet2) const {
    // Traditional Vedic hierarchy for Graha Yuddha
    std::map<Planet, int> hierarchy = {
        {Planet::JUPITER, 5},
        {Planet::SATURN, 4},
        {Planet::MARS, 3},
        {Planet::VENUS, 2},
        {Planet::MERCURY, 1}
    };

    int strength1 = hierarchy.count(planet1) ? hierarchy[planet1] : 0;
    int strength2 = hierarchy.count(planet2) ? hierarchy[planet2] : 0;

    return (strength1 > strength2) ? planet1 : planet2;
}

std::string ConjunctionCalculator::getGrahaYuddhaEffectDescription(Planet winner, Planet planet1, Planet planet2) const {
    std::string loser = (winner == planet1) ? planetToString(planet2) : planetToString(planet1);
    std::string winnerStr = planetToString(winner);

    std::stringstream ss;
    ss << winnerStr << " defeats " << loser << " in planetary war. ";

    // Add traditional effects based on winner
    if (winner == Planet::JUPITER) {
        ss << "Enhances wisdom, righteousness, and fortune.";
    } else if (winner == Planet::SATURN) {
        ss << "Brings discipline, delays, and karmic lessons.";
    } else if (winner == Planet::MARS) {
        ss << "Increases energy, courage, and potential conflicts.";
    } else if (winner == Planet::VENUS) {
        ss << "Enhances beauty, arts, and relationships.";
    } else if (winner == Planet::MERCURY) {
        ss << "Strengthens intellect, communication, and commerce.";
    }

    return ss.str();
}

std::string ConjunctionCalculator::generateGrahaYuddhaReport(const std::vector<ConjunctionEvent>& wars) const {
    std::stringstream ss;

    ss << "\n=== GRAHA YUDDHA (PLANETARY WAR) REPORT ===\n\n";
    ss << "Total planetary wars found: " << wars.size() << "\n\n";

    if (wars.empty()) {
        ss << "No Graha Yuddha events found in the specified time range.\n";
        ss << "Graha Yuddha occurs when two visible planets are within 1° of each other.\n";
        return ss.str();
    }

    ss << "Graha Yuddha Events:\n";
    ss << "════════════════════\n\n";

    for (const auto& war : wars) {
        ss << "📅 Date: " << war.getDateString() << "\n";
        ss << "🪐 Combatants: " << war.getPlanetsString() << "\n";
        ss << "📍 Location: " << war.longitude << "° " << zodiacSignToString(war.sign) << "\n";
        ss << "🎯 Orb: " << std::fixed << std::setprecision(3) << war.orb << "°\n";
        ss << "🏆 Winner: " << planetToString(war.grahaYuddhaWinner) << "\n";
        ss << "✨ Effect: " << war.grahaYuddhaEffect << "\n";
        ss << "───────────────────────────────────────────────────────\n\n";
    }

    ss << "📚 GRAHA YUDDHA PRINCIPLES:\n";
    ss << "• The planet that is more southward (higher ecliptic latitude) wins\n";
    ss << "• If latitudes are equal, the brighter (closer) planet wins\n";
    ss << "• Traditional hierarchy: Jupiter > Saturn > Mars > Venus > Mercury\n";
    ss << "• The winning planet's significations are enhanced\n";
    ss << "• The losing planet's significations are weakened or modified\n";

    return ss.str();
}

} // namespace Astro
