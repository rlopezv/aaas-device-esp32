#ifndef _weathermeasurement_h_
#define _weathermeasurement_h_

#include <Arduino.h>

#ifdef __cplusplus
extern "C"{
#endif

/* 
 * This class holds all measurement data of one measurement iteration
 */
class WeatherData
{
public:
    float     temperature = 0;
    float     preasure = 0;
    float     luminosity = 0;
    float     uv = 0;
    float     ir = 0;
    float     humidity = 0;
    int       rain = 0;
    void lora_message(char* outStr);
    void logger_message(char* outStr);
};


/* 
 * This class is responsible for the measurements of the plant
 */
class WeatherMeasurement
{
public:
    void init();
    void measure(WeatherData *data);
    bool status();
    void lora_status_message(char* outStr);
    void logger_status_message(char* outStr);
private:
    bool loggingEnabled = true;
    void readBME280(WeatherData *data);
    void readRainDetector(WeatherData *data);
    void readSI1145(WeatherData *data);
    bool initBME280();
    bool initRainDetector();
    bool initSI1145();
 
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif