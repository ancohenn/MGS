// libraries
#include <MKRIMU.h>

void setup() {

//initialize the IMU
 Serial.begin(9600);
if (!IMU.begin()) {
  Serial.println("Failed to initialize IMU!");

  while (1);
}

//initialise the variables for storing x,y,z acceleration in the second state
float x2, y2, z2;

  // declare x,y,z variables that store the first state of acceleration in Gs
  float x1, y1, z1;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x1, y1, z1);

    //find the difference between the last 2 states of the acceleration coordinates
    float at = sqrt(x1 * x1 + y1 * y1 + z1 * z1);

    //in steady state (i.e not moving) the value of at should be 1
    //so anything significantly above or below that (+-.1G), should indicate that
    //it has moved off of the steady state and hence; a fall can be inferred!
    //if you want to make the fall detect more or less sensitive,
    //you can play with these threshold numbers
    if (at > 1.10 || at < 0.90) {
      char json[] = "{\"fall\":\"true\"}";

      //Print state in the 'fall' event
      Serial.println("Fall Detected");

      Serial.println(at);

      //in the event of a change in acceleration, the value of 'at'
      //will be at a state to trigger this if loop for multiple times a second
      //so we delay for 5 seconds so that a fall will only trigger once / 5s
      delay(5000);
    }
  }
}
