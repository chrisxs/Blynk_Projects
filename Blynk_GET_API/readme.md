# 使用使用ESP8266对Blynk服务器送请求，通过反馈的JSON内容再显示在ESP8266上

1. 上传代码后第一次开机先做一次恢复设置：

    1.1. 开机前按着按钮，通电

    1.2. 出现带有WiFi图标的提示后，等待重置即可

**如果仍然开不了机，并且串口查看信息是不断重复开机码的，可以试试以：**
1. 上传代码后第一次开机先做一次恢复出厂，清空flash，防止如果系统不断崩溃并且无法上传新代码，清除不再需要的数据和其他应用程序。这里用esptool工具：  
   1.1. `pip install esptool`，安装`esptool`  
   1.2. `python -m esptool`测试运行  
   1.3. 把ESP8266或者32连接到你的电脑进入flash模式（按不同板子操作不同，有些板子不需要如D1 Mini，具体百度）  
   1.4. 按自己板子类型运行：  
    ESP8266：`python -m esptool --chip esp8266 erase_flash`  
    ESP32：`python -m esptool --chip esp32 erase_flash`  
   1.5. 留意提示，等待重启


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
- 修改注释
- 精简代码
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
