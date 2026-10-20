#include "PluginProcessor.h"
#include "PluginEditor.h"

MasterStudioUltimateAudioProcessor::APVTS::ParameterLayout
MasterStudioUltimateAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "inputGain", "Input", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f, " dB"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "hpFreq", "High-pass", juce::NormalisableRange<float> (20.0f, 500.0f, 1.0f, 0.35f), 30.0f, " Hz"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "threshold", "Comp Threshold", juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f), -18.0f, " dB"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "ratio", "Comp Ratio", juce::NormalisableRange<float> (1.0f, 12.0f, 0.1f), 2.5f, ":1"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "attack", "Attack", juce::NormalisableRange<float> (1.0f, 100.0f, 0.1f), 10.0f, " ms"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "release", "Release", juce::NormalisableRange<float> (10.0f, 500.0f, 1.0f), 100.0f, " ms"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "saturation", "Saturation", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "outputGain", "Output", juce::NormalisableRange<float> (-24.0f, 12.0f, 0.1f), 0.0f, " dB"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "limitCeiling", "Limiter Ceiling", juce::NormalisableRange<float> (-12.0f, 0.0f, 0.1f), -1.0f, " dB"));
    return layout;
}

MasterStudioUltimateAudioProcessor::MasterStudioUltimateAudioProcessor()
    : AudioProcessor (BusesProperties()
          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

bool MasterStudioUltimateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();
    return in == out && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void MasterStudioUltimateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec = { sampleRate, static_cast<juce::uint32> (samplesPerBlock),
             static_cast<juce::uint32> (getTotalNumOutputChannels()) };

    highPass.prepare (spec);
    compressor.prepare (spec);
    limiter.prepare (spec);

    highPass.reset();
    compressor.reset();
    limiter.reset();

    highPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 30.0);
    compressor.setAttack (10.0f);
    compressor.setRelease (100.0f);
    compressor.setRatio (2.5f);
    compressor.setThreshold (-18.0f);
    limiter.setThreshold (juce::Decibels::decibelsToGain (-1.0f));
    limiter.setRelease (80.0f);
}

void MasterStudioUltimateAudioProcessor::releaseResources()
{
}

void MasterStudioUltimateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalInput = getTotalNumInputChannels();
    const auto totalOutput = getTotalNumOutputChannels();

    for (int ch = totalInput; ch < totalOutput; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    const auto inputDb = parameters.getRawParameterValue ("inputGain")->load();
    const auto hpHz = parameters.getRawParameterValue ("hpFreq")->load();
    const auto threshold = parameters.getRawParameterValue ("threshold")->load();
    const auto ratio = parameters.getRawParameterValue ("ratio")->load();
    const auto attack = parameters.getRawParameterValue ("attack")->load();
    const auto release = parameters.getRawParameterValue ("release")->load();
    const auto saturation = parameters.getRawParameterValue ("saturation")->load();
    const auto outputDb = parameters.getRawParameterValue ("outputGain")->load();
    const auto ceilingDb = parameters.getRawParameterValue ("limitCeiling")->load();

    highPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (spec.sampleRate, hpHz);
    compressor.setThreshold (threshold);
    compressor.setRatio (ratio);
    compressor.setAttack (attack);
    compressor.setRelease (release);
    limiter.setThreshold (juce::Decibels::decibelsToGain (ceilingDb));

    buffer.applyGain (juce::Decibels::decibelsToGain (inputDb));

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    highPass.process (context);
    compressor.process (context);

    if (saturation > 0.0001f)
    {
        const float drive = 1.0f + saturation * 8.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                data[i] = std::tanh (data[i] * drive) / std::tanh (drive);
        }
    }

    buffer.applyGain (juce::Decibels::decibelsToGain (outputDb));
    limiter.process (context);
}

juce::AudioProcessorEditor* MasterStudioUltimateAudioProcessor::createEditor()
{
    return new MasterStudioUltimateAudioProcessorEditor (*this);
}

void MasterStudioUltimateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MasterStudioUltimateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MasterStudioUltimateAudioProcessor();
}
