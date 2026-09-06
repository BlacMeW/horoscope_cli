#pragma once

#include <string>
#include <vector>
#include <map>
#include <cmath>

namespace Astro {

/// Base-60 Sexagesimal angle representation (D;M,S) as used in Babylonian & Indian astronomy
class Sexagesimal {
public:
    int deg;
    int min;
    int sec;

    Sexagesimal(int d = 0, int m = 0, int s = 0) : deg(d), min(m), sec(s) {}

    double toDecimal() const {
        int sign = (deg < 0) ? -1 : 1;
        int d = std::abs(deg);
        double v = d + (min / 60.0) + (sec / 3600.0);
        return v * sign;
    }

    static Sexagesimal fromDecimal(double value) {
        int sign = (value < 0) ? -1 : 1;
        double absVal = std::abs(value);
        int d = static_cast<int>(std::floor(absVal));
        double mFloat = (absVal - d) * 60.0;
        int m = static_cast<int>(std::floor(mFloat));
        int s = static_cast<int>(std::round((mFloat - m) * 60.0));
        return Sexagesimal(d * sign, m, s);
    }

    Sexagesimal operator+(const Sexagesimal& other) const {
        return Sexagesimal::fromDecimal(toDecimal() + other.toDecimal());
    }

    Sexagesimal operator-(const Sexagesimal& other) const {
        return Sexagesimal::fromDecimal(toDecimal() - other.toDecimal());
    }

    Sexagesimal mulScalar(double factor) const {
        return Sexagesimal::fromDecimal(toDecimal() * factor);
    }

    Sexagesimal normalize() const {
        double val = std::fmod(toDecimal(), 360.0);
        if (val < 0.0) val += 360.0;
        return Sexagesimal::fromDecimal(val);
    }

    std::string toString() const;
    std::string toDmsString() const;
};

/// Babylonian System A and System B Astronomical Calculations
class BabylonianCalculator {
public:
    static double zigzagTri(double angleRad);
    static double stepSign(double angleRad);
    static double getCorrectionAmplitude(const std::string& planetName);
    static double computeBabylonianCorrection(double meanMRad, const std::string& planetName,
                                              double dtDays = 0.0, double n = 0.0,
                                              double customAmpDeg = -1.0, double stepBiasRatio = 0.35);
    static double computeBabylonianTheta(double theta0Rad, double meanMRad, const std::string& planetName,
                                         double dtDays = 0.0, double n = 0.0);
};

/// Ptolemaic Orbital Equation and Equant Solver
class PtolemaicSolver {
public:
    static double solvePtolemaicTheta(double MRad, double e, int iterations = 10, double tol = 1e-12);
    static double solveKeplerEquation(double MRad, double e, int iterations = 15, double tol = 1e-12);
    static double eccentricToTrueAnomaly(double ERad, double e);
    static double radiusFromEccentricAnomaly(double a, double e, double ERad);
};

/// Surya Siddhanta Multi-Stage Planetary Calculation Model
class SuryaSiddhantaCalculator {
public:
    struct PlanetParameters {
        std::string key;
        std::string name;
        std::string sanskritName;
        double revolutionsPerMahayuga;
        double sighraRevolutions;
        double mandaSize;      // epicycle circumference in degrees
        double sighraSize;     // epicycle circumference in degrees
        double mandaAngleDeg;  // initial apogee longitude (Mandocca at KY epoch)
        double mandaUccaSpeed; // angular speed of apogee (for Moon)
        bool isInferior;
        int maxStage;
    };

    struct CalculationStage {
        std::string planetName;
        double meanLonDeg;       // Stage 1: Madhyama Graha
        double mandoccaDeg;      // Apogee direction
        double mandaCorrDeg;     // Stage 2: Mandaphala (Equation of Center)
        double mandaLonDeg;      // Post-manda longitude
        double sighroccaDeg;     // Parallax / heliocentric fast motion
        double sighraCorrDeg;    // Stage 3: Sighraphala
        double spashtaLonDeg;    // Stage 4: Spashta Graha (True Geocentric Position)
        double modernLonDeg;     // High-precision modern ephemeris comparison
        double diffDeg;          // Difference vs modern ephemeris
    };

    static const std::vector<PlanetParameters>& getPlanets();
    static double getJulianDayKYStart(); // Kali Yuga epoch: 17 Feb 3102 BCE (JD 588465.5)
    static double calculateLahiriAyanamsha(double julianDay);
    static CalculationStage calculatePlanet(const std::string& planetKey, double julianDay, double modernLonDeg = -1.0);
    static std::vector<CalculationStage> calculateAll(double julianDay, const std::map<std::string, double>& modernPositions = {});
    static std::string generateComparisonReport(double julianDay, int year, int month, int day,
                                                const std::map<std::string, double>& modernPositions = {});
};

} // namespace Astro
