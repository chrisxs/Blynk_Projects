# MiniWiFiManager ESP8266WiFi配置程序（Blynk版本）

尽量基于Arduino原生不依赖第三方库的WiFiManager，代码简单。比起WiFiManager，优点是轻便快速，可以随时设置WiFi连接。OTA功能未完成，暂时用ElegantOTA，页面全部默认账号密码为：`admin`

## 使用方法

1. 写入程序后会新建一个名为：`ESP8266AP`的WiFi热点

2. 进入该热点后配置WiFi即可

3. 预览：

![](https://github.com/chrisxs/Arduino_Cloud/blob/main/MiniWiFiManager/demo.png)

## OTA方法：

在AP模式或者在连接WiFi后，根据页面提示，在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件上传即可，也可以进入`ip:8266/update`）

## Blynk配置方法

1. 首选连接好WiFi，点击`已连接WiFi的，点击此处配置Blynk`（AP模式时候不可用,但可以进入`ip:8080`）

2. 浏览器地址栏输入本设备的`ip:8080`

3. 按照指示做即可

**已保存的Blynk的信息不会被删除隐藏，用于方便校对信息和排查**

### 以上页面除了OTA页面外，都需要账号密码验证，通过修改文件更改密码,不需要密码可以直接注释掉：
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
2023年5月6日
- 增加Web页面密码访问验证功能
---
2023年3月28日
- 上传