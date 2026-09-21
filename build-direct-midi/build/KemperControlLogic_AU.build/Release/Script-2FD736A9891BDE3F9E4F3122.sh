#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/JuceLibraryCode/KemperControlLogic_AU/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/Debug/AU/Kemper Control.component/Contents"
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/Debug/AU/Kemper Control.component" -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/Debug/AU/Kemper Control.component" -Ddest=/Users/adriendeurveilher/Library/Audio/Plug-Ins/Components -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/JuceLibraryCode/KemperControlLogic_AU/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/Release/AU/Kemper Control.component/Contents"
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/Release/AU/Kemper Control.component" -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/Release/AU/Kemper Control.component" -Ddest=/Users/adriendeurveilher/Library/Audio/Plug-Ins/Components -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/JuceLibraryCode/KemperControlLogic_AU/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/MinSizeRel/AU/Kemper Control.component/Contents"
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/MinSizeRel/AU/Kemper Control.component" -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/MinSizeRel/AU/Kemper Control.component" -Ddest=/Users/adriendeurveilher/Library/Audio/Plug-Ins/Components -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/JuceLibraryCode/KemperControlLogic_AU/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/RelWithDebInfo/AU/Kemper Control.component/Contents"
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/RelWithDebInfo/AU/Kemper Control.component" -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake "-Dsrc=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControlLogic_artefacts/RelWithDebInfo/AU/Kemper Control.component" -Ddest=/Users/adriendeurveilher/Library/Audio/Plug-Ins/Components -P /Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/copyDir.cmake
fi

