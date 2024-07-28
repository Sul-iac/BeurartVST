/*
  ==============================================================================

    OutputOneAndTwo.cpp
    Created: 16 Jul 2024 3:14:41pm
    Author:  barro

  ==============================================================================
*/

#include "../SerialDevice.h"


void SerialDevice::handleOutVoie1(int sliderValue)
{
    if (serialPortOutput) {
        DBG("serialPortOutput is valid. Sending data...");
        juce::String commandString = "<V1_OUT=" + juce::String(sliderValue) + ">";
        DBG("Command String: " + commandString);
        const char* commandData = commandString.toRawUTF8();
        int dataSize = static_cast<int>(strlen(commandData));

        serialPortOutput->write(reinterpret_cast<const uint8_t*>(commandData), dataSize);
    }
    else {
        DBG("serialPortOutput is null, cannot send data.");
    }
}

void SerialDevice::handleOutVoie2(int sliderValue)
{
    if (serialPortOutput) {
        DBG("serialPortOutput is valid. Sending data...");
        juce::String commandString = "<V2_OUT=" + juce::String(sliderValue) + ">";
        DBG("Command String: " + commandString);
        const char* commandData = commandString.toRawUTF8();
        int dataSize = static_cast<int>(strlen(commandData));

        serialPortOutput->write(reinterpret_cast<const uint8_t*>(commandData), dataSize);
    }
    else {
        DBG("serialPortOutput is null, cannot send data.");
    }
}