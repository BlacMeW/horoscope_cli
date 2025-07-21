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
        {"Purnima", "पूर्णिमा", Planet::SATURN, "Full Moon", true, "Completion, fulfillment"},
        // Krishna Paksha
        {"Pratipad K", "प्रतिपदा कृष्ण", Planet::SUN, "Beginning Dark", false, "New challenges"},
        {"Dwitiya K", "द्वितीया कृष्ण", Planet::MOON, "Second Dark", false, "Letting go"},
        {"Tritiya K", "तृतीया कृष्ण", Planet::MARS, "Third Dark", false, "Inner strength"},
        {"Chaturthi K", "चतुर्थी कृष्ण", Planet::MERCURY, "Fourth Dark", false, "Mental clarity"},
        {"Panchami K", "पंचमी कृष्ण", Planet::JUPITER, "Fifth Dark", false, "Wisdom through trials"},
        {"Shashthi K", "षष्ठी कृष्ण", Planet::VENUS, "Sixth Dark", false, "Purification"},
        {"Saptami K", "सप्तमी कृष्ण", Planet::SATURN, "Seventh Dark", false, "Patience"},
        {"Ashtami K", "अष्टमी कृष्ण", Planet::NORTH_NODE, "Eighth Dark", false, "Power struggles"},
        {"Navami K", "नवमी कृष्ण", Planet::SUN, "Ninth Dark", false, "Spiritual completion"},
        {"Dashami K", "दशमी कृष्ण", Planet::MOON, "Tenth Dark", false, "Emotional release"},
        {"Ekadashi K", "एकादशी कृष्ण", Planet::MARS, "Eleventh Dark", false, "Deep fasting"},
        {"Dwadashi K", "द्वादशी कृष्ण", Planet::MERCURY, "Twelfth Dark", false, "Mental preparation"},
        {"Trayodashi K", "त्रयोदशी कृष्ण", Planet::JUPITER, "Thirteenth Dark", false, "Final learning"},
        {"Chaturdashi K", "चतुर्दशी कृष्ण", Planet::VENUS, "Fourteenth Dark", false, "Last prayers"},
        {"Amavasya", "अमावस्या", Planet::SATURN, "New Moon", false, "Rebirth, renewal"}
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

    // Ekadashi festivals - detailed list
    festivalMap["Chaitra_Ekadashi_S"] = {"Kamada Ekadashi"};
    festivalMap["Chaitra_Ekadashi_K"] = {"Papmochani Ekadashi"};
    festivalMap["Vaishakha_Ekadashi_S"] = {"Varuthini Ekadashi"};
    festivalMap["Vaishakha_Ekadashi_K"] = {"Mohini Ekadashi"};
    festivalMap["Jyeshtha_Ekadashi_S"] = {"Apara Ekadashi"};
    festivalMap["Jyeshtha_Ekadashi_K"] = {"Nirjala Ekadashi"};
    festivalMap["Ashadha_Ekadashi_S"] = {"Yogini Ekadashi"};
    festivalMap["Ashadha_Ekadashi_K"] = {"Kamika Ekadashi"};
    festivalMap["Shravana_Ekadashi_S"] = {"Putrada Ekadashi"};
    festivalMap["Shravana_Ekadashi_K"] = {"Aja Ekadashi"};
    festivalMap["Bhadrapada_Ekadashi_S"] = {"Parsva Ekadashi"};
    festivalMap["Bhadrapada_Ekadashi_K"] = {"Indira Ekadashi"};
    festivalMap["Ashwini_Ekadashi_S"] = {"Papankusha Ekadashi"};
    festivalMap["Ashwini_Ekadashi_K"] = {"Rama Ekadashi"};
    festivalMap["Kartika_Ekadashi_S"] = {"Devuthani Ekadashi", "Prabodhini Ekadashi"};
    festivalMap["Kartika_Ekadashi_K"] = {"Utpanna Ekadashi"};
    festivalMap["Margashirsha_Ekadashi_S"] = {"Mokshada Ekadashi"};
    festivalMap["Margashirsha_Ekadashi_K"] = {"Saphala Ekadashi"};
    festivalMap["Pausha_Ekadashi_S"] = {"Putrada Ekadashi"};
    festivalMap["Pausha_Ekadashi_K"] = {"Shattila Ekadashi"};
    festivalMap["Magha_Ekadashi_S"] = {"Jaya Ekadashi"};
    festivalMap["Magha_Ekadashi_K"] = {"Vijaya Ekadashi"};
    festivalMap["Phalguna_Ekadashi_S"] = {"Amalaki Ekadashi"};
    festivalMap["Phalguna_Ekadashi_K"] = {"Papamochani Ekadashi"};

    // Amavasya festivals
    festivalMap["Chaitra_Amavasya"] = {"Chaitra Amavasya"};
    festivalMap["Vaishakha_Amavasya"] = {"Shani Amavasya"};
    festivalMap["Jyeshtha_Amavasya"] = {"Vat Savitri Amavasya"};
    festivalMap["Ashadha_Amavasya"] = {"Hariyali Amavasya"};
    festivalMap["Shravana_Amavasya"] = {"Pithori Amavasya"};
    festivalMap["Bhadrapada_Amavasya"] = {"Pitru Amavasya", "Sarva Pitru Amavasya"};
    festivalMap["Ashwini_Amavasya"] = {"Sharad Amavasya"};
    festivalMap["Kartika_Amavasya"] = {"Diwali", "Kali Chaudas", "Lakshmi Puja"};
    festivalMap["Margashirsha_Amavasya"] = {"Margashirsha Amavasya"};
    festivalMap["Pausha_Amavasya"] = {"Pausha Amavasya"};
    festivalMap["Magha_Amavasya"] = {"Mauni Amavasya", "Maghi Amavasya"};
    festivalMap["Phalguna_Amavasya"] = {"Phalguna Amavasya"};

    // Navratri festivals
    festivalMap["Chaitra_Navratri"] = {"Vasant Navratri", "Chaitra Navratri"};
    festivalMap["Ashwini_Navratri"] = {"Sharad Navratri", "Maha Navratri"};

    // Sankranti festivals
    festivalMap["Mesha_Sankranti"] = {"Mesha Sankranti", "Baisakhi"};
    festivalMap["Vrishabha_Sankranti"] = {"Vrishabha Sankranti"};
    festivalMap["Mithuna_Sankranti"] = {"Mithuna Sankranti"};
    festivalMap["Karka_Sankranti"] = {"Karka Sankranti", "Dakshinayana"};
    festivalMap["Simha_Sankranti"] = {"Simha Sankranti"};
    festivalMap["Kanya_Sankranti"] = {"Kanya Sankranti"};
    festivalMap["Tula_Sankranti"] = {"Tula Sankranti"};
    festivalMap["Vrishchika_Sankranti"] = {"Vrishchika Sankranti"};
    festivalMap["Dhanu_Sankranti"] = {"Dhanu Sankranti"};
    festivalMap["Makara_Sankranti"] = {"Makar Sankranti", "Uttarayana"};
    festivalMap["Kumbha_Sankranti"] = {"Kumbha Sankranti"};
    festivalMap["Meena_Sankranti"] = {"Meena Sankranti"};

    // Krishna Paksha special days
    festivalMap["Janmashtami"] = {"Krishna Janmashtami", "Gokulashtami"};
    festivalMap["Shivratri"] = {"Maha Shivratri"};
    festivalMap["Ganesh_Chaturthi"] = {"Ganesh Chaturthi", "Vinayaka Chavithi"};
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

        // Calculate enhanced timing information
        calculateSunMoonTimes(panchanga, latitude, longitude);
        calculateRahuKaal(panchanga);
        calculateYamaganda(panchanga);
        calculateGulikai(panchanga);
        calculateDurMuhurtam(panchanga);
        calculateVarjyam(panchanga);
        calculateSpecialYogas(panchanga);
        calculateNakshatraPada(panchanga);
        calculateChandraTaraBalam(panchanga);
        calculateRituAyana(panchanga);
        calculateShoolDirections(panchanga);
        identifyVrataUpavas(panchanga);

        // Set additional astronomical data
        panchanga.ayanamsaValue = getAyanamsaValue(julianDay);
        panchanga.julianDay = julianDay;
        panchanga.kaliyugaYear = calculateKaliYear(julianDay);
        panchanga.shakaYear = calculateShakaYear(julianDay);
        panchanga.vikramYear = calculateVikramYear(julianDay);

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
    panchanga.festivals.clear();
    panchanga.ekadashiNames.clear();

    if (panchanga.isPurnima) {
        std::string key = monthName + "_Purnima";
        auto it = festivalMap.find(key);
        if (it != festivalMap.end()) {
            panchanga.festivals.insert(panchanga.festivals.end(), it->second.begin(), it->second.end());
        }
    }

    if (panchanga.isAmavasya) {
        std::string key = monthName + "_Amavasya";
        auto it = festivalMap.find(key);
        if (it != festivalMap.end()) {
            panchanga.festivals.insert(panchanga.festivals.end(), it->second.begin(), it->second.end());
        }
    }

    if (panchanga.isEkadashi) {
        // Determine specific Ekadashi name based on month and paksha
        std::string paksha = panchanga.isShukla ? "S" : "K";
        std::string key = monthName + "_Ekadashi_" + paksha;
        auto it = festivalMap.find(key);
        if (it != festivalMap.end()) {
            panchanga.ekadashiNames = it->second;
            panchanga.festivals.insert(panchanga.festivals.end(), it->second.begin(), it->second.end());
        } else {
            panchanga.festivals.push_back("Ekadashi Vrata");
        }
    }

    // Check for Sankranti festivals
    if (panchanga.isSankranti) {
        std::string rashiName = getRashiName(panchanga.sunRashi);
        // Extract just the rashi name without the English translation
        size_t spacePos = rashiName.find(' ');
        if (spacePos != std::string::npos) {
            rashiName = rashiName.substr(0, spacePos);
        }
        std::string key = rashiName + "_Sankranti";
        auto it = festivalMap.find(key);
        if (it != festivalMap.end()) {
            panchanga.festivals.insert(panchanga.festivals.end(), it->second.begin(), it->second.end());
        }
    }

    // Check for special tithi festivals
    int tithiNum = static_cast<int>(panchanga.tithi);
    if (tithiNum == 8 && monthName == "Bhadrapada" && panchanga.isKrishna) {
        panchanga.festivals.push_back("Krishna Janmashtami");
    }

    if (tithiNum == 14 && monthName == "Phalguna" && panchanga.isKrishna) {
        panchanga.festivals.push_back("Maha Shivratri");
    }

    if (tithiNum == 4 && monthName == "Bhadrapada" && panchanga.isShukla) {
        panchanga.festivals.push_back("Ganesh Chaturthi");
    }

    // Check for Navratri periods
    if ((monthName == "Chaitra" && tithiNum >= 1 && tithiNum <= 9 && panchanga.isShukla) ||
        (monthName == "Ashwini" && tithiNum >= 1 && tithiNum <= 9 && panchanga.isShukla)) {
        panchanga.isNavratri = true;
        if (monthName == "Chaitra") {
            panchanga.festivals.push_back("Chaitra Navratri - Day " + std::to_string(tithiNum));
        } else {
            panchanga.festivals.push_back("Sharad Navratri - Day " + std::to_string(tithiNum));
        }
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

void HinduCalendar::calculateSunMoonTimes(PanchangaData& panchanga, double latitude, double longitude) const {
    try {
        char errorString[256];
        double geopos[3] = {longitude, latitude, 0.0}; // longitude, latitude, altitude
        double riseSet[2];

        // Calculate sunrise and sunset
        int result = swe_rise_trans(panchanga.julianDay - 0.5, SE_SUN, nullptr, SEFLG_SIDEREAL,
                                   SE_CALC_RISE, geopos, 1013.25, 10.0, riseSet, errorString);
        if (result >= 0) {
            // Convert to local time (approximate)
            panchanga.sunriseTime = (riseSet[0] - floor(riseSet[0])) * 24.0;
            if (panchanga.sunriseTime < 0) panchanga.sunriseTime += 24.0;
        } else {
            panchanga.sunriseTime = 6.0; // Default sunrise
        }

        result = swe_rise_trans(panchanga.julianDay - 0.5, SE_SUN, nullptr, SEFLG_SIDEREAL,
                               SE_CALC_SET, geopos, 1013.25, 10.0, riseSet, errorString);
        if (result >= 0) {
            panchanga.sunsetTime = (riseSet[0] - floor(riseSet[0])) * 24.0;
            if (panchanga.sunsetTime < 0) panchanga.sunsetTime += 24.0;
        } else {
            panchanga.sunsetTime = 18.0; // Default sunset
        }

        // Calculate moonrise and moonset
        result = swe_rise_trans(panchanga.julianDay - 0.5, SE_MOON, nullptr, SEFLG_SIDEREAL,
                               SE_CALC_RISE, geopos, 1013.25, 10.0, riseSet, errorString);
        if (result >= 0) {
            panchanga.moonriseTime = (riseSet[0] - floor(riseSet[0])) * 24.0;
            if (panchanga.moonriseTime < 0) panchanga.moonriseTime += 24.0;
        } else {
            panchanga.moonriseTime = 7.0; // Default moonrise
        }

        result = swe_rise_trans(panchanga.julianDay - 0.5, SE_MOON, nullptr, SEFLG_SIDEREAL,
                               SE_CALC_SET, geopos, 1013.25, 10.0, riseSet, errorString);
        if (result >= 0) {
            panchanga.moonsetTime = (riseSet[0] - floor(riseSet[0])) * 24.0;
            if (panchanga.moonsetTime < 0) panchanga.moonsetTime += 24.0;
        } else {
            panchanga.moonsetTime = 19.0; // Default moonset
        }

        // Calculate day and night lengths
        panchanga.dayLength = panchanga.sunsetTime - panchanga.sunriseTime;
        if (panchanga.dayLength < 0) panchanga.dayLength += 24.0;
        panchanga.nightLength = 24.0 - panchanga.dayLength;

        // Calculate Brahma Muhurta (1.5 hours before sunrise)
        panchanga.brahmaMuhurtaStart = calculateBrahmaMuhurta(panchanga.sunriseTime, true);
        panchanga.brahmaMuhurtaEnd = calculateBrahmaMuhurta(panchanga.sunriseTime, false);

        // Calculate Abhijit Muhurta (middle of the day)
        panchanga.abhijitStart = calculateAbhijitMuhurta(panchanga.sunriseTime, panchanga.sunsetTime, true);
        panchanga.abhijitEnd = calculateAbhijitMuhurta(panchanga.sunriseTime, panchanga.sunsetTime, false);

        // Calculate Godhuli Bela (evening twilight)
        panchanga.godhuliBelStart = calculateGodhuliBela(panchanga.sunsetTime, true);
        panchanga.godhuliBelEnd = calculateGodhuliBela(panchanga.sunsetTime, false);

        // Calculate Nishita Muhurta (midnight)
        double nextSunrise = panchanga.sunriseTime + 24.0; // Next day sunrise
        panchanga.nishitaMuhurtaStart = calculateNishitaMuhurta(panchanga.sunsetTime, nextSunrise, true);
        panchanga.nishitaMuhurtaEnd = calculateNishitaMuhurta(panchanga.sunsetTime, nextSunrise, false);

    } catch (const std::exception& e) {
        // Set default values in case of error
        panchanga.sunriseTime = 6.0;
        panchanga.sunsetTime = 18.0;
        panchanga.moonriseTime = 7.0;
        panchanga.moonsetTime = 19.0;
        panchanga.dayLength = 12.0;
        panchanga.nightLength = 12.0;
    }
}

void HinduCalendar::calculateRahuKaal(PanchangaData& panchanga) const {
    // Rahu Kaal is 1/8th of day length, starting at different times for each weekday
    double dayEighth = panchanga.dayLength / 8.0;
    int weekday = static_cast<int>(panchanga.vara);

    // Starting periods for each weekday (0=Sunday)
    int rahuPeriods[] = {7, 1, 6, 4, 5, 3, 2}; // Sunday to Saturday

    if (weekday >= 0 && weekday <= 6) {
        int period = rahuPeriods[weekday] - 1; // Convert to 0-based
        panchanga.rahuKaalStart = panchanga.sunriseTime + (period * dayEighth);
        panchanga.rahuKaalEnd = panchanga.rahuKaalStart + dayEighth;
    }
}

void HinduCalendar::calculateYamaganda(PanchangaData& panchanga) const {
    // Yamaganda is also 1/8th of day length, different periods for each weekday
    double dayEighth = panchanga.dayLength / 8.0;
    int weekday = static_cast<int>(panchanga.vara);

    // Starting periods for Yamaganda
    int yamagandaPeriods[] = {4, 3, 2, 1, 7, 6, 5}; // Sunday to Saturday

    if (weekday >= 0 && weekday <= 6) {
        int period = yamagandaPeriods[weekday] - 1; // Convert to 0-based
        panchanga.yamagandaStart = panchanga.sunriseTime + (period * dayEighth);
        panchanga.yamagandaEnd = panchanga.yamagandaStart + dayEighth;
    }
}

void HinduCalendar::calculateGulikai(PanchangaData& panchanga) const {
    // Gulikai periods
    double dayEighth = panchanga.dayLength / 8.0;
    int weekday = static_cast<int>(panchanga.vara);

    // Starting periods for Gulikai
    int gulikaiPeriods[] = {6, 5, 4, 3, 2, 1, 7}; // Sunday to Saturday

    if (weekday >= 0 && weekday <= 6) {
        int period = gulikaiPeriods[weekday] - 1; // Convert to 0-based
        panchanga.gulikaiStart = panchanga.sunriseTime + (period * dayEighth);
        panchanga.gulikaiEnd = panchanga.gulikaiStart + dayEighth;
    }
}

void HinduCalendar::calculateDurMuhurtam(PanchangaData& panchanga) const {
    // Dur Muhurtam is typically around midday
    double dayCenter = panchanga.sunriseTime + (panchanga.dayLength / 2.0);
    panchanga.durMuhurtamStart = dayCenter - 0.75; // 45 minutes before midday
    panchanga.durMuhurtamEnd = dayCenter + 0.75;   // 45 minutes after midday
}

void HinduCalendar::calculateVarjyam(PanchangaData& panchanga) const {
    // Varjyam calculations based on nakshatra
    // This is a simplified calculation - actual Varjyam is complex
    panchanga.varjyamTimes.clear();

    // Sample calculation based on nakshatra number
    int nakNum = static_cast<int>(panchanga.nakshatra);
    double varjyamDuration = 1.0 + (nakNum % 3) * 0.5; // 1 to 2 hours

    // Add multiple Varjyam periods during the day
    double startTime = panchanga.sunriseTime + 6.0;
    panchanga.varjyamTimes.push_back({startTime, startTime + varjyamDuration});

    startTime = panchanga.sunriseTime + 9.0;
    panchanga.varjyamTimes.push_back({startTime, startTime + varjyamDuration/2});
}

void HinduCalendar::calculateSpecialYogas(PanchangaData& panchanga) const {
    // Calculate special astronomical yogas
    panchanga.isSarvarthaSiddhi = false;
    panchanga.isAmritaSiddhi = false;
    panchanga.isDwipushkar = false;
    panchanga.isTripushkar = false;
    panchanga.isRaviPushya = false;
    panchanga.isGuruPushya = false;

    // Sarvartha Siddhi Yoga conditions
    if ((panchanga.vara == Vara::WEDNESDAY && panchanga.nakshatra == HinduNakshatra::HASTA) ||
        (panchanga.vara == Vara::FRIDAY && panchanga.nakshatra == HinduNakshatra::REVATI) ||
        (panchanga.vara == Vara::SUNDAY && panchanga.nakshatra == HinduNakshatra::PUSHYA)) {
        panchanga.isSarvarthaSiddhi = true;
    }

    // Amrita Siddhi Yoga conditions
    if ((panchanga.vara == Vara::MONDAY && panchanga.nakshatra == HinduNakshatra::ROHINI) ||
        (panchanga.vara == Vara::TUESDAY && panchanga.nakshatra == HinduNakshatra::ASHWINI) ||
        (panchanga.vara == Vara::THURSDAY && panchanga.nakshatra == HinduNakshatra::PUNARVASU)) {
        panchanga.isAmritaSiddhi = true;
    }

    // Ravi Pushya Yoga (Sunday + Pushya nakshatra)
    if (panchanga.vara == Vara::SUNDAY && panchanga.nakshatra == HinduNakshatra::PUSHYA) {
        panchanga.isRaviPushya = true;
    }

    // Check for Dwipushkar and Tripushkar based on complex calculations
    // This is simplified - actual calculation involves planetary positions
    int tithiNum = static_cast<int>(panchanga.tithi);
    int nakNum = static_cast<int>(panchanga.nakshatra);

    if ((tithiNum + nakNum) % 7 == 0) {
        panchanga.isDwipushkar = true;
    }

    if ((tithiNum + nakNum) % 9 == 0) {
        panchanga.isTripushkar = true;
    }
}

void HinduCalendar::calculateNakshatraPada(PanchangaData& panchanga) const {
    // Each nakshatra has 4 padas, each spanning 3°20' (200 minutes)
    double nakshatraStart = (static_cast<int>(panchanga.nakshatra) - 1) * 13.333333;
    double offsetInNakshatra = fmod(panchanga.moonLongitude - nakshatraStart, 13.333333);
    if (offsetInNakshatra < 0) offsetInNakshatra += 13.333333;

    panchanga.nakshatraPada = static_cast<int>(offsetInNakshatra / 3.333333) + 1;
    if (panchanga.nakshatraPada > 4) panchanga.nakshatraPada = 4;

    // Calculate when current pada ends
    double nextPadaStart = panchanga.nakshatraPada * 3.333333;
    double remainingDegrees = nextPadaStart - offsetInNakshatra;

    // Approximate time calculation (simplified)
    panchanga.nakshatraPadaEndTime = remainingDegrees / 0.5; // Rough estimate
}

void HinduCalendar::calculateChandraTaraBalam(PanchangaData& panchanga) const {
    // Good Chandra Balam rashis (simplified calculation)
    panchanga.goodChandraBalam.clear();
    panchanga.goodTaraBalam.clear();

    Rashi currentMoonRashi = panchanga.moonRashi;
    int currentMoonRashiNum = static_cast<int>(currentMoonRashi);

    // Add favorable rashis (2, 3, 4, 6, 8, 9, 12 from moon rashi)
    std::vector<int> favorableOffsets = {2, 3, 4, 6, 8, 9, 12};
    for (int offset : favorableOffsets) {
        int favorableRashi = ((currentMoonRashiNum - 1 + offset - 1) % 12) + 1;
        panchanga.goodChandraBalam.push_back(static_cast<Rashi>(favorableRashi));
    }

    // Good Tara Balam nakshatras (simplified)
    HinduNakshatra currentNak = panchanga.nakshatra;
    int currentNakNum = static_cast<int>(currentNak);

    // Add favorable nakshatras
    std::vector<int> favorableNakOffsets = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    for (int offset : favorableNakOffsets) {
        int favorableNak = ((currentNakNum - 1 + offset - 1) % 27) + 1;
        panchanga.goodTaraBalam.push_back(static_cast<HinduNakshatra>(favorableNak));
    }
}

void HinduCalendar::calculateRituAyana(PanchangaData& panchanga) const {
    // Determine Ritu (season) based on sun's longitude
    double sunLong = panchanga.sunLongitude;

    if (sunLong >= 320 || sunLong < 50) {
        panchanga.ritu = "Shishira (Late Winter)";
    } else if (sunLong >= 50 && sunLong < 80) {
        panchanga.ritu = "Vasanta (Spring)";
    } else if (sunLong >= 80 && sunLong < 110) {
        panchanga.ritu = "Grishma (Summer)";
    } else if (sunLong >= 110 && sunLong < 140) {
        panchanga.ritu = "Varsha (Monsoon)";
    } else if (sunLong >= 140 && sunLong < 170) {
        panchanga.ritu = "Sharad (Autumn)";
    } else {
        panchanga.ritu = "Hemanta (Early Winter)";
    }

    // Determine Ayana (solar movement)
    if (sunLong >= 270 || sunLong < 90) {
        panchanga.ayana = "Uttarayana (Northward)";
    } else {
        panchanga.ayana = "Dakshinayana (Southward)";
    }
}

void HinduCalendar::calculateShoolDirections(PanchangaData& panchanga) const {
    // Disha Shool based on weekday
    int weekday = static_cast<int>(panchanga.vara);
    std::vector<std::string> dishaShools = {
        "East", "North", "South", "West", "North", "South", "West"
    };

    if (weekday >= 0 && weekday <= 6) {
        panchanga.dishaShool = dishaShools[weekday];
    }

    // Nakshatra Shool based on nakshatra
    int nakNum = static_cast<int>(panchanga.nakshatra);
    std::vector<std::string> nakShools = {
        "South", "East", "West", "North", "South", "East", "West", "North", "South",
        "East", "West", "North", "South", "East", "West", "North", "South", "East",
        "West", "North", "South", "East", "West", "North", "South", "East", "West"
    };

    if (nakNum >= 1 && nakNum <= 27) {
        panchanga.nakshatraShool = nakShools[nakNum - 1];
    }
}

void HinduCalendar::identifyVrataUpavas(PanchangaData& panchanga) const {
    panchanga.vrataList.clear();
    panchanga.isFastingDay = false;

    // Identify major vratas
    if (panchanga.isEkadashi) {
        panchanga.vrataList.push_back("Ekadashi Vrata");
        panchanga.isFastingDay = true;
    }

    if (panchanga.isPurnima) {
        panchanga.vrataList.push_back("Purnima Vrata");
    }

    if (panchanga.isAmavasya) {
        panchanga.vrataList.push_back("Amavasya Vrata");
    }

    // Weekly vratas
    if (panchanga.vara == Vara::MONDAY) {
        panchanga.vrataList.push_back("Somwar Vrata");
    } else if (panchanga.vara == Vara::TUESDAY) {
        panchanga.vrataList.push_back("Mangalwar Vrata");
    } else if (panchanga.vara == Vara::SATURDAY) {
        panchanga.vrataList.push_back("Shaniwar Vrata");
    }

    // Nakshatra-based vratas
    if (panchanga.nakshatra == HinduNakshatra::ROHINI) {
        panchanga.vrataList.push_back("Rohini Vrata");
    }

    // Check for Ganda Mool
    std::vector<HinduNakshatra> gandaMoolNaks = {
        HinduNakshatra::ASHWINI, HinduNakshatra::ASHLESHA, HinduNakshatra::MAGHA,
        HinduNakshatra::JYESHTHA, HinduNakshatra::MULA, HinduNakshatra::REVATI
    };

    for (auto gnak : gandaMoolNaks) {
        if (panchanga.nakshatra == gnak) {
            panchanga.isGandaMool = true;
            panchanga.vrataList.push_back("Ganda Mool Shanti");
            break;
        }
    }

    // Check for Panchak (simplified)
    std::vector<HinduNakshatra> panchakNaks = {
        HinduNakshatra::DHANISHTA, HinduNakshatra::SHATABHISHA,
        HinduNakshatra::PURVA_BHADRAPADA, HinduNakshatra::UTTARA_BHADRAPADA,
        HinduNakshatra::REVATI
    };

    for (auto pnak : panchakNaks) {
        if (panchanga.nakshatra == pnak) {
            panchanga.isPanchak = true;
            break;
        }
    }

    // Check for Bhadra (Vishti Karana)
    if (panchanga.karana == Karana::VISHTI) {
        panchanga.isBhadra = true;
    }
}

// Helper methods for muhurta calculations
double HinduCalendar::calculateBrahmaMuhurta(double sunriseTime, bool isStart) const {
    // Brahma Muhurta is 1.5 hours before sunrise, lasting for 48 minutes
    if (isStart) {
        return sunriseTime - 1.5;
    } else {
        return sunriseTime - 0.7; // 42 minutes duration
    }
}

double HinduCalendar::calculateAbhijitMuhurta(double sunriseTime, double sunsetTime, bool isStart) const {
    // Abhijit is in the middle of the day, lasting for 48 minutes
    double midday = sunriseTime + ((sunsetTime - sunriseTime) / 2.0);
    if (isStart) {
        return midday - 0.4; // 24 minutes before midday
    } else {
        return midday + 0.4; // 24 minutes after midday
    }
}

double HinduCalendar::calculateGodhuliBela(double sunsetTime, bool isStart) const {
    // Godhuli Bela is around sunset time
    if (isStart) {
        return sunsetTime - 0.25; // 15 minutes before sunset
    } else {
        return sunsetTime + 0.25; // 15 minutes after sunset
    }
}

double HinduCalendar::calculateNishitaMuhurta(double sunsetTime, double nextSunriseTime, bool isStart) const {
    // Nishita Muhurta is around midnight
    double midnight = sunsetTime + ((nextSunriseTime - sunsetTime) / 2.0);
    if (isStart) {
        return midnight - 0.4; // 24 minutes before midnight
    } else {
        return midnight + 0.4; // 24 minutes after midnight
    }
}

// Formatting methods
std::string PanchangaData::getTimeString(double hours) const {
    if (hours < 0 || hours >= 24) {
        hours = fmod(hours, 24.0);
        if (hours < 0) hours += 24.0;
    }

    int h = static_cast<int>(hours);
    int m = static_cast<int>((hours - h) * 60);

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", h, m);
    return std::string(timeStr);
}

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

    oss << "\n══════════════════════════════════════════════════════════════════════════════════\n";
    oss << "                          🕉️  COMPLETE HINDU PANCHANGA  🕉️\n";
    oss << "══════════════════════════════════════════════════════════════════════════════════\n\n";

    oss << "📊 CALCULATION SETTINGS:\n";
    oss << "   Ayanamsa: " << getAyanamsaName() << " (" << std::fixed << std::setprecision(4)
        << panchanga.ayanamsaValue << "°)\n";
    oss << "   Method: " << getCalculationMethodName() << "\n";
    oss << "   System: " << (calendarSystem == CalendarSystem::LUNAR_BASED ? "Lunar-based" :
                              calendarSystem == CalendarSystem::SOLAR_BASED ? "Solar-based" :
                              "Luni-Solar") << "\n";
    oss << "   Julian Day: " << std::fixed << std::setprecision(1) << panchanga.julianDay << "\n\n";

    oss << "📅 DATE INFORMATION:\n";
    oss << "   Vikram Samvat: " << panchanga.vikramYear << "\n";
    oss << "   Shaka Samvat: " << panchanga.shakaYear << "\n";
    oss << "   Kali Yuga: " << panchanga.kaliyugaYear << "\n";
    oss << "   Hindu Month: " << getHinduMonthName(panchanga.month) << "\n";
    oss << "   Paksha: " << (panchanga.isShukla ? "Shukla (Bright)" : "Krishna (Dark)") << "\n\n";

    oss << "🌟 PANCHANGAM (FIVE LIMBS):\n";
    oss << "   1. Tithi:     " << getTithiName(panchanga.tithi)
        << " (ends at " << panchanga.getTimeString(panchanga.tithiEndTime) << ")\n";
    oss << "   2. Vara:      " << getVaraName(panchanga.vara) << "\n";
    oss << "   3. Nakshatra: " << getNakshatraName(panchanga.nakshatra)
        << " (Pada " << panchanga.nakshatraPada << ", ends at "
        << panchanga.getTimeString(panchanga.nakshatraEndTime) << ")\n";
    oss << "   4. Yoga:      " << getYogaName(panchanga.yoga)
        << " (ends at " << panchanga.getTimeString(panchanga.yogaEndTime) << ")\n";
    oss << "   5. Karana:    " << getKaranaName(panchanga.karana)
        << " (ends at " << panchanga.getTimeString(panchanga.karanaEndTime) << ")\n\n";

    oss << "☀️ SUN & MOON INFORMATION:\n";
    oss << "   Sunrise:    " << panchanga.getTimeString(panchanga.sunriseTime) << "\n";
    oss << "   Sunset:     " << panchanga.getTimeString(panchanga.sunsetTime) << "\n";
    oss << "   Moonrise:   " << panchanga.getTimeString(panchanga.moonriseTime) << "\n";
    oss << "   Moonset:    " << panchanga.getTimeString(panchanga.moonsetTime) << "\n";
    oss << "   Day Length: " << std::fixed << std::setprecision(1) << panchanga.dayLength << " hours\n";
    oss << "   Night Length: " << std::fixed << std::setprecision(1) << panchanga.nightLength << " hours\n\n";

    oss << "🌞 CELESTIAL POSITIONS:\n";
    oss << "   Sun in:   " << getRashiName(panchanga.sunRashi)
        << " (" << std::fixed << std::setprecision(2) << panchanga.sunLongitude << "°)\n";
    oss << "   Moon in:  " << getRashiName(panchanga.moonRashi)
        << " (" << std::fixed << std::setprecision(2) << panchanga.moonLongitude << "°)\n";
    oss << "   Lunar Phase: " << std::fixed << std::setprecision(1) << panchanga.lunarPhase << "°\n";
    oss << "   Ritu: " << panchanga.ritu << "\n";
    oss << "   Ayana: " << panchanga.ayana << "\n\n";

    oss << "🕐 AUSPICIOUS TIMINGS:\n";
    oss << "   Brahma Muhurta: " << panchanga.getTimeString(panchanga.brahmaMuhurtaStart)
        << " to " << panchanga.getTimeString(panchanga.brahmaMuhurtaEnd) << "\n";
    oss << "   Abhijit Muhurta: " << panchanga.getTimeString(panchanga.abhijitStart)
        << " to " << panchanga.getTimeString(panchanga.abhijitEnd) << "\n";
    oss << "   Godhuli Bela: " << panchanga.getTimeString(panchanga.godhuliBelStart)
        << " to " << panchanga.getTimeString(panchanga.godhuliBelEnd) << "\n";
    oss << "   Nishita Muhurta: " << panchanga.getTimeString(panchanga.nishitaMuhurtaStart)
        << " to " << panchanga.getTimeString(panchanga.nishitaMuhurtaEnd) << "\n\n";

    oss << "⚠️  INAUSPICIOUS TIMINGS:\n";
    oss << "   Rahu Kaal: " << panchanga.getTimeString(panchanga.rahuKaalStart)
        << " to " << panchanga.getTimeString(panchanga.rahuKaalEnd) << "\n";
    oss << "   Yamaganda: " << panchanga.getTimeString(panchanga.yamagandaStart)
        << " to " << panchanga.getTimeString(panchanga.yamagandaEnd) << "\n";
    oss << "   Gulikai: " << panchanga.getTimeString(panchanga.gulikaiStart)
        << " to " << panchanga.getTimeString(panchanga.gulikaiEnd) << "\n";
    oss << "   Dur Muhurtam: " << panchanga.getTimeString(panchanga.durMuhurtamStart)
        << " to " << panchanga.getTimeString(panchanga.durMuhurtamEnd) << "\n";

    if (!panchanga.varjyamTimes.empty()) {
        oss << "   Varjyam: ";
        for (size_t i = 0; i < panchanga.varjyamTimes.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << panchanga.getTimeString(panchanga.varjyamTimes[i].first)
                << " to " << panchanga.getTimeString(panchanga.varjyamTimes[i].second);
        }
        oss << "\n";
    }
    oss << "\n";

    oss << "🧭 SHOOL DIRECTIONS:\n";
    oss << "   Disha Shool: " << panchanga.dishaShool << "\n";
    oss << "   Nakshatra Shool: " << panchanga.nakshatraShool << "\n\n";

    if (panchanga.isEkadashi || panchanga.isPurnima || panchanga.isAmavasya ||
        panchanga.isSankranti || panchanga.isNavratri || panchanga.isGandaMool ||
        panchanga.isPanchak || panchanga.isBhadra) {
        oss << "🌙 SPECIAL OBSERVANCES:\n";
        if (panchanga.isEkadashi) oss << "   • Ekadashi - Fasting day\n";
        if (panchanga.isPurnima) oss << "   • Purnima - Full Moon\n";
        if (panchanga.isAmavasya) oss << "   • Amavasya - New Moon\n";
        if (panchanga.isSankranti) oss << "   • Sankranti - Solar transition\n";
        if (panchanga.isNavratri) oss << "   • Navratri period\n";
        if (panchanga.isGandaMool) oss << "   • Ganda Mool Nakshatra\n";
        if (panchanga.isPanchak) oss << "   • Panchak period\n";
        if (panchanga.isBhadra) oss << "   • Bhadra (Vishti Karana)\n";
        oss << "\n";
    }

    if (panchanga.isSarvarthaSiddhi || panchanga.isAmritaSiddhi || panchanga.isDwipushkar ||
        panchanga.isTripushkar || panchanga.isRaviPushya || panchanga.isGuruPushya) {
        oss << "✨ SPECIAL YOGAS:\n";
        if (panchanga.isSarvarthaSiddhi) oss << "   • Sarvartha Siddhi Yoga\n";
        if (panchanga.isAmritaSiddhi) oss << "   • Amrita Siddhi Yoga\n";
        if (panchanga.isDwipushkar) oss << "   • Dwipushkar Yoga\n";
        if (panchanga.isTripushkar) oss << "   • Tripushkar Yoga\n";
        if (panchanga.isRaviPushya) oss << "   • Ravi Pushya Yoga\n";
        if (panchanga.isGuruPushya) oss << "   • Guru Pushya Yoga\n";
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

    if (!panchanga.vrataList.empty()) {
        oss << "🙏 VRATA & UPAVAS:\n";
        for (const auto& vrata : panchanga.vrataList) {
            oss << "   • " << vrata << "\n";
        }
        if (panchanga.isFastingDay) {
            oss << "   ⚠️  Fasting Day - Observe dietary restrictions\n";
        }
        oss << "\n";
    }

    if (!panchanga.goodChandraBalam.empty()) {
        oss << "🌛 GOOD CHANDRA BALAM FOR:\n   ";
        for (size_t i = 0; i < panchanga.goodChandraBalam.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << getRashiName(panchanga.goodChandraBalam[i]);
        }
        oss << "\n\n";
    }

    if (!panchanga.goodTaraBalam.empty()) {
        oss << "⭐ GOOD TARA BALAM FOR:\n   ";
        for (size_t i = 0; i < panchanga.goodTaraBalam.size() && i < 5; ++i) {
            if (i > 0) oss << ", ";
            oss << getNakshatraName(panchanga.goodTaraBalam[i]);
        }
        if (panchanga.goodTaraBalam.size() > 5) {
            oss << " and " << (panchanga.goodTaraBalam.size() - 5) << " more";
        }
        oss << "\n\n";
    }

    oss << "🕐 MUHURTA ASSESSMENT:\n";
    oss << "   Status: " << (panchanga.isShubhaMuhurta ? "✅ Shubha (Auspicious)" : "⚠️  Exercise Caution") << "\n";
    oss << "   Note: " << panchanga.muhurtaDescription << "\n\n";

    oss << "══════════════════════════════════════════════════════════════════════════════════\n";
    oss << "                         Generated with Enhanced Hindu Calendar\n";
    oss << "══════════════════════════════════════════════════════════════════════════════════\n";

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
    oss << "  \"panchangaBasics\": {\n";
    oss << "    \"tithi\": {\n";
    oss << "      \"number\": " << static_cast<int>(panchanga.tithi) << ",\n";
    oss << "      \"name\": \"" << getTithiName(panchanga.tithi) << "\",\n";
    oss << "      \"endTime\": \"" << panchanga.getTimeString(panchanga.tithiEndTime) << "\"\n";
    oss << "    },\n";
    oss << "    \"vara\": {\n";
    oss << "      \"number\": " << static_cast<int>(panchanga.vara) << ",\n";
    oss << "      \"name\": \"" << getVaraName(panchanga.vara) << "\"\n";
    oss << "    },\n";
    oss << "    \"nakshatra\": {\n";
    oss << "      \"number\": " << static_cast<int>(panchanga.nakshatra) << ",\n";
    oss << "      \"name\": \"" << getNakshatraName(panchanga.nakshatra) << "\",\n";
    oss << "      \"pada\": " << panchanga.nakshatraPada << ",\n";
    oss << "      \"endTime\": \"" << panchanga.getTimeString(panchanga.nakshatraEndTime) << "\"\n";
    oss << "    },\n";
    oss << "    \"yoga\": {\n";
    oss << "      \"number\": " << static_cast<int>(panchanga.yoga) << ",\n";
    oss << "      \"name\": \"" << getYogaName(panchanga.yoga) << "\",\n";
    oss << "      \"endTime\": \"" << panchanga.getTimeString(panchanga.yogaEndTime) << "\"\n";
    oss << "    },\n";
    oss << "    \"karana\": {\n";
    oss << "      \"number\": " << static_cast<int>(panchanga.karana) << ",\n";
    oss << "      \"name\": \"" << getKaranaName(panchanga.karana) << "\",\n";
    oss << "      \"endTime\": \"" << panchanga.getTimeString(panchanga.karanaEndTime) << "\"\n";
    oss << "    }\n";
    oss << "  },\n";

    oss << "  \"dateInformation\": {\n";
    oss << "    \"vikramYear\": " << panchanga.vikramYear << ",\n";
    oss << "    \"shakaYear\": " << panchanga.shakaYear << ",\n";
    oss << "    \"kaliyugaYear\": " << panchanga.kaliyugaYear << ",\n";
    oss << "    \"month\": \"" << getHinduMonthName(panchanga.month) << "\",\n";
    oss << "    \"paksha\": \"" << (panchanga.isShukla ? "Shukla" : "Krishna") << "\",\n";
    oss << "    \"julianDay\": " << std::fixed << std::setprecision(1) << panchanga.julianDay << "\n";
    oss << "  },\n";

    oss << "  \"sunMoonInformation\": {\n";
    oss << "    \"sunrise\": \"" << panchanga.getTimeString(panchanga.sunriseTime) << "\",\n";
    oss << "    \"sunset\": \"" << panchanga.getTimeString(panchanga.sunsetTime) << "\",\n";
    oss << "    \"moonrise\": \"" << panchanga.getTimeString(panchanga.moonriseTime) << "\",\n";
    oss << "    \"moonset\": \"" << panchanga.getTimeString(panchanga.moonsetTime) << "\",\n";
    oss << "    \"dayLength\": " << std::fixed << std::setprecision(2) << panchanga.dayLength << ",\n";
    oss << "    \"nightLength\": " << std::fixed << std::setprecision(2) << panchanga.nightLength << "\n";
    oss << "  },\n";

    oss << "  \"celestial\": {\n";
    oss << "    \"sunLongitude\": " << std::fixed << std::setprecision(4) << panchanga.sunLongitude << ",\n";
    oss << "    \"moonLongitude\": " << std::fixed << std::setprecision(4) << panchanga.moonLongitude << ",\n";
    oss << "    \"lunarPhase\": " << std::fixed << std::setprecision(4) << panchanga.lunarPhase << ",\n";
    oss << "    \"sunRashi\": \"" << getRashiName(panchanga.sunRashi) << "\",\n";
    oss << "    \"moonRashi\": \"" << getRashiName(panchanga.moonRashi) << "\",\n";
    oss << "    \"ayanamsaValue\": " << std::fixed << std::setprecision(6) << panchanga.ayanamsaValue << ",\n";
    oss << "    \"ritu\": \"" << panchanga.ritu << "\",\n";
    oss << "    \"ayana\": \"" << panchanga.ayana << "\"\n";
    oss << "  },\n";

    oss << "  \"auspiciousTimings\": {\n";
    oss << "    \"brahmaMuhurta\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.brahmaMuhurtaStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.brahmaMuhurtaEnd) << "\"\n";
    oss << "    },\n";
    oss << "    \"abhijitMuhurta\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.abhijitStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.abhijitEnd) << "\"\n";
    oss << "    },\n";
    oss << "    \"godhuliBela\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.godhuliBelStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.godhuliBelEnd) << "\"\n";
    oss << "    },\n";
    oss << "    \"nishitaMuhurta\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.nishitaMuhurtaStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.nishitaMuhurtaEnd) << "\"\n";
    oss << "    }\n";
    oss << "  },\n";

    oss << "  \"inauspiciousTimings\": {\n";
    oss << "    \"rahuKaal\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.rahuKaalStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.rahuKaalEnd) << "\"\n";
    oss << "    },\n";
    oss << "    \"yamaganda\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.yamagandaStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.yamagandaEnd) << "\"\n";
    oss << "    },\n";
    oss << "    \"gulikai\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.gulikaiStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.gulikaiEnd) << "\"\n";
    oss << "    },\n";
    oss << "    \"durMuhurtam\": {\n";
    oss << "      \"start\": \"" << panchanga.getTimeString(panchanga.durMuhurtamStart) << "\",\n";
    oss << "      \"end\": \"" << panchanga.getTimeString(panchanga.durMuhurtamEnd) << "\"\n";
    oss << "    },\n";
    oss << "    \"varjyam\": [";
    for (size_t i = 0; i < panchanga.varjyamTimes.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "{\n";
        oss << "        \"start\": \"" << panchanga.getTimeString(panchanga.varjyamTimes[i].first) << "\",\n";
        oss << "        \"end\": \"" << panchanga.getTimeString(panchanga.varjyamTimes[i].second) << "\"\n";
        oss << "      }";
    }
    oss << "]\n";
    oss << "  },\n";

    oss << "  \"specialDays\": {\n";
    oss << "    \"isEkadashi\": " << (panchanga.isEkadashi ? "true" : "false") << ",\n";
    oss << "    \"isPurnima\": " << (panchanga.isPurnima ? "true" : "false") << ",\n";
    oss << "    \"isAmavasya\": " << (panchanga.isAmavasya ? "true" : "false") << ",\n";
    oss << "    \"isSankranti\": " << (panchanga.isSankranti ? "true" : "false") << ",\n";
    oss << "    \"isNavratri\": " << (panchanga.isNavratri ? "true" : "false") << ",\n";
    oss << "    \"isGandaMool\": " << (panchanga.isGandaMool ? "true" : "false") << ",\n";
    oss << "    \"isPanchak\": " << (panchanga.isPanchak ? "true" : "false") << ",\n";
    oss << "    \"isBhadra\": " << (panchanga.isBhadra ? "true" : "false") << ",\n";
    oss << "    \"isFastingDay\": " << (panchanga.isFastingDay ? "true" : "false") << "\n";
    oss << "  },\n";

    oss << "  \"specialYogas\": {\n";
    oss << "    \"isSarvarthaSiddhi\": " << (panchanga.isSarvarthaSiddhi ? "true" : "false") << ",\n";
    oss << "    \"isAmritaSiddhi\": " << (panchanga.isAmritaSiddhi ? "true" : "false") << ",\n";
    oss << "    \"isDwipushkar\": " << (panchanga.isDwipushkar ? "true" : "false") << ",\n";
    oss << "    \"isTripushkar\": " << (panchanga.isTripushkar ? "true" : "false") << ",\n";
    oss << "    \"isRaviPushya\": " << (panchanga.isRaviPushya ? "true" : "false") << ",\n";
    oss << "    \"isGuruPushya\": " << (panchanga.isGuruPushya ? "true" : "false") << "\n";
    oss << "  },\n";

    oss << "  \"directions\": {\n";
    oss << "    \"dishaShool\": \"" << panchanga.dishaShool << "\",\n";
    oss << "    \"nakshatraShool\": \"" << panchanga.nakshatraShool << "\"\n";
    oss << "  },\n";

    oss << "  \"festivals\": [";
    for (size_t i = 0; i < panchanga.festivals.size(); ++i) {
        oss << "\"" << panchanga.festivals[i] << "\"";
        if (i < panchanga.festivals.size() - 1) oss << ", ";
    }
    oss << "],\n";

    oss << "  \"ekadashiNames\": [";
    for (size_t i = 0; i < panchanga.ekadashiNames.size(); ++i) {
        oss << "\"" << panchanga.ekadashiNames[i] << "\"";
        if (i < panchanga.ekadashiNames.size() - 1) oss << ", ";
    }
    oss << "],\n";

    oss << "  \"vrataUpavas\": [";
    for (size_t i = 0; i < panchanga.vrataList.size(); ++i) {
        oss << "\"" << panchanga.vrataList[i] << "\"";
        if (i < panchanga.vrataList.size() - 1) oss << ", ";
    }
    oss << "],\n";

    oss << "  \"chandraBalam\": [";
    for (size_t i = 0; i < panchanga.goodChandraBalam.size(); ++i) {
        oss << "\"" << getRashiName(panchanga.goodChandraBalam[i]) << "\"";
        if (i < panchanga.goodChandraBalam.size() - 1) oss << ", ";
    }
    oss << "],\n";

    oss << "  \"taraBalam\": [";
    for (size_t i = 0; i < panchanga.goodTaraBalam.size(); ++i) {
        oss << "\"" << getNakshatraName(panchanga.goodTaraBalam[i]) << "\"";
        if (i < panchanga.goodTaraBalam.size() - 1) oss << ", ";
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
