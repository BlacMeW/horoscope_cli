#!/bin/bash

# Build script for Horoscope CLI
# This script provides an easy way to build the project

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] && [ ! -f "Makefile" ]; then
    print_error "No CMakeLists.txt or Makefile found. Are you in the project directory?"
    exit 1
fi

# Default values
BUILD_TYPE="Release"
USE_CMAKE=true
INSTALL=false
CLEAN=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --make)
            USE_CMAKE=false
            shift
            ;;
        --install)
            INSTALL=true
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --debug     Build in debug mode"
            echo "  --make      Use Makefile instead of CMake"
            echo "  --install   Install after building"
            echo "  --clean     Clean before building"
            echo "  --help      Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Check dependencies
print_status "Checking dependencies..."

# Check for compiler
if ! command -v g++ &> /dev/null; then
    print_error "g++ compiler not found. Please install build-essential."
    exit 1
fi

# Check for Swiss Ephemeris
if ! ldconfig -p | grep -q libswe; then
    print_warning "Swiss Ephemeris library (libswe) not found."
    print_status "Attempting to install Swiss Ephemeris..."

    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y libswe-dev
    elif command -v yum &> /dev/null; then
        sudo yum install -y swisseph-devel
    elif command -v pacman &> /dev/null; then
        sudo pacman -S swisseph
    else
        print_error "Cannot automatically install Swiss Ephemeris. Please install manually."
        print_status "On Ubuntu/Debian: sudo apt-get install libswe-dev"
        print_status "On CentOS/RHEL: sudo yum install swisseph-devel"
        print_status "On Arch: sudo pacman -S swisseph"
        exit 1
    fi
fi

print_success "Dependencies check passed."

# Clean if requested
if [ "$CLEAN" = true ]; then
    print_status "Cleaning previous build..."
    if [ "$USE_CMAKE" = true ]; then
        rm -rf build
    else
        make clean 2>/dev/null || true
    fi
fi

# Build the project
if [ "$USE_CMAKE" = true ]; then
    print_status "Building with CMake..."

    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Install cmake or use --make option."
        exit 1
    fi

    mkdir -p build
    cd build

    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
    make -j$(nproc)

    EXECUTABLE_PATH="build/horoscope_cli"
else
    print_status "Building with Make..."

    if [ "$BUILD_TYPE" = "Debug" ]; then
        make debug
    else
        make
    fi

    EXECUTABLE_PATH="bin/horoscope_cli"
fi

print_success "Build completed successfully!"

# Test the executable
if [ -f "$EXECUTABLE_PATH" ]; then
    print_status "Testing executable..."
    if ./$EXECUTABLE_PATH --version; then
        print_success "Executable test passed."
    else
        print_warning "Executable test failed, but build completed."
    fi
else
    print_error "Executable not found at expected location: $EXECUTABLE_PATH"
    exit 1
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    print_status "Installing..."
    if [ "$USE_CMAKE" = true ]; then
        cd build
        sudo make install
        cd ..
    else
        make install
    fi
    print_success "Installation completed."
fi

print_success "All done! You can run the horoscope calculator with:"
if [ "$INSTALL" = true ]; then
    echo "horoscope_cli --help"
else
    echo "./$EXECUTABLE_PATH --help"
fi
