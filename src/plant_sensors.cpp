#include "plant_sensors.h"
#include "globals.h"
#include <Wire.h>
#include <ttgo_plant.h>
#include <BH1750.h>
#include "HTU21D.h"


#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_SM 34

#define ADD_SI7021 0x40
#define ADD_BH1750 0x23

BH1750 bh1750(ADD_BH1750);
HTU21D si7021;

const int AIR_VALUE = 3550;
const int WATER_VALUE = 1200;

bool si7021_status = true;
bool bh1750_status = true;
bool sm_status = true;

void PlantData::lora_message(char *outStr)
{
    sprintf(outStr, "%s#%d#%2.2f#%2.2f#%2.2f#%d",
            MESSAGE_TYPE_DATA,
            SENSOR_TYPE,
            luminosity,
            humidity,
            temperature,
            sm);
}

void PlantData::logger_message(char *outStr)
{
    sprintf(outStr, "%s[L(Lux):%2.2f,H(%%):%2.2f,T(ºC):%2.2f,SM:%d]",
            SENSOR_CLASS,
            luminosity,
            humidity,
            temperature,
            sm);
}

void PlantMeasurement::lora_status_message(char* outStr) 
{
    sprintf(outStr, "%s#%u#SI7021:%u#BI1750:%u#SM:%u",
        MESSAGE_TYPE_STATUS,
        SENSOR_TYPE,
        si7021_status,
        bh1750_status,
        sm_status
    );
}

void PlantMeasurement::logger_status_message(char* outStr) 
{
    sprintf(outStr, "%s[SI7021:%s,BI1750:%s,SM:%s]",
        SENSOR_CLASS,
        si7021_status?"true":"false",
        bh1750_status?"true":"false",
        sm_status?"true":"false"
    );
}

void PlantMeasurement::init()
{
    Wire.begin(PIN_SDA, PIN_SCL);
    sm_status = initSM();
    si7021_status = initSI7021();
    bh1750_status = initBH1750();
}

void PlantMeasurement::measure(PlantData *data)
{
    readSI7021(data);
    readSM(data);
    readBH1750(data);
}

bool PlantMeasurement::initSI7021()
{
    Serial.println("Init SI7021");
    return si7021.begin(PIN_SDA, PIN_SCL);

}

// bool PlantMeasurement::initSI7021()
// {
//     Serial.println("Init SI7021");
//     return true;
// }

bool PlantMeasurement::initSM()
{
    Serial.println("Init SM");
    pinMode(PIN_SM, INPUT);
    return true;
}

bool PlantMeasurement::initBH1750()
{
    Serial.println("Init BH1750");
    return bh1750.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
}

void PlantMeasurement::readSI7021(PlantData *data) {

  float humidity    = si7021.readCompensatedHumidity();
  float temperature = si7021.readTemperature();
  Serial.printf("T:%2.2f-H:%2.2f\r\n",temperature, humidity);
  data->humidity = (float) humidity;
  data->temperature = (float) temperature;

}

void PlantMeasurement::readSM(PlantData *data)
{
    Serial.println("Read SM");
   int bitVal = analogRead(PIN_SM);
    Serial.println(bitVal);
    int sm = 100 - map(bitVal, WATER_VALUE, AIR_VALUE, 0, 100);
    data->sm = sm;
    Serial.print("SM:");
    Serial.println(sm);
}

void PlantMeasurement::readBH1750(PlantData *data)
{
    Serial.println("Read bh1750");
    float lux = bh1750.readLightLevel();
    data->luminosity = lux;
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

}

bool PlantMeasurement::status()
{
    return sm_status && si7021_status && bh1750_status;

}

// Illuminance (lux)	Surfaces illuminated by
// 0.0001	Moonless, overcast night sky (starlight)
// 0.002	Moonless clear night sky with airglow
// 0.05–0.36	Full moon on a clear night[4]
// 3.4	Dark limit of civil twilight under a clear sky
// 20–50	Public areas with dark surroundings
// 50	Family living room lights
// 80	Office building hallway/toilet lighting
// 100	Very dark overcast day
// 320–500	Office lighting
// 400	Sunrise or sunset on a clear day.
// 1000	Overcast day; typical TV studio lighting
// 10,000–25,000	Full daylight (not direct sun)
// 32,000–100,000	Direct sunlight