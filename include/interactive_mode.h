#ifndef INTERACTIVE_MODE_H
#define INTERACTIVE_MODE_H

#include "astro_types.h"
#include "birth_chart.h"
#include "advanced_astrology.h"
#include "predictive_astrology.h"
#include <string>
#include <vector>
#include <map>

namespace Astro {

struct UserProfile {
    std::string name;
    std::string birthDate;
    std::string birthTime;
    double latitude;
    double longitude;
    double timezone;
    std::string location;
    ZodiacMode preferredZodiac;
    AyanamsaType preferredAyanamsa;
    HouseSystem preferredHouseSystem;
    std::string language;
    std::vector<std::string> interests; // KP, Vedic, Western, Financial, etc.
    std::map<std::string, std::string> preferences;
};

struct InteractiveSession {
    UserProfile user;
    BirthChart currentChart;
    std::vector<std::string> commandHistory;
    std::map<std::string, std::string> sessionData;
    bool isActive;
    std::string currentMode; // basic, advanced, predictive, comparison
};

struct MenuOption {
    int id;
    std::string title;
    std::string description;
    std::string command;
    std::vector<std::string> requiredData;
    bool isAvailable;
};

class InteractiveMode {
private:
    InteractiveSession session;
    std::vector<MenuOption> mainMenu;
    std::vector<MenuOption> chartMenu;
    std::vector<MenuOption> predictionMenu;
    std::vector<MenuOption> analysisMenu;
    std::vector<MenuOption> settingsMenu;

    // Helper methods
    void initializeMenus();
    void displayWelcome();
    void displayMainMenu();
    void displaySubmenu(const std::vector<MenuOption>& menu, const std::string& title);
    std::string getUserInput(const std::string& prompt);
    bool validateInput(const std::string& input, const std::string& type);
    void saveSession();
    void loadSession();
    void showProgress(const std::string& operation);

public:
    InteractiveMode();
    ~InteractiveMode();

    // Main interactive functions
    void startInteractiveMode();
    void createUserProfile();
    void loadUserProfile(const std::string& filename);
    void saveUserProfile(const std::string& filename);

    // Interactive menus
    void showMainMenu();
    void showChartGenerationMenu();
    void showPredictiveMenu();
    void showAdvancedAnalysisMenu();
    void showComparisonMenu();
    void showToolsMenu();
    void showSettingsMenu();
    void showHelpMenu();

    // Chart operations
    void interactiveChartGeneration();
    void interactiveProgressions();
    void interactiveTransits();
    void interactiveReturns();
    void interactiveHarmonics();
    void interactiveSynastry();

    // Prediction operations
    void interactiveDashaAnalysis();
    void interactiveYearlyForecast();
    void interactiveMonthlyForecast();
    void interactiveEventPrediction();
    void interactiveTimingAnalysis();
    void interactiveRemedialAnalysis();

    // Data management
    void interactiveDataEntry();
    void validateBirthData();
    void searchLocation();
    void calculateTimezone();
    void adjustForDST();

    // Advanced features
    void interactiveElectionalAstrology();
    void interactiveMundaneAstrology();
    void interactiveFinancialAstrology();
    void interactiveMedicalAstrology();
    void interactiveRelocationAstrology();

    // Utilities
    void showCalculationProgress();
    void displayResults(const std::string& results);
    void exportResults(const std::string& format);
    void printToFile(const std::string& content);
    void emailResults();

    // Educational features
    void showTutorial();
    void explainConcepts(const std::string& topic);
    void showExamples();
    void recommendLearning();

    // Session management
    void pauseSession();
    void resumeSession();
    void clearSession();
    void showSessionHistory();
    void undoLastOperation();

    // Configuration
    void configureSettings();
    void setLanguage(const std::string& language);
    void setOutputFormat(const std::string& format);
    void setCalculationPrecision(const std::string& precision);
    void manageProfiles();

    // Batch operations
    void batchChartGeneration();
    void batchPredictionAnalysis();
    void batchCompatibilityAnalysis();
    void batchElectionalAnalysis();

    // Integration features
    void connectToAPI();
    void exportToCalendar();
    void importFromFile();
    void syncWithCloud();

    // Wizard modes
    void beginnerWizard();
    void advancedWizard();
    void professionalWizard();
    void quickAnalysisWizard();

    // Help and support
    void showContextHelp();
    void showTroubleshooting();
    void showFrequentlyAskedQuestions();
    void submitFeedback();

    // Command processing
    bool processCommand(const std::string& command);
    std::vector<std::string> parseCommand(const std::string& command);
    void executeCommand(const std::vector<std::string>& args);

    // Output formatting
    void formatOutput(const std::string& content, const std::string& format);
    void displayTable(const std::vector<std::vector<std::string>>& data, const std::vector<std::string>& headers);
    void displayChart(const std::string& chartData);
    void displayGraph(const std::vector<double>& data, const std::string& title);

    // Error handling
    void handleError(const std::string& error);
    void showErrorHelp(const std::string& errorType);
    void suggestAlternatives(const std::string& failedCommand);
};

// Configuration Manager
class ConfigurationManager {
private:
    std::map<std::string, std::string> settings;
    std::string configFile;

public:
    ConfigurationManager(const std::string& configPath = "config.ini");

    void loadConfiguration();
    void saveConfiguration();
    void setDefault(const std::string& key, const std::string& value);
    std::string get(const std::string& key, const std::string& defaultValue = "");
    void set(const std::string& key, const std::string& value);
    bool getBool(const std::string& key, bool defaultValue = false);
    int getInt(const std::string& key, int defaultValue = 0);
    double getDouble(const std::string& key, double defaultValue = 0.0);

    void resetToDefaults();
    std::vector<std::string> getKeys();
    void removeKey(const std::string& key);
    bool hasKey(const std::string& key);

    // Specialized getters
    std::string getDataPath();
    std::string getOutputFormat();
    std::string getLanguage();
    ZodiacMode getZodiacMode();
    AyanamsaType getAyanamsa();
    HouseSystem getHouseSystem();

    // Specialized setters
    void setDataPath(const std::string& path);
    void setOutputFormat(const std::string& format);
    void setLanguage(const std::string& language);
    void setZodiacMode(ZodiacMode mode);
    void setAyanamsa(AyanamsaType ayanamsa);
    void setHouseSystem(HouseSystem system);
};

// Command Line Interface Enhancement
class EnhancedCLI {
private:
    ConfigurationManager config;
    std::vector<std::string> commandHistory;

public:
    EnhancedCLI();

    // Auto-completion
    std::vector<std::string> getCompletions(const std::string& partial);
    std::string completeCommand(const std::string& partial);

    // Command history
    void addToHistory(const std::string& command);
    std::vector<std::string> getHistory();
    std::string getLastCommand();

    // Input validation
    bool validateDate(const std::string& date);
    bool validateTime(const std::string& time);
    bool validateCoordinates(double lat, double lon);
    bool validateTimezone(double tz);

    // Helpful suggestions
    std::vector<std::string> suggestCommands(const std::string& context);
    std::vector<std::string> suggestOptions(const std::string& command);
    std::string suggestCorrection(const std::string& wrongCommand);

    // Progress indicators
    void showProgressBar(int percentage);
    void showSpinner(const std::string& message);
    void showCountdown(int seconds, const std::string& message);

    // Enhanced output
    void printColorized(const std::string& text, const std::string& color);
    void printTable(const std::vector<std::vector<std::string>>& data, const std::vector<std::string>& headers);
    void printBox(const std::string& content, const std::string& title = "");
    void printSeparator(int length = 80, char character = '=');
};

} // namespace Astro

#endif // INTERACTIVE_MODE_H
