#!/bin/bash

# Example script demonstrating Eastern/Vedic chart drawing features
# This script shows various chart styles and their differences

EXECUTABLE="./bin/horoscope_cli"
if [ ! -f "$EXECUTABLE" ]; then
    EXECUTABLE="./build/horoscope_cli"
fi

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: horoscope_cli executable not found. Build the project first."
    exit 1
fi

echo "=== EASTERN CHART DRAWING EXAMPLES ==="
echo "======================================"

# Example birth data (Mahatma Gandhi)
DATE="1869-10-02"
TIME="07:45:00"
LAT="21.6416"
LON="69.6293"
TIMEZONE="5.5"

echo ""
echo "Birth Data: October 2, 1869, 7:45 AM, Porbandar, India"
echo "======================================================="

echo ""
echo "1. WESTERN STYLE CHART (Traditional)"
echo "------------------------------------"
$EXECUTABLE --date "$DATE" --time "$TIME" --lat "$LAT" --lon "$LON" --timezone "$TIMEZONE" --chart-style western

echo ""
echo "2. NORTH INDIAN VEDIC CHART"
echo "---------------------------"
$EXECUTABLE --date "$DATE" --time "$TIME" --lat "$LAT" --lon "$LON" --timezone "$TIMEZONE" --chart-style north-indian

echo ""
echo "3. SOUTH INDIAN VEDIC CHART"
echo "---------------------------"
$EXECUTABLE --date "$DATE" --time "$TIME" --lat "$LAT" --lon "$LON" --timezone "$TIMEZONE" --chart-style south-indian

echo ""
echo "=== COMPARING DIFFERENT BIRTH CHARTS ==="
echo "========================================"

# Example 2: Albert Einstein
echo ""
echo "Albert Einstein: March 14, 1879, 11:30 AM, Ulm, Germany"
echo "========================================================"
echo ""
echo "North Indian Style:"
$EXECUTABLE --date "1879-03-14" --time "11:30:00" --lat 48.4011 --lon 9.9876 --timezone 1 --chart-style north-indian

# Example 3: Steve Jobs
echo ""
echo "Steve Jobs: February 24, 1955, 7:15 PM, San Francisco, CA"
echo "========================================================="
echo ""
echo "South Indian Style:"
$EXECUTABLE --date "1955-02-24" --time "19:15:00" --lat 37.7749 --lon -122.4194 --timezone -8 --chart-style south-indian

echo ""
echo "=== CHART STYLE COMPARISON ==="
echo "============================="
echo ""
echo "Same birth data shown in different chart styles:"
echo "Date: January 1, 2000, 12:00 PM, New Delhi, India"
echo ""

SAMPLE_DATE="2000-01-01"
SAMPLE_TIME="12:00:00"
SAMPLE_LAT="28.6139"
SAMPLE_LON="77.2090"
SAMPLE_TZ="5.5"

echo "Western Style:"
echo "-------------"
$EXECUTABLE --date "$SAMPLE_DATE" --time "$SAMPLE_TIME" --lat "$SAMPLE_LAT" --lon "$SAMPLE_LON" --timezone "$SAMPLE_TZ" --chart-style western | head -25

echo ""
echo "North Indian Style:"
echo "------------------"
$EXECUTABLE --date "$SAMPLE_DATE" --time "$SAMPLE_TIME" --lat "$SAMPLE_LAT" --lon "$SAMPLE_LON" --timezone "$SAMPLE_TZ" --chart-style north-indian

echo ""
echo "South Indian Style:"
echo "------------------"
$EXECUTABLE --date "$SAMPLE_DATE" --time "$SAMPLE_TIME" --lat "$SAMPLE_LAT" --lon "$SAMPLE_LON" --timezone "$SAMPLE_TZ" --chart-style south-indian

echo ""
echo "=== NOTES ==="
echo "============"
echo "- Western charts show detailed planetary positions and aspects"
echo "- North Indian charts use a diamond-shaped format"
echo "- South Indian charts use a square format with fixed house positions"
echo "- All charts show the same astronomical data, just in different visual formats"
echo "- Planet symbols: Su=Sun, Mo=Moon, Me=Mercury, Ve=Venus, Ma=Mars, Ju=Jupiter, Sa=Saturn"
echo "- Ra=Rahu (North Node), Ke=Ketu (South Node), Ur=Uranus, Ne=Neptune, Pl=Pluto"
