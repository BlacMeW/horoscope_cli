#!/bin/bash

# Enhanced Build Script for Horoscope CLI v2.0
# This script builds the enhanced version with all new features

set -e  # Exit on any error

# Color definitions for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="Horoscope CLI"
VERSION="2.0.0"
BUILD_TYPE="Release"
CMAKE_GENERATOR="Unix Makefiles"
ENABLE_TESTING=false
ENABLE_PLUGINS=true
ENABLE_AI=true
ENABLE_MULTILANG=true

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

print_header() {
    echo -e "${PURPLE}=================================================================================${NC}"
    echo -e "${PURPLE}                    $1${NC}"
    echo -e "${PURPLE}=================================================================================${NC}"
}

# Function to check system requirements
check_requirements() {
    print_header "Checking System Requirements"

    # Check for required tools
    local required_tools=("g++" "make" "cmake" "pkg-config")
    local missing_tools=()

    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            missing_tools+=("$tool")
        else
            print_success "$tool is available"
        fi
    done

    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        print_status "Please install missing tools and try again"
        exit 1
    fi

    # Check C++ compiler version
    local gcc_version=$(g++ --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
    print_status "GCC version: $gcc_version"

    # Check for Swiss Ephemeris data files
    if [ -d "data" ] && [ -f "data/seas_18.se1" ]; then
        print_success "Swiss Ephemeris data files found"
    else
        print_warning "Swiss Ephemeris data files not found in data/ directory"
        print_status "Charts will still work but with reduced accuracy for historical dates"
    fi

    echo
}

# Function to clean previous builds
clean_build() {
    print_header "Cleaning Previous Builds"

    if [ -d "build" ]; then
        print_status "Removing existing build directory..."
        rm -rf build
    fi

    if [ -d "obj" ]; then
        print_status "Removing object files..."
        rm -rf obj/*.o
    fi

    if [ -f "bin/horoscope_cli" ]; then
        print_status "Removing previous binary..."
        rm -f bin/horoscope_cli
    fi

    print_success "Clean completed"
    echo
}

# Function to create necessary directories
setup_directories() {
    print_header "Setting Up Build Environment"

    local dirs=("build" "bin" "obj" "logs")

    for dir in "${dirs[@]}"; do
        if [ ! -d "$dir" ]; then
            mkdir -p "$dir"
            print_status "Created directory: $dir"
        fi
    done

    print_success "Directory setup completed"
    echo
}

# Function to configure build with CMake
configure_cmake() {
    print_header "Configuring Build with CMake"

    cd build

    local cmake_options=(
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
        "-DCMAKE_INSTALL_PREFIX=../install"
        "-DENABLE_TESTING=$ENABLE_TESTING"
        "-DENABLE_PLUGINS=$ENABLE_PLUGINS"
        "-DENABLE_AI_FEATURES=$ENABLE_AI"
        "-DENABLE_MULTILANG=$ENABLE_MULTILANG"
        "-DCMAKE_CXX_STANDARD=17"
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
    )

    print_status "Running CMake configuration..."
    cmake "${cmake_options[@]}" .. | tee ../logs/cmake_config.log

    if [ $? -eq 0 ]; then
        print_success "CMake configuration successful"
    else
        print_error "CMake configuration failed"
        cd ..
        exit 1
    fi

    cd ..
    echo
}

# Function to build with make
build_project() {
    print_header "Building Project"

    cd build

    # Determine number of CPU cores for parallel building
    local num_cores=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

    print_status "Building with $num_cores parallel jobs..."
    make -j$num_cores | tee ../logs/build.log

    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        cd ..
        exit 1
    fi

    cd ..
    echo
}

# Alternative build with traditional Makefile
build_traditional() {
    print_header "Building with Traditional Makefile"

    # Determine number of CPU cores for parallel building
    local num_cores=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

    print_status "Building with $num_cores parallel jobs..."
    make -j$num_cores 2>&1 | tee logs/build.log

    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi

    echo
}

# Function to run tests
run_tests() {
    if [ "$ENABLE_TESTING" = true ]; then
        print_header "Running Tests"

        if [ -f "test.sh" ]; then
            print_status "Running test suite..."
            ./test.sh | tee logs/test.log

            if [ $? -eq 0 ]; then
                print_success "All tests passed"
            else
                print_warning "Some tests failed - check logs/test.log"
            fi
        else
            print_warning "Test script not found"
        fi

        echo
    fi
}

# Function to verify build
verify_build() {
    print_header "Verifying Build"

    if [ -f "bin/horoscope_cli" ]; then
        print_success "Binary created successfully: bin/horoscope_cli"

        # Check if binary is executable
        if [ -x "bin/horoscope_cli" ]; then
            print_success "Binary is executable"

            # Get binary size
            local size=$(du -h bin/horoscope_cli | cut -f1)
            print_status "Binary size: $size"

            # Test basic functionality
            print_status "Testing basic functionality..."
            if ./bin/horoscope_cli --version &>/dev/null; then
                print_success "Version check passed"
            else
                print_warning "Version check failed - binary may not be fully functional"
            fi

            if ./bin/horoscope_cli --help &>/dev/null; then
                print_success "Help display works"
            else
                print_warning "Help display failed"
            fi

        else
            print_error "Binary exists but is not executable"
            exit 1
        fi
    else
        print_error "Binary not found after build"
        exit 1
    fi

    echo
}

# Function to display build summary
display_summary() {
    print_header "Build Summary"

    echo -e "${CYAN}Project:${NC} $PROJECT_NAME"
    echo -e "${CYAN}Version:${NC} $VERSION"
    echo -e "${CYAN}Build Type:${NC} $BUILD_TYPE"
    echo -e "${CYAN}Features:${NC}"

    if [ "$ENABLE_PLUGINS" = true ]; then
        echo -e "  ${GREEN}✓${NC} Plugin System"
    fi

    if [ "$ENABLE_AI" = true ]; then
        echo -e "  ${GREEN}✓${NC} AI Interpretation"
    fi

    if [ "$ENABLE_MULTILANG" = true ]; then
        echo -e "  ${GREEN}✓${NC} Multi-language Support"
    fi

    if [ "$ENABLE_TESTING" = true ]; then
        echo -e "  ${GREEN}✓${NC} Testing Enabled"
    fi

    echo
    echo -e "${CYAN}Build completed at:${NC} $(date)"
    echo -e "${CYAN}Binary location:${NC} bin/horoscope_cli"
    echo -e "${CYAN}Log files:${NC} logs/"

    if [ -f "bin/horoscope_cli" ]; then
        local size=$(du -h bin/horoscope_cli | cut -f1)
        echo -e "${CYAN}Binary size:${NC} $size"
    fi

    echo
    print_success "Build process completed successfully!"
    echo
    echo -e "${YELLOW}Quick start:${NC}"
    echo -e "  ./bin/horoscope_cli --help"
    echo -e "  ./bin/horoscope_cli --interactive"
    echo -e "  ./bin/horoscope_cli --date 1990-01-15 --time 14:30:00 --lat 40.7128 --lon -74.0060"
    echo
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -c, --clean         Clean previous builds before building"
    echo "  -t, --test          Enable testing"
    echo "  -d, --debug         Build in debug mode"
    echo "  -r, --release       Build in release mode (default)"
    echo "  --no-plugins        Disable plugin system"
    echo "  --no-ai             Disable AI features"
    echo "  --no-multilang      Disable multi-language support"
    echo "  --traditional       Use traditional Makefile instead of CMake"
    echo
    echo "Examples:"
    echo "  $0                  # Standard release build"
    echo "  $0 --clean --test   # Clean build with testing"
    echo "  $0 --debug          # Debug build"
    echo "  $0 --traditional    # Build with traditional Makefile"
}

# Main function
main() {
    local clean_first=false
    local use_traditional=false

    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -c|--clean)
                clean_first=true
                shift
                ;;
            -t|--test)
                ENABLE_TESTING=true
                shift
                ;;
            -d|--debug)
                BUILD_TYPE="Debug"
                shift
                ;;
            -r|--release)
                BUILD_TYPE="Release"
                shift
                ;;
            --no-plugins)
                ENABLE_PLUGINS=false
                shift
                ;;
            --no-ai)
                ENABLE_AI=false
                shift
                ;;
            --no-multilang)
                ENABLE_MULTILANG=false
                shift
                ;;
            --traditional)
                use_traditional=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done

    # Print banner
    clear
    print_header "$PROJECT_NAME v$VERSION - Enhanced Build System"
    echo -e "${CYAN}Building enhanced astrology software with professional features${NC}"
    echo

    # Execute build steps
    check_requirements

    if [ "$clean_first" = true ]; then
        clean_build
    fi

    setup_directories

    if [ "$use_traditional" = true ]; then
        build_traditional
    else
        configure_cmake
        build_project
    fi

    run_tests
    verify_build
    display_summary
}

# Script entry point
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
