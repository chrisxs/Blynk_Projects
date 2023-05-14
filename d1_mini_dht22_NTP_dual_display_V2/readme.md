# D1 Mini（ESP8266）DHT22双显示屏时钟温度计 V2

基于[MiniWiFiManager](https://github.com/chrisxs/Blynk_Projects/tree/main/MiniWiFiManager_Blynk_Version)写的程序，Web界面功能：OTA、AP配网、Blynk设置。不需要必须依赖Blynk才能运行，可以独立作为连接WiFi经过NTP校时后显示用。

## 使用方法

## 使用方法

1. 第一次开机先做一次恢复出厂，清空flash，防止如果系统不断崩溃并且无法上传新代码，清除不再需要的数据和其他应用程序。这里用esptool工具：  
   1.1. `pip install esptool`，安装`esptool`  
   1.2. `python -m esptool`测试运行  
   1.3. 把ESP8266或者32连接到你的电脑进入flash模式（按不同板子操作不同，有些板子不需要如D1 Mini，具体百度）  
   1.4. 按自己板子类型运行：  
    ESP8266：`python -m esptool --chip esp8266 erase_flash`  
    ESP32：`python -m esptool --chip esp32 erase_flash`  
   1.5. 留意提示，等待重启



## WiFi配置使用方法

1. 写入程序后会新建一个名为：`双显示屏时钟-DEMO`的WiFi热点

2. 进入该热点后配置WiFi即可

3. 预览：

![](https://github.com/chrisxs/Arduino_Cloud/blob/main/MiniWiFiManager/demo.png)

4. 连接过WiFi后，随时可以更改WiFi连接信息。

## OTA方法：

在AP模式或者在连接WiFi后，根据页面提示，在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件上传即可，也可以进入`ip:8266/update`

## Blynk配置方法

1. 首选连接好WiFi，点击`已连接WiFi的，点击此处配置Blynk`（AP模式时候不可用,但可以进入`ip:8080`）

2. 浏览器地址栏输入本设备的`ip:8080`

3. 按照指示做即可

**已保存的Blynk的信息不会被删除隐藏，用于方便校对信息和排查**

### 以上页面除了OTA页面外，都需要账号密码验证，默认账号名称和密码均为：`admin`，可以通过修改文件更改密码,不需要密码可以直接注释掉：
- 修改`wifi_setting.h`中的：

```
{
  // 获取用户名和密码
  const char *http_username = "admin";
  const char *http_password = "admin";

  // 检查是否提供了用户名和密码
  if (!server.authenticate(http_username, http_password))
  {
    // 如果没有提供用户名和密码，则发送身份验证失败的响应
    server.requestAuthentication();
    return;
  }
  ```

- 修改`blynk_config.h`中的:
```
  // 获取用户名和密码
  const char *http_username = "admin";
  const char *http_password = "admin";

  // 检查是否提供了用户名和密码
  if (!blynk_config_server.authenticate(http_username, http_password))
  {
    // 如果没有提供用户名和密码，则发送身份验证失败的响应
    blynk_config_server.requestAuthentication();
    return;
  }
  ```

## 使用库：
```
d1_mini dependencies...
Platform espressif8266 @ 4.2.0 (required: espressif8266)
├── framework-arduinoespressif8266 @ 3.30102.0 (required: platformio/framework-arduinoespressif8266 @ ~3.30102.0)
├── tool-esptool @ 1.413.0 (required: platformio/tool-esptool @ <2)
├── tool-esptoolpy @ 1.30000.201119 (required: platformio/tool-esptoolpy @ ~1.30000.0)
├── tool-mklittlefs @ 1.203.210628 (required: platformio/tool-mklittlefs @ ~1.203.0)
├── tool-mkspiffs @ 1.200.0 (required: platformio/tool-mkspiffs @ ~1.200.0)
└── toolchain-xtensa @ 2.100300.220621 (required: platformio/toolchain-xtensa @ ~2.100300.0)

Libraries
├── Adafruit Unified Sensor @ 1.1.9 (required: adafruit/Adafruit Unified Sensor @ 1.1.9)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk @ 1.2.0)
├── DHT sensor library @ 1.4.4 (required: adafruit/DHT sensor library @ 1.4.4)
├── ESP8266 and ESP32 OLED driver for SSD1306 displays @ 4.4.0 (required: thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays 
@ 4.4.0)
└── ElegantOTA @ 2.2.9 (required: ayushsharma82/ElegantOTA @ 2.2.9)
```
## 更新日志：
2023年5月13日
- 加入web后台诊断界面，进入`ip/dht22`可以查看诊断信息界面
---
2023年5月6日
- 增加Web页面访问密码功能
---
2023年5月5日
- 上传代码
---
