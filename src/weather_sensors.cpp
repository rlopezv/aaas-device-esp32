#include "weather_sensors.h"
#include "globals.h"
#include <Wire.h>
#include <BMP280.h>
#include <ttgo_weather.h>

#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_RAIN 34

BMP280 bmp;

bool bme280_status = true;
bool si1145_status = true;
bool rain_status = true;

void WeatherData::lora_message(char* outStr) 
{
    sprintf(outStr, "%s#%d#%2.2f#%2.2f#%2.2f#%2.2f#%2.2f#%2.2f#%d",
        MESSAGE_TYPE_DATA,
        SENSOR_TYPE,
        luminosity,
        uv,
        ir,
        humidity,
        preasure,
        temperature,
        rain
    );
}

void WeatherData::logger_message(char* outStr) 
{
    sprintf(outStr, "%s[L(Lux):%2.2f,UV():%2.2f,IR():%2.2f,H(%%):%2.2f,P(mb):%2.2f,T(ºC):%2.2f,SM:%d]",
        SENSOR_CLASS,
        luminosity,
        uv,
        ir,
        humidity,
        preasure,
        temperature,
        rain
    );
}

void WeatherMeasurement::lora_status_message(char* outStr) 
{
    sprintf(outStr, "%s#BME280:%s#SI1145%s#RAIN:%s",
        MESSAGE_TYPE_STATUS,
        bme280_status?"true":"false",
        si1145_status?"true":"false",
        rain_status?"true":"false"
    );
}

void WeatherMeasurement::logger_status_message(char* outStr) 
{
    sprintf(outStr, "%s[BME280:%d,SI1145:%d,RAIN:%d",
        SENSOR_CLASS,
        bme280_status,
        si1145_status,
        rain_status
    );
}

void WeatherMeasurement::init() 
{
    Wire.begin();
    initBME280();
    initRainDetector();
    initSI1145();
}

bool WeatherMeasurement::initSI1145() 
{
  Serial.println("init SI1145");
    
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
  if(!bmp.begin(PIN_SDA,PIN_SCL)){
    Serial.println("BME init failed!");
    while(1);
  }
  else Serial.println("BME init success!");
  
  bmp.setOversampling(4);
}

void WeatherMeasurement::measure(WeatherData *data)
{
    readBME280(data);
    readRainDetector(data);
}

void WeatherMeasurement::readBME280(WeatherData *data) 
{
  Serial.println("read BME280");
  double tempCels,preasure;
  char result = bmp.startMeasurment();
 
  if(result!=0){
    delay(result);
    result = bmp.getTemperatureAndPressure(tempCels,preasure);
    
      if(result!=0)
      {
        Serial.printf("T:%2.2f P:%2.2f",tempCels, preasure);
        data->temperature= tempCels;
        data->preasure=preasure;
      }
      else {
        Serial.println("Error.");
      }
  }
  else {
    Serial.println("Error.");
  }
}

void WeatherMeasurement::readRainDetector(WeatherData *data) 
{
  Serial.println("readRainDetector");
    int bitVal = analogRead(PIN_RAIN);
    int rain = map(bitVal, 1800, 4056, 100, 0);
    data->rain = rain;
    Serial.print("Rain:");
    Serial.println(rain);
}

void WeatherMeasurement::readSI1145(WeatherData *data) 
{
  Serial.println("readRainDetector");
    int bitVal = analogRead(PIN_RAIN);
    int rain = map(bitVal, 1800, 4056, 100, 0);
    data->rain = rain;
    Serial.print("Rain:");
    Serial.println(rain);
}

bool WeatherMeasurement::status() {

} 



