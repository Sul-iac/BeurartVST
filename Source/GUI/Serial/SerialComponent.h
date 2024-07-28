#pragma once

#include <JuceHeader.h>
#include "Source/Serial/SerialDevice.h"
#include "juce_serialport/juce_serialport.h"

class SerialComponent : public juce::Component,
                        private juce::Slider::Listener
{
public:
    SerialComponent(SerialDevice& serialDeviceRef);


    void sliderValueChanged(juce::Slider* slider);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void init();

private:

    SerialDevice& serialDeviceRef;

    juce::Slider volume1;
    juce::Slider volume2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialComponent)
};

