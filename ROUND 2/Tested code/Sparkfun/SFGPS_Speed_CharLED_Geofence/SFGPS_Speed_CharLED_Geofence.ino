#include <Wire.h>
#include <ArduinoGraphics.h>
#include <Arduino_MKRRGB.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

float latitude = 0;
float longitude = 0;
float speed = 0;

int ratio_speed = 0;
#define LED LED_BUILTIN

SFE_UBLOX_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  //while (!Serial); //Wait for user to open terminal
  Serial.println("GPS Speedometer with Geofencing Demo");

  Wire.begin();

  MATRIX.begin();

  MATRIX.brightness(10);

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    //Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
  myGNSS.setI2COutput(COM_TYPE_UBX); // Limit I2C output to UBX (disable the NMEA noise)

  Serial.println(F("Waiting for a 3D fix..."));

  byte fixType = 0;

  while (fixType < 3)
  {
    fixType = myGNSS.getFixType(); // Get the fix type
    Serial.print(F("Fix: ")); // Print it
    Serial.print(fixType);
    if (fixType == 0) Serial.print(F(" = No fix"));
    else if (fixType == 1) Serial.print(F(" = Dead reckoning"));
    else if (fixType == 2) Serial.print(F(" = 2D"));
    else if (fixType == 3) Serial.print(F(" = 3D"));
    else if (fixType == 4) Serial.print(F(" = GNSS + Dead reckoning"));
    else if (fixType == 5) Serial.print(F(" = Time only"));
    Serial.println();
    delay(1000);
  }
  Serial.println(F("3D fix found!"));

  float latitude = myGNSS.getLatitude();
  float longitude = myGNSS.getLongitude();

  Serial.print("Geofence Location: ");
  Serial.print(latitude / 10000000, 7);
  Serial.print(", ");
  Serial.print(longitude / 10000000, 7);
  Serial.println();

  uint32_t radius = 500; // Set the radius to 5m (radius is in m * 10^-2 i.e. cm)

  byte confidence = 2; // Set the confidence level: 0=none, 1=68%, 2=95%, 3=99.7%, 4=99.99%

  // Call clearGeofences() to clear all existing geofences.
  Serial.print(F("Clearing any existing geofences. clearGeofences returned: "));
  Serial.println(myGNSS.clearGeofences());

  // It is possible to define up to four geofences.
  // Call addGeofence up to four times to define them.
  Serial.println(F("Setting the geofences:"));

  Serial.print(F("addGeofence for geofence 1 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));

  radius = 10000; // 10m = 1000
  Serial.print(F("addGeofence for geofence 2 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));

  radius = 15000; // 15m
  Serial.print(F("addGeofence for geofence 3 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));

  radius = 20000; // 20m
  Serial.print(F("addGeofence for geofence 4 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));
}

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    float latitude = myGNSS.getLatitude();
    float longitude = myGNSS.getLongitude();

    Serial.print("Location: ");
    Serial.print(latitude / 10000000, 7);
    Serial.print(", ");
    Serial.print(longitude / 10000000, 7);

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
  //==================================================================================
  //                              GeoFence
  //==================================================================================
  geofenceState currentGeofenceState; // Create storage for the geofence state

  bool result = myGNSS.getGeofenceState(currentGeofenceState);

  Serial.print(F("getGeofenceState returned: ")); // Print the combined state
  Serial.print(result); // Get the geofence state

  if (!result) // If getGeofenceState did not return true
  {
    Serial.println(F(".")); // Tidy up
    return; // and go round the loop again
  }

  Serial.print(F(". status is: ")); // Print the status
  Serial.print(currentGeofenceState.status);

  Serial.print(F(". numFences is: ")); // Print the numFences
  Serial.print(currentGeofenceState.numFences);

  Serial.print(F(". combState is: ")); // Print the combined state
  Serial.print(currentGeofenceState.combState);

  if (currentGeofenceState.combState == 0)
  {
    Serial.print(F(" = Unknown"));
    digitalWrite(LED, LOW);
  }
  if (currentGeofenceState.combState == 1)
  {
    Serial.print(F(" = Inside"));
    digitalWrite(LED, HIGH);
  }
  else if (currentGeofenceState.combState == 2)
  {
    Serial.print(F(" = Outside"));
    digitalWrite(LED, LOW);
  }

  Serial.print(F(". The individual states are: ")); // Print the state of each geofence
  for (int i = 0; i < currentGeofenceState.numFences; i++)
  {
    if (i > 0) Serial.print(F(","));
    Serial.print(currentGeofenceState.states[i]);
  }
  Serial.println();

  //==================================================================================

  delay(600);

}
