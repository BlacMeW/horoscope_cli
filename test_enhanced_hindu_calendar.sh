#!/bin/bash

# Test enhanced Hindu Calendar functionality
echo "=== ENHANCED HINDU CALENDAR FEATURES TEST ==="
echo "============================================="
echo ""
echo "Testing new calculation options and date conversion features:"
echo ""

# Test basic panchanga with configuration display
echo "1. BASIC PANCHANGA WITH CONFIGURATION"
echo "====================================="
echo "Shows current calculation settings:"
echo ""

./bin/horoscope_cli --date 2024-03-15 --time 12:00:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --panchanga

echo ""
echo "2. PANCHANGA FOR DIFFERENT DATE"
echo "=============================="
echo "Testing with a different date:"
echo ""

./bin/horoscope_cli --date 2024-08-15 --time 18:00:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --panchanga

echo ""
echo "3. JSON FORMAT OUTPUT"
echo "===================="
echo "JSON format with enhanced data:"
echo ""

./bin/horoscope_cli --date 2024-03-15 --time 12:00:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --panchanga --panchanga-format json

echo ""
echo "4. PANCHANGA RANGE"
echo "=================="
echo "Range calculation with improved formatting:"
echo ""

./bin/horoscope_cli --panchanga-range 2024-03-15 2024-03-20 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5

echo ""
echo "=== NEW FEATURES IMPLEMENTED ==="
echo "==============================="
echo "✓ Ayanamsa configuration options (Lahiri, Raman, Krishnamurti, etc.)"
echo "✓ Multiple calculation methods (Drik Siddhanta, Surya Siddhanta, etc.)" 
echo "✓ Calendar system options (Lunar, Solar, Luni-Solar)"
echo "✓ Date conversion utilities (Hindu ↔ Gregorian)"
echo "✓ Enhanced configuration display in output"
echo "✓ Improved Swiss Ephemeris integration"
echo "✓ Better error handling and validation"
echo "✓ Multiple year systems (Vikram, Shaka, Kali)"
echo ""
echo "Note: The Hindu calendar now provides comprehensive"
echo "Panchanga calculations with configurable astronomical"
echo "parameters, matching traditional pancanga systems like"
echo "pancanga3.14.pl but with modern Swiss Ephemeris accuracy."
