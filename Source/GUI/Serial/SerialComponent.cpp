#include "SerialComponent.h"
#include "Source/Serial/SerialDevice.h"

using namespace juce;

SerialComponent::SerialComponent(SerialDevice& serialDeviceRef) 
    : serialDeviceRef(serialDeviceRef)
{
    setOpaque(true);

    volume1.setSliderStyle(juce::Slider::LinearHorizontal);
    volume1.setRange(0.0, 220.0, 1.0);
    volume1.setValue(0.0);
    volume1.addListener(this);
    //========================//
    volume2.setSliderStyle(juce::Slider::LinearHorizontal);
    volume2.setRange(0.0, 220.0, 1.0);
    volume2.setValue(0.0);
    volume2.addListener(this);
    //========================//



    addAndMakeVisible(volume1);
    addAndMakeVisible(volume2);
}

void SerialComponent::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void SerialComponent::resized()
{
    volume1.setBounds(50, 50, 350, 50);
    volume2.setBounds(50, 35, 350, 35);
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
    else if (slider == &volume2)
    {
        int sliderValue = static_cast<int>(volume2.getValue());
        serialDeviceRef.handleOutVoie2(sliderValue);
    }
}



