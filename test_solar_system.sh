#!/bin/bash

# Solar System ASCII Art Demonstration Script
# This script demonstrates the different solar system visualization features
# including various planetary perspectives

echo "==========================================================="
echo "Solar System ASCII Art Demo for Horoscope CLI"
echo "==========================================================="

# Build the project first
echo "Building project..."
make clean && make

if [ $? -ne 0 ]; then
    echo "Error: Failed to build project"
    exit 1
fi

echo ""
echo "1. Solar System Orbital Paths (no birth data needed):"
echo "------------------------------------------------------"
./bin/horoscope_cli --solar-system

echo ""
echo "2. Heliocentric View (Sun-centered) - January 15, 1990:"
echo "-------------------------------------------------------"
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system

echo ""
echo "3. Geocentric View (Earth-centered) - January 15, 1990:"
echo "-------------------------------------------------------"
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective geocentric

echo ""
echo "4. Mars-Centric View - January 15, 1990:"
echo "----------------------------------------"
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective mars-centric

echo ""
echo "5. Jupiter-Centric View - January 15, 1990:"
echo "-------------------------------------------"
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective jupiter-centric

echo ""
echo "6. Saturn-Centric View - January 15, 1990:"
echo "------------------------------------------"
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system --perspective saturn-centric

echo ""
echo "7. Historical Example - July 4, 1776 (Independence Day) - Heliocentric:"
echo "-----------------------------------------------------------------------"
./bin/horoscope_cli --date 1776-07-04 --time 12:00:00 --lat 39.9526 --lon -75.1652 --timezone -5 --chart-style solar-system

echo ""
echo "8. Historical Example - July 4, 1776 (Independence Day) - Geocentric:"
echo "---------------------------------------------------------------------"
./bin/horoscope_cli --date 1776-07-04 --time 12:00:00 --lat 39.9526 --lon -75.1652 --timezone -5 --chart-style solar-system --perspective geocentric

echo ""
echo "==========================================================="
echo "Demo completed! Available perspectives:"
echo "- heliocentric (default): Sun-centered view"
echo "- geocentric: Earth-centered view"
echo "- mars-centric: Mars-centered view"
echo "- jupiter-centric: Jupiter-centered view"
echo "- saturn-centric: Saturn-centered view"
echo "==========================================================="
