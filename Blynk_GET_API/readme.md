# 使用使用ESP8266对Blynk服务器送请求，通过反馈的JSON内容再显示在ESP8266上

1. 上传代码后第一次开机先做一次恢复设置：

    1.1. 开机前按着按钮，通电

    1.2. 出现带有WiFi图标的提示后，等待重置即可

**如果仍然开不了机，并且串口查看信息是不断重复开机码的，可以试试以下代码，清理eeprom。（之前用过WiFiManager没清理干净WiFi记录的可能会有这情况）**

```
/*
EEPROM Clear

Sets all of the bytes of the EEPROM to 0.
This example code is in the public domain.

*/

#include <EEPROM.h>

void setup() {
EEPROM.begin(512);
// write a 0 to all 512 bytes of the EEPROM
for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }

// turn the LED on when we're done
pinMode(13, OUTPUT);
digitalWrite(13, HIGH);
EEPROM.end();
}

void loop() {}
```

4. 用浏览器验证API，地址栏输入：`https://你的域名:blynk端口号/你的设备token/get/v0`,成功的话会返回V0上的数值如：`["25.870"]`

5. 完成以上操作后，开机，打开手机可以看见一个名为`Get_Blynk_API`的热点，点击config wifi进入后按着项目填写，然后按save即可。

6. 配置好后，会出现提示，等待重启即可。

7. 程序按轮流形式读取api，代码预留4个虚拟引脚，即V0-V4位依次读取，如果你的项目中没有这么多引脚将不会显示不存在的引脚，你也可以按需要修改代码中的引脚。

## 使用库：
- thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 - displays @ ^4.3.0
- tzapu/WiFiManager@0.16.0
- bblanchon/ArduinoJson@6.17.3

## 更新日志：

2023年3月22日
- 增加OLED提示的显示代码
---


2023年3月20日
- 改成使用WiFiManager，使用AP模式连接ESP8266，输入相关项目和选择连接WiFi

- Blynk的可选填入项目：
    - 服务器URL
    - 服务器端口
    - 密钥

- 加入基于IDE的OTA功能（准备弃用）

- 精简部分代码

---

2023年3月19日
- 重新上传
