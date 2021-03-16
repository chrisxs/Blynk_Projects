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
    int pos = 90;
    servo1.write(pos);
    servo2.write(pos);
    servo3.write(pos);
    servo4.write(pos);
    servo5.write(pos);
    servo6.write(pos);
}

void Pos_0()
{
    int pos = 90;
    servo1.write(pos);
    delay(1000);
    servo2.write(pos);
    delay(1000);
    servo3.write(pos);
    delay(1000);
    servo4.write(pos);
    delay(1000);
    servo5.write(pos);
    delay(1000);
    servo6.write(pos);
    delay(1000);

    Blynk.virtualWrite(V1, pos);
    Blynk.virtualWrite(V2, pos);
    Blynk.virtualWrite(V3, pos);
    Blynk.virtualWrite(V4, pos);
    Blynk.virtualWrite(V5, pos);
    Blynk.virtualWrite(V6, pos);
}

void Pos_1()
{
    int pos1 = 145;
    int pos2 = 110;
    int pos3 = 125;
    int pos4 = 149;
    int pos5 = 90;
    int pos6 = 10;
    servo1.write(pos1);
    delay(500);
    servo2.write(pos2);
    delay(500);
    servo3.write(pos3);
    delay(500);
    servo4.write(pos4);
    delay(500);
    servo5.write(pos5);
    delay(500);
    servo6.write(pos6);
    delay(500);

    Blynk.virtualWrite(V1, pos1);
    Blynk.virtualWrite(V2, pos2);
    Blynk.virtualWrite(V3, pos3);
    Blynk.virtualWrite(V4, pos4);
    Blynk.virtualWrite(V5, pos5);
    Blynk.virtualWrite(V6, pos6);
}

void Pos_2()
{
    int pos1 = 145;
    int pos2 = 70;
    int pos3 = 125;
    int pos4 = 150;
    int pos5 = 160;
    int pos6 = 100;
    servo1.write(pos1);
    delay(500);
    servo2.write(pos2);
    delay(500);
    servo3.write(pos3);
    delay(500);
    servo4.write(pos4);
    delay(500);
    servo5.write(pos5);
    delay(500);
    servo6.write(pos6);
    delay(500);

    Blynk.virtualWrite(V1, pos1);
    Blynk.virtualWrite(V2, pos2);
    Blynk.virtualWrite(V3, pos3);
    Blynk.virtualWrite(V4, pos4);
    Blynk.virtualWrite(V5, pos5);
    Blynk.virtualWrite(V6, pos6);
}

void Pos_3()
{
    int pos1 = 90;
    int pos2 = 70;
    int pos3 = 90;
    int pos4 = 160;
    int pos5 = 90;
    int pos6 = 10;
    servo1.write(pos1);
    delay(500);
    servo2.write(pos2);
    delay(500);
    servo3.write(pos3);
    delay(500);
    servo4.write(pos4);
    delay(500);
    servo5.write(pos5);
    delay(500);
    servo6.write(pos6);
    delay(500);

    Blynk.virtualWrite(V1, pos1);
    Blynk.virtualWrite(V2, pos2);
    Blynk.virtualWrite(V3, pos3);
    Blynk.virtualWrite(V4, pos4);
    Blynk.virtualWrite(V5, pos5);
    Blynk.virtualWrite(V6, pos6);
}