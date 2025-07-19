#!/bin/bash

# East Indian Chart Style Examples
# Demonstrates the Bengali/Oriya astrology chart format

EXECUTABLE="./bin/horoscope_cli"
if [ ! -f "$EXECUTABLE" ]; then
    EXECUTABLE="./build/horoscope_cli"
fi

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: horoscope_cli executable not found. Build the project first."
    exit 1
fi

echo "=== EAST INDIAN CHART STYLE EXAMPLES ==="
echo "========================================"
echo ""
echo "The East Indian style represents the traditional Bengali and Oriya"
echo "astrology chart format, featuring a rectangular 3x4 grid layout."
echo ""

# Famous Bengali personalities for demonstration

# Rabindranath Tagore
echo "1. RABINDRANATH TAGORE (Nobel Prize Winner)"
echo "==========================================="
echo "Date: May 7, 1861, 2:31 AM, Kolkata, India"
echo ""
$EXECUTABLE --date "1861-05-07" --time "02:31:00" --lat 22.5726 --lon 88.3639 --timezone 5.5 --chart-style east-indian

echo ""
echo "2. SWAMI VIVEKANANDA (Spiritual Leader)"
echo "======================================"
echo "Date: January 12, 1863, 6:33 AM, Kolkata, India"
echo ""
$EXECUTABLE --date "1863-01-12" --time "06:33:00" --lat 22.5726 --lon 88.3639 --timezone 5.5 --chart-style east-indian

echo ""
echo "3. SUBHAS CHANDRA BOSE (Freedom Fighter)"
echo "========================================"
echo "Date: January 23, 1897, 12:10 PM, Cuttack, Orissa, India"
echo ""
$EXECUTABLE --date "1897-01-23" --time "12:10:00" --lat 20.2961 --lon 85.8245 --timezone 5.5 --chart-style east-indian

echo ""
echo "4. COMPARISON: SAME BIRTH DATA IN ALL CHART STYLES"
echo "=================================================="
echo "Birth Data: January 1, 2000, 12:00 PM, Kolkata, India"
echo ""

SAMPLE_DATE="2000-01-01"
SAMPLE_TIME="12:00:00"
SAMPLE_LAT="22.5726"
SAMPLE_LON="88.3639"
SAMPLE_TZ="5.5"

echo "North Indian Style:"
echo "------------------"
$EXECUTABLE --date "$SAMPLE_DATE" --time "$SAMPLE_TIME" --lat "$SAMPLE_LAT" --lon "$SAMPLE_LON" --timezone "$SAMPLE_TZ" --chart-style north-indian

echo ""
echo "South Indian Style:"
echo "------------------"
$EXECUTABLE --date "$SAMPLE_DATE" --time "$SAMPLE_TIME" --lat "$SAMPLE_LAT" --lon "$SAMPLE_LON" --timezone "$SAMPLE_TZ" --chart-style south-indian

echo ""
echo "East Indian Style:"
echo "-----------------"
$EXECUTABLE --date "$SAMPLE_DATE" --time "$SAMPLE_TIME" --lat "$SAMPLE_LAT" --lon "$SAMPLE_LON" --timezone "$SAMPLE_TZ" --chart-style east-indian

echo ""
echo "=== EAST INDIAN CHART FEATURES ==="
echo "=================================="
echo "• Rectangular 3x4 grid layout"
echo "• House 1 (Ascendant) in top-right position"
echo "• Houses arranged clockwise from Ascendant"
echo "• Traditional Bengali and Oriya astrology format"
echo "• Systematic grid arrangement for easy reading"
echo "• Compatible with all house systems"
echo ""
echo "Planet Symbols:"
echo "Su=Sun, Mo=Moon, Me=Mercury, Ve=Venus, Ma=Mars"
echo "Ju=Jupiter, Sa=Saturn, Ra=Rahu, Ke=Ketu"
echo "Ur=Uranus, Ne=Neptune, Pl=Pluto"
