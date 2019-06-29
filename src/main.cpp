#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <SSD1306.h>

#include "plant_sensors.h"
#include "weather_sensors.h"
#include "dummy_sensors.h"
#include "globals.h"


#define WEATHER_SENSORS 0
#define PLANT_SENSORS 1

#define PIN_SDA 21
#define PIN_SCL 22

#define ADD_SSD1306 0x3c

static uint8_t tx_buffer[30];
static uint8_t rx_buffer[30];
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    //For board revision V1.5 use GPIO12 for LoRa RST
    .rst = 12,
    //For board revision(s) newer than V1.5 use GPIO19 for LoRa RST
    //.rst = 19,
    //If DIO2 is not connected use:
    .dio = {/*dio0*/ 26, /*dio1*/ 33, /*dio2*/ LMIC_UNUSED_PIN}
    //If DIO2 is connected use:
    //.dio = {/*dio0*/ 26, /*dio1*/ 33, /*dio2*/ 32}
};

/* Queue to store the measurement data
 */
QueueHandle_t xQueue;
const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

/* Measurement variables
 * The wait periods for measurement and sending are defined here
 */
#if PLANT_SENSORS == 1
#include "ttgo_plant.h"
PlantMeasurement measurement;
PlantData currentData;
PlantData data;
#elif WEATHER_SENSORS == 1
#include "ttgo_weather.h"
WeatherMeasurement measurement;
WeatherData currentData;
WeatherData data;
#else
#include "ttgo_dummy.h"
DummyMeasurement measurement;
DummyData currentData;
DummyData *data;
#endif
int measurementWaitPeriod = 5 * 60 * 1000;
int sendingWaitPeriod = 5 * 60 * 1000;
int sendingStatusWaitPeriod = 2 * 60 * 1000;
unsigned long lastMeasurement = millis() - measurementWaitPeriod;
unsigned long lastSending = millis() - sendingWaitPeriod;
unsigned long lastStatusSending = millis() - sendingStatusWaitPeriod;

SSD1306 display(ADD_SSD1306, PIN_SDA, PIN_SCL);

/* Prototypes */
void initOled();
void initQueue();
void initLmic();
void measureAndSend(osjob_t *j);
void measure();
void send();
void messageSent(bool removeFromQueue);
void displayData(char *data);
void displayQueue();
void displayOTAInformation();

void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }

void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }

void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

long getValue(String data, char separator)
{
  long value = 0;
  int index = data.indexOf(separator);
  if (index!=-1) {
    String sValue = data.substring(index+1, data.length()+1);
    value = sValue.toInt()*1000;
  } 
	return value;
}

//Init OLED
void initOled() 
{
    Serial.println("(I) - init OLED");
    pinMode(16,OUTPUT);
    pinMode(2,OUTPUT);
  
    digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
    delay(50); 
    digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
    Serial.println(F("oled - ok"));
    display.init();
    display.flipScreenVertically();  
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
}

void oledDisplay(String text) {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, SENSOR_CLASS);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 26, text);
  display.display();
}

void oledDisplay(String text1, String text2) {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, SENSOR_CLASS);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 26, text1);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 52, text2);
  display.display();
}

void initQueue()
{
  Serial.println("(I) - init queue");
#ifdef PLANT_SENSORS
  xQueue = xQueueCreate(1000, sizeof(PlantData));
#elif WEATHER_SENSORS
  xQueue = xQueueCreate(1000, sizeof(WeatherData));
#else
  xQueue = xQueueCreate(1000, sizeof(DummyData));
#endif
  Serial.println("queue - ok");
}

void initLmic()
{
  Serial.println("(I) - init lmic");
  os_init();
  LMIC_reset();
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  // Set up the channels used by the network, which corresponds
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set.
  //LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  //LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
  Serial.println("lmic - ok");
  displayOTAInformation();
}

void initMeasurement()
{
  Serial.println("(I) - init sensors");
  measurement.init();
  if (measurement.status()) {
  Serial.println("sensors - ok");
  } else {
    Serial.println("sensors - ko");
  }
}

void messageSent(bool removeFromQueue)
{

  // if sending was successful remove the message from the queue
  if (removeFromQueue && uxQueueMessagesWaiting(xQueue) > 0)
  {
    void *data;
    if (xQueueReceive(xQueue, &data, xTicksToWait))
    {
      Serial.printf("(S) - removed message from queue (waiting: %d, free: %d)\n",
                    uxQueueMessagesWaiting(xQueue),
                    uxQueueSpacesAvailable(xQueue));
      displayQueue();
    }
  }

  // trigger the next measurement
  os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), measureAndSend);
}

void measure()
{
  Serial.println("(M) ================================");
  // check if it is time for measurement
  unsigned long elapsedTime = millis() - lastMeasurement;
  if (elapsedTime > measurementWaitPeriod)
  {
    lastMeasurement = millis();

    Serial.println("(M) ================================");
    Serial.println("(M) - start measurement");
    measurement.measure(&currentData);
    char msg[80] = "";
    currentData.logger_message(msg);
    displayData(msg);

    if (xQueueSend(xQueue, &currentData, xTicksToWait))
    {
      Serial.printf("(M) - added message to queue (waiting: %d, free: %d)\n",
                    uxQueueMessagesWaiting(xQueue),
                    uxQueueSpacesAvailable(xQueue));
      displayQueue();
    }
  }
  else
  {
    int remainingSeconds = (measurementWaitPeriod - elapsedTime) / 1000;
    Serial.printf("Next measurement in %03d seconds\r\n", remainingSeconds);
  }
}

void send()
{
  // check if it is time for sending
  unsigned long elapsedTime = millis() - lastSending;
  unsigned long elapsedStatusTime = millis() - lastStatusSending;
  //If has something to send
  if (elapsedTime > sendingWaitPeriod && xQueuePeek(xQueue, &data, xTicksToWait))
  {
    lastSending = millis();
    Serial.println("(S) ================================");
    Serial.println("(S) - start sending");
    // get lora message formatted
    char msg[60] = "";
    data.lora_message(msg);
    Serial.printf("(S) - message: %s\n", msg);
    oledDisplay("Sending data",msg);
    // convert char-array to int-array
    uint8_t lmic_data[strlen(msg)];
    for (int idx = 0; idx < strlen(msg); idx++)
    {
      lmic_data[idx] = (uint8_t)msg[idx];
    }
    Serial.printf("(S) - message size: %d\n", sizeof(lmic_data));

    // sending data via lorawan
    LMIC_setTxData2(1, lmic_data, sizeof(lmic_data), 1);
  //Build and sends an status message
  } else if (elapsedStatusTime > sendingStatusWaitPeriod) {
    lastStatusSending = millis();
    Serial.println("(S) ================================");
    Serial.println("(S) - start sending status");
    // get lora message formatted
    char msg[60] = "";
    measurement.lora_status_message(msg);
    Serial.printf("(S) - status message: %s\n", msg);
    oledDisplay("Sending statatus",msg);
    // convert char-array to int-array
    uint8_t lmic_data[strlen(msg)];
    for (int idx = 0; idx < strlen(msg); idx++)
    {
      lmic_data[idx] = (uint8_t)msg[idx];
    }
    Serial.printf("(S) - message size: %d\n", sizeof(lmic_data));

    // sending data via lorawan unconfirmed
    LMIC_setTxData2(1, lmic_data, sizeof(lmic_data), 1);
  }
  else
  {
    int remainingSeconds = (sendingWaitPeriod - elapsedTime)/1000 ;
    int remainingStatusSeconds = (sendingStatusWaitPeriod - elapsedStatusTime)/1000 ;  
    // nothing to send - go into next cycle
    os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), measureAndSend);
    Serial.printf("Next data sending in %03d seconds\r\n", remainingSeconds);
    Serial.printf("Next status sending in %03d seconds\r\n", remainingStatusSeconds);
 }
}

void measureAndSend(osjob_t *j)
{
  measure();
  //os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(1), measureAndSend);
  send();
}

static void process_message(u1_t *rx_buffer, int dataBeg ,int dataLen)
{
  if (dataLen > 0)
  {
    char buffer[dataLen];
    for (int i = 0; i < dataLen; i++)
    {
      buffer[i] = rx_buffer[dataBeg + i];
      Serial.print(buffer[i]);
    }
    buffer[dataLen] = 0;
    Serial.println();
    String command(buffer);
    Serial.println("Received command:"+command);
    oledDisplay("Received command", command);
    if (command.startsWith(COMMAND_SAMPLING_PERIOD)) {
      long value = getValue(command,':');
      Serial.printf("COMMAND_SAMPLING_PERIOD---->%lu",value);
      if (value>0) {
        measurementWaitPeriod = value;
      }
    } else if (command.startsWith(COMMAND_SENDING_PERIOD)) {
      long value = getValue(command,':');
      Serial.printf("COMMAND_SENDING_PERIOD---->%lu",value);
      if (value>0) {
        sendingWaitPeriod = value;
      }
    } else if (command.startsWith(COMMAND_STATUS_PERIOD)) {
      long value = getValue(command,':');
      Serial.printf("COMMAND_STATUS_PERIOD---->%lu",value);
      if (value>0) {
        sendingStatusWaitPeriod = value; 
      }
    } else {
      Serial.printf("unknown command---->%s\r\n",buffer);
    }

    //memset(rx_buffer, 0, sizeof(dataLen));
  }
}

void do_send(osjob_t *j)
{
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, tx_buffer, sizeof(tx_buffer) - 1, 1);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent(ev_t ev)
{
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev)
  {
  case EV_SCAN_TIMEOUT:
    Serial.println(F("EV_SCAN_TIMEOUT"));
    break;
  case EV_BEACON_FOUND:
    Serial.println(F("EV_BEACON_FOUND"));
    break;
  case EV_BEACON_MISSED:
    Serial.println(F("EV_BEACON_MISSED"));
    break;
  case EV_BEACON_TRACKED:
    Serial.println(F("EV_BEACON_TRACKED"));
    break;
  case EV_JOINING:
    Serial.println(F("EV_JOINING"));
    oledDisplay("Connecting to LoRa");
    break;
  case EV_JOINED:
    Serial.println(F("EV_JOINED"));
    oledDisplay("Joined LoRa");
    // Disable link check validation (automatically enabled
    // during join, but not supported by TTN at this time).
    LMIC_setLinkCheckMode(0);
    break;
  case EV_RFU1:
    Serial.println(F("EV_RFU1"));
    break;
  case EV_JOIN_FAILED:
    Serial.println(F("EV_JOIN_FAILED"));
    oledDisplay("Joining LoRa failed");
    break;
  case EV_REJOIN_FAILED:
    Serial.println(F("EV_REJOIN_FAILED"));
    break;
  case EV_TXCOMPLETE:
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    if (LMIC.txrxFlags & TXRX_ACK)
    {
      Serial.println(F("(R) - received ack"));
      oledDisplay("Send complete");
      messageSent(true);
    }
    if (LMIC.dataLen)
    {
      Serial.println(F("*******************************************"));
      Serial.println(F("(R) - Received "));
      Serial.println(LMIC.dataLen);
      Serial.print(F(" bytes of payload:"));
      Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
      Serial.println();
      process_message(LMIC.frame, LMIC.dataBeg, LMIC.dataLen);
    }
    else
    {
      messageSent(false);
    }
    // Schedule next transmission
    //os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
    break;
  case EV_LOST_TSYNC:
    Serial.println(F("EV_LOST_TSYNC"));
    break;
  case EV_RESET:
    Serial.println(F("EV_RESET"));
    break;
  case EV_RXCOMPLETE:
    // data received in ping slot
    Serial.println(F("EV_RXCOMPLETE"));
    break;
  case EV_LINK_DEAD:
    Serial.println(F("EV_LINK_DEAD"));
    break;
  case EV_LINK_ALIVE:
    Serial.println(F("EV_LINK_ALIVE"));
    break;
  default:
    Serial.println(F("Unknown event"));
    break;
  }
}

void displayQueue()
{
  Serial.println("queue");

  //u8x8.printf("queue %03d", uxQueueMessagesWaiting(xQueue));
}

void displayData(char *msg)
{
  Serial.print("MSG:");
  Serial.println(msg);
}

void covertandprint(unsigned char value)
{
  switch (value)
  {
  case 0:
    Serial.print("0");
    break;
  case 1:
    Serial.print("1");
    break;
  case 2:
    Serial.print("2");
    break;
  case 3:
    Serial.print("3");
    break;
  case 4:
    Serial.print("4");
    break;
  case 5:
    Serial.print("5");
    break;
  case 6:
    Serial.print("6");
    break;
  case 7:
    Serial.print("7");
    break;
  case 8:
    Serial.print("8");
    break;
  case 9:
    Serial.print("9");
    break;
  case 10:
    Serial.print("A");
    break;
  case 11:
    Serial.print("B");
    break;
  case 12:
    Serial.print("C");
    break;
  case 13:
    Serial.print("D");
    break;
  case 14:
    Serial.print("E");
    break;
  case 15:
    Serial.print("F");
    break;
  default:
    Serial.print("?");
    break;
  }
}

void scanI2C()
{
  byte error, address;

  Serial.println("Scan I2C");
  Wire.begin();

  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    //Serial.println("Scanning");
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
}

void displayOTAInformation()
{
  unsigned char i;
  unsigned char chartemp;

  Serial.println(F("OTAA mode to join network"));
  Serial.print("DevEui: ");
  for (i = 0; i <= 7; i++)
  {
    chartemp = pgm_read_word_near(DEVEUI + 7 - i);
    covertandprint((chartemp >> 4) & 0xf);
    covertandprint(chartemp & 0xf);
  }
  Serial.println("");
  Serial.print("AppEui: ");
  for (i = 0; i <= 7; i++)
  {
    chartemp = pgm_read_word_near(APPEUI + 7 - i);
    covertandprint((chartemp >> 4) & 0xf);
    covertandprint(chartemp & 0xf);
  }

  Serial.println("");
  Serial.print("AppKey: ");
  //memcpy_P(buftemp, APPKEY, 16);
  for (i = 0; i <= 15; i++)
  {
    chartemp = pgm_read_word_near(APPKEY + i);
    //Serial.print(buftemp[i],HEX);
    covertandprint((chartemp >> 4) & 0xf);
    covertandprint(chartemp & 0xf);
  }
  Serial.println("");
}

void setup()
{
  Serial.begin(115200);
  scanI2C();
  delay(1000);

  Serial.println(F("(I) ================================"));
  Serial.println(F("(I) - init started"));
  initOled();
  initLmic();
  initQueue();
  initMeasurement();
  os_setCallback(&sendjob, measureAndSend);
  oledDisplay("Init completed");
}

void loop()
{
  os_runloop_once();
}