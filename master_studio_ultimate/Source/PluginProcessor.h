
#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

class MasterStudioUltimateAudioProcessor final
    : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    MasterStudioUltimateAudioProcessor();

    ~MasterStudioUltimateAudioProcessor() override = default;

    // Cycle de vie du traitement audio
    void prepareToPlay(
        double sampleRate,
        int samplesPerBlock) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(
        const BusesLayout& layouts) const override;

    // Traitement audio
    void processBlock(
        juce::AudioBuffer<float>& buffer,
        juce::MidiBuffer& midiMessages) override;

    // Interface graphique
    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override
    {
        return true;
    }

    // Informations du plugin
    const juce::String getName() const override
    {
        return JucePlugin_Name;
    }

    bool acceptsMidi() const override
    {
        return false;
    }

    bool producesMidi() const override
    {
        return false;
    }

    bool isMidiEffect() const override
    {
        return false;
    }

    double getTailLengthSeconds() const override
    {
        return 2.0;
    }

    // Gestion des programmes
    int getNumPrograms() override
    {
        return 1;
    }

    int getCurrentProgram() override
    {
        return 0;
    }

    void setCurrentProgram(int) override
    {
    }

    const juce::String getProgramName(int) override
    {
        return {};
    }

    void changeProgramName(
        int,
        const juce::String&) override
    {
    }

    // Sauvegarde et restauration des paramètres
    void getStateInformation(
        juce::MemoryBlock& destinationData) override;

    void setStateInformation(
        const void* data,
        int sizeInBytes) override;

    // Paramètres audio accessibles par l'éditeur
    APVTS parameters;

private:
    // Création des paramètres
    static APVTS::ParameterLayout createParameterLayout();

    // Filtre passe-haut
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>
    > highPass;

    // Compresseur
    juce::dsp::Compressor<float> compressor;

    // Limiteur
    juce::dsp::Limiter<float> limiter;

    // Configuration DSP
    juce::dsp::ProcessSpec spec {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MasterStudioUltimateAudioProcessor)
};
