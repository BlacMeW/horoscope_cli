#!/bin/bash

# Solar System ASCII Art Demonstration Script
# This script demonstrates the different solar system visualization features

echo "========================================================="
echo "Solar System ASCII Art Demo for Horoscope CLI"
echo "========================================================="

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
echo "2. Solar System with Planet Positions for January 15, 1990:"
echo "------------------------------------------------------------"
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5 --chart-style solar-system

echo ""
echo "3. Solar System with Planet Positions for July 4, 1776 (Independence Day):"
echo "---------------------------------------------------------------------------"
./bin/horoscope_cli --date 1776-07-04 --time 12:00:00 --lat 39.9526 --lon -75.1652 --timezone -5 --chart-style solar-system

echo ""
echo "========================================================="
echo "Demo completed!"
echo "========================================================="
