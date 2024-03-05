#include "SerialComponent.h"
#include "Source/Serial/SerialDevice.h"

using namespace juce;

SerialComponent::SerialComponent(SerialDevice& serialDeviceRef) 
    : serialDeviceRef(serialDeviceRef)
{
    setOpaque(true);

    addAndMakeVisible(volume1);
    volume1.setSliderStyle(juce::Slider::LinearHorizontal);
    volume1.setRange(0.0, 220.0, 1.0);
    volume1.setValue(0.0);
    volume1.addListener(this);
}

void SerialComponent::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void SerialComponent::resized()
{
    volume1.setBounds(getLocalBounds());
}

void SerialComponent::init()
{
}

void SerialComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volume1)
    {
        int sliderValue = static_cast<int>(volume1.getValue());
        serialDeviceRef.handleOutVoie1(sliderValue);
    }
}



