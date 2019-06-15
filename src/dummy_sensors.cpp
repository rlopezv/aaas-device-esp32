#include "dummy_sensors.h"

#include <Wire.h>
#include <ttgo_dummy.h>

const float MAX = 0;
const float MIN = 100;

void DummyData::lora_message(char* outStr) 
{
    sprintf(outStr, "%d#%2.2f",
        SENSOR_TYPE,
        dummy
    );
}

void DummyData::logger_message(char* outStr) 
{
    sprintf(outStr, "%s[D:%2.2f]",
         SENSOR_CLASS,
         dummy
    );
}

void DummyMeasurement::init() 
{

    Wire.begin();

}

void DummyMeasurement::measure(DummyData *data)
{
    readDummy(data);
}

void DummyMeasurement::readDummy(DummyData *data) 
{
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    data->dummy = MIN + scale * ( MAX - MIN );
    Serial.print("Reading dummy value:");
    Serial.println(String(data->dummy));
}

