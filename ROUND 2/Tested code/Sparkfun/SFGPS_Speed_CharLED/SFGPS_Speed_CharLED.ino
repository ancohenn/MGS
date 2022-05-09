#include <Wire.h>
#include <ArduinoGraphics.h>
#include <Arduino_MKRRGB.h>

float speed = 0;
int ratio_speed = 0;

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  //while (!Serial); //Wait for user to open terminal
  Serial.println("GPS Speedometer Demo");

  Wire.begin();

  MATRIX.begin();

  MATRIX.brightness(10);

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    //Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
}

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    float latitude = myGNSS.getLatitude();
    Serial.print(("Lat: "));
    Serial.print(latitude, 7);

    float longitude = myGNSS.getLongitude();
    Serial.print((" Long: "));
    Serial.print(longitude, 7);

    speed = myGNSS.getGroundSpeed();
    Serial.print((" Speed: "));
    Serial.print(speed / 447);
    Serial.print((" (mph)"));
    MATRIX.beginText(2, 0, 127, 0, 0); // X, Y, then R, G, B
    Serial.println();
  }

  ratio_speed = speed / 447;
  MATRIX.print(ratio_speed);
  MATRIX.endText();

  delay(100);

}
