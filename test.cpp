#include "MicroBit.h"
#include "MicroBitUARTService.h"
#include "Qbit-api.h"

extern MicroBit uBit;

static void onIR(MicroBitEvent e)
{
    uBit.display.scroll(ManagedString("IR:") + ManagedString(e.value));
}

static void onNoIR(MicroBitEvent e)
{
    uBit.display.print('x');
}

static MicroBitUARTService *uBut_uart = NULL;
static void onUartDelimMatch(MicroBitEvent e)
{
    uBit.display.scroll(ManagedString("BT:") + ManagedString(e.value));
    ManagedString charStr = uBut_uart->readUntil("$", ASYNC);
    uBit.display.scroll(charStr);
}

void ubit_test(MicroBit &uBit)
{
#if 0   // Bluetooth UART service
    // startUartService()
    uBut_uart = new MicroBitUARTService(*uBit.ble, 61, 60);
    if (uBut_uart)
    {
        uBut_uart->eventOn("$");
        uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, onUartDelimMatch);
    }
#endif

#if 0   // Qbit exports:
    // Set Qbit carrier mode
    qbit::qbit_carrier();
    // Set Qbit(V2.0 version or newer) run|speed %speed|and oriention %oriention
    // orientation: OrientionType_STOP, OrientionType_GO_AHEAD, OrientionType_GO_BACK, OrientionType_TURN_LEFT, OrientionType_TURN_RIGHT 
    qbit::setQbitRunSpeed(50, orientation);
    // Set the center balance angle of the Qbit
    qbit::setBLAngle();
    // on remote-control|%code|pressed
    qbit::onQbit_remote_ir_pressed(code, onIR);
    // on remote-control stop send
    qbit::onQbit_remote_no_ir(onIR);
    // Ultrasonic distance(cm)
    qbit::Ultrasonic();
    // Set Qbit balance %runType
    // runType: RunType_RUN, RunType_STOP
    qbit::setQbitRun(runType);
    // avoid obstacle|%sensor|detect obstacle
    // sensor: SENSOR1_OBSTACLE, SENSOR2_OBSTACLE
    qbit::obstacleSensor(sensor);
    // set light brightness %brightness
    qbit::setBrightness();
    // Set|%lightoffset|color to %rgb
    // light: QbitRGBLight::Light1, QbitRGBLight::Light2
    // rgb: QbitRGBLight::Red, QbitRGBLight::Orange, QbitRGBLight::Yellow, QbitRGBLight::Green, QbitRGBLight::Blue, QbitRGBLight::Indigo, QbitRGBLight::Violet, QbitRGBLight::Purple, QbitRGBLight::White 
    qbit::setPixelRGB(light, rgb);
    // Set|%lightoffset|color to %rgb
    qbit::setPixelRGBArgs(light, rgb);
    // Show light
    qbit::showLight();
    // Clear light
    qbit::clearLight();
    // Set extension pin|%ext|%iostatus
    // ext: Exts_Ext1, Exts_Ext2
    // iostatus: pinIOStatus_Low, pinIOStatus_Hight
    qbit::setExtsIO(ext, iostatus);
    // Read extension pin|%ext|digital
    qbit::readExtsIODigital(ext);
    // Read extension 1 pin analog
    qbit::readExt1Analog();
    // Get Qbit current voltage (mV)
    qbit::getBatVoltage();
    // Get bluetooth command type %str
    // str: "CMD|10|", "CMD|01|...|"
    qbit::analyzeBluetoothCmd(str)
    // Get bluetooth command|%str|argument at %index
    // str: "CMD|10|", "CMD|01|...|"
    qbit::getArgs(str, index);
    // Bluetooth command type %type
    // type: (CmdType)(0 .. 10)
    qbit::getBluetoothCmdtype(type);
    // Car run type %type
    // type: (CarRunCmdType)(0 .. 8)
    qbit::getRunCarType(type);
    // Convert ultrasonic distance %data
    qbut::convertUltrasonic(data);
    // Convert temperature %data
    qbut::convertTemperature(data);
    // Convert light %data
    qbut::convertLight(data);
    // Convert battery %data
    qbut::convertBattery(data);
#endif

    qbit::onQbit_remote_ir_pressed((qbit::IRKEY)MICROBIT_EVT_ANY, onIR);
    qbit::onQbit_remote_no_ir(onNoIR);

    qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Green);
    qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Blue);

    for (int i = 0; true; i+=1)
    {
        qbit::showLight();
        uBit.sleep(10);

        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, i & 0x100 ? qbit::QbitRGBLight::Green : qbit::QbitRGBLight::Blue);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, i & 0x100 ? qbit::QbitRGBLight::Blue : qbit::QbitRGBLight::Green);
        qbit::setBrightness(i & 0xff);
        
#if 0
        if ((i & 0x3FF) == 0x000)
        {
            uBit.display.scroll(ManagedString("D:") + qbit::convertUltrasonic(qbit::Ultrasonic()));
        }
        else if ((i & 0x3FF) == 0x100)
        {
            uBit.display.scroll(ManagedString("T:") + qbit::convertTemperature(uBit.thermometer.getTemperature()));
        }
        else if ((i & 0x3FF) == 0x200)
        {
            uBit.display.scroll(ManagedString("L:") + qbit::convertLight(uBit.display.readLightLevel()));
        }
        else if ((i & 0x3FF) == 0x300)
        {
            uBit.display.scroll(ManagedString("B:") + qbit::convertBattery(qbit::getBatVoltage()));
        }
#endif
    }
}
