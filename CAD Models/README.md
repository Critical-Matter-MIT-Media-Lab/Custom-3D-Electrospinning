# CAD Models — Custom Syringe Pump

This folder contains the STEP files for every part of the custom syringe pump used in our 3D electrospinning platform. The pump is mounted on the UR20 robot's end effector and pumps polymer solution at electrospinning-grade flow rates (typically **0.1 – 5 mL/hr**) regardless of tool orientation.

For a labeled overview of the syringe pump in context, see the [main README](../README.md#mechanical-setup) and the figure [`Images/02_Custom 3D Electrospinning with UR20.jpg`](../Images/02_Custom%203D%20Electrospinning%20with%20UR20.jpg).

---

## File format

All parts are exported from Onshape as **STEP (`.step`)** files. STEP is a neutral CAD format and can be opened in any major CAD package — Fusion 360, SolidWorks, Onshape, FreeCAD, Rhino, etc. — and converted to STL for 3D printing.

> [!TIP]
> Looking for the live, editable Onshape source? Open an issue on this repo and we can share the public document link.

---

## Part inventory

The pump consists of three categories of parts: **3D-printed structural parts**, **off-the-shelf hardware**, and the **stepper motor assembly**. The "Source" column tells you what to do with each.

### Structural (3D-print these)

| File | Part | Notes |
|---|---|---|
| `Part Studio 1 - Head Support.step` | Head support | Mounts the stepper motor and one end of the linear rails |
| `Part Studio 1 - End Support.step` | End support | Mounts the other end of the linear rails and supports the syringe tip |
| `Part Studio 1 - Pusher.step` | Pusher | The moving carriage that pushes the syringe plunger |
| `Part Studio 1 - Syringe Clamp.step` | Syringe clamp | Holds the syringe body in place against the End Support |
| `Part Studio 1 - Flange Connector.step` (and variants `(1)`–`(7)`) | Flange connectors | Small adapter parts — print one of each variant as needed for the linear bearings, leadscrew nut, and end-of-rail caps |

**Recommended print settings (starting point — tune for your printer):**

- Material: **PLA** or **PETG** (PETG preferred if the pump will be near solvents)
- Layer height: 0.2 mm
- Walls: 4
- Infill: 40 % (gyroid or grid)
- Supports: only where overhangs exceed 50°

### Off-the-shelf hardware (purchase these)

| File | Part | Typical specification |
|---|---|---|
| `Part Studio 1 - Lead Screw.step` | Leadscrew | **2 mm lead** trapezoidal leadscrew (T8 × 2). The firmware assumes 2 mm/rev — see [`Arduino Code/`](../Arduino%20Code) before changing this |
| `Part Studio 1 - Steel Rod.step` (×4 variants) | Smooth linear rails | Hardened steel, diameter to match the linear bearings (commonly 8 mm) |
| `Part Studio 1 - Linear Bearing.step` (×2 variants) | Linear bearings | LM-series ball bearings sized to the rails |

### Stepper assembly

| File | Part | Notes |
|---|---|---|
| `Part Studio 1 - ActualStepper.step` | Stepper motor reference geometry | NEMA-17 bipolar stepper, 200 steps/rev. The firmware drives this at **64× microstepping** via a DRV8825 driver — see [`Arduino Code/`](../Arduino%20Code) for wiring and pulse timing |

---

## Assembly at a glance

```
Head Support  ──[ stepper ]──┐                                ┌──  End Support
                             │                                │
                  ┌──────────┴────[ leadscrew ]────────────┐  │
                  │                                          │  │
                  └──[ Pusher (rides on linear bearings) ]──┘  │
                             │                                │
                  ┌──────────┴──── steel rails ───────────────┴──── (×2)
                  │
                  └────────────[ syringe + clamp ]─────────────────
```

1. Mount the stepper motor to the **Head Support** with M3 screws.
2. Couple the stepper shaft to the **leadscrew** with a flexible coupler.
3. Insert the **steel rails** through the Head Support and End Support, capturing the **linear bearings** through the Pusher.
4. Thread the leadscrew through the leadscrew nut on the Pusher. Cap the far end at the End Support.
5. Seat the **syringe** in the End Support and lock it down with the **Syringe Clamp**. The syringe plunger should sit centered against the Pusher.
6. Wire the stepper to your DRV8825 driver and ESP32 — see [`Arduino Code/README.md`](../Arduino%20Code).

---

## Notes for modification

- **Different syringe size?** No CAD changes needed for most plastic syringes — the firmware exposes a `mm per mL` calibration value in its web UI to compensate. See [`Arduino Code/`](../Arduino%20Code).
- **Different leadscrew lead?** You **must** update the firmware constant: the conversion `6,400 µsteps per mm` assumes 200 steps/rev × 64 microsteps × (1 / 2 mm per rev). See the comments at the top of `KeratinStepperMotorControl.ino`.
- **Different stepper or microstepping?** Same — see firmware constants.

---

## License

These CAD files are released under the [Creative Commons Attribution 4.0 International License (CC-BY-4.0)](../LICENSE). © 2026 Critical Matter Group, MIT Media Lab. Please credit the Critical Matter Group when reusing or remixing.
