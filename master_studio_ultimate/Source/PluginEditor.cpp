
#include "PluginEditor.h"

MasterStudioUltimateAudioProcessorEditor::
MasterStudioUltimateAudioProcessorEditor(
    MasterStudioUltimateAudioProcessor& p)
    : juce::AudioProcessorEditor(&p),
      processor(p)
{
    setSize(760, 430);

    // Titre principal
    title.setText(
        "MASTER STUDIO ULTIMATE",
        juce::dontSendNotification
    );

    title.setFont(
        juce::Font(24.0f, juce::Font::bold)
    );

    title.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    addAndMakeVisible(title);

    // Sous-titre
    subtitle.setText(
        "MIX / MASTER PROTOTYPE | Vocal tools planned for next stage",
        juce::dontSendNotification
    );

    subtitle.setFont(juce::Font(12.0f));

    subtitle.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey
    );

    addAndMakeVisible(subtitle);

    // Création des neuf curseurs et de leurs étiquettes
    for (const auto& control : controls)
    {
        auto* label = labels.add(new juce::Label());

        label->setText(
            control.first,
            juce::dontSendNotification
        );

        label->setJustificationType(
            juce::Justification::centred
        );

        label->setColour(
            juce::Label::textColourId,
            juce::Colours::whitesmoke
        );

        addAndMakeVisible(label);

        auto* slider = sliders.add(
            new juce::Slider(
                juce::Slider::RotaryHorizontalVerticalDrag,
                juce::Slider::TextBoxBelow
            )
        );

        slider->setTextBoxStyle(
            juce::Slider::TextBoxBelow,
            false,
            82,
            22
        );

        slider->setColour(
            juce::Slider::rotarySliderFillColourId,
            juce::Colour(0xff56b6c2)
        );

        slider->setColour(
            juce::Slider::thumbColourId,
            juce::Colour(0xff61dafb)
        );

        slider->setColour(
            juce::Slider::textBoxTextColourId,
            juce::Colours::white
        );

        slider->setColour(
            juce::Slider::textBoxBackgroundColourId,
            juce::Colour(0xff242b34)
        );

        addAndMakeVisible(slider);

        // Liaison du curseur au paramètre audio correspondant
        attachments.push_back(
            std::make_unique<SliderAttachment>(
                processor.parameters,
                control.second,
                *slider
            )
        );
    }
}

// Dessin de l'interface
void MasterStudioUltimateAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    // Fond général
    g.fillAll(juce::Colour(0xff15191f));

    // Panneau principal
    g.setColour(juce::Colour(0xff242b34));

    g.fillRoundedRectangle(
        18.0f,
        86.0f,
        static_cast<float>(getWidth() - 36),
        static_cast<float>(getHeight() - 104),
        12.0f
    );

    // Barre décorative turquoise
    g.setColour(juce::Colour(0xff56b6c2));

    g.fillRoundedRectangle(
        18.0f,
        18.0f,
        5.0f,
        44.0f,
        2.0f
    );
}

// Positionnement des composants
void MasterStudioUltimateAudioProcessorEditor::resized()
{
    title.setBounds(
        34,
        16,
        getWidth() - 50,
        34
    );

    subtitle.setBounds(
        34,
        50,
        getWidth() - 50,
        24
    );

    constexpr int columns = 5;
    constexpr int top = 100;
    constexpr int left = 26;
    constexpr int horizontalPadding = 52;
    constexpr int cellHeight = 148;

    const int cellWidth =
        (getWidth() - horizontalPadding) / columns;

    for (int i = 0; i < sliders.size(); ++i)
    {
        const int row = i / columns;
        const int column = i % columns;

        const int x = left + column * cellWidth;
        const int y = top + row * cellHeight;

        labels[i]->setBounds(
            x,
            y,
            cellWidth - 4,
            24
        );

        sliders[i]->setBounds(
            x + 4,
            y + 24,
            cellWidth - 12,
            112
        );
    }
}
