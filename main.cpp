/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"
#include "MicroBitUARTService.h"
#include "Qbit-api.h"


#define SPEED_GO    60
#define SPEED_TURN  30


MicroBit uBit;

static MicroBitUARTService *uBut_uart = NULL;
bool connectedBT = false;

#define NO_IRKEY ((qbit::IRKEY)0)
static qbit::IRKEY activeIRkey = NO_IRKEY;
static bool needIrStop = false;

static bool doCarRun(qbit::CarRunCmdType type)
{
    switch (type)
    {
    case qbit::RunCmdType_STOP:
        qbit::setQbitRunSpeed(0, qbit::OrientionType_STOP);
        qbit::clearLight();
        uBit.display.clear();
        break;

    case qbit::RunCmdType_GO_AHEAD:
        qbit::setQbitRunSpeed(SPEED_GO, qbit::OrientionType_GO_AHEAD);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::White);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::White);
        qbit::showLight();
        uBit.display.print("^");
        break;

    case qbit::RunCmdType_GO_BACK:
        qbit::setQbitRunSpeed(SPEED_GO, qbit::OrientionType_GO_BACK);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Red);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Red);
        qbit::showLight();
        uBit.display.print("-");
        break;

    case qbit::RunCmdType_TURN_LEFT:
        qbit::setQbitRunSpeed(SPEED_TURN, qbit::OrientionType_TURN_LEFT);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Orange);
        qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light2, 0);
        qbit::showLight();
        uBit.display.print(">");
        break;

    case qbit::RunCmdType_TURN_RIGHT:
        qbit::setQbitRunSpeed(SPEED_TURN, qbit::OrientionType_TURN_RIGHT);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Orange);
        qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light1, 0);
        qbit::showLight();
        uBit.display.print("<");
        break;

    case qbit::RunCmdType_GO_AHEAD_SLOW:
        qbit::setQbitRunSpeed(SPEED_GO / 2, qbit::OrientionType_GO_AHEAD);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::White);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::White);
        qbit::showLight();
        uBit.display.print(".");
        break;

    case qbit::RunCmdType_TURN_LEFT_SLOW:
        qbit::setQbitRunSpeed(SPEED_TURN / 2, qbit::OrientionType_TURN_LEFT);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Red);
        qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light2, 0);
        qbit::showLight();
        uBit.display.print(">");
        break;

    case qbit::RunCmdType_TURN_RIGHT_SLOW:
        qbit::setQbitRunSpeed(SPEED_TURN / 2, qbit::OrientionType_TURN_RIGHT);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Red);
        qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light1, 0);
        qbit::showLight();
        uBit.display.print("<");
        break;

    default:
        return false;
    }

    return true;
}

static void onIR(MicroBitEvent e)
{
    activeIRkey = (qbit::IRKEY)e.value;
    switch (activeIRkey)
    {
    // Arrows - move
    case qbit::UP:
        needIrStop = doCarRun(qbit::RunCmdType_GO_AHEAD);
        break;

    case qbit::DOWN:
        needIrStop = doCarRun(qbit::RunCmdType_GO_BACK);
        break;

    case qbit::LEFT:
        needIrStop = doCarRun(qbit::RunCmdType_TURN_LEFT);
        break;

    case qbit::RIGHT:
        needIrStop = doCarRun(qbit::RunCmdType_TURN_RIGHT);
        break;

    // A(red) - Turn balance off
    case qbit::A:
        uBit.display.print('x');
        qbit::clearLight();
        qbit::setQbitRun(qbit::RunType_STOP);
        break;

    // B - Obstacle sensor
    case qbit::B:
        {
            uBit.display.clear();

            MicroBitImage image(uBit.display.image.getWidth(), uBit.display.image.getHeight());
            image.print('_');
            uBit.display.print(image);
            // Combine '-' with '<' and/or '>'
            if (qbit::obstacleSensor(qbit::SENSOR1_OBSTACLE))
            {
                image.print('>');
                uBit.display.printAsync(image, 0, 0, true);
            }
            if (qbit::obstacleSensor(qbit::SENSOR2_OBSTACLE))
            {
                image.print('<');
                uBit.display.printAsync(image, 0, 0, true);
            }
        }
        break;

    // C(green) - Turn balance on
    case qbit::C:
        qbit::setQbitRun(qbit::RunType_RUN);
        uBit.display.scrollAsync(ManagedString("V:") + qbit::getBatVoltage());
        break;

    // D - Distance sensor
    case qbit::D:
        uBit.display.scrollAsync(ManagedString("D:") + qbit::Ultrasonic());
        break;

    // 0-3 - RGB LED brightness
    case qbit::R0:
        qbit::setBrightness(10);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Green);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Green);
        qbit::showLight();
        break;

    case qbit::R1:
        qbit::setBrightness(100);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Green);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Green);
        qbit::showLight();
        break;

    case qbit::R2:
        qbit::setBrightness(200);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Green);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Green);
        qbit::showLight();
        break;

    case qbit::R3:
        qbit::setBrightness(255);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Green);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Green);
        qbit::showLight();
        break;

    // 4 - Carrier mode
    case qbit::R4:
        qbit::qbit_carrier();
        uBit.display.scrollAsync("Carrier");
        break;

    // 5 - Set balance angle
    case qbit::R5:
        qbit::setBLAngle();
        uBit.display.scrollAsync("setBLAngle");
        break;

    // 6, 7 - Toggle Ext1, Ext2
    case qbit::R6:
    case qbit::R7:
        {
            qbit::Exts ext = e.value == qbit::R6 ? qbit::Exts_Ext1 : qbit::Exts_Ext2;
            qbit::pinIOStatus iostatus = qbit::readExtsIODigital(ext) ? qbit::pinIOStatus_Low : qbit::pinIOStatus_Hight;
            qbit::setExtsIO(ext, iostatus);
            uBit.display.scrollAsync(ManagedString("E") + ext + ":" + iostatus);
        }
        break;

    // 8 - Read Ext1 analog
    case qbit::R8:
        uBit.display.scrollAsync(ManagedString("E1:") + qbit::readExt1Analog());
        break;
    }
}

static void onNoIR(MicroBitEvent e)
{
    if (needIrStop)
    {
        doCarRun(qbit::RunCmdType_STOP);
        needIrStop = false;
    }
    activeIRkey = NO_IRKEY;
}

static void onConnected(MicroBitEvent)
{
    uBit.display.scroll("C");
    connectedBT = true;
}

static void onDisconnected(MicroBitEvent)
{
    uBit.display.scroll("D");
    connectedBT = false;
}

static void onUartDelimMatch(MicroBitEvent e)
{
    ManagedString cmdStr = uBut_uart->readUntil("$", ASYNC);

    qbit::CmdType cmdType = (qbit::CmdType)qbit::analyzeBluetoothCmd(cmdStr);
    switch (cmdType)
    {
    case qbit::CmdType_NO_COMMAND:
        break;

    case qbit::CmdType_CAR_RUN:
        {
            int type = qbit::getArgs(cmdStr, 1);
            if (doCarRun((qbit::CarRunCmdType)type))
            {
                uBut_uart->send(cmdStr + "$");
            }
        }
        break;

    case qbit::CmdType_SERVO:
        //TODO:
        break;

    case qbit::CmdType_ULTRASONIC:
        uBut_uart->send(qbit::convertUltrasonic(qbit::Ultrasonic()));
        break;

    case qbit::CmdType_TEMPERATURE:
        uBut_uart->send(qbit::convertTemperature(uBit.thermometer.getTemperature()));
        break;

    case qbit::CmdType_SOUND:
        //TODO:
        break;

    case qbit::CmdType_LIGHT:
        uBut_uart->send(qbit::convertLight(uBit.display.readLightLevel()));
        break;

    case qbit::CmdType_BAT:
        uBut_uart->send(qbit::convertBattery(qbit::getBatVoltage()));
        break;

    case qbit::CmdType_RGB_LIGHT:
        {
            int arg1 = qbit::getArgs(cmdStr, 1);
            int arg2 = qbit::getArgs(cmdStr, 1);
            qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light1, arg1);
            qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light2, arg2);
            qbit::showLight();
//            uBut_uart->send(cmdStr + "$");
        }
        break;

    case qbit::CmdType_DIDI:
        //TODO:
        break;

    case qbit::CmdType_VERSION:
        uBut_uart->send(cmdStr.substring(0, 7) + "22|$");
        break;
    }
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    // Insert your code here!
    uBit.display.scroll("IR+BT");

    // Bluetooth UART service
    uBut_uart = new MicroBitUARTService(*uBit.ble, 32, 32);
    if (uBut_uart)
    {
        uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
        uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
        uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, onUartDelimMatch);
        uBut_uart->eventOn("$");
    }

    // Qbit
    qbit::qbitInit(&uBit.messageBus, &uBit.serial, &uBit.io);

    qbit::onQbit_remote_ir_pressed((qbit::IRKEY)MICROBIT_EVT_ANY, onIR);
    qbit::onQbit_remote_no_ir(onNoIR);

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}
