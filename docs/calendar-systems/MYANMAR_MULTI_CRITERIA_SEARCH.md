# Myanmar Calendar Multi-Criteria Search with AND/OR Logic

## Overview
The Myanmar Calendar now supports advanced multi-criteria search functionality with logical operators (AND/OR), allowing you to combine multiple search criteria in flexible ways.

## Key Features

### ✅ What's New
- **AND Logic** (default): All criteria must match
- **OR Logic**: Any criteria can match
- **Mixed Criteria**: Combine different types of criteria (moon phases, astrological days, weekdays, etc.)
- **Helper Methods**: Pre-built functions for common searches

### 🔍 Search Logic Modes

#### AND Logic (Default)
All specified criteria must match for a date to be included in results.
```cpp
// Example: Sabbath AND Full Moon
criteria.searchSabbath = true;
criteria.exactMoonPhase = 1; // Full moon
criteria.logicMode = LogicMode::AND;
// Result: Only dates that are BOTH sabbath AND full moon
```

#### OR Logic
Any of the specified criteria can match for a date to be included.
```cpp
// Example: Full Moon OR Yatyaza
criteria.exactMoonPhase = 1; // Full moon
criteria.searchYatyaza = true;
criteria.logicMode = LogicMode::OR;
// Result: Dates that are EITHER full moon OR yatyaza (or both)
```

## Programming Interface

### Core Types
```cpp
enum class LogicMode {
    AND,  // All criteria must match (default)
    OR    // Any criteria can match
};

struct SearchCriteria {
    // ... existing criteria fields ...
    LogicMode logicMode = LogicMode::AND;
    // ... date range fields ...
};
```

### Helper Methods
```cpp
// Pre-built searches for common use cases
std::vector<SearchResult> searchFullMoonOrYatyaza(startDate, endDate, lat, lng);
std::vector<SearchResult> searchSabbathAndFullMoon(startDate, endDate, lat, lng);
std::vector<SearchResult> searchMultiCriteria(criteria, logicMode, startDate, endDate, lat, lng);
```

### Custom Search Example
```cpp
MyanmarCalendar calendar;
calendar.initialize();

// Search for "Sabbath OR Thamanyo OR Pyathada"
MyanmarCalendar::SearchCriteria criteria;
criteria.searchSabbath = true;
criteria.searchThamanyo = true;
criteria.searchPyathada = true;

auto results = calendar.searchMultiCriteria(
    criteria,
    MyanmarCalendar::LogicMode::OR,
    "2024-01-01", "2024-01-31",
    16.8409, 96.1735  // Yangon coordinates
);

for (const auto& result : results) {
    std::cout << result.gregorianDate << " - " << result.matchDescription << std::endl;
}
```

## Command Line Interface

### Basic Syntax
```bash
./horoscope_cli --myanmar-search START_DATE END_DATE \
    [criteria1] [criteria2] ... \
    --myanmar-search-logic and|or \
    [other options]
```

### Examples

#### Full Moon OR Yatyaza
```bash
./horoscope_cli --myanmar-search 2024-01-01 2024-01-31 \
    --myanmar-search-moon-phase 1 \
    --myanmar-search-yatyaza \
    --myanmar-search-logic or
```

#### Sabbath AND Full Moon
```bash
./horoscope_cli --myanmar-search 2024-01-01 2024-03-31 \
    --myanmar-search-sabbath \
    --myanmar-search-moon-phase 1 \
    --myanmar-search-logic and
```

#### Weekend Days (Saturday OR Sunday)
```bash
./horoscope_cli --myanmar-search 2024-02-01 2024-02-29 \
    --myanmar-search-weekday 0 \
    --myanmar-search-weekday 1 \
    --myanmar-search-logic or
```

#### Multiple Astrological Events
```bash
./horoscope_cli --myanmar-search 2024-01-01 2024-01-15 \
    --myanmar-search-sabbath \
    --myanmar-search-yatyaza \
    --myanmar-search-pyathada \
    --myanmar-search-logic or
```

## Available Search Criteria

### Temporal Criteria
- `--myanmar-search-year YEAR`
- `--myanmar-search-year-range START END`
- `--myanmar-search-month MONTH`
- `--myanmar-search-month-range START END`
- `--myanmar-search-weekday WEEKDAY` (0=Saturday, 6=Friday)

### Lunar Criteria
- `--myanmar-search-moon-phase PHASE` (0=Waxing, 1=Full, 2=Waning, 3=New)
- `--myanmar-search-moon-phase-range START END`
- `--myanmar-search-fortnight-day DAY` (1-15)
- `--myanmar-search-fortnight-day-range START END`

### Religious/Astrological Criteria
- `--myanmar-search-sabbath` - Buddhist sabbath days
- `--myanmar-search-sabbath-eve` - Day before sabbath
- `--myanmar-search-yatyaza` - Inauspicious days
- `--myanmar-search-pyathada` - Inauspicious days
- `--myanmar-search-thamanyo` - Auspicious days

### Logic Control
- `--myanmar-search-logic and` - All criteria must match (default)
- `--myanmar-search-logic or` - Any criteria can match

## Common Use Cases

### 🚫 Avoidance Planning
Find days to avoid (inauspicious combinations):
```bash
# Avoid both full moon and yatyaza days
--myanmar-search-moon-phase 1 --myanmar-search-yatyaza --myanmar-search-logic or
```

### ✅ Auspicious Event Planning
Find highly favorable days:
```bash
# Sabbath AND full moon (very auspicious)
--myanmar-search-sabbath --myanmar-search-moon-phase 1 --myanmar-search-logic and
```

### 📅 Flexible Scheduling
Find available days matching multiple criteria:
```bash
# Weekend OR sabbath days
--myanmar-search-weekday 0 --myanmar-search-weekday 1 --myanmar-search-sabbath --myanmar-search-logic or
```

### 🎯 Specific Combinations
Target exact astrological alignments:
```bash
# Thamanyo AND waxing moon (growing fortune)
--myanmar-search-thamanyo --myanmar-search-moon-phase 0 --myanmar-search-logic and
```

## Match Scoring

Each result includes a match score:
- **1.0**: Perfect match (all criteria matched)
- **0.5**: Partial match (some criteria matched with OR logic)
- **0.33**: One-third match (1 of 3 criteria matched)

## Best Practices

1. **Start with OR logic** for exploratory searches
2. **Use AND logic** for precise requirements
3. **Combine temporal and astrological criteria** for practical planning
4. **Check match scores** to understand result quality
5. **Use helper methods** for common patterns

## Files and Examples

- `examples/myanmar_multi_criteria_search_examples.sh` - Comprehensive examples
- `test_myanmar_multi_search.cpp` - Programming examples
- `example_full_moon_or_yatyaza.sh` - Quick start example

## Technical Notes

- Results are sorted by match score (descending), then by date (ascending)
- All searches require latitude/longitude for accurate calculations
- Logic is applied consistently across all criteria types
- Search ranges should be reasonable (avoid very large date ranges for performance)
