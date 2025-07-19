#pragma once

#include "astro_types.h"
#include <vector>
#include <map>

namespace Astro {

class AspectCalculator {
public:
    AspectCalculator();

    // Calculate all aspects between planets
    std::vector<Aspect> calculateAspects(const std::vector<PlanetPosition>& positions);

    // Calculate aspects for a specific planet
    std::vector<Aspect> calculateAspectsForPlanet(const std::vector<PlanetPosition>& positions,
                                                  Planet planet);

    // Set orb tolerances for different aspects
    void setOrbTolerance(AspectType aspect, double orb);

    // Set default orb tolerances
    void setDefaultOrbs();

    // Check if two angles form a specific aspect
    bool isAspect(double angle1, double angle2, AspectType aspect, double& orb) const;

    // Get all supported aspects
    static std::vector<AspectType> getSupportedAspects();

private:
    std::map<AspectType, double> orbTolerances;

    // Calculate aspect between two planets
    bool calculateAspectBetweenPlanets(const PlanetPosition& pos1,
                                     const PlanetPosition& pos2,
                                     Aspect& aspect) const;

    // Determine if aspect is applying or separating
    bool isAspectApplying(const PlanetPosition& pos1,
                         const PlanetPosition& pos2,
                         AspectType aspect) const;
};

} // namespace Astro
