#ifndef _plantmeasurement_h_
#define _plantmeasurement_h_

#include <Arduino.h>

#ifdef __cplusplus
extern "C"{
#endif

/* 
 * This class holds all measurement data of one measurement iteration
 */
class PlantData
{
public:
    float     luminosity = 0;
    float     humidity = 0;
    float     temperature =0;
    int       sm = 0;
    const int8_t LENGTH = 20;
    void lora_message(char* outStr);
    void logger_message(char* outStr);

};


/* 
 * This class is responsible for the measurements of the plant
 */
class PlantMeasurement
{
public:
    void init();
    void measure(PlantData *data);
    bool status();
    void lora_status_message(char* outStr);
    void logger_status_message(char* outStr);
private:
    bool loggingEnabled = true;
    void readSI7021(PlantData *data);
    void readSM(PlantData *data);
    void readBH1750(PlantData *data);
    bool initSI7021();
    bool initSM();
    bool initBH1750();
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif