// @odinlindal, 10/27/2025
// SmarterCoffee project: ESP32 countdown timer + WiFi control
// basically controls a coffee machine through relay, web + buttons

#include <SevSeg.h>
#include <WiFi.h>
#include <WebServer.h>
#include "page.h" // html page for web control UI

#define RELAY_ON  HIGH
#define RELAY_OFF LOW

// ----------- WiFi stuff -----------
const char* ssid = "YOUR_SSID_HERE";
const char* password = "YOUR_SECURITY_KEY_HERE";

WebServer server(80); // web server running on port 80

// ----------- Display + pins -----------
SevSeg sevseg;

int relayPin = 2;          // relay output
int startButtonPin = 26;   // start button
int cycleButtonPin = 27;   // mode select button
int programState = 1;      // which mode is active (1-4)
bool machineRunning = false; // true = relay ON

// ----------- Timer vars -----------
unsigned long countdownStart = 0; // when timer started
unsigned long timerDuration = 0;  // how long total (sec)
unsigned long remainingSecs = 0;  // time left
unsigned long holdStart = 0;      // used for long press detection

// ----------- helper: get duration by mode -----------
int getProgramMins(int state) {
  switch (state) {
    case 1: return 30;  // mode 1 = 30 min
    case 2: return 45;  // mode 2 = 45 min
    case 3: return 60;  // mode 3 = 1 hour
    case 4: return 120; // mode 4 = 2 hours
    default: return 30;
  }
}

// ----------- web handlers -----------
void handleRoot() {
  server.send_P(200, "text/html", MAIN_page); // send main UI
}

// toggle machine state on/off
void handleToggle() {
  if (machineRunning) {
    machineRunning = false;
    digitalWrite(relayPin, RELAY_OFF);
  } else {
    // start new countdown
    timerDuration = getProgramMins(programState) * 60;
    remainingSecs = timerDuration;
    countdownStart = millis();
    machineRunning = true;
    digitalWrite(relayPin, RELAY_ON);
  }
  server.send(200, "text/plain", "OK");
}

// cycle through modes via web
void handleNextMode() {
  if (!machineRunning) {
    programState = (programState % 4) + 1;
  }
  server.send(200, "text/plain", "OK");
}

// send JSON status for web polling
void handleStatus() {
  unsigned long timeLeft = 0;

  if (machineRunning) {
    unsigned long elapsed = (millis() - countdownStart) / 1000;
    if (elapsed < timerDuration) {
      timeLeft = timerDuration - elapsed;
    } else {
      // finished -> turn off
      timeLeft = 0;
      machineRunning = false;
      digitalWrite(relayPin, RELAY_OFF);
    }
  }

  // format remaining time as mm:ss
  int mins = timeLeft / 60;
  int secs = timeLeft % 60;
  char buf[8];
  sprintf(buf, "%02d:%02d", mins, secs);

  // build JSON manually (lazy way)
  String json = "{\"is_on\":";
  json += (machineRunning ? "true" : "false");
  json += ",\"mode\":";
  json += getProgramMins(programState);
  json += ",\"remaining\":\"";
  json += buf;
  json += "\"}";

  server.send(200, "application/json", json);
}

// ----------- setup() -----------
void setup() {
  Serial.begin(115200);

  // setup pins
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, RELAY_OFF);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(cycleButtonPin, INPUT_PULLUP);

  // 7-seg display setup
  byte numDigits = 4;
  byte digitPins[]   = {23, 22, 21, 19};
  byte segmentPins[] = {18, 17, 16, 15, 14, 13, 12, 4};
  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_ANODE;
  bool updateWithDelays = false;
  bool leadingZeros = false;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins,
               resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(100); // full bright

  // WiFi connect animation
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  const char* loadingPatterns[] = {"----", "\\\\--", "|---", "/---"};
  int frame = 0;

  // loop until connected
  while (WiFi.status() != WL_CONNECTED) {
    sevseg.setChars(loadingPatterns[frame]);
    sevseg.refreshDisplay();
    frame = (frame + 1) % 4;
    delay(250);
    Serial.print(".");
  }

  // connected!
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // display IP addr on 7seg one part at a time
  IPAddress ip = WiFi.localIP();
  char buf[5]; // 4 chars + null
  for (int i = 0; i < 4; i++) {
    int part = ip[i];
    snprintf(buf, sizeof(buf), "%4d", part); // right align (e.g. "  10")
    unsigned long showUntil = millis() + 1250;
    while (millis() < showUntil) {
      sevseg.setChars(buf);
      sevseg.refreshDisplay();
      delay(5);
    }
  }

  // setup web routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/nextmode", handleNextMode);
  server.on("/status", handleStatus);
  server.begin();

  Serial.println("HTTP server started");
}

// ----------- loop() -----------
void loop() {
  server.handleClient(); // handle web requests

  int startButtonState = digitalRead(startButtonPin);
  int cycleButtonState = digitalRead(cycleButtonPin);

  // ---- start button ----
  // short press starts countdown if idle
  if (startButtonState == LOW && !machineRunning) {
    timerDuration = getProgramMins(programState) * 60;
    remainingSecs = timerDuration;
    countdownStart = millis();
    machineRunning = true;
    digitalWrite(relayPin, RELAY_ON);
    delay(300); // debounce
  }

  // ---- cycle button ----
  // long hold = stop, short press = change mode
  if (cycleButtonState == LOW) {
    if (holdStart == 0) holdStart = millis();
    if (millis() - holdStart > 1000 && machineRunning) {
      // long hold detected while running -> stop
      machineRunning = false;
      digitalWrite(relayPin, RELAY_OFF);
      remainingSecs = 0;
      Serial.println("Countdown stopped by long hold.");
    }
  } else {
    // short press (under 1s) only when idle
    if (holdStart != 0 && (millis() - holdStart < 1000) && !machineRunning) {
      programState = (programState % 4) + 1;
      delay(200); // debounce
    }
    holdStart = 0;
  }

  // ---- countdown logic ----
  if (machineRunning) {
    unsigned long elapsed = (millis() - countdownStart) / 1000;
    if (elapsed >= timerDuration) {
      // done -> stop machine
      machineRunning = false;
      digitalWrite(relayPin, RELAY_OFF);
    } else {
      remainingSecs = timerDuration - elapsed;
    }
  }

  // ---- display update ----
  if (machineRunning) {
    // show countdown MM:SS or H.MM
    int mins = remainingSecs / 60;
    int secs = remainingSecs % 60;
    if (mins >= 60) {
      int hours = mins / 60;
      int minsOnly = mins % 60;
      sevseg.setNumber(hours * 100 + minsOnly, 2); // H.MM
    } else {
      sevseg.setNumber(mins * 100 + secs, 2); // MM.SS
    }
  } else {
    // idle -> show selected program
    if (programState == 1) sevseg.setNumber(30);
    else if (programState == 2) sevseg.setNumber(45);
    else if (programState == 3) sevseg.setNumber(100, 2);
    else if (programState == 4) sevseg.setNumber(200, 2);
  }

  sevseg.refreshDisplay(); // always refresh
}
