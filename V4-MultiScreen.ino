
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>
#include <Keypad_I2C.h>

RTC_DS1307 rtc;

const byte ROWS = 4; // Set the number of Rows
const byte COLS = 4; // Set the number of Columns

// Set the Key at Use (4x4)
char keys [ROWS] [COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// define active Pin (4x4)
byte rowPins [ROWS] = {0, 1, 2, 3}; // Connect to Keyboard Row Pin
byte colPins [COLS] = {4, 5, 6, 7}; // Connect to Pin column of keypad.

Keypad_I2C keypad (makeKeymap (keys), rowPins, colPins, ROWS, COLS, 0x20, PCF8574);

char daysOfWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

LiquidCrystal_I2C lcd(0x27, 16, 2);  

const int sensorPin = 32;  // Input pin for sensor (Vp)
const int button = 25; 
volatile int pulseCount = 0;
volatile int rpmPulseCount = 0;  // Separate variable for RPM calculation
volatile int EPulseCount = 0;
volatile int ppi = 60;
unsigned long lastDebounceTime = 0;
unsigned long lastDebounceTimeb = 0;
unsigned long debounceDelay = 200;  // Adjust this value as needed for your application
unsigned long startTime = 0;
unsigned long startTimeE = 0;
unsigned long LastupdateTime = 0;
float meter ;
const unsigned long measurementPeriod = 15000;  // Measurement period in milliseconds
float rpm = 0;
float eff =0 ;
byte currentMenu = 0;
const byte NUMBER_OF_SCREENS = 7;
byte buttonState = HIGH;
byte lastButtonState = HIGH;
char key;
String inp="";
const int weftPin=27;
const int warpPin=26;
int weftcount=0;
int warpcount=0;
int ns=0;
int cs=0;
unsigned long shiftStartTimes [5];
int initialPicks=0;
int calculatedPicks=0;
void IRAM_ATTR weftInt() {
 weftcount++;
}

void IRAM_ATTR warpInt() {
 warpcount++;
}

void IRAM_ATTR handleInterrupt() {
  unsigned long currentMillis = millis();

  // Check if enough time has passed since the last pulse
  if (currentMillis - lastDebounceTime >= debounceDelay) {
    pulseCount++;  // Increment pulse count for PICK
    rpmPulseCount++;  // Increment pulse count for RPM
    EPulseCount++;
    lastDebounceTime = currentMillis;  // Save the current time
  }
}

 void IRAM_ATTR buttonInt() {
    unsigned long currentMillisb = millis();
    if (currentMillisb - lastDebounceTimeb >= 500) {
        currentMenu++;
        currentMenu = currentMenu % NUMBER_OF_SCREENS; // loop back to Menu 0 after last menu
        lastDebounceTimeb = currentMillisb;
    }
 }


void setup() {
  Serial.begin(115200);
    // SETUP RTC MODULE
  if (! rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (1);
  }
  lcd.init();
  lcd.backlight();
  Wire .begin (); // Call the connection Wire
  keypad.begin (makeKeymap (keys)); // Call the connection
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(button, INPUT_PULLUP);
  pinMode(weftPin, INPUT_PULLUP);
  pinMode(warpPin, INPUT_PULLUP);
  // Attach interrupt to the sensor pin
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(weftPin), weftInt, RISING);
  attachInterrupt(digitalPinToInterrupt(warpPin), warpInt, RISING);
  attachInterrupt(digitalPinToInterrupt(button), buttonInt, FALLING);
  startTime = millis();  // Record the start time
  LastupdateTime=millis();
  startTimeE=millis();
}

void loop() {

  // You can use the pulseCount variable for PICK and rpmPulseCount for RPM
 

  // Serial.println(digitalRead(button));
   unsigned long currentTime = millis();
     // Measure initial picks during the first 10 seconds
    if (currentTime - startTimeE <= 10000) {
        initialPicks = EPulseCount;
        //Serial.println(initialPicks);
    }
    if (currentTime - LastupdateTime <=60000){
      calculatedPicks = EPulseCount;
    }
    else{
      Serial.println(initialPicks*6);
      Serial.println(calculatedPicks);
      float efficiency= (calculatedPicks/(initialPicks*6.0))*100;
      Serial.println(efficiency);
      eff = efficiency;
      LastupdateTime=millis();
      calculatedPicks=0;
      EPulseCount=0;
    }
   if (currentTime - startTime >= measurementPeriod) {
     // Calculate RPM over the measurement period and display
     rpm = (rpmPulseCount * 4) ;  // RPM calculation
     rpmPulseCount = 0;  // Reset pulse count for RPM calculation
     startTime = currentTime;  // Reset the start time
   }
  // buttonState = digitalRead(button);
  // if (buttonState == LOW && lastButtonState == HIGH) // check that button has transition to LOW. i.e.: just been pressed
  //   {
  //       currentMenu++;
  //       currentMenu = currentMenu % NUMBER_OF_SCREENS; // loop back to Menu 0 after last menu
  //   }
  DateTime now = rtc.now();
  switch (currentMenu) {
        case 0:
          lcd.clear();
          lcd.setCursor(3,0); // Set the cursor to the beginning of the first line
          lcd.print("PICK:");
          lcd.print(pulseCount);    // Print the PICK count
          //lcd.setCursor(5,1);  // Set the cursor to the beginning of the second line
          //lcd.print("RPM=");
          // Check if the measurement period has elapsed
          lcd.setCursor(3, 1);  // Set the cursor to the beginning of the second line
          lcd.print("RPM:");
          lcd.print(rpm);
            break;

        case 1:

        key = keypad.getKey();
        if (key=='*'){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter PICK/INCH:");
          lcd.setCursor(0, 1);

          inp = ""; // String to store input characters
          
          while (true) {
            key = keypad.getKey();
            if (key != NO_KEY) {
              if (key == '#') {
                break; // Exit the loop if '#' is pressed
              } else {
                inp += key; // Append the key to the input string
                lcd.print(key); // Print the pressed key on the LCD
              }
            }
          }
          // Convert input string to integer and assign it to ppi
          ppi = inp.toInt();
          Serial.println(ppi);
        }
          else{
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("PICK/INCH : ");
          lcd.setCursor(5, 1);
          lcd.print(ppi);
          }
          break;

        case 2:
          lcd.clear();
          lcd.setCursor(1,0); // Set the cursor to the beginning of the first line
          lcd.print("Meter :");
          lcd.setCursor(7,1);
          meter= (pulseCount/ppi)*0.0254;
          lcd.print(meter,3); 
            break;

        case 3:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Warp Cut : ");
          lcd.print(warpcount);
          lcd.setCursor(0,1);
          lcd.print("Weft Cut : ");
          lcd.print(weftcount);
          break;

        case 4:
          lcd.clear();
          lcd.setCursor(0,0); // Set the cursor to the beginning of the first line
          lcd.print("Date:  Time: ");
          lcd.setCursor(0,1);
          lcd.print(now.day(), DEC);
          lcd.print('/');
          lcd.print(now.month(), DEC);
          lcd.print('/');
          lcd.print(now.year()%100, DEC);
          lcd.print(' ');
          lcd.print(now.hour(), DEC);
          Serial.println(now.hour(),DEC);
          lcd.print(':');
          lcd.print(now.minute(), DEC);
          lcd.print(':');
          lcd.print(now.second(), DEC);

            break;           

        case 5:
          key = keypad.getKey();
          if (key == '*') {
            // Input the number of shifts and their start times
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("No of Shifts :");
            inp = "";
            while (true) {
              key = keypad.getKey();
              if (key != NO_KEY) {
                if (key == '#') {
                  break; // Exit the loop if '#' is pressed
                } else {
                  inp += key; // Append the key to the input string
                  lcd.print(key); // Print the pressed key on the LCD
                }
              }
            }
            // Convert input string to integer and assign it to ns
            ns = inp.toInt();
            Serial.println(ns);
            for (int i = 0; i < ns; i++) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Shift ");
              lcd.print(i + 1);
              lcd.print(" Start ");
              lcd.setCursor(0, 1);
              lcd.print("HHMM: ");
              inp = "";
              while (true) {
                key = keypad.getKey();
                if (key != NO_KEY) {
                  if (key == '#') {
                    break; // Exit the loop if '#' is pressed
                  } else {
                    inp += key; // Append the key to the input string
                    lcd.print(key); // Print the pressed key on the LCD
                  }
                }
              }
              shiftStartTimes[i] = inp.toInt(); // Record start time for shift i
              Serial.println(shiftStartTimes[i]);
            }
          } else {
            int currTime = now.hour() * 100 + now.minute();
            int cs = 0;

            // Check if the current time is part of the last shift of the previous day
            if (currTime >= shiftStartTimes[ns - 1] || currTime < shiftStartTimes[0]) {
              // If so, set the current shift to the last shift of the previous day
              cs = ns;
            } else {
              // Otherwise, find the current shift based on the current time
              for (int i = 0; i < ns; i++) {
                if (currTime >= shiftStartTimes[i]) {
                  cs = i + 1; // Shift numbering starts from 1
                }
              }
            }

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Current Shift : ");
            lcd.setCursor(5, 1);
            lcd.print(cs);
          }
          break;

          case 6 :

            lcd.clear();
            lcd.setCursor(1,0); // Set the cursor to the beginning of the first line
            lcd.print("Efficiency :");
            lcd.setCursor(7,1);
            lcd.print(eff); 
            break;




  }


  //lastButtonState = buttonState;
  delay(50);
}
