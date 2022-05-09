#include "thingProperties.h"
#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> 

int ratio_speed = 0;

SFE_UBLOX_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.


void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
  
  Wire.begin();
  
  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    //Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  // Your code here 
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    float latitude = myGNSS.getLatitude();
    Serial.print(("Lat: "));
    Serial.print(latitude);

    float longitude = myGNSS.getLongitude();
    Serial.print((" Long: "));
    Serial.print(longitude);

    float speed = myGNSS.getGroundSpeed();
    Serial.print((" Speed: "));
    Serial.print(speed);
    Serial.print((" (mph)"));

    Serial.println();
  }

}

void initProperties(){
  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(latitude, READ, ON_CHANGE);
  ArduinoCloud.addProperty(longitude, READ, ON_CHANGE);
}
