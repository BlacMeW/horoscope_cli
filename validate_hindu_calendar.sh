#!/bin/bash

echo "═══════════════════════════════════════════════════════════════════"
echo "              🕉️  HINDU CALENDAR VALIDATION TESTS  🕉️"
echo "═══════════════════════════════════════════════════════════════════"

# Test 1: Basic Panchanga for a specific date
echo
echo "TEST 1: Basic Panchanga Display"
echo "Date: January 15, 1990 (Delhi)"
echo "─────────────────────────────────────────────────────────────────"
./bin/horoscope_cli --date 1990-01-15 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 2: JSON Format Output
echo "TEST 2: JSON Format Output"
echo "─────────────────────────────────────────────────────────────────"
./bin/horoscope_cli --date 2024-01-14 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga --panchanga-format json

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 3: Date Range (3 days)
echo "TEST 3: Panchanga Range (3 days in table format)"
echo "─────────────────────────────────────────────────────────────────"
./bin/horoscope_cli --date 2024-01-14 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga-range 2024-01-14 2024-01-16

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 4: CSV Format for Range
echo "TEST 4: CSV Format for Date Range"
echo "─────────────────────────────────────────────────────────────────"
./bin/horoscope_cli --date 2024-01-14 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga-range 2024-01-14 2024-01-16 --panchanga-format csv

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 5: Different Location (Mumbai)
echo "TEST 5: Different Location - Mumbai"
echo "─────────────────────────────────────────────────────────────────"
./bin/horoscope_cli --date 2024-01-14 --time 12:00:00 --lat 19.0760 --lon 72.8777 --timezone 5.5 --panchanga

echo
echo "═══════════════════════════════════════════════════════════════════"
echo

# Test 6: Historical Date (ancient period)
echo "TEST 6: Historical Date - Year 1000 AD"
echo "─────────────────────────────────────────────────────────────────"
./bin/horoscope_cli --date 1000-01-15 --time 12:00:00 --lat 28.6139 --lon 77.2090 --timezone 5.5 --panchanga

echo
echo "═══════════════════════════════════════════════════════════════════"
echo "                     🎉  ALL TESTS COMPLETED  🎉"
echo "═══════════════════════════════════════════════════════════════════"
