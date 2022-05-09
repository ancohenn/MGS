#include <Arduino_MKRGPS.h>
#include <ArduinoGraphics.h>
#include <Arduino_MKRRGB.h>

int ratio_speed = 0;
float speed;

void Display(int ratio_speed)
{
  MATRIX.print(ratio_speed);
  MATRIX.endText();
}
void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // If you are using the MKR GPS as shield, change the next line to pass
  // the GPS_MODE_SHIELD parameter to the GPS.begin(...)
  if (!GPS.begin(GPS_MODE_SHIELD)) {
    Serial.println("Failed to initialize GPS!");
    while (1);
  }
  MATRIX.begin();

  MATRIX.brightness(10);

}
void loop() {
  // check if there is new GPS data available
  if (GPS.available()) {
    // read GPS values
    float latitude   = GPS.latitude();
    float longitude  = GPS.longitude();
    speed            = GPS.speed();

    // print GPS values
    Serial.print("Location: ");
    Serial.print(latitude, 7);
    Serial.print(", ");
    Serial.println(longitude, 7);

    Serial.print("Speed: ");
    Serial.print(speed / 1.609344);
    Serial.println("mph");

    Serial.println();

  }
  ratio_speed = speed / 1.609344;

  MATRIX.beginText(1, 0, 127, 0, 0); // X, Y, then R, G, B
  Display(ratio_speed);

  delay(100);
}
