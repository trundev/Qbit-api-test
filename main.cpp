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
#include "Qbit-api.h"

MicroBit uBit;

static void onIR(MicroBitEvent e)
{
    switch (e.value)
    {
    case qbit::UP:
        qbit::setQbitRunSpeed(80, qbit::OrientionType_GO_AHEAD);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::White);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::White);
        qbit::showLight();
        uBit.display.print("^");
        break;

    case qbit::DOWN:
        qbit::setQbitRunSpeed(80, qbit::OrientionType_GO_BACK);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Red);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Red);
        qbit::showLight();
        uBit.display.print("-");
        break;

    case qbit::LEFT:
        qbit::setQbitRunSpeed(60, qbit::OrientionType_TURN_LEFT);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light1, qbit::QbitRGBLight::Orange);
        qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light2, 0);
        qbit::showLight();
        uBit.display.print(">");
        break;

    case qbit::RIGHT:
        qbit::setQbitRunSpeed(60, qbit::OrientionType_TURN_RIGHT);
        qbit::setPixelRGB(qbit::QbitRGBLight::Light2, qbit::QbitRGBLight::Orange);
        qbit::setPixelRGBArgs(qbit::QbitRGBLight::Light1, 0);
        qbit::showLight();
        uBit.display.print("<");
        break;

    case qbit::A:
        uBit.display.print('x');
        qbit::clearLight();
        qbit::setQbitRun(qbit::RunType_STOP);
        break;

    case qbit::C:
        uBit.display.scroll(ManagedString("V:") + qbit::getBatVoltage());
        qbit::setQbitRun(qbit::RunType_RUN);
        break;

    case qbit::D:
        uBit.display.scroll(ManagedString("D:") + qbit::Ultrasonic());
        break;

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
    }
}

static void onNoIR(MicroBitEvent e)
{
    qbit::setQbitRunSpeed(0, qbit::OrientionType_STOP);
    qbit::clearLight();
    uBit.display.clear();
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    // Insert your code here!
    uBit.display.scroll("IR");

    qbit::qbitInit(&uBit.messageBus, &uBit.serial, &uBit.io);

    qbit::onQbit_remote_ir_pressed((qbit::IRKEY)MICROBIT_EVT_ANY, onIR);
    qbit::onQbit_remote_no_ir(onNoIR);

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}
