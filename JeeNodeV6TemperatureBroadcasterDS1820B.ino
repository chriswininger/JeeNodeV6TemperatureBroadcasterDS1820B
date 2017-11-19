#include <JeeLib.h>
#include <RF12sio.h> 
#include <OneWire.h> 
#include <DallasTemperature.h>

#define VERSION "0.0.1"
#define FILTERSETTLETIME 5000 
#define ONE_WIRE_BUS 4
#define SET_NODE 4     // wireless node ID 
#define SET_GROUP 210    // wireless net group 
#define SEND_MODE      2     // set to 3 if fuses are e=06/h=DE/l=CE, else set to 2
#define FILTERSETTLETIME 5000 //  Time (ms) to allow the filters to settle before sending data
#define BATT_SENSE_PORT 2    // sense battery voltage on this port
boolean settled = false;

// setup to read sensor data on Port1 D
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

Port battPort (BATT_SENSE_PORT);

// define structure rf12 packets
typedef struct { int tempDS1820B; } Payload;
Payload payload;

void setup() {
  Serial.begin(57600);
  Serial.print("JeeNode Temp Sensor -- v");
  Serial.print(VERSION);
  Serial.println(); 
  Serial.print("Node: "); 
  Serial.print(SET_NODE);
  Serial.print(", Network: "); 
  Serial.println(SET_GROUP);

  // initialize the RFM12B for wireless transmission
  rf12_initialize(SET_NODE, RF12_433MHZ, SET_GROUP);
  rf12_sleep(RF12_SLEEP);

  // initialize temperature sensor library
  sensors.begin(); 
}

void loop() {
     // read the current temperature from the DS1820B sensor
     sensors.requestTemperatures();
    /*
     Multiply temeprature reading which is float by 100 and cast to integer for broadcast
      On the other end we can can /100.0 to cast back to float. It is not an issue for
      RFM12B to broadcast floats, but EMONCMS which is used for our receiver has this
    */
    payload.tempDS1820B = sensors.getTempCByIndex(0) * 100;
    Serial.print("Temperature: ");
    Serial.print(payload.tempDS1820B/100.0);

    // === working on sensing battery health ===
    /*
    payload.battVolts = readBatt();
    Serial.print(", Battery: ");
    Serial.print(payload.battVolts);*/
  
    Serial.println("");                                         
  
    delay(100);

    // because millis() returns to zero after 50 days ! 
    if (!settled && millis() > FILTERSETTLETIME) {
      Serial.println("Settled, begin broadcast");
      settled = true;
    }

     if (settled) {
      send_rf_data(payload);
    }

    delay(1000);
}
