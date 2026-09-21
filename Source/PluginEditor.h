#pragma once

#include "PluginProcessor.h"

class KemperControlLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    KemperControlLookAndFeel();

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&,
                              bool isMouseOver, bool isButtonDown) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&, bool isMouseOver,
                        bool isButtonDown) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool isMouseOver,
                          bool isButtonDown) override;
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;
    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox&) override;
    void positionComboBoxText(juce::ComboBox&, juce::Label&) override;

    static juce::Colour green() noexcept { return juce::Colour(0xffa4f45a); }
    static juce::Colour greenDim() noexcept { return juce::Colour(0xff4f8d35); }
    static juce::Colour panel() noexcept { return juce::Colour(0xff1b2225); }
    static juce::Colour surface() noexcept { return juce::Colour(0xff252e32); }
    static juce::Colour text() noexcept { return juce::Colour(0xffd8e0e2); }
    static juce::Colour muted() noexcept { return juce::Colour(0xff8b9a9f); }
};

class KemperControlAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit KemperControlAudioProcessorEditor(KemperControlAudioProcessor&);
    ~KemperControlAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    KemperControlAudioProcessor& kemperProcessor;
    KemperControlLookAndFeel lookAndFeel;

    void refreshMidiPorts();
    void timerCallback() override;
    void configureToggle(juce::ToggleButton&, const juce::String& label,
                         const juce::String& tooltip);
    void configureDial(juce::Slider&, const juce::String& tooltip);

    juce::Array<juce::MidiDeviceInfo> midiPorts;
    juce::ComboBox midiOutputBox, midiChannelBox;
    juce::TextButton aboutButton{"ABOUT"};
    juce::TextButton resendButton{"SEND STATE"};
    juce::Label midiStatus, bankReadout;
    int portRefreshTicks = 0;

    juce::Slider bankSlider;
    std::unique_ptr<SliderAttachment> bankAttachment;

    std::array<juce::ToggleButton, 5> rigButtons;
    std::array<std::unique_ptr<ButtonAttachment>, 5> rigAttachments;

    std::array<juce::ToggleButton, 8> effectButtons;
    std::array<std::unique_ptr<ButtonAttachment>, 8> effectAttachments;

    std::array<juce::ToggleButton, 4> extraButtons;
    std::array<std::unique_ptr<ButtonAttachment>, 4> extraAttachments;

    std::array<juce::ToggleButton, 7> looperButtons;
    std::array<std::unique_ptr<ButtonAttachment>, 7> looperAttachments;

    juce::Slider morphPedal, expPedal, wahPedal, gainPedal;
    juce::ToggleButton morphButton;
    std::unique_ptr<SliderAttachment> morphPedalAttachment;
    std::unique_ptr<SliderAttachment> expPedalAttachment;
    std::unique_ptr<SliderAttachment> wahPedalAttachment;
    std::unique_ptr<SliderAttachment> gainPedalAttachment;
    std::unique_ptr<ButtonAttachment> morphButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KemperControlAudioProcessorEditor)
};
