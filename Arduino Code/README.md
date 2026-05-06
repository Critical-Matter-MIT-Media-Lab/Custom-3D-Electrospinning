# Arduino Code — ESP32 Syringe Pump Firmware

This folder contains the firmware that drives the custom syringe pump.

It runs on an **ESP32**, drives a **NEMA-17 stepper** through a **DRV8825** driver, and exposes a small **Wi-Fi web UI** for setting flow rate, direction, and syringe size — no router needed.

---

## File

- `KeratinStepperMotorControl.ino` — single-file Arduino sketch. The name is a leftover from our keratin-electrospinning experiments; the firmware itself is fully general and works for any polymer.

---

## What it does

- **Creates a Wi-Fi access point** named `ESP32-Syringe` (open, no password by default).
- **Serves a web UI at `http://192.168.4.1`** with controls for:
  - **Start / Pause**
  - **Direction** (clockwise = dispense, counter-clockwise = retract)
  - **Syringe size** (mm of plunger travel per mL)
  - **Flow rate** (mL/hr)
- **Drives the stepper at microsecond precision** via an ESP32 hardware timer running at 1 MHz. The web UI never touches the step pin — it only updates the timer's half-period.

---

## Hardware

| Component | Spec |
|---|---|
| Microcontroller | ESP32 (any DevKit-style board with the standard pinout) |
| Stepper driver | DRV8825 with at least 12–24 V motor supply and 64× microstepping enabled |
| Stepper motor | NEMA-17 bipolar, 200 steps/rev |
| Leadscrew | T8 × 2 (2 mm lead) — see [`CAD Models/`](../CAD%20Models) |

### Wiring

| ESP32 GPIO | DRV8825 pin | Notes |
|---|---|---|
| GPIO **17** | `STEP` | Pulse train output |
| GPIO **16** | `DIR` | HIGH = clockwise, LOW = counter-clockwise |
| GND | `GND` (logic) | Common ground with ESP32 |
| — | `SLEEP` ↔ `RESET` | Tie together to keep the driver awake |
| — | `M0`, `M1`, `M2` | All HIGH for **64× microstepping** (or pull `M2` HIGH and tie `M0/M1` per the DRV8825 datasheet — the firmware assumes 64×) |

The motor power supply (12–24 V) connects to the `VMOT` / `GND` pair on the driver; **be sure to add the recommended 100 µF electrolytic capacitor across `VMOT` and `GND`** as close to the driver as possible.

> [!CAUTION]
> Make sure the stepper logic ground and the motor power ground are common with the ESP32 ground. Floating grounds are a common cause of missed steps and erratic behavior.

---

## How the flow rate is computed

The firmware converts the user-set flow rate (mL/hr) into a step pulse half-period (µs) using:

```
halfPeriodUs = (281,250 × mm_per_mL) / mL_per_hr
```

This number comes from the gearing math at the top of the sketch:

- **Stepper:** 200 steps/rev × 64 microsteps = **12,800 µsteps/rev**
- **Leadscrew:** **2 mm/rev**
- **=> 6,400 µsteps per mm**

If you change the leadscrew, microstepping, or stepper, **update the constant accordingly** — see the comment block at the top of `KeratinStepperMotorControl.ino`.

### Syringe-size calibration

The web UI's **Syringe size** dropdown sets `mm_per_mL` — the millimeters of plunger travel that correspond to 1 mL of dispensed volume. Defaults included:

| Option | Use when |
|---|---|
| `1 mm = 1 mL` | (Default — calibrated for our typical disposable syringe) |
| `2 mm = 1 mL` | Smaller-bore syringes |
| `0.5 mm = 1 mL` | Larger-bore syringes |

To add a new option, edit the `<select id="syringe">` block in the embedded HTML inside the `.ino` file (search for `CHANGE THESE FOR CHANGING SYRINGE OPTIONS`).

> [!TIP]
> The most reliable way to calibrate a new syringe is empirical: dispense for a fixed time at a known mL/hr setting, weigh the output, and adjust `mm_per_mL` until the weight matches the expected volume.

---

## Quickstart

1. **Install the ESP32 board package** in the Arduino IDE.
   - `File → Preferences → Additional Boards Manager URLs:` add `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - `Tools → Board Manager`, search **esp32**, install **esp32 by Espressif Systems**.
2. **Open `KeratinStepperMotorControl.ino`** in the Arduino IDE.
3. **Select your ESP32 board** under `Tools → Board → ESP32 Arduino`.
4. **Upload** to the ESP32.
5. Open the **Serial Monitor** at 115200 baud — you should see the access-point IP printed (`192.168.4.1`).
6. **Connect your phone or laptop** to the `ESP32-Syringe` Wi-Fi network.
7. Open `http://192.168.4.1` in a browser. Set your **syringe size** and **flow rate**, press **Apply**, then **Start**.

---

## Customizing the access point

Edit the constants at the top of the `.ino`:

```c
const char* AP_SSID = "ESP32-Syringe";   // Network name
const char* AP_PASS = "";                // 8+ chars to enable WPA2; empty = open
const uint8_t AP_CHANNEL = 6;            // Wi-Fi channel
```

If you set a password, it must be **at least 8 characters** — otherwise the firmware falls back to an open network.

---

## License

This firmware is released under the [Creative Commons Attribution 4.0 International License (CC-BY-4.0)](../LICENSE). © 2026 Critical Matter Group, MIT Media Lab. Please credit the Critical Matter Group when reusing or modifying.
