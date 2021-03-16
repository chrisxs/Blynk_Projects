#include <Servo.h>

Servo servo1, servo2, servo3, servo4, servo5, servo6;

void AtatchServo()
{
    servo1.attach(D0);
    servo2.attach(D3);
    servo3.attach(D4);
    servo4.attach(D6);
    servo5.attach(D7);
    servo6.attach(D8);
}

void ServoDefaultPOS()
{
    servo1.write(100);
    servo2.write(100);
    servo3.write(100);
    servo4.write(100);
    servo5.write(100);
    servo6.write(100);
}