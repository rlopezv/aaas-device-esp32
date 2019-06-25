# ESP32 - Device

This repository contains the program to be included in the TTGO LoRa v2.1.6 for AAAS.

The project has been developed used platfom.io using Ardunio framework.

It supports two monitoring devices that will communicate through LoRa:

 - Weather Station 
 - Plant Monitoring

## Uplink

The devices send two types of messages:

 - status, with included sensor status
 - data, with measured data

## message format

measuretype(status|data)#sensorType(1|2)#values(with ':' for splitting values)
### Weather:

    status#2#BME280:1#VEML6070:1#BH1750:1#RAIN:1
    data#2#1.67#0#39.58#939.66#28.01#0

### Plant:

    status#1#SI7021:1#BI1750:1#SM:1
	data#1#13.33#38.24#28.06#4
    
## Downlink
The devices support commands for changing the periods applied to sampling, status sending and data sending.

    sampling:value(in seconds)

    sending:value(in seconds)

    status:value(in seconds)



### Sample messages

WEATHER STATUS

    {
      "applicationID": "2",
      "applicationName": "application",
      "deviceName": "TTGO-WEATHER",
      "devEUI": "6d3512d0c41ef44d",
      "rxInfo": [
        {
          "gatewayID": "b827ebfffe9fadaa",
          "name": "gateway",
          "time": "2019-06-25T08:43:38.512447Z",
          "rssi": -43,
          "loRaSNR": 7,
          "location": {
            "latitude": 40.38486,
            "longitude": -3.60689,
            "altitude": 698
          }
        }
      ],
      "txInfo": {
        "frequency": 868500000,
        "dr": 5
      },
      "adr": true,
      "fCnt": 23,
      "fPort": 1,
      "data": "c3RhdHVzIzIjQk1FMjgwOjEjVkVNTDYwNzA6MSNCSDE3NTA6MSNSQUlOOjE=",
      "object": {
        "BH1750": true,
        "BME280": true,
        "RAIN": true,
        "VEML6070": true,
        "content": "status",
        "gwDate": "2019-06-25T08:42:52.000Z",
        "type": "WEATHER"
      }
    }

WEATHER data

    {
      "applicationID": "2",
      "applicationName": "application",
      "deviceName": "TTGO-WEATHER",
      "devEUI": "6d3512d0c41ef44d",
      "rxInfo": [
        {
          "gatewayID": "b827ebfffe9fadaa",
          "name": "gateway",
          "time": "2019-06-25T08:46:48.414003Z",
          "rssi": -41,
          "loRaSNR": 10.5,
          "location": {
            "latitude": 40.38486,
            "longitude": -3.60701,
            "altitude": 692
          }
        }
      ],
      "txInfo": {
        "frequency": 868300000,
        "dr": 5
      },
      "adr": true,
      "fCnt": 41,
      "fPort": 1,
      "data": "ZGF0YSMyIzIuNTAjMCMzOS42MyM5MzkuNjUjMjguMjYjMA==",
      "object": {
        "content": "data",
        "gwDate": "2019-06-25T08:45:56.908Z",
        "humidity": 39.63,
        "luminity": 2.5,
        "pressure": 939.65,
        "rain": 0,
        "temperature": 28.26,
        "type": "WEATHER",
        "uv": 0
      }
    }


PLANT STATUS
   

     {
          "applicationID": "2",
          "applicationName": "application",
          "deviceName": "TTGO-PLANT",
          "devEUI": "ead31f10c9610e41",
          "rxInfo": [
            {
              "gatewayID": "b827ebfffe9fadaa",
              "name": "gateway",
              "time": "2019-06-25T08:59:34.632114Z",
              "rssi": -45,
              "loRaSNR": 9,
              "location": {
                "latitude": 40.38506,
                "longitude": -3.60707,
                "altitude": 708
              }
            }
          ],
          "txInfo": {
            "frequency": 868300000,
            "dr": 5
          },
          "adr": true,
          "fCnt": 4,
          "fPort": 1,
          "data": "c3RhdHVzIzEjU0k3MDIxOjEjQkkxNzUwOjEjU006MQ==",
          "object": {
            "BI1750": true,
            "SI7021": true,
            "SM": true,
            "content": "status",
            "gwDate": "2019-06-25T08:58:50.131Z",
            "type": "PLANT"
          }
        }

PLANT MEASURES

    {
      "applicationID": "2",
      "applicationName": "application",
      "deviceName": "TTGO-PLANT",
      "devEUI": "ead31f10c9610e41",
      "rxInfo": [
        {
          "gatewayID": "b827ebfffe9fadaa",
          "name": "gateway",
          "time": "2019-06-25T09:07:27.865394Z",
          "rssi": -45,
          "loRaSNR": 9.5,
          "location": {
            "latitude": 40.38496,
            "longitude": -3.60705,
            "altitude": 695
          }
        }
      ],
      "txInfo": {
        "frequency": 868300000,
        "dr": 5
      },
      "adr": true,
      "fCnt": 25,
      "fPort": 1,
      "data": "ZGF0YSMxIzYuNjcjMzcuODcjMjguMTEjMw==",
      "object": {
        "content": "data",
        "gwDate": "2019-06-25T09:06:46.369Z",
        "humidity": 37.87,
        "light": 6.67,
        "sm": 3,
        "temperature": 28.11,
        "type": "PLANT"
      }
    }





