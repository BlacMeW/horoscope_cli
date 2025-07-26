# 🌟 Horoscope CLI - Professional Astrology Toolkit ⭐

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/BlacMeW/horoscope_cli)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Swiss Ephemeris](https://img.shields.io/badge/powered%20by-Swiss%20Ephemeris-orange.svg)](https://www.astro.com/swisseph/)
[![AI Powered](https://img.shields.io/badge/AI-Powered%20Interpretations-purple.svg)](#ai-interpretation)
[![Multi-Language](https://img.shields.io/badge/languages-26%20supported-red.svg)](#multi-language-support)

> **The most comprehensive command-line astrology application ever created!** 🚀
>
> Generate birth charts, analyze planetary patterns, predict future events, and receive AI-powered interpretations in 26+ languages with professional-grade accuracy using the Swiss Ephemeris.

## ✨ What's New in Version 2.0

### 🤖 **AI-Powered Interpretations**
- **Intelligent Chart Analysis**: Advanced AI interprets your birth chart with psychological depth
- **Personality Profiling**: Detailed personality insights based on planetary patterns
- **Career Guidance**: AI-generated career advice and professional development paths
- **Relationship Analysis**: Comprehensive compatibility and relationship dynamics
- **Health Insights**: Wellness guidance and health pattern recognition
- **Spiritual Guidance**: Life purpose analysis and spiritual development paths

### 🌍 **Multi-Language Support (26 Languages)**
- **Complete Localization**: Full translation of all astrological terms
- **Native Scripts**: Support for Devanagari, Arabic, Chinese, Japanese scripts
- **Cultural Adaptation**: Region-specific calendar systems and cultural preferences
- **Supported Languages**: English, Hindi, Sanskrit, Spanish, French, German, Italian, Portuguese, Russian, Chinese (Simplified/Traditional), Japanese, Korean, Arabic, Thai, Burmese, Tamil, Telugu, Bengali, Gujarati, Marathi, Kannada, Malayalam, Punjabi, Urdu, Nepali

### 🔮 **Advanced Astrological Analysis**
- **Midpoint Analysis**: Planetary midpoint calculations and interpretations
- **Harmonic Charts**: 4th, 5th, 7th, 9th harmonic analysis
- **Arabic Parts**: Complete Arabic Parts/Lots calculations
- **Fixed Stars**: Major fixed star influences and interpretations
- **Asteroid Analysis**: Ceres, Pallas, Juno, Vesta analysis
- **Vertex/Anti-Vertex**: Karmic and destiny point analysis

### 🎯 **Predictive Astrology Suite**
- **Vedic Dasha Systems**: Vimshottari, Ashtottari dasha calculations
- **Secondary Progressions**: Progressed chart analysis for any date
- **Advanced Transits**: Comprehensive transit analysis with timing
- **Planetary Returns**: Solar, Lunar, and planetary return charts
- **Electional Astrology**: Find auspicious timing for important events
- **Yearly/Monthly Forecasts**: Detailed predictive reports

### 🖥️ **Interactive Mode**
- **Guided Wizards**: Step-by-step chart generation
- **Professional Interface**: Advanced mode for professional astrologers
- **Quick Analysis**: Rapid chart interpretation for beginners
- **Session Management**: Save and resume analysis sessions

### 💕 **Relationship Analysis**
- **Synastry Analysis**: Complete compatibility analysis between two charts
- **Composite Charts**: Relationship chart synthesis
- **Compatibility Scoring**: Numerical compatibility ratings
- **Relationship Timing**: Best periods for relationships

### 🏥💰 **Specialized Astrology**
- **Financial Astrology**: Stock market and investment timing
- **Medical Astrology**: Health insights and Ayurvedic analysis
- **Mundane Astrology**: Political and economic predictions
- **Relocation Astrology**: How moving affects your chart

### 🔌 **Plugin System**
- **Extensible Architecture**: Add custom interpretation engines
- **Plugin Marketplace**: Download community-created plugins
- **Custom Calculations**: Extend with new astrological techniques
- **Integration APIs**: Connect with external services

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/BlacMeW/horoscope_cli.git
cd horoscope_cli

# Build the application
make

# Or use CMake
mkdir build && cd build
cmake .. && make
```

### Basic Usage

```bash
# Generate a basic birth chart
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5

# AI-powered interpretation
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 40.7128 --lon -74.0060 --timezone -5 \
                    --ai-interpretation

# Interactive mode
./bin/horoscope_cli --interactive

# Multi-language chart in Hindi
./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 \
                    --lat 28.6139 --lon 77.2090 --timezone 5.5 \
                    --language hindi --native-script
```

## 🌟 Core Features

### 📊 **Chart Generation**
- **Multiple Chart Types**: Western, Vedic (North/South/East Indian), Solar System views
- **House Systems**: Placidus, Koch, Equal, Whole Sign, Campanus, Regiomontanus
- **Zodiac Systems**: Tropical and Sidereal with 21+ Ayanamsa types
- **Historical Range**: 6000 BC to 7000 AD with full accuracy

### 🌞 **Eclipse & Conjunction Analysis**
- **Eclipse Predictions**: Solar and lunar eclipses with global visibility
- **Planetary Conjunctions**: Multi-planet alignments with customizable orbs
- **Graha Yuddha**: Vedic planetary war calculations
- **Historical Analysis**: Research eclipses and conjunctions across millennia

### 🇮🇳 **Hindu Calendar System (Panchanga)**
- **Complete Panchanga**: Tithi, Vara, Nakshatra, Yoga, Karana
- **Festival Calendar**: 100+ Hindu festivals with specific dates
- **Muhurta Calculations**: Auspicious and inauspicious timings
- **Ekadashi Detection**: All Ekadashi varieties with proper names
- **Search Functionality**: Find specific combinations across date ranges

### 🇲🇲 **Myanmar Calendar System**
- **Traditional Calendar**: Myanmar Era and Sasana Era calculations
- **Astrological Days**: Sabbath, Yatyaza, Pyathada, Thamanyo
- **Moon Phases**: Traditional Myanmar lunar calendar
- **Cultural Events**: Myanmar festivals and observances

### 🧮 **KP System (Krishnamurti Paddhati)**
- **5-Level Sub-Lords**: Complete KP subdivision system
- **Nakshatra System**: 27 nakshatras with pada calculations
- **Transition Tracking**: Planetary transitions through KP levels
- **Cuspal Sub-Lords**: House cusp analysis

### 📈 **Ephemeris Tables**
- **High Precision**: Arc-second accuracy using Swiss Ephemeris
- **Flexible Intervals**: Daily, weekly, monthly calculations
- **Multiple Formats**: Table, CSV, JSON output
- **Historical Data**: Access to millennia of astronomical data

## 🎨 Advanced Features

### 🔮 **Predictive Analysis**
- **Life Phase Analysis**: Identify major life phases and transitions
- **Event Prediction**: Predict major life events with timing
- **Remedial Measures**: Vedic remedies including gems, mantras, yantras
- **Timing Analysis**: Find best times for important decisions

### 📊 **Professional Analysis**
- **Pattern Recognition**: Automatically detect chart patterns
- **Strength Analysis**: Planetary and house strength calculations
- **Yogas & Doshas**: Vedic yoga and dosha identification
- **Statistical Analysis**: Batch processing for research

### 🎯 **Specialized Applications**
- **Business Astrology**: Corporate chart analysis and timing
- **Sports Astrology**: Athletic performance analysis
- **Weather Prediction**: Astrological weather forecasting
- **Agricultural Timing**: Planting and harvesting guidance

## 📱 Output Formats

### 📄 **Multiple Output Formats**
- **Text**: Beautiful ASCII art charts with Unicode symbols
- **JSON**: Complete data structure for programming integration
- **CSV**: Spreadsheet-compatible tabular data
- **HTML**: Web-ready formatted reports
- **PDF**: Professional printable documents
- **XML**: Structured data exchange format

### 🎨 **Visualization Options**
- **ASCII Art Charts**: Detailed visual chart representations
- **Color Output**: Syntax highlighting and colored elements
- **Theme Support**: Dark, light, classic, and custom themes
- **Unicode Symbols**: Traditional astrological symbols
- **Progress Indicators**: Real-time calculation progress

## 🌍 Global Support

### 🗺️ **Location Features**
- **City Database**: 1000+ major world cities with coordinates
- **Automatic Timezone**: Intelligent timezone detection
- **DST Handling**: Daylight Saving Time calculations
- **Historical Zones**: Historical timezone data

### 📅 **Calendar Systems**
- **Gregorian**: Standard Western calendar
- **Hindu**: Vikram Samvat, Shaka Samvat
- **Myanmar**: Traditional Myanmar calendar
- **Chinese**: Traditional Chinese calendar elements
- **Islamic**: Hijri calendar conversions

## 🔧 Technical Excellence

### ⚡ **Performance**
- **Swiss Ephemeris**: NASA-quality astronomical calculations
- **Optimized Code**: Lightning-fast calculations
- **Memory Efficient**: Minimal resource usage
- **Batch Processing**: Handle thousands of charts efficiently

### 🛡️ **Reliability**
- **Extensive Testing**: Comprehensive test suite
- **Error Handling**: Graceful error recovery
- **Data Validation**: Input validation and sanitization
- **Backup Systems**: Automatic data backup

### 🔌 **Extensibility**
- **Plugin Architecture**: Add custom functionality
- **API Integration**: Connect with external services
- **Scripting Support**: Automation and batch operations
- **Configuration**: Extensive customization options

## 📚 Documentation

- **[User Manual](docs/MANUAL.md)**: Complete usage guide
- **[Installation Guide](docs/INSTALL.md)**: Detailed setup instructions
- **[Feature Documentation](docs/FEATURES.md)**: In-depth feature explanations
- **[API Reference](docs/API.md)**: Programming interface documentation
- **[Plugin Development](docs/PLUGINS.md)**: Create custom extensions
- **[Examples](examples/)**: Practical usage examples

## 🤝 Community & Support

### 💬 **Get Help**
- **GitHub Issues**: Bug reports and feature requests
- **Discussions**: Community Q&A and sharing
- **Documentation**: Comprehensive guides and examples
- **Professional Support**: Enterprise support available

### 🎯 **For Developers**
- **Open Source**: MIT license for maximum flexibility
- **Contributions Welcome**: Pull requests and improvements
- **Plugin System**: Extend functionality easily
- **API Access**: Full programmatic access

### 🏆 **Professional Use**
- **Research**: Academic and professional research tool
- **Consultation**: Professional astrology practice
- **Education**: Teaching and learning astrology
- **Integration**: Embed in larger applications

## 🌟 What Users Say

> *"The most comprehensive astrology software I've ever used. The AI interpretations are incredibly accurate and insightful."* - **Professional Astrologer**

> *"Finally, a command-line tool that handles everything from basic charts to advanced research. The multi-language support is fantastic."* - **Astrology Researcher**

> *"The plugin system allowed me to add custom calculations for my research. Excellent architecture!"* - **Software Developer**

## 🚀 Future Roadmap

### 🔮 **Upcoming Features**
- **3D Chart Visualization**: Interactive 3D chart displays
- **Voice Interface**: Voice-controlled chart generation
- **Mobile App**: Companion mobile application
- **Cloud Sync**: Cross-device synchronization
- **Machine Learning**: Enhanced AI predictions
- **Augmented Reality**: AR chart overlays

### 🌍 **Expansion Plans**
- **More Languages**: Additional language support
- **Regional Variations**: Local astrological traditions
- **Historical Systems**: Ancient astrological methods
- **Modern Techniques**: Latest astrological research

## 📞 Contact & Support

- **Website**: [horoscope-cli.org](https://horoscope-cli.org)
- **GitHub**: [github.com/BlacMeW/horoscope_cli](https://github.com/BlacMeW/horoscope_cli)
- **Email**: support@horoscope-cli.org
- **Discord**: [Join our community](https://discord.gg/horoscope-cli)
- **Twitter**: [@HoroscopeCLI](https://twitter.com/HoroscopeCLI)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Swiss Ephemeris**: For providing NASA-quality astronomical calculations
- **Unicode Consortium**: For astrological symbol standardization
- **Astrology Community**: For feedback and feature suggestions
- **Open Source Contributors**: For code contributions and improvements

---

<div align="center">

**⭐ Star this repository if you find it useful! ⭐**

**Made with ❤️ for the astrology community**

[⬆ Back to Top](#-horoscope-cli---professional-astrology-toolkit-)

</div>
