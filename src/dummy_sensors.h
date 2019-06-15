#ifndef _dummymeasurement_h_
#define _dummymeasurement_h_

#include <Arduino.h>

#ifdef __cplusplus
extern "C"{
#endif

/* 
 * This class holds all measurement data of one measurement iteration
 */
class DummyData
{
public:
    float     dummy = 0;
    void lora_message(char* outStr);
    void logger_message(char* outStr);
};


/* 
 * This class is responsible for the measurements of the plant
 */
class DummyMeasurement
{
public:
    void init();
    void measure(DummyData *data);
private:
    bool loggingEnabled = true;
    void readDummy(DummyData *data);
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif