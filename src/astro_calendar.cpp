#include "astro_calendar.h"
#include "hindu_calendar.h"
#include "myanmar_calendar.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "swephexp.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <memory>
#include <cstdint>

namespace Astro {

// Month names array
const std::array<std::string, 12> AstroCalendar::monthNames = {{
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
}};

// PlanetaryTransition methods
std::string PlanetaryTransition::getDescription() const {
    std::stringstream ss;
    switch (type) {
        case TransitionType::SIGN_CHANGE:
            ss << getPlanetName(planet) << " enters " << toSign;
            break;
        case TransitionType::RETROGRADE_START:
            ss << getPlanetName(planet) << " goes retrograde in " << toSign;
            break;
        case TransitionType::RETROGRADE_END:
            ss << getPlanetName(planet) << " goes direct in " << toSign;
            break;
        case TransitionType::CONJUNCTION:
            ss << getPlanetName(planet) << " conjunct " << getPlanetName(secondPlanet);
            break;
        case TransitionType::ECLIPSE:
            ss << description;
            break;
        case TransitionType::NEW_MOON:
            ss << "New Moon in " << toSign;
            break;
        case TransitionType::FULL_MOON:
            ss << "Full Moon in " << toSign;
            break;
        case TransitionType::NAKSHATRA_CHANGE:
            ss << getPlanetName(planet) << " enters nakshatra";
            break;
        case TransitionType::KP_SUB_LORD_CHANGE:
            ss << getPlanetName(planet) << " KP sub-lord changes";
            break;
        case TransitionType::ASPECT_FORMATION:
            ss << getPlanetName(planet) << " forms aspect with " << getPlanetName(secondPlanet);
            break;
        case TransitionType::TRANSIT_INGRESS:
            ss << getPlanetName(planet) << " transits into " << toSign;
            break;
        case TransitionType::STATIONARY_DIRECT:
            ss << getPlanetName(planet) << " stationary direct";
            break;
        case TransitionType::STATIONARY_RETRO:
            ss << getPlanetName(planet) << " stationary retrograde";
            break;
        case TransitionType::RETROGRADE:
            ss << getPlanetName(planet) << " retrograde motion";
            break;
        case TransitionType::DIRECT:
            ss << getPlanetName(planet) << " direct motion";
            break;
    }
    return ss.str();
}

std::string PlanetaryTransition::getFormattedDate() const {
    int year, month, day;
    AstroCalendar::julianDayToGregorian(julianDay, year, month, day);
    return AstroCalendar::formatDate(year, month, day);
}

// KPStarLordTransition methods
std::string KPStarLordTransition::getDescription() const {
    std::stringstream ss;
    ss << getPlanetName(planet) << " Level-" << level << ": "
       << fromStarName << " → " << toStarName;
    return ss.str();
}

std::string KPStarLordTransition::getDetailedAnalysis() const {
    std::stringstream ss;
    ss << "🔮 KP Analysis - " << getPlanetName(planet) << "\n";
    ss << "   Level " << level << " Star Lord Change\n";
    ss << "   From: " << fromStarName << " (Star " << fromStar << ")\n";
    ss << "   To: " << toStarName << " (Star " << toStar << ")\n";
    ss << "   Duration: " << std::fixed << std::setprecision(2) << duration << " days\n";
    ss << "   Impact: " << (isAuspicious ? "🌟 Favorable" : "⚠️ Challenging") << "\n";
    ss << "   Significance: " << significance;
    return ss.str();
}

// AstroCalendarDay methods
std::string AstroCalendarDay::getShortSummary() const {
    std::stringstream ss;
    if (hasPanchangaData) {
        ss << "T:" << static_cast<int>(panchangaData.tithi) << " ";
        ss << "N:" << static_cast<int>(panchangaData.nakshatra) << " ";
    }
    if (hasMyanmarData) {
        ss << "ME:" << myanmarData.myanmarYear << " ";
    }
    if (isAuspicious) ss << "✅";
    else if (isInauspicious) ss << "⚠️";
    return ss.str();
}

std::string AstroCalendarDay::getDetailedDescription() const {
    std::stringstream ss;
    ss << "═══ " << gregorianDateStr << " ═══\n";

    if (hasPanchangaData) {
        ss << "🕉️ Hindu: Tithi " << static_cast<int>(panchangaData.tithi);
        ss << ", Nakshatra " << static_cast<int>(panchangaData.nakshatra) << "\n";
    }

    if (hasMyanmarData) {
        ss << "🇲🇲 Myanmar: " << myanmarData.myanmarYear << " ME, ";
        ss << "Day " << myanmarData.dayOfMonth << "\n";
    }

    if (!allFestivals.empty()) {
        ss << "🎊 Festivals: " << getFestivalsSummary() << "\n";
    }

    if (!planetaryTransitions.empty()) {
        ss << "🪐 Transits: " << getPlanetaryTransitionsSummary() << "\n";
    }

    ss << "Quality: " << dayQualityDescription;
    return ss.str();
}

std::string AstroCalendarDay::getFestivalsSummary() const {
    std::stringstream ss;
    for (size_t i = 0; i < allFestivals.size() && i < 3; ++i) {
        if (i > 0) ss << ", ";
        ss << allFestivals[i];
    }
    if (allFestivals.size() > 3) {
        ss << " +" << (allFestivals.size() - 3) << " more";
    }
    return ss.str();
}

std::string AstroCalendarDay::getPlanetaryTransitionsSummary() const {
    std::stringstream ss;
    for (size_t i = 0; i < planetaryTransitions.size() && i < 2; ++i) {
        if (i > 0) ss << ", ";
        ss << planetaryTransitions[i].getDescription();
    }
    if (planetaryTransitions.size() > 2) {
        ss << " +" << (planetaryTransitions.size() - 2) << " more";
    }
    return ss.str();
}

// AstroCalendarMonth methods
std::string AstroCalendarMonth::getMonthSummary() const {
    std::stringstream ss;
    ss << monthName << " " << year << " - ";
    ss << auspiciousDays << " auspicious, ";
    ss << inauspiciousDays << " inauspicious, ";
    ss << neutralDays << " neutral days";
    return ss.str();
}

// AstroCalendar implementation
AstroCalendar::AstroCalendar() {
    hinduCalendar = std::make_unique<HinduCalendar>();
    myanmarCalendar = std::make_unique<MyanmarCalendar>();
    ephemerisManager = std::make_unique<EphemerisManager>();
    kpSystem = std::make_unique<KPSystem>();
    // PlanetCalculator will be initialized after ephemerisManager is ready
}

AstroCalendar::~AstroCalendar() = default;

bool AstroCalendar::initialize(double lat, double lon) {
    latitude = lat;
    longitude = lon;

    try {
        // Initialize all calendar systems
        if (!hinduCalendar->initialize()) {
            lastError = "Failed to initialize Hindu Calendar: " + hinduCalendar->getLastError();
            return false;
        }

        if (!myanmarCalendar->initialize()) {
            lastError = "Failed to initialize Myanmar Calendar: " + myanmarCalendar->getLastError();
            return false;
        }

        if (!ephemerisManager->initialize()) {
            lastError = "Failed to initialize Ephemeris Manager: " + ephemerisManager->getLastError();
            return false;
        }

        // Now initialize PlanetCalculator with the ephemeris manager
        planetCalculator = std::make_unique<PlanetCalculator>(*ephemerisManager);

        initialized = true;
        return true;

    } catch (const std::exception& e) {
        lastError = "Exception during initialization: " + std::string(e.what());
        return false;
    }
}

AstroCalendarDay AstroCalendar::calculateAstroCalendarDay(const BirthData& birthData) const {
    return calculateAstroCalendarDay(birthData.year, birthData.month, birthData.day);
}

AstroCalendarDay AstroCalendar::calculateAstroCalendarDay(int year, int month, int day) const {
    if (!initialized) {
        throw std::runtime_error("AstroCalendar not initialized");
    }

    double jd = gregorianToJulianDay(year, month, day);
    return calculateDayData(jd, year, month, day);
}

AstroCalendarMonth AstroCalendar::calculateAstroCalendarMonth(int year, int month) const {
    if (!initialized) {
        throw std::runtime_error("AstroCalendar not initialized");
    }

    AstroCalendarMonth monthData;
    monthData.year = year;
    monthData.month = month;
    monthData.monthName = getGregorianMonthName(month);

    int daysInMonth = getDaysInMonth(year, month);
    monthData.days.reserve(daysInMonth);

    // Calculate data for each day
    for (int day = 1; day <= daysInMonth; ++day) {
        try {
            AstroCalendarDay dayData = calculateAstroCalendarDay(year, month, day);
            monthData.days.push_back(dayData);

            // Update statistics
            if (dayData.isAuspicious) {
                monthData.auspiciousDays++;
            } else if (dayData.isInauspicious) {
                monthData.inauspiciousDays++;
            } else {
                monthData.neutralDays++;
            }

        } catch (const std::exception& e) {
            lastError = "Error calculating day " + std::to_string(day) + ": " + e.what();
        }
    }

    // Calculate monthly transitions if requested
    if (includePlanetaryTransitions) {
        monthData.monthlyTransitions = findMonthlyTransitions(year, month);
    }

    return monthData;
}

AstroCalendarDay AstroCalendar::calculateDayData(double julianDay, int gregYear, int gregMonth, int gregDay) const {
    AstroCalendarDay day;

    // Basic Gregorian data
    day.gregorianYear = gregYear;
    day.gregorianMonth = gregMonth;
    day.gregorianDay = gregDay;
    day.gregorianDateStr = formatDate(gregYear, gregMonth, gregDay);
    day.julianDay = julianDay;

    try {
        // Calculate Hindu Calendar (Panchanga) data
        day.panchangaData = hinduCalendar->calculatePanchanga(julianDay, latitude, longitude);
        day.hasPanchangaData = true;

    } catch (const std::exception& e) {
        lastError = "Hindu calendar calculation failed: " + std::string(e.what());
        day.hasPanchangaData = false;
    }

    try {
        // Calculate Myanmar Calendar data
        day.myanmarData = myanmarCalendar->calculateMyanmarCalendar(julianDay);
        day.hasMyanmarData = true;

    } catch (const std::exception& e) {
        lastError = "Myanmar calendar calculation failed: " + std::string(e.what());
        day.hasMyanmarData = false;
    }

    // Calculate planetary transitions if requested
    if (includePlanetaryTransitions) {
        day.planetaryTransitions = calculatePlanetaryTransitions(julianDay);
    }

    // Calculate KP transitions if requested
    if (includeKPTransitions) {
        try {
            day.kpTransitions = calculateKPTransitions(julianDay);
            day.hasKPData = true;

            // Calculate planetary positions for KP analysis
            if (planetCalculator) {
                BirthData tempData;
                tempData.year = gregYear;
                tempData.month = gregMonth;
                tempData.day = gregDay;
                tempData.hour = 12;
                tempData.minute = 0;
                tempData.second = 0;
                tempData.latitude = latitude;
                tempData.longitude = longitude;

                planetCalculator->calculateAllPlanets(tempData, day.planetPositions);

                // Calculate KP data for each planet
                for (const auto& pos : day.planetPositions) {
                    KPPosition kpData = kpSystem->calculateKPPosition(pos.longitude);
                    day.kpPlanetData[pos.planet] = kpData;

                    // Store sign and nakshatra info
                    day.planetSigns[pos.planet] = zodiacSignToString(static_cast<ZodiacSign>(static_cast<int>(pos.longitude / 30.0)));
                    day.planetNakshatras[pos.planet] = static_cast<int>((pos.longitude / 13.333333) + 1);
                }
            }

        } catch (const std::exception& e) {
            lastError = "KP calculation failed: " + std::string(e.what());
            day.hasKPData = false;
        }
    }

    // Compile festivals and events
    compileFestivalsAndEvents(day);

    // Calculate overall day quality
    calculateDayQuality(day);

    return day;
}

std::vector<PlanetaryTransition> AstroCalendar::calculatePlanetaryTransitions(double julianDay) const {
    std::vector<PlanetaryTransition> transitions;

    try {
        // Check for lunar phases
        double moonPhase[6];
        // Calculate moon phase using ephemeris
        if (swe_calc_ut(julianDay, SE_MOON, SEFLG_SWIEPH, moonPhase, nullptr) >= 0) {
            double sunPos[6];
            if (swe_calc_ut(julianDay, SE_SUN, SEFLG_SWIEPH, sunPos, nullptr) >= 0) {
                double phaseDiff = std::fmod(moonPhase[0] - sunPos[0] + 360.0, 360.0);

                if (std::abs(phaseDiff) < 1.0) { // New Moon
                    PlanetaryTransition newMoon;
                    newMoon.type = TransitionType::NEW_MOON;
                    newMoon.planet = Planet::MOON;
                    newMoon.julianDay = julianDay;
                    newMoon.description = "New Moon - New beginnings";
                    newMoon.isAuspicious = true;
                    newMoon.recommendations = "Good time for setting intentions and starting new projects";
                    transitions.push_back(newMoon);
                }

                if (std::abs(phaseDiff - 180.0) < 1.0) { // Full Moon
                    PlanetaryTransition fullMoon;
                    fullMoon.type = TransitionType::FULL_MOON;
                    fullMoon.planet = Planet::MOON;
                    fullMoon.julianDay = julianDay;
                    fullMoon.description = "Full Moon - Peak energy";
                    fullMoon.isAuspicious = true;
                    fullMoon.recommendations = "Time for completion, manifestation, and releasing";
                    transitions.push_back(fullMoon);
                }
            }
        }

    } catch (const std::exception& e) {
        lastError = "Error calculating planetary transitions: " + std::string(e.what());
    }

    return transitions;
}

std::vector<PlanetaryTransition> AstroCalendar::findMonthlyTransitions(int year, int month) const {
    std::vector<PlanetaryTransition> transitions;

    // This is a placeholder - would need more sophisticated ephemeris calculations
    // to find actual planetary sign changes, retrogrades, etc.

    return transitions;
}

std::vector<KPStarLordTransition> AstroCalendar::calculateKPTransitions(double julianDay) const {
    std::vector<KPStarLordTransition> transitions;

    if (!kpSystem) {
        return transitions;
    }

    try {
        // Calculate for major planets
        std::vector<Planet> planets = {
            Planet::SUN, Planet::MOON, Planet::MARS, Planet::MERCURY,
            Planet::JUPITER, Planet::VENUS, Planet::SATURN
        };

        for (const Planet& planet : planets) {
            double planetPos[6];
            int planetId = static_cast<int>(planet);

            if (swe_calc_ut(julianDay, planetId, SEFLG_SWIEPH, planetPos, nullptr) >= 0) {
                double longitude = planetPos[0];

                // Calculate KP data for each level requested
                for (int level = 1; level <= kpLevels; level++) {
                    KPPosition currentKP = kpSystem->calculateKPPosition(longitude);
                    KPPosition nextDayKP;

                    // Check next day to see if there's a transition
                    if (swe_calc_ut(julianDay + 1.0, planetId, SEFLG_SWIEPH, planetPos, nullptr) >= 0) {
                        nextDayKP = kpSystem->calculateKPPosition(planetPos[0]);

                        // Check for level transitions
                        bool hasTransition = false;
                        int fromStar = 0, toStar = 0;

                        switch (level) {
                            case 1:
                                if (currentKP.nakshatra.lord != nextDayKP.nakshatra.lord) {
                                    fromStar = static_cast<int>(currentKP.nakshatra.lord);
                                    toStar = static_cast<int>(nextDayKP.nakshatra.lord);
                                    hasTransition = true;
                                }
                                break;
                            case 2:
                                if (currentKP.subLord != nextDayKP.subLord) {
                                    fromStar = static_cast<int>(currentKP.subLord);
                                    toStar = static_cast<int>(nextDayKP.subLord);
                                    hasTransition = true;
                                }
                                break;
                            case 3:
                                if (currentKP.subSubLord != nextDayKP.subSubLord) {
                                    fromStar = static_cast<int>(currentKP.subSubLord);
                                    toStar = static_cast<int>(nextDayKP.subSubLord);
                                    hasTransition = true;
                                }
                                break;
                            case 4:
                                if (currentKP.subSubSubLord != nextDayKP.subSubSubLord) {
                                    fromStar = static_cast<int>(currentKP.subSubSubLord);
                                    toStar = static_cast<int>(nextDayKP.subSubSubLord);
                                    hasTransition = true;
                                }
                                break;
                        }

                        if (hasTransition) {
                            KPStarLordTransition transition;
                            transition.planet = planet;
                            transition.level = level;
                            transition.fromStar = fromStar;
                            transition.toStar = toStar;
                            transition.fromStarName = "Star " + std::to_string(static_cast<int>(fromStar));
                            transition.toStarName = "Star " + std::to_string(static_cast<int>(toStar));
                            transition.julianDay = julianDay;
                            transition.duration = 24.0; // Default 24 hours
                            transition.isAuspicious = true; // Default
                            transition.significance = 0.5; // Default significance

                            transitions.push_back(transition);
                        }
                    }
                }
            }
        }

    } catch (const std::exception& e) {
        lastError = "Error calculating KP transitions: " + std::string(e.what());
    }

    return transitions;
}

void AstroCalendar::compileFestivalsAndEvents(AstroCalendarDay& day) const {
    day.allFestivals.clear();
    day.allEvents.clear();

    // Add Hindu festivals
    if (day.hasPanchangaData) {
        for (const auto& festival : day.panchangaData.festivals) {
            day.allFestivals.push_back("🕉️ " + festival);
        }
        for (const auto& event : day.panchangaData.specialEvents) {
            day.allEvents.push_back("🕉️ " + event);
        }
    }

    // Add Myanmar festivals
    if (day.hasMyanmarData) {
        for (const auto& festival : day.myanmarData.festivals) {
            day.allFestivals.push_back("🇲🇲 " + festival);
        }
    }
}

void AstroCalendar::calculateDayQuality(AstroCalendarDay& day) const {
    int score = 5; // Neutral starting point

    // Hindu calendar factors
    if (day.hasPanchangaData) {
        if (day.panchangaData.isShubhaMuhurta) {
            score += 2;
            day.astrologicalRecommendations.push_back("Auspicious Hindu muhurta");
        }
        if (day.panchangaData.isAshubhaMuhurta) {
            score -= 2;
            day.astrologicalRecommendations.push_back("Inauspicious Hindu muhurta");
        }
        if (day.panchangaData.isEkadashi) {
            score += 1;
            day.astrologicalRecommendations.push_back("Ekadashi - spiritual practices favored");
        }
    }

    // Myanmar calendar factors
    if (day.hasMyanmarData) {
        if (day.myanmarData.isThamanyo) {
            score += 2;
            day.astrologicalRecommendations.push_back("Thamanyo - auspicious day");
        }
        if (day.myanmarData.isYatyaza) {
            score -= 2;
            day.astrologicalRecommendations.push_back("Yatyaza - avoid new ventures");
        }
        if (day.myanmarData.isSabbath) {
            score += 1;
            day.astrologicalRecommendations.push_back("Buddhist Sabbath - meditation favored");
        }
    }

    // Planetary transition factors
    for (const auto& transition : day.planetaryTransitions) {
        if (transition.isAuspicious) {
            score += 1;
        } else {
            score -= 1;
        }
    }

    // Clamp score to 0-10 range
    day.auspiciousScore = std::max(0, std::min(10, score));

    if (day.auspiciousScore >= 7) {
        day.isAuspicious = true;
        day.dayQualityDescription = "Highly Auspicious Day";
    } else if (day.auspiciousScore <= 3) {
        day.isInauspicious = true;
        day.dayQualityDescription = "Inauspicious Day - Exercise Caution";
    } else {
        day.dayQualityDescription = "Neutral Day";
    }
}

// Output formatting methods
std::string AstroCalendar::generateDayCalendar(const AstroCalendarDay& day, const std::string& format) const {
    if (format == "json") {
        return generateJSON(day);
    }

    std::stringstream ss;
    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "               🌟 ASTRO CALENDAR - " << day.gregorianDateStr << " 🌟\n";
    ss << "═══════════════════════════════════════════════════════════════════\n\n";

    // Quality indicator
    ss << "📊 DAY QUALITY: " << day.dayQualityDescription << " (Score: " << day.auspiciousScore << "/10)\n";
    if (day.isAuspicious) ss << "   Status: ✅ AUSPICIOUS DAY\n";
    else if (day.isInauspicious) ss << "   Status: ⚠️ INAUSPICIOUS DAY\n";
    else ss << "   Status: ⚪ NEUTRAL DAY\n";
    ss << "\n";

    // Hindu Calendar section
    if (day.hasPanchangaData) {
        ss << "🕉️ HINDU CALENDAR (PANCHANGA):\n";
        ss << "   Tithi: " << static_cast<int>(day.panchangaData.tithi) << " ";
        ss << (day.panchangaData.isShukla ? "(Shukla)" : "(Krishna)") << "\n";
        ss << "   Vara: " << static_cast<int>(day.panchangaData.vara) << "\n";
        ss << "   Nakshatra: " << static_cast<int>(day.panchangaData.nakshatra) << "\n";
        ss << "   Yoga: " << static_cast<int>(day.panchangaData.yoga) << "\n";
        ss << "   Karana: " << static_cast<int>(day.panchangaData.karana) << "\n";
        ss << "\n";
    }

    // Myanmar Calendar section
    if (day.hasMyanmarData) {
        ss << "🇲🇲 MYANMAR CALENDAR:\n";
        ss << "   Myanmar Year: " << day.myanmarData.myanmarYear << " ME\n";
        ss << "   Sasana Year: " << day.myanmarData.sasanaYear << " SE\n";
        ss << "   Month: " << static_cast<int>(day.myanmarData.month) << "\n";
        ss << "   Day: " << day.myanmarData.dayOfMonth << "\n";
        ss << "   Moon Phase: " << static_cast<int>(day.myanmarData.moonPhase) << "\n";
        ss << "\n";
    }

    // Festivals and Events
    if (!day.allFestivals.empty()) {
        ss << "🎊 FESTIVALS:\n";
        for (const auto& festival : day.allFestivals) {
            ss << "   • " << festival << "\n";
        }
        ss << "\n";
    }

    // Planetary Transitions
    if (!day.planetaryTransitions.empty()) {
        ss << "🪐 PLANETARY EVENTS:\n";
        for (const auto& transition : day.planetaryTransitions) {
            ss << "   • " << transition.getDescription() << "\n";
            if (!transition.recommendations.empty()) {
                ss << "     " << transition.recommendations << "\n";
            }
        }
        ss << "\n";
    }

    // Recommendations
    if (!day.astrologicalRecommendations.empty()) {
        ss << "💡 RECOMMENDATIONS:\n";
        for (const auto& rec : day.astrologicalRecommendations) {
            ss << "   • " << rec << "\n";
        }
        ss << "\n";
    }

    ss << "═══════════════════════════════════════════════════════════════════\n";

    return ss.str();
}

std::string AstroCalendar::generateMonthlyCalendar(const AstroCalendarMonth& monthData, const std::string& format) const {
    if (format == "json") {
        return generateJSON(monthData);
    } else if (format == "csv") {
        return generateCSV(monthData);
    } else if (format == "table") {
        return generateTable(monthData);
    } else if (format == "professional") {
        return generateProfessionalAstroCalendar(monthData);
    } else {
        return generateCalendarLayout(monthData);
    }
}

std::string AstroCalendar::generateJSON(const AstroCalendarDay& day) const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"gregorianDate\": \"" << day.gregorianDateStr << "\",\n";
    ss << "  \"julianDay\": " << std::fixed << std::setprecision(6) << day.julianDay << ",\n";
    ss << "  \"dayQuality\": {\n";
    ss << "    \"score\": " << day.auspiciousScore << ",\n";
    ss << "    \"description\": \"" << day.dayQualityDescription << "\",\n";
    ss << "    \"isAuspicious\": " << (day.isAuspicious ? "true" : "false") << ",\n";
    ss << "    \"isInauspicious\": " << (day.isInauspicious ? "true" : "false") << "\n";
    ss << "  },\n";

    if (day.hasPanchangaData) {
        ss << "  \"hinduCalendar\": {\n";
        ss << "    \"tithi\": " << static_cast<int>(day.panchangaData.tithi) << ",\n";
        ss << "    \"vara\": " << static_cast<int>(day.panchangaData.vara) << ",\n";
        ss << "    \"nakshatra\": " << static_cast<int>(day.panchangaData.nakshatra) << ",\n";
        ss << "    \"yoga\": " << static_cast<int>(day.panchangaData.yoga) << ",\n";
        ss << "    \"karana\": " << static_cast<int>(day.panchangaData.karana) << ",\n";
        ss << "    \"isShukla\": " << (day.panchangaData.isShukla ? "true" : "false") << "\n";
        ss << "  },\n";
    }

    if (day.hasMyanmarData) {
        ss << "  \"myanmarCalendar\": {\n";
        ss << "    \"myanmarYear\": " << day.myanmarData.myanmarYear << ",\n";
        ss << "    \"sasanaYear\": " << day.myanmarData.sasanaYear << ",\n";
        ss << "    \"month\": " << static_cast<int>(day.myanmarData.month) << ",\n";
        ss << "    \"day\": " << day.myanmarData.dayOfMonth << ",\n";
        ss << "    \"moonPhase\": " << static_cast<int>(day.myanmarData.moonPhase) << "\n";
        ss << "  },\n";
    }

    // Festivals
    ss << "  \"festivals\": [";
    for (size_t i = 0; i < day.allFestivals.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "\"" << day.allFestivals[i] << "\"";
    }
    ss << "],\n";

    // Planetary transitions
    ss << "  \"planetaryTransitions\": [";
    for (size_t i = 0; i < day.planetaryTransitions.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "{\"description\":\"" << day.planetaryTransitions[i].getDescription() << "\"}";
    }
    ss << "]\n";

    ss << "}";
    return ss.str();
}

std::string AstroCalendar::generateJSON(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"year\": " << monthData.year << ",\n";
    ss << "  \"month\": " << monthData.month << ",\n";
    ss << "  \"monthName\": \"" << monthData.monthName << "\",\n";
    ss << "  \"summary\": \"" << monthData.getMonthSummary() << "\",\n";
    ss << "  \"statistics\": {\n";
    ss << "    \"auspiciousDays\": " << monthData.auspiciousDays << ",\n";
    ss << "    \"inauspiciousDays\": " << monthData.inauspiciousDays << ",\n";
    ss << "    \"neutralDays\": " << monthData.neutralDays << "\n";
    ss << "  },\n";
    ss << "  \"days\": [\n";

    for (size_t i = 0; i < monthData.days.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "    " << generateJSON(monthData.days[i]);
    }

    ss << "\n  ]\n";
    ss << "}";
    return ss.str();
}

// Helper methods for enhanced UI/UX
std::string AstroCalendar::getWeekdayName(const AstroCalendarDay& day) const {
    static const std::vector<std::string> weekdays = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    // Calculate weekday from Julian Day (JD 0 = Monday)
    int jd = static_cast<int>(day.julianDay);
    int weekday = (jd + 1) % 7; // Adjust for 0=Sunday indexing
    return weekdays[weekday];
}

std::string AstroCalendar::getTithiDescription(int tithi) const {
    static const std::vector<std::string> tithiNames = {
        "Amavasya (New Moon)", "Pratipad", "Dwitiya", "Tritiya", "Chaturthi",
        "Panchami", "Shashthi", "Saptami", "Ashtami", "Navami", "Dashami",
        "Ekadashi", "Dwadashi", "Trayodashi", "Chaturdashi", "Purnima (Full Moon)"
    };

    if (tithi >= 0 && tithi < tithiNames.size()) {
        return tithiNames[tithi];
    }
    return "Unknown Tithi";
}

std::string AstroCalendar::generateCalendarLayout(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "           🌟 ASTRO CALENDAR - " << monthData.monthName << " " << monthData.year << " 🌟\n";
    ss << "═══════════════════════════════════════════════════════════════════\n";
    ss << "           🕉️ Hindu  🇲🇲 Myanmar  🪐 Planetary  🎊 Festivals\n";
    ss << "═══════════════════════════════════════════════════════════════════\n\n";

    ss << monthData.getMonthSummary() << "\n\n";

    // Calendar header
    ss << " Sun    Mon    Tue    Wed    Thu    Fri    Sat\n";
    ss << "─────  ─────  ─────  ─────  ─────  ─────  ─────\n";

    // Find first day of month and day of week
    double firstDayJD = gregorianToJulianDay(monthData.year, monthData.month, 1);
    int firstDayOfWeek = (static_cast<int>(firstDayJD + 1.5)) % 7; // 0=Sunday

    // Print leading spaces for first week
    for (int i = 0; i < firstDayOfWeek; ++i) {
        ss << "       ";
    }

    // Print calendar days
    for (size_t dayIdx = 0; dayIdx < monthData.days.size(); ++dayIdx) {
        const auto& day = monthData.days[dayIdx];
        int dayOfWeek = (firstDayOfWeek + dayIdx) % 7;

        // Day number
        ss << std::setw(2) << day.gregorianDay;

        // Quality indicator
        if (day.isAuspicious) ss << "✅";
        else if (day.isInauspicious) ss << "⚠️";
        else ss << "  ";

        // Festival indicator
        if (!day.allFestivals.empty()) ss << "🎊";
        else ss << " ";

        // Planetary event indicator
        if (!day.planetaryTransitions.empty()) ss << "🪐";
        else ss << " ";

        // End of week - new line
        if (dayOfWeek == 6) {
            ss << "\n";
        } else {
            ss << " ";
        }
    }

    ss << "\n\n";

    // Legend
    ss << "Legend:\n";
    ss << "✅ Auspicious day    ⚠️ Inauspicious day    🎊 Festival    🪐 Planetary event\n\n";

    // Significant days summary
    if (!monthData.significantDays.empty()) {
        ss << "Significant Days:\n";
        for (const auto& [day, description] : monthData.significantDays) {
            ss << "  " << std::setw(2) << day << ": " << description << "\n";
        }
        ss << "\n";
    }

    ss << "═══════════════════════════════════════════════════════════════════\n";

    return ss.str();
}

// Utility methods
std::string AstroCalendar::getGregorianMonthName(int month) const {
    if (month >= 1 && month <= 12) {
        return monthNames[month - 1];
    }
    return "Unknown";
}

int AstroCalendar::getDaysInMonth(int year, int month) const {
    static const int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year)) {
        return 29;
    }

    if (month >= 1 && month <= 12) {
        return daysPerMonth[month - 1];
    }

    return 30; // default
}

bool AstroCalendar::isLeapYear(int year) const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

double AstroCalendar::gregorianToJulianDay(int year, int month, int day) {
    return swe_julday(year, month, day, 12.0, SE_GREG_CAL);
}

void AstroCalendar::julianDayToGregorian(double jd, int& year, int& month, int& day) {
    int32 y, m, d, hour, min;
    double sec;
    swe_jdet_to_utc(jd, SE_GREG_CAL, &y, &m, &d, &hour, &min, &sec);
    year = y;
    month = m;
    day = d;
}

std::string AstroCalendar::formatDate(int year, int month, int day) {
    std::stringstream ss;
    ss << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setw(2) << day;
    return ss.str();
}

std::string AstroCalendar::generateCSV(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    // CSV header
    ss << "Date,Day_Quality_Score,Is_Auspicious,Hindu_Tithi,Hindu_Nakshatra,Myanmar_Year,Myanmar_Day,";
    ss << "Festivals,Planetary_Events,Recommendations\n";

    for (const auto& day : monthData.days) {
        ss << day.gregorianDateStr << ",";
        ss << day.auspiciousScore << ",";
        ss << (day.isAuspicious ? "Yes" : "No") << ",";

        if (day.hasPanchangaData) {
            ss << static_cast<int>(day.panchangaData.tithi) << ",";
            ss << static_cast<int>(day.panchangaData.nakshatra) << ",";
        } else {
            ss << ",,";
        }

        if (day.hasMyanmarData) {
            ss << day.myanmarData.myanmarYear << ",";
            ss << day.myanmarData.dayOfMonth << ",";
        } else {
            ss << ",,";
        }

        // Festivals (quoted and comma-separated)
        ss << "\"";
        for (size_t i = 0; i < day.allFestivals.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << day.allFestivals[i];
        }
        ss << "\",";

        // Planetary events
        ss << "\"";
        for (size_t i = 0; i < day.planetaryTransitions.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << day.planetaryTransitions[i].getDescription();
        }
        ss << "\",";

        // Recommendations
        ss << "\"";
        for (size_t i = 0; i < day.astrologicalRecommendations.size(); ++i) {
            if (i > 0) ss << "; ";
            ss << day.astrologicalRecommendations[i];
        }
        ss << "\"\n";
    }

    return ss.str();
}

std::string AstroCalendar::generateTable(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    // Enhanced header with emojis and better formatting
    ss << "╔═══════════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                       🌟 ASTRO CALENDAR 🌟                                           ║\n";
    ss << "║              " << monthData.monthName << " " << monthData.year << " - Comprehensive Astrological Overview              ║\n";
    ss << "╠═══════════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║ 📅 Gregorian | 🔮 Quality | 🕉️ Hindu | 🇲🇲 Myanmar | 🎉 Festivals | 🪐 Planetary    ║\n";
    ss << "╠═══════════════════════════════════════════════════════════════════════════════════════╣\n";

    for (const auto& day : monthData.days) {
        // Enhanced date formatting with weekday
        std::string dateStr = formatEnhancedDate(day);
        ss << "║ " << std::left << std::setw(12) << dateStr;

        // Enhanced quality indicator with color-like symbols
        std::string quality = getEnhancedQualityIndicator(day);
        ss << " │ " << std::setw(8) << quality;

        // Enhanced Hindu calendar info
        std::string hindu = getEnhancedHinduInfo(day);
        ss << " │ " << std::setw(8) << hindu;

        // Enhanced Myanmar calendar info
        std::string myanmar = getEnhancedMyanmarInfo(day);
        ss << " │ " << std::setw(8) << myanmar;

        // Enhanced festivals display
        std::string festivals = getEnhancedFestivalsDisplay(day);
        ss << " │ " << std::setw(11) << festivals;

        // Enhanced planetary info
        std::string planetary = getEnhancedPlanetaryDisplay(day);
        ss << " │ " << std::setw(10) << planetary;

        ss << " ║\n";

        // Add detailed line for highly significant days
        if (day.auspiciousScore >= 8 || day.isInauspicious || !day.allFestivals.empty() || !day.planetaryTransitions.empty()) {
            ss << "║   " << getDetailedDayInfo(day) << std::string(80 - getDetailedDayInfo(day).length(), ' ') << " ║\n";
        }
    }

    // Enhanced footer with monthly summary
    ss << "╠═══════════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║                           📊 MONTHLY SUMMARY                                         ║\n";
    ss << "╠═══════════════════════════════════════════════════════════════════════════════════════╣\n";

    // Add monthly statistics and highlights
    ss << getEnhancedMonthlySummary(monthData);

    ss << "╠═══════════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║ 🔮 Legend: ⭐Excellent 🌟Good ✨Fair ⚪Neutral ⚠️Caution 🚫Avoid                    ║\n";
    ss << "║ 🕉️ Hindu: T=Tithi N=Nakshatra | 🇲🇲 Myanmar: ME=Myanmar Era SE=Sasana Era          ║\n";
    ss << "╚═══════════════════════════════════════════════════════════════════════════════════════╝\n";

    return ss.str();
}

std::string AstroCalendar::generateProfessionalAstroCalendar(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    // Professional header similar to Saint John Astronomy Club style
    ss << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                        🌟 PROFESSIONAL ASTRO CALENDAR 🌟                                   ║\n";
    ss << "║                     " << monthData.monthName << " " << monthData.year << " - Advanced Astronomical Analysis                        ║\n";
    ss << "╠══════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║                   🌙 Lunar Phases • 🪐 Planetary Transits • 🔮 KP Analysis                 ║\n";
    ss << "║           🕉️ Panchanga • 🇲🇲 Myanmar Calendar • ✨ Three Calendar Integration              ║\n";
    ss << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝\n\n";

    // Major planetary transits header for the month
    ss << "🪐 MAJOR PLANETARY TRANSITS - " << monthData.monthName << " " << monthData.year << "\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════════════════\n";

    // Collect major transits for the month
    std::map<std::string, std::vector<std::string>> transitsByPlanet;
    for (const auto& day : monthData.days) {
        for (const auto& transit : day.planetaryTransitions) {
            if (transit.strength >= 3) { // Only major transits
                std::string planetName = getPlanetName(transit.planet);
                std::string transitDesc = transit.getDescription() + " (" +
                    std::to_string(day.gregorianDay) + ")";
                transitsByPlanet[planetName].push_back(transitDesc);
            }
        }
    }

    // Display transits by planet
    for (const auto& [planet, transits] : transitsByPlanet) {
        ss << planet << ": ";
        for (size_t i = 0; i < transits.size(); ++i) {
            if (i > 0) ss << " • ";
            ss << transits[i];
        }
        ss << "\n";
    }
    ss << "\n";

    // KP Star Lord Changes (if enabled)
    if (includeKPTransitions) {
        ss << "🔮 KP STAR LORD TRANSITIONS\n";
        ss << "═══════════════════════════════════════════════════════════════════════════════════════════\n";

        for (int level = 1; level <= kpLevels; level++) {
            ss << "Level " << level << " (" << getKPLevelDescription(level) << "):\n";

            for (const auto& day : monthData.days) {
                for (const auto& kpTransit : day.kpTransitions) {
                    if (kpTransit.level == level) {
                        ss << "  " << std::setw(2) << day.gregorianDay << ": "
                           << kpTransit.getDescription() << "\n";
                    }
                }
            }
            ss << "\n";
        }
    }

    // Calendar grid with enhanced information
    ss << "📅 MONTHLY CALENDAR GRID\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════════════════\n";

    // Weekday headers
    ss << " Sun        Mon        Tue        Wed        Thu        Fri        Sat\n";
    ss << "──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────\n";

    // Find first day of month
    double firstDayJD = gregorianToJulianDay(monthData.year, monthData.month, 1);
    int firstDayOfWeek = (static_cast<int>(firstDayJD + 1.5)) % 7; // 0=Sunday

    // Print leading spaces
    for (int i = 0; i < firstDayOfWeek; ++i) {
        ss << "          │";
    }

    // Print each day with detailed info
    for (size_t dayIdx = 0; dayIdx < monthData.days.size(); ++dayIdx) {
        const auto& day = monthData.days[dayIdx];
        int dayOfWeek = (firstDayOfWeek + dayIdx) % 7;

        // Day cell with multiple lines of info
        ss << generateProfessionalDayCell(day);

        if (dayOfWeek == 6) {
            ss << "\n";
            // Add separator line between weeks
            if (dayIdx + 1 < monthData.days.size()) {
                ss << "──────────┼──────────┼──────────┼──────────┼──────────┼──────────┼──────────\n";
            }
        } else {
            ss << "│";
        }
    }

    ss << "\n\n";

    // Legend and symbols explanation
    ss << "🔍 SYMBOLS & ABBREVIATIONS\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════════════════\n";
    ss << "Quality: ⭐Excellent 🌟Good ✨Fair ⚪Neutral ⚠️Caution 🚫Avoid\n";
    ss << "Hindu: T=Tithi E=Ekadashi P=Purnima A=Amavasya N=Nakshatra\n";
    ss << "Myanmar: ME=Myanmar Era S=Sabbath P=Pyathada Y=Yatyaza\n";
    ss << "Planets: ☉Sun ☽Moon ♂Mars ☿Mercury ♃Jupiter ♀Venus ♄Saturn\n";
    ss << "Transits: I=Ingress R=Retrograde D=Direct C=Conjunction E=Eclipse\n";
    if (includeKPTransitions) {
        ss << "KP Levels: 1=Star 2=Sub 3=Sub-Sub 4=Sub-Sub-Sub\n";
    }
    ss << "\n";

    // Important notes
    ss << "📋 CALENDAR NOTES\n";
    ss << "═══════════════════════════════════════════════════════════════════════════════════════════\n";
    ss << "• All times in local coordinates (Lat: " << std::fixed << std::setprecision(2)
       << latitude << "° Lon: " << longitude << "°)\n";
    ss << "• Planetary positions calculated using Swiss Ephemeris\n";
    ss << "• Three calendar systems integrated: Gregorian, Hindu (Panchanga), Myanmar\n";
    if (includeKPTransitions) {
        ss << "• KP (Krishnamurti Paddhati) star lord transitions included\n";
    }
    ss << "• Quality ratings based on combined astrological factors\n";

    return ss.str();
}

// Enhanced formatting helper methods
std::string AstroCalendar::generateProfessionalDayCell(const AstroCalendarDay& day) const {
    std::stringstream ss;

    // Day number with quality indicator
    ss << std::setw(2) << day.gregorianDay;
    if (day.auspiciousScore >= 8) ss << "⭐";
    else if (day.auspiciousScore >= 6) ss << "🌟";
    else if (day.auspiciousScore >= 4) ss << "✨";
    else if (day.auspiciousScore >= 2) ss << "⚪";
    else ss << "⚠️";

    // Hindu calendar info (compact)
    if (day.hasPanchangaData) {
        ss << " T" << static_cast<int>(day.panchangaData.tithi);
        if (day.panchangaData.isEkadashi) ss << "E";
        if (day.panchangaData.isPurnima) ss << "P";
        if (day.panchangaData.isAmavasya) ss << "A";
    }

    // Myanmar calendar info (very compact)
    if (day.hasMyanmarData) {
        ss << " " << (day.myanmarData.myanmarYear % 100) << "ME";
        if (day.myanmarData.isSabbath) ss << "S";
    }

    // Planetary events (symbols only)
    for (const auto& transit : day.planetaryTransitions) {
        switch (transit.type) {
            case TransitionType::SIGN_CHANGE:
            case TransitionType::TRANSIT_INGRESS:
                ss << "I";
                break;
            case TransitionType::RETROGRADE_START:
            case TransitionType::STATIONARY_RETRO:
                ss << "R";
                break;
            case TransitionType::RETROGRADE_END:
            case TransitionType::STATIONARY_DIRECT:
                ss << "D";
                break;
            case TransitionType::CONJUNCTION:
                ss << "C";
                break;
            case TransitionType::ECLIPSE:
                ss << "E";
                break;
            case TransitionType::NEW_MOON:
                ss << "🌑";
                break;
            case TransitionType::FULL_MOON:
                ss << "🌕";
                break;
            default:
                break;
        }
    }

    // KP level indicators (if enabled)
    if (includeKPTransitions && !day.kpTransitions.empty()) {
        ss << " KP";
        for (const auto& kpTransit : day.kpTransitions) {
            ss << kpTransit.level;
        }
    }

    // Festival indicator
    if (!day.allFestivals.empty()) {
        ss << "🎉";
    }

    // Pad to cell width
    std::string cellContent = ss.str();
    if (cellContent.length() < 10) {
        cellContent.append(10 - cellContent.length(), ' ');
    }

    return cellContent.substr(0, 10); // Ensure exactly 10 characters
}

std::string AstroCalendar::getKPLevelDescription(int level) const {
    switch (level) {
        case 1: return "Star Lords";
        case 2: return "Sub Lords";
        case 3: return "Sub-Sub Lords";
        case 4: return "Sub-Sub-Sub Lords";
        default: return "Unknown Level";
    }
}

std::string AstroCalendar::formatEnhancedDate(const AstroCalendarDay& day) const {
    std::array<std::string, 7> weekdays = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    int weekday = static_cast<int>(day.julianDay + 1.5) % 7; // Calculate weekday from Julian Day

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << day.gregorianDay << " "
       << weekdays[weekday];
    return ss.str();
}

std::string AstroCalendar::getEnhancedQualityIndicator(const AstroCalendarDay& day) const {
    std::string indicator;

    if (day.auspiciousScore >= 9) indicator = "⭐";      // Excellent
    else if (day.auspiciousScore >= 7) indicator = "🌟"; // Very Good
    else if (day.auspiciousScore >= 5) indicator = "✨"; // Good
    else if (day.auspiciousScore >= 3) indicator = "⚪"; // Fair
    else if (day.isInauspicious) indicator = "🚫";       // Avoid
    else indicator = "⚠️";                               // Caution

    return indicator + std::to_string(day.auspiciousScore);
}

std::string AstroCalendar::getEnhancedHinduInfo(const AstroCalendarDay& day) const {
    if (!day.hasPanchangaData) return "-";

    std::stringstream ss;
    ss << "T" << static_cast<int>(day.panchangaData.tithi);
    if (day.panchangaData.isEkadashi) ss << "E";
    if (day.panchangaData.isPurnima) ss << "P";
    if (day.panchangaData.isAmavasya) ss << "A";

    return ss.str();
}

// Utility functions
std::string AstroCalendar::formatAstroCalendarDate(const AstroCalendarDay& day) const {
    return day.gregorianDateStr;
}

std::string getSeasonName(int month) {
    if (month >= 3 && month <= 5) return "Spring";
    if (month >= 6 && month <= 8) return "Summer";
    if (month >= 9 && month <= 11) return "Autumn";
    return "Winter";
}

std::string getMoonPhaseName(double lunarPhase) {
    if (lunarPhase < 1.0 || lunarPhase > 359.0) return "New Moon";
    if (lunarPhase >= 89.0 && lunarPhase <= 91.0) return "First Quarter";
    if (lunarPhase >= 179.0 && lunarPhase <= 181.0) return "Full Moon";
    if (lunarPhase >= 269.0 && lunarPhase <= 271.0) return "Last Quarter";
    if (lunarPhase < 90.0) return "Waxing Crescent";
    if (lunarPhase < 180.0) return "Waxing Gibbous";
    if (lunarPhase < 270.0) return "Waning Gibbous";
    return "Waning Crescent";
}

std::string AstroCalendar::getPlanetaryWeather(const std::vector<PlanetaryTransition>& transitions) const {
    int auspiciousCount = 0;
    int inauspiciousCount = 0;

    for (const auto& transition : transitions) {
        if (transition.isAuspicious) auspiciousCount++;
        else inauspiciousCount++;
    }

    if (auspiciousCount > inauspiciousCount) return "Favorable planetary weather";
    if (inauspiciousCount > auspiciousCount) return "Challenging planetary weather";
    return "Neutral planetary weather";
}

// Enhanced UI helper methods for AstroCalendar
std::string AstroCalendar::getEnhancedMyanmarInfo(const AstroCalendarDay& day) const {
    if (!day.hasMyanmarData) return "-";

    std::stringstream ss;
    ss << std::to_string(day.myanmarData.myanmarYear % 100) << "ME"; // Show last 2 digits
    if (day.myanmarData.isSabbath) ss << "S";
    if (day.myanmarData.isPyathada) ss << "P";

    return ss.str();
}

std::string AstroCalendar::getEnhancedFestivalsDisplay(const AstroCalendarDay& day) const {
    if (day.allFestivals.empty()) return "-";

    std::string festivals;
    int count = 0;
    for (const auto& festival : day.allFestivals) {
        if (count >= 2) {
            festivals += "+";
            break;
        }
        if (count > 0) festivals += ",";

        // Abbreviate common festival names
        std::string abbrev = abbreviateFestivalName(festival);
        festivals += abbrev;
        count++;
    }

    if (festivals.length() > 11) {
        festivals = festivals.substr(0, 8) + "...";
    }

    return festivals;
}

std::string AstroCalendar::getEnhancedPlanetaryDisplay(const AstroCalendarDay& day) const {
    if (day.planetaryTransitions.empty()) return "-";

    std::string planetary;
    int count = 0;
    for (const auto& transition : day.planetaryTransitions) {
        if (count >= 2) {
            planetary += "+";
            break;
        }
        if (count > 0) planetary += " ";

        planetary += getPlanetarySymbol(transition);
        count++;
    }

    return planetary;
}

std::string AstroCalendar::getDetailedDayInfo(const AstroCalendarDay& day) const {
    std::string details;

    // Add significant astrological information
    if (day.auspiciousScore >= 8) {
        details += "✨ Highly Auspicious ";
    }
    if (day.isInauspicious) {
        details += "⚠️ Exercise Caution ";
    }

    // Add important festivals
    if (!day.allFestivals.empty()) {
        details += "🎉 " + day.allFestivals[0] + " ";
    }

    // Add significant planetary events
    for (const auto& transition : day.planetaryTransitions) {
        if (transition.significance == "High") {
            details += "🪐 " + transition.getDescription() + " ";
            break;
        }
    }

    return details;
}

std::string AstroCalendar::getSeasonName(int month) const {
    switch (month) {
        case 12: case 1: case 2: return "Winter ❄️";
        case 3: case 4: case 5: return "Spring 🌸";
        case 6: case 7: case 8: return "Summer ☀️";
        case 9: case 10: case 11: return "Autumn 🍂";
        default: return "Unknown";
    }
}

std::string AstroCalendar::getEnhancedMonthlySummary(const AstroCalendarMonth& monthData) const {
    std::stringstream ss;

    // Count special days
    int excellentDays = 0, goodDays = 0, cautionDays = 0;
    int festivalDays = 0, planetaryEventDays = 0;

    for (const auto& day : monthData.days) {
        if (day.auspiciousScore >= 8) excellentDays++;
        else if (day.auspiciousScore >= 6) goodDays++;
        else if (day.isInauspicious) cautionDays++;

        if (!day.allFestivals.empty()) festivalDays++;
        if (!day.planetaryTransitions.empty()) planetaryEventDays++;
    }

    ss << "║ 📈 Quality Distribution: " << excellentDays << " Excellent, "
       << goodDays << " Good, " << cautionDays << " Caution days" << std::string(23, ' ') << "║\n";

    ss << "║ 🎉 Festival Days: " << festivalDays
       << " | 🪐 Planetary Events: " << planetaryEventDays << std::string(40, ' ') << "║\n";

    // Add season and lunar information
    std::string season = getSeasonName(monthData.month);
    ss << "║ 🌱 Season: " << season << " | 🌙 New/Full Moons: "
       << countMoonPhases(monthData) << std::string(35, ' ') << "║\n";

    return ss.str();
}

std::string AstroCalendar::abbreviateFestivalName(const std::string& festival) const {
    // Common abbreviations for festivals
    if (festival.find("Ekadashi") != std::string::npos) return "Eka";
    if (festival.find("Purnima") != std::string::npos) return "Pur";
    if (festival.find("Amavasya") != std::string::npos) return "Ama";
    if (festival.find("Sankranti") != std::string::npos) return "San";
    if (festival.find("Festival") != std::string::npos) {
        return festival.substr(0, 3);
    }

    // Return first 3-4 characters for other festivals
    return festival.substr(0, std::min(4, static_cast<int>(festival.length())));
}

std::string AstroCalendar::getPlanetarySymbol(const PlanetaryTransition& transition) const {
    std::string symbol;

    switch (transition.type) {
        case TransitionType::SIGN_CHANGE:
            symbol = "🔄";
            break;
        case TransitionType::RETROGRADE_START:
            symbol = "⏪";
            break;
        case TransitionType::RETROGRADE_END:
            symbol = "⏩";
            break;
        case TransitionType::CONJUNCTION:
            symbol = "🤝";
            break;
        case TransitionType::ECLIPSE:
            symbol = "🌑";
            break;
        case TransitionType::NEW_MOON:
            symbol = "🌑";
            break;
        case TransitionType::FULL_MOON:
            symbol = "🌕";
            break;
        default:
            symbol = "🪐";
    }

    return symbol;
}

int AstroCalendar::countMoonPhases(const AstroCalendarMonth& monthData) const {
    int moonPhases = 0;

    for (const auto& day : monthData.days) {
        for (const auto& transition : day.planetaryTransitions) {
            if (transition.type == TransitionType::NEW_MOON ||
                transition.type == TransitionType::FULL_MOON) {
                moonPhases++;
            }
        }
    }

    return moonPhases;
}

std::string AstroCalendar::formatCalendarCell(const AstroCalendarDay& day) const {
    std::stringstream ss;

    // Day number with quality indicator
    ss << std::setfill(' ') << std::setw(2) << day.gregorianDay;

    // Quality symbol
    std::string quality = getEnhancedQualityIndicator(day);
    ss << quality.substr(0, 1); // Just the symbol

    // Hindu indicator (single letter)
    if (day.hasPanchangaData) {
        if (day.panchangaData.isEkadashi) ss << "E";
        else if (day.panchangaData.isPurnima) ss << "P";
        else if (day.panchangaData.isAmavasya) ss << "A";
        else ss << "H";
    } else {
        ss << " ";
    }

    // Myanmar indicator
    if (day.hasMyanmarData && (day.myanmarData.isSabbath || day.myanmarData.isPyathada)) {
        ss << "M";
    } else {
        ss << " ";
    }

    // Festival indicator
    if (!day.allFestivals.empty()) {
        ss << "F";
    } else {
        ss << " ";
    }

    return ss.str();
}

int AstroCalendar::getFirstDayOfMonth(int year, int month) const {
    // Calculate first day of month (0=Sunday, 1=Monday, etc.)
    double jd = gregorianToJulianDay(year, month, 1);
    return static_cast<int>(jd + 1.5) % 7;
}

int AstroCalendar::countExcellentDays(const AstroCalendarMonth& monthData) const {
    int count = 0;
    for (const auto& day : monthData.days) {
        if (day.auspiciousScore >= 8) count++;
    }
    return count;
}

int AstroCalendar::countFestivalDays(const AstroCalendarMonth& monthData) const {
    int count = 0;
    for (const auto& day : monthData.days) {
        if (!day.allFestivals.empty()) count++;
    }
    return count;
}

int AstroCalendar::countPlanetaryDays(const AstroCalendarMonth& monthData) const {
    int count = 0;
    for (const auto& day : monthData.days) {
        if (!day.planetaryTransitions.empty()) count++;
    }
    return count;
}

int AstroCalendar::countCautionDays(const AstroCalendarMonth& monthData) const {
    int count = 0;
    for (const auto& day : monthData.days) {
        if (day.isInauspicious) count++;
    }
    return count;
}

// Enhanced daily view generation
std::string AstroCalendar::generateDetailedDayView(const AstroCalendarDay& day) const {
    std::stringstream ss;

    // Beautiful header for individual day
    ss << "╔═══════════════════════════════════════════════════════════════════════════════════╗\n";
    ss << "║                          🌟 DAILY ASTROLOGICAL OVERVIEW 🌟                       ║\n";
    ss << "║                    " << day.gregorianDateStr << " - " << getWeekdayName(day) << "                    ║\n";
    ss << "╠═══════════════════════════════════════════════════════════════════════════════════╣\n";

    // Day Quality Section
    ss << "║ 🔮 DAY QUALITY                                                                    ║\n";
    ss << "╠═══════════════════════════════════════════════════════════════════════════════════╣\n";
    ss << "║ Overall Rating: " << getEnhancedQualityIndicator(day) << " (" << day.auspiciousScore << "/10)";
    ss << std::string(45, ' ') << "║\n";
    ss << "║ Status: " << (day.isAuspicious ? "✅ Auspicious Day" :
                          (day.isInauspicious ? "⚠️ Exercise Caution" : "⚪ Neutral Day"));
    ss << std::string(35, ' ') << "║\n";

    ss << "╚═══════════════════════════════════════════════════════════════════════════════════╝\n";

    return ss.str();
}

std::string AstroCalendar::getKPTransitionDisplay(const std::vector<KPStarLordTransition>& kpTransitions) const {
    if (kpTransitions.empty()) return "";

    std::stringstream ss;
    ss << "\n┌─ KP Star Lord Transitions:\n";

    for (const auto& transition : kpTransitions) {
        ss << "├─ " << getPlanetName(transition.planet)
           << " (" << getKPLevelDescription(transition.level) << "): "
           << transition.getDescription() << "\n";

        if (!transition.getDetailedAnalysis().empty()) {
            ss << "│  📊 " << transition.getDetailedAnalysis() << "\n";
        }
    }

    ss << "└─────────────────────────────────────────\n";
    return ss.str();
}

std::string AstroCalendar::formatKPTransitionsTable(const std::vector<KPStarLordTransition>& kpTransitions) const {
    if (kpTransitions.empty()) return "";

    std::stringstream ss;
    ss << "\n┌─ KP TRANSITIONS TABLE ─────────────────────────┐\n";
    ss << "│ Planet    │ Level │ Transition             │\n";
    ss << "├───────────┼───────┼────────────────────────┤\n";

    for (const auto& transition : kpTransitions) {
        std::string planet = getPlanetName(transition.planet);
        planet.resize(9, ' '); // Pad to 9 characters

        std::string level = std::to_string(transition.level);
        level.resize(5, ' '); // Pad to 5 characters

        std::string desc = transition.getDescription();
        if (desc.length() > 22) {
            desc = desc.substr(0, 19) + "...";
        } else {
            desc.resize(22, ' ');
        }

        ss << "│ " << planet << " │ " << level << " │ " << desc << " │\n";
    }

    ss << "└───────────┴───────┴────────────────────────┘\n";
    return ss.str();
}

std::string AstroCalendar::getAdvancedPlanetaryDisplay(const AstroCalendarDay& day) const {
    std::stringstream ss;

    // Major planetary transits
    if (!day.planetaryTransitions.empty()) {
        ss << "\n╔══════════ PLANETARY TRANSITS ══════════╗\n";
        for (const auto& transition : day.planetaryTransitions) {
            std::string type = "";
            switch (transition.type) {
                case TransitionType::SIGN_CHANGE: type = "🌟 Sign Change"; break;
                case TransitionType::RETROGRADE: type = "⏪ Retrograde"; break;
                case TransitionType::DIRECT: type = "⏩ Direct"; break;
                case TransitionType::CONJUNCTION: type = "🤝 Conjunction"; break;
                case TransitionType::NAKSHATRA_CHANGE: type = "⭐ Nakshatra"; break;
                case TransitionType::KP_SUB_LORD_CHANGE: type = "🔄 KP Change"; break;
                case TransitionType::ASPECT_FORMATION: type = "📐 Aspect"; break;
                case TransitionType::TRANSIT_INGRESS: type = "🚀 Ingress"; break;
                case TransitionType::STATIONARY_DIRECT: type = "⏸️ Station-D"; break;
                case TransitionType::STATIONARY_RETRO: type = "⏸️ Station-R"; break;
                default: type = "📊 Transit"; break;
            }

            ss << "║ " << type << ": " << transition.getDescription() << "\n";

            if (transition.strength > 0.0) {
                ss << "║   Strength: " << std::fixed << std::setprecision(2)
                   << transition.strength << "/10.0\n";
            }

            if (!transition.affectedHouses.empty()) {
                ss << "║   Houses: ";
                for (size_t i = 0; i < transition.affectedHouses.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << transition.affectedHouses[i];
                }
                ss << "\n";
            }
        }
        ss << "╚════════════════════════════════════════╝\n";
    }

    return ss.str();
}

} // namespace Astro
