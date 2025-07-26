#ifndef PREDICTIVE_ASTROLOGY_H
#define PREDICTIVE_ASTROLOGY_H

#include "astro_types.h"
#include "birth_chart.h"
#include <vector>
#include <string>
#include <map>

namespace Astro {

struct DashaData {
    std::string dashaType;     // Vimshottari, Ashtottari, etc.
    Planet mainPlanet;         // Main period planet
    Planet subPlanet;          // Sub period planet
    Planet subSubPlanet;       // Sub-sub period planet
    std::string startDate;     // Start date of period
    std::string endDate;       // End date of period
    int durationYears;         // Duration in years
    int durationMonths;        // Duration in months
    int durationDays;          // Duration in days
    std::string interpretation; // Predicted effects
    std::string remedies;      // Suggested remedies
    double strength;           // Strength rating (0-100)
};

struct PredictionEvent {
    std::string date;          // Date of predicted event
    std::string eventType;     // Career, Relationship, Health, etc.
    std::string description;   // Event description
    double probability;        // Probability (0-100)
    std::string timing;        // Exact, Approximate, Range
    std::vector<std::string> indicators; // Astrological indicators
    std::string advice;        // Recommended actions
    std::string category;      // Major, Moderate, Minor
};

struct YearlyForecast {
    int year;
    std::string overallTheme;
    std::vector<std::string> majorEvents;
    std::vector<std::string> opportunities;
    std::vector<std::string> challenges;
    std::vector<std::string> healthFocus;
    std::vector<std::string> careerFocus;
    std::vector<std::string> relationshipFocus;
    std::vector<std::string> financeFocus;
    std::vector<std::string> spiritualFocus;
    std::vector<std::string> luckyPeriods;
    std::vector<std::string> cautiousPeriods;
    std::string jupiterTransit;
    std::string saturnTransit;
    std::string rahuKetuTransit;
};

struct VedicPrediction {
    DashaData currentDasha;
    std::vector<DashaData> upcomingDashas;
    std::vector<std::string> yogas;       // Planetary combinations
    std::vector<std::string> doshas;      // Afflictions
    std::vector<std::string> remedies;    // Vedic remedies
    std::string lifePhase;               // Current life phase
    std::vector<std::string> maritalPredictions;
    std::vector<std::string> careerPredictions;
    std::vector<std::string> healthPredictions;
    std::vector<std::string> financialPredictions;
    std::vector<std::string> educationPredictions;
    std::vector<std::string> spiritualPredictions;
};

struct MonthlyForecast {
    int year;
    int month;
    std::string monthName;
    std::string overallEnergy;
    std::vector<std::string> keyDates;
    std::vector<std::string> opportunities;
    std::vector<std::string> challenges;
    std::vector<std::string> healthTips;
    std::vector<std::string> careerAdvice;
    std::vector<std::string> relationshipInsights;
    std::vector<std::string> financialGuidance;
    std::string newMoonDate;
    std::string fullMoonDate;
    std::vector<std::string> eclipseEffects;
    std::vector<std::string> mercuryRetrograde;
};

struct LifePhase {
    std::string phaseName;     // Childhood, Youth, Adulthood, etc.
    std::string startDate;
    std::string endDate;
    std::vector<std::string> themes;
    std::vector<std::string> challenges;
    std::vector<std::string> opportunities;
    std::vector<std::string> lessons;
    std::string dominantPlanet;
    std::vector<std::string> keyEvents;
    std::string spiritualFocus;
};

class PredictiveAstrology {
private:
    BirthChart birthChart;
    double latitude;
    double longitude;

    // Helper methods
    std::vector<DashaData> calculateVimshottariDasha() const;
    std::vector<DashaData> calculateAshtottariDasha() const;
    std::vector<std::string> analyzeYogas() const;
    std::vector<std::string> analyzeDoshas() const;
    std::vector<std::string> calculateRemedies() const;
    double calculatePlanetStrength(Planet planet) const;

public:
    PredictiveAstrology(const BirthChart& chart, double lat, double lon);

    // Dasha Systems
    std::vector<DashaData> calculateMainDashas(const std::string& dashaType = "vimshottari") const;
    DashaData getCurrentDasha(const std::string& date = "") const;
    std::vector<DashaData> getFutureDashas(int years = 10) const;
    std::string generateDashaReport(int years = 20) const;

    // Life Predictions
    std::vector<PredictionEvent> predictMajorEvents(int years = 5) const;
    std::vector<PredictionEvent> predictCareerEvents(int years = 3) const;
    std::vector<PredictionEvent> predictRelationshipEvents(int years = 3) const;
    std::vector<PredictionEvent> predictHealthEvents(int years = 3) const;
    std::vector<PredictionEvent> predictFinancialEvents(int years = 3) const;

    // Time-Based Forecasts
    YearlyForecast generateYearlyForecast(int year) const;
    MonthlyForecast generateMonthlyForecast(int year, int month) const;
    std::vector<std::string> generateWeeklyForecast(const std::string& weekStartDate) const;
    std::vector<std::string> generateDailyForecast(const std::string& date) const;

    // Vedic Predictions
    VedicPrediction generateVedicPrediction() const;
    std::vector<std::string> predictMarriageTiming() const;
    std::vector<std::string> predictChildbirthTiming() const;
    std::vector<std::string> predictCareerMilestones() const;
    std::vector<std::string> predictHealthChallenges() const;
    std::vector<std::string> predictSpiritualJourney() const;

    // Life Phases
    std::vector<LifePhase> calculateLifePhases() const;
    LifePhase getCurrentLifePhase() const;
    LifePhase getNextLifePhase() const;
    std::string generateLifePhaseReport() const;

    // Timing Analysis
    std::vector<std::string> findAuspiciousTimingForMarriage(int years = 2) const;
    std::vector<std::string> findAuspiciousTimingForBusiness(int years = 1) const;
    std::vector<std::string> findAuspiciousTimingForTravel(int months = 6) const;
    std::vector<std::string> findAuspiciousTimingForEducation(int years = 1) const;
    std::vector<std::string> findBestTimingForSurgery(int months = 6) const;

    // Mundane Predictions
    std::vector<std::string> predictWeatherEvents(const std::string& region, int months = 3) const;
    std::vector<std::string> predictEconomicTrends(const std::string& country, int months = 6) const;
    std::vector<std::string> predictPoliticalEvents(const std::string& country, int months = 12) const;
    std::vector<std::string> predictNaturalDisasters(const std::string& region, int months = 6) const;

    // Remedial Measures
    std::vector<std::string> calculateGemstoneRemedies() const;
    std::vector<std::string> calculateMantraRemedies() const;
    std::vector<std::string> calculateYantraRemedies() const;
    std::vector<std::string> calculateCharityRemedies() const;
    std::vector<std::string> calculateFastingRemedies() const;
    std::vector<std::string> calculateColorTherapyRemedies() const;
    std::vector<std::string> calculateRudrakshRemedies() const;

    // Specialized Predictions
    std::vector<std::string> predictStockMarketTrends(const std::string& symbol, int months = 3) const;
    std::vector<std::string> predictRealEstateMarket(const std::string& location, int months = 6) const;
    std::vector<std::string> predictSportsPerformance(const std::string& sport, int months = 6) const;
    std::vector<std::string> predictAcademicPerformance(int years = 2) const;

    // Report Generation
    std::string generateComprehensivePredictionReport(int years = 5) const;
    std::string generateVedicPredictionReport() const;
    std::string generateRemedialReport() const;
    std::string generateTimingReport(const std::string& purpose) const;

    // JSON Output
    std::string generatePredictionJSON(int years = 3) const;
    std::string generateDashaJSON(int years = 10) const;
    std::string generateVedicPredictionJSON() const;
    std::string generateYearlyForecastJSON(int year) const;
    std::string generateMonthlyForecastJSON(int year, int month) const;
};

} // namespace Astro

#endif // PREDICTIVE_ASTROLOGY_H
