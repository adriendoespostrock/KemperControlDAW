#pragma once

#include <JuceHeader.h>
#include "DirectMidiOutput.h"

class KemperControlAudioProcessor : public juce::AudioProcessor
{
public:
    KemperControlAudioProcessor();
    ~KemperControlAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Kemper Control"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return JucePlugin_IsMidiEffect != 0; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    DirectMidiOutput directOutput;
    void requestResend() noexcept { resendRequested.store(true); }

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void sendBankAndSlot(juce::MidiBuffer& output, int samplePosition,
                         int bank, int slot) const;
    void sendCurrentSlot(juce::MidiBuffer& output, int samplePosition,
                         int slot) const;

    static void sendCC(juce::MidiBuffer& output, int samplePosition,
                       int controller, int value);
    static void sendProgramChange(juce::MidiBuffer& output, int samplePosition,
                                  int program);
    static void sendNRPN(juce::MidiBuffer& output, int samplePosition,
                         int parameterMSB, int parameterLSB, int value);

    void resetPreviousValues();
    std::atomic<bool> resendRequested{false};
    std::uint64_t previousOutputGeneration = 0;
    bool wasOffline = false;
    int previousBank = -1;
    int previousSlot = -1;

    std::array<int, 8> previousEffects{};
    std::array<int, 4> previousExtraButtons{};
    std::array<int, 7> previousLooperButtons{};

    int previousMorphPedal = -1;
    int previousExpPedal = -1;
    int previousWahPedal = -1;
    int previousGainPedal = -1;
    int previousMorphButton = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KemperControlAudioProcessor)
};

