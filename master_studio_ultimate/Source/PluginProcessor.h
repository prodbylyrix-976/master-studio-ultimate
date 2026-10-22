
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

//==============================================================
// Paramètres du plugin
//==============================================================

MasterStudioUltimateAudioProcessor::APVTS::ParameterLayout
MasterStudioUltimateAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "inputGain",
        "Input",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        " dB"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "hpFreq",
        "High-pass",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.35f),
        30.0f,
        " Hz"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "threshold",
        "Comp Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -18.0f,
        " dB"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ratio",
        "Comp Ratio",
        juce::NormalisableRange<float>(1.0f, 12.0f, 0.1f),
        2.5f,
        ":1"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "attack",
        "Attack",
        juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f),
        10.0f,
        " ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "release",
        "Release",
        juce::NormalisableRange<float>(10.0f, 500.0f, 1.0f),
        100.0f,
        " ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "saturation",
        "Saturation",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "outputGain",
        "Output",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f),
        0.0f,
        " dB"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "limitCeiling",
        "Limiter Ceiling",
        juce::NormalisableRange<float>(-12.0f, 0.0f, 0.1f),
        -1.0f,
        " dB"
    ));

    return layout;
}

//==============================================================
// Constructeur
//==============================================================

MasterStudioUltimateAudioProcessor::MasterStudioUltimateAudioProcessor()
    : juce::AudioProcessor(
          BusesProperties()
              .withInput(
                  "Input",
                  juce::AudioChannelSet::stereo(),
                  true
              )
              .withOutput(
                  "Output",
                  juce::AudioChannelSet::stereo(),
                  true
              )
      ),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

//==============================================================
// Configuration des entrées et sorties
//==============================================================

bool MasterStudioUltimateAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    const auto& inputLayout = layouts.getMainInputChannelSet();
    const auto& outputLayout = layouts.getMainOutputChannelSet();

    if (inputLayout != outputLayout)
        return false;

    return outputLayout == juce::AudioChannelSet::mono()
        || outputLayout == juce::AudioChannelSet::stereo();
}

//==============================================================
// Préparation du traitement audio
//==============================================================

void MasterStudioUltimateAudioProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize =
        static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels =
        static_cast<juce::uint32>(getTotalNumOutputChannels());

    highPass.prepare(spec);
    compressor.prepare(spec);
    limiter.prepare(spec);

    highPass.reset();
    compressor.reset();
    limiter.reset();

    const float hpHz =
        parameters.getRawParameterValue("hpFreq")->load();

    highPass.state =
        *juce::dsp::IIR::Coefficients<float>::makeHighPass(
            sampleRate,
            static_cast<double>(hpHz)
        );

    compressor.setThreshold(
        parameters.getRawParameterValue("threshold")->load()
    );

    compressor.setRatio(
        parameters.getRawParameterValue("ratio")->load()
    );

    compressor.setAttack(
        parameters.getRawParameterValue("attack")->load()
    );

    compressor.setRelease(
        parameters.getRawParameterValue("release")->load()
    );

    // JUCE attend un seuil en décibels pour le limiteur.
    limiter.setThreshold(
        parameters.getRawParameterValue("limitCeiling")->load()
    );

    limiter.setRelease(80.0f);
}

//==============================================================
// Libération des ressources
//==============================================================

void MasterStudioUltimateAudioProcessor::releaseResources()
{
    highPass.reset();
    compressor.reset();
    limiter.reset();
}

//==============================================================
// Traitement audio principal
//==============================================================

void MasterStudioUltimateAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int totalInputChannels = getTotalNumInputChannels();
    const int totalOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Nettoyage des éventuelles sorties sans entrée correspondante.
    for (int channel = totalInputChannels;
         channel < totalOutputChannels;
         ++channel)
    {
        buffer.clear(channel, 0, numSamples);
    }

    if (numSamples == 0 || spec.sampleRate <= 0.0)
        return;

    // Lecture des paramètres.
    const float inputDb =
        parameters.getRawParameterValue("inputGain")->load();

    const float hpHz =
        parameters.getRawParameterValue("hpFreq")->load();

    const float threshold =
        parameters.getRawParameterValue("threshold")->load();

    const float ratio =
        parameters.getRawParameterValue("ratio")->load();

    const float attack =
        parameters.getRawParameterValue("attack")->load();

    const float release =
        parameters.getRawParameterValue("release")->load();

    const float saturation =
        parameters.getRawParameterValue("saturation")->load();

    const float outputDb =
        parameters.getRawParameterValue("outputGain")->load();

    const float ceilingDb =
        parameters.getRawParameterValue("limitCeiling")->load();

    // Mise à jour du filtre passe-haut.
    highPass.state =
        *juce::dsp::IIR::Coefficients<float>::makeHighPass(
            spec.sampleRate,
            static_cast<double>(hpHz)
        );

    // Mise à jour du compresseur.
    compressor.setThreshold(threshold);
    compressor.setRatio(ratio);
    compressor.setAttack(attack);
    compressor.setRelease(release);

    // Le seuil du limiteur JUCE est exprimé en dB.
    limiter.setThreshold(ceilingDb);

    // Gain d'entrée.
    buffer.applyGain(
        juce::Decibels::decibelsToGain(inputDb)
    );

    // Création du bloc audio et du contexte de traitement.
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // 1. Filtre passe-haut.
    highPass.process(context);

    // 2. Compression dynamique.
    compressor.process(context);

    // 3. Saturation harmonique optionnelle.
    if (saturation > 0.0001f)
    {
        const float drive = 1.0f + saturation * 8.0f;
        const float normalisation = std::tanh(drive);

        for (int channel = 0;
             channel < totalOutputChannels;
             ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                channelData[sample] =
                    std::tanh(channelData[sample] * drive)
                    / normalisation;
            }
        }
    }

    // 4. Gain de sortie.
    buffer.applyGain(
        juce::Decibels::decibelsToGain(outputDb)
    );

    // 5. Limiteur de sortie.
    limiter.process(context);
}

//==============================================================
// Création de l'interface graphique
//==============================================================

juce::AudioProcessorEditor*
MasterStudioUltimateAudioProcessor::createEditor()
{
    return new MasterStudioUltimateAudioProcessorEditor(*this);
}

//==============================================================
// Sauvegarde de l'état du plugin
//==============================================================

void MasterStudioUltimateAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
{
    const auto state = parameters.copyState();

    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    if (xml != nullptr)
        copyXmlToBinary(*xml, destData);
}

//==============================================================
// Restauration de l'état du plugin
//==============================================================

void MasterStudioUltimateAudioProcessor::setStateInformation(
    const void* data,
    int sizeInBytes)
{
    if (data == nullptr || sizeInBytes <= 0)
        return;

    std::unique_ptr<juce::XmlElement> xml(
        getXmlFromBinary(data, sizeInBytes)
    );

    if (xml != nullptr
        && xml->hasTagName(parameters.state.getType().toString()))
    {
        const auto restoredState = juce::ValueTree::fromXml(*xml);

        if (restoredState.isValid())
            parameters.replaceState(restoredState);
    }
}

//==============================================================
// Point d'entrée du plugin JUCE
//==============================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MasterStudioUltimateAudioProcessor();
}
