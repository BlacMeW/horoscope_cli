#ifndef ADVANCED_ASTROLOGY_H
#define ADVANCED_ASTROLOGY_H

#include "astro_types.h"
#include "birth_chart.h"
#include <vector>
#include <string>
#include <map>

namespace Astro {

struct MidpointData {
    Planet planet1;
    Planet planet2;
    double longitude;      // Midpoint longitude
    std::string sign;      // Zodiac sign
    int degree;           // Degree within sign
    int minute;           // Minutes
    std::string aspect;   // Aspect to other planets if any
    double orb;           // Orb of aspect
    std::string interpretation; // Astrological meaning
};

struct HarmonicData {
    int harmonic;         // Harmonic number (4th, 5th, 7th, 9th, etc.)
    std::map<Planet, double> planetPositions; // Planet positions in harmonic
    std::vector<std::string> patterns; // Patterns found
    double powerIndex;    // Strength of harmonic
    std::string meaning;  // Astrological interpretation
};

struct ProgressionData {
    Planet planet;
    double natalPosition;    // Natal position
    double progressedPosition; // Progressed position
    double dailyMotion;     // Daily motion
    std::string direction;  // Direct or Retrograde
    std::vector<std::string> aspects; // Aspects to natal/progressed planets
    std::string interpretation; // Meaning of progression
};

struct TransitData {
    Planet transitingPlanet;
    Planet natalPlanet;
    double transitPosition;
    double natalPosition;
    std::string aspectType;    // Conjunction, Opposition, etc.
    double orb;
    std::string peakDate;      // Date of exact aspect
    bool isApplying;          // True if applying, false if separating
    std::string intensity;    // Major, Minor, Background
    std::string interpretation; // Astrological meaning
};

struct ReturnData {
    Planet planet;
    std::string returnType;   // Solar, Lunar, Jupiter, etc.
    std::string exactTime;    // Time of exact return
    double returnPosition;    // Position at return
    std::vector<std::string> aspects; // Aspects in return chart
    std::string theme;        // Theme for this return period
    int durationDays;        // Duration until next return
};

struct ElectionData {
    std::string date;
    std::string time;
    double score;            // Electional score (0-100)
    std::vector<std::string> favorableFactors; // Positive aspects
    std::vector<std::string> challengingFactors; // Difficult aspects
    std::string recommendation; // Best use for this timing
    std::string moonPhase;   // Moon phase description
    std::string voidOfCourse; // Void of course Moon timing
};

class AdvancedAstrology {
private:
    BirthChart birthChart;
    double latitude;
    double longitude;

public:
    AdvancedAstrology(const BirthChart& chart, double lat, double lon);

    // Midpoint Analysis
    std::vector<MidpointData> calculateMidpoints() const;
    std::vector<MidpointData> calculateSolarArcMidpoints() const;
    MidpointData calculatePlanetMidpoint(Planet p1, Planet p2) const;

    // Harmonic Analysis
    std::vector<HarmonicData> calculateHarmonics() const;
    HarmonicData calculateSpecificHarmonic(int harmonic) const;
    std::vector<std::string> findHarmonicPatterns(const HarmonicData& harmonic) const;

    // Secondary Progressions
    std::vector<ProgressionData> calculateProgressions(const std::string& targetDate) const;
    ProgressionData calculatePlanetProgression(Planet planet, const std::string& targetDate) const;
    std::vector<std::string> findProgressedAspects(const std::string& targetDate) const;

    // Transits
    std::vector<TransitData> calculateCurrentTransits() const;
    std::vector<TransitData> calculateTransitsForDate(const std::string& date) const;
    std::vector<TransitData> calculateTransitRange(const std::string& startDate, const std::string& endDate) const;
    std::vector<TransitData> findMajorTransits(const std::string& startDate, const std::string& endDate) const;

    // Returns
    std::vector<ReturnData> calculateReturns(const std::string& startDate, const std::string& endDate) const;
    ReturnData calculateSolarReturn(int year) const;
    ReturnData calculateLunarReturn(const std::string& targetMonth) const;
    ReturnData calculatePlanetaryReturn(Planet planet, const std::string& nearDate) const;

    // Electional Astrology
    std::vector<ElectionData> findAuspiciousTimes(const std::string& startDate, const std::string& endDate,
                                                   const std::string& purpose) const;
    ElectionData analyzeElectionTime(const std::string& date, const std::string& time) const;
    double calculateElectionScore(const std::string& date, const std::string& time, const std::string& purpose) const;

    // Advanced Techniques
    std::vector<std::string> calculateArabicParts() const;
    std::vector<std::string> calculateFixedStars() const;
    std::vector<std::string> calculateAsteroids() const;
    std::vector<std::string> calculateVertexAxis() const;

    // Relationship Analysis (Synastry)
    std::vector<std::string> calculateSynastryAspects(const BirthChart& otherChart) const;
    std::vector<std::string> calculateCompositeChart(const BirthChart& otherChart) const;
    double calculateCompatibilityScore(const BirthChart& otherChart) const;

    // Financial Astrology
    std::vector<std::string> analyzeDailyMarkets(const std::string& date) const;
    std::vector<std::string> findTradingOpportunities(const std::string& startDate, const std::string& endDate) const;
    std::vector<ElectionData> findBusinessStartTimes(const std::string& startDate, const std::string& endDate) const;

    // Weather & Natural Events
    std::vector<std::string> predictWeatherPatterns(const std::string& startDate, const std::string& endDate) const;
    std::vector<std::string> predictNaturalEvents(const std::string& startDate, const std::string& endDate) const;

    // Output Formatting
    std::string generateMidpointReport() const;
    std::string generateHarmonicReport() const;
    std::string generateProgressionReport(const std::string& targetDate) const;
    std::string generateTransitReport(const std::string& startDate, const std::string& endDate) const;
    std::string generateReturnReport(const std::string& startDate, const std::string& endDate) const;
    std::string generateElectionReport(const std::string& startDate, const std::string& endDate, const std::string& purpose) const;

    // JSON Output
    std::string generateAdvancedAnalysisJSON() const;
    std::string generateMidpointJSON() const;
    std::string generateHarmonicJSON() const;
    std::string generateProgressionJSON(const std::string& targetDate) const;
    std::string generateTransitJSON(const std::string& startDate, const std::string& endDate) const;
};

} // namespace Astro

#endif // ADVANCED_ASTROLOGY_H
