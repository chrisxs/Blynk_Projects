/////设置舵机在Blynk中的滑动条虚拟引脚/////
BLYNK_WRITE(V1)
{
  int state = param.asInt();
  servo1.write(param.asInt());
}

BLYNK_WRITE(V2)
{
  int state = param.asInt();
  servo2.write(param.asInt());
}

BLYNK_WRITE(V3)
{
  int state = param.asInt();
  servo3.write(param.asInt());
}

BLYNK_WRITE(V4)
{
  int state = param.asInt();
  servo4.write(param.asInt());
}

BLYNK_WRITE(V5)
{
  int state = param.asInt();
  servo5.write(state);
}

BLYNK_WRITE(V6)
{
  int state = param.asInt();
  servo6.write(state);
}