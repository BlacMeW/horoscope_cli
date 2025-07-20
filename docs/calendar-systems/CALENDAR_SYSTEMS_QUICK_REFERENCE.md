# Quick Reference: Three Hindu Calendar Systems

## 📋 **SYSTEM COMPARISON TABLE**

| **Aspect** | **🌙 LUNAR-BASED** | **☀️ SOLAR-BASED** | **🌙☀️ LUNI-SOLAR** |
|------------|-------------------|------------------|-------------------|
| **Primary Reference** | Moon phases & position | Sun's zodiacal position | Both Sun & Moon |
| **Month Length** | ~29.53 days | ~30.44 days | Variable (29-32 days) |
| **Year Length** | ~354 days (12 lunar months) | ~365.25 days (solar year) | ~365.25 days (with intercalation) |
| **Month Boundaries** | New Moon (Amavasya) | Solar transitions (Sankranti) | New Moon + Solar corrections |
| **Season Alignment** | Drifts over time | Fixed to seasons | Maintained via leap months |
| **Leap Adjustments** | None (gradual drift) | None needed | Adhik Maas (extra month) |
| **Tithi Calculation** | Pure lunar phase difference | Solar month + lunar overlay | Traditional lunar-solar method |
| **Regional Usage** | Islamic traditions, Pure lunar | Tamil calendar, South India | Most of India, Classical texts |
| **Best For** | Lunar festivals, Moon-based rituals | Agriculture, Seasonal planning | Traditional Hindu festivals |
| **Complexity** | Simple | Moderate | Complex (most authentic) |
| **Historical Accuracy** | Ancient lunar calendars | Regional solar systems | Classical Hindu astronomy |

---

## 🎯 **WHEN TO USE EACH SYSTEM**

### 🌙 **LUNAR-BASED**
- **Islamic calendar compatibility**
- **Pure lunar religious observances**
- **Moon-phase dependent rituals**
- **Ancient lunar calendar research**
- **Simple lunar cycle calculations**

### ☀️ **SOLAR-BASED**
- **Tamil calendar requirements**
- **Agricultural planning**
- **Seasonal business cycles**
- **Solar festival calculations**
- **Fixed seasonal alignments**

### 🌙☀️ **LUNI-SOLAR** (Recommended Default)
- **Traditional Hindu festivals**
- **Classical astrological calculations**
- **Mainstream Indian calendars**
- **Historical Panchanga compatibility**
- **Comprehensive astronomical accuracy**

---

## ⚙️ **TECHNICAL IMPLEMENTATION**

```cpp
// Three calendar system options in the enum
enum class CalendarSystem {
    LUNAR_BASED = 1,     // Pure lunar calendar
    SOLAR_BASED = 2,     // Solar calendar with lunar tithis
    LUNI_SOLAR = 3       // Combined system (most common)
};

// Constructor examples
HinduCalendar lunarCal(AyanamsaType::LAHIRI,
                       CalculationMethod::DRIK_SIDDHANTA,
                       CalendarSystem::LUNAR_BASED);

HinduCalendar solarCal(AyanamsaType::LAHIRI,
                       CalculationMethod::SURYA_SIDDHANTA,
                       CalendarSystem::SOLAR_BASED);

HinduCalendar defaultCal;  // Uses LUNI_SOLAR by default
```

---

## 📊 **EXAMPLE OUTPUT DIFFERENCES**

For the same date/time (July 25, 2025, 12:00 PM, New Delhi):

### **LUNAR-BASED RESULT**
```
Hindu Month: Based on lunar cycle count
Paksha: Pure lunar phase calculation
Tithi: Moon-Sun angular difference
Year: Lunar year count (354-day cycles)
```

### **SOLAR-BASED RESULT**
```
Hindu Month: Solar zodiac transitions
Paksha: Solar month + lunar phase overlay
Tithi: Solar-adjusted lunar calculation
Year: Solar year alignment (365.25 days)
```

### **LUNI-SOLAR RESULT**
```
Hindu Month: Traditional combined method
Paksha: Classical lunar-solar calculation
Tithi: Traditional Panchanga method
Year: Vikram Samvat with intercalation
```

---

## 🏛️ **HISTORICAL CONTEXT**

- **LUNAR-BASED**: Oldest system, used in ancient civilizations
- **SOLAR-BASED**: Regional adaptations, especially in South India
- **LUNI-SOLAR**: Classical Hindu astronomy, most traditional texts

---

## 🔄 **CONVERSION BETWEEN SYSTEMS**

The enhanced Hindu calendar automatically:
- Converts between calendar systems
- Maintains astronomical accuracy
- Preserves traditional calculation methods
- Handles intercalation rules
- Provides consistent API interface

---

*For complete documentation, see `CALENDAR_SYSTEMS_COMPARISON.md`*
