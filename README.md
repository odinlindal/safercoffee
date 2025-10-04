# ☕ SaferCoffee Countdown Timer
**Author:** @odinlindal  
**Date:** 10/04/2025  
**Platform:** Arduino Nano  

---

## 🔧 Overview
The **SaferCoffee Countdown Timer** is a simple Arduino-based timer designed to automatically power off a coffee machine (or any appliance) after a selected time period — ensuring safety and energy savings.

The user can:
- Choose between **4 preset time modes** (30 min, 45 min, 1 hr, 2 hr)
- Start the countdown with a single button press
- Cancel the countdown by holding the **cycle** button for 1 second

During the countdown, the remaining time is displayed on a **4-digit 7-segment display**, and a **relay module** controls power to the connected appliance.

---

## 🧩 Components Used

| Part | Description | Function |
|------|--------------|-----------|
| **Arduino Nano** | Main microcontroller | Runs countdown logic, reads button input, updates display, and controls relay |
| **Relay Module (HiLetgo 5V Active-Low)** | [Amazon link](https://www.amazon.com/dp/B0CZ6VNK65?ref=ppx_yo2ov_dt_b_fed_asin_title) | Switches power to the coffee maker safely (active LOW control) |
| **3461BS-1 4-Digit 7-Segment Display** | Common anode type | Displays the remaining time in minutes and seconds (or hours/minutes) |
| **4 × 1 kΩ Resistors** | Current-limiting resistors | Used for button pull-ups or display signal protection (depending on wiring setup) |
| **Breadboard & Jumper Wires** | – | Used for prototyping and interconnections between all components |

---

## 🔌 Wiring Summary

| Display Pin | Arduino Pin | Notes |
|--------------|--------------|-------|
| 1–12 | Digital pins (mapped in code) | Direct connection |
| 6, 8, 9, 12 | Through 1kΩ resistors | Protects Arduino output pins |
| Relay VCC | 5V | Power for relay |
| Relay GND | GND | Common ground |
| Relay IN | A0 | Controlled output (active LOW) |
| Start Button | A1 | Pulls LOW when pressed |
| Cycle Button | A2 | Pulls LOW when pressed |

---

## ⚙️ Functionality

1. **Cycle Button (A2)**  
   - Short press: Cycles between preset durations  
   - Long press (>1s during countdown): Cancels and stops the timer

2. **Start Button (A1)**  
   - Starts the countdown for the currently selected duration

3. **Relay (A0)**  
   - Turns ON when the countdown starts  
   - Turns OFF automatically when time expires or is cancelled  

4. **Display**  
   - Shows selected time when idle (e.g., `30`, `45`, `1.00`, `2.00`)  
   - Shows live countdown in `MM.SS` or `H.MM` format while running  

---

## 🧩 Circuit Photos

<p align="center">
  <img src="images/coffee1.jpg" width="600" alt="SaferCoffee Wiring Photo 1">
</p>

<p align="center">
  <img src="images/coffee2.jpg" width="600" alt="SaferCoffee Wiring Photo 2">
</p>

<p align="center">
  <img src="images/coffee3.jpg" width="600" alt="SaferCoffee Wiring Photo 3">
</p>

---

## ⚡ Wiring Diagram (Text Overview)

### Arduino Nano → Relay Module (HiLetgo 5V Active-LOW)
| Arduino Pin | Relay Module Pin | Description |
|--------------|------------------|--------------|
| **A0** | **IN** | Control signal (LOW = relay ON) |
| **5V** | **VCC** | Power for relay module |
| **GND** | **GND** | Shared ground |

**Relay Output:**
- The module’s **COM** and **NO** (Normally Open) terminals connect in series with your appliance’s power line.  
  When the relay activates, COM connects to NO, completing the circuit.

---

### Arduino Nano → Buttons
| Arduino Pin | Button | Wiring |
|--------------|---------|--------|
| **A1** | **Start Button** | One side → GND, other side → A1 (uses internal pull-up) |
| **A2** | **Cycle Button** | One side → GND, other side → A2 (uses internal pull-up) |

> Both buttons are configured with `pinMode(INPUT_PULLUP)` in code — meaning they read **LOW when pressed**.

---

### Arduino Nano → 4-Digit Display (3461BS-1, Common Anode)
| Arduino Pin | Display Pin | Segment / Digit |
|--------------|-------------|-----------------|
| **12** | D1 | Digit 1 (leftmost) |
| **9** | D2 | Digit 2 |
| **8** | D3 | Digit 3 |
| **6** | D4 (rightmost) |
| **11** | Segment A |
| **7**  | Segment B |
| **4**  | Segment C |
| **2**  | Segment D |
| **1**  | Segment E |
| **10** | Segment F |
| **5**  | Segment G |
| **3**  | Decimal Point (DP) |

> The 3461BS-1 display is **common anode**, so each digit’s anode connects to Arduino pins 12, 9, 8, 6 through the **SevSeg library configuration**.

---

## 🧠 How It Works

1. **Idle Mode**
   - Display shows the currently selected time preset (e.g., `30`, `45`, `1.00`, `2.00`)
   - Cycle button switches between presets

2. **Countdown Mode**
   - When “Start” is pressed, the relay activates (power ON)
   - Timer begins counting down second-by-second
   - Display updates continuously
   - Holding the Cycle button for >1 s cancels the timer

3. **End of Countdown**
   - Relay turns OFF (power cut)
   - Display freezes briefly, then returns to idle preset mode

---

## 🧰 Libraries Required
- [**SevSeg**](https://github.com/DeanIsMe/SevSeg) — handles multiplexed 7-segment display control  
  Install via Arduino IDE → *Sketch → Include Library → Manage Libraries...* → Search for “SevSeg”

---

## 🪜 Future Improvements
- Non-blocking countdown (so both buttons respond anytime)
- EEPROM save for last-used duration
- Buzzer or LED alert when countdown finishes
- Enclosure and permanent PCB version

---

## ⚠️ Safety Note
If controlling a **high-voltage appliance**, ensure:
- You use a **proper relay module rated for mains voltage (120–240 V)**  
- You **never handle the AC wiring while powered**
- The **Arduino and relay module share only GND and signal**, not the AC circuit directly

---
