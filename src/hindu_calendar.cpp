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

        // Calculate Hindu day based on tithi
        int tithiNum = static_cast<int>(panchanga.tithi);
        if (panchanga.isShukla) {
            panchanga.day = tithiNum; // Shukla paksha: 1-15
        } else {
            panchanga.day = tithiNum - 15; // Krishna paksha: 1-15 (tithi 16-30 becomes day 1-15)
        }

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
        calculateVarnaInformation(panchanga);  // Calculate Savarna days
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
    double gTime;
    swe_revjul(jd, SE_GREG_CAL, &year, &month, &day, &gTime);
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
    // Enhanced Sankranti detection with exact timing
    double sunLongMod30 = std::fmod(panchanga.sunLongitude, 30.0);
    bool isSankrantiClose = (sunLongMod30 < 2.0) || (sunLongMod30 > 28.0);

    if (isSankrantiClose) {
        panchanga.isSankranti = true;
        panchanga.isSankrantiToday = isSankrantiOccurringToday(panchanga.julianDay, 1.0); // Increased tolerance

        // Determine which Sankranti this is
        std::string rashiName = getRashiName(panchanga.sunRashi);
        size_t spacePos = rashiName.find(' ');
        if (spacePos != std::string::npos) {
            rashiName = rashiName.substr(0, spacePos);
        }
        panchanga.sankrantiName = rashiName + " Sankranti";

        // Always calculate exact Sankranti time when close to transition
        Rashi nextRashi = static_cast<Rashi>((static_cast<int>(panchanga.sunRashi) % 12) + 1);
        panchanga.sankrantiTime = calculateSankrantiTime(panchanga.julianDay, panchanga.sunRashi, nextRashi);

        // Enhanced Sankranti detection: check if transition occurred within last 24 hours
        bool sankrantiWithinDay = false;
        if (panchanga.sankrantiTime > 0) {
            sankrantiWithinDay = true;
        } else {
            // If current method fails, try checking if we just crossed the boundary
            if (sunLongMod30 < 5.0) { // Just entered new rashi
                sankrantiWithinDay = true;
                panchanga.sankrantiTime = 6.0; // Approximate early morning time
            }
        }

        // Note: Sankranti information is displayed in Special Observances section
        // No need to add duplicate entries to Special Events
    }

    // Calculate next Sankranti information
    double daysUntil = 0.0;
    panchanga.nextSankranti = calculateNextSankranti(panchanga.julianDay, daysUntil);
    panchanga.daysToNextSankranti = daysUntil;

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
        double geopos[3] = {longitude, latitude, 0.0}; // longitude, latitude, altitude in meters
        double riseSetTime;

        // Calculate timezone offset based on longitude
        // Standard timezone offset = longitude / 15.0 (rough approximation)
        double timezoneOffset = 0.0;

        // More precise timezone adjustments based on longitude ranges and actual timezone boundaries
        if (longitude >= 97.5 && longitude <= 106.0) {
            // Thailand (Bangkok: 100.5°E), Laos, Cambodia, Vietnam
            timezoneOffset = 7.0; // ICT (UTC+7)
        } else if (longitude >= 68.0 && longitude <= 97.4) {
            // India, Sri Lanka, Bangladesh
            timezoneOffset = 5.5; // IST (UTC+5:30)
        } else if (longitude >= 106.1 && longitude <= 120.0) {
            // Malaysia, Singapore, Philippines, Brunei, parts of Indonesia
            timezoneOffset = 8.0; // MYT/SGT/PHT (UTC+8)
        } else if (longitude >= 120.1 && longitude <= 135.0) {
            // China, Taiwan, Western Australia
            timezoneOffset = 8.0; // CST/AWST (UTC+8)
        } else if (longitude >= 135.1 && longitude <= 150.0) {
            // Japan, Korea, Eastern Australia
            timezoneOffset = 9.0; // JST/KST/AEST (UTC+9)
        } else if (longitude >= 60.0 && longitude <= 67.9) {
            // Afghanistan, parts of Pakistan
            timezoneOffset = 4.5; // AFT (UTC+4:30)
        } else if (longitude >= 45.0 && longitude <= 59.9) {
            // Gulf states, parts of Russia
            timezoneOffset = 4.0; // GST (UTC+4)
        } else {
            // Default calculation: longitude-based approximation
            timezoneOffset = round(longitude / 15.0);
            // Clamp to reasonable range
            if (timezoneOffset < -12.0) timezoneOffset = -12.0;
            if (timezoneOffset > 14.0) timezoneOffset = 14.0;
        }

        // Get the Julian Day for the start of the day (midnight UTC)
        double julianDayStart = floor(panchanga.julianDay) + 0.5; // Start of day in UTC

        // Enhanced Swiss Ephemeris calculations with seasonal atmospheric corrections
        // Use topocentric calculations and location-specific atmospheric conditions
        double geoposEnhanced[3] = {longitude, latitude, 0.0}; // Use actual altitude (sea level default)

        // Get seasonal atmospheric parameters based on Swiss Ephemeris documentation
        int year, month, day;
        double hour;
        swe_revjul(panchanga.julianDay, SE_GREG_CAL, &year, &month, &day, &hour);

        double pressure, temperature;
        getSeasonalAtmosphericParams(month, latitude, longitude, &pressure, &temperature);

        // Enhanced calculation flags for maximum precision (Swiss Ephemeris documentation)
        int32 ephemerisFlags = SEFLG_SWIEPH | SEFLG_TOPOCTR | SEFLG_SPEED;

        // Calculate sunrise with seasonal atmospheric correction
        int result = swe_rise_trans(julianDayStart, SE_SUN, nullptr,
                                   ephemerisFlags, // Enhanced precision flags
                                   SE_CALC_RISE | SE_BIT_DISC_BOTTOM, // Bottom of solar disc
                                   geoposEnhanced, pressure, temperature, // Seasonal atmospheric parameters
                                   &riseSetTime, errorString);
        if (result >= 0) {
            // Convert to local solar time
            double timeOffset = riseSetTime - julianDayStart;
            double hoursFromMidnight = timeOffset * 24.0;

            // Apply timezone correction
            panchanga.sunriseTime = hoursFromMidnight + timezoneOffset;

            // Normalize to 0-24 range
            while (panchanga.sunriseTime < 0) panchanga.sunriseTime += 24.0;
            while (panchanga.sunriseTime >= 24.0) panchanga.sunriseTime -= 24.0;
        } else {
            handleCalculationError(result, errorString, panchanga, "sunrise", latitude);
        }

        // Calculate sunset with seasonal atmospheric correction
        result = swe_rise_trans(julianDayStart, SE_SUN, nullptr,
                               ephemerisFlags, // Enhanced precision flags
                               SE_CALC_SET | SE_BIT_DISC_BOTTOM, // Bottom of solar disc
                               geoposEnhanced, pressure, temperature, // Seasonal atmospheric parameters
                               &riseSetTime, errorString);
        if (result >= 0) {
            double timeOffset = riseSetTime - julianDayStart;
            double hoursFromMidnight = timeOffset * 24.0;

            panchanga.sunsetTime = hoursFromMidnight + timezoneOffset;

            while (panchanga.sunsetTime < 0) panchanga.sunsetTime += 24.0;
            while (panchanga.sunsetTime >= 24.0) panchanga.sunsetTime -= 24.0;
        } else {
            handleCalculationError(result, errorString, panchanga, "sunset", latitude);
        }

        // Calculate moonrise with seasonal atmospheric correction
        result = swe_rise_trans(julianDayStart, SE_MOON, nullptr,
                               ephemerisFlags, // Enhanced precision flags
                               SE_CALC_RISE | SE_BIT_DISC_CENTER, // Center of lunar disc
                               geoposEnhanced, pressure, temperature, // Seasonal atmospheric parameters
                               &riseSetTime, errorString);
        if (result >= 0) {
            double timeOffset = riseSetTime - julianDayStart;
            double hoursFromMidnight = timeOffset * 24.0;

            panchanga.moonriseTime = hoursFromMidnight + timezoneOffset;

            while (panchanga.moonriseTime < 0) panchanga.moonriseTime += 24.0;
            while (panchanga.moonriseTime >= 24.0) panchanga.moonriseTime -= 24.0;
        } else {
            handleCalculationError(result, errorString, panchanga, "moonrise", latitude);
        }

        // Calculate moonset with seasonal atmospheric correction
        result = swe_rise_trans(julianDayStart, SE_MOON, nullptr,
                               ephemerisFlags, // Enhanced precision flags
                               SE_CALC_SET | SE_BIT_DISC_CENTER, // Center of lunar disc
                               geoposEnhanced, pressure, temperature, // Seasonal atmospheric parameters
                               &riseSetTime, errorString);
        if (result >= 0) {
            double timeOffset = riseSetTime - julianDayStart;
            double hoursFromMidnight = timeOffset * 24.0;

            panchanga.moonsetTime = hoursFromMidnight + timezoneOffset;

            while (panchanga.moonsetTime < 0) panchanga.moonsetTime += 24.0;
            while (panchanga.moonsetTime >= 24.0) panchanga.moonsetTime -= 24.0;
        } else {
            handleCalculationError(result, errorString, panchanga, "moonset", latitude);
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
        handleException(e, panchanga);
    }
}

void HinduCalendar::getSeasonalAtmosphericParams(int month, double latitude, double longitude,
                                                double* pressure, double* temperature) const {
    // Swiss Ephemeris seasonal atmospheric corrections based on geographical location

    if (latitude >= 10.0 && latitude <= 20.0 && longitude >= 97.0 && longitude <= 106.0) {
        // Bangkok/Thailand tropical climate (13.7°N, 100.5°E)
        if (month >= 3 && month <= 5) {
            // Hot season (March-May): lower pressure, higher temperature
            *pressure = 1010.0;
            *temperature = 32.0;
        } else if (month >= 6 && month <= 10) {
            // Rainy season (June-October): lowest pressure, moderate temperature
            *pressure = 1008.0;
            *temperature = 28.0;
        } else {
            // Cool season (November-February): higher pressure, cooler temperature
            *pressure = 1015.0;
            *temperature = 24.0;
        }
    } else if (latitude >= 8.0 && latitude <= 28.0 && longitude >= 68.0 && longitude <= 97.0) {
        // India/Sri Lanka monsoon climate
        if (month >= 4 && month <= 6) {
            // Pre-monsoon hot season
            *pressure = 1008.0;
            *temperature = 35.0;
        } else if (month >= 7 && month <= 9) {
            // Monsoon season
            *pressure = 1005.0;
            *temperature = 30.0;
        } else if (month >= 10 && month <= 11) {
            // Post-monsoon
            *pressure = 1012.0;
            *temperature = 28.0;
        } else {
            // Winter season
            *pressure = 1018.0;
            *temperature = 20.0;
        }
    } else if (latitude >= 30.0 || latitude <= -30.0) {
        // Temperate zones with strong seasonal variation
        bool isNorthernHemisphere = latitude > 0;
        int effectiveMonth = isNorthernHemisphere ? month : ((month + 6 - 1) % 12) + 1;

        if (effectiveMonth >= 6 && effectiveMonth <= 8) {
            // Summer
            *pressure = 1010.0;
            *temperature = 25.0;
        } else if (effectiveMonth >= 12 || effectiveMonth <= 2) {
            // Winter
            *pressure = 1020.0;
            *temperature = 0.0;
        } else {
            // Spring/Autumn
            *pressure = 1015.0;
            *temperature = 15.0;
        }
    } else {
        // Default: Use Swiss Ephemeris standard atmospheric conditions
        *pressure = 1013.25; // Standard sea level pressure
        *temperature = 10.0;  // Standard temperature
    }
}

void HinduCalendar::handleCalculationError(int result, const char* errorString,
                                          PanchangaData& panchanga, const std::string& calculation,
                                          double latitude) const {
    if (result == -4) {
        // No rise/set found - could be polar regions or extreme conditions
        if (std::abs(latitude) > 66.5) {
            // Polar regions: handle midnight sun or polar night
            handlePolarConditions(panchanga, calculation, latitude);
        } else {
            // Unusual condition - use reasonable fallback
            setReasonableFallback(panchanga, calculation);
            std::cerr << "Warning: No " << calculation << " found for location (lat: "
                     << latitude << "). Using fallback value." << std::endl;
        }
    } else if (result == -1) {
        // Date out of Swiss Ephemeris range (13,000 BCE to 17,000 CE)
        std::cerr << "Error: Date outside Swiss Ephemeris range for " << calculation << std::endl;
        setReasonableFallback(panchanga, calculation);
    } else if (result == -2) {
        // Ephemeris file not found
        std::cerr << "Error: Swiss Ephemeris files not found for " << calculation << std::endl;
        setReasonableFallback(panchanga, calculation);
    } else {
        // Other Swiss Ephemeris errors
        std::cerr << "Swiss Ephemeris " << calculation << " error: " << errorString << std::endl;
        setReasonableFallback(panchanga, calculation);
    }
}

void HinduCalendar::handlePolarConditions(PanchangaData& panchanga, const std::string& calculation,
                                         double latitude) const {
    // Handle polar regions where sun may not rise/set for extended periods
    if (calculation == "sunrise" || calculation == "sunset") {
        if (latitude > 66.5) {
            // Arctic: assume midnight sun in summer, polar night in winter
            // Get month to determine season
            int year, month, day;
            double hour;
            swe_revjul(panchanga.julianDay, SE_GREG_CAL, &year, &month, &day, &hour);

            if (month >= 4 && month <= 8) {
                // Summer: midnight sun
                panchanga.sunriseTime = 0.0;  // Sun never sets
                panchanga.sunsetTime = 23.99;
            } else {
                // Winter: polar night
                panchanga.sunriseTime = 12.0; // Nominal noon
                panchanga.sunsetTime = 12.0;  // Sun never rises
            }
        } else {
            // Antarctic (similar but opposite seasons)
            setReasonableFallback(panchanga, calculation);
        }
    } else {
        // Moon calculations in polar regions
        setReasonableFallback(panchanga, calculation);
    }
}

void HinduCalendar::setReasonableFallback(PanchangaData& panchanga, const std::string& calculation) const {
    // Set reasonable fallback values based on calculation type
    if (calculation == "sunrise") {
        panchanga.sunriseTime = 6.0;
    } else if (calculation == "sunset") {
        panchanga.sunsetTime = 18.0;
    } else if (calculation == "moonrise") {
        panchanga.moonriseTime = 8.0;
    } else if (calculation == "moonset") {
        panchanga.moonsetTime = 20.0;
    }
}

void HinduCalendar::handleException(const std::exception& e, PanchangaData& panchanga) const {
    std::cerr << "Exception in calculateSunMoonTimes: " << e.what() << std::endl;

    // Set safe default values
    panchanga.sunriseTime = 6.0;
    panchanga.sunsetTime = 18.0;
    panchanga.moonriseTime = 7.0;
    panchanga.moonsetTime = 19.0;
    panchanga.dayLength = 12.0;
    panchanga.nightLength = 12.0;
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

        // Ensure times are in valid range
        while (panchanga.rahuKaalStart >= 24.0) panchanga.rahuKaalStart -= 24.0;
        while (panchanga.rahuKaalEnd >= 24.0) panchanga.rahuKaalEnd -= 24.0;
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

        while (panchanga.yamagandaStart >= 24.0) panchanga.yamagandaStart -= 24.0;
        while (panchanga.yamagandaEnd >= 24.0) panchanga.yamagandaEnd -= 24.0;
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

        while (panchanga.gulikaiStart >= 24.0) panchanga.gulikaiStart -= 24.0;
        while (panchanga.gulikaiEnd >= 24.0) panchanga.gulikaiEnd -= 24.0;
    }
}

void HinduCalendar::calculateDurMuhurtam(PanchangaData& panchanga) const {
    // Dur Muhurtam is typically around midday
    double dayCenter = panchanga.sunriseTime + (panchanga.dayLength / 2.0);
    panchanga.durMuhurtamStart = dayCenter - 0.75; // 45 minutes before midday
    panchanga.durMuhurtamEnd = dayCenter + 0.75;   // 45 minutes after midday

    while (panchanga.durMuhurtamStart < 0) panchanga.durMuhurtamStart += 24.0;
    while (panchanga.durMuhurtamStart >= 24.0) panchanga.durMuhurtamStart -= 24.0;
    while (panchanga.durMuhurtamEnd >= 24.0) panchanga.durMuhurtamEnd -= 24.0;
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

void HinduCalendar::calculateVarnaInformation(PanchangaData& panchanga) const {
    // Calculate Varna based on weekday (traditional classification)
    int weekday = static_cast<int>(panchanga.vara);
    std::vector<std::string> dayVarnas = {
        "Kshatriya",  // Sunday - ruled by Sun (royal/warrior)
        "Vaishya",    // Monday - ruled by Moon (merchant/agricultural)
        "Kshatriya",  // Tuesday - ruled by Mars (warrior)
        "Brahmin",    // Wednesday - ruled by Mercury (intellectual/priestly)
        "Brahmin",    // Thursday - ruled by Jupiter (spiritual/learned)
        "Vaishya",    // Friday - ruled by Venus (artistic/commercial)
        "Shudra"      // Saturday - ruled by Saturn (service/labor)
    };

    if (weekday >= 0 && weekday <= 6) {
        panchanga.varnaDay = dayVarnas[weekday];
    }

    // Calculate Varna based on Tithi (traditional lunar day classification)
    int tithiNum = static_cast<int>(panchanga.tithi);
    std::string tithiVarna;

    if (tithiNum >= 1 && tithiNum <= 30) {
        // Simplify tithi to 1-15 range for both pakshas
        int simpleTithi = ((tithiNum - 1) % 15) + 1;

        if (simpleTithi >= 1 && simpleTithi <= 3) {
            tithiVarna = "Brahmin";     // Beginning tithis - spiritual
        } else if (simpleTithi >= 4 && simpleTithi <= 7) {
            tithiVarna = "Kshatriya";   // Middle tithis - active/royal
        } else if (simpleTithi >= 8 && simpleTithi <= 11) {
            tithiVarna = "Vaishya";     // Later tithis - commercial
        } else if (simpleTithi >= 12 && simpleTithi <= 15) {
            tithiVarna = "Shudra";      // Final tithis - service/completion
        }
    }
    panchanga.varnaTithi = tithiVarna;

    // Calculate Varna based on Nakshatra (based on traditional gana classification)
    int nakNum = static_cast<int>(panchanga.nakshatra);
    std::string nakshatraVarna;

    // Classification based on traditional nakshatra gana and nature
    std::vector<std::string> nakVarnas = {
        "Kshatriya", // Ashwini - Swift, royal
        "Shudra",    // Bharani - Service, death deity
        "Kshatriya", // Krittika - Fire, sharp
        "Vaishya",   // Rohini - Beauty, commerce
        "Vaishya",   // Mrigashira - Soft, seeking
        "Shudra",    // Ardra - Sharp, destructive
        "Brahmin",   // Punarvasu - Divine, renewal
        "Brahmin",   // Pushya - Divine, nourishing
        "Shudra",    // Ashlesha - Sharp, serpent
        "Kshatriya", // Magha - Royal, fierce
        "Kshatriya", // Purva Phalguni - Royal, fierce
        "Kshatriya", // Uttara Phalguni - Royal, fixed
        "Brahmin",   // Hasta - Divine, skillful
        "Shudra",    // Chitra - Mixed, artistic
        "Brahmin",   // Swati - Divine, independent
        "Shudra",    // Vishakha - Mixed, forked
        "Brahmin",   // Anuradha - Divine, soft
        "Shudra",    // Jyeshtha - Sharp, competitive
        "Shudra",    // Mula - Sharp, destructive
        "Kshatriya", // Purva Ashadha - Fierce, invincible
        "Kshatriya", // Uttara Ashadha - Fixed, victory
        "Brahmin",   // Shravana - Divine, learning
        "Kshatriya", // Dhanishta - Royal, wealthy
        "Shudra",    // Shatabhisha - Independent, healing
        "Kshatriya", // Purva Bhadrapada - Fierce, transformative
        "Kshatriya", // Uttara Bhadrapada - Fixed, depth
        "Brahmin"    // Revati - Divine, nourishing
    };

    if (nakNum >= 1 && nakNum <= 27) {
        panchanga.varnaNakshatra = nakVarnas[nakNum - 1];
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
    double time;
    if (isStart) {
        time = sunriseTime - 1.5;
    } else {
        time = sunriseTime - 0.7; // 42 minutes duration
    }

    // Handle negative times (previous day)
    while (time < 0) time += 24.0;
    while (time >= 24.0) time -= 24.0;
    return time;
}

double HinduCalendar::calculateAbhijitMuhurta(double sunriseTime, double sunsetTime, bool isStart) const {
    // Abhijit is in the middle of the day, lasting for 48 minutes
    double midday = sunriseTime + ((sunsetTime - sunriseTime) / 2.0);
    double time;
    if (isStart) {
        time = midday - 0.4; // 24 minutes before midday
    } else {
        time = midday + 0.4; // 24 minutes after midday
    }

    while (time < 0) time += 24.0;
    while (time >= 24.0) time -= 24.0;
    return time;
}

double HinduCalendar::calculateGodhuliBela(double sunsetTime, bool isStart) const {
    // Godhuli Bela is around sunset time
    double time;
    if (isStart) {
        time = sunsetTime - 0.25; // 15 minutes before sunset
    } else {
        time = sunsetTime + 0.25; // 15 minutes after sunset
    }

    while (time < 0) time += 24.0;
    while (time >= 24.0) time -= 24.0;
    return time;
}

double HinduCalendar::calculateNishitaMuhurta(double sunsetTime, double nextSunriseTime, bool isStart) const {
    // Nishita Muhurta is around midnight
    double midnight = sunsetTime + ((nextSunriseTime - sunsetTime) / 2.0);
    double time;
    if (isStart) {
        time = midnight - 0.4; // 24 minutes before midnight
    } else {
        time = midnight + 0.4; // 24 minutes after midnight
    }

    while (time < 0) time += 24.0;
    while (time >= 24.0) time -= 24.0;
    return time;
}

// Formatting methods
std::string PanchangaData::getTimeString(double hours) const {
    if (hours < 0 || hours >= 24) {
        hours = fmod(hours, 24.0);
        if (hours < 0) hours += 24.0;
    }

    int h = static_cast<int>(hours);
    int m = static_cast<int>((hours - h) * 60);
    int s = static_cast<int>(((hours - h) * 60 - m) * 60);

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", h, m, s);
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

    // Two-column layout for basic information
    oss << "+------------------------------------------------------------+------------------------------------------------------------+\n";
    oss << "|                        📊 CALCULATION                       |                         📅 DATE INFO                       |\n";
    oss << "+------------------------------------------------------------+------------------------------------------------------------+\n";

    std::string ayanamsaName = getAyanamsaName();
    if (ayanamsaName.length() > 46) ayanamsaName = ayanamsaName.substr(0, 46);
    oss << "│ Ayanamsa: " << std::left << std::setw(48) << ayanamsaName
        << "│ Vikram Samvat: " << std::left << std::setw(42) << panchanga.vikramYear << "│\n";

    std::string methodName = getCalculationMethodName();
    if (methodName.length() > 50) methodName = methodName.substr(0, 50);
    oss << "│ Method: " << std::left << std::setw(50) << methodName
        << "│ Shaka Samvat: " << std::left << std::setw(43) << panchanga.shakaYear << "│\n";

    std::string systemName = (calendarSystem == CalendarSystem::LUNAR_BASED ? "Lunar-based" :
                             calendarSystem == CalendarSystem::SOLAR_BASED ? "Solar-based" : "Luni-Solar");
    oss << "│ System: " << std::left << std::setw(50) << systemName
        << "│ Kali Yuga: " << std::left << std::setw(46) << panchanga.kaliyugaYear << "│\n";

    oss << "│ Julian Day: " << std::left << std::setw(47) << std::fixed << std::setprecision(1) << panchanga.julianDay
        << "│ Hindu Month: " << std::left << std::setw(44) << getHinduMonthName(panchanga.month) << "│\n";

    std::string ayanamsaVal = std::to_string(panchanga.ayanamsaValue).substr(0, 7) + "°";
    std::string pakshaStr = (panchanga.isShukla ? "Shukla (Bright)" : "Krishna (Dark)");
    oss << "│ Ayanamsa: " << std::left << std::setw(48) << ayanamsaVal
        << "│ Paksha: " << std::left << std::setw(49) << pakshaStr << "│\n";

    oss << "+------------------------------------------------------------+------------------------------------------------------------+\n\n";

    // Two-column layout for Panchanga elements and Sun/Moon info
    oss << "+------------------------------------------------------------+------------------------------------------------------------+\n";
    oss << "|                     🌟 PANCHANGAM (FIVE)                    |                    ☀️ SUN & MOON INFO                    |\n";
    oss << "+------------------------------------------------------------+------------------------------------------------------------+\n";

    std::string tithiStr = "1. Tithi: " + getTithiName(panchanga.tithi);
    if (tithiStr.length() > 58) tithiStr = tithiStr.substr(0, 58);
    std::string sunriseStr = "Sunrise: " + panchanga.getTimeString(panchanga.sunriseTime);
    oss << "| " << std::left << std::setw(59) << tithiStr
        << "| " << std::left << std::setw(59) << sunriseStr << "|\n";

    std::string tithiEndStr = "   (ends: " + panchanga.getTimeString(panchanga.tithiEndTime) + ")";
    std::string sunsetStr = "Sunset: " + panchanga.getTimeString(panchanga.sunsetTime);
    oss << "| " << std::left << std::setw(59) << tithiEndStr
        << "| " << std::left << std::setw(59) << sunsetStr << "|\n";

    std::string varaStr = "2. Vara: " + getVaraName(panchanga.vara);
    std::string moonriseStr = "Moonrise: " + panchanga.getTimeString(panchanga.moonriseTime);
    oss << "| " << std::left << std::setw(59) << varaStr
        << "| " << std::left << std::setw(59) << moonriseStr << "|\n";

    std::string nakStr = "3. Nakshatra: " + getNakshatraName(panchanga.nakshatra);
    if (nakStr.length() > 58) nakStr = nakStr.substr(0, 58);
    std::string moonsetStr = "Moonset: " + panchanga.getTimeString(panchanga.moonsetTime);
    oss << "| " << std::left << std::setw(59) << nakStr
        << "| " << std::left << std::setw(59) << moonsetStr << "|\n";

    std::string nakEndStr = "   (Pada " + std::to_string(panchanga.nakshatraPada) + ", ends: " +
                           panchanga.getTimeString(panchanga.nakshatraEndTime) + ")";
    if (nakEndStr.length() > 58) nakEndStr = nakEndStr.substr(0, 58);
    std::string dayLenStr = "Day Length: " + std::to_string(static_cast<int>(panchanga.dayLength)) + "." +
                           std::to_string(static_cast<int>((panchanga.dayLength - static_cast<int>(panchanga.dayLength)) * 10)) + " hours";
    oss << "| " << std::left << std::setw(59) << nakEndStr
        << "| " << std::left << std::setw(59) << dayLenStr << "|\n";

    std::string yogaStr = "4. Yoga: " + getYogaName(panchanga.yoga);
    if (yogaStr.length() > 58) yogaStr = yogaStr.substr(0, 58);
    std::string nightLenStr = "Night Length: " + std::to_string(static_cast<int>(panchanga.nightLength)) + "." +
                             std::to_string(static_cast<int>((panchanga.nightLength - static_cast<int>(panchanga.nightLength)) * 10)) + " hours";
    oss << "| " << std::left << std::setw(59) << yogaStr
        << "| " << std::left << std::setw(59) << nightLenStr << "|\n";

    std::string yogaEndStr = "   (ends: " + panchanga.getTimeString(panchanga.yogaEndTime) + ")";
    std::string emptyStr = "";
    oss << "| " << std::left << std::setw(59) << yogaEndStr
        << "| " << std::left << std::setw(59) << emptyStr << "|\n";

    std::string karanaStr = "5. Karana: " + getKaranaName(panchanga.karana);
    if (karanaStr.length() > 58) karanaStr = karanaStr.substr(0, 58);
    oss << "| " << std::left << std::setw(59) << karanaStr
        << "| " << std::left << std::setw(59) << emptyStr << "|\n";

    std::string karanaEndStr = "   (ends: " + panchanga.getTimeString(panchanga.karanaEndTime) + ")";
    oss << "| " << std::left << std::setw(59) << karanaEndStr
        << "| " << std::left << std::setw(59) << emptyStr << "|\n";

    oss << "+------------------------------------------------------------+------------------------------------------------------------+\n\n";

    // Celestial positions section
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

    oss << "🎯 VARNA (SAVARNA) CLASSIFICATION:\n";
    oss << "   Day Varna (Weekday): " << panchanga.varnaDay << "\n";
    oss << "   Tithi Varna (Lunar Day): " << panchanga.varnaTithi << "\n";
    oss << "   Nakshatra Varna (Star): " << panchanga.varnaNakshatra << "\n";
    oss << "   Julian Day: " << std::fixed << std::setprecision(1) << panchanga.julianDay << "\n\n";

    if (panchanga.isEkadashi || panchanga.isPurnima || panchanga.isAmavasya ||
        panchanga.isSankranti || panchanga.isNavratri || panchanga.isGandaMool ||
        panchanga.isPanchak || panchanga.isBhadra) {
        oss << "🌙 SPECIAL OBSERVANCES:\n";
        if (panchanga.isEkadashi) oss << "   • Ekadashi - Fasting day\n";
        if (panchanga.isPurnima) oss << "   • Purnima - Full Moon\n";
        if (panchanga.isAmavasya) oss << "   • Amavasya - New Moon\n";
        if (panchanga.isSankranti) {
            // Enhanced Sankranti display with full date-time information
            if (panchanga.sankrantiTime > 0) {
                int year, month, day;
                double gTime;

                // Validate Julian Day before conversion
                if (panchanga.julianDay > 0 && panchanga.julianDay < 5000000) {
                    swe_revjul(panchanga.julianDay, SE_GREG_CAL, &year, &month, &day, &gTime);

                    // Validate the converted date makes sense
                    if (year > 0 && year < 3000 && month >= 1 && month <= 12 && day >= 1 && day <= 31) {
                        char dateStr[32];
                        snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d", year, month, day);

                        oss << "   • Sankranti - Solar transition (at " << std::string(dateStr) << " "
                            << panchanga.getTimeString(panchanga.sankrantiTime) << ")\n";
                    } else {
                        oss << "   • Sankranti - Solar transition (time: "
                            << panchanga.getTimeString(panchanga.sankrantiTime) << ")\n";
                    }
                } else {
                    oss << "   • Sankranti - Solar transition (time: "
                        << panchanga.getTimeString(panchanga.sankrantiTime) << ")\n";
                }
            } else {
                oss << "   • Sankranti - Solar transition\n";
            }
        }
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

            // Julian Day criteria
            if (criteria.exactJulianDay > 0.0) {
                totalCriteria++;
                bool match = (abs(jd - criteria.exactJulianDay) <= criteria.julianDayTolerance);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "JD=" + std::to_string(static_cast<long>(jd));
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            } else if (criteria.julianDayRangeStart > 0.0 && criteria.julianDayRangeEnd > 0.0) {
                totalCriteria++;
                bool match = (jd >= criteria.julianDayRangeStart && jd <= criteria.julianDayRangeEnd);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "JD=" + std::to_string(static_cast<long>(criteria.julianDayRangeStart)) + "-" + std::to_string(static_cast<long>(criteria.julianDayRangeEnd));
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Varna (Savarna) criteria
            if (!criteria.exactVarnaDay.empty()) {
                totalCriteria++;
                bool match = (panchanga.varnaDay == criteria.exactVarnaDay);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "VarnaDay=" + panchanga.varnaDay;
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (!criteria.exactVarnaTithi.empty()) {
                totalCriteria++;
                bool match = (panchanga.varnaTithi == criteria.exactVarnaTithi);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "VarnaTithi=" + panchanga.varnaTithi;
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (!criteria.exactVarnaNakshatra.empty()) {
                totalCriteria++;
                bool match = (panchanga.varnaNakshatra == criteria.exactVarnaNakshatra);
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "VarnaNakshatra=" + panchanga.varnaNakshatra;
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            // Boolean Varna searches
            if (criteria.searchBrahminDays) {
                totalCriteria++;
                bool match = (panchanga.varnaDay == "Brahmin");
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "BrahminDay";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchKshatriyaDays) {
                totalCriteria++;
                bool match = (panchanga.varnaDay == "Kshatriya");
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "KshatriyaDay";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchVaishyaDays) {
                totalCriteria++;
                bool match = (panchanga.varnaDay == "Vaishya");
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "VaishyaDay";
                }
                isMatch = (criteria.logicMode == LogicMode::AND) ? (isMatch && match) : (isMatch || match);
            }

            if (criteria.searchShudradays) {
                totalCriteria++;
                bool match = (panchanga.varnaDay == "Shudra");
                if (match) {
                    matchCount++;
                    if (!matchDescription.empty()) matchDescription += ", ";
                    matchDescription += "ShudraDay";
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

// Julian Day search methods
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByJulianDay(double julianDay, const std::string& startDate, const std::string& endDate, double latitude, double longitude, double tolerance) const {
    SearchCriteria criteria;
    criteria.exactJulianDay = julianDay;
    criteria.julianDayTolerance = tolerance;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByJulianDayRange(double jdStart, double jdEnd, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.julianDayRangeStart = jdStart;
    criteria.julianDayRangeEnd = jdEnd;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Varna (Savarna) search methods
std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByVarnaDay(const std::string& varnaType, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactVarnaDay = varnaType;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByVarnaTithi(const std::string& varnaType, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactVarnaTithi = varnaType;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchByVarnaNakshatra(const std::string& varnaType, const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.exactVarnaNakshatra = varnaType;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchBrahminDays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchBrahminDays = true;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchKshatriyaDays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchKshatriyaDays = true;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchVaishyaDays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchVaishyaDays = true;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchShudradays(const std::string& startDate, const std::string& endDate, double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchShudradays = true;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

// Simplified JD search methods
HinduCalendar::SearchResult HinduCalendar::searchJulianDayOnly(double julianDay, double latitude, double longitude) const {
    SearchResult result;

    // Calculate Panchanga data for this JD
    result.panchangaData = calculatePanchanga(julianDay, latitude, longitude);
    result.julianDay = julianDay;

    // Convert JD to Gregorian date
    int year, month, day;
    julianDayToGregorianDate(julianDay, year, month, day);

    std::ostringstream dateStream;
    dateStream << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setw(2) << day;
    result.gregorianDate = dateStream.str();

    // Calculate weekday (0=Sunday, 6=Saturday)
    result.weekday = static_cast<int>(calculateVara(julianDay));

    result.matchScore = 1.0;  // Perfect match since we found the exact JD
    result.matchDescription = "Exact Julian Day match: " + std::to_string(julianDay);

    result.matchScore = 1.0;  // Perfect match since we found the exact JD
    result.matchDescription = "Exact Julian Day match: " + std::to_string(julianDay);

    return result;
}

PanchangaData HinduCalendar::calculatePanchangaFromJD(double julianDay, double latitude, double longitude) const {
    return calculatePanchanga(julianDay, latitude, longitude);
}

// Enhanced Sankranti calculation methods implementation
double HinduCalendar::calculateSankrantiTime(double julianDay, Rashi currentRashi, Rashi nextRashi) const {
    // Calculate exact time when Sun transits from currentRashi to nextRashi
    double targetLongitude = static_cast<int>(nextRashi) * 30.0;
    if (targetLongitude == 0.0) targetLongitude = 360.0; // Handle Meena to Mesha transition

    // Use iterative method to find exact transition time
    double searchStart = julianDay - 1.0; // Search 1 day before
    double searchEnd = julianDay + 1.0;   // Search 1 day after
    double precision = 0.0001; // Better precision (8.64 seconds)

    double bestTime = 12.0; // Default noon
    double bestDistance = 999.0;

    for (int iterations = 0; iterations < 100 && (searchEnd - searchStart) > precision; iterations++) {
        double midPoint = (searchStart + searchEnd) / 2.0;

        // Calculate sun position at midpoint
        double sunPos[6];
        char errorString[256];
        int result = swe_calc(midPoint, SE_SUN, SEFLG_SIDEREAL, sunPos, errorString);

        if (result < 0) {
            continue; // Skip invalid calculations
        }

        double sunLongitude = sunPos[0];

        // Normalize longitude to 0-360 range
        while (sunLongitude < 0) sunLongitude += 360.0;
        while (sunLongitude >= 360.0) sunLongitude -= 360.0;

        // Calculate distance to target
        double distanceToTarget = std::abs(sunLongitude - targetLongitude);
        if (distanceToTarget > 180.0) distanceToTarget = 360.0 - distanceToTarget;

        // Check if this is the best we've found
        if (distanceToTarget < bestDistance) {
            bestDistance = distanceToTarget;
            bestTime = (midPoint - floor(midPoint)) * 24.0;
        }

        // Check if we've found the transition time
        if (distanceToTarget < 0.01) { // Close enough (within 0.01 degrees)
            return bestTime;
        }

        // Determine which half to search next
        double rawDistance = targetLongitude - sunLongitude;
        if (rawDistance > 180.0) rawDistance -= 360.0;
        if (rawDistance < -180.0) rawDistance += 360.0;

        if (rawDistance > 0) {
            searchStart = midPoint;
        } else {
            searchEnd = midPoint;
        }
    }

    // Return the best time found, ensuring it's valid
    if (bestTime < 0) bestTime = 0.0;
    if (bestTime >= 24.0) bestTime = 23.99;

    return bestTime;
    return (searchStart - floor(searchStart)) * 24.0;
}

std::string HinduCalendar::calculateNextSankranti(double julianDay, double& daysUntil) const {
    // Get current sun longitude
    double sunPos[6];
    char errorString[256];
    int result = swe_calc(julianDay, SE_SUN, SEFLG_SIDEREAL, sunPos, errorString);

    if (result < 0) {
        daysUntil = 30.0; // Default to ~30 days if calculation fails
        return "Unknown Sankranti";
    }

    double currentLongitude = sunPos[0];
    while (currentLongitude < 0) currentLongitude += 360.0;
    while (currentLongitude >= 360.0) currentLongitude -= 360.0;

    // Calculate which rashi we're currently in and the next one
    int currentRashiNum = static_cast<int>(currentLongitude / 30.0) + 1;
    int nextRashiNum = (currentRashiNum % 12) + 1;

    // Calculate degrees until next Sankranti
    double nextSankrantiLongitude = nextRashiNum * 30.0;
    if (nextSankrantiLongitude > 360.0) nextSankrantiLongitude = 0.0;

    double degreesToGo = nextSankrantiLongitude - currentLongitude;
    if (degreesToGo <= 0) degreesToGo += 360.0;

    // Sun moves approximately 1 degree per day
    daysUntil = degreesToGo;

    // Get next Sankranti name
    Rashi nextRashi = static_cast<Rashi>(nextRashiNum);
    std::string nextRashiName = getRashiName(nextRashi);
    size_t spacePos = nextRashiName.find(' ');
    if (spacePos != std::string::npos) {
        nextRashiName = nextRashiName.substr(0, spacePos);
    }

    return nextRashiName + " Sankranti";
}

bool HinduCalendar::isSankrantiOccurringToday(double julianDay, double tolerance) const {
    // Check if Sankranti occurs within the tolerance period (default 0.5 days = 12 hours)
    double startCheck = julianDay - tolerance;
    double endCheck = julianDay + tolerance;

    // Get initial sun position
    double initialSunPos[6];
    char errorString[256];
    int result = swe_calc(startCheck, SE_SUN, SEFLG_SIDEREAL, initialSunPos, errorString);
    if (result < 0) return false;

    double initialLongitude = initialSunPos[0];
    while (initialLongitude < 0) initialLongitude += 360.0;
    while (initialLongitude >= 360.0) initialLongitude -= 360.0;

    int initialRashi = static_cast<int>(initialLongitude / 30.0);

    // Check every 2 hours for Sankranti transition
    for (double checkJD = startCheck; checkJD <= endCheck; checkJD += 0.083333) { // 2 hours = 0.083333 days
        double sunPos[6];
        result = swe_calc(checkJD, SE_SUN, SEFLG_SIDEREAL, sunPos, errorString);

        if (result >= 0) {
            double sunLongitude = sunPos[0];
            while (sunLongitude < 0) sunLongitude += 360.0;
            while (sunLongitude >= 360.0) sunLongitude -= 360.0;

            int currentRashi = static_cast<int>(sunLongitude / 30.0);

            // Check if Sun has moved to a different Rashi (Sankranti has occurred)
            if (currentRashi != initialRashi) {
                return true;
            }

            // Also check if sun is very close to a 30-degree boundary
            double modulo = std::fmod(sunLongitude, 30.0);
            if (modulo < 1.0 || modulo > 29.0) {
                return true;
            }
        }
    }

    return false;
}

std::vector<std::pair<double, std::string>> HinduCalendar::getSankrantiTimesForMonth(int year, int month) const {
    std::vector<std::pair<double, std::string>> sankrantiTimes;

    // Calculate Julian Day for first and last day of month
    double startJD = gregorianDateToJulianDay(year, month, 1);
    int daysInMonth = (month == 2) ? (isLeapYear(year) ? 29 : 28) :
                     ((month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31);
    double endJD = gregorianDateToJulianDay(year, month, daysInMonth);

    // Check each day of the month for Sankranti
    for (double jd = startJD; jd <= endJD; jd += 1.0) {
        if (isSankrantiOccurringToday(jd, 0.5)) {
            // Calculate which Sankranti this is
            double sunPos[6];
            char errorString[256];
            int result = swe_calc(jd, SE_SUN, SEFLG_SIDEREAL, sunPos, errorString);

            if (result >= 0) {
                double sunLongitude = sunPos[0];
                while (sunLongitude < 0) sunLongitude += 360.0;
                while (sunLongitude >= 360.0) sunLongitude -= 360.0;

                int rashiNum = static_cast<int>(sunLongitude / 30.0) + 1;
                if (rashiNum > 12) rashiNum = 1;

                Rashi rashi = static_cast<Rashi>(rashiNum);
                std::string rashiName = getRashiName(rashi);
                size_t spacePos = rashiName.find(' ');
                if (spacePos != std::string::npos) {
                    rashiName = rashiName.substr(0, spacePos);
                }

                sankrantiTimes.push_back({jd, rashiName + " Sankranti"});
            }
        }
    }

    return sankrantiTimes;
}

std::vector<std::pair<double, std::string>> HinduCalendar::getSankrantiTimesForYear(int year) const {
    std::vector<std::pair<double, std::string>> sankrantiTimes;

    // Check each month of the year
    for (int month = 1; month <= 12; month++) {
        auto monthSankrantis = getSankrantiTimesForMonth(year, month);
        sankrantiTimes.insert(sankrantiTimes.end(), monthSankrantis.begin(), monthSankrantis.end());
    }

    return sankrantiTimes;
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchSankranti(const std::string& startDate,
                                                                       const std::string& endDate,
                                                                       double latitude, double longitude) const {
    SearchCriteria criteria;
    criteria.searchSankranti = true;
    criteria.searchStartDate = startDate;
    criteria.searchEndDate = endDate;

    return searchHinduCalendar(criteria, latitude, longitude);
}

std::vector<HinduCalendar::SearchResult> HinduCalendar::searchSpecificSankranti(Rashi rashi,
                                                                               const std::string& startDate,
                                                                               const std::string& endDate,
                                                                               double latitude, double longitude) const {
    std::vector<SearchResult> allSankrantis = searchSankranti(startDate, endDate, latitude, longitude);
    std::vector<SearchResult> specificSankrantis;

    // Filter for the specific Rashi Sankranti
    std::string targetRashiName = getRashiName(rashi);
    size_t spacePos = targetRashiName.find(' ');
    if (spacePos != std::string::npos) {
        targetRashiName = targetRashiName.substr(0, spacePos);
    }

    for (const auto& result : allSankrantis) {
        if (result.panchangaData.sankrantiName.find(targetRashiName) != std::string::npos) {
            specificSankrantis.push_back(result);
        }
    }

    return specificSankrantis;
}

// Advanced Astronomical Calculation Methods Implementation

HinduCalendar::CoordinateSet HinduCalendar::calculateAllCoordinates(int body, double julianDay,
                                                                   double latitude, double longitude,
                                                                   double elevation) const {
    CoordinateSet coords = {};
    double xx[6];
    char errorString[AS_MAXCH];

    try {
        // Set topocentric position
        swe_set_topo(longitude, latitude, elevation);

        // Calculate astrometric coordinates (ICRF/J2000)
        int result = swe_calc(julianDay, body, SEFLG_SWIEPH | SEFLG_J2000, xx, errorString);
        if (result == ERR) {
            std::cerr << "Error calculating astrometric coordinates: " << errorString << std::endl;
            return coords;
        }

        coords.astrometric.rightAscension = xx[0];
        coords.astrometric.declination = xx[1];
        coords.astrometric.distance = xx[2];

        // Calculate apparent coordinates (true equinox of date)
        result = swe_calc(julianDay, body, SEFLG_SWIEPH | SEFLG_EQUATORIAL, xx, errorString);
        if (result == ERR) {
            std::cerr << "Error calculating apparent coordinates: " << errorString << std::endl;
            return coords;
        }

        coords.apparent.rightAscension = xx[0];
        coords.apparent.declination = xx[1];

        // Convert to ecliptic coordinates
        result = swe_calc(julianDay, body, SEFLG_SWIEPH, xx, errorString);
        if (result == ERR) {
            std::cerr << "Error calculating ecliptic coordinates: " << errorString << std::endl;
            return coords;
        }

        coords.apparent.eclipticLongitude = xx[0];
        coords.apparent.eclipticLatitude = xx[1];

        // Calculate topocentric coordinates
        result = swe_calc(julianDay, body, SEFLG_SWIEPH | SEFLG_TOPOCTR | SEFLG_EQUATORIAL, xx, errorString);
        if (result == ERR) {
            std::cerr << "Error calculating topocentric coordinates: " << errorString << std::endl;
            return coords;
        }

        coords.topocentric.rightAscension = xx[0];
        coords.topocentric.declination = xx[1];

        // Calculate azimuth and elevation
        double azalt[3];
        double geopos[3] = {longitude, latitude, elevation};
        swe_azalt(julianDay, SE_EQU2HOR, geopos, 1013.25, 15.0, xx, azalt);

        coords.topocentric.azimuth = azalt[0];
        coords.topocentric.elevation = azalt[1];

        // Calculate hour angle
        double siderealTime = swe_sidtime(julianDay);
        coords.topocentric.hourAngle = (siderealTime * 15.0 + longitude) - coords.topocentric.rightAscension;

        // Normalize hour angle to [-180, 180]
        while (coords.topocentric.hourAngle > 180.0) coords.topocentric.hourAngle -= 360.0;
        while (coords.topocentric.hourAngle < -180.0) coords.topocentric.hourAngle += 360.0;

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateAllCoordinates: " << e.what() << std::endl;
    }

    return coords;
}

HinduCalendar::AtmosphericModel HinduCalendar::getSeasonalAtmosphere(double julianDay,
                                                                    double latitude,
                                                                    double longitude) const {
    AtmosphericModel model = {};

    try {
        // Extract month for seasonal corrections
        int year, month, day, hour, minute;
        double second;
        swe_jdet_to_utc(julianDay, SE_GREG_CAL, &year, &month, &day, &hour, &minute, &second);

        // Apply seasonal corrections based on location
        getSeasonalAtmosphericParams(month, latitude, longitude,
                                   &model.pressure, &model.temperature);

        // Set other parameters based on climate zone
        if (abs(latitude) < 23.5) {
            // Tropical zone
            model.humidity = 0.7;        // Higher humidity
            model.wavelength = 0.55;     // Yellow light
            model.lapseRate = 0.0065;    // Standard atmosphere
        } else if (abs(latitude) < 66.5) {
            // Temperate zone
            model.humidity = 0.6;        // Moderate humidity
            model.wavelength = 0.55;     // Yellow light
            model.lapseRate = 0.0065;    // Standard atmosphere
        } else {
            // Polar zone
            model.humidity = 0.5;        // Lower humidity
            model.wavelength = 0.55;     // Yellow light
            model.lapseRate = 0.0080;    // Modified lapse rate
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in getSeasonalAtmosphere: " << e.what() << std::endl;

        // Default values
        model.pressure = 1013.25;
        model.temperature = 15.0;
        model.humidity = 0.6;
        model.wavelength = 0.55;
        model.lapseRate = 0.0065;
    }

    return model;
}

HinduCalendar::PolarConditions HinduCalendar::detectPolarConditions(double latitude,
                                                                   double declination,
                                                                   double julianDay) const {
    PolarConditions conditions = {};

    try {
        // Calculate limiting latitude for given declination
        double limitingLatitude = 90.0 - abs(declination);

        if (abs(latitude) > limitingLatitude) {
            if ((latitude > 0 && declination > 0) || (latitude < 0 && declination < 0)) {
                conditions.isPolarDay = true;
                conditions.description = "Continuous daylight - object never sets";
            } else {
                conditions.isPolarNight = true;
                conditions.description = "Continuous night - object never rises";
            }

            // Calculate approximate duration
            conditions.continuousDays = calculatePolarDuration(latitude, declination, julianDay);
        } else if (abs(latitude) > 60.0) {
            // Extended twilight zone
            conditions.isExtendedTwilight = true;
            conditions.description = "Extended twilight period";
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in detectPolarConditions: " << e.what() << std::endl;
    }

    return conditions;
}

std::vector<HinduCalendar::RiseSetEvent> HinduCalendar::calculateAllEvents(double jdStart,
                                                                          double latitude,
                                                                          double longitude,
                                                                          double timezone,
                                                                          double elevation) const {
    std::vector<RiseSetEvent> events;

    try {
        // Calculate for Sun
        addSolarEvents(events, jdStart, latitude, longitude, timezone, elevation);

        // Calculate for Moon
        addLunarEvents(events, jdStart, latitude, longitude, timezone, elevation);

        // Sort chronologically
        std::sort(events.begin(), events.end(),
                 [](const RiseSetEvent& a, const RiseSetEvent& b) {
                     return a.julianDay < b.julianDay;
                 });

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateAllEvents: " << e.what() << std::endl;
    }

    return events;
}

HinduCalendar::HorizonData HinduCalendar::calculateHorizon(double observerHeight,
                                                          double temperature,
                                                          double pressure) const {
    HorizonData horizon = {};

    try {
        // Geometric horizon (no refraction)
        horizon.geometricHorizon = calculateGeometricHorizon(observerHeight);

        // Apparent horizon (with refraction)
        double refractionCorrection = calculateRefractionCorrection(temperature, pressure);
        horizon.apparentHorizon = horizon.geometricHorizon - refractionCorrection;

        // Nautical horizon (center of object at geometric horizon)
        horizon.nauticalHorizon = horizon.geometricHorizon;

        // Astronomical horizon (upper limb at apparent horizon)
        horizon.astronomicalHorizon = horizon.apparentHorizon - 0.2667;  // Solar semidiameter

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateHorizon: " << e.what() << std::endl;
    }

    return horizon;
}

double HinduCalendar::calculateCustomHorizon(double observerElevation, double targetElevation) const {
    try {
        // Geometric horizon depression
        double geometricHorizon = calculateGeometricHorizon(observerElevation);

        // Standard refraction correction
        double refractionCorrection = 0.5667;  // 34 arcminutes

        // Solar/lunar semidiameter
        double semidiameter = 0.2667;  // 16 arcminutes

        return geometricHorizon - refractionCorrection - semidiameter + targetElevation;

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateCustomHorizon: " << e.what() << std::endl;
        return -0.8333;  // Standard horizon
    }
}

double HinduCalendar::calculateRefraction(double elevation, const AtmosphericModel& atm) const {
    try {
        if (elevation < -2.0) return 0.0;  // Below useful horizon

        // Barometric and temperature corrections
        double pressureCorrection = atm.pressure / 1013.25;
        double temperatureCorrection = 283.0 / (273.0 + atm.temperature);

        // Basic refraction formula (Bennett 1982)
        double elevationRad = elevation * M_PI / 180.0;
        double refraction = 1.0 / tan(elevationRad + 7.31 * M_PI / 180.0 / (elevation + 4.4));

        // Apply atmospheric corrections
        refraction *= pressureCorrection * temperatureCorrection;

        // Humidity correction for lower elevations
        if (elevation < 15.0) {
            double humidityCorrection = 1.0 + 0.0013 * atm.humidity * (1.0 - elevation / 15.0);
            refraction *= humidityCorrection;
        }

        return refraction / 60.0;  // Convert to degrees

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateRefraction: " << e.what() << std::endl;
        return 0.0;
    }
}

HinduCalendar::RiseSetEvent HinduCalendar::calculatePreciseRiseSet(int body, double jdStart,
                                                                  double latitude, double longitude,
                                                                  double elevation, double timezone) const {
    RiseSetEvent result = {};
    result.objectName = (body == SE_SUN) ? "Sun" : (body == SE_MOON) ? "Moon" : "Planet";
    result.eventType = "rise";
    result.isValid = false;

    try {
        // Setup atmospheric parameters
        AtmosphericModel atmosphere = getSeasonalAtmosphere(jdStart, latitude, longitude);

        // Calculate custom horizon
        double customHorizon = calculateCustomHorizon(elevation);

        // Setup Swiss Ephemeris parameters
        int sweFlags = SEFLG_SWIEPH | SEFLG_TOPOCTR | SEFLG_SPEED;
        swe_set_topo(longitude, latitude, elevation);

        // Perform rise/set calculation
        double riseSet[10];
        char errorString[AS_MAXCH];
        double geopos[3] = {longitude, latitude, elevation};

        int sweResult = swe_rise_trans(jdStart, body, nullptr, sweFlags,
                                     SE_CALC_RISE, geopos,
                                     atmosphere.pressure, atmosphere.temperature,
                                     riseSet, errorString);

        if (sweResult == ERR) {
            // Handle polar conditions
            double declination = getCurrentDeclination(body, jdStart);
            PolarConditions polar = detectPolarConditions(latitude, declination, jdStart);

            if (polar.isPolarDay || polar.isPolarNight) {
                result.notes = polar.description;
                return result;
            }

            result.notes = "Calculation error: " + std::string(errorString);
            return result;
        }

        // Calculate all coordinate systems
        result.coordinates = calculateAllCoordinates(body, riseSet[0], latitude, longitude, elevation);

        result.isValid = true;
        result.julianDay = riseSet[0];
        result.localTime = (riseSet[0] - floor(riseSet[0])) * 24.0 + timezone;
        result.azimuth = result.coordinates.topocentric.azimuth;
        result.elevation = result.coordinates.topocentric.elevation;

    } catch (const std::exception& e) {
        result.notes = "Exception: " + std::string(e.what());
    }

    return result;
}

double HinduCalendar::calculateCulminationTime(int body, double julianDay, double latitude) const {
    try {
        double culmination[10];
        char errorString[AS_MAXCH];
        double geopos[3] = {0.0, latitude, 0.0};

        int sweFlags = SEFLG_SWIEPH | SEFLG_TOPOCTR;

        int result = swe_rise_trans(julianDay, body, nullptr, sweFlags,
                                  SE_CALC_MTRANSIT, geopos,
                                  1013.25, 15.0, culmination, errorString);

        if (result == ERR) {
            std::cerr << "Error calculating culmination: " << errorString << std::endl;
            return -1.0;
        }

        return culmination[0];

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateCulminationTime: " << e.what() << std::endl;
        return -1.0;
    }
}

void HinduCalendar::handlePolarRiseSet(PanchangaData& panchanga, double latitude,
                                      const PolarConditions& polar) const {
    try {
        if (polar.isPolarDay) {
            // Sun/Moon continuously above horizon
            panchanga.sunriseTime = 0.0;        // Midnight
            panchanga.sunsetTime = 24.0;        // Next midnight
            panchanga.dayLength = 24.0;
            panchanga.nightLength = 0.0;
            panchanga.specialEvents.push_back("Polar Day - Continuous Sunlight");
        }
        else if (polar.isPolarNight) {
            // Sun/Moon continuously below horizon
            panchanga.sunriseTime = -1.0;       // No sunrise
            panchanga.sunsetTime = -1.0;        // No sunset
            panchanga.dayLength = 0.0;
            panchanga.nightLength = 24.0;
            panchanga.specialEvents.push_back("Polar Night - Continuous Darkness");
        }
        else if (polar.isExtendedTwilight) {
            // Use extended search window for polar regions
            calculateWithExtendedSearch(panchanga, latitude, 72.0);  // 3-day search
            panchanga.specialEvents.push_back("Extended Twilight Period");
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in handlePolarRiseSet: " << e.what() << std::endl;
    }
}

// Helper method implementations
double HinduCalendar::calculateGeometricHorizon(double height) const {
    try {
        // Height in meters, result in degrees
        const double EARTH_RADIUS_KM = 6371.0;
        return -sqrt(2.0 * height / 1000.0 / EARTH_RADIUS_KM) * 180.0 / M_PI;
    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateGeometricHorizon: " << e.what() << std::endl;
        return 0.0;
    }
}

double HinduCalendar::calculateRefractionCorrection(double temperature, double pressure) const {
    try {
        // Standard atmospheric refraction at horizon
        double standardRefraction = 0.5667;  // 34.0 arcminutes

        // Temperature and pressure corrections
        double tempCorrection = 283.0 / (273.0 + temperature);
        double pressureCorrection = pressure / 1013.25;

        return standardRefraction * tempCorrection * pressureCorrection;
    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateRefractionCorrection: " << e.what() << std::endl;
        return 0.5667;  // Standard refraction
    }
}

double HinduCalendar::getCurrentDeclination(int body, double julianDay) const {
    try {
        double xx[6];
        char errorString[AS_MAXCH];

        int result = swe_calc(julianDay, body, SEFLG_SWIEPH | SEFLG_EQUATORIAL, xx, errorString);
        if (result == ERR) {
            std::cerr << "Error getting declination: " << errorString << std::endl;
            return 0.0;
        }

        return xx[1];  // Declination

    } catch (const std::exception& e) {
        std::cerr << "Exception in getCurrentDeclination: " << e.what() << std::endl;
        return 0.0;
    }
}

double HinduCalendar::calculatePolarDuration(double latitude, double declination, double julianDay) const {
    try {
        // Simplified calculation for polar duration
        double limitingLatitude = 90.0 - abs(declination);
        double excessLatitude = abs(latitude) - limitingLatitude;

        // Rough approximation: more excess latitude = longer duration
        return std::min(180.0, excessLatitude * 4.0);  // Maximum 6 months

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculatePolarDuration: " << e.what() << std::endl;
        return 1.0;
    }
}

void HinduCalendar::calculateWithExtendedSearch(PanchangaData& panchanga, double latitude, double searchHours) const {
    try {
        // Implementation for extended search window
        // This would use a longer search period for extreme latitudes
        // For now, use standard calculations with extended timeout
        calculateSunMoonTimes(panchanga, latitude, 0.0);  // Use existing implementation

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateWithExtendedSearch: " << e.what() << std::endl;
    }
}

void HinduCalendar::addSolarEvents(std::vector<RiseSetEvent>& events, double jdStart,
                                  double latitude, double longitude, double timezone, double elevation) const {
    try {
        // Search for sunrise
        auto sunrise = calculatePreciseRiseSet(SE_SUN, jdStart, latitude, longitude, elevation, timezone);
        sunrise.eventType = "sunrise";
        if (sunrise.isValid) events.push_back(sunrise);

        // Search for sunset - use different calculation
        double geopos[3] = {longitude, latitude, elevation};
        double tret[10];
        char serr[AS_MAXCH];
        int flags = SEFLG_SWIEPH;

        AtmosphericModel atmosphere = getSeasonalAtmosphere(jdStart, latitude, longitude);
        double customHorizon = calculateCustomHorizon(elevation);

        int result = swe_rise_trans(jdStart, SE_SUN, nullptr, flags, SE_CALC_SET,
                                   geopos, atmosphere.pressure, atmosphere.temperature,
                                   tret, serr);

        if (result != ERR) {
            RiseSetEvent sunset = {};
            sunset.objectName = "Sun";
            sunset.eventType = "sunset";
            sunset.julianDay = tret[0];
            sunset.localTime = (tret[0] - floor(tret[0])) * 24.0 + timezone;
            sunset.isValid = true;
            sunset.coordinates = calculateAllCoordinates(SE_SUN, tret[0], latitude, longitude, elevation);
            sunset.azimuth = sunset.coordinates.topocentric.azimuth;
            sunset.elevation = sunset.coordinates.topocentric.elevation;
            events.push_back(sunset);
        }

        // Search for culmination (always possible)
        double culminationJD = calculateCulminationTime(SE_SUN, jdStart, latitude);
        if (culminationJD > 0) {
            RiseSetEvent culmination = {};
            culmination.objectName = "Sun";
            culmination.eventType = "culmination";
            culmination.julianDay = culminationJD;
            culmination.localTime = (culminationJD - floor(culminationJD)) * 24.0 + timezone;
            culmination.isValid = true;
            culmination.coordinates = calculateAllCoordinates(SE_SUN, culminationJD, latitude, longitude, elevation);
            events.push_back(culmination);
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in addSolarEvents: " << e.what() << std::endl;
    }
}

void HinduCalendar::addLunarEvents(std::vector<RiseSetEvent>& events, double jdStart,
                                  double latitude, double longitude, double timezone, double elevation) const {
    try {
        // Search for moonrise
        auto moonrise = calculatePreciseRiseSet(SE_MOON, jdStart, latitude, longitude, elevation, timezone);
        moonrise.eventType = "moonrise";
        if (moonrise.isValid) events.push_back(moonrise);

        // Search for moonset
        double geopos[3] = {longitude, latitude, elevation};
        double tret[10];
        char serr[AS_MAXCH];
        int flags = SEFLG_SWIEPH;

        AtmosphericModel atmosphere = getSeasonalAtmosphere(jdStart, latitude, longitude);
        double customHorizon = calculateCustomHorizon(elevation);

        int result = swe_rise_trans(jdStart, SE_MOON, nullptr, flags, SE_CALC_SET,
                                   geopos, atmosphere.pressure, atmosphere.temperature,
                                   tret, serr);

        if (result != ERR) {
            RiseSetEvent moonset = {};
            moonset.objectName = "Moon";
            moonset.eventType = "moonset";
            moonset.julianDay = tret[0];
            moonset.localTime = (tret[0] - floor(tret[0])) * 24.0 + timezone;
            moonset.isValid = true;
            moonset.coordinates = calculateAllCoordinates(SE_MOON, tret[0], latitude, longitude, elevation);
            moonset.azimuth = moonset.coordinates.topocentric.azimuth;
            moonset.elevation = moonset.coordinates.topocentric.elevation;
            events.push_back(moonset);
        }

        // Search for culmination
        double culminationJD = calculateCulminationTime(SE_MOON, jdStart, latitude);
        if (culminationJD > 0) {
            RiseSetEvent culmination = {};
            culmination.objectName = "Moon";
            culmination.eventType = "culmination";
            culmination.julianDay = culminationJD;
            culmination.localTime = (culminationJD - floor(culminationJD)) * 24.0 + timezone;
            culmination.isValid = true;
            culmination.coordinates = calculateAllCoordinates(SE_MOON, culminationJD, latitude, longitude, elevation);
            events.push_back(culmination);
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in addLunarEvents: " << e.what() << std::endl;
    }
}

// Additional helper methods for advanced calculations
HinduCalendar::RiseSetEvent HinduCalendar::findRiseEvent(int body, double jdStart, double latitude,
                                                        double longitude, double timezone, double elevation) const {
    return calculatePreciseRiseSet(body, jdStart, latitude, longitude, elevation, timezone);
}

HinduCalendar::RiseSetEvent HinduCalendar::findSetEvent(int body, double jdStart, double latitude,
                                                       double longitude, double timezone, double elevation) const {
    RiseSetEvent event = {};
    event.objectName = (body == SE_SUN) ? "Sun" : (body == SE_MOON) ? "Moon" : "Planet";
    event.eventType = "set";
    event.isValid = false;

    try {
        AtmosphericModel atmosphere = getSeasonalAtmosphere(jdStart, latitude, longitude);
        double customHorizon = calculateCustomHorizon(elevation);
        double geopos[3] = {longitude, latitude, elevation};
        double tret[10];
        char serr[AS_MAXCH];
        int flags = SEFLG_SWIEPH;

        int result = swe_rise_trans(jdStart, body, nullptr, flags, SE_CALC_SET,
                                   geopos, atmosphere.pressure, atmosphere.temperature,
                                   tret, serr);

        if (result != ERR) {
            event.julianDay = tret[0];
            event.localTime = (tret[0] - floor(tret[0])) * 24.0 + timezone;
            event.coordinates = calculateAllCoordinates(body, tret[0], latitude, longitude, elevation);
            event.azimuth = event.coordinates.topocentric.azimuth;
            event.elevation = event.coordinates.topocentric.elevation;
            event.isValid = true;
        } else {
            event.notes = "Set calculation failed: " + std::string(serr);
        }

    } catch (const std::exception& e) {
        event.notes = "Exception in findSetEvent: " + std::string(e.what());
    }

    return event;
}

HinduCalendar::RiseSetEvent HinduCalendar::findCulminationEvent(int body, double jdStart, double latitude,
                                                               double longitude, double timezone) const {
    RiseSetEvent event = {};
    event.objectName = (body == SE_SUN) ? "Sun" : (body == SE_MOON) ? "Moon" : "Planet";
    event.eventType = "culmination";
    event.isValid = false;

    try {
        double culminationJD = calculateCulminationTime(body, jdStart, latitude);
        if (culminationJD > 0) {
            event.julianDay = culminationJD;
            event.localTime = (culminationJD - floor(culminationJD)) * 24.0 + timezone;
            event.coordinates = calculateAllCoordinates(body, culminationJD, latitude, longitude, 0.0);
            event.azimuth = event.coordinates.topocentric.azimuth;
            event.elevation = event.coordinates.topocentric.elevation;
            event.isValid = true;
        } else {
            event.notes = "Culmination calculation failed";
        }

    } catch (const std::exception& e) {
        event.notes = "Exception in findCulminationEvent: " + std::string(e.what());
    }

    return event;
}

double HinduCalendar::calculateLunarParallax(double latitude, double elevation) const {
    try {
        // Simplified lunar parallax calculation
        // For more accurate results, would need current lunar distance
        double earthRadiusKm = 6371.0;
        double meanLunarDistanceKm = 384400.0;

        // Geocentric to topocentric correction
        double horizParallax = asin(earthRadiusKm / meanLunarDistanceKm) * 180.0 / M_PI;

        // Elevation correction (small effect)
        double elevationCorrectionFactor = 1.0 + elevation / 1000.0 / earthRadiusKm;

        return horizParallax * elevationCorrectionFactor * cos(latitude * M_PI / 180.0);

    } catch (const std::exception& e) {
        std::cerr << "Exception in calculateLunarParallax: " << e.what() << std::endl;
        return 0.0;
    }
}

// Time system conversion methods
double HinduCalendar::getDeltaT(double julianDay) const {
    try {
        // Delta T = TT - UT1 (in seconds)
        // Polynomial approximation for different epochs
        double year = 2000.0 + (julianDay - 2451545.0) / 365.25;

        if (year >= 2005 && year < 2050) {
            // Modern era polynomial
            double t = year - 2000.0;
            return 62.92 + 0.32217 * t + 0.005589 * t * t;
        } else if (year >= 1900 && year < 2005) {
            // 20th century approximation
            double t = year - 1900.0;
            return -2.79 + 1.494119 * t - 0.0598939 * t * t + 0.0061966 * t * t * t - 0.000197 * t * t * t * t;
        } else {
            // Long-term extrapolation
            double t = (year - 1820.0) / 100.0;
            return -20.0 + 32.0 * t * t;
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in getDeltaT: " << e.what() << std::endl;
        return 70.0;  // Approximate current value
    }
}

int HinduCalendar::getLeapSeconds(double jdUtc) const {
    try {
        // Simplified leap second table (major entries)
        if (jdUtc >= 2457754.5) return 37;  // 2017-01-01
        if (jdUtc >= 2457204.5) return 36;  // 2015-07-01
        if (jdUtc >= 2456109.5) return 35;  // 2012-07-01
        if (jdUtc >= 2454832.5) return 34;  // 2009-01-01
        if (jdUtc >= 2453736.5) return 33;  // 2006-01-01
        if (jdUtc >= 2441317.5) return 32;  // 1999-01-01 baseline
        return 0;  // Before 1972

    } catch (const std::exception& e) {
        std::cerr << "Exception in getLeapSeconds: " << e.what() << std::endl;
        return 37;  // Current value
    }
}

double HinduCalendar::utcToTdb(double jdUtc) const {
    try {
        // Convert UTC to TDB for precise ephemeris calculations
        double deltaT = getDeltaT(jdUtc);
        double ttJd = jdUtc + deltaT / 86400.0;

        // TDB - TT correction (periodic terms, usually small)
        double tdbMinusTt = 0.001658 * sin(628.3076 * (ttJd - 2451545.0) / 365.25) / 86400.0;

        return ttJd + tdbMinusTt;

    } catch (const std::exception& e) {
        std::cerr << "Exception in utcToTdb: " << e.what() << std::endl;
        return jdUtc + 70.0 / 86400.0;  // Approximate correction
    }
}

// Helper function to determine if a Gregorian year is a leap year
bool isLeapYear(int year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    if (year % 4 == 0) return true;
    return false;
}

} // namespace Astro
