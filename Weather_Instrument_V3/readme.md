# D1 Mini（ESP8266）4合1气象采集仪 V3

## 使用传感器
- 温湿度：SHT31
- 光照度：BH1750FVI
- 大气压：BMP085

## 使用方法
 1. 第一次开机先做一次恢复出厂，清空flash，防止如果系统不断崩溃并且无法上传新代码，清除不再需要的数据和其他应用程序。这里用esptool工具：  
   1.1. `pip install esptool`，安装`esptool`  
   1.2. `python -m esptool`测试运行  
   1.3. 把ESP8266或者32连接到你的电脑进入flash模式（按不同板子操作不同，有些板子不需要如D1 Mini，具体百度）  
   1.4. 按自己板子类型运行：  
    ESP8266：`python -m esptool --chip esp8266 erase_flash`  
    ESP32：`python -m esptool --chip esp32 erase_flash`  
   1.5. 留意提示，等待重启

2. 按下重置按钮`D5`，板载LED会亮起并闪耀，等待约30秒
3. 用手机或者电脑连接名为：`4合1气象仪-demo`的WiFi热点，配置好里面的相关信息即可

**目前~~Web OTA和~~Web 串口会跟WiFiManager有冲突，所以未加入**
## OTA方法：
1. 在代码上传过并成功连接WiFi后，编译程序
2. 在显示屏或者串口查看本机IP，浏览器输入该IP地址：`ip地址:8266/update`,选择`Firmware`然后选择文件
3. 然后在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件即可

## WEB串口：


## 使用库：
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
├── Adafruit BMP085 Library @ 1.2.2 (required: adafruit/Adafruit BMP085 Library)
│   ├── Adafruit BusIO @ 1.14.1 (required: Adafruit BusIO)
│   └── Adafruit Unified Sensor @ 1.1.9 (required: Adafruit Unified Sensor)
├── ArduinoJson @ 6.21.0 (required: bblanchon/ArduinoJson)
├── BH1750FVI @ 1.1.1 (required: peterembedded/BH1750FVI)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk)
├── WEMOS SHT3x @ 1.0.0 (required: wemos/WEMOS SHT3x)
└── WiFiManager @ 0.16.0 (required: tzapu/WiFiManager)
```
## 更新日志：
2023年3月27日2
- 增加错误处理，防止在传感器松脱的情况下程序死机
---
2023年3月25日
- 统一从ArduinoIDE转移到PlatformIO


