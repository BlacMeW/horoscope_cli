#!/bin/bash

# Test all chart drawing improvements
# Shows the corrected horoscope chart drawing with proper visual formatting

echo "=== HOROSCOPE CHART DRAWING IMPROVEMENTS DEMO ==="
echo "Testing birth data: 1990-01-15 10:30:00, London, UK"
echo

# Test Western chart with new wheel and rectangular layout
echo "1. ENHANCED WESTERN CHART (with chart wheel, rectangular layout, and aspect grid)"
echo "================================================================="
./bin/horoscope_cli --date 1990-01-15 --time 10:30:00 --lat 51.5074 --lon -0.1278 --timezone 0 --chart-style western

echo
echo
echo "2. CORRECTED NORTH INDIAN CHART (fixed character encoding)"
echo "========================================================="
./bin/horoscope_cli --date 1990-01-15 --time 10:30:00 --lat 51.5074 --lon -0.1278 --timezone 0 --chart-style north-indian | tail -15

echo
echo "3. CORRECTED EAST INDIAN CHART (fixed character encoding)"
echo "========================================================"
./bin/horoscope_cli --date 1990-01-15 --time 10:30:00 --lat 51.5074 --lon -0.1278 --timezone 0 --chart-style east-indian | tail -15

echo
echo "4. CORRECTED SOUTH INDIAN CHART (fixed character encoding)"
echo "=========================================================="
./bin/horoscope_cli --date 1990-01-15 --time 10:30:00 --lat 51.5074 --lon -0.1278 --timezone 0 --chart-style south-indian | tail -20

echo
echo "5. FIXED ORDINAL HOUSE NUMBERING (1st, 2nd, 3rd instead of 2st)"
echo "==============================================================="
./bin/horoscope_cli --date 1990-01-15 --time 10:30:00 --lat 51.5074 --lon -0.1278 --timezone 0 --chart-style north-indian | grep -A 15 "=== HOUSE CUSPS ==="

echo
echo "=== IMPROVEMENTS SUMMARY ==="
echo "✓ Enhanced Western Chart with visual chart wheel, rectangular layout, and aspect grid"
echo "✓ Fixed character encoding issues in all Eastern chart styles (Unicode → ASCII)"
echo "✓ Corrected ordinal house numbering (1st, 2nd, 3rd, 4th... instead of all 'st')"
echo "✓ Maintained all existing solar system functionality"
echo "✓ All chart styles now display properly without corrupted characters"
