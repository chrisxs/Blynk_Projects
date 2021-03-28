/////传感器/////
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MAX44009.h>

MAX44009 light;
Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1006)

void sendSensor()
{
    float t = bme.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0F;
    float l = light.get_lux() + 110;
    float m = bme.readAltitude(SEALEVELPRESSURE_HPA);

    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
    Blynk.virtualWrite(V2, p);
    Blynk.virtualWrite(V3, l);
    Blynk.virtualWrite(V4, m);
}