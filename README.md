# Horoscope CLI

A C++ command-line tool for calculating horoscopes using Swiss Ephemeris.

## Features

- Calculate planetary positions for any given date, time, and location
- Generate birth charts with house calculations
- Calculate aspects between planets
- Support for various house systems (Placidus, Koch, Equal House, etc.)
- Accurate astronomical calculations using Swiss Ephemeris

## Dependencies

- Swiss Ephemeris library (libswe-dev)
- CMake 3.10 or higher
- C++17 compatible compiler

## Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libswe-dev cmake build-essential
```

### Building
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./horoscope_cli --date "1990-01-15" --time "14:30:00" --lat 40.7128 --lon -74.0060 --timezone -5
```

### Parameters

- `--date`: Birth date in YYYY-MM-DD format
- `--time`: Birth time in HH:MM:SS format (24-hour)
- `--lat`: Latitude in decimal degrees
- `--lon`: Longitude in decimal degrees
- `--timezone`: Timezone offset from UTC in hours
- `--house-system`: House system (P=Placidus, K=Koch, E=Equal, W=Whole Sign)

### Example Output

```
Birth Chart for 1990-01-15 14:30:00 UTC-5
Location: 40.7128°N, 74.0060°W

=== PLANETARY POSITIONS ===
Sun:        25°42' Capricorn (House 5)
Moon:       12°18' Scorpio (House 3)
Mercury:    05°33' Aquarius (House 6)
Venus:      18°27' Sagittarius (House 4)
Mars:       10°15' Capricorn (House 5)
Jupiter:    08°42' Cancer (House 11)
Saturn:     22°55' Capricorn (House 5)
Uranus:     08°38' Capricorn (House 5)
Neptune:    13°29' Capricorn (House 5)
Pluto:      17°32' Scorpio (House 3)

=== HOUSE CUSPS ===
1st House: 15°23' Virgo
2nd House: 10°45' Libra
3rd House: 08°12' Scorpio
...

=== MAJOR ASPECTS ===
Sun conjunct Mars (orb: 2°27')
Moon square Venus (orb: 1°15')
...
```

## License

MIT License
