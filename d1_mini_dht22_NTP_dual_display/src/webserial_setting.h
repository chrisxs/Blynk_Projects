/*#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>*/
#include <WebSerial.h>

void recvMsg(uint8_t *data, size_t len); // 声明一个函数 recvMsg，用于接收数据

AsyncWebServer serialserver(340); // 创建一个名为 serialserver 的异步 Web 服务器，端口为 340

/* 反馈WebSerial收到的信息 */
void recvMsg(uint8_t *data, size_t len) // 接收数据的函数
{
    WebSerial.println("接收到数据..."); // 打印一条提示信息，表示已接收到数据
    String d = "";                      // 创建一个空字符串变量 d，用于存储接收到的数据
    for (int i = 0; i < len; i++)       // 循环读取接收到的数据
    {
        d += char(data[i]); // 将接收到的字节数据转换为字符并拼接到字符串变量 d 中
    }
    WebSerial.println(d); // 打印接收到的数据
}