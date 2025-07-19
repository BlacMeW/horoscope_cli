# Three Calendar Systems in Hindu Astrology

## Overview

The enhanced Hindu calendar system supports three distinct calculation approaches, each with different characteristics and use cases. These systems determine how lunar and solar elements are combined to create the traditional Panchanga.

---

## 1. 🌙 **LUNAR-BASED CALENDAR SYSTEM**

### Description
A pure lunar calendar system that primarily follows the Moon's phases and positions for all calculations.

### Key Characteristics
- **Primary Reference**: Moon's position and phases
- **Month Length**: ~29.53 days (one lunar cycle)
- **Year Length**: ~354 days (12 lunar months)
- **Tithi Calculation**: Based purely on lunar phase differences
- **Month Determination**: Lunar month boundaries

### Usage Scenarios
- Traditional Islamic calendar style
- Pure lunar festival calculations
- Regions following strict lunar traditions
- Specific religious observances

### Advantages
- Simple and consistent with lunar cycles
- Clear phase-based calculations
- Traditional accuracy for lunar events

### Limitations
- Drifts against seasons over time
- Requires periodic adjustments
- Less accurate for solar-dependent activities

---

## 2. ☀️ **SOLAR-BASED CALENDAR SYSTEM**

### Description
A solar calendar system that uses the Sun's position as the primary reference while incorporating lunar elements for Tithi calculations.

### Key Characteristics
- **Primary Reference**: Sun's position through zodiac signs
- **Month Length**: ~30.44 days (solar month)
- **Year Length**: ~365.25 days (one solar year)
- **Month Determination**: Solar transitions (Sankranti)
- **Season Alignment**: Fixed to seasonal cycles

### Usage Scenarios
- Tamil calendar system
- Regional calendars in South India
- Agricultural planning
- Seasonal festival calculations

### Advantages
- Aligned with seasons and solar year
- Consistent for agricultural purposes
- No seasonal drift over time
- Good for solar-dependent calculations

### Limitations
- Less traditional for lunar observances
- Complex lunar tithi calculations
- May not match classical texts exactly

---

## 3. 🌙☀️ **LUNI-SOLAR CALENDAR SYSTEM** (Default)

### Description
A combined system that integrates both lunar and solar elements, representing the most traditional and widely-used Hindu calendar approach.

### Key Characteristics
- **Dual Reference**: Both Sun and Moon positions
- **Month Structure**: Lunar months with solar year alignment
- **Intercalation**: Periodic leap months (Adhik Maas)
- **Seasonal Sync**: Solar corrections prevent seasonal drift
- **Traditional Accuracy**: Matches classical Panchanga systems

### Usage Scenarios
- Most Hindu festivals and observances
- Traditional Panchanga calculations
- Mainstream Indian calendar systems
- Classical astrological calculations

### Advantages
- Best of both lunar and solar systems
- Seasonal alignment maintained
- Traditional compatibility
- Comprehensive astronomical accuracy
- Supports all classical calculations

### Limitations
- More complex calculations
- Requires intercalation rules
- Higher computational overhead

---

## Implementation Details

### Code Configuration
```cpp
enum class CalendarSystem {
    LUNAR_BASED = 1,     // Pure lunar calendar
    SOLAR_BASED = 2,     // Solar calendar with lunar tithis
    LUNI_SOLAR = 3       // Combined system (most common)
};

// Usage example
HinduCalendar lunarCal(AyanamsaType::LAHIRI, CalculationMethod::DRIK_SIDDHANTA, CalendarSystem::LUNAR_BASED);
HinduCalendar solarCal(AyanamsaType::LAHIRI, CalculationMethod::DRIK_SIDDHANTA, CalendarSystem::SOLAR_BASED);
HinduCalendar luniSolarCal; // Default: LUNI_SOLAR
```

### Calculation Differences

| Element | Lunar-Based | Solar-Based | Luni-Solar |
|---------|-------------|-------------|------------|
| **Month Start** | New Moon | Sankranti | New Moon + Solar Correction |
| **Year Length** | 354 days | 365.25 days | ~365.25 days (with Adhik Maas) |
| **Tithi Basis** | Pure lunar phase | Solar month + lunar phase | Traditional lunar-solar |
| **Season Sync** | Drifts | Fixed | Maintained |
| **Leap Adjustment** | None | None | Adhik Maas |

---

## Practical Examples

### Example Date: July 25, 2025 (From Attachment)

Based on the Pancanga data shown in the attachment:

#### Lunar-Based System
- **Focus**: Moon in Shravana Nakshatra
- **Month**: Determined by lunar phase
- **Tithi**: Pure lunar calculation (shown as waning 15 - new moon)
- **Year**: Lunar year count

#### Solar-Based System
- **Focus**: Sun's zodiacal position
- **Month**: Solar month (Ashadha → Shravana transition)
- **Tithi**: Solar month + lunar phase overlay
- **Year**: Solar year alignment

#### Luni-Solar System (Traditional)
- **Focus**: Integrated approach
- **Month**: Traditional Shravana month
- **Tithi**: Classical calculation method
- **Year**: Vikrama 2082 with seasonal alignment
- **Special**: Krsnpaksa (Krishna Paksha) waning phase

---

## Regional Preferences

### North India
- **Preferred**: Luni-Solar (Vikram Samvat)
- **Characteristics**: Amanta system (month ends on Amavasya)
- **Festivals**: Based on traditional luni-solar calculations

### South India
- **Preferred**: Solar-based (Tamil regions) or Luni-Solar
- **Characteristics**: Purnimanta system (month ends on Purnima)
- **Regional Variations**: Tamil solar calendar, Telugu luni-solar

### Western India
- **Preferred**: Luni-Solar
- **Characteristics**: Mixed traditions
- **Commercial**: Often solar for business purposes

---

## Choosing the Right System

### Use Lunar-Based When:
- Following Islamic or pure lunar traditions
- Calculating lunar-specific religious events
- Working with lunar phase-dependent activities
- Research into ancient lunar calendars

### Use Solar-Based When:
- Agricultural planning and seasonal activities
- Regional requirements (Tamil calendar)
- Solar festival calculations
- Seasonal business planning

### Use Luni-Solar When:
- Traditional Hindu festival calculations
- Classical astrological work
- Mainstream Panchanga requirements
- Historical compatibility needed
- **Most recommended for general use**

---

## Technical Implementation Notes

The system automatically handles:
- **Ayanamsa corrections** for all three systems
- **Intercalation rules** for luni-solar system
- **Seasonal adjustments** where applicable
- **Regional variations** through configuration
- **Traditional compatibility** with classical texts

Each system maintains the same interface but applies different calculation algorithms internally, ensuring consistent API usage while providing authentic results for each tradition.
