// global constants
const int DIFFERENCE_TO_TURN_ON = 100;
const int sensorPin = A0;

// global variables
int value = 0;
int lowestValue = 0;
bool lastStateOn = false;
unsigned long lastTime;
char buffer[50]; 
bool IS_PROD = true;

void setup() {
  Serial.begin(9600);
  unsigned long lastTime = 0;
}

void loop() {
  value = analogRead(sensorPin);
  bool currentStateOn = false;

  // check if the LDR sensor was turned on
  if (value - lowestValue > DIFFERENCE_TO_TURN_ON) {
    currentStateOn = true;
  }

  if (currentStateOn && lastTime == 0) {
    lastTime = millis();
  } else if (currentStateOn) {
    //millis() rollover after 49 days which this code will handle
    unsigned long nowTime = millis();
    unsigned long newBlinkTimeDiff = (unsigned long)(nowTime - lastTime); //using unsigned long to handle millis() rollover
    if (IS_PROD) {
      sprintf(buffer, "[SEND] %d\n", newBlinkTimeDiff);  
      Serial.write(buffer);                              // Send the formatted string as bytes
    } else {
      Serial.print("[SEND] ");
      Serial.print(newBlinkTimeDiff);
      Serial.write('\n');
    }
    lastTime = nowTime;
  }

  delay(1000);
}
