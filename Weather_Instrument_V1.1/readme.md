# D1 Mini（ESP8266）4合1气象采集仪 V1.2
基于MiniWiFiManager重做，使用更方便简单。

## 使用传感器
- 温湿度：si7201
- 光照度：bh1750fvi
- 大气压：bmp180

## 使用方法
第一次开机先做一次恢复出厂，清空flash，防止如果系统不断崩溃并且无法上传新代码，清除不再需要的数据和其他应用程序。这里用esptool工具：  
   1.1. `pip install esptool`，安装`esptool`  
   1.2. `python -m esptool`测试运行  
   1.3. 把ESP8266或者32连接到你的电脑进入flash模式（按不同板子操作不同，有些板子不需要如D1 Mini，具体百度）  
   1.4. 按自己板子类型运行：  
    ESP8266：`python -m esptool --chip esp8266 erase_flash`  
    ESP32：`python -m esptool --chip esp32 erase_flash`  
   1.5. 留意提示，等待重启




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
├── Adafruit BMP085 Library @ 1.2.2 (required: adafruit/Adafruit BMP085 Library @ ^1.2.2)
│   ├── Adafruit BusIO @ 1.14.1 (required: Adafruit BusIO)
│   └── Adafruit Unified Sensor @ 1.1.9 (required: Adafruit Unified Sensor)
├── Adafruit Si7021 Library @ 1.5.1 (required: adafruit/Adafruit Si7021 Library @ ^1.5.1)
├── BH1750FVI @ 1.1.1 (required: PeterEmbedded/BH1750FVI)
├── Blynk @ 1.2.0 (required: blynkkk/Blynk @ ^1.2.0)
└── ElegantOTA @ 2.2.9 (required: ayushsharma82/ElegantOTA)
```
## 更新日志：
2023年5月21日
上传代码

