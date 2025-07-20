#pragma once

#include "birth_chart.h"
#include "ephemeris_manager.h"
#include "planet_calculator.h"
#include "house_calculator.h"
#include "aspect_calculator.h"

namespace Astro {

class HoroscopeCalculator {
public:
    HoroscopeCalculator();
    ~HoroscopeCalculator();

    // Initialize the calculator with ephemeris data path
    bool initialize(const std::string& ephemerisPath = "");

    // Calculate complete birth chart
    bool calculateBirthChart(const BirthData& birthData,
                           HouseSystem houseSystem,
                           BirthChart& chart);

    // Calculate birth chart with zodiac mode and ayanamsa
    bool calculateBirthChart(const BirthData& birthData,
                           HouseSystem houseSystem,
                           ZodiacMode zodiacMode,
                           AyanamsaType ayanamsa,
                           BirthChart& chart);

    // Set house system to use
    void setHouseSystem(HouseSystem system);

    // Set zodiac calculation mode (tropical/sidereal)
    void setZodiacMode(ZodiacMode mode);

    // Set ayanamsa for sidereal calculations
    void setAyanamsa(AyanamsaType ayanamsa);

    // Set calculation flags
    void setCalculationFlags(const std::vector<CalculationFlag>& flags);

    // Set which planets to include
    void setPlanetsToCalculate(const std::vector<Planet>& planets);

    // Set aspect orb tolerances
    void setAspectOrbs(const std::map<AspectType, double>& orbs);

    // Get error message
    std::string getLastError() const;

    // Check if calculator is properly initialized
    bool isInitialized() const;

private:
    EphemerisManager ephemerisManager;
    PlanetCalculator planetCalculator;
    HouseCalculator houseCalculator;
    AspectCalculator aspectCalculator;

    HouseSystem defaultHouseSystem;
    ZodiacMode zodiacMode;
    AyanamsaType ayanamsa;
    std::vector<CalculationFlag> calculationFlags;
    std::string lastError;
    bool initialized;

    // Validate birth data
    bool validateBirthData(const BirthData& birthData);
};

} // namespace Astro
