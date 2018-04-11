#include <JeeLib.h>
#include <RF12sio.h> 
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "avr/sleep.h"

#define DEBUG 1
#define TRANSMIT_INTERVAL 2 // interval in minutes
#define VERSION "0.0.1"
#define FILTERSETTLETIME 5000 
#define ONE_WIRE_BUS 4
// #define SET_NODE 4     // Node ID for garden
#define SET_NODE 3 // Node ID for outdoor temp
#define SET_GROUP 210    // wireless net group 
#define SEND_MODE      2     // set to 3 if fuses are e=06/h=DE/l=CE, else set to 2
#define FILTERSETTLETIME 5000 //  Time (ms) to allow the filters to settle before sending data
#define BATT_SENSE_PORT 2    // sense battery voltage on this port

boolean settled = false;
long loopWaitMS = 60000 * TRANSMIT_INTERVAL;

// setup to read sensor data on Port1 D
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

Port battPort (BATT_SENSE_PORT);

// define structure rf12 packets
typedef struct { float tempDS1820B; } Payload;
Payload payload;

void setup() {
  Serial.begin(57600);

  if (DEBUG) {
    Serial.print("JeeNode Temp Sensor -- v");
    Serial.print(VERSION);
    Serial.println();
    Serial.print("Node: "); 
    Serial.print(SET_NODE);
    Serial.print(", Network: "); 
    Serial.println(SET_GROUP);
  }
  // initialize the RFM12B for wireless transmission
  rf12_initialize(SET_NODE, RF12_433MHZ, SET_GROUP);
  rf12_sleep(RF12_SLEEP);

  // initialize temperature sensor library
  sensors.begin(); 
}

void loop() {
    // read the current temperature from the DS1820B sensor
    sensors.requestTemperatures();
    payload.tempDS1820B = sensors.getTempCByIndex(0);

    if(DEBUG) {
      Serial.print("Temperature: ");
      Serial.print(payload.tempDS1820B);
      Serial.println("");
    }

    delay(100);

    // because millis() returns to zero after 50 days ! 
    if (!settled && millis() > FILTERSETTLETIME) {
      if (DEBUG) {
        Serial.println("Settled, begin broadcast");
      }
      settled = true;
    }

     if (settled) {
      send_rf_data(payload);

      // explicit delay to reduce power consumption
      mySleep(loopWaitMS);
    } else {
      delay(500);  
    }
}
