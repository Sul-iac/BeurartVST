#pragma once

#include <JuceHeader.h>
#include "Source/Serial/SerialDevice.h"
#include "juce_serialport/juce_serialport.h"

class SerialComponent : public juce::Component,
                        private juce::Slider::Listener,
                        private juce::ComboBox::Listener
{
public:
    SerialComponent(SerialDevice& serialDeviceRef, SerialPortListMonitor& serialPortListMonitorRef);

    void sliderValueChanged(juce::Slider* slider);
    void comboBoxChanged(juce::ComboBox* comboBox);
    void refreshPortList();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void init();

private:

//====== reference de classe ======// 

    SerialDevice& serialDeviceRef;
    SerialPortListMonitor& serialPortListMonitorRef;

//======== Slider ===========//
    juce::Slider volume1;

//======== ComboBox =========//

    juce::Label textLabel { {}, "Oui je suis une boite avec des trucs dedans" };
    juce::Font textFont   { 12.0f };
    juce::ComboBox portSelectionUI;





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialComponent)
};

