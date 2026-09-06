# Tests & Debug Utilities (`horoscope_cli`)

This directory contains standalone unit tests, verification suites, and debug utilities for the `horoscope_cli` project.

## Directory Layout

- **`tests/*.cpp`**: C++ test cases and calculation verification programs:
  - Astrological calculation tests (`test_ayanamsa_*.cpp`, `test_house_*.cpp`, `test_planetary_*.cpp`, `test_planet_rise_set.cpp`, etc.)
  - Calendar system tests (`test_chinese_calendar.cpp`, `test_enhanced_panchanga.cpp`, `test_myanmar_*.cpp`, etc.)
  - Core algorithm tests (`test_jd_*.cpp`, `test_julian_day_*.cpp`, `test_kp_*.cpp`, etc.)
  - Debug scripts (`debug_*.cpp`)
- **`tests/*.sh`**: Test runner shell scripts:
  - `test.sh`: Main functional test script testing CLI options, birth charts, output formats, and chart styles.
  - `test_enhanced_hindu_calendar.sh`: Build & execution script for Hindu Panchanga tests.
  - `validate_hindu_calendar.sh`: Validation suite for Hindu calendar dates, formats, and ranges.
- **`tests/bin/`**: Directory containing compiled test and debug executables.

## Running Tests

From the project root directory:

```bash
# Run the main test suite
./tests/test.sh

# Run Hindu calendar validation suite
./tests/validate_hindu_calendar.sh

# Build and run enhanced Hindu Panchanga test
./tests/test_enhanced_hindu_calendar.sh
```

## Compiling Individual Tests

To compile an individual test from the repository root:

```bash
g++ -std=c++17 -O2 -Iinclude -Ithird_party/swisseph \
    -o tests/bin/<test_name> tests/<test_name>.cpp \
    src/<dependencies>.cpp third_party/swisseph/*.c -lm
```
