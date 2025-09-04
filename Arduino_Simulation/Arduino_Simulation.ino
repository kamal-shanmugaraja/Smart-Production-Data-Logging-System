const int pulsePin = 7;  // Output pin for simulated proximity sensor pulses
const int ledPin = 13;   // LED pin for indication
const int pulseCountTarget = 218;  // Target pulse count per minute
const int weftPin = 2;  
const int warpPin = 3;  
const int warpPulse=8;
const int weftPulse=9;
volatile bool interruptOccurred = false;  // Flag variable to indicate interrupt

void  weftInt() {
    digitalWrite(weftPulse, HIGH);  // Simulate pulse

    //delayMicroseconds(500);         // Adjust delay for pulse duration
    //digitalWrite(weftPulse, LOW);   // Reset pulse

    interruptOccurred = true;  // Set the flag
  //Serial.print(interruptOccurred);
}

void  warpInt() {
    digitalWrite(warpPulse, HIGH);  // Simulate pulse

    //delayMicroseconds(500);         // Adjust delay for pulse duration
    //digitalWrite(warpPulse, LOW);   // Reset pulse
  interruptOccurred = true;  // Set the flag
}

void setup() {
  pinMode(pulsePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(warpPulse, OUTPUT);
  pinMode(weftPulse, OUTPUT);
  pinMode(weftPin, INPUT_PULLUP);
  pinMode(warpPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(weftPin), weftInt, FALLING);
  attachInterrupt(digitalPinToInterrupt(warpPin), warpInt, FALLING);
  Serial.begin(115200);
}

void loop() {
  if (!interruptOccurred) {
    simulatePulses();
  }
}

void simulatePulses() {
  int pulseDelay = 60000 / pulseCountTarget;  // Calculate delay for desired pulse count per minute
  for (int i = 0; i < pulseCountTarget; i++) {
    digitalWrite(pulsePin, HIGH);  // Simulate pulse
    digitalWrite(ledPin, HIGH);    // Indicate pulse with LED
    delayMicroseconds(500);         // Adjust delay for pulse duration
    digitalWrite(pulsePin, LOW);   // Reset pulse
    digitalWrite(ledPin, LOW);
    delayMicroseconds(500);         // Adjust delay for pulse spacing
    if (interruptOccurred) {
     // interruptOccurred = false;  // Reset the flag
      Serial.println(digitalRead(8));
      break;  // If interrupt occurred, break out of the loop
    }
    delay(pulseDelay);              // Delay to achieve desired pulse count per minute
  }
}
