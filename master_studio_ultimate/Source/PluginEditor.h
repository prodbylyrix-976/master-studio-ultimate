#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

class MasterStudioUltimateAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit MasterStudioUltimateAudioProcessorEditor (MasterStudioUltimateAudioProcessor&);
    ~MasterStudioUltimateAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    MasterStudioUltimateAudioProcessor& processor;
    juce::Label title;
    juce::Label subtitle;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> labels;
    std::vector<std::unique_ptr<SliderAttachment>> attachments;
    const std::array<std::pair<const char*, const char*>, 9> controls {{
        { "Input", "inputGain" }, { "High-pass Hz", "hpFreq" },
        { "Threshold dB", "threshold" }, { "Ratio", "ratio" },
        { "Attack ms", "attack" }, { "Release ms", "release" },
        { "Saturation", "saturation" }, { "Output dB", "outputGain" },
        { "Limiter ceiling", "limitCeiling" }
    }};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterStudioUltimateAudioProcessorEditor)
};
