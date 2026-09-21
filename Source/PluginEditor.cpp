#include "PluginEditor.h"

namespace
{
    const auto bg = juce::Colour(0xff101517);
    const auto line = juce::Colour(0xff334044);

    void drawPanel(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour(KemperControlLookAndFeel::panel());
        g.fillRoundedRectangle(bounds, 10.0f);
        g.setColour(line);
        g.drawRoundedRectangle(bounds.reduced(0.5f), 10.0f, 1.0f);
    }

    void drawSectionTitle(juce::Graphics& g, const juce::String& title,
                          int x, int y, int width)
    {
        g.setColour(KemperControlLookAndFeel::muted());
        g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
        g.drawText(title.toUpperCase(), x, y, width, 20,
                   juce::Justification::left, false);
        // Keep this compatible with older JUCE versions. Section titles use a
        // fixed bold font and uppercase text.
        const auto textWidth = static_cast<float>(title.toUpperCase().length()) * 9.0f;
        g.setColour(line);
        g.drawLine(static_cast<float>(x + textWidth + 16), y + 10.0f,
                   static_cast<float>(x + width), y + 10.0f, 1.0f);
    }
}

KemperControlLookAndFeel::KemperControlLookAndFeel()
{
    setColour(juce::ToggleButton::textColourId, text());
    setColour(juce::TextButton::textColourOffId, text());
    setColour(juce::TextButton::textColourOnId, juce::Colour(0xff0c130d));
    setColour(juce::ComboBox::backgroundColourId, surface());
    setColour(juce::ComboBox::textColourId, text());
    setColour(juce::Label::textColourId, text());
}

void KemperControlLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                                     juce::Button& button,
                                                     const juce::Colour&, bool isMouseOver,
                                                     bool isButtonDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    const bool on = button.getToggleState();
    auto fill = on ? green().withAlpha(isButtonDown ? 0.82f : 0.70f) : surface();
    if (isButtonDown) fill = fill.brighter(0.10f);
    else if (isMouseOver) fill = fill.brighter(0.08f);
    g.setColour(fill);
    g.fillRoundedRectangle(bounds, 7.0f);
    g.setColour(on ? green() : line.brighter(0.25f));
    g.drawRoundedRectangle(bounds, 7.0f, on ? 1.6f : 1.0f);
}

void KemperControlLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                                bool, bool)
{
    g.setColour(button.getToggleState() ? juce::Colour(0xff0c130d) : text());
    g.setFont(juce::FontOptions(button.getHeight() > 38 ? 17.0f : 12.0f)
                  .withStyle("Bold"));
    g.drawText(button.getButtonText(), button.getLocalBounds(),
               juce::Justification::centred, false);
}

void KemperControlLookAndFeel::drawToggleButton(juce::Graphics& g,
                                                  juce::ToggleButton& button,
                                                  bool isMouseOver, bool isButtonDown)
{
    drawButtonBackground(g, button, {}, isMouseOver, isButtonDown);
    g.setColour(button.getToggleState() ? juce::Colour(0xff0c130d) : text());
    g.setFont(juce::FontOptions(button.getHeight() > 38 ? 17.0f : 12.0f).withStyle("Bold"));
    g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, false);
    if (button.getToggleState())
    {
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        g.fillEllipse(static_cast<float>(button.getWidth() / 2 - 3), 7.0f, 6.0f, 6.0f);
    }
}

void KemperControlLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width,
                                                  int height, float sliderPosProportional,
                                                  float rotaryStartAngle, float rotaryEndAngle,
                                                  juce::Slider& slider)
{
    const auto area = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                             static_cast<float>(width), static_cast<float>(height));
    const auto diameter = juce::jmin(area.getWidth(), area.getHeight()) - 16.0f;
    const auto centre = area.getCentre();
    const auto radius = diameter * 0.5f;
    const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    g.setColour(juce::Colour(0xff0d1113));
    g.fillEllipse(centre.x - radius, centre.y - radius, diameter, diameter);
    g.setColour(line.brighter(0.30f));
    g.drawEllipse(centre.x - radius, centre.y - radius, diameter, diameter, 1.0f);

    juce::Path arc;
    arc.addCentredArc(centre.x, centre.y, radius - 1.0f, radius - 1.0f,
                      0.0f, rotaryStartAngle, angle, true);
    g.setColour(green());
    g.strokePath(arc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved,
                                           juce::PathStrokeType::rounded));
    g.setColour(muted());
    for (int i = 0; i < 15; ++i)
    {
        const auto tick = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * i / 14.0f;
        const auto outer = juce::Point<float> { centre.x + (radius + 6.0f) * std::cos(tick), centre.y + (radius + 6.0f) * std::sin(tick) };
        const auto inner = juce::Point<float> { centre.x + (radius + (i == 0 || i == 14 ? 1.0f : 4.0f)) * std::cos(tick), centre.y + (radius + (i == 0 || i == 14 ? 1.0f : 4.0f)) * std::sin(tick) };
        g.drawLine(inner.x, inner.y, outer.x, outer.y, i % 7 == 0 ? 1.6f : 1.0f);
    }
    g.setColour(green());
    const auto pointer = juce::Point<float> { centre.x + (radius - 9.0f) * std::cos(angle), centre.y + (radius - 9.0f) * std::sin(angle) };
    g.drawLine(centre.x, centre.y, pointer.x, pointer.y, 3.0f);
    juce::ignoreUnused(slider);
}

void KemperControlLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                             bool isButtonDown, int, int, int, int,
                                             juce::ComboBox& box)
{
    g.setColour(isButtonDown ? surface().brighter(0.1f) : surface());
    g.fillRoundedRectangle(0.5f, 0.5f, static_cast<float>(width - 1), static_cast<float>(height - 1), 6.0f);
    g.setColour(box.hasKeyboardFocus(false) ? green() : line.brighter(0.2f));
    g.drawRoundedRectangle(0.5f, 0.5f, static_cast<float>(width - 1), static_cast<float>(height - 1), 6.0f, 1.0f);
    g.setColour(green());
    juce::Path arrow;
    arrow.addTriangle(static_cast<float>(width - 18), height * 0.42f,
                      static_cast<float>(width - 8), height * 0.42f,
                      static_cast<float>(width - 13), height * 0.64f);
    g.fillPath(arrow);
}

void KemperControlLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setFont(juce::FontOptions(13.0f));
    label.setColour(juce::Label::textColourId, text());
    label.setJustificationType(juce::Justification::centredLeft);
    label.setBounds(10, 0, box.getWidth() - 28, box.getHeight());
}

KemperControlAudioProcessorEditor::KemperControlAudioProcessorEditor(
    KemperControlAudioProcessor& p)
    : AudioProcessorEditor(&p), kemperProcessor(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(1000, 800);
    setResizable(true, true);
    setResizeLimits(860, 700, 1400, 1050);

    bankReadout.setColour(juce::Label::textColourId, KemperControlLookAndFeel::green());
    bankReadout.setFont(juce::FontOptions(25.0f).withStyle("Bold"));
    bankReadout.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bankReadout);

    addAndMakeVisible(midiOutputBox);
    addAndMakeVisible(midiChannelBox);
    addAndMakeVisible(aboutButton);
    addAndMakeVisible(resendButton);
    addAndMakeVisible(midiStatus);
    aboutButton.setTooltip("À propos de Kemper Control");
    resendButton.setTooltip("Renvoyer les valeurs courantes au Kemper");
    midiStatus.setColour(juce::Label::textColourId, KemperControlLookAndFeel::muted());

    aboutButton.onClick = []
    {
        const juce::String youtubeUrl = "https://www.youtube.com/@WhenWavesCollide";
        juce::AlertWindow::showAsync(
            juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::InfoIcon)
                .withTitle("Kemper Control")
                .withMessage("Kemper Control — Player Edition\n\n"
                             "Développé par Adrien Deurveilher,\n"
                             "guitariste de When Waves Collide.\n\n"
                             "When Waves Collide sur YouTube :\n" + youtubeUrl)
                .withButton("OPEN YOUTUBE")
                .withButton("CLOSE"),
            [youtubeUrl] (int result)
            {
                if (result == 1)
                    juce::URL(youtubeUrl).launchInDefaultBrowser();
            });
    };

    for (int ch = 1; ch <= 16; ++ch) midiChannelBox.addItem(juce::String(ch), ch);
    refreshMidiPorts();
    midiOutputBox.onChange = [this]
    {
        auto settings = kemperProcessor.directOutput.getSettings();
        const auto index = midiOutputBox.getSelectedId() - 2;
        settings.id = juce::isPositiveAndBelow(index, midiPorts.size()) ? midiPorts[index].identifier : juce::String();
        settings.name = juce::isPositiveAndBelow(index, midiPorts.size()) ? midiPorts[index].name : juce::String();
        kemperProcessor.directOutput.configure(settings);
        kemperProcessor.updateHostDisplay(juce::AudioProcessor::ChangeDetails{}.withNonParameterStateChanged(true));
    };
    midiChannelBox.onChange = [this]
    {
        auto settings = kemperProcessor.directOutput.getSettings();
        settings.channel = midiChannelBox.getSelectedId();
        kemperProcessor.directOutput.configure(settings);
        kemperProcessor.updateHostDisplay(juce::AudioProcessor::ChangeDetails{}.withNonParameterStateChanged(true));
    };
    resendButton.onClick = [this] { kemperProcessor.requestResend(); };

    bankSlider.setSliderStyle(juce::Slider::IncDecButtons);
    bankSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 76, 32);
    bankSlider.setRange(1.0, 124.0, 1.0);
    bankSlider.setNumDecimalPlacesToDisplay(0);
    bankSlider.setTooltip("Banque 1 à 124");
    addAndMakeVisible(bankSlider);
    bankAttachment = std::make_unique<SliderAttachment>(kemperProcessor.parameters, "bank", bankSlider);
    // AudioParameterChoice uses zero-based indices internally, while the
    // Kemper and the user-facing control use Bank 1..124.
    bankSlider.textFromValueFunction = [] (double value)
    {
        return juce::String(juce::roundToInt(static_cast<float>(value)) + 1);
    };
    bankSlider.valueFromTextFunction = [] (const juce::String& text)
    {
        return static_cast<double>(juce::jlimit(0, 123, text.getIntValue() - 1));
    };

    const std::array<juce::String, 5> rigLabels = { "1", "2", "3", "4", "5" };
    const std::array<juce::String, 8> effectLabels = { "A", "B", "C", "D", "X", "MOD", "DLY", "REV" };
    const std::array<juce::String, 8> effectIDs = { "effectA", "effectB", "effectC", "effectD", "effectX", "effectMOD", "effectDLY", "effectREV" };
    for (size_t i = 0; i < rigButtons.size(); ++i)
    {
        configureToggle(rigButtons[i], rigLabels[i], "Sélectionner le slot " + rigLabels[i]);
        rigButtons[i].setClickingTogglesState(false);
        rigButtons[i].onClick = [this, i]
        {
            if (auto* parameter = kemperProcessor.parameters.getParameter("slot"))
                parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(i)));
        };
    }
    for (size_t i = 0; i < effectButtons.size(); ++i)
    {
        configureToggle(effectButtons[i], effectLabels[i], "Activer ou désactiver le slot " + effectLabels[i]);
        effectAttachments[i] = std::make_unique<ButtonAttachment>(kemperProcessor.parameters, effectIDs[i], effectButtons[i]);
    }

    const std::array<juce::String, 4> extraLabels = { "I", "II", "III", "IIII" };
    const std::array<juce::String, 4> extraIDs = { "extraI", "extraII", "extraIII", "extraIIII" };
    for (size_t i = 0; i < extraButtons.size(); ++i)
    {
        configureToggle(extraButtons[i], extraLabels[i], "Commande Kemper " + extraLabels[i]);
        extraAttachments[i] = std::make_unique<ButtonAttachment>(kemperProcessor.parameters, extraIDs[i], extraButtons[i]);
    }

    const std::array<juce::String, 7> looperLabels = { "REC / PLAY", "STOP", "TRIGGER", "REVERSE", "HALF", "UNDO", "ERASE" };
    const std::array<juce::String, 7> looperIDs = { "looperRec", "looperStop", "looperTrigger", "looperReverse", "looperHalf", "looperUndo", "looperErase" };
    for (size_t i = 0; i < looperButtons.size(); ++i)
    {
        configureToggle(looperButtons[i], looperLabels[i], "Looper : " + looperLabels[i]);
        looperAttachments[i] = std::make_unique<ButtonAttachment>(kemperProcessor.parameters, looperIDs[i], looperButtons[i]);
    }

    configureDial(morphPedal, "Morph Pedal"); configureDial(expPedal, "Expression Pedal");
    configureDial(wahPedal, "Wah Pedal"); configureDial(gainPedal, "Gain Pedal");
    addAndMakeVisible(morphPedal); addAndMakeVisible(expPedal); addAndMakeVisible(wahPedal); addAndMakeVisible(gainPedal);
    morphPedalAttachment = std::make_unique<SliderAttachment>(kemperProcessor.parameters, "morphPedal", morphPedal);
    expPedalAttachment = std::make_unique<SliderAttachment>(kemperProcessor.parameters, "expPedal", expPedal);
    wahPedalAttachment = std::make_unique<SliderAttachment>(kemperProcessor.parameters, "wahPedal", wahPedal);
    gainPedalAttachment = std::make_unique<SliderAttachment>(kemperProcessor.parameters, "gainPedal", gainPedal);
    configureToggle(morphButton, "M", "Morph button");
    addAndMakeVisible(morphButton);
    morphButtonAttachment = std::make_unique<ButtonAttachment>(kemperProcessor.parameters, "morphButton", morphButton);

    startTimerHz(8);
}

KemperControlAudioProcessorEditor::~KemperControlAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void KemperControlAudioProcessorEditor::configureToggle(juce::ToggleButton& button,
                                                         const juce::String& label,
                                                         const juce::String& tooltip)
{
    button.setButtonText(label);
    button.setClickingTogglesState(true);
    button.setTooltip(tooltip);
    addAndMakeVisible(button);
}

void KemperControlAudioProcessorEditor::configureDial(juce::Slider& slider,
                                                       const juce::String& tooltip)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange(0.0, 1.0, 0.001);
    slider.setTooltip(tooltip);
    addAndMakeVisible(slider);
}

void KemperControlAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    g.setColour(KemperControlLookAndFeel::green());
    g.setFont(juce::FontOptions(20.0f).withStyle("Bold"));
    g.drawText("KEMPER CONTROL", 28, 18, 300, 28, juce::Justification::left, false);
    g.setColour(KemperControlLookAndFeel::muted());
    g.setFont(juce::FontOptions(11.0f).withStyle("Bold"));
    g.drawText("PLAYER EDITION", 330, 23, 150, 20, juce::Justification::left, false);
    g.setColour(KemperControlLookAndFeel::green());
    g.fillEllipse(static_cast<float>(getWidth() - 186), 28.0f, 8.0f, 8.0f);
    g.setColour(KemperControlLookAndFeel::muted());
    g.drawText("MIDI OUT", getWidth() - 170, 21, 90, 22, juce::Justification::left, false);
    g.drawText("DIRECT", getWidth() - 78, 21, 58, 22, juce::Justification::right, false);

    drawPanel(g, { 20.0f, 62.0f, static_cast<float>(getWidth() - 40), 190.0f });
    drawSectionTitle(g, "RIG SELECT", 42, 78, getWidth() - 84);
    g.setColour(KemperControlLookAndFeel::muted());
    g.setFont(juce::FontOptions(11.0f).withStyle("Bold"));
    g.drawText("BANK", 78, 111, 80, 18, juce::Justification::centred, false);
    g.drawText("MORPH SWITCH", getWidth() - 220, 111, 160, 18, juce::Justification::centred, false);
    g.drawText("COMMAND SELECTED", getWidth() / 2 - 100, 158, 200, 18, juce::Justification::centred, false);

    drawPanel(g, { 20.0f, 264.0f, static_cast<float>(getWidth() - 40), 104.0f });
    drawSectionTitle(g, "EFFECT SLOTS", 42, 279, getWidth() - 84);
    for (int i = 0; i < 7; ++i)
    {
        const auto x1 = 120.0f + static_cast<float>(i) * ((getWidth() - 240.0f) / 7.0f);
        g.setColour(line);
        g.drawLine(x1, 335.0f, x1 + 18.0f, 335.0f, 1.0f);
    }

    drawPanel(g, { 20.0f, 380.0f, static_cast<float>(getWidth() - 40), 178.0f });
    drawSectionTitle(g, "PEDALS", 42, 395, 520);
    drawSectionTitle(g, "EXTRA COMMANDS", 610, 395, getWidth() - 650);
    g.setColour(KemperControlLookAndFeel::muted());
    g.setFont(juce::FontOptions(11.0f).withStyle("Bold"));
    const std::array<juce::String, 4> pedalLabels = { "MORPH", "EXP", "WAH", "GAIN" };
    for (int i = 0; i < 4; ++i)
        g.drawText(pedalLabels[static_cast<size_t>(i)], 55 + i * 132, 413, 105, 18, juce::Justification::centred, false);

    drawPanel(g, { 20.0f, 570.0f, static_cast<float>(getWidth() - 40), 95.0f });
    drawSectionTitle(g, "LOOPER", 42, 583, getWidth() - 84);

    g.setColour(line);
    g.drawLine(20.0f, 680.0f, static_cast<float>(getWidth() - 20), 680.0f, 1.0f);
    g.setColour(KemperControlLookAndFeel::muted());
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("MIDI OUTPUT", 28, 693, 110, 18, juce::Justification::left, false);
    g.drawText("CHANNEL", 560, 693, 80, 18, juce::Justification::left, false);
}

void KemperControlAudioProcessorEditor::resized()
{
    const auto w = getWidth();
    bankSlider.setBounds(72, 127, 130, 35);
    bankReadout.setBounds(w / 2 - 210, 112, 420, 46);

    const auto rigWidth = (w - 140) / 5;
    for (int i = 0; i < 5; ++i)
        rigButtons[static_cast<size_t>(i)].setBounds(50 + i * (rigWidth + 12), 193, rigWidth, 42);

    const auto effectGap = 8;
    const auto effectMargin = 35;
    const auto effectWidth = (w - 2 * effectMargin - 7 * effectGap) / 8;
    for (int i = 0; i < 8; ++i)
        effectButtons[static_cast<size_t>(i)].setBounds(effectMargin + i * (effectWidth + effectGap), 304, effectWidth, 47);

    const auto knobWidth = 106;
    morphPedal.setBounds(52, 437, knobWidth, 98);
    expPedal.setBounds(184, 437, knobWidth, 98);
    wahPedal.setBounds(316, 437, knobWidth, 98);
    gainPedal.setBounds(448, 437, knobWidth, 98);
    morphButton.setBounds(w - 190, 127, 135, 42);

    const auto extraX = w / 2 + 120;
    const auto extraGap = 8;
    const auto extraWidth = juce::jmax(48, (w - extraX - 45 - 3 * extraGap) / 4);
    for (int i = 0; i < 4; ++i)
        extraButtons[static_cast<size_t>(i)].setBounds(extraX + i * (extraWidth + extraGap), 454, extraWidth, 38);
    const auto looperMargin = 35;
    const auto looperGap = 6;
    const auto looperWidth = juce::jmax(65, (w - 2 * looperMargin - 6 * looperGap) / 7);
    for (int i = 0; i < 7; ++i)
        looperButtons[static_cast<size_t>(i)].setBounds(looperMargin + i * (looperWidth + looperGap), 613, looperWidth, 30);

    midiOutputBox.setBounds(28, 713, 440, 34);
    midiChannelBox.setBounds(650, 713, 82, 34);
    aboutButton.setBounds(500, 18, 72, 28);
    resendButton.setBounds(w - 180, 713, 150, 34);
    midiStatus.setBounds(28, 747, w - 350, 20);
}

void KemperControlAudioProcessorEditor::refreshMidiPorts()
{
    const auto settings = kemperProcessor.directOutput.getSettings();
    midiPorts = juce::MidiOutput::getAvailableDevices();
    midiOutputBox.clear(juce::dontSendNotification);
    midiOutputBox.addItem("NO MIDI OUTPUT", 1);
    int selected = 1;
    for (int i = 0; i < midiPorts.size(); ++i)
    {
        midiOutputBox.addItem(midiPorts[i].name, i + 2);
        if (midiPorts[i].identifier == settings.id) selected = i + 2;
    }
    if (selected == 1 && settings.id.isNotEmpty())
    {
        selected = midiPorts.size() + 2;
        midiOutputBox.addItem("UNAVAILABLE: " + settings.name, selected);
    }
    midiOutputBox.setSelectedId(selected, juce::dontSendNotification);
    midiChannelBox.setSelectedId(settings.channel, juce::dontSendNotification);
}

void KemperControlAudioProcessorEditor::timerCallback()
{
    const auto bank = juce::roundToInt(kemperProcessor.parameters.getRawParameterValue("bank")->load()) + 1;
    const auto slot = juce::roundToInt(kemperProcessor.parameters.getRawParameterValue("slot")->load()) + 1;
    bankReadout.setText("BANK " + juce::String(bank).paddedLeft('0', 2)
                        + " / SLOT " + juce::String(slot).paddedLeft('0', 2), juce::dontSendNotification);
    midiStatus.setText(kemperProcessor.directOutput.getStatus(), juce::dontSendNotification);
    const auto settings = kemperProcessor.directOutput.getSettings();
    midiChannelBox.setSelectedId(settings.channel, juce::dontSendNotification);
    for (size_t i = 0; i < rigButtons.size(); ++i)
        rigButtons[i].setToggleState(slot == static_cast<int>(i + 1), juce::dontSendNotification);
    const auto index = midiOutputBox.getSelectedId() - 2;
    const auto shown = juce::isPositiveAndBelow(index, midiPorts.size()) ? midiPorts[index].identifier : juce::String();
    if (shown != settings.id) refreshMidiPorts();
    if (++portRefreshTicks >= 8)
    {
        refreshMidiPorts();
        portRefreshTicks = 0;
    }
}
