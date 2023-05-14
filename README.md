# Blynk_Projects
 
**平常无聊做的Blynk项目,以后都集中归纳统一改用IDE为:PlatformIO，不再使用ArduinoIDE。文件夹里头没有完善readme说明的项目，都是有大BUG或者未完成的，慎用。**

# ESP8266、ESP32恢复出厂方法：
1. 上传代码后第一次开机先做一次恢复出厂，清空flash，防止如果系统不断崩溃并且无法上传新代码，清除不再需要的数据和其他应用程序。这里用esptool工具：  
   1.1. `pip install esptool`，安装`esptool`  
   1.2. `python -m esptool`测试运行  
   1.3. 把ESP8266或者32连接到你的电脑进入flash模式（按不同板子操作不同，有些板子不需要如D1 Mini，具体百度）  
   1.4. 按自己板子类型运行：  
    ESP8266：`python -m esptool --chip esp8266 erase_flash`  
    ESP32：`python -m esptool --chip esp32 erase_flash`  
   1.5. 留意提示，等待重启

