# Installation Instructions for Horoscope CLI

## System Requirements

- Linux operating system (Ubuntu, Debian, CentOS, Arch, etc.)
- C++17 compatible compiler (GCC 7+ or Clang 5+)
- Swiss Ephemeris library
- CMake 3.10+ (optional, can use Makefile)

## Dependencies Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libswe-dev
```

### CentOS/RHEL/Fedora
```bash
# For CentOS/RHEL
sudo yum install gcc-c++ cmake swisseph-devel

# For Fedora
sudo dnf install gcc-c++ cmake swisseph-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel cmake swisseph
```

### Manual Swiss Ephemeris Installation
If Swiss Ephemeris is not available in your package manager:

1. Download from https://www.astro.com/swisseph/
2. Extract and compile:
```bash
tar -xzf swisseph_unix.tar.gz
cd swisseph/src
make
sudo make install
```

## Building the Project

### Option 1: Using the build script (Recommended)
```bash
./build.sh
```

For debug build:
```bash
./build.sh --debug
```

To install system-wide:
```bash
./build.sh --install
```

### Option 2: Using CMake manually
```bash
mkdir build
cd build
cmake ..
make
```

### Option 3: Using Makefile
```bash
make
```

## Installation

### System-wide installation
```bash
# Using CMake
cd build
sudo make install

# Using Makefile
sudo make install
```

### Local installation
The executable will be created in:
- `build/horoscope_cli` (CMake)
- `bin/horoscope_cli` (Makefile)

## Testing

Run the test suite:
```bash
./test.sh
```

## Usage

```bash
horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060 --timezone -5
```

For full usage information:
```bash
horoscope_cli --help
```

## Troubleshooting

### Swiss Ephemeris not found
If you get errors about Swiss Ephemeris:
1. Install the development package: `libswe-dev` or `swisseph-devel`
2. Set the ephemeris path: `--ephe-path /usr/share/swisseph`

### Compiler errors
Make sure you have a C++17 compatible compiler:
```bash
g++ --version  # Should be 7.0 or higher
```

### Permission errors
If you get permission errors during installation:
```bash
sudo ./build.sh --install
```
