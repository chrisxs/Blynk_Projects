#include <BlynkSimpleEsp8266.h>

char auth[] = "";                       // Blynk Token
char ssid[] = "";                       // WiFi名称
char pass[] = "";                       // WiFi密码
char blynk_server[] = "";               // Blynk服务器路径
int blynk_port = 8080;                  // Blynk端口号
char ntp_server[] = "time.windows.com"; // NTP服务器

void sendSensor(); // 发送传感器数据函数

BlynkTimer timer;

#define DHTPIN D2
#define DHTTYPE DHT22

BLYNK_CONNECTED()
{
    Blynk.syncVirtual(V5); // 在连接 Blynk 服务器后同步虚拟引脚 V5
}