/////传感器/////
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MAX44009.h>


MAX44009 light;
Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1020)

void sendSensor()
{
    float t = bme.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0F;
    float l = light.get_lux();

    Blynk.virtualWrite(V0, h);
    Blynk.virtualWrite(V1, t);
    Blynk.virtualWrite(V2, p);
    Blynk.virtualWrite(V3, l);
}