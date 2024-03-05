#pragma once

#include "SerialPortListMonitor.h"
#include <juce_serialport/juce_serialport.h>
#include <mutex>

// NOTE: This class could be named for the name of the thing you are connecting to: LEDContllerDevice, DrumMachineDevice, SolarMonitorDevice, etc
class SerialDevice : private juce::Thread                             //rajouter ici "private juce::Timer" si jamais j'ai besoin de remettre la fonction timerCallback 
{
public:
    SerialDevice();
    ~SerialDevice();
    void open(void);
    void close(void);
    void init(juce::String newSerialPortName);

    void handleOutVoie1(int sliderValue);
    //void sendSliderVolumeVoie1(int sliderValue);


private:
    enum class ThreadTask
    {
        idle,
        delayBeforeOpening,
        openSerialPort,
        closeSerialPort,
        processSerialPort,
    };

    juce::CriticalSection serialPortListLock;
    juce::String serialPortName;
    std::unique_ptr<SerialPort> serialPort;
    std::unique_ptr<SerialPortInputStream> serialPortInput;
    std::unique_ptr<SerialPortOutputStream> serialPortOutput;
    ThreadTask threadTask{ ThreadTask::idle };
    uint64_t delayStartTime{ 0 };

    // NOTE: included but not used in this example. shows how to monitor for serial port device list changes
    SerialPortListMonitor serialPortListMonitor;

    bool openSerialPort(void);
    void closeSerialPort(void);
    void handleCommand(uint8_t command, uint8_t* data, int dataSize);

//===========================================================//
    void handleOutVoie2(int sliderValue);

    void run() override;
};