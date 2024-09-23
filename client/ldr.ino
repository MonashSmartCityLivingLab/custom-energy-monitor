// global constants
const int DIFFERENCE_TO_TURN_ON = 100;
const int sensorPin = A0;

// global variables
int value = 0;
int lowestValue = 0;
bool lastStateOn = false;
unsigned long lastTime;

void setup() {
  Serial.begin(9600);
  lastTime = millis();
}

void loop() {
  value = analogRead(sensorPin);
  bool currentStateOn = false;

  // check if the LDR sensor was turned on
  if (value - lowestValue > DIFFERENCE_TO_TURN_ON) {
    currentStateOn = true;
  }

  // Output the current state using Serial.write() as a byte (1 for ON, 0 for OFF)
  // Serial.write(currentStateOn ? 1 : 0);
  
  // check if the state has changed
  if (currentStateOn && !lastStateOn) {
    // last state off -> on : send data to Raspberry Pi
    unsigned long currentTime = millis();
    unsigned long timeDifference = currentTime - lastTime;
    lastTime = currentTime;

    Serial.write("State changed to ON\n");  // Using write for sending data as bytes
    Serial.write((uint8_t*)&timeDifference, sizeof(timeDifference)); // Write time difference as bytes
    
    lastStateOn = true;
  } else if (!currentStateOn && lastStateOn) {
    // last state on -> off
    Serial.write("State changed to OFF\n");
    lastStateOn = false;
  }

  delay(1000);
}
