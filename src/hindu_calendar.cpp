#include "hindu_calendar.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "myanmar_calendar.h"
#include "astro_types.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <map>
#include <ctime>

extern "C" {
#include "swephexp.h"
}

namespace Astro {

HinduCalendar::HinduCalendar() :
    ayanamsa(AyanamsaType::LAHIRI),
    calculationMethod(CalculationMethod::DRIK_SIDDHANTA),
    calendarSystem(CalendarSystem::LUNI_SOLAR),
    useModernCalculations(true),
    initialized(false) {
}

HinduCalendar::HinduCalendar(AyanamsaType ayanamsa, CalculationMethod method, CalendarSystem system)
    : ayanamsa(ayanamsa), calculationMethod(method),
      calendarSystem(system), useModernCalculations(true), initialized(false) {
}

HinduCalendar::~HinduCalendar() {
}

bool HinduCalendar::initialize() {
    try {
        // Set the ayanamsa for Swiss Ephemeris
        swe_set_sid_mode(getSweAyanamsaId(), 0, 0);

        initializeNakshatraData();
        initializeTithiData();
        initializeYogaData();
        initializeKaranaData();
        initializeFestivalData();

        initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError = "Failed to initialize Hindu Calendar: " + std::string(e.what());
        return false;
    }
}

bool HinduCalendar::initialize(AyanamsaType ayanamsaType, CalculationMethod method) {
    ayanamsa = ayanamsaType;
    calculationMethod = method;
    return initialize();
}

void HinduCalendar::initializeNakshatraData() {
    nakshatraData = {
        {"Ashwini", "अश्विनी", Planet::SOUTH_NODE, "Horse's head", "Ashwini Kumaras", 0.0, 13.333333, "Dev", "Dev", "Swift"},
        {"Bharani", "भरणी", Planet::VENUS, "Yoni", "Yama", 13.333333, 26.666667, "Manush", "Manush", "Fierce"},
        {"Krittika", "कृत्तिका", Planet::SUN, "Razor/Knife", "Agni", 26.666667, 40.0, "Rakshasa", "Rakshasa", "Mixed"},
        {"Rohini", "रोहिणी", Planet::MOON, "Cart/Chariot", "Brahma", 40.0, 53.333333, "Manush", "Manush", "Fixed"},
        {"Mrigashira", "मृगशिरा", Planet::MARS, "Deer's head", "Soma", 53.333333, 66.666667, "Dev", "Dev", "Soft"},
        {"Ardra", "आर्द्रा", Planet::NORTH_NODE, "Teardrop", "Rudra", 66.666667, 80.0, "Manush", "Manush", "Sharp"},
        {"Punarvasu", "पुनर्वसु", Planet::JUPITER, "Quiver of arrows", "Aditi", 80.0, 93.333333, "Dev", "Dev", "Moveable"},
        {"Pushya", "पुष्य", Planet::SATURN, "Cow's udder", "Brihaspati", 93.333333, 106.666667, "Dev", "Dev", "Light"},
        {"Ashlesha", "आश्लेषा", Planet::MERCURY, "Serpent", "Sarpa", 106.666667, 120.0, "Rakshasa", "Rakshasa", "Sharp"},
        {"Magha", "मघा", Planet::SOUTH_NODE, "Royal throne", "Pitrs", 120.0, 133.333333, "Rakshasa", "Rakshasa", "Fierce"},
        {"Purva Phalguni", "पूर्व फाल्गुनी", Planet::VENUS, "Front legs of bed", "Bhaga", 133.333333, 146.666667, "Manush", "Manush", "Fierce"},
        {"Uttara Phalguni", "उत्तर फाल्गुनी", Planet::SUN, "Back legs of bed", "Aryaman", 146.666667, 160.0, "Manush", "Manush", "Fixed"},
        {"Hasta", "हस्त", Planet::MOON, "Hand", "Savitar", 160.0, 173.333333, "Dev", "Dev", "Light"},
        {"Chitra", "चित्रा", Planet::MARS, "Bright jewel", "Vishvakarma", 173.333333, 186.666667, "Rakshasa", "Rakshasa", "Soft"},
        {"Swati", "स्वाति", Planet::NORTH_NODE, "Young shoot", "Vayu", 186.666667, 200.0, "Dev", "Dev", "Moveable"},
        {"Vishakha", "विशाखा", Planet::JUPITER, "Triumphal arch", "Indragni", 200.0, 213.333333, "Rakshasa", "Rakshasa", "Mixed"},
        {"Anuradha", "अनुराधा", Planet::SATURN, "Lotus", "Mitra", 213.333333, 226.666667, "Dev", "Dev", "Soft"},
        {"Jyeshtha", "ज्येष्ठा", Planet::MERCURY, "Earring", "Indra", 226.666667, 240.0, "Rakshasa", "Rakshasa", "Sharp"},
        {"Mula", "मूल", Planet::SOUTH_NODE, "Bunch of roots", "Nirriti", 240.0, 253.333333, "Rakshasa", "Rakshasa", "Sharp"},
        {"Purva Ashadha", "पूर्वाषाढ़ा", Planet::VENUS, "Elephant tusk", "Apas", 253.333333, 266.666667, "Manush", "Manush", "Fierce"},
        {"Uttara Ashadha", "उत्तराषाढ़ा", Planet::SUN, "Elephant tusk", "Vishvedevas", 266.666667, 280.0, "Manush", "Manush", "Fixed"},
        {"Shravana", "श्रवण", Planet::MOON, "Ear", "Vishnu", 280.0, 293.333333, "Dev", "Dev", "Moveable"},
        {"Dhanishta", "धनिष्ठा", Planet::MARS, "Drum", "Vasus", 293.333333, 306.666667, "Rakshasa", "Rakshasa", "Moveable"},
        {"Shatabhisha", "शतभिषा", Planet::NORTH_NODE, "Empty circle", "Varuna", 306.666667, 320.0, "Rakshasa", "Rakshasa", "Moveable"},
        {"Purva Bhadrapada", "पूर्व भाद्रपदा", Planet::JUPITER, "Sword", "Aja Ekapada", 320.0, 333.333333, "Manush", "Manush", "Fierce"},
        {"Uttara Bhadrapada", "उत्तर भाद्रपदा", Planet::SATURN, "Twins", "Ahir Budhnya", 333.333333, 346.666667, "Manush", "Manush", "Fixed"},
        {"Revati", "रेवती", Planet::MERCURY, "Fish", "Pushan", 346.666667, 360.0, "Dev", "Dev", "Soft"}
    };
}

void HinduCalendar::initializeTithiData() {
    tithiData = {
        {"Pratipad", "प्रतिपदा", Planet::SUN, "Beginning", true, "New starts, worship"},
        {"Dwitiya", "द्वितीया", Planet::MOON, "Second", true, "Growth, learning"},
        {"Tritiya", "तृतीया", Planet::MARS, "Third", true, "Action, energy"},
        {"Chaturthi", "चतुर्थी", Planet::MERCURY, "Fourth", true, "Intelligence, obstacles removal"},
        {"Panchami", "पंचमी", Planet::JUPITER, "Fifth", true, "Knowledge, wisdom"},
        {"Shashthi", "षष्ठी", Planet::VENUS, "Sixth", true, "Love, beauty, comfort"},
        {"Saptami", "सप्तमी", Planet::SATURN, "Seventh", true, "Discipline, hard work"},
        {"Ashtami", "अष्टमी", Planet::NORTH_NODE, "Eighth", false, "Obstacles, power"},
        {"Navami", "नवमी", Planet::SUN, "Ninth", true, "Completion of tasks"},
        {"Dashami", "दशमी", Planet::MOON, "Tenth", true, "Success in endeavors"},
        {"Ekadashi", "एकादशी", Planet::MARS, "Eleventh", true, "Fasting, spirituality"},
        {"Dwadashi", "द्वादशी", Planet::MERCURY, "Twelfth", true, "Communication, travel"},
        {"Trayodashi", "त्रयोदशी", Planet::JUPITER, "Thirteenth", true, "Learning, teaching"},
        {"Chaturdashi", "चतुर्दशी", Planet::VENUS, "Fourteenth", true, "Devotion, prayers"},
        {"Purnima", "पूर्णिमा", Planet::SATURN, "Full Moon", true, "Completion, fulfillment"}
    };
}

void HinduCalendar::initializeYogaData() {
    yogaData = {
        {"Vishkumbha", "विष्कम्भ", "Inauspicious", "Obstacles"},
        {"Priti", "प्रीति", "Auspicious", "Love, affection"},
        {"Ayushman", "आयुष्मान्", "Very Auspicious", "Longevity"},
        {"Saubhagya", "सौभाग्य", "Auspicious", "Good fortune"},
        {"Shobhana", "शोभन", "Very Auspicious", "Beauty, prosperity"},
        {"Atiganda", "अतिगण्ड", "Inauspicious", "Great obstacles"},
        {"Sukarman", "सुकर्म", "Auspicious", "Good deeds"},
        {"Dhriti", "धृति", "Auspicious", "Steadfastness"},
        {"Shula", "शूल", "Very Inauspicious", "Pain, suffering"},
        {"Ganda", "गण्ड", "Inauspicious", "Obstacles"},
        {"Vriddhi", "वृद्धि", "Auspicious", "Growth, increase"},
        {"Dhruva", "ध्रुव", "Very Auspicious", "Stability"},
        {"Vyaghata", "व्याघात", "Inauspicious", "Accidents"},
        {"Harshana", "हर्षण", "Auspicious", "Joy, happiness"},
        {"Vajra", "वज्र", "Inauspicious", "Hard like diamond"},
        {"Siddhi", "सिद्धि", "Very Auspicious", "Success, achievement"},
        {"Vyatipata", "व्यतीपात", "Very Inauspicious", "Calamity"},
        {"Variyan", "वरीयान्", "Auspicious", "Excellent"},
        {"Parigha", "परिघ", "Inauspicious", "Iron club"},
        {"Shiva", "शिव", "Very Auspicious", "Auspicious"},
        {"Siddha", "सिद्ध", "Very Auspicious", "Accomplished"},
        {"Sadhya", "साध्य", "Auspicious", "Achievable"},
        {"Shubha", "शुभ", "Very Auspicious", "Auspicious"},
        {"Shukla", "शुक्ल", "Auspicious", "Bright, pure"},
        {"Brahma", "ब्रह्म", "Very Auspicious", "Sacred"},
        {"Indra", "इन्द्र", "Auspicious", "Powerful"},
        {"Vaidhriti", "वैधृति", "Very Inauspicious", "Separation"}
    };
}

void HinduCalendar::initializeKaranaData() {
    karanaData = {
        {"Bava", "बव", "Movable", true},
        {"Balava", "बालव", "Movable", true},
        {"Kaulava", "कौलव", "Movable", true},
        {"Taitila", "तैतिल", "Movable", true},
        {"Gara", "गर", "Movable", true},
        {"Vanija", "वाणिज", "Movable", true},
        {"Vishti", "विष्टि", "Movable", true},
        {"Shakuni", "शकुनि", "Fixed", false},
        {"Chatushpada", "चतुष्पद", "Fixed", false},
        {"Naga", "नाग", "Fixed", false},
        {"Kimstughna", "किंस्तुघ्न", "Fixed", false}
    };
}

void HinduCalendar::initializeFestivalData() {
    // Major Hindu festivals mapped to Tithi and month
    festivalMap["Chaitra_Purnima"] = {"Hanuman Jayanti", "Chaitanya Mahaprabhu Jayanti"};
    festivalMap["Vaishakha_Purnima"] = {"Buddha Purnima", "Kurma Jayanti"};
    festivalMap["Jyeshtha_Purnima"] = {"Vat Purnima", "Savitri Vrata"};
    festivalMap["Ashadha_Purnima"] = {"Guru Purnima", "Vyasa Purnima"};
    festivalMap["Shravana_Purnima"] = {"Raksha Bandhan", "Hayagriva Jayanti"};
    festivalMap["Bhadrapada_Purnima"] = {"Madhu Purnima"};
    festivalMap["Ashwini_Purnima"] = {"Sharad Purnima", "Kojagari Purnima"};
    festivalMap["Kartika_Purnima"] = {"Kartika Purnima", "Dev Diwali"};
    festivalMap["Margashirsha_Purnima"] = {"Dattatreya Jayanti"};
    festivalMap["Pausha_Purnima"] = {"Shakambhari Purnima"};
    festivalMap["Magha_Purnima"] = {"Maghi Purnima"};
    festivalMap["Phalguna_Purnima"] = {"Holi", "Holika Dahan"};

    // Ekadashi festivals
    festivalMap["Ekadashi"] = {"Ekadashi Vrata", "Hari Vasara"};

    // Amavasya festivals
    festivalMap["Chaitra_Amavasya"] = {"Chaitra Amavasya"};
    festivalMap["Kartika_Amavasya"] = {"Diwali", "Kali Chaudas"};
    festivalMap["Magha_Amavasya"] = {"Mauni Amavasya"};
}

PanchangaData HinduCalendar::calculatePanchanga(const BirthData& birthData) const {
    if (!initialized) {
        PanchangaData empty = {};
        return empty;
    }

    return calculatePanchanga(birthData.getJulianDay(), birthData.latitude, birthData.longitude);
}

PanchangaData HinduCalendar::calculatePanchanga(double julianDay, double latitude, double longitude) const {
    PanchangaData panchanga = {};

    if (!initialized) {
        return panchanga;
    }

    try {
        // Calculate planetary positions for the given JD
        double sunPos[6], moonPos[6];
        char errorString[256];

        // Get Sun position
        int result = swe_calc(julianDay, SE_SUN, SEFLG_SIDEREAL | SEFLG_SPEED, sunPos, errorString);
        if (result < 0) {
            lastError = "Failed to calculate Sun position: " + std::string(errorString);
            return panchanga;
        }

        // Get Moon position
        result = swe_calc(julianDay, SE_MOON, SEFLG_SIDEREAL | SEFLG_SPEED, moonPos, errorString);
        if (result < 0) {
            lastError = "Failed to calculate Moon position: " + std::string(errorString);
            return panchanga;
        }

        double sunLongitude = sunPos[0];
        double moonLongitude = moonPos[0];
        double sunSpeed = sunPos[3];
        double moonSpeed = moonPos[3];

        // Calculate lunar phase
        double lunarPhase = calculateLunarPhase(sunLongitude, moonLongitude);
        panchanga.lunarPhase = lunarPhase;
        panchanga.sunLongitude = sunLongitude;
        panchanga.moonLongitude = moonLongitude;

        // Calculate the five main elements
        panchanga.tithi = calculateTithi(lunarPhase);
        panchanga.vara = calculateVara(julianDay);
        panchanga.nakshatra = calculateNakshatra(moonLongitude);
        panchanga.yoga = calculateYoga(sunLongitude, moonLongitude);
        panchanga.karana = calculateKarana(lunarPhase, true); // Assume first half for now

        // Calculate zodiac signs
        panchanga.sunRashi = calculateRashi(sunLongitude);
        panchanga.moonRashi = calculateRashi(moonLongitude);

        // Calculate Hindu month and year
        panchanga.month = calculateHinduMonth(sunLongitude);
        panchanga.year = calculateVikramYear(julianDay);

        // Determine paksha (fortnight)
        panchanga.isShukla = (static_cast<int>(panchanga.tithi) <= 15);
        panchanga.isKrishna = !panchanga.isShukla;

        // Calculate end times
        panchanga.tithiEndTime = calculateTithiEndTime(lunarPhase, sunSpeed, moonSpeed);
        panchanga.nakshatraEndTime = calculateNakshatraEndTime(moonLongitude, moonSpeed);
        panchanga.yogaEndTime = calculateYogaEndTime(sunLongitude, moonLongitude, sunSpeed, moonSpeed);
        panchanga.karanaEndTime = calculateKaranaEndTime(lunarPhase, sunSpeed, moonSpeed);

        // Special day identification
        panchanga.isEkadashi = (panchanga.tithi == Tithi::EKADASHI || panchanga.tithi == Tithi::EKADASHI_K);
        panchanga.isPurnima = (panchanga.tithi == Tithi::PURNIMA);
        panchanga.isAmavasya = (panchanga.tithi == Tithi::AMAVASYA);

        // Identify festivals and special events
        identifyFestivals(panchanga);
        identifySpecialEvents(panchanga);

        // Calculate muhurta
        calculateMuhurta(panchanga);

    } catch (const std::exception& e) {
        lastError = "Error calculating Panchanga: " + std::string(e.what());
    }

    return panchanga;
}

double HinduCalendar::calculateLunarPhase(double sunLong, double moonLong) const {
    double phase = moonLong - sunLong;
    if (phase < 0) phase += 360.0;
    return phase;
}

Tithi HinduCalendar::calculateTithi(double lunarPhase) const {
    // Each tithi spans 12 degrees
    int tithiNumber = static_cast<int>(lunarPhase / 12.0) + 1;
    if (tithiNumber > 30) tithiNumber = 30;
    return static_cast<Tithi>(tithiNumber);
}

HinduNakshatra HinduCalendar::calculateNakshatra(double moonLongitude) const {
    // Each nakshatra spans 13°20' (800 minutes)
    int nakshatraNumber = static_cast<int>(moonLongitude / NAKSHATRA_SPAN) + 1;
    if (nakshatraNumber > 27) nakshatraNumber = 27;
    return static_cast<HinduNakshatra>(nakshatraNumber);
}

Yoga HinduCalendar::calculateYoga(double sunLong, double moonLong) const {
    double yogaLongitude = sunLong + moonLong;
    if (yogaLongitude >= 360.0) yogaLongitude -= 360.0;

    int yogaNumber = static_cast<int>(yogaLongitude / YOGA_SPAN) + 1;
    if (yogaNumber > 27) yogaNumber = 27;
    return static_cast<Yoga>(yogaNumber);
}

Karana HinduCalendar::calculateKarana(double lunarPhase, bool isFirstHalf) const {
    // Each tithi has 2 karanas, each spanning 6 degrees
    int karanaNumber = static_cast<int>(lunarPhase / 6.0) + 1;

    // Handle special karanas for the last 4 half-days of the lunar month
    if (karanaNumber >= 57) {
        switch (karanaNumber - 56) {
            case 1: return Karana::SHAKUNI;
            case 2: return Karana::CHATUSHPADA;
            case 3: return Karana::NAGA;
            case 4: return Karana::KIMSTUGHNA;
            default: return Karana::BAVA;
        }
    }

    // Movable karanas cycle through 1-7
    int movableKarana = ((karanaNumber - 1) % 7) + 1;
    return static_cast<Karana>(movableKarana);
}

Vara HinduCalendar::calculateVara(double julianDay) const {
    // Sunday = JD modulo 7 == 1
    int vara = static_cast<int>(std::floor(julianDay + 1.5)) % 7;
    return static_cast<Vara>(vara);
}

HinduMonth HinduCalendar::calculateHinduMonth(double sunLongitude) const {
    // Chaitra starts when Sun enters Aries (0 degrees)
    int monthNumber = static_cast<int>(sunLongitude / 30.0) + 1;
    if (monthNumber > 12) monthNumber = 12;
    return static_cast<HinduMonth>(monthNumber);
}

Rashi HinduCalendar::calculateRashi(double longitude) const {
    int rashiNumber = static_cast<int>(longitude / 30.0) + 1;
    if (rashiNumber > 12) rashiNumber = 12;
    return static_cast<Rashi>(rashiNumber);
}

double HinduCalendar::calculateTithiEndTime(double currentPhase, double sunSpeed, double moonSpeed) const {
    // Calculate when current tithi ends
    int currentTithi = static_cast<int>(currentPhase / 12.0);
    double nextTithiStart = (currentTithi + 1) * 12.0;
    double remainingDegrees = nextTithiStart - currentPhase;

    // Rate of phase change per day
    double phaseSpeed = moonSpeed - sunSpeed; // degrees per day
    if (phaseSpeed <= 0) return 24.0; // Error condition

    double remainingHours = (remainingDegrees / phaseSpeed) * 24.0;
    return remainingHours;
}

double HinduCalendar::calculateNakshatraEndTime(double moonLong, double moonSpeed) const {
    int currentNak = static_cast<int>(moonLong / NAKSHATRA_SPAN);
    double nextNakStart = (currentNak + 1) * NAKSHATRA_SPAN;
    double remainingDegrees = nextNakStart - moonLong;

    if (moonSpeed <= 0) return 24.0;

    double remainingHours = (remainingDegrees / moonSpeed) * 24.0;
    return remainingHours;
}

double HinduCalendar::calculateYogaEndTime(double sunLong, double moonLong, double sunSpeed, double moonSpeed) const {
    double yogaLong = sunLong + moonLong;
    if (yogaLong >= 360.0) yogaLong -= 360.0;

    int currentYoga = static_cast<int>(yogaLong / YOGA_SPAN);
    double nextYogaStart = (currentYoga + 1) * YOGA_SPAN;
    double remainingDegrees = nextYogaStart - yogaLong;

    double yogaSpeed = sunSpeed + moonSpeed; // degrees per day
    if (yogaSpeed <= 0) return 24.0;

    double remainingHours = (remainingDegrees / yogaSpeed) * 24.0;
    return remainingHours;
}

double HinduCalendar::calculateKaranaEndTime(double currentPhase, double sunSpeed, double moonSpeed) const {
    int currentKarana = static_cast<int>(currentPhase / 6.0);
    double nextKaranaStart = (currentKarana + 1) * 6.0;
    double remainingDegrees = nextKaranaStart - currentPhase;

    double phaseSpeed = moonSpeed - sunSpeed;
    if (phaseSpeed <= 0) return 24.0;

    double remainingHours = (remainingDegrees / phaseSpeed) * 24.0;
    return remainingHours;
}

int HinduCalendar::calculateVikramYear(double julianDay) const {
    // Vikram Samvat epoch (57 BC)
    int year = static_cast<int>((julianDay - VIKRAM_EPOCH_JD) / SIDEREAL_YEAR) + 1;
    return year;
}

int HinduCalendar::calculateShakaYear(double julianDay) const {
    // Shaka Samvat epoch (79 AD)
    int year = static_cast<int>((julianDay - SHAKA_EPOCH_JD) / SIDEREAL_YEAR) + 1;
    return year;
}

int HinduCalendar::calculateKaliYear(double julianDay) const {
    // Kali Yuga epoch (3102 BC)
    int year = static_cast<int>((julianDay - KALI_EPOCH_JD) / SIDEREAL_YEAR) + 1;
    return year;
}

void HinduCalendar::setAyanamsa(AyanamsaType type) {
    ayanamsa = type;
    if (initialized) {
        swe_set_sid_mode(getSweAyanamsaId(), 0, 0);
    }
}

void HinduCalendar::setCalculationMethod(CalculationMethod method) {
    calculationMethod = method;
}

void HinduCalendar::setCalendarSystem(CalendarSystem system) {
    calendarSystem = system;
}

int HinduCalendar::getSweAyanamsaId() const {
    switch (ayanamsa) {
        case AyanamsaType::LAHIRI: return SE_SIDM_LAHIRI;
        case AyanamsaType::RAMAN: return SE_SIDM_RAMAN;
        case AyanamsaType::KRISHNAMURTI: return SE_SIDM_KRISHNAMURTI;
        case AyanamsaType::YUKTESHWAR: return SE_SIDM_YUKTESHWAR;
        case AyanamsaType::JN_BHASIN: return SE_SIDM_JN_BHASIN;
        case AyanamsaType::SASSANIAN: return SE_SIDM_SASSANIAN;
        case AyanamsaType::GALACTIC_CENTER: return SE_SIDM_GALCENT_0SAG;
        default: return SE_SIDM_LAHIRI;
    }
}

double HinduCalendar::getAyanamsaValue(double julianDay) const {
    return swe_get_ayanamsa_ut(julianDay);
}

std::string HinduCalendar::getAyanamsaName() const {
    switch (ayanamsa) {
        case AyanamsaType::LAHIRI: return "Lahiri/Chitrapaksha";
        case AyanamsaType::RAMAN: return "B.V. Raman";
        case AyanamsaType::KRISHNAMURTI: return "K.S. Krishnamurti";
        case AyanamsaType::YUKTESHWAR: return "Sri Yukteshwar";
        case AyanamsaType::JN_BHASIN: return "J.N. Bhasin";
        case AyanamsaType::SASSANIAN: return "Sassanian";
        case AyanamsaType::GALACTIC_CENTER: return "Galactic Center";
        case AyanamsaType::J2000: return "J2000";
        case AyanamsaType::J1900: return "J1900";
        case AyanamsaType::B1950: return "B1950";
        default: return "Unknown";
    }
}

std::string HinduCalendar::getCalculationMethodName() const {
    switch (calculationMethod) {
        case CalculationMethod::DRIK_SIDDHANTA: return "Drik Siddhanta (Modern Astronomical)";
        case CalculationMethod::SURYA_SIDDHANTA: return "Surya Siddhanta";
        case CalculationMethod::ARYA_SIDDHANTA: return "Arya Siddhanta";
        case CalculationMethod::BRAHMA_SIDDHANTA: return "Brahma Siddhanta";
        case CalculationMethod::MODERN_MIXED: return "Modern Mixed Approach";
        default: return "Unknown";
    }
}

double HinduCalendar::hinduDateToJulianDay(int year, int month, int day, bool isKrishna) const {
    // Convert Hindu date to Julian Day
    // This is a simplified conversion - more complex algorithms exist
    double baseJD = VIKRAM_EPOCH_JD + (year - 1) * SIDEREAL_YEAR;
    double monthDays = (month - 1) * 30.0; // Approximate month length
    double dayOffset = day;

    if (isKrishna) {
        dayOffset += 15.0; // Krishna paksha starts after day 15
    }

    return baseJD + monthDays + dayOffset;
}

void HinduCalendar::julianDayToHinduDate(double jd, int& year, int& month, int& day, bool& isKrishna) const {
    // Convert Julian Day to Hindu date
    double daysSinceEpoch = jd - VIKRAM_EPOCH_JD;
    year = static_cast<int>(daysSinceEpoch / SIDEREAL_YEAR) + 1;

    double remainingDays = std::fmod(daysSinceEpoch, SIDEREAL_YEAR);
    month = static_cast<int>(remainingDays / 30.0) + 1;

    double dayInMonth = std::fmod(remainingDays, 30.0);
    if (dayInMonth > 15) {
        isKrishna = true;
        day = static_cast<int>(dayInMonth - 15);
    } else {
        isKrishna = false;
        day = static_cast<int>(dayInMonth);
    }

    if (day == 0) day = 1; // Ensure valid day
}

void HinduCalendar::julianDayToGregorianDate(double jd, int& year, int& month, int& day) const {
    swe_jdut1_to_utc(jd, SE_GREG_CAL, &year, &month, &day, nullptr, nullptr, nullptr);
}

double HinduCalendar::gregorianDateToJulianDay(int year, int month, int day, double hour) const {
    return swe_julday(year, month, day, hour, SE_GREG_CAL);
}

std::string HinduCalendar::hinduDateToGregorian(int hinduYear, int hinduMonth, int hinduDay, bool isKrishna) const {
    double jd = hinduDateToJulianDay(hinduYear, hinduMonth, hinduDay, isKrishna);
    int year, month, day;
    julianDayToGregorianDate(jd, year, month, day);

    std::ostringstream oss;
    oss << year << "-";
    if (month < 10) oss << "0";
    oss << month << "-";
    if (day < 10) oss << "0";
    oss << day;
    return oss.str();
}

std::string HinduCalendar::gregorianDateToHindu(int gregYear, int gregMonth, int gregDay) const {
    double jd = gregorianDateToJulianDay(gregYear, gregMonth, gregDay);
    int year, month, day;
    bool isKrishna;
    julianDayToHinduDate(jd, year, month, day, isKrishna);

    std::ostringstream oss;
    oss << getHinduMonthName(static_cast<HinduMonth>(month)) << " " << day
        << " (" << (isKrishna ? "Krishna" : "Shukla") << "), " << year << " VS";
    return oss.str();
}

void HinduCalendar::identifyFestivals(PanchangaData& panchanga) const {
    // Check for major festivals based on tithi and month
    std::string monthName = getHinduMonthName(panchanga.month);

    if (panchanga.isPurnima) {
        std::string key = monthName + "_Purnima";
        auto it = festivalMap.find(key);
        if (it != festivalMap.end()) {
            panchanga.festivals = it->second;
        }
    }

    if (panchanga.isAmavasya) {
        std::string key = monthName + "_Amavasya";
        auto it = festivalMap.find(key);
        if (it != festivalMap.end()) {
            panchanga.festivals = it->second;
        }
    }

    if (panchanga.isEkadashi) {
        panchanga.festivals.push_back("Ekadashi Vrata");
    }
}

void HinduCalendar::identifySpecialEvents(PanchangaData& panchanga) const {
    // Check for solar transitions (Sankranti)
    if (std::fmod(panchanga.sunLongitude, 30.0) < 1.0) {
        panchanga.isSankranti = true;
        panchanga.specialEvents.push_back("Sankranti - " + getRashiName(panchanga.sunRashi));
    }

    // Add other special astronomical events
    if (panchanga.yoga == Yoga::VYATIPATA || panchanga.yoga == Yoga::VAIDHRITI) {
        panchanga.specialEvents.push_back("Inauspicious Yoga: " + getYogaName(panchanga.yoga));
    }

    if (panchanga.yoga == Yoga::SIDDHI || panchanga.yoga == Yoga::SHIVA || panchanga.yoga == Yoga::SIDDHA) {
        panchanga.specialEvents.push_back("Very Auspicious Yoga: " + getYogaName(panchanga.yoga));
    }
}

void HinduCalendar::calculateMuhurta(PanchangaData& panchanga) const {
    // Simple muhurta calculation based on tithi, nakshatra, and yoga
    bool isTithiGood = (static_cast<int>(panchanga.tithi) % 7 != 4) && (static_cast<int>(panchanga.tithi) % 7 != 9);
    bool isNakshatraGood = (static_cast<int>(panchanga.nakshatra) % 9 != 8);
    bool isYogaGood = (panchanga.yoga != Yoga::VYATIPATA && panchanga.yoga != Yoga::VAIDHRITI &&
                       panchanga.yoga != Yoga::SHULA && panchanga.yoga != Yoga::GANDA);

    panchanga.isShubhaMuhurta = (isTithiGood && isNakshatraGood && isYogaGood);
    panchanga.isAshubhaMuhurta = !panchanga.isShubhaMuhurta;

    if (panchanga.isShubhaMuhurta) {
        panchanga.muhurtaDescription = "Generally auspicious time for most activities";
    } else {
        panchanga.muhurtaDescription = "Exercise caution; consult detailed muhurta for important activities";
    }
}

// Formatting methods
std::string PanchangaData::getFormattedTithi() const {
    return "Tithi: " + std::to_string(static_cast<int>(tithi)) + " (" +
           (isKrishna ? "Krishna Paksha" : "Shukla Paksha") + ")";
}

std::string PanchangaData::getFormattedNakshatra() const {
    return "Nakshatra: " + std::to_string(static_cast<int>(nakshatra));
}

std::string PanchangaData::getFormattedYoga() const {
    return "Yoga: " + std::to_string(static_cast<int>(yoga));
}

std::string PanchangaData::getFormattedKarana() const {
    return "Karana: " + std::to_string(static_cast<int>(karana));
}

std::string PanchangaData::getSummary() const {
    std::ostringstream oss;
    oss << "Hindu Calendar Summary:\n";
    oss << "Tithi: " << static_cast<int>(tithi) << " (" << (isKrishna ? "Krishna" : "Shukla") << ")\n";
    oss << "Nakshatra: " << static_cast<int>(nakshatra) << "\n";
    oss << "Yoga: " << static_cast<int>(yoga) << "\n";
    oss << "Karana: " << static_cast<int>(karana) << "\n";
    oss << "Vara: " << static_cast<int>(vara) << "\n";
    return oss.str();
}

std::string HinduCalendar::generatePanchangaTable(const PanchangaData& panchanga) const {
    std::ostringstream oss;

    oss << "\n═══════════════════════════════════════════════════════════════════\n";
    oss << "                          🕉️  HINDU PANCHANGA  🕉️\n";
    oss << "═══════════════════════════════════════════════════════════════════\n\n";

    oss << "� CALCULATION SETTINGS:\n";
    oss << "   Ayanamsa: " << getAyanamsaName() << "\n";
    oss << "   Method: " << getCalculationMethodName() << "\n";
    oss << "   System: " << (calendarSystem == CalendarSystem::LUNAR_BASED ? "Lunar-based" :
                              calendarSystem == CalendarSystem::SOLAR_BASED ? "Solar-based" :
                              "Luni-Solar") << "\n\n";

    oss << "�📅 DATE INFORMATION:\n";
    oss << "   Hindu Year (Vikram Samvat): " << panchanga.year << "\n";
    oss << "   Hindu Month: " << getHinduMonthName(panchanga.month) << "\n";
    oss << "   Paksha: " << (panchanga.isShukla ? "Shukla (Bright)" : "Krishna (Dark)") << "\n\n";

    oss << "🌟 PANCHANGAM (FIVE LIMBS):\n";
    oss << "   1. Tithi:     " << getTithiName(panchanga.tithi)
        << " (ends in " << std::fixed << std::setprecision(1) << panchanga.tithiEndTime << " hours)\n";
    oss << "   2. Vara:      " << getVaraName(panchanga.vara) << "\n";
    oss << "   3. Nakshatra: " << getNakshatraName(panchanga.nakshatra)
        << " (ends in " << std::fixed << std::setprecision(1) << panchanga.nakshatraEndTime << " hours)\n";
    oss << "   4. Yoga:      " << getYogaName(panchanga.yoga)
        << " (ends in " << std::fixed << std::setprecision(1) << panchanga.yogaEndTime << " hours)\n";
    oss << "   5. Karana:    " << getKaranaName(panchanga.karana)
        << " (ends in " << std::fixed << std::setprecision(1) << panchanga.karanaEndTime << " hours)\n\n";

    oss << "🌞 CELESTIAL POSITIONS:\n";
    oss << "   Sun in:   " << getRashiName(panchanga.sunRashi)
        << " (" << std::fixed << std::setprecision(2) << panchanga.sunLongitude << "°)\n";
    oss << "   Moon in:  " << getRashiName(panchanga.moonRashi)
        << " (" << std::fixed << std::setprecision(2) << panchanga.moonLongitude << "°)\n";
    oss << "   Lunar Phase: " << std::fixed << std::setprecision(1) << panchanga.lunarPhase << "°\n\n";

    if (panchanga.isEkadashi || panchanga.isPurnima || panchanga.isAmavasya || panchanga.isSankranti) {
        oss << "🌙 SPECIAL OBSERVANCES:\n";
        if (panchanga.isEkadashi) oss << "   • Ekadashi - Fasting day\n";
        if (panchanga.isPurnima) oss << "   • Purnima - Full Moon\n";
        if (panchanga.isAmavasya) oss << "   • Amavasya - New Moon\n";
        if (panchanga.isSankranti) oss << "   • Sankranti - Solar transition\n";
        oss << "\n";
    }

    if (!panchanga.festivals.empty()) {
        oss << "🎉 FESTIVALS:\n";
        for (const auto& festival : panchanga.festivals) {
            oss << "   • " << festival << "\n";
        }
        oss << "\n";
    }

    if (!panchanga.specialEvents.empty()) {
        oss << "⭐ SPECIAL EVENTS:\n";
        for (const auto& event : panchanga.specialEvents) {
            oss << "   • " << event << "\n";
        }
        oss << "\n";
    }

    oss << "🕐 MUHURTA ASSESSMENT:\n";
    oss << "   Status: " << (panchanga.isShubhaMuhurta ? "✅ Shubha (Auspicious)" : "⚠️  Exercise Caution") << "\n";
    oss << "   Note: " << panchanga.muhurtaDescription << "\n\n";

    oss << "═══════════════════════════════════════════════════════════════════\n";

    return oss.str();
}

// Utility name methods
std::string HinduCalendar::getNakshatraName(HinduNakshatra nak) const {
    if (static_cast<int>(nak) >= 1 && static_cast<int>(nak) <= 27) {
        return nakshatraData[static_cast<int>(nak) - 1].name;
    }
    return "Unknown";
}

std::string HinduCalendar::getTithiName(Tithi tithi) const {
    static const std::vector<std::string> tithiNames = {
        "", "Pratipad", "Dwitiya", "Tritiya", "Chaturthi", "Panchami", "Shashthi", "Saptami",
        "Ashtami", "Navami", "Dashami", "Ekadashi", "Dwadashi", "Trayodashi", "Chaturdashi", "Purnima",
        "Pratipad K", "Dwitiya K", "Tritiya K", "Chaturthi K", "Panchami K", "Shashthi K", "Saptami K",
        "Ashtami K", "Navami K", "Dashami K", "Ekadashi K", "Dwadashi K", "Trayodashi K", "Chaturdashi K", "Amavasya"
    };

    int index = static_cast<int>(tithi);
    if (index >= 1 && index <= 30) {
        return tithiNames[index];
    }
    return "Unknown";
}

std::string HinduCalendar::getYogaName(Yoga yoga) const {
    int index = static_cast<int>(yoga);
    if (index >= 1 && index <= 27) {
        return yogaData[index - 1].name;
    }
    return "Unknown";
}

std::string HinduCalendar::getKaranaName(Karana karana) const {
    int index = static_cast<int>(karana);
    if (index >= 1 && index <= 11) {
        return karanaData[index - 1].name;
    }
    return "Unknown";
}

std::string HinduCalendar::getVaraName(Vara vara) const {
    static const std::vector<std::string> varaNames = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

    int index = static_cast<int>(vara);
    if (index >= 0 && index <= 6) {
        return varaNames[index];
    }
    return "Unknown";
}

std::string HinduCalendar::getHinduMonthName(HinduMonth month) const {
    static const std::vector<std::string> monthNames = {
        "", "Chaitra", "Vaishakha", "Jyeshtha", "Ashadha", "Shravana", "Bhadrapada",
        "Ashwini", "Kartika", "Margashirsha", "Pausha", "Magha", "Phalguna"
    };

    int index = static_cast<int>(month);
    if (index >= 1 && index <= 12) {
        return monthNames[index];
    }
    return "Unknown";
}

std::string HinduCalendar::getRashiName(Rashi rashi) const {
    static const std::vector<std::string> rashiNames = {
        "", "Mesha (Aries)", "Vrishabha (Taurus)", "Mithuna (Gemini)", "Karka (Cancer)",
        "Simha (Leo)", "Kanya (Virgo)", "Tula (Libra)", "Vrishchika (Scorpio)",
        "Dhanu (Sagittarius)", "Makara (Capricorn)", "Kumbha (Aquarius)", "Meena (Pisces)"
    };

    int index = static_cast<int>(rashi);
    if (index >= 1 && index <= 12) {
        return rashiNames[index];
    }
    return "Unknown";
}

std::vector<PanchangaData> HinduCalendar::calculatePanchangaRange(const std::string& fromDate,
                                                               const std::string& toDate,
                                                               double latitude, double longitude) const {
    std::vector<PanchangaData> results;

    if (!initialized) {
        return results;
    }

    try {
        // Parse date strings to get Julian days
        int fromYear, fromMonth, fromDay;
        int toYear, toMonth, toDay;

        if (!Astro::parseBCDate(fromDate, fromYear, fromMonth, fromDay) ||
            !Astro::parseBCDate(toDate, toYear, toMonth, toDay)) {
            lastError = "Invalid date format in range";
            return results;
        }

        // Convert to Julian days
        double fromJD = swe_julday(fromYear, fromMonth, fromDay, 0.0, SE_GREG_CAL);
        double toJD = swe_julday(toYear, toMonth, toDay, 0.0, SE_GREG_CAL);

        // Calculate panchanga for each day
        for (double jd = fromJD; jd <= toJD; jd += 1.0) {
            PanchangaData panchanga = calculatePanchanga(jd, latitude, longitude);
            if (panchanga.tithi != Tithi::PRATIPAD || jd == fromJD) { // Basic validation
                results.push_back(panchanga);
            }
        }

    } catch (const std::exception& e) {
        lastError = "Error calculating panchanga range: " + std::string(e.what());
    }

    return results;
}

std::string HinduCalendar::generatePanchangaTable(const std::vector<PanchangaData>& panchangaList) const {
    if (panchangaList.empty()) {
        return "No Panchanga data available.";
    }

    std::ostringstream oss;

    oss << "\n═══════════════════════════════════════════════════════════════════\n";
    oss << "              🕉️  HINDU PANCHANGA TABLE  🕉️\n";
    oss << "═══════════════════════════════════════════════════════════════════\n\n";

    // Header
    oss << "Date       | Tithi      | Vara      | Nakshatra    | Yoga       | Karana    | Festivals\n";
    oss << "-----------|------------|-----------|--------------|------------|-----------|----------\n";

    for (const auto& panchanga : panchangaList) {
        // Format date from Julian day (would need date calculation)
        oss << "Date       | ";
        oss << std::setw(10) << getTithiName(panchanga.tithi) << " | ";
        oss << std::setw(9) << getVaraName(panchanga.vara) << " | ";
        oss << std::setw(12) << getNakshatraName(panchanga.nakshatra) << " | ";
        oss << std::setw(10) << getYogaName(panchanga.yoga) << " | ";
        oss << std::setw(9) << getKaranaName(panchanga.karana) << " | ";

        if (!panchanga.festivals.empty()) {
            oss << panchanga.festivals[0];
            for (size_t i = 1; i < panchanga.festivals.size(); ++i) {
                oss << ", " << panchanga.festivals[i];
            }
        }
        oss << "\n";
    }

    oss << "\n═══════════════════════════════════════════════════════════════════\n";

    return oss.str();
}

std::string HinduCalendar::generatePanchangaTableFormat(const std::vector<PanchangaData>& panchangaList,
                                                        const std::vector<double>& julianDays) const {
    if (julianDays.empty()) {
        return "No Julian Day data available.";
    }

    std::ostringstream oss;
    oss << "Pancanga based on Suryasiddhanta (AD 1000 ca) Use Mean Position\n";
    oss << std::string(110, '-') << "\n";

    // Loop based on Julian Days - each calendar calculates independently
    for (size_t i = 0; i < julianDays.size(); ++i) {
        double jd = julianDays[i];

        // Reset stream formatting state at start of each iteration
        oss << std::setfill(' ') << std::setprecision(6);
        oss.unsetf(std::ios::fixed | std::ios::showpos);

        // === GREGORIAN CALENDAR - Independent calculation from JD ===
        int gregYear, gregMonth, gregDay, gregHour, gregMin;
        double gregSec;
        swe_jdet_to_utc(jd, SE_GREG_CAL, &gregYear, &gregMonth, &gregDay, &gregHour, &gregMin, &gregSec);

        // === JULIAN CALENDAR - Simple approximation from Gregorian ===
        // Julian calendar is approximately 13 days behind Gregorian in current era
        int julYear = gregYear;
        int julMonth = gregMonth;
        int julDay = gregDay - 13;

        // Handle month/year rollover for negative days
        while (julDay <= 0) {
            julMonth--;
            if (julMonth <= 0) {
                julMonth = 12;
                julYear--;
            }
            // Days in month (Julian calendar)
            int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (julMonth == 2 && julYear % 4 == 0) daysInMonth[1] = 29; // Julian leap year rule
            julDay += daysInMonth[julMonth - 1];
        }


        // === HINDU CALENDAR - Independent calculation from JD ===
        PanchangaData hinduData = calculatePanchanga(jd, 0.0, 0.0);

        // === MYANMAR CALENDAR - Independent calculation from JD ===
        MyanmarCalendar myanmarCalendar;
        MyanmarCalendarData myanmarData;
        bool hasMyanmarData = false;

        if (myanmarCalendar.initialize()) {
            try {
                myanmarData = myanmarCalendar.calculateMyanmarCalendar(jd);
                hasMyanmarData = true;
            } catch (...) {
                hasMyanmarData = false;
            }
        }

        // === OUTPUT FORMATTING ===
        std::vector<std::string> dayNames = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        std::vector<std::string> months = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        // Calculate day of week from Julian Day
        int dayOfWeek = static_cast<int>(jd + 1.5) % 7; // JD 0 = Monday, so adjust

        // Line 1: Julian Calendar | Gregorian Calendar
        oss << "Julian (" << julYear << " " << months[julMonth-1] << " ";
        if (julDay < 10) oss << "0";  // Manual padding
        oss << julDay << " " << dayNames[dayOfWeek] << ")";
        oss << std::string(25, ' ');
        oss << "Gregorian (" << gregYear << " " << months[gregMonth-1] << " ";
        if (gregDay < 10) oss << "0";  // Manual padding
        oss << gregDay << " " << dayNames[dayOfWeek] << ")\n";

        // Line 2: Traditional Years and Hindu Month/Tithi
        int kaliYear = calculateKaliYear(jd);
        int shakaYear = calculateShakaYear(jd);
        std::string hinduMonth = getHinduMonthName(hinduData.month);
        if (hinduMonth.length() > 8) hinduMonth = hinduMonth.substr(0, 8);
        std::string paksha = hinduData.isShukla ? "SUKLAPAKSA" : "KRSNAPAKSA";
        int tithiNum = static_cast<int>(hinduData.tithi);
        double tithiFraction = hinduData.lunarPhase / 360.0;

        oss << "KALI  " << kaliYear << "        SAKA  " << shakaYear;
        oss << std::string(10, ' ');
        oss << "VIKEAMA " << hinduData.year << "       " << std::setw(8) << std::left << hinduMonth;
        oss << "        " << std::setw(11) << paksha << "        " << std::setw(2) << tithiNum
            << "        (" << std::fixed << std::setprecision(4) << tithiFraction << ")\n";

        // Line 3: Myanmar Calendar
        if (hasMyanmarData) {
            std::vector<std::string> myanmarMonths = {
                "Tagu", "Kason", "Nayon", "Waso", "Wagaung", "Tawthalin",
                "Thadingyut", "Tazaungmon", "Nadaw", "Pyatho", "Tabodwe", "Tabaung"
            };

            std::string myanmarMonth = "Waso";
            int monthIndex = static_cast<int>(myanmarData.month);
            if (monthIndex >= 0 && monthIndex < static_cast<int>(myanmarMonths.size())) {
                myanmarMonth = myanmarMonths[monthIndex];
            }

            std::string phase = "waxing";
            switch (myanmarData.moonPhase) {
                case MyanmarMoonPhase::WAXING: phase = "waxing"; break;
                case MyanmarMoonPhase::WANING: phase = "waning"; break;
                case MyanmarMoonPhase::FULL_MOON: phase = "full"; break;
                case MyanmarMoonPhase::NEW_MOON: phase = "new"; break;
            }

            oss << "Myanmar " << myanmarData.myanmarYear << "      (Common) "
                << myanmarMonth;  // Removed setw formatting
            oss << "        " << phase;  // Removed setw formatting
            oss << "        " << myanmarData.fortnightDay << "\n";
        } else {
            // Fallback if Myanmar calendar fails
            oss << "Myanmar " << (gregYear - 638) << "      (Common) "
                << std::setw(8) << std::left << "Waso"
                << "        " << std::setw(6) << std::left << "waxing"
                << "        " << std::setw(2) << 1 << "\n";
        }

        if (i < julianDays.size() - 1) {
            oss << std::string(110, '-') << "\n";
        }
    }

    return oss.str();
}

std::string HinduCalendar::generateJSON(const PanchangaData& panchanga) const {
    std::ostringstream oss;

    oss << "{\n";
    oss << "  \"tithi\": {\n";
    oss << "    \"number\": " << static_cast<int>(panchanga.tithi) << ",\n";
    oss << "    \"name\": \"" << getTithiName(panchanga.tithi) << "\",\n";
    oss << "    \"endTime\": " << panchanga.tithiEndTime << "\n";
    oss << "  },\n";
    oss << "  \"vara\": {\n";
    oss << "    \"number\": " << static_cast<int>(panchanga.vara) << ",\n";
    oss << "    \"name\": \"" << getVaraName(panchanga.vara) << "\"\n";
    oss << "  },\n";
    oss << "  \"nakshatra\": {\n";
    oss << "    \"number\": " << static_cast<int>(panchanga.nakshatra) << ",\n";
    oss << "    \"name\": \"" << getNakshatraName(panchanga.nakshatra) << "\",\n";
    oss << "    \"endTime\": " << panchanga.nakshatraEndTime << "\n";
    oss << "  },\n";
    oss << "  \"yoga\": {\n";
    oss << "    \"number\": " << static_cast<int>(panchanga.yoga) << ",\n";
    oss << "    \"name\": \"" << getYogaName(panchanga.yoga) << "\",\n";
    oss << "    \"endTime\": " << panchanga.yogaEndTime << "\n";
    oss << "  },\n";
    oss << "  \"karana\": {\n";
    oss << "    \"number\": " << static_cast<int>(panchanga.karana) << ",\n";
    oss << "    \"name\": \"" << getKaranaName(panchanga.karana) << "\",\n";
    oss << "    \"endTime\": " << panchanga.karanaEndTime << "\n";
    oss << "  },\n";
    oss << "  \"hinduCalendar\": {\n";
    oss << "    \"year\": " << panchanga.year << ",\n";
    oss << "    \"month\": \"" << getHinduMonthName(panchanga.month) << "\",\n";
    oss << "    \"paksha\": \"" << (panchanga.isShukla ? "Shukla" : "Krishna") << "\"\n";
    oss << "  },\n";
    oss << "  \"celestial\": {\n";
    oss << "    \"sunLongitude\": " << panchanga.sunLongitude << ",\n";
    oss << "    \"moonLongitude\": " << panchanga.moonLongitude << ",\n";
    oss << "    \"lunarPhase\": " << panchanga.lunarPhase << ",\n";
    oss << "    \"sunRashi\": \"" << getRashiName(panchanga.sunRashi) << "\",\n";
    oss << "    \"moonRashi\": \"" << getRashiName(panchanga.moonRashi) << "\"\n";
    oss << "  },\n";
    oss << "  \"specialDays\": {\n";
    oss << "    \"isEkadashi\": " << (panchanga.isEkadashi ? "true" : "false") << ",\n";
    oss << "    \"isPurnima\": " << (panchanga.isPurnima ? "true" : "false") << ",\n";
    oss << "    \"isAmavasya\": " << (panchanga.isAmavasya ? "true" : "false") << ",\n";
    oss << "    \"isSankranti\": " << (panchanga.isSankranti ? "true" : "false") << "\n";
    oss << "  },\n";
    oss << "  \"festivals\": [";
    for (size_t i = 0; i < panchanga.festivals.size(); ++i) {
        oss << "\"" << panchanga.festivals[i] << "\"";
        if (i < panchanga.festivals.size() - 1) oss << ", ";
    }
    oss << "],\n";
    oss << "  \"muhurta\": {\n";
    oss << "    \"isShubha\": " << (panchanga.isShubhaMuhurta ? "true" : "false") << ",\n";
    oss << "    \"description\": \"" << panchanga.muhurtaDescription << "\"\n";
    oss << "  }\n";
    oss << "}";

    return oss.str();
}

std::string HinduCalendar::generateCSV(const std::vector<PanchangaData>& panchangaList) const {
    if (panchangaList.empty()) {
        return "No data available";
    }

    std::ostringstream oss;

    // Header
    oss << "Date,Tithi,Vara,Nakshatra,Yoga,Karana,Hindu_Month,Hindu_Year,Paksha,";
    oss << "Sun_Longitude,Moon_Longitude,Lunar_Phase,Is_Ekadashi,Is_Purnima,Is_Amavasya,";
    oss << "Is_Sankranti,Festivals,Muhurta_Status\n";

    for (const auto& panchanga : panchangaList) {
        oss << "Date,"; // Would need actual date formatting
        oss << getTithiName(panchanga.tithi) << ",";
        oss << getVaraName(panchanga.vara) << ",";
        oss << getNakshatraName(panchanga.nakshatra) << ",";
        oss << getYogaName(panchanga.yoga) << ",";
        oss << getKaranaName(panchanga.karana) << ",";
        oss << getHinduMonthName(panchanga.month) << ",";
        oss << panchanga.year << ",";
        oss << (panchanga.isShukla ? "Shukla" : "Krishna") << ",";
        oss << panchanga.sunLongitude << ",";
        oss << panchanga.moonLongitude << ",";
        oss << panchanga.lunarPhase << ",";
        oss << (panchanga.isEkadashi ? "Yes" : "No") << ",";
        oss << (panchanga.isPurnima ? "Yes" : "No") << ",";
        oss << (panchanga.isAmavasya ? "Yes" : "No") << ",";
        oss << (panchanga.isSankranti ? "Yes" : "No") << ",";

        // Festivals (combine into one field)
        if (!panchanga.festivals.empty()) {
            oss << "\"";
            for (size_t i = 0; i < panchanga.festivals.size(); ++i) {
                oss << panchanga.festivals[i];
                if (i < panchanga.festivals.size() - 1) oss << "; ";
            }
            oss << "\"";
        }
        oss << ",";
        oss << (panchanga.isShubhaMuhurta ? "Shubha" : "Caution");
        oss << "\n";
    }

    return oss.str();
}

bool HinduCalendar::parseDate(const std::string& dateStr, int& year, int& month, int& day) const {
    if (dateStr.length() < 10) return false;

    try {
        // Simple YYYY-MM-DD parsing
        year = std::stoi(dateStr.substr(0, 4));
        month = std::stoi(dateStr.substr(5, 2));
        day = std::stoi(dateStr.substr(8, 2));

        // Basic validation
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Main search function - comprehensive Hindu calendar search
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchHinduCalendar(const SearchCriteria& criteria, double latitude, double longitude) const {
    std::vector<SearchResult> results;

    if (!initialized) {
        return results;
    }

    // Parse search date range
    int startYear, startMonth, startDay;
    int endYear, endMonth, endDay;

    if (!Astro::parseBCDate(criteria.searchStartDate, startYear, startMonth, startDay) ||
        !Astro::parseBCDate(criteria.searchEndDate, endYear, endMonth, endDay)) {
        return results; // Invalid date range
    }

    // Calculate Julian day range
    double startJD = gregorianDateToJulianDay(startYear, startMonth, startDay, 0.0);
    double endJD = gregorianDateToJulianDay(endYear, endMonth, endDay, 0.0);

    // Search each day in the range
    for (double jd = startJD; jd <= endJD; jd += 1.0) {
        try {
            // Calculate panchanga for this day
            PanchangaData panchanga = calculatePanchanga(jd, latitude, longitude);

            // Calculate weekday (0=Sunday, 6=Saturday)
            int weekday = static_cast<int>(jd + 1.5) % 7;

            // Convert to Gregorian date for result
            int gregYear, gregMonth, gregDay;
            int gregHour, gregMin;
            double gregSec;
            swe_jdet_to_utc(jd, SE_GREG_CAL, &gregYear, &gregMonth, &gregDay, &gregHour, &gregMin, &gregSec);

            char dateBuffer[32];
            snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d", gregYear, gregMonth, gregDay);

            // Check all criteria and calculate match score
            double matchScore = 0.0;
            std::string matchDescription;
            int matchCount = 0;
            int totalCriteria = 0;
            bool isMatch = (criteria.logicMode == LogicMode::AND);

            // Year criteria
            if (criteria.exactYear != -1) {
                totalCriteria++;
                bool match = (gregYear == criteria.exactYear);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Year=" + std::to_string(gregYear);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.yearRangeStart != -1 && criteria.yearRangeEnd != -1) {
                totalCriteria++;
                bool match = (gregYear >= criteria.yearRangeStart && gregYear <= criteria.yearRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Year=" + std::to_string(criteria.yearRangeStart) + "-" + std::to_string(criteria.yearRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Month criteria
            if (criteria.exactMonth != -1) {
                totalCriteria++;
                bool match = (gregMonth == criteria.exactMonth);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Month=" + std::to_string(gregMonth);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.monthRangeStart != -1 && criteria.monthRangeEnd != -1) {
                totalCriteria++;
                bool match = (gregMonth >= criteria.monthRangeStart && gregMonth <= criteria.monthRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Month=" + std::to_string(criteria.monthRangeStart) + "-" + std::to_string(criteria.monthRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Tithi criteria
            int currentTithi = static_cast<int>(panchanga.tithi);
            if (criteria.exactTithi != -1) {
                totalCriteria++;
                bool match = false;
                if (criteria.exactMatch) {
                    match = (currentTithi == criteria.exactTithi);
                } else {
                    // Near match for tithi
                    int diff = abs(currentTithi - criteria.exactTithi);
                    match = (diff <= criteria.nearMatchTolerance || (diff >= 29 && criteria.nearMatchTolerance >= 1));
                }
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Tithi=" + getTithiName(panchanga.tithi);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.tithiRangeStart != -1 && criteria.tithiRangeEnd != -1) {
                totalCriteria++;
                bool match = (currentTithi >= criteria.tithiRangeStart && currentTithi <= criteria.tithiRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Tithi=" + std::to_string(criteria.tithiRangeStart) + "-" + std::to_string(criteria.tithiRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Weekday criteria
            if (criteria.exactWeekday != -1) {
                totalCriteria++;
                bool match = (weekday == criteria.exactWeekday);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Weekday=" + getVaraName(static_cast<Vara>(weekday));
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Nakshatra criteria
            int currentNakshatra = static_cast<int>(panchanga.nakshatra);
            if (criteria.exactNakshatra != -1) {
                totalCriteria++;
                bool match = (currentNakshatra == criteria.exactNakshatra);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Nakshatra=" + getNakshatraName(panchanga.nakshatra);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.nakshatraRangeStart != -1 && criteria.nakshatraRangeEnd != -1) {
                totalCriteria++;
                bool match = (currentNakshatra >= criteria.nakshatraRangeStart && currentNakshatra <= criteria.nakshatraRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Nakshatra=" + std::to_string(criteria.nakshatraRangeStart) + "-" + std::to_string(criteria.nakshatraRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Yoga criteria
            int currentYoga = static_cast<int>(panchanga.yoga);
            if (criteria.exactYoga != -1) {
                totalCriteria++;
                bool match = (currentYoga == criteria.exactYoga);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Yoga=" + getYogaName(panchanga.yoga);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.yogaRangeStart != -1 && criteria.yogaRangeEnd != -1) {
                totalCriteria++;
                bool match = (currentYoga >= criteria.yogaRangeStart && currentYoga <= criteria.yogaRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Yoga=" + std::to_string(criteria.yogaRangeStart) + "-" + std::to_string(criteria.yogaRangeEnd);
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Special day criteria
            if (criteria.searchEkadashi) {
                totalCriteria++;
                bool match = panchanga.isEkadashi;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Ekadashi";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchPurnima) {
                totalCriteria++;
                bool match = panchanga.isPurnima;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Purnima";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchAmavasya) {
                totalCriteria++;
                bool match = panchanga.isAmavasya;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Amavasya";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchSankranti) {
                totalCriteria++;
                bool match = panchanga.isSankranti;
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "Sankranti";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Calculate match score
            if (totalCriteria > 0) {
                matchScore = static_cast<double>(matchCount) / static_cast<double>(totalCriteria);
            } else {
                // If no specific criteria are provided, return all days with base score
                matchScore = 0.5;
                matchDescription = "All days (no specific criteria)";
            }

            // Add to results if there's a match (or if no criteria specified)
            if (isMatch && matchScore > 0.0) {
                SearchResult result;
                result.gregorianDate = std::string(dateBuffer);
                result.panchangaData = panchanga;
                result.julianDay = jd;
                result.weekday = weekday;
                result.matchScore = matchScore;
                result.matchDescription = matchDescription;

                results.push_back(result);
            }

        } catch (const std::exception&) {
            // Skip this day if calculation fails
            continue;
        }
    }

    // Sort results by match score (highest first), then by date
    std::sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        if (a.matchScore != b.matchScore) {
            return a.matchScore > b.matchScore;
        }
        return a.julianDay < b.julianDay;
    });

    return results;
}

// Search by specific tithi
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByTithi(int tithi, const std::string& startDate, const std::string& endDate, double latitude, double longitude, bool exactMatch) const {
    SearchCriteria criteria;
    criteria.exactTithi = tithi;
    criteria.exactMatch = exactMatch;
    criteria.nearMatchTolerance = 1;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Search by weekday
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByWeekday(int weekday, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactWeekday = weekday;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Search by month
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByMonth(int month, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactMonth = month;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Search by year
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByYear(int year, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactYear = year;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Multi-criteria search: Ekadashi AND Purnima
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchEkadashiAndPurnima(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchEkadashi = true;
    criteria.searchPurnima = true;
    criteria.logicMode = LogicMode::AND;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Multi-criteria search: Purnima OR Amavasya
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchPurnimaOrAmavasya(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchPurnima = true;
    criteria.searchAmavasya = true;
    criteria.logicMode = LogicMode::OR;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Search by specific nakshatra
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByNakshatra(int nakshatra, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactNakshatra = nakshatra;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Generic multi-criteria search with specified logic mode
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchMultiCriteria(const SearchCriteria& criteria, LogicMode logicMode, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria modifiedCriteria = criteria;
    modifiedCriteria.logicMode = logicMode;
    modifiedCriteria.searchStartDate = startDate;
    modifiedCriteria.searchEndDate = endDate;

    return searchHinduCalendar(modifiedCriteria, latitude, longitude);
}

} // namespace Astro
