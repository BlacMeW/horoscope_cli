#include "aspect_calculator.h"
#include <cmath>
#include <algorithm>

namespace Astro {

AspectCalculator::AspectCalculator() {
    setDefaultOrbs();
}

std::vector<Aspect> AspectCalculator::calculateAspects(const std::vector<PlanetPosition>& positions) {
    std::vector<Aspect> aspects;

    for (size_t i = 0; i < positions.size(); i++) {
        for (size_t j = i + 1; j < positions.size(); j++) {
            Aspect aspect;
            if (calculateAspectBetweenPlanets(positions[i], positions[j], aspect)) {
                aspects.push_back(aspect);
            }
        }
    }

    // Sort aspects by orb (tightest first)
    std::sort(aspects.begin(), aspects.end(),
              [](const Aspect& a, const Aspect& b) {
                  return a.orb < b.orb;
              });

    return aspects;
}

std::vector<Aspect> AspectCalculator::calculateAspectsForPlanet(const std::vector<PlanetPosition>& positions,
                                                               Planet planet) {
    std::vector<Aspect> aspects;

    // Find the target planet
    auto planetIt = std::find_if(positions.begin(), positions.end(),
                                 [planet](const PlanetPosition& pos) {
                                     return pos.planet == planet;
                                 });

    if (planetIt == positions.end()) {
        return aspects; // Planet not found
    }

    // Calculate aspects with all other planets
    for (const auto& otherPos : positions) {
        if (otherPos.planet != planet) {
            Aspect aspect;
            if (calculateAspectBetweenPlanets(*planetIt, otherPos, aspect)) {
                aspects.push_back(aspect);
            }
        }
    }

    // Sort by orb
    std::sort(aspects.begin(), aspects.end(),
              [](const Aspect& a, const Aspect& b) {
                  return a.orb < b.orb;
              });

    return aspects;
}

void AspectCalculator::setOrbTolerance(AspectType aspect, double orb) {
    orbTolerances[aspect] = orb;
}

void AspectCalculator::setDefaultOrbs() {
    orbTolerances[AspectType::CONJUNCTION] = 8.0;
    orbTolerances[AspectType::OPPOSITION] = 8.0;
    orbTolerances[AspectType::SQUARE] = 6.0;
    orbTolerances[AspectType::TRINE] = 6.0;
    orbTolerances[AspectType::SEXTILE] = 4.0;
}

bool AspectCalculator::isAspect(double angle1, double angle2, AspectType aspect, double& orb) const {
    double diff = std::abs(angle1 - angle2);
    if (diff > 180.0) diff = 360.0 - diff;

    double aspectAngle = static_cast<double>(aspect);
    orb = std::abs(diff - aspectAngle);

    auto it = orbTolerances.find(aspect);
    if (it != orbTolerances.end()) {
        return orb <= it->second;
    }

    return false; // Unknown aspect type
}

std::vector<AspectType> AspectCalculator::getSupportedAspects() {
    return {
        AspectType::CONJUNCTION,
        AspectType::SEXTILE,
        AspectType::SQUARE,
        AspectType::TRINE,
        AspectType::OPPOSITION
    };
}

bool AspectCalculator::calculateAspectBetweenPlanets(const PlanetPosition& pos1,
                                                   const PlanetPosition& pos2,
                                                   Aspect& aspect) const {
    std::vector<AspectType> aspectTypes = getSupportedAspects();

    for (AspectType aspectType : aspectTypes) {
        double orb;
        if (isAspect(pos1.longitude, pos2.longitude, aspectType, orb)) {
            aspect.planet1 = pos1.planet;
            aspect.planet2 = pos2.planet;
            aspect.type = aspectType;
            aspect.orb = orb;
            aspect.isApplying = isAspectApplying(pos1, pos2, aspectType);
            return true;
        }
    }

    return false; // No aspect found within orb tolerances
}

bool AspectCalculator::isAspectApplying(const PlanetPosition& pos1,
                                      const PlanetPosition& pos2,
                                      AspectType aspect) const {
    // Determine if planets are moving toward (applying) or away from (separating) the exact aspect
    
    double speed1 = pos1.speed;
    double speed2 = pos2.speed;
    double relativeSpeed = speed1 - speed2;

    // If no relative motion, aspect is stable (neither applying nor separating)
    if (std::abs(relativeSpeed) < 0.01) {
        return false;
    }

    double long1 = pos1.longitude;
    double long2 = pos2.longitude;
    
    // Calculate current angular separation
    double currentSeparation = long1 - long2;
    if (currentSeparation > 180.0) currentSeparation -= 360.0;
    if (currentSeparation < -180.0) currentSeparation += 360.0;
    
    double targetAngle = static_cast<double>(aspect);
    
    // For aspects other than conjunction, need to consider both directions
    double distanceToAspect;
    if (aspect == AspectType::CONJUNCTION) {
        distanceToAspect = std::abs(currentSeparation);
    } else {
        // Calculate shortest distance to aspect angle (considering both positive and negative directions)
        double dist1 = std::abs(currentSeparation - targetAngle);
        double dist2 = std::abs(currentSeparation + targetAngle);
        double dist3 = std::abs(currentSeparation - (targetAngle - 360.0));
        double dist4 = std::abs(currentSeparation + (targetAngle - 360.0));
        distanceToAspect = std::min({dist1, dist2, dist3, dist4});
    }
    
    // Calculate future separation after a small time increment (1 day)
    double futureSeparation = currentSeparation + relativeSpeed;
    if (futureSeparation > 180.0) futureSeparation -= 360.0;
    if (futureSeparation < -180.0) futureSeparation += 360.0;
    
    double futureDistanceToAspect;
    if (aspect == AspectType::CONJUNCTION) {
        futureDistanceToAspect = std::abs(futureSeparation);
    } else {
        double dist1 = std::abs(futureSeparation - targetAngle);
        double dist2 = std::abs(futureSeparation + targetAngle);
        double dist3 = std::abs(futureSeparation - (targetAngle - 360.0));
        double dist4 = std::abs(futureSeparation + (targetAngle - 360.0));
        futureDistanceToAspect = std::min({dist1, dist2, dist3, dist4});
    }
    
    // Aspect is applying if future distance is smaller than current distance
    return futureDistanceToAspect < distanceToAspect;
}

} // namespace Astro
