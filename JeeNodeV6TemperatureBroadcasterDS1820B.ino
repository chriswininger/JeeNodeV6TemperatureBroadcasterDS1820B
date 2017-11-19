#include <JeeLib.h>
#include <RF12sio.h> 
#include <OneWire.h> 
#include <DallasTemperature.h>

#define FILTERSETTLETIME 5000 
#define ONE_WIRE_BUS 4
#define SET_NODE 4     // wireless node ID 
#define SET_GROUP 210    // wireless net group 
#define SEND_MODE      2     // set to 3 if fuses are e=06/h=DE/l=CE, else set to 2
#define FILTERSETTLETIME 5000 //  Time (ms) to allow the filters to settle before sending data
boolean settled = false;

// setup to read sensor data on Port1 D
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

// define structure rf12 packets
typedef struct { int power1, power2, power3, battery; } PayloadTX;      // create structure - a neat way of packaging data for RF comms
PayloadTX emontx;

void setup() {
  Serial.begin(57600);
  Serial.println("JeeNode Temp Sensor -v2"); 
  Serial.print("Node: "); 
  Serial.print(SET_NODE);
  Serial.print(" Network: "); 
  Serial.println(SET_GROUP);

  // initialize the RFM12B for wireless transmission
  rf12_initialize(SET_NODE, RF12_433MHZ, SET_GROUP);
  rf12_sleep(RF12_SLEEP);

  // initialize temperature sensor library
  sensors.begin(); 
}

void loop() {
    /*
     Multiply temeprature reading which is float by 100 and cast to integer for broadcast
      On the other end we can can /100.0 to cast back to float. It is not an issue for
      RFM12B to broadcast floats, but EMONCMS which is used for our receiver has this
    */
    emontx.power1 = sensors.getTempCByIndex(0) * 100;
    Serial.print(emontx.power1);                                         
  
    emontx.power2 = 222;
    Serial.print(" "); Serial.print(emontx.power2);

    emontx.power3 = 333;
    Serial.print(" "); Serial.print(emontx.power3);

    emontx.battery = 444;

    Serial.print(" "); Serial.print(emontx.battery);
    Serial.println();
    delay(100);

    // because millis() returns to zero after 50 days ! 
    if (!settled && millis() > FILTERSETTLETIME) {
      Serial.println("Settled, begin broadcast");
      settled = true;
    }

     if (settled) {
      send_rf_data();
    }

    delay(1000);
}
