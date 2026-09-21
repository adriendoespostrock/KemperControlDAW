#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_Standalone/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/Debug/Standalone/Kemper Control.app/Contents"
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_Standalone/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/Release/Standalone/Kemper Control.app/Contents"
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_Standalone/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/MinSizeRel/Standalone/Kemper Control.app/Contents"
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -E copy /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_Standalone/PkgInfo "/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/RelWithDebInfo/Standalone/Kemper Control.app/Contents"
fi

