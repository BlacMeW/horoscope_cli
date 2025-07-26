#!/bin/bash

# Demo script for Hindu Monthly Calendar functionality
# Shows various features including BC era support

echo "=== Hindu Monthly Calendar Demo ==="
echo "This demo shows the Hindu Monthly Calendar functionality"
echo "Unlike Myanmar calendar, Hindu calendar supports BC dates"
echo ""

# Check if the binary exists
if [ ! -f "./bin/horoscope_cli" ]; then
    echo "Error: horoscope_cli binary not found. Please build first with 'make' or './build.sh'"
    exit 1
fi

echo "📅 Demo 1: Current month Hindu calendar (Traditional format)"
echo "Command: ./bin/horoscope_cli --hindu-monthly $(date +%Y-%m)"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly $(date +%Y-%m)
echo ""

echo "📅 Demo 2: Detailed Hindu calendar with Muhurta timings"
echo "Command: ./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format detailed --hindu-monthly-muhurta"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format detailed --hindu-monthly-muhurta
echo ""

echo "📅 Demo 3: Panchanga-focused view (shows all 5 elements)"
echo "Command: ./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format panchanga"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format panchanga
echo ""

echo "📅 Demo 4: Festival-focused calendar"
echo "Command: ./bin/horoscope_cli --hindu-monthly 2025-12 --hindu-monthly-format festivals"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly 2025-12 --hindu-monthly-format festivals
echo ""

echo "📅 Demo 5: BC Era Example - March 500 BC (Buddha's time)"
echo "Command: ./bin/horoscope_cli --hindu-monthly 500BC-03 --hindu-monthly-format traditional"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly 500BC-03 --hindu-monthly-format traditional
echo ""

echo "📅 Demo 6: BC Era using astronomical year format (-499 = 500 BC)"
echo "Command: ./bin/horoscope_cli --hindu-monthly -0499-03 --hindu-monthly-format panchanga"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly -0499-03 --hindu-monthly-format panchanga
echo ""

echo "📅 Demo 7: Ancient Era - 1000 BC (Vedic period)"
echo "Command: ./bin/horoscope_cli --hindu-monthly 1000BC-01 --hindu-monthly-format astronomical"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly 1000BC-01 --hindu-monthly-format astronomical
echo ""

echo "📅 Demo 8: Compact format for quick reference"
echo "Command: ./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format compact"
echo "------------------------------------------------------------"
./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format compact
echo ""

echo "📅 Demo 9: Export to CSV format"
echo "Command: ./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format csv"
echo "------------------------------------------------------------"
echo "CSV output (first 10 lines):"
./bin/horoscope_cli --hindu-monthly $(date +%Y-%m) --hindu-monthly-format csv | head -10
echo ""

echo "📅 Demo 10: Short form usage examples"
echo "Command: ./bin/horoscope_cli -hm $(date +%Y-%m)"
echo "------------------------------------------------------------"
./bin/horoscope_cli -hm $(date +%Y-%m)
echo ""

echo "=== Hindu Monthly Calendar Demo Complete ==="
echo ""
echo "Key Features Demonstrated:"
echo "✓ Traditional Panchanga calculations"
echo "✓ BC era support (500BC, 1000BC formats)"
echo "✓ Astronomical year format (-0499 for 500 BC)"
echo "✓ Multiple display formats (traditional, detailed, panchanga, etc.)"
echo "✓ Muhurta timing calculations"
echo "✓ Festival and special event highlighting"
echo "✓ CSV export for data analysis"
echo "✓ Short form commands (-hm)"
echo ""
echo "This Hindu calendar works with any date from 3000 BC onwards,"
echo "making it suitable for historical and astronomical research."
echo ""
echo "For more options, run: ./bin/horoscope_cli --help"
