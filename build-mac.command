#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
kemper_juce_dir="${1:-$PWD/JUCE}"
if ! command -v cmake >/dev/null 2>&1; then
    echo "CMake is required. Install it, then run this script from Terminal."
    exit 1
fi
if [[ ! -f "$kemper_juce_dir/CMakeLists.txt" ]]; then
    echo 'Usage: bash build-mac.command /absolute/path/to/JUCE'
    exit 1
fi
cmake -S . -B build-direct-midi -G Xcode \
    -DJUCE_DIR="$kemper_juce_dir" \
    '-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64'
cmake --build build-direct-midi --config Release --target \
    KemperControl_VST3 KemperControl_Standalone KemperControlLogic_AU --parallel 4
echo "Build finished. VST3 and AU were copied to the plug-in folders."
