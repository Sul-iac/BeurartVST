 #include <JuceHeader.h>
#include "GUI/MainComponent.h"
#include "Serial/SerialDevice.h"
#include <juce_serialport/juce_serialport.h>
#include <Source/Serial/SerialPortListMonitor.h>

// NOTE: this is hard coded for test purposes, and should be replaced with a way to set it in the UI

//const juce::String kSerialPortName{ "\\\\.\\COM3" };

class JuceSerialApplication : public juce::JUCEApplication
{
public:
    JuceSerialApplication() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise([[maybe_unused]] const juce::String& commandLine) override
    {
        initSerial();
        initUi();
    }

    void shutdown() override
    {
        mainWindow = nullptr; // (deletes our window)
    }

    void anotherInstanceStarted([[maybe_unused]] const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    void suspended()
    {
    }

    void resumed()
    {
    }

    void systemRequestedQuit()
    {
        quit();
    }

    void initSerial()
    {
        // NOTE: for example purposes, the SerialDevice class tries to open the serial port once a name has been assigned
       //serialDevice.init(kSerialPortName);

            SerialPortListMonitor portListMonitor;
            juce::StringPairArray serialPorts = portListMonitor.getSerialPortList();
            const juce::String serialPortName = serialPorts.getAllValues()[0];
            serialDevice.init(serialPortName);

    }

    void initUi()
    {
        mainWindow.reset(new MainWindow(getApplicationName(), serialDevice, portListMonitor));
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name, SerialDevice& serialDeviceRef, SerialPortListMonitor& serialPortListMonitorRef)
            : DocumentWindow(name,
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons),
            mainComponent(serialDeviceRef, serialPortListMonitorRef)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(serialDeviceRef, serialPortListMonitorRef), true);
            setUsingNativeTitleBar(false);
#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, true);
#endif

            setVisible(true);
            centreWithSize(getWidth(), getHeight());

        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        MainComponent mainComponent;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;

    SerialDevice serialDevice;
    SerialPortListMonitor portListMonitor;
};

START_JUCE_APPLICATION(JuceSerialApplication)
