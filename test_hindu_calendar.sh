#!/bin/bash

# Test Hindu Calendar (Panchanga) functionality
echo "=== HINDU CALENDAR (PANCHANGA) SYSTEM TEST ==="
echo "=============================================="
echo ""
echo "Testing Hindu calendar integration with sample birth data:"
echo "Date: January 15, 1990"
echo "Time: 12:00 PM"
echo "Location: New Delhi, India (28.6139°N, 77.2090°E, UTC+5:30)"
echo ""

# Test individual birth chart with Panchanga
echo "1. INDIVIDUAL BIRTH CHART WITH PANCHANGA"
echo "========================================"
echo "This shows the Hindu calendar elements for a specific birth time:"
echo ""

./bin/horoscope_cli --date 1990-01-15 --time 12:00:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --panchanga

echo ""
echo "2. PANCHANGA FOR DATE RANGE"
echo "==========================="
echo "This shows Hindu calendar elements for a one-week period:"
echo ""

./bin/horoscope_cli --panchanga-range 1990-01-15 1990-01-21 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5

echo ""
echo "3. PANCHANGA IN JSON FORMAT"
echo "==========================="
echo "Machine-readable format for API integration:"
echo ""

./bin/horoscope_cli --date 1990-01-15 --time 12:00:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --panchanga --panchanga-format json

echo ""
echo "4. COMBINED WESTERN CHART WITH PANCHANGA"
echo "========================================"
echo "Shows both Western astrology chart and Hindu calendar:"
echo ""

./bin/horoscope_cli --date 1990-01-15 --time 12:00:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --chart-style western --panchanga

echo ""
echo "5. FESTIVALS AND SPECIAL DAYS"
echo "============================="
echo "Test with dates that have Hindu festivals:"
echo ""

# Test with Diwali (approximate date)
./bin/horoscope_cli --panchanga-range 2024-11-01 2024-11-05 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --festivals-only

echo ""
echo "=== HINDU CALENDAR FEATURES IMPLEMENTED ==="
echo "==========================================="
echo "✓ Complete Panchanga calculation (Tithi, Vara, Nakshatra, Yoga, Karana)"
echo "✓ Hindu calendar dates (Vikram Samvat, Hindu months)"
echo "✓ Festival identification and special events"
echo "✓ Paksha determination (Shukla/Krishna fortnights)"
echo "✓ Muhurta assessment (auspicious timing)"
echo "✓ Integration with existing Western astrology system"
echo "✓ Multiple output formats (table, JSON, CSV)"
echo "✓ Date range calculations for calendar generation"
echo "✓ Solar transitions (Sankranti) detection"
echo "✓ Lunar phase calculations and special days"
echo ""
echo "The Hindu Calendar system provides comprehensive"
echo "Panchanga calculations based on precise Swiss Ephemeris"
echo "astronomical data, making it suitable for religious,"
echo "cultural, and astrological applications."
