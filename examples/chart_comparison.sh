#!/bin/bash

# Quick demo of Eastern chart drawing
# Shows the same birth chart in all three styles

if [ $# -ne 5 ]; then
    echo "Usage: $0 <date> <time> <latitude> <longitude> <timezone>"
    echo "Example: $0 1990-01-15 14:30:00 40.7128 -74.0060 -5"
    exit 1
fi

DATE="$1"
TIME="$2"
LAT="$3"
LON="$4"
TZ="$5"

EXECUTABLE="./bin/horoscope_cli"
if [ ! -f "$EXECUTABLE" ]; then
    EXECUTABLE="./build/horoscope_cli"
fi

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: horoscope_cli executable not found. Build the project first."
    exit 1
fi

echo "=== CHART COMPARISON FOR $DATE $TIME ==="
echo "Location: $LAT°, $LON°, UTC$TZ"
echo "=========================================="

echo ""
echo ">>> WESTERN STYLE CHART <<<"
echo "---------------------------"
$EXECUTABLE --date "$DATE" --time "$TIME" --lat "$LAT" --lon "$LON" --timezone "$TZ" --chart-style western

echo ""
echo ">>> NORTH INDIAN VEDIC CHART <<<"
echo "--------------------------------"
$EXECUTABLE --date "$DATE" --time "$TIME" --lat "$LAT" --lon "$LON" --timezone "$TZ" --chart-style north-indian

echo ""
echo ">>> SOUTH INDIAN VEDIC CHART <<<"
echo "--------------------------------"
$EXECUTABLE --date "$DATE" --time "$TIME" --lat "$LAT" --lon "$LON" --timezone "$TZ" --chart-style south-indian

echo ""
echo ">>> EAST INDIAN VEDIC CHART <<<"
echo "-------------------------------"
$EXECUTABLE --date "$DATE" --time "$TIME" --lat "$LAT" --lon "$LON" --timezone "$TZ" --chart-style east-indian

echo ""
echo "=== NOTES ==="
echo "============"
echo "All four charts show the same astronomical data."
echo "The difference is in the visual presentation:"
echo "- Western: Detailed text format with aspects"
echo "- North Indian: Diamond-shaped house layout"
echo "- South Indian: Square format with fixed house positions"
echo "- East Indian: Rectangular 3x4 grid layout (Bengali style)"
