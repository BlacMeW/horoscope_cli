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
    // Determine if the faster planet is moving toward the aspect
    // This is a simplified calculation

    double speed1 = pos1.speed;
    double speed2 = pos2.speed;

    // If speeds are very close, consider it stable
    if (std::abs(speed1 - speed2) < 0.01) {
        return false;
    }

    // The faster planet
    Planet fasterPlanet = (speed1 > speed2) ? pos1.planet : pos2.planet;
    double fasterLong = (speed1 > speed2) ? pos1.longitude : pos2.longitude;
    double slowerLong = (speed1 > speed2) ? pos2.longitude : pos1.longitude;

    double targetAngle = static_cast<double>(aspect);
    double currentAngle = std::abs(fasterLong - slowerLong);
    if (currentAngle > 180.0) currentAngle = 360.0 - currentAngle;

    // Simplified: if the current angle is less than target, it's applying
    return currentAngle < targetAngle;
}

} // namespace Astro
