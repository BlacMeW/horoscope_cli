#!/bin/bash

echo "═══════════════════════════════════════════════════════════════════"
echo "              🕉️  HINDU CALENDAR VALIDATION TESTS  🕉️"
echo "═══════════════════════════════════════════════════════════════════"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

EXECUTABLE=""
for candidate in "$ROOT_DIR/bin/horoscope_cli" "$ROOT_DIR/build/horoscope_cli" "$ROOT_DIR/horoscope_cli" "./bin/horoscope_cli" "./horoscope_cli"; do
    if [ -x "$candidate" ]; then
        EXECUTABLE="$candidate"
        break
    fi
done

if [ -z "$EXECUTABLE" ]; then
    echo "Error: horoscope_cli executable not found."
    exit 1
fi

# Test 1: Basic Panchanga for a specific date
echo
echo "TEST 1: Basic Panchanga Display"
echo "Date: January 15, 1990 (Delhi)"
echo "─────────────────────────────────────────────────────────────────"
$EXECUTABLE --date 1990-01-15 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 2: JSON Format Output
echo "TEST 2: JSON Format Output"
echo "─────────────────────────────────────────────────────────────────"
$EXECUTABLE --date 2024-01-14 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga --panchanga-format json

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 3: Date Range (3 days)
echo "TEST 3: Panchanga Range (3 days in table format)"
echo "─────────────────────────────────────────────────────────────────"
$EXECUTABLE --date 2024-01-14 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga-range 2024-01-14 2024-01-16

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 4: CSV Format for Range
echo "TEST 4: CSV Format for Date Range"
echo "─────────────────────────────────────────────────────────────────"
$EXECUTABLE --date 2024-01-14 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga-range 2024-01-14 2024-01-16 --panchanga-format csv

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 5: Different Location (Mumbai)
echo "TEST 5: Different Location - Mumbai"
echo "─────────────────────────────────────────────────────────────────"
$EXECUTABLE --date 2024-01-14 --time 12:00:00 --lat 19.0760 --lon 72.8777 --timezone 5.5 --panchanga

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 6: Historical Date (ancient period)
echo "TEST 6: Historical Date - Year 1000 AD"
echo "─────────────────────────────────────────────────────────────────"
$EXECUTABLE --date 1000-01-15 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga

echo
echo "═══════════════════════════════════════════════════════════════════"
echo "                     🎉  ALL TESTS COMPLETED  🎉"
echo "═══════════════════════════════════════════════════════════════════"
