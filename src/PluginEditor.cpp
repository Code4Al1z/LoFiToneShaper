#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), parameters(p.parameters)
{
    // Create sliders for controlling parameters

    // Low-pass cutoff slider
    lowPassSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lowPassSlider.setRange(20.0f, 20000.0f, 1.0f);
    lowPassSlider.setValue(*parameters.getRawParameterValue("lowPassCutoff"));
    lowPassSlider.addListener(this);
    addAndMakeVisible(lowPassSlider);

    // High-pass cutoff slider
    highPassSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    highPassSlider.setRange(20.0f, 20000.0f, 1.0f);
    highPassSlider.setValue(*parameters.getRawParameterValue("highPassCutoff"));
    highPassSlider.addListener(this);
    addAndMakeVisible(highPassSlider);

    // Focus gain slider
    focusGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    focusGainSlider.setRange(-12.0f, 12.0f, 0.1f);
    focusGainSlider.setValue(*parameters.getRawParameterValue("focusGain"));
    focusGainSlider.addListener(this);
    addAndMakeVisible(focusGainSlider);

    // Drive slider
    driveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    driveSlider.setRange(0.0f, 1.0f, 0.01f);
    driveSlider.setValue(*parameters.getRawParameterValue("drive"));
    driveSlider.addListener(this);
    addAndMakeVisible(driveSlider);

    // Bit depth slider
    bitDepthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bitDepthSlider.setRange(4, 16, 1);
    bitDepthSlider.setValue(*parameters.getRawParameterValue("bitDepth"));
    bitDepthSlider.addListener(this);
    addAndMakeVisible(bitDepthSlider);

    // Sample rate reduction slider
    sampleRateReductionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sampleRateReductionSlider.setRange(1, 10, 1);
    sampleRateReductionSlider.setValue(*parameters.getRawParameterValue("sampleRateReduction"));
    sampleRateReductionSlider.addListener(this);
    addAndMakeVisible(sampleRateReductionSlider);

    setSize(400, 300);
}


AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void AudioPluginAudioProcessorEditor::resized()
{
    int sliderHeight = 30;
    int padding = 10;
    int y = 10;

    auto setBounds = [&](juce::Slider& slider)
        {
            slider.setBounds(10, y, getWidth() - 20, sliderHeight);
            y += sliderHeight + padding;
        };

    setBounds(lowPassSlider);
    setBounds(highPassSlider);
    setBounds(focusGainSlider);
    setBounds(driveSlider);
    setBounds(bitDepthSlider);
    setBounds(sampleRateReductionSlider);
}

void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &lowPassSlider)
	{
		*parameters.getRawParameterValue("lowPassCutoff") = lowPassSlider.getValue();
	}
	else if (slider == &highPassSlider)
	{
		*parameters.getRawParameterValue("highPassCutoff") = highPassSlider.getValue();
	}
	else if (slider == &focusGainSlider)
	{
		*parameters.getRawParameterValue("focusGain") = focusGainSlider.getValue();
	}
	else if (slider == &driveSlider)
	{
		*parameters.getRawParameterValue("drive") = driveSlider.getValue();
	}
	else if (slider == &bitDepthSlider)
	{
		*parameters.getRawParameterValue("bitDepth") = bitDepthSlider.getValue();
	}
	else if (slider == &sampleRateReductionSlider)
	{
		*parameters.getRawParameterValue("sampleRateReduction") = sampleRateReductionSlider.getValue();
	}
}
