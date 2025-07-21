#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <string>
#include <map>
#include <vector>

namespace Astro {

enum class Language {
    ENGLISH,
    HINDI,
    SANSKRIT,
    SPANISH,
    FRENCH,
    GERMAN,
    ITALIAN,
    PORTUGUESE,
    RUSSIAN,
    CHINESE_SIMPLIFIED,
    CHINESE_TRADITIONAL,
    JAPANESE,
    KOREAN,
    ARABIC,
    THAI,
    BURMESE,
    TAMIL,
    TELUGU,
    BENGALI,
    GUJARATI,
    MARATHI,
    KANNADA,
    MALAYALAM,
    PUNJABI,
    URDU,
    NEPALI
};

struct LocalizedText {
    std::map<Language, std::string> translations;

    std::string get(Language lang = Language::ENGLISH) const;
    void set(Language lang, const std::string& text);
};

struct CulturalSettings {
    Language language;
    std::string country;
    std::string timezone;
    std::string dateFormat;      // DD/MM/YYYY, MM/DD/YYYY, YYYY-MM-DD
    std::string timeFormat;      // 12h, 24h
    std::string numberFormat;    // Decimal separator, thousands separator
    std::string currency;
    bool useNativeScript;        // Devanagari, Arabic script, etc.
    bool useLocalCalendar;       // Vikram Samvat, Buddhist calendar, etc.
    std::vector<std::string> preferredAstrologySystems; // Vedic, Western, Chinese, etc.
};

class Localization {
private:
    static Language currentLanguage;
    static std::map<std::string, LocalizedText> textDatabase;
    static CulturalSettings culturalSettings;

    // Text databases for different languages
    void loadEnglishTexts();
    void loadHindiTexts();
    void loadSanskritTexts();
    void loadSpanishTexts();
    void loadFrenchTexts();
    void loadGermanTexts();
    void loadItalianTexts();
    void loadPortugueseTexts();
    void loadRussianTexts();
    void loadChineseTexts();
    void loadJapaneseTexts();
    void loadKoreanTexts();
    void loadArabicTexts();
    void loadThaiTexts();
    void loadBurmeseTexts();
    void loadIndianLanguageTexts();

public:
    // Language management
    static void setLanguage(Language lang);
    static Language getLanguage();
    static std::vector<Language> getSupportedLanguages();
    static std::string getLanguageName(Language lang);
    static std::string getLanguageCode(Language lang);

    // Text retrieval
    static std::string getText(const std::string& key);
    static std::string getFormattedText(const std::string& key, const std::vector<std::string>& args);
    static bool hasTranslation(const std::string& key, Language lang);

    // Cultural settings
    static void setCulturalSettings(const CulturalSettings& settings);
    static CulturalSettings getCulturalSettings();
    static std::string formatDate(const std::string& date);
    static std::string formatTime(const std::string& time);
    static std::string formatNumber(double number);
    static std::string formatCurrency(double amount);

    // Astrological terms
    static std::string getPlanetName(Planet planet);
    static std::string getSignName(ZodiacSign sign);
    static std::string getHouseName(int house);
    static std::string getAspectName(AspectType aspect);
    static std::string getNakshatraName(int nakshatra);
    static std::string getTithiName(int tithi);
    static std::string getVaraName(int vara);
    static std::string getYogaName(int yoga);
    static std::string getKaranaName(int karana);
    static std::string getAyanamsaName(AyanamsaType ayanamsa);
    static std::string getHouseSystemName(HouseSystem system);

    // Calendar terms
    static std::string getMonthName(int month);
    static std::string getWeekdayName(int weekday);
    static std::string getSeasonName(int season);
    static std::string getFestivalName(const std::string& festival);

    // Script conversion
    static std::string convertToNativeScript(const std::string& text);
    static std::string convertFromNativeScript(const std::string& text);
    static bool supportsNativeScript(Language lang);

    // Number systems
    static std::string convertToLocalNumbers(const std::string& numbers);
    static std::string convertFromLocalNumbers(const std::string& numbers);

    // Regional preferences
    static std::vector<std::string> getRegionalCalendars(const std::string& region);
    static std::vector<std::string> getRegionalAstrologySystems(const std::string& region);
    static std::vector<std::string> getRegionalFestivals(const std::string& region);

    // Initialization
    static void initialize();
    static void loadFromFile(const std::string& filename);
    static void saveToFile(const std::string& filename);

    // Text keys (constants)
    struct Keys {
        // Common terms
        static constexpr const char* PLANET = "planet";
        static constexpr const char* SIGN = "sign";
        static constexpr const char* HOUSE = "house";
        static constexpr const char* ASPECT = "aspect";
        static constexpr const char* DEGREE = "degree";
        static constexpr const char* MINUTE = "minute";
        static constexpr const char* SECOND = "second";

        // Chart types
        static constexpr const char* BIRTH_CHART = "birth_chart";
        static constexpr const char* NATAL_CHART = "natal_chart";
        static constexpr const char* HOROSCOPE = "horoscope";
        static constexpr const char* TRANSIT_CHART = "transit_chart";
        static constexpr const char* PROGRESSION_CHART = "progression_chart";

        // Hindu calendar terms
        static constexpr const char* TITHI = "tithi";
        static constexpr const char* NAKSHATRA = "nakshatra";
        static constexpr const char* YOGA = "yoga";
        static constexpr const char* KARANA = "karana";
        static constexpr const char* VARA = "vara";
        static constexpr const char* PANCHANGA = "panchanga";
        static constexpr const char* VIKRAM_SAMVAT = "vikram_samvat";
        static constexpr const char* SHAKA_SAMVAT = "shaka_samvat";

        // Predictions
        static constexpr const char* DASHA = "dasha";
        static constexpr const char* ANTARDASHA = "antardasha";
        static constexpr const char* PRATYANTARDASHA = "pratyantardasha";
        static constexpr const char* PREDICTION = "prediction";
        static constexpr const char* FORECAST = "forecast";
        static constexpr const char* REMEDY = "remedy";

        // Time periods
        static constexpr const char* DAILY = "daily";
        static constexpr const char* WEEKLY = "weekly";
        static constexpr const char* MONTHLY = "monthly";
        static constexpr const char* YEARLY = "yearly";
        static constexpr const char* SUNRISE = "sunrise";
        static constexpr const char* SUNSET = "sunset";
        static constexpr const char* MOONRISE = "moonrise";
        static constexpr const char* MOONSET = "moonset";

        // Auspicious times
        static constexpr const char* BRAHMA_MUHURTA = "brahma_muhurta";
        static constexpr const char* ABHIJIT_MUHURTA = "abhijit_muhurta";
        static constexpr const char* RAHU_KAAL = "rahu_kaal";
        static constexpr const char* YAMAGANDA = "yamaganda";
        static constexpr const char* GULIKAI = "gulikai";

        // Festivals
        static constexpr const char* EKADASHI = "ekadashi";
        static constexpr const char* PURNIMA = "purnima";
        static constexpr const char* AMAVASYA = "amavasya";
        static constexpr const char* SANKRANTI = "sankranti";

        // Actions
        static constexpr const char* CALCULATE = "calculate";
        static constexpr const char* GENERATE = "generate";
        static constexpr const char* DISPLAY = "display";
        static constexpr const char* EXPORT = "export";
        static constexpr const char* SAVE = "save";
        static constexpr const char* LOAD = "load";

        // Status
        static constexpr const char* SUCCESS = "success";
        static constexpr const char* ERROR = "error";
        static constexpr const char* WARNING = "warning";
        static constexpr const char* PROCESSING = "processing";
        static constexpr const char* COMPLETED = "completed";

        // Help and documentation
        static constexpr const char* HELP = "help";
        static constexpr const char* USAGE = "usage";
        static constexpr const char* EXAMPLE = "example";
        static constexpr const char* DESCRIPTION = "description";
        static constexpr const char* OPTIONS = "options";
    };
};

// Utility macros for easy localization
#define _(key) Localization::getText(key)
#define _f(key, ...) Localization::getFormattedText(key, {__VA_ARGS__})

// Template for easy creation of localized text
template<typename... Args>
std::string formatLocalizedText(const std::string& key, Args... args) {
    std::vector<std::string> argVector;
    ((argVector.push_back(std::to_string(args))), ...);
    return Localization::getFormattedText(key, argVector);
}

} // namespace Astro

#endif // LOCALIZATION_H
