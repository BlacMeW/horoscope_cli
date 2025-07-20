# 📚 Calendar Search Examples Documentation

This directory contains comprehensive examples demonstrating the calendar search capabilities of the Horoscope CLI application.

## 📄 Available Example Files

### 1. `calendar_search_quick_reference.sh` 📖
**Purpose**: Quick demonstration and complete parameter reference
**What it does**:
- Shows basic Hindu and Myanmar calendar searches
- Provides complete parameter reference tables
- Includes practical usage tips and common locations
- Perfect for getting started quickly

**Run it**: `./calendar_search_quick_reference.sh`

### 2. `hindu_calendar_search_examples.sh` 🇮🇳
**Purpose**: Comprehensive Hindu calendar search examples
**What it does**:
- 20 detailed examples covering all Hindu calendar search features
- Year, month, tithi, paksha, weekday, nakshatra, yoga, karana searches
- Multi-criteria combinations and near matching examples
- Festival planning, seasonal searches, and complex scenarios

**Run it**: `./hindu_calendar_search_examples.sh`

### 3. `myanmar_calendar_search_examples.sh` 🇲🇲
**Purpose**: Comprehensive Myanmar calendar search examples
**What it does**:
- 24 detailed examples covering all Myanmar calendar search features
- Year, month, moon phase, weekday, fortnight day searches
- Buddhist sabbath, astrological day (yatyaza, pyathada, thamanyo) searches
- Festival planning, religious observance, and wedding planning examples

**Run it**: `./myanmar_calendar_search_examples.sh`

### 4. `combined_calendar_search_examples.sh` 🌏
**Purpose**: Comparative analysis of both calendar systems
**What it does**:
- 10 side-by-side comparisons of Hindu vs Myanmar calendar searches
- Full moon, new moon, Sunday, and religious day comparisons
- Cross-cultural event planning examples
- Shows differences and similarities between calendar systems

**Run it**: `./combined_calendar_search_examples.sh`

## 🚀 Quick Start

1. **Compile the project first**:
   ```bash
   cd /DATA/FlutterCodes/githubCodes/horoscope_cli
   make clean && make
   ```

2. **Start with the quick reference**:
   ```bash
   cd examples
   ./calendar_search_quick_reference.sh
   ```

3. **Explore specific calendar systems**:
   ```bash
   # Hindu calendar examples
   ./hindu_calendar_search_examples.sh

   # Myanmar calendar examples
   ./myanmar_calendar_search_examples.sh

   # Combined comparison
   ./combined_calendar_search_examples.sh
   ```

## 🔍 Parameter Summary

### Hindu Calendar Search Parameters
- **Base**: `--hindu-search FROM TO`
- **Year**: `--search-year YEAR`, `--search-year-range START END`
- **Month**: `--search-month N`, `--search-month-range START END`
- **Lunar Day**: `--search-tithi N`, `--search-tithi-range START END`
- **Moon Phase**: `--search-paksha N` (0=Waxing, 1=Waning)
- **Weekday**: `--search-weekday N` (0=Sunday...6=Saturday)
- **Nakshatra**: `--search-nakshatra N`, `--search-nakshatra-range START END`
- **Yoga**: `--search-yoga N`, `--search-yoga-range START END`
- **Karana**: `--search-karana N`, `--search-karana-range START END`
- **Matching**: `--search-exact`, `--search-near N`

### Myanmar Calendar Search Parameters
- **Base**: `--myanmar-search FROM TO`
- **Year**: `--myanmar-search-year YEAR`, `--myanmar-search-year-range START END`
- **Month**: `--myanmar-search-month N`, `--myanmar-search-month-range START END`
- **Moon Phase**: `--myanmar-search-moon-phase N`, `--myanmar-search-moon-phase-range START END`
- **Weekday**: `--myanmar-search-weekday N` (0=Saturday...6=Friday)
- **Fortnight**: `--myanmar-search-fortnight-day N`, `--myanmar-search-fortnight-day-range START END`
- **Religious**: `--myanmar-search-sabbath`, `--myanmar-search-sabbath-eve`
- **Astrological**: `--myanmar-search-yatyaza`, `--myanmar-search-pyathada`, `--myanmar-search-thamanyo`
- **Matching**: `--myanmar-search-exact`, `--myanmar-search-near N`

## 📍 Location Requirements

All searches require location parameters for accurate calculations:
```bash
--lat LATITUDE --lon LONGITUDE --timezone OFFSET
```

**Common Locations**:
- **Delhi**: `--lat 28.61 --lon 77.21 --timezone "+05:30"`
- **Mumbai**: `--lat 19.08 --lon 72.88 --timezone "+05:30"`
- **Yangon**: `--lat 16.84 --lon 96.17 --timezone "+06:30"`
- **Bangkok**: `--lat 13.76 --lon 100.50 --timezone "+07:00"`

## 💡 Usage Tips

1. **Start Simple**: Begin with basic searches like weekday or month
2. **Combine Criteria**: Use multiple parameters for complex requirements
3. **Use Near Matching**: Add tolerance for flexible date planning
4. **Compare Systems**: Run both Hindu and Myanmar searches for cultural events
5. **Check Location**: Ensure correct latitude/longitude for accuracy

## 🎯 Common Use Cases

- **Festival Planning**: Search for specific tithis or moon phases
- **Wedding Planning**: Find auspicious days avoiding inauspicious periods
- **Religious Observance**: Locate sabbath days and religious festivals
- **Business Planning**: Avoid inauspicious days, prefer auspicious times
- **Cultural Research**: Compare date systems across Hindu and Myanmar traditions

## 📊 Example Output

Each search returns:
- **Match Score**: Relevance score for each result
- **Calendar Details**: Full panchanga/Myanmar calendar information
- **Astrological Info**: Planetary positions, moon phases, observances
- **Festival Data**: Religious festivals and special events
- **Muhurta Assessment**: Auspiciousness evaluation

## 🔧 Troubleshooting

- **No Results**: Try wider date ranges or near matching with tolerance
- **Unknown Arguments**: Check parameter spelling (--search-* vs --hindu-search-*)
- **Compilation Issues**: Ensure Swiss Ephemeris is properly linked
- **Permission Errors**: Make scripts executable with `chmod +x *.sh`

---

**Created**: July 2025
**Version**: Complete calendar search implementation
**Features**: Hindu Panchanga & Myanmar calendar search with 50+ examples
