#pragma once

#include <JuceHeader.h>
#include "Serial/SerialComponent.h"
#include "Source/Serial/SerialDevice.h"

class MainComponent : public juce::Component
{
public:
    MainComponent(SerialDevice& serialDeviceRef, SerialPortListMonitor& serialPortListMonitorRef);
    ~MainComponent() override;
   

private:

    SerialComponent serialComponent;
    void resized() override;
    void paint(juce::Graphics& g) override;
    SerialDevice serialDevice;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
