#include "SerialComponent.h"
#include "Source/Serial/SerialDevice.h"

using namespace juce;

SerialComponent::SerialComponent(SerialDevice& serialDeviceRef, SerialPortListMonitor& serialPortListMonitorRef) 
                                  : serialDeviceRef(serialDeviceRef), 
                                    serialPortListMonitorRef(serialPortListMonitorRef)
{
    setOpaque(true);
  //========= V1_OUT ================//
    addAndMakeVisible(volume1);
    volume1.setSliderStyle(juce::Slider::LinearHorizontal);
    volume1.setRange(0.0, 220.0, 1.0);
    volume1.setValue(0.0);
    volume1.addListener(this);
    
  //========= UI Port slector =======//
    addAndMakeVisible(portSelectionUI);
    setSize(400, 200);
    textLabel.setFont(textFont);

    juce::StringPairArray portList = serialPortListMonitorRef.getSerialPortList();
    portSelectionUI.clear();
    for (int i = 0; i < portList.size(); i++)
    {
        portSelectionUI.addItem(portList.getAllValues()[i], i + 1);
    }

    portSelectionUI.addListener(this);

}

void SerialComponent::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void SerialComponent::resized()
{
    volume1.setBounds(getLocalBounds());
    portSelectionUI.setBounds(getLocalBounds());
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

void SerialComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    int selectedIndex = portSelectionUI.getSelectedItemIndex();

    if (selectedIndex >= 0)
    {
        juce::String selectedPortName = portSelectionUI.getItemText(selectedIndex);
        //juceSerialApplicationRef.initSerial(selectedIndex);
    }
}

void SerialComponent::refreshPortList()
{
    
}

