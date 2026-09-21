#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo VST3 /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_VST3/PkgInfo
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo VST3 /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_VST3/PkgInfo
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo VST3 /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_VST3/PkgInfo
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo VST3 /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/KemperControl_artefacts/JuceLibraryCode/KemperControl_VST3/PkgInfo
fi

