#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr int kemperMidiChannel = 1;

    constexpr std::array<int, 8> effectCCs =
    {
        17, // A
        18, // B
        19, // C
        20, // D
        22, // X
        24, // MOD
        27, // DLY
        29  // REV
    };

    constexpr std::array<int, 4> extraCCs = { 75, 76, 77, 78 };

    constexpr std::array<int, 7> looperNRPNs =
    {
        88, // Rec / Play / Overdub
        89, // Stop
        90, // Trigger
        91, // Reverse
        92, // Half
        93, // Undo
        94  // Erase
    };

    int parameterInt(const std::atomic<float>* value)
    {
        return juce::roundToInt(value->load());
    }

    int parameterMidiValue(const std::atomic<float>* value)
    {
        return juce::jlimit(0, 127, juce::roundToInt(value->load() * 127.0f));
    }

    juce::StringArray numberChoices(int first, int last)
    {
        juce::StringArray choices;
        for (int value = first; value <= last; ++value)
            choices.add(juce::String(value));
        return choices;
    }
}

KemperControlAudioProcessor::KemperControlAudioProcessor()
    : AudioProcessor(
#if JucePlugin_IsMidiEffect
          BusesProperties()
#else
          BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    previousEffects.fill(-1);
    previousExtraButtons.fill(-1);
    previousLooperButtons.fill(-1);
}

juce::AudioProcessorValueTreeState::ParameterLayout
KemperControlAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Hosts expose these as stepped choices instead of continuously editable
    // numeric parameters. The IDs remain unchanged for existing automation.
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(juce::String("bank"), 1),
        "Bank", numberChoices(1, 124), 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(juce::String("slot"), 1),
        "Slot", numberChoices(1, 5), 0));

    const std::array<const char*, 8> effectNames =
    {
        "effectA", "effectB", "effectC", "effectD",
        "effectX", "effectMOD", "effectDLY", "effectREV"
    };

    const std::array<const char*, 8> effectLabels =
    {
        "A", "B", "C", "D", "X", "MOD", "DLY", "REV"
    };

    for (size_t i = 0; i < effectNames.size(); ++i)
        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(juce::String(effectNames[i]), 1),
            juce::String(effectLabels[i]), false));

    const std::array<const char*, 4> extraNames =
    {
        "extraI", "extraII", "extraIII", "extraIIII"
    };

    const std::array<const char*, 4> extraLabels =
    {
        "I", "II", "III", "IIII"
    };

    for (size_t i = 0; i < extraNames.size(); ++i)
        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(juce::String(extraNames[i]), 1),
            juce::String(extraLabels[i]), false));

    const std::array<const char*, 7> looperNames =
    {
        "looperRec", "looperStop", "looperTrigger", "looperReverse",
        "looperHalf", "looperUndo", "looperErase"
    };

    const std::array<const char*, 7> looperLabels =
    {
        "Rec / Play / Overdub", "Stop", "Trigger", "Reverse",
        "Half", "Undo", "Erase"
    };

    for (size_t i = 0; i < looperNames.size(); ++i)
        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(juce::String(looperNames[i]), 1),
            juce::String(looperLabels[i]), false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(juce::String("morphPedal"), 1),
        "Morph Pedal", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(juce::String("expPedal"), 1),
        "Exp Pedal", juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(juce::String("wahPedal"), 1),
        "Wah Pedal", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(juce::String("gainPedal"), 1),
        "Gain Pedal", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(juce::String("morphButton"), 1),
        "Morph Button", false));

    return layout;
}

void KemperControlAudioProcessor::prepareToPlay(double, int)
{
    resetPreviousValues();
}

void KemperControlAudioProcessor::resetPreviousValues()
{
    previousBank = -1;
    previousSlot = -1;
    previousEffects.fill(-1);
    previousExtraButtons.fill(-1);
    previousLooperButtons.fill(-1);
    previousMorphPedal = -1;
    previousExpPedal = -1;
    previousWahPedal = -1;
    previousGainPedal = -1;
    previousMorphButton = -1;
}

void KemperControlAudioProcessor::releaseResources()
{
}

bool KemperControlAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    return layouts.inputBuses.isEmpty() && layouts.outputBuses.isEmpty();
#else
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
#endif
}

void KemperControlAudioProcessor::sendCC(juce::MidiBuffer& output,
                                         int samplePosition,
                                         int controller,
                                         int value)
{
    output.addEvent(
        juce::MidiMessage::controllerEvent(
            kemperMidiChannel,
            controller,
            juce::jlimit(0, 127, value)),
        samplePosition);
}

void KemperControlAudioProcessor::sendProgramChange(juce::MidiBuffer& output,
                                                    int samplePosition,
                                                    int program)
{
    output.addEvent(
        juce::MidiMessage::programChange(
            kemperMidiChannel,
            juce::jlimit(0, 127, program)),
        samplePosition);
}

void KemperControlAudioProcessor::sendNRPN(juce::MidiBuffer& output,
                                           int samplePosition,
                                           int parameterMSB,
                                           int parameterLSB,
                                           int value)
{
    const int safeValue = juce::jlimit(0, 16383, value);

    sendCC(output, samplePosition, 99, parameterMSB);
    sendCC(output, samplePosition, 98, parameterLSB);
    sendCC(output, samplePosition, 6, safeValue >> 7);
    sendCC(output, samplePosition, 38, safeValue & 0x7f);
}

void KemperControlAudioProcessor::sendCurrentSlot(juce::MidiBuffer& output,
                                                  int samplePosition,
                                                  int slot) const
{
    for (int i = 1; i <= 5; ++i)
    {
        if (i != slot)
            sendCC(output, samplePosition, 49 + i, 0);
    }

    if (juce::isPositiveAndBelow(slot, 6))
    {
        // CC 50-54 emulate a physical rig-button press and release. A
        // subsequent press of the currently selected slot triggers Morph.
        sendCC(output, samplePosition, 49 + slot, 1);
        sendCC(output, samplePosition + 1, 49 + slot, 0);
    }
}

void KemperControlAudioProcessor::sendBankAndSlot(juce::MidiBuffer& output,
                                                  int samplePosition,
                                                  int bank,
                                                  int slot) const
{
    const int absoluteRig = (bank - 1) * 5 + 1;
    const int bankValue = (absoluteRig - 1) / 128;
    const int program = absoluteRig - bankValue * 128;

    sendCC(output, samplePosition, 32, bankValue);
    sendProgramChange(output, samplePosition + 1, program);
    sendCurrentSlot(output, samplePosition + 2, slot);
}

void KemperControlAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages)
{
    buffer.clear(); // Silent instrument output for VST3/Standalone host compatibility.

    midiMessages.clear(); // Direct output only: never echo MIDI or duplicate host output.
    const bool offline = isNonRealtime();
    directOutput.setRealtime(!offline);
    if (offline) { wasOffline = true; return; }
    const auto generation = directOutput.readyGeneration();
    if (generation == 0) return;
    const bool resend = resendRequested.exchange(false);
    if (wasOffline || generation != previousOutputGeneration || resend)
    {
        resetPreviousValues();
        previousOutputGeneration = generation;
        wasOffline = false;
    }
    juce::MidiBuffer output;

    const int eventPosition = 0;

    // AudioParameterChoice stores a zero-based index internally.
    const int bank = parameterInt(parameters.getRawParameterValue("bank")) + 1;
    const int slot = parameterInt(parameters.getRawParameterValue("slot")) + 1;

    if (bank != previousBank)
    {
        sendBankAndSlot(output, eventPosition, bank, slot);
        previousBank = bank;
        previousSlot = slot;
    }
    else if (slot != previousSlot)
    {
        sendCurrentSlot(output, eventPosition, slot);
        previousSlot = slot;
    }

    const std::array<const char*, 8> effectNames =
    {
        "effectA", "effectB", "effectC", "effectD",
        "effectX", "effectMOD", "effectDLY", "effectREV"
    };

    for (size_t i = 0; i < effectNames.size(); ++i)
    {
        const int value = parameterInt(parameters.getRawParameterValue(effectNames[i]));
        if (value != previousEffects[i])
        {
            sendCC(output, eventPosition, effectCCs[i], value != 0 ? 1 : 0);
            previousEffects[i] = value;
        }
    }

    const std::array<const char*, 4> extraNames =
    {
        "extraI", "extraII", "extraIII", "extraIIII"
    };

    for (size_t i = 0; i < extraNames.size(); ++i)
    {
        const int value = parameterInt(parameters.getRawParameterValue(extraNames[i]));
        if (value != previousExtraButtons[i])
        {
            sendCC(output, eventPosition, extraCCs[i], value != 0 ? 1 : 0);
            previousExtraButtons[i] = value;
        }
    }

    const std::array<const char*, 7> looperNames =
    {
        "looperRec", "looperStop", "looperTrigger", "looperReverse",
        "looperHalf", "looperUndo", "looperErase"
    };

    for (size_t i = 0; i < looperNames.size(); ++i)
    {
        const int value = parameterInt(parameters.getRawParameterValue(looperNames[i]));
        if (value != previousLooperButtons[i])
        {
            sendNRPN(output, eventPosition, 125, looperNRPNs[i], value != 0 ? 1 : 0);
            previousLooperButtons[i] = value;
        }
    }

    const int morphPedal = parameterMidiValue(parameters.getRawParameterValue("morphPedal"));
    if (morphPedal != previousMorphPedal)
    {
        sendCC(output, eventPosition, 11, morphPedal);
        previousMorphPedal = morphPedal;
    }

    const int expPedal = parameterMidiValue(parameters.getRawParameterValue("expPedal"));
    if (expPedal != previousExpPedal)
    {
        sendCC(output, eventPosition, 1, expPedal);
        previousExpPedal = expPedal;
    }

    const int wahPedal = parameterMidiValue(parameters.getRawParameterValue("wahPedal"));
    if (wahPedal != previousWahPedal)
    {
        sendCC(output, eventPosition, 7, wahPedal);
        previousWahPedal = wahPedal;
    }

    const int gainPedal = parameterMidiValue(parameters.getRawParameterValue("gainPedal"));
    if (gainPedal != previousGainPedal)
    {
        sendCC(output, eventPosition, 72, gainPedal);
        previousGainPedal = gainPedal;
    }

    const int morphButton = parameterInt(parameters.getRawParameterValue("morphButton"));
    if (morphButton != previousMorphButton)
    {
        // A subsequent press of the currently selected rig/slot button
        // toggles the Kemper between its base and Morph states. Both changes
        // of the UI toggle therefore act as another press of that same rig.
        if (previousMorphButton != -1)
            sendCurrentSlot(output, eventPosition + 4, slot);
        previousMorphButton = morphButton;
    }

    directOutput.enqueue(output, getSampleRate() > 0 ? getSampleRate() : 44100.0,
                         generation);
}

juce::AudioProcessorEditor* KemperControlAudioProcessor::createEditor()
{
    return new KemperControlAudioProcessorEditor(*this);
}

void KemperControlAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    const auto output = directOutput.getSettings();
    state.setProperty("directMidiId", output.id, nullptr);
    state.setProperty("directMidiName", output.name, nullptr);
    state.setProperty("directMidiChannel", output.channel, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void KemperControlAudioProcessor::setStateInformation(const void* data,
                                                      int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        if (xml->hasTagName(parameters.state.getType()))
        {
            auto state = juce::ValueTree::fromXml(*xml);
            parameters.replaceState(state);
            directOutput.configure({ state.getProperty("directMidiId").toString(),
                state.getProperty("directMidiName").toString(),
                static_cast<int>(state.getProperty("directMidiChannel", 1)) });
            requestResend();
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KemperControlAudioProcessor();
}
