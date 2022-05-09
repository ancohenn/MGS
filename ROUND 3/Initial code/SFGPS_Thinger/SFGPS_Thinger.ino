#define THINGER_SERIAL_DEBUG
#include <ThingerMKRGSM.h>
#include "arduino_secrets.h"
#include <Wire.h>
#include <ArduinoGraphics.h>
#include <Arduino_MKRRGB.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

ThingerMKRGSM thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

float latitude = 0;
float longitude = 0;
float speed = 0;
int mph_speed = 0;

SFE_UBLOX_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);

  // set APN
  thing.set_apn(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);

  Wire.begin();

  MATRIX.begin();

  MATRIX.brightness(10);

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    //Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

    thing["location"] >> [](pson & out) {;

      out["lat"] = myGNSS.getLatitude();
      out["lon"] = myGNSS.getLongitude();
      Serial.print("Latitute: ");
      Serial.print(latitude / 10000000, 7);
      Serial.print("Longitute: ");
      Serial.print(longitude / 10000000, 7);
      };

    thing["speed"] >> [](pson & out) {;
      
      out["speed"] = myGNSS.getGroundSpeed();
      Serial.print("Speed: ");
      Serial.print(speed / 447);
      };
}

void loop()
{
   
    
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    MATRIX.beginText(2, 0, 127, 0, 0); // X, Y, then R, G, B
    Serial.println();
  speed = myGNSS.getGroundSpeed();
  mph_speed = speed / 447;
  MATRIX.print(mph_speed);
  MATRIX.endText();

  thing.handle();
        
      latitude = myGNSS.getLatitude() ;
      longitude = myGNSS.getLongitude();
      
      thing["location"] >> [](pson & out) {;

      out["lat"] = latitude/ 10000000, 7;
      out["lon"] = longitude/ 10000000, 7;
      };

      
      Serial.print("Latitute: ");
      Serial.print(latitude);
      Serial.print("Longitute: ");
      Serial.print(longitude);
      
      thing["speed"] >> [](pson & out) {;
      
      out["speed"] = myGNSS.getGroundSpeed()/ 447;
       };
       
      Serial.print("Speed: ");
      Serial.print(speed / 447);
      
  thing.write_bucket("location", "location");

  thing.write_bucket("speed", "speed");

  }
}
