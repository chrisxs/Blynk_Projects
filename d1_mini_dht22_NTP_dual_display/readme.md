# D1 Mini（ESP8266）DHT22双显示屏时钟温度计

## 使用方法
 1. 第一次开机先做一次恢复出厂，清空flash，防止如果系统不断崩溃并且无法上传新代码，清除不再需要的数据和其他应用程序。这里用esptool工具：  
   1.1. `pip install esptool`，安装`esptool`  
   1.2. `python -m esptool`测试运行  
   1.3. 把ESP8266或者32连接到你的电脑进入flash模式（按不同板子操作不同，有些板子不需要如D1 Mini，具体百度）  
   1.4. 按自己板子类型运行：  
    ESP8266：`python -m esptool --chip esp8266 erase_flash`  
    ESP32：`python -m esptool --chip esp32 erase_flash`  
   1.5. 留意提示，等待重启

3. 在代码中分别设置好以下内容：
```
char auth[] = "你的Blynk token";
char ssid[] = "你的WiFi_SSID";
char pass[] = "你的WiFi密码";
char blynk_server[] = "Blynk服务器路径";
int blynk_port = 8080; //Blynk端口号
char ntp_server[] = "time.windows.com"; //NTP服务器
```

## OTA方法：
1. 在代码上传过并成功连接WiFi后，编译程序
2. 在显示屏或者串口查看本机IP，浏览器输入该IP地址：`ip地址:8266/update`,选择`Firmware`然后选择文件
3. 然后在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件即可

## WEB串口：
- 在显示屏或者串口查看本机IP，浏览器输入该IP地址：ip地址:340/webserial
- 串口文本框中输入`debug`，然后点击`send`，即可显示WiFi_SSID等相关信息

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
├── Adafruit Unified Sensor @ 1.1.9 (required: adafruit/Adafruit Unified Sensor)
├── AsyncElegantOTA @ 2.2.7 (required: ayushsharma82/AsyncElegantOTA)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk)
├── DHT sensor library @ 1.4.4 (required: adafruit/DHT sensor library)
├── ESP Async WebServer @ 1.2.3 (required: ESP Async WebServer)
│   ├── AsyncTCP @ 1.1.1 (required: AsyncTCP)
│   └── ESPAsyncTCP @ 1.2.2 (required: ESPAsyncTCP)
├── ESP8266 and ESP32 OLED driver for SSD1306 displays @ 4.4.0 (required: thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays)
└── WebSerial @ 1.3.0 (required: ayushsharma82/WebSerial)
```
## 更新日志：
2023年3月28日
- 优化代码
---
2023年3月25日
- 增加WEB OTA功能
- 增加Web串口功能
---
2023年3月24日
- 上传代码
- 修改注释
- 改用web方式OTA
---
