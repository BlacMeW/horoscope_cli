#include "ancient_astronomy.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Astro {

// ─────────────────────────────────────────────────────────────────────────────
// Sexagesimal Implementation
// ─────────────────────────────────────────────────────────────────────────────

std::string Sexagesimal::toString() const {
    std::ostringstream oss;
    oss << deg << ";" << std::setw(2) << std::setfill('0') << min
        << "," << std::setw(2) << std::setfill('0') << sec;
    return oss.str();
}

std::string Sexagesimal::toDmsString() const {
    std::ostringstream oss;
    oss << deg << "° " << std::setw(2) << std::setfill('0') << min
        << "' " << std::setw(2) << std::setfill('0') << sec << "\"";
    return oss.str();
}

// ─────────────────────────────────────────────────────────────────────────────
// Babylonian Calculator Implementation
// ─────────────────────────────────────────────────────────────────────────────

double BabylonianCalculator::zigzagTri(double angleRad) {
    const double twoPi = 2.0 * M_PI;
    double ph = std::fmod(angleRad, twoPi);
    if (ph < 0.0) ph += twoPi;
    double t = ph - M_PI;
    return 1.0 - (2.0 / M_PI) * std::abs(t);
}

double BabylonianCalculator::stepSign(double angleRad) {
    return (std::sin(angleRad) >= 0.0) ? 1.0 : -1.0;
}

double BabylonianCalculator::getCorrectionAmplitude(const std::string& planetName) {
    std::string p = planetName;
    std::transform(p.begin(), p.end(), p.begin(), ::tolower);
    if (p == "moon") return 6.0;
    if (p == "mercury") return 3.0;
    if (p == "venus") return 1.5;
    if (p == "mars") return 0.8;
    if (p == "jupiter") return 0.5;
    if (p == "saturn") return 0.4;
    if (p == "uranus" || p == "neptune") return 0.25;
    if (p == "pluto") return 0.2;
    return 0.4;
}

double BabylonianCalculator::computeBabylonianCorrection(double meanMRad, const std::string& planetName,
                                                         double dtDays, double n,
                                                         double customAmpDeg, double stepBiasRatio) {
    double ampDeg = (customAmpDeg >= 0.0) ? customAmpDeg : getCorrectionAmplitude(planetName);
    double ampRad = ampDeg * (M_PI / 180.0);
    double phase = meanMRad + n * dtDays;
    double zig = zigzagTri(phase);
    double step = stepSign(phase);
    return ampRad * zig + ampRad * stepBiasRatio * step;
}

double BabylonianCalculator::computeBabylonianTheta(double theta0Rad, double meanMRad, const std::string& planetName,
                                                    double dtDays, double n) {
    double corr = computeBabylonianCorrection(meanMRad, planetName, dtDays, n);
    return theta0Rad + corr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Ptolemaic Solver Implementation
// ─────────────────────────────────────────────────────────────────────────────

double PtolemaicSolver::solvePtolemaicTheta(double MRad, double e, int iterations, double tol) {
    double theta = MRad;
    for (int i = 0; i < iterations; ++i) {
        double next = MRad - 2.0 * e * std::sin(theta);
        if (std::abs(next - theta) < tol) {
            theta = next;
            break;
        }
        theta = next;
    }
    return std::atan2(std::sin(theta), std::cos(theta));
}

double PtolemaicSolver::solveKeplerEquation(double MRad, double e, int iterations, double tol) {
    double E = MRad + e * std::sin(MRad);
    for (int i = 0; i < iterations; ++i) {
        double f = E - e * std::sin(E) - MRad;
        double df = 1.0 - e * std::cos(E);
        double delta = f / df;
        E -= delta;
        if (std::abs(delta) < tol) break;
    }
    return E;
}

double PtolemaicSolver::eccentricToTrueAnomaly(double ERad, double e) {
    double sinNu = std::sqrt(1.0 - e * e) * std::sin(ERad) / (1.0 - e * std::cos(ERad));
    double cosNu = (std::cos(ERad) - e) / (1.0 - e * std::cos(ERad));
    return std::atan2(sinNu, cosNu);
}

double PtolemaicSolver::radiusFromEccentricAnomaly(double a, double e, double ERad) {
    return a * (1.0 - e * std::cos(ERad));
}

// ─────────────────────────────────────────────────────────────────────────────
// Surya Siddhanta Calculator Implementation
// ─────────────────────────────────────────────────────────────────────────────

double SuryaSiddhantaCalculator::getJulianDayKYStart() {
    return 588465.5; // Kali Yuga start: 17 Feb 3102 BCE
}

double SuryaSiddhantaCalculator::calculateLahiriAyanamsha(double julianDay) {
    const double ayanamshaJ2000 = 23.85;
    const double precessionDegPerYear = 0.013969;
    double julianYears = (julianDay - 2451545.0) / 365.25;
    return ayanamshaJ2000 + julianYears * precessionDegPerYear;
}

const std::vector<SuryaSiddhantaCalculator::PlanetParameters>& SuryaSiddhantaCalculator::getPlanets() {
    static const std::vector<PlanetParameters> planets = {
        {"sun", "Sun", "Ravi", 4320000.0, 0.0, 20.0, 0.0, 77.0, 0.0, false, 2},
        {"moon", "Moon", "Chandra", 57753336.0, 0.0, 30.0, 0.0, 0.0, 488219.0, false, 2},
        {"mars", "Mars", "Mangala", 2296824.0, 4320000.0, 40.0, 70.0, 130.0, 0.0, false, 4},
        {"mercury", "Mercury", "Budha", 4320000.0, 17937000.0, 25.0, 50.0, 220.0, 0.0, true, 4},
        {"jupiter", "Jupiter", "Guru", 364220.0, 4320000.0, 30.0, 40.0, 170.0, 0.0, false, 4},
        {"venus", "Venus", "Shukra", 4320000.0, 7022376.0, 15.0, 90.0, 80.0, 0.0, true, 4},
        {"saturn", "Saturn", "Shani", 146564.0, 4320000.0, 38.0, 30.0, 236.0, 0.0, false, 4},
        {"rahu", "Rahu", "Rahu (Node)", -232300.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, 1}
    };
    return planets;
}

SuryaSiddhantaCalculator::CalculationStage SuryaSiddhantaCalculator::calculatePlanet(
    const std::string& planetKey, double julianDay, double modernLonDeg) {
    const auto& list = getPlanets();
    auto it = std::find_if(list.begin(), list.end(), [&](const PlanetParameters& p) {
        return p.key == planetKey;
    });

    CalculationStage stage;
    if (it == list.end()) return stage;
    const auto& p = *it;
    stage.planetName = p.name;
    stage.modernLonDeg = modernLonDeg;

    const double civilDaysMahayuga = 1577917828.0;
    double elapsedDays = julianDay - getJulianDayKYStart();

    // Stage 1: Madhyama (Mean Longitude)
    double meanSpeedDegPerDay = (p.revolutionsPerMahayuga * 360.0) / civilDaysMahayuga;
    stage.meanLonDeg = std::fmod(std::fmod(elapsedDays * meanSpeedDegPerDay, 360.0) + 360.0, 360.0);

    // Apogee (Mandocca)
    double mandoccaSpeed = (p.mandaUccaSpeed * 360.0) / civilDaysMahayuga;
    stage.mandoccaDeg = std::fmod(std::fmod(p.mandaAngleDeg + elapsedDays * mandoccaSpeed, 360.0) + 360.0, 360.0);

    // Epicycle geometry simulation
    double orbitR = 150.0;
    double meanAngleRad = stage.meanLonDeg * (M_PI / 180.0);
    double meanX = orbitR * std::cos(meanAngleRad);
    double meanY = orbitR * std::sin(meanAngleRad);

    // Stage 2: Mandaphala (Equation of Center)
    double mandaAngleRad = stage.mandoccaDeg * (M_PI / 180.0);
    double mandaX = meanX + p.mandaSize * std::cos(mandaAngleRad);
    double mandaY = meanY + p.mandaSize * std::sin(mandaAngleRad);
    stage.mandaLonDeg = std::fmod(std::atan2(mandaY, mandaX) * (180.0 / M_PI) + 360.0, 360.0);
    stage.mandaCorrDeg = stage.mandaLonDeg - stage.meanLonDeg;
    if (stage.mandaCorrDeg > 180.0) stage.mandaCorrDeg -= 360.0;
    if (stage.mandaCorrDeg < -180.0) stage.mandaCorrDeg += 360.0;

    // Stage 3: Sighraphala
    double trueX = mandaX;
    double trueY = mandaY;
    if (p.maxStage >= 4 && p.sighraSize > 0.0) {
        double sighraSpeedDegPerDay = (p.sighraRevolutions * 360.0) / civilDaysMahayuga;
        stage.sighroccaDeg = std::fmod(std::fmod(elapsedDays * sighraSpeedDegPerDay, 360.0) + 360.0, 360.0);
        double sighraAngleRad = stage.sighroccaDeg * (M_PI / 180.0);
        trueX += p.sighraSize * std::cos(sighraAngleRad);
        trueY += p.sighraSize * std::sin(sighraAngleRad);
    } else {
        stage.sighroccaDeg = 0.0;
    }

    // Stage 4: Spashta Graha (True Geocentric Position)
    stage.spashtaLonDeg = std::fmod(std::atan2(trueY, trueX) * (180.0 / M_PI) + 360.0, 360.0);
    stage.sighraCorrDeg = stage.spashtaLonDeg - stage.mandaLonDeg;
    if (stage.sighraCorrDeg > 180.0) stage.sighraCorrDeg -= 360.0;
    if (stage.sighraCorrDeg < -180.0) stage.sighraCorrDeg += 360.0;

    if (modernLonDeg >= 0.0) {
        stage.diffDeg = stage.spashtaLonDeg - modernLonDeg;
        if (stage.diffDeg > 180.0) stage.diffDeg -= 360.0;
        if (stage.diffDeg < -180.0) stage.diffDeg += 360.0;
    } else {
        stage.diffDeg = 0.0;
    }

    return stage;
}

std::vector<SuryaSiddhantaCalculator::CalculationStage> SuryaSiddhantaCalculator::calculateAll(
    double julianDay, const std::map<std::string, double>& modernPositions) {
    std::vector<CalculationStage> stages;
    for (const auto& p : getPlanets()) {
        double modern = -1.0;
        auto it = modernPositions.find(p.key);
        if (it != modernPositions.end()) {
            modern = it->second;
        }
        stages.push_back(calculatePlanet(p.key, julianDay, modern));
    }
    return stages;
}

std::string SuryaSiddhantaCalculator::generateComparisonReport(
    double julianDay, int year, int month, int day,
    const std::map<std::string, double>& modernPositions) {
    auto stages = calculateAll(julianDay, modernPositions);
    double ayanamsha = calculateLahiriAyanamsha(julianDay);

    std::ostringstream oss;
    oss << "=========================================================================================================\n";
    oss << "                     ANCIENT ASTRONOMY & SURYA SIDDHANTA MODEL COMPARISON                                \n";
    oss << "=========================================================================================================\n";
    oss << " Epoch Date  : " << year << "-" << std::setw(2) << std::setfill('0') << month
        << "-" << std::setw(2) << std::setfill('0') << day << " (JD: "
        << std::fixed << std::setprecision(2) << julianDay << ")\n";
    oss << " Kali Yuga   : " << std::fixed << std::setprecision(1) << (julianDay - getJulianDayKYStart())
        << " days elapsed since 17 Feb 3102 BCE\n";
    oss << " Ayanamsha   : " << std::fixed << std::setprecision(2) << ayanamsha << " deg (Lahiri Precession Rate)\n";
    oss << " Reference   : Surya Siddhanta (SS) Authentic Epicycles & Babylonian System A/B Corrections\n";
    oss << "---------------------------------------------------------------------------------------------------------\n";
    oss << " Planet     | Mean (Madhyama) | Manda Corr | Sighra Corr | SS True (Spashta) | Modern Ephem | Difference \n";
    oss << "------------+-----------------+------------+-------------+-------------------+--------------+------------\n";

    for (const auto& s : stages) {
        oss << " " << std::left << std::setw(10) << std::setfill(' ') << s.planetName << " | "
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(14) << s.meanLonDeg << "° | "
            << std::setw(9) << s.mandaCorrDeg << "° | "
            << std::setw(10) << s.sighraCorrDeg << "° | "
            << std::setw(16) << s.spashtaLonDeg << "° | ";
        if (s.modernLonDeg >= 0.0) {
            oss << std::setw(11) << s.modernLonDeg << "° | "
                << std::setw(9) << s.diffDeg << "°\n";
        } else {
            oss << std::setw(11) << "N/A" << " | "
                << std::setw(9) << "N/A" << "\n";
        }
    }

    oss << "---------------------------------------------------------------------------------------------------------\n";
    oss << " BABYLONIAN SYSTEM A/B VELOCITY ANOMALY SAMPLE (Base-60 Sexagesimal Units):\n";
    for (const auto& p : {"mars", "jupiter", "saturn", "venus", "mercury"}) {
        double meanM = 1.0; // sample phase
        double corr = BabylonianCalculator::computeBabylonianCorrection(meanM, p);
        Sexagesimal sex = Sexagesimal::fromDecimal(corr * (180.0 / M_PI));
        oss << "   • " << std::left << std::setw(8) << p
            << " Correction: " << std::right << std::fixed << std::setprecision(4)
            << (corr * 180.0 / M_PI) << "° (" << sex.toString() << " in D;M,S)\n";
    }
    oss << "=========================================================================================================\n";

    return oss.str();
}

} // namespace Astro
