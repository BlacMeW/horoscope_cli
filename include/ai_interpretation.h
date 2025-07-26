#ifndef AI_INTERPRETATION_H
#define AI_INTERPRETATION_H

#include "astro_types.h"
#include "birth_chart.h"
#include <string>
#include <vector>
#include <map>

namespace Astro {

struct InterpretationRule {
    std::string id;
    std::string description;
    std::vector<std::string> conditions;
    double weight;              // Importance weight (0.1 to 1.0)
    std::string category;       // Personality, Career, Health, etc.
    std::string interpretation;
    std::vector<std::string> keywords;
    bool isPositive;           // True for positive, false for challenging
};

struct ChartPattern {
    std::string name;          // Grand Trine, T-Square, etc.
    std::vector<Planet> planets;
    std::vector<AspectType> aspects;
    double strength;           // 0-100
    std::string description;
    std::string interpretation;
    std::vector<std::string> keywords;
    std::string effect;        // Harmonious, Challenging, Dynamic
};

struct PersonalityInsight {
    std::string trait;         // Ambitious, Creative, Analytical, etc.
    double strength;           // 0-100
    std::string description;
    std::vector<std::string> indicators; // Astrological factors
    std::string category;      // Core, Secondary, Hidden
    std::vector<std::string> manifestations; // How it shows up
    std::vector<std::string> challenges; // Potential difficulties
    std::vector<std::string> opportunities; // Growth areas
};

struct LifeTheme {
    std::string theme;         // Leadership, Service, Creativity, etc.
    double prominence;         // 0-100
    std::string description;
    std::vector<std::string> supportingFactors;
    std::vector<std::string> challenges;
    std::string lifePhase;     // When most prominent
    std::vector<std::string> advice;
    std::string spiritualMeaning;
};

struct RelationshipPattern {
    std::string type;          // Romantic, Professional, Friendship
    std::vector<std::string> strengths;
    std::vector<std::string> challenges;
    std::vector<std::string> communication;
    std::vector<std::string> compatibility;
    double overallScore;       // 0-100
    std::string advice;
    std::vector<std::string> indicators;
};

struct CareerGuidance {
    std::vector<std::string> idealCareers;
    std::vector<std::string> workStyle;
    std::vector<std::string> strengths;
    std::vector<std::string> challenges;
    std::vector<std::string> workEnvironment;
    std::vector<std::string> leadership;
    std::vector<std::string> collaboration;
    std::string timing;        // Best career periods
    std::vector<std::string> advice;
};

struct HealthInsights {
    std::vector<std::string> strengths;
    std::vector<std::string> vulnerabilities;
    std::vector<std::string> bodyParts;     // Ruled by planets
    std::vector<std::string> prevention;
    std::vector<std::string> wellness;
    std::vector<std::string> diet;
    std::vector<std::string> exercise;
    std::vector<std::string> stressManagement;
    std::string constitution;  // Ayurvedic dosha
};

struct SpiritualGuidance {
    std::vector<std::string> path;          // Spiritual inclinations
    std::vector<std::string> practices;     // Recommended practices
    std::vector<std::string> challenges;    // Spiritual obstacles
    std::vector<std::string> gifts;         // Spiritual talents
    std::string purpose;                    // Life purpose
    std::vector<std::string> lessons;       // Karmic lessons
    std::vector<std::string> dharma;        // Life dharma
    std::string enlightenment;              // Path to growth
};

class AIInterpretation {
private:
    BirthChart chart;
    std::vector<InterpretationRule> rules;
    std::vector<ChartPattern> patterns;

    // Core AI analysis methods
    std::vector<ChartPattern> detectPatterns() const;
    std::vector<PersonalityInsight> analyzePersonality() const;
    std::vector<LifeTheme> identifyLifeThemes() const;
    double calculatePatternStrength(const ChartPattern& pattern) const;
    double calculateTraitStrength(const std::string& trait) const;

    // Specialized analysis
    std::vector<std::string> analyzePlanetaryDominance() const;
    std::vector<std::string> analyzeElementBalance() const;
    std::vector<std::string> analyzeModalityBalance() const;
    std::vector<std::string> analyzeHouseEmphasis() const;
    std::vector<std::string> analyzeAspectPatterns() const;

    // Rule-based reasoning
    bool evaluateCondition(const std::string& condition) const;
    double calculateRuleWeight(const InterpretationRule& rule) const;
    std::vector<InterpretationRule> getApplicableRules() const;

    // Natural language generation
    std::string generateNarrativeText(const std::vector<std::string>& points) const;
    std::string combineInterpretations(const std::vector<std::string>& interpretations) const;
    std::string addTransitions(const std::string& text) const;
    std::string improveReadability(const std::string& text) const;

public:
    AIInterpretation(const BirthChart& birthChart);

    // Main interpretation methods
    std::string generateFullInterpretation() const;
    std::string generatePersonalityProfile() const;
    std::string generateLifePurposeAnalysis() const;
    std::string generateRelationshipProfile() const;
    std::string generateCareerProfile() const;
    std::string generateHealthProfile() const;
    std::string generateSpiritualProfile() const;

    // Detailed analysis
    std::vector<PersonalityInsight> getPersonalityInsights() const;
    std::vector<LifeTheme> getLifeThemes() const;
    RelationshipPattern getRelationshipPattern() const;
    CareerGuidance getCareerGuidance() const;
    HealthInsights getHealthInsights() const;
    SpiritualGuidance getSpiritualGuidance() const;

    // Pattern recognition
    std::vector<ChartPattern> getMajorPatterns() const;
    std::vector<ChartPattern> getMinorPatterns() const;
    std::vector<std::string> getUnusualPatterns() const;
    std::vector<std::string> getStrengthPatterns() const;
    std::vector<std::string> getChallengePatterns() const;

    // Specialized interpretations
    std::string interpretSunMoonCombination() const;
    std::string interpretAscendantRuler() const;
    std::string interpretMidheaven() const;
    std::string interpretNodalAxis() const;
    std::string interpretChiron() const;
    std::string interpretVertexAxis() const;

    // Comparative analysis
    std::string compareSynastry(const BirthChart& otherChart) const;
    std::string analyzeComposite(const BirthChart& otherChart) const;
    std::string analyzeDavison(const BirthChart& otherChart) const;

    // Time-based interpretations
    std::string interpretProgressions(const std::string& targetDate) const;
    std::string interpretTransits(const std::string& targetDate) const;
    std::string interpretSolarReturn(int year) const;
    std::string interpretLunarReturn(const std::string& month) const;

    // Predictive interpretations
    std::string predictNextYear() const;
    std::string predictLifePhases() const;
    std::string predictChallenges() const;
    std::string predictOpportunities() const;
    std::string predictRelationships() const;
    std::string predictCareerEvolution() const;

    // Remedial suggestions
    std::vector<std::string> suggestRemedies() const;
    std::vector<std::string> suggestAffirmations() const;
    std::vector<std::string> suggestMeditations() const;
    std::vector<std::string> suggestCrystals() const;
    std::vector<std::string> suggestColors() const;
    std::vector<std::string> suggestAromas() const;

    // Learning and growth
    std::vector<std::string> identifyLearningStyles() const;
    std::vector<std::string> suggestPersonalGrowth() const;
    std::vector<std::string> identifyTalents() const;
    std::vector<std::string> suggestCreativeOutlets() const;

    // Customization
    void setInterpretationStyle(const std::string& style); // Psychological, Spiritual, Practical
    void setDetailLevel(const std::string& level);         // Brief, Standard, Detailed
    void setFocus(const std::vector<std::string>& areas);   // Personality, Career, Relationships
    void setLanguageStyle(const std::string& style);       // Formal, Casual, Poetic

    // Output formats
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
    std::string generateHTMLReport() const;
    std::string generateMarkdownReport() const;

    // Confidence and reliability
    double getInterpretationConfidence() const;
    std::vector<std::string> getInterpretationSources() const;
    std::vector<std::string> getUncertainties() const;
    std::string getReliabilityStatement() const;

    // Rule management
    void loadInterpretationRules(const std::string& filename);
    void saveInterpretationRules(const std::string& filename);
    void addCustomRule(const InterpretationRule& rule);
    void updateRuleWeights(const std::map<std::string, double>& weights);

    // Machine learning integration
    void trainFromFeedback(const std::vector<std::pair<std::string, double>>& feedback);
    void updateFromUserRatings(const std::map<std::string, int>& ratings);
    void improvePredictions(const std::vector<std::string>& actualOutcomes);

    // Quality assurance
    bool validateInterpretation() const;
    std::vector<std::string> checkConsistency() const;
    std::vector<std::string> identifyContradictions() const;
    double calculateReportQuality() const;
};

// Interpretation templates and knowledge base
class InterpretationKnowledgeBase {
private:
    std::map<std::string, std::vector<InterpretationRule>> rulesByCategory;
    std::map<std::string, std::vector<std::string>> templates;
    std::map<std::string, std::vector<std::string>> keywords;

public:
    InterpretationKnowledgeBase();

    void loadKnowledgeBase();
    void saveKnowledgeBase();

    std::vector<InterpretationRule> getRulesForCategory(const std::string& category) const;
    std::vector<std::string> getTemplatesForType(const std::string& type) const;
    std::vector<std::string> getKeywordsForConcept(const std::string& concept) const;

    void addRule(const InterpretationRule& rule);
    void addTemplate(const std::string& type, const std::string& templateText);
    void addKeywords(const std::string& concept, const std::vector<std::string>& keywords);

    void updateFromResearch(const std::string& researchData);
    void incorporateUserFeedback(const std::string& feedback);
    void refineRules(const std::vector<std::string>& refinements);
};

} // namespace Astro

#endif // AI_INTERPRETATION_H
