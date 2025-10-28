# ☕ SmarterCoffee (ESP32 + WiFi)

simple project that turns a normal coffee maker into a “smart” one.  
uses an **ESP32**, **relay**, **7-segment display**, and **two buttons** for mode + start/stop.  
you can also control it from your phone or laptop over Wi-Fi through a little web interface.

---

## 🧠 what it does
- runs a countdown timer (30 / 45 / 60 / 120 min modes)
- toggles a relay that powers the coffee machine
- shows time left on a 4-digit 7-segment display
- can be controlled by:
  - hardware buttons (start / cycle)
  - or a Wi-Fi web dashboard with live updates + a toggle switch

---

## 🧩 hardware setup
- **ESP32 Dev Board**
- **Relay module** (active HIGH)
- **4-digit 7-segment display** (common anode)
- **2 push buttons** (start + cycle)

- ## 🖥️ web interface
the ESP32 hosts its own little webpage (stored in `page.h`).

it looks something like this:

![Circuit On](images/circuit_on.png)
![Circuit Off](images/circuit_off.png)
![WebPage](images/webpage.png)

features:
- toggle switch to start/stop brewing
- “Next Mode” button to cycle durations
- live countdown timer updated every second

---

## ⚙️ how to use
1. open `wifiCoffee.ino`
2. put your Wi-Fi credentials here:
   ```cpp
   const char* ssid = "YOUR_SSID_HERE";
   const char* password = "YOUR_SECURITY_KEY_HERE";
3. upload to your ESP32
4. open Serial Monitor → get the IP address
5. visit that IP in your browser (same Wi-Fi network)

## 🧵 extra notes

holding the cycle button for 1s while running cancels the countdown.

web dashboard syncs with physical buttons — either one works.

the IP address flashes on the display when Wi-Fi connects.

## 🛠️ dependencies

make sure you have:

SevSeg library (for 7-segment display)

WiFi.h and WebServer.h (included with ESP32 board support)

## 🧑‍💻 author

made by @odinlindal, Oct 2025
mostly for fun and caffeine reasons ☕
