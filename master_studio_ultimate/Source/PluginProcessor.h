#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

class MasterStudioUltimateAudioProcessorEditor;

class MasterStudioUltimateAudioProcessor final
    : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    MasterStudioUltimateAudioProcessor();
    ~MasterStudioUltimateAudioProcessor() override = default;

    static APVTS::ParameterLayout createParameterLayout();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(
        juce::AudioBuffer<float>&,
        juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override
    {
        return JucePlugin_Name;
    }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    APVTS parameters;

private:
    juce::dsp::ProcessSpec spec;

    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>> highPass;

    juce::dsp::Compressor<float> compressor;
    juce::dsp::Limiter<float> limiter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MasterStudioUltimateAudioProcessor)
};
