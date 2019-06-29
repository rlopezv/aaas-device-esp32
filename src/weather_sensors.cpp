#include "weather_sensors.h"
#include "globals.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <ttgo_weather.h>
#include <Adafruit_VEML6070.h>

#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_RAIN 34

#define ADD_BH1750 0x23
#define ADD_BME280 0x76

const int DRY_VALUE = 4095;
const int WATER_VALUE = 800;

Adafruit_BME280 bme280; // I2C
BH1750 bh1750p(ADD_BH1750);
Adafruit_VEML6070 veml6070 = Adafruit_VEML6070();

bool bme280_status = true;
bool veml6070_status = true;
bool bh1750p_status = true;
bool rain_status = true;

void WeatherData::lora_message(char* outStr) 
{
    sprintf(outStr, "%s#%u#%2.2f#%u#%2.2f#%2.2f#%2.2f#%u",
        MESSAGE_TYPE_DATA,
        SENSOR_TYPE,
        luminosity,
        uv,
        humidity,
        preasure,
        temperature,
        rain
    );
}

void WeatherData::logger_message(char* outStr) 
{
    sprintf(outStr, "%s[L(Lux):%2.2f,UV():%u,H(%%):%2.2f,P(mb):%2.2f,T(ºC):%2.2f,SM:%u]",
        SENSOR_CLASS,
        luminosity,
        uv,
        humidity,
        preasure,
        temperature,
        rain
    );
}

void WeatherMeasurement::lora_status_message(char* outStr) 
{
    sprintf(outStr, "%s#%d#BME280:%i#VEML6070:%i#BH1750:%i#RAIN:%i",
        MESSAGE_TYPE_STATUS,
        SENSOR_TYPE,
        bme280_status,
        veml6070_status,
        bh1750p_status,
        rain_status
    );
}

void WeatherMeasurement::logger_status_message(char* outStr) 
{
    sprintf(outStr, "%s[BME280:%d,VEML6070:%d,BH1750:%d,RAIN:%d]",
        SENSOR_CLASS,
        bme280_status,
        veml6070_status,
        bh1750p_status,
        rain_status
    );
}

void WeatherMeasurement::init() 
{
    Wire.begin();
    bme280_status = initBME280();
    bh1750p_status = initBH1750();
    rain_status = initRainDetector();
    veml6070_status = initVEML6070();
    Serial.printf("BME280:%i\r\n",bme280_status);
    Serial.printf("BH1750:%i\r\n",bh1750p_status);
    Serial.printf("VEML6070:%i\r\n",veml6070_status);
    Serial.printf("rain:%i\r\n",rain_status);

}

bool WeatherMeasurement::initBH1750()
{
    Serial.println("Init BH1750");
    return bh1750p.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
}

bool WeatherMeasurement::initVEML6070() 
{
  Serial.println("init VEML6070");
  veml6070.begin(VEML6070_1_T);
  return true;
}

bool WeatherMeasurement::initRainDetector() 
{
  Serial.println("init RainDetector");
    pinMode(PIN_RAIN, INPUT);
    return true;
}

bool WeatherMeasurement::initBME280() 
{
  Serial.println("init BME280");
  return bme280.begin(ADD_BME280);;
}

void WeatherMeasurement::measure(WeatherData *data)
{
    readBME280(data);
    readBH1750(data);
    readVEML6070(data);
    readRainDetector(data);
}

void WeatherMeasurement::readBME280(WeatherData *data) 
{

  float temperature,preasure, humidity;
  if (bme280_status) {
    temperature = bme280.readTemperature();
    preasure = bme280.readPressure() / 100.0F;
    humidity = bme280.readHumidity();
    Serial.printf("T:%2.2f,P:%2.2f, H:%2.2f",temperature, preasure, humidity);
        data->temperature= temperature;
        data->preasure=preasure;
        data->humidity = humidity;
  }  else {
    Serial.printf("Error in sensor");
  }
}

void WeatherMeasurement::readRainDetector(WeatherData *data) 
{
    Serial.println("readRainDetector");
    int bitVal = analogRead(PIN_RAIN);
    int rain = 100 - map(bitVal, WATER_VALUE, DRY_VALUE, 0, 100);
    data->rain = rain;
    Serial.print("Rain:");
    Serial.println(rain);
}

void WeatherMeasurement::readVEML6070(WeatherData *data) 
{
  Serial.println("readVEML6070");
  uint16_t uv =  veml6070.readUV();
  data->uv = uv;
  Serial.print("UV:");
  Serial.println(uv);
}

void WeatherMeasurement::readBH1750(WeatherData *data)
{
    Serial.println("Read bh1750");
    float lux = bh1750p.readLightLevel();
    data->luminosity = lux;
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

}

bool WeatherMeasurement::status() {
  return bme280_status && veml6070_status && bh1750p_status && rain_status;
} 



