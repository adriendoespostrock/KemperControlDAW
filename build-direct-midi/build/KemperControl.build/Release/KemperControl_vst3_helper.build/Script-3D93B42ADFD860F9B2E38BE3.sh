#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -GXcode -S/Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/juce_vst3_helper -B/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl -Dhelper_name=vst3_helper -Dsource_file=/Users/adriendeurveilher/Desktop/JUCE/modules/juce_audio_plugin_client/VST3/juce_VST3ManifestHelper.cpp -Dshared_defs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_defs_Debug.txt -Dshared_incs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_incs_Debug.txt -DCMAKE_CXX_COMPILER=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
  /opt/homebrew/bin/cmake --build /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -GXcode -S/Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/juce_vst3_helper -B/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl -Dhelper_name=vst3_helper -Dsource_file=/Users/adriendeurveilher/Desktop/JUCE/modules/juce_audio_plugin_client/VST3/juce_VST3ManifestHelper.cpp -Dshared_defs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_defs_Release.txt -Dshared_incs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_incs_Release.txt -DCMAKE_CXX_COMPILER=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
  /opt/homebrew/bin/cmake --build /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -GXcode -S/Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/juce_vst3_helper -B/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl -Dhelper_name=vst3_helper -Dsource_file=/Users/adriendeurveilher/Desktop/JUCE/modules/juce_audio_plugin_client/VST3/juce_VST3ManifestHelper.cpp -Dshared_defs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_defs_MinSizeRel.txt -Dshared_incs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_incs_MinSizeRel.txt -DCMAKE_CXX_COMPILER=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
  /opt/homebrew/bin/cmake --build /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi
  /opt/homebrew/bin/cmake -GXcode -S/Users/adriendeurveilher/Desktop/JUCE/extras/Build/CMake/juce_vst3_helper -B/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl -Dhelper_name=vst3_helper -Dsource_file=/Users/adriendeurveilher/Desktop/JUCE/modules/juce_audio_plugin_client/VST3/juce_VST3ManifestHelper.cpp -Dshared_defs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_defs_RelWithDebInfo.txt -Dshared_incs_file=/Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl/shared_incs_RelWithDebInfo.txt -DCMAKE_CXX_COMPILER=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
  /opt/homebrew/bin/cmake --build /Users/adriendeurveilher/Desktop/KemperControl/build-direct-midi/vst3_helpers/KemperControl
fi

