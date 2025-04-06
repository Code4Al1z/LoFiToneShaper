#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr)
{
    parameters.createAndAddParameter("lowPassCutoff", "Low Pass Cutoff", "Hz", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.5f), 8000.0f, nullptr, nullptr);
    parameters.createAndAddParameter("highPassCutoff", "High Pass Cutoff", "Hz", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.5f), 200.0f, nullptr, nullptr);
    parameters.createAndAddParameter("focusGain", "Focus Gain", "dB", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f, 0.5f), 3.0f, nullptr, nullptr);
    parameters.createAndAddParameter("drive", "Drive", "", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.5f, nullptr, nullptr);
    parameters.createAndAddParameter("bitDepth", "Bit Depth", "", juce::NormalisableRange<float>(4, 16, 1), 6.0f, nullptr, nullptr);
    parameters.createAndAddParameter("sampleRateReduction", "Sample Rate Reduction", "", juce::NormalisableRange<float>(1, 10, 1), 4.0f, nullptr, nullptr);

    // Attach parameters to the AudioProcessorValueTreeState
    parameters.state = juce::ValueTree("savedParams");
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.numChannels = getTotalNumOutputChannels();

    lowPassFilter.prepare(processSpec);
    highPassFilter.prepare(processSpec);
    focusFilter.prepare(processSpec);

    // Use values from the parameters
    float lowPassCutoff = *parameters.getRawParameterValue("lowPassCutoff");
    float highPassCutoff = *parameters.getRawParameterValue("highPassCutoff");
    float focusFreq = 2000.0f;
    float focusGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("focusGain")->load());

    lowPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, lowPassCutoff);
    highPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, highPassCutoff);
    focusFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, focusFreq, 1.0f, focusGain);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    highPassFilter.process(context);
    lowPassFilter.process(context);
    focusFilter.process(context);

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    int bitDepth = (int)*parameters.getRawParameterValue("bitDepth");
    int sampleRateReduction = (int)*parameters.getRawParameterValue("sampleRateReduction");

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            float processedSample = applyBitReduction(inputSample, bitDepth, sampleRateReduction, sampleCounter, heldSample);
            channelData[sample] = processedSample;

            float drive = *parameters.getRawParameterValue("drive");
            inputSample = applySaturation(inputSample, drive);
        }
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

float AudioPluginAudioProcessor::applyBitReduction(float input, int bitDepth, int sampleRateReduction, int& sampleCounter, float& heldSample)
{
    if (++sampleCounter >= sampleRateReduction)
    {
        sampleCounter = 0;
        float maxVal = std::pow(2.0f, bitDepth - 1) - 1.0f;
        heldSample = std::round(input * maxVal) / maxVal;
    }
    return heldSample;
}

float AudioPluginAudioProcessor::applySaturation(float inputSample, float drive)
{
    float gain = 1.0f + drive * 10.0f;
    inputSample *= gain;
    return std::tanh(inputSample);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
