# MiniWiFiManager ESP8266WiFi配置程序（Blynk版本）

尽量基于Arduino原生不依赖第三方库的WiFiManager，代码简单。比起WiFiManager，优点是轻便快速，可以随时设置WiFi连接。OTA功能未完成，暂时用ElegantOTA

## 使用方法

1. 写入程序后会新建一个名为：`ESP8266AP`的WiFi热点

2. 进入该热点后配置WiFi即可

3. 预览：

![](https://github.com/chrisxs/Arduino_Cloud/blob/main/MiniWiFiManager/demo.png)

## OTA方法：

在AP模式或者在连接WiFi后，根据页面提示，在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件上传即可

## Blynk配置方法

1. 首选连接好WiFi（AP模式时候不可用）

2. 浏览器地址栏输入本设备的`ip:8080`

3. 按照指示做即可

## 使用库

```
d1_mini dependencies...
Platform espressif8266 @ 2.6.3 (required: espressif8266)
├── framework-arduinoespressif8266 @ 3.20704.0 (required: platformio/framework-arduinoespressif8266 @ ~3.20704.0)
├── tool-esptool @ 1.413.0 (required: platformio/tool-esptool @ <2)
├── tool-esptoolpy @ 1.30000.201119 (required: platformio/tool-esptoolpy @ ~1.30000.0)
├── tool-mklittlefs @ 1.203.210628 (required: platformio/tool-mklittlefs @ ~1.203.0)  
├── tool-mkspiffs @ 1.200.0 (required: platformio/tool-mkspiffs @ ~1.200.0)
└── toolchain-xtensa @ 2.40802.200502 (required: platformio/toolchain-xtensa @ ~2.40802.0)

Libraries
├── Blynk @ 1.2.0 (required: blynkkk/Blynk @ ^1.2.0)
└── ElegantOTA @ 2.2.9 (required: ayushsharma82/ElegantOTA)
```

## 更新日志
---
2023年3月28日
- 上传