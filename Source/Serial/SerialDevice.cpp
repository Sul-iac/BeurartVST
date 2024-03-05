#include "SerialDevice.h"

#define kBPS 115200
const auto kNumberOfDecimalPlaces{ 4 };

// NOTE: This is a very basic protocol without any error checking. To add error checking, you would want to calculate an error check (checksum, crc, etc)
//       and add that to the end of the packet. On the receiving end you would calculate the error check and compare it to the one sent

enum ParseState
{
    waitingForStartByte1,                   // start byte is "<"
    waitingForCommand,                      // attend les commandes donnees
    waitingForCommandDataSize,              // attend la taille de la commande  
    waitingForCommandData,                  // attend la donnee demande par la commande
    waitingForEndByte1                      // end byte est ">"
};
ParseState parseState = waitingForStartByte1;

const int kStartByte1 = '<';
const int kEndByte2 = '>';

enum Command
{
    none,
    PG_SELECT,
    CHN_SELECT,
    BANK_SELECT,
    BANK_LOAD,
    BANK_SAVE,
//++==========++//
    V1_MODE,
    V1_INVERT,
    V1_48V,
    V1_RF,
    V1_CRT1,
    V1_MUTE,
    V1_OUT,
    V1_DRIVE,
    V1_CLEAN,
    V1_VU,
    V1_BYPASS,
    V1_BPDRIVE,
    V1_BSSBST,
    V1_GNDL,
    V1_BPHPF,
    V1_HPF,
    V1_CAG,
//++=========++//
    V2_MODE,
    V2_INVERT,
    V2_48V,
    V2_RF,
    V2_CRT2,
    V2_MUTE,
    V2_DRIVE,
    V2_CLEAN,
    V2_VU,
    V2_BYPASS,
    V2_BPDRIVE,
    V2_BSSBST,
    V2_GNDL,
    V2_BPHPF,
    V2_HPF,
    V2_CAG,
    V2_OUT,
    endOfList
};

const int kMaxPayloadSize = 20;
Command gTestCommandToExecute{ Command::V1_OUT };

SerialDevice::SerialDevice()
    : Thread(juce::String("SerialDevice"))
{
    // start the serial thread reading data
    startThread();

    // NOTE: this timer is used to send commands for testing purposes
    //startTimer(500);
}

SerialDevice::~SerialDevice()
{
    stopThread(500);
    closeSerialPort();
}

void SerialDevice::init(juce::String newSerialPortName)
{
    serialPortName = newSerialPortName;
}



/*void SerialDevice::setTempo(float tempoToSend)
{
    if (serialPortOutput.get() == nullptr)
        return;

    // NOTE: by sending an int instead of a float we don't have to worry about the receiving end storing floats in the same format as the send
    const auto tempo_as_int{ static_cast<uint32_t>(tempoToSend * std::pow(10, kNumberOfDecimalPlaces)) };
    const std::vector<uint8_t> data{ kStartByte1, Command::V1_OUT, 4,
                                      static_cast<uint8_t>(tempo_as_int & 0xff), static_cast<uint8_t>((tempo_as_int >> 8) & 0xff),
                                      static_cast<uint8_t>((tempo_as_int >> 16) & 0xff), static_cast<uint8_t>((tempo_as_int >> 24) & 0xff) };
    serialPortOutput->write(data.data(), data.size());
}*/


void SerialDevice::open(void)                                   //prepare le port a s'ouvrir en settant le THreadTask. Il ne sera ouvert qu'une fois appele dans run()
{
    if (serialPortName.length() > 0)
        threadTask = ThreadTask::openSerialPort;                
}

void SerialDevice::close(void)                                  //prepare le port a se fermer 
{
    threadTask = ThreadTask::closeSerialPort;
}

bool SerialDevice::openSerialPort(void)                         //on definie la commande d'ouverture
{
    serialPort = std::make_unique<SerialPort>([](juce::String, juce::String) {});
    bool opened = serialPort->open(serialPortName);            // store les donnees de la fonction open dans un variable bool 

    if (opened)
    {
        SerialPortConfig serialPortConfig;                      
        serialPort->getConfig(serialPortConfig);
        serialPortConfig.bps = kBPS;
        serialPortConfig.databits = 8;
        serialPortConfig.parity = SerialPortConfig::SERIALPORT_PARITY_NONE;             // la parity est un error check qui ajoute un extra bit a chaque charactere puis compare si ils sont la. Il est desactive
        serialPortConfig.stopbits = SerialPortConfig::STOPBITS_1;                       // error checking qui ajoute un charactere de fin de data
        serialPort->setConfig(serialPortConfig);                                        // remet a jour les parametres a chaque ouverture

        serialPortInput = std::make_unique<SerialPortInputStream>(serialPort.get());    // creer les streams d'entree 
        serialPortOutput = std::make_unique<SerialPortOutputStream>(serialPort.get());  // creer le stream de sortie. On peut maintenant enovyer et recevoir les donnees
        juce::Logger::outputDebugString("Serial port: " + serialPortName + " opened");  // message
    }
    else
    {
        // report error
        juce::Logger::outputDebugString("Unable to open serial port:" + serialPortName);
    }

    return opened;
}

void SerialDevice::closeSerialPort(void)
{
    serialPortOutput = nullptr;                     //ferme les canaux de sortie et d'entree
    serialPortInput = nullptr;
    if (serialPort != nullptr)                      //si ils ne se ferment pas
    {
        serialPort->close();                        // on ferme le serialPort et on l'initialise
        serialPort = nullptr;
    }
}

// NOTE: these handleXXXXCommand functions store the received data into the data model, and should also alert listeners of the change
//       I usually use ValueTrees for the data model, and use the property change callbacks to notify listeners

void SerialDevice::handleOutVoie1(int sliderValue)
{
    juce::String commandString = "<V1_OUT=";
    commandString += juce::String(sliderValue);
    commandString += ">";

    DBG("Command String: " << commandString); 
    DBG("Data Size: " << commandString.length());

    const char* commandData = commandString.toRawUTF8();
    int dataSize = juce::String(commandData).length();

    if (serialPortOutput.get() != nullptr)
        serialPortOutput->write(reinterpret_cast<const uint8_t*>(commandData), dataSize);
}

void SerialDevice::handleOutVoie2(int sliderValue)
{
    uint8_t data[2];
    data[0] = Command::V2_OUT;
    data[1] = static_cast<uint8_t>(sliderValue);

    if (data[1] < 0)
        data[1] = 0;
    else if (data[1] > 219)
        data[1] = 220;

    if (serialPortOutput.get() != nullptr)
        serialPortOutput->write(data, 2);
}

//=============================== TEST ====================================//


/*void SerialDevice::handleOutVoie2(uint8_t* data, int dataSize)
)
{
    if (dataSize < 2)
        return;

    uint8_t volumeVoie2 = data[1];

    if (volumeVoie2 < 0)
        volumeVoie2 = 0;
    if (volumeVoie2 > 219)
        volumeVoie2 = 220;

    uint8_t spiData[1];
    spiData[0] = volumeVoie2;

    if (serialPortOutput.get() != nullptr)
        serialPortOutput->write(spiData, 1);

    logiquement pareil pour la voie 1;
}*/

     //=========================================== Handle Commande ===================================================//

void SerialDevice::handleCommand(uint8_t command, uint8_t* data, int dataSize)
{
    switch (command)
    {

      /*case Command::V1_OUT: handleOutVoie1(data, dataSize); break;
        case Command::V2_OUT: handleOutVoie2(data, dataSize); break;
        case Command::PG_SELECT: handlePageselection(data, dataSize); break;
        case Command::CHN_SELECT: handleChannelSelection(data, dataSize); break;
        case Command::BANK_SELECT: handleBankSelection(data, dataSize); break;
        case Command::BANK_LOAD: handleBankLoading(data, dataSize); break;
        case Command::BANK_SAVE: handleBankSave(data, dataSize); break;
//++++===========================================================================++++//
                        //-----------VOIE 1---------//

        case Command::V1_MODE: handleModeVoie1(data, dataSize); break;
        case Command::V1_INVERT: handleInvertionVoie1(data, dataSize); break;
        case Command::V1_48V: handle48VoltVoie1(data, dataSize); break;
        case Command::V1_RF: handleV1Rf(data, dataSize); break;
        case Command::V1_CRT1: handleV1Crt1(data, dataSize); break;
        case Command::V1_MUTE: handleMuteVoie1(data, dataSize); break;
       

        case Command::V1_DRIVE: handleDriveVoie1(data, dataSize); break;
        case Command::V1_CLEAN: handleCleanVoie1(data, dataSize); break;
        case Command::V1_VU: handleVuVoie1(data, dataSize); break;
        case Command::V1_BYPASS: handleBypassCleanVoie1(data, dataSize); break;
        case Command::V1_BPDRIVE: handleByPassDriveVoie1(data, dataSize); break;
        case Command::V1_BSSBST: handleBassBoostVoie1(data, dataSize); break;
        case Command::V1_GNDL: handleGndlVoie1(data, dataSize); break;
        case Command::V1_BPHPF: handleByPassHighPassFilterVoie1(data, dataSize); break;
        case Command::V1_HPF: handleHighPassFilterVoie1(data, dataSize); break;
        case Command::V1_CAG: handleCagVoie1(data, dataSize); break;


//===================================================================================================//
                       //-------VOIE 2----------//

        case Command::V2_MODE: handleModeVoie2(data, dataSize); break;
        case Command::V2_INVERT: handleInvertionVoie2(data, dataSize); break;
        case Command::V2_48V: handle48VoltVoie2(data, dataSize); break;
        case Command::V2_RF: handleRfVoie2(data, dataSize); break; 
        case Command::V2_CRT2: handleCrtVoie2(data, dataSize); break;
        case Command::V2_MUTE: handleMuteVoie2(data, dataSize); break;
        case Command::V2_DRIVE: handleDriveVoie2(data, dataSize); break; 
        case Command::V2_CLEAN: handleCleanVoie2(data, dataSize); break;
        case Command::V2_VU: handleVuVoie2(data, dataSize); break;
        case Command::V2_BYPASS: handleByPassCleanVoie2(data, dataSize); break;
        case Command::V2_BPDRIVE: handleByPassDriveVoie2(data, dataSize); break;
        case Command::V2_BSSBST: handleBassBoostVoie2(data, dataSize); break;
        case Command::V2_GNDL: handleGndlVoie2(data, dataSize); break; 
        case Command::V2_BPHPF: handleByPassHighPassFilterVoie2(data, dataSize); break;
        case Command::V2_HPF: handleHighPassFilterVoie2(data, dataSize); break;
        case Command::V2_CAG: handleCagVoie2(data, dataSize); break;*/
 
        
    }
}

#define kSerialPortBufferLen 256
void SerialDevice::run()                                            // c'est dans cette fonction la commande d'ouverture du port se trouve
{
    const int kMaxCommandDataBytes = 16;
    uint8_t commandData[kMaxCommandDataBytes];
    uint8_t command = Command::none;
    uint8_t commandDataSize = 0;
    uint8_t commandDataCount = 0;
    while (!threadShouldExit())                                     // tant que le thread ne rencontre pas d'erreur particuliere et ne dois pas fermer
    {
        juce::ScopedLock lock(serialPortListLock);
        switch (threadTask)
        {
        case ThreadTask::idle:                                      // il est dans cet etat qund il n'y a pas de commandes envoyee, il est au repos
        {
            wait(1);
            if (serialPortName.isNotEmpty())                        
                threadTask = ThreadTask::openSerialPort;            // s'il a trouver un nom de port, le thread ouvre le serial port
        }
        break;
        case ThreadTask::openSerialPort:                            // si le thread est bien ouvert
        {
            if (openSerialPort())
            {
                threadTask = ThreadTask::processSerialPort;         // si le port est ouvert, le thread processSerialPort
            }
            else
            {
                threadTask = ThreadTask::delayBeforeOpening;        // sinon le thread passe a la fonction delayBeforeOpening (temps d;attente avant d'ouvrir le port
                delayStartTime = static_cast<uint64_t>(juce::Time::getMillisecondCounterHiRes()); // track le debut de la periode de delais avant d'essayer de rouvrir le port
            }
        }
        break;

        case ThreadTask::delayBeforeOpening:                                                 // attend avant de rouvrir le port
        {
            wait(1);                                                                        // attend une seconde avant de reessayer d'ouvrir (pour eviter les surcharges)
            if (juce::Time::getApproximateMillisecondCounter() > delayStartTime + 1000)     // check si le temps de une seconde est passe. Si le temps passe est plus grand que delayTime + 1000, le port est ouvert.                                                                                     
                threadTask = ThreadTask::openSerialPort;
        }
        break;

        case ThreadTask::closeSerialPort:   
        {
            closeSerialPort();                                                              // on appelle closeSerialPort. 
            threadTask = ThreadTask::idle;                                                  // le thread passe en mode statique pour attend une autre commande
        }
        break;

        case ThreadTask::processSerialPort:                                                 // dans le cas ou tout se deroule comme prevu
        {
            auto  bytesRead = 0;                                                       // on intitialise les variables de lecture de bytes
            auto  dataIndex = 0;                                                       // pareil pour l'index de donnees de bytes
            uint8_t incomingData[kSerialPortBufferLen];                                // creer un tableau pour stocker la data arrivant dans le buffer

            if ((serialPortInput != nullptr) && (!serialPortInput->isExhausted()))         // si l'input n'est pas vide et que l'input de serialPort n'est pas finis:
            {
                bytesRead = serialPortInput->read(incomingData, kSerialPortBufferLen);     // lis le contenue des donnees de l'input et les met dans incoming data, le nbr max de bytes a lire etant kSerial...
                if (bytesRead < 1){                                                         // si le nombre de Byte lu est inferieur a 1
                    wait(1);
                    continue;                                                              //attend 1 milliseconde et continue. Evite les repetitions de loop quand il n'y a pas de commandes
                }
                else
                {
                    // TODO: extract into function or class
                    // parseIncomingData (incomingData, bytesRead)
                    auto resetParser = [this, &command, &commandDataSize, &commandDataCount]()   //(auto = il va automatiquement typer la variable). il va remettre a zero le parsing des packets
                        {
                            command = Command::none;                                             // il va reinitialiser la variable command a NULL
                            commandDataSize = 0;                                                 // reinitialise la dataSize
                            commandDataCount = 0;                                                // reinitialize la dataCount
                            parseState = ParseState::waitingForStartByte1;                       // le parsing state est reinitialiser a sa valeur de depart (l.17)
                        };
                    // parse incoming data
                    while (dataIndex < bytesRead)                                               // tant que l'index est inferieur au nombre de Bytes lue, donc qu'il y a encore quelque chose a lire
                    {
                        const uint8_t dataByte = incomingData[dataIndex];                       // extrait la ligne a l'index dataIndex de l'incomingData, dans dataByte

                        juce::Logger::outputDebugString(juce::String(dataByte));                // affiche sur l'output la valeur du byte actuel (pour debug et voir la data, on peut voir byte par byte, pratique pour comprendre comment la data est transmise)
                        switch (parseState)                                                     
                        {
                        case ParseState::waitingForStartByte1:                                    
                        {
                            if (dataByte == kStartByte1)                                        // si on est sur le charactere de debut <
                                parseState = ParseState::waitingForCommand;                     // on passe a la commande suivante

                        }
                        break;
                        case ParseState::waitingForCommand:                                     // il attend que la suite de la commande soit une commande valide, donc un caractere qu'il connait faisant partie des characteres des commandes
                        {
                            if (dataByte >= Command::none && dataByte < Command::endOfList)     // il check si l'index lie une commande valide: none = 0 et endOfList = 32
                            {
                                command = dataByte;                                             // dataByste est store dans command
                                parseState = ParseState::waitingForCommandDataSize;             // on passe a la commande suivante
                            }
                            else
                            {
                                resetParser();                                                  // sinon reset le parser
                            }
                        }
                        break;
                        case ParseState::waitingForCommandDataSize:                                                     // il attend la data Payload associe avec la commande              
                        {
                            // /!\ on lit ici que bytes par bytes, pour lire deux bytes, 
                            // il faudra faire deux states pour lire la longueur, un pour le LSB et un pour le MSB
                            if (dataByte >= 0 && dataByte <= kMaxCommandDataBytes)                                      // check si la commande a une taille valide    
                            {
                                commandDataSize = dataByte;                                                             // on met les donnees dans commandDataSize
                                parseState = ParseState::waitingForCommandData;                                         // on passe au suivant
                            }
                            else
                            {
                                resetParser();
                            }
                        }
                        break;
                        case ParseState::waitingForCommandData:                                                         // attend les data Bytes, le nombre de bytes etant definit par commandDataSize obtenue precedemment
                        {
                            if (commandDataSize != 0)
                            {
                                commandData[commandDataCount] = dataByte;                                               // on met les dataByte dans le tableau commandData
                                ++commandDataCount;                                                                     // on incremente l'index du tableau
                            }
                            if (commandDataCount == commandDataSize)                                                    // regarde si tous les bytes on ete recues
                            {
                                handleCommand(command, commandData, commandDataSize);                                   // execute la commande
                                resetParser();
                            }
                        }
                        break;
                        default:
                        {
                            jassertfalse;                                                                              // le code a un probleme
                        }
                        }
                        ++dataIndex;
                    }
                }
            }
        }
        break;
        }
    }
}

 //cette partie du programme fait que chaque programme s'execute l'un apres l'autre */

/*void SerialDevice::timerCallback()
{
    if (serialPortOutput.get() == nullptr)
        return;
    switch (gTestCommandToExecute)
    {
    case Command::V1_OUT:
    {
        juce::Logger::outputDebugString("setting lightColor");
        setLightColor(100);
        gTestCommandToExecute = Command::tempo;
    }
    break;
    case Command::tempo:
    {
        juce::Logger::outputDebugString("setting tempo");
        setTempo(120.f);
        gTestCommandToExecute = Command::chargingAlarmLevel;
    }
    break;
    case Command::chargingAlarmLevel:
    {
        juce::Logger::outputDebugString("setting ChargingAlarmLevel");
        setChargingAlarmLevel(0, 50);
        gTestCommandToExecute = Command::lightColor;
    }
    break;
    }
}*/

