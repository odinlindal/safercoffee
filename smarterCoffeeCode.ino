// @odinlindal, 10/04/2025
// Code for countdown timer, SaferCoffee project

#include <SevSeg.h>

SevSeg sevseg; // Create a display object

// Define input pins
int relayPin = A0;
int startButtonPin = A1;
int cycleButtonPin = A2;
int programState = 1; // Variable for keeping track of time mode

// Method for running the countdown, takes an integer programState as variable to determine how many minutes the timer should go for
void runMachine(int programState) {

  // Create a variable for keeping track of how many minutes the timer should run, defined by the programState int
  int mins = 0;
  if(programState == 1) { // Half an hour
    mins = 30;
  } else if (programState == 2) { // 45 Minutes
    mins = 45;
  } else if (programState == 3) { // 1 Hour
    mins = 60;
  } else if (programState == 4) { // 2 Hours
    mins = 120;
  }
  long remainingSecs = mins * 60; // Countdown in seconds
  unsigned long previousMillis = millis();
  unsigned long holdOff = 0;

  digitalWrite(relayPin, LOW); // Turn relay ON

  // Loop while there is time left
  while (remainingSecs > 0) {

    // Read the "cycle/off" button
    int cycleState = digitalRead(cycleButtonPin);

    // Long press on cycle button = OFF
    if (cycleState == LOW) {
      if (holdOff == 0) holdOff = millis();
      if (millis() - holdOff > 1000) {
        break; // shutdown
      }
    } else {
      holdOff = 0; // reset if released
    }

    unsigned long currentMillis = millis();

    // Decrement every 1 second
    if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis;
      remainingSecs--;
    }

    // Calculate display format
    int minutes = remainingSecs / 60;
    int seconds = remainingSecs % 60;

    if (minutes >= 60) {
      int hours = minutes / 60;
      int minsOnly = minutes % 60;
      sevseg.setNumber(hours * 100 + minsOnly, 2); // H.MM
    } else {
      // show MM.SS
      sevseg.setNumber(minutes * 100 + seconds, 2); // MM.SS
    }

    sevseg.refreshDisplay(); // Refreshes the display every cycle
  }

  digitalWrite(relayPin, HIGH); // Turn relay OFF after countdown is done
  delay(500); // Prevent re-trigger
}

void setup() {
  // Set up relay + buttons
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(cycleButtonPin, INPUT_PULLUP);

  // Display setup
  byte numDigits = 4; 
  byte digitPins[]   = {12, 9, 8, 6}; // D1, D2, D3, D4
  byte segmentPins[] = {11, 7, 4, 2, 1, 10, 5, 3}; 
  // order: A, B, C, D, E, F, G, DP

  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_ANODE; // Display type
  bool updateWithDelays = false;
  bool leadingZeros = false;

  // Initialize the display
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros);
  sevseg.setBrightness(90); // Adjust brightness 0–100
}

void loop() {
  // Read both buttons
  int startButtonState = digitalRead(startButtonPin);
  int cycleButtonState = digitalRead(cycleButtonPin);

  // Check which mode is selected and display the appropriate minutes / hours
  if (programState == 1) {
    sevseg.setNumber(30);
  } else if (programState == 2) {
    sevseg.setNumber(45);
  } else if (programState == 3) {
    sevseg.setNumber(100, 2);
  } else if (programState == 4) {
    sevseg.setNumber(200, 2);
  }

  // Refresh the display continuously
  sevseg.refreshDisplay();

  // If "start" button is pressed, start the countdown
  if (startButtonState == LOW) {
    runMachine(programState);
  }
  // If "cycle/stop" button is pressed, cycle through the different time selections
  if (cycleButtonState == LOW) {
    if (programState < 4) programState++;
    else programState = 1;
    delay(200);
  }
}
