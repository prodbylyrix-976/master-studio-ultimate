#include "PluginEditor.h"

MasterStudioUltimateAudioProcessorEditor::MasterStudioUltimateAudioProcessorEditor
    (MasterStudioUltimateAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (760, 430);
    title.setText ("MASTER STUDIO ULTIMATE", juce::dontSendNotification);
    title.setFont (juce::Font (24.0f, juce::Font::bold));
    title.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (title);

    subtitle.setText ("MIX / MASTER PROTOTYPE  •  Vocal tools planned for next stage",
                      juce::dontSendNotification);
    subtitle.setFont (juce::Font (12.0f));
    subtitle.setColour (juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible (subtitle);

    for (const auto& control : controls)
    {
        auto* label = labels.add (new juce::Label());
        label->setText (control.first, juce::dontSendNotification);
        label->setJustificationType (juce::Justification::centred);
        label->setColour (juce::Label::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible (label);

        auto* slider = sliders.add (new juce::Slider (juce::Slider::RotaryHorizontalVerticalDrag,
                                                      juce::Slider::TextBoxBelow));
        slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 82, 22);
        slider->setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff56b6c2));
        slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff61dafb));
        addAndMakeVisible (slider);
        attachments.push_back (std::make_unique<SliderAttachment> (processor.parameters, control.second, *slider));
    }
}

void MasterStudioUltimateAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff15191f));
    g.setColour (juce::Colour (0xff242b34));
    g.fillRoundedRectangle (18.0f, 86.0f, static_cast<float> (getWidth() - 36),
                            static_cast<float> (getHeight() - 104), 12.0f);
    g.setColour (juce::Colour (0xff56b6c2));
    g.fillRoundedRectangle (18.0f, 18.0f, 5.0f, 44.0f, 2.0f);
}

void MasterStudioUltimateAudioProcessorEditor::resized()
{
    title.setBounds (34, 16, getWidth() - 50, 34);
    subtitle.setBounds (34, 50, getWidth() - 50, 24);

    const int cols = 5;
    const int top = 104;
    const int cellW = (getWidth() - 52) / cols;
    const int cellH = 148;

    for (int i = 0; i < sliders.size(); ++i)
    {
        const int row = i / cols;
        const int col = i % cols;
        const int x = 26 + col * cellW;
        const int y = top + row * cellH;
        labels[i]->setBounds (x, y, cellW - 4, 24);
        sliders[i]->setBounds (x + 4, y + 24, cellW - 12, 112);
    }
}
