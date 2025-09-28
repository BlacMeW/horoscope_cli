#pragma once

#include "conjunction_calculator.h"
#include "astro_types.h"
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <future>
#include <map>

namespace Astro {

// Refinement result structure (inspired by birthfinder)
struct NewtonRaphsonResult {
    double time;
    double separation;
    int iterations;
    bool converged;
    double tolerance;
    std::string method; // "bisection" or "newton-raphson"
};

// Advanced conjunction finder with multiple algorithms
class AdvancedConjunctionFinder {
public:
    AdvancedConjunctionFinder();
    ~AdvancedConjunctionFinder();

    // Initialize with ephemeris data
    bool initialize(const std::string& ephePath = "");

    // Find conjunctions using multi-algorithm approach
    std::vector<ConjunctionEvent> findConjunctionsAdvanced(
        const BirthData& fromDate, 
        const BirthData& toDate,
        double maxOrb = 3.0,
        bool useNewtonRaphson = true,
        bool useProgressiveTolerance = true) const;

    // Progressive conjunction finder with multiple tolerance levels
    std::vector<ConjunctionEvent> findConjunctionsProgressive(
        const BirthData& fromDate,
        const BirthData& toDate,
        const std::vector<double>& toleranceLevels = {5.0, 1.0, 0.5, 0.1, 0.05, 0.01}) const;

    // Multi-planet conjunction finder
    std::vector<ConjunctionEvent> findMultiPlanetConjunctions(
        const BirthData& fromDate,
        const BirthData& toDate,
        int minPlanets = 3,
        double maxOrb = 5.0) const;

    // Single conjunction search with refinement
    ConjunctionEvent findConjunctionRefined(
        Planet planet1,
        Planet planet2,
        double startJD,
        double endJD,
        double tolerance = 0.001,
        bool useNewtonRaphson = true) const;

    // Background conjunction search (for large date ranges)
    std::future<std::vector<ConjunctionEvent>> findConjunctionsAsync(
        const BirthData& fromDate,
        const BirthData& toDate,
        double maxOrb = 3.0) const;

    // Graha Yuddha (Planetary Wars) detection with advanced algorithms
    std::vector<ConjunctionEvent> findGrahaYuddha(
        const BirthData& fromDate,
        const BirthData& toDate,
        double maxOrb = 1.0) const;

private:
    bool isInitialized;
    std::string ephemerisPath;
    mutable std::string lastError;

    // Position cache to avoid redundant calculations
    mutable std::map<std::pair<Planet, double>, std::pair<double, double>> positionCache;

    // Core search algorithms
    NewtonRaphsonResult searchForward(
        Planet planet1, 
        Planet planet2,
        double startJD,
        double stepSize = 1.0,
        double tolerance = 0.001) const;

    NewtonRaphsonResult searchBackward(
        Planet planet1,
        Planet planet2, 
        double startJD,
        double stepSize = 1.0,
        double tolerance = 0.001) const;

    // Refinement methods
    NewtonRaphsonResult refineBisection(
        Planet planet1,
        Planet planet2,
        double startJD,
        double endJD,
        double tolerance = 0.001) const;

    NewtonRaphsonResult refineNewtonRaphson(
        Planet planet1,
        Planet planet2,
        double approximateJD,
        double tolerance = 0.001,
        int maxIterations = 20) const;

    // Utility functions
    double calculateSeparation(Planet planet1, Planet planet2, double jd) const;
    double calculateSeparationDerivative(Planet planet1, Planet planet2, double jd) const;
    std::pair<double, double> getPlanetPosition(Planet planet, double jd) const;
    
    // Multi-planet detection
    std::vector<Planet> findPlanetsInRange(double longitude, double orb, double jd) const;
    bool areMultiplePlanetsClose(const std::vector<Planet>& planets, double jd, double maxOrb) const;

    // Progressive search helpers
    std::vector<ConjunctionEvent> searchWithTolerance(
        const BirthData& fromDate,
        const BirthData& toDate,
        double tolerance) const;

    // Validation and filtering
    bool isValidConjunction(const ConjunctionEvent& conjunction, double maxOrb) const;
    void removeDuplicates(std::vector<ConjunctionEvent>& conjunctions) const;
    void sortByAccuracy(std::vector<ConjunctionEvent>& conjunctions) const;

    // Graha Yuddha helpers
    Planet determineGrahaYuddhaWinner(const std::vector<Planet>& planets, double jd) const;
    std::string getGrahaYuddhaEffect(const std::vector<Planet>& planets, Planet winner) const;

    // Error handling
    void setError(const std::string& error) const { lastError = error; }

public:
    std::string getLastError() const { return lastError; }
    void clearCache() const { positionCache.clear(); }
    size_t getCacheSize() const { return positionCache.size(); }
};

// Factory function to create appropriate conjunction finder
std::unique_ptr<AdvancedConjunctionFinder> createConjunctionFinder(bool useAdvanced = true);

} // namespace Astro