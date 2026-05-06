<div align="center">

# Custom 3D Electrospinning

### A robotic platform for three-dimensional, field-conditioned nanofiber fabrication

*An open hardware + firmware companion to the SIGGRAPH 2026 paper*
**[Electrospun Fields: 3D Nano-Fiber Material Computation as Design Method](#-publications)**

[Critical Matter Group](https://www.media.mit.edu/groups/critical-matter/overview/) · MIT Media Lab

</div>

![Hero — UR20 robotic arm electrospinning onto a wire garment, alongside a close-up of an electrospun conductive PLA mask](Images/00_Hero%20Image_Robot%20Electrospinning%20on%20Garment%20and%20Electrospun%20Mask.jpg)

---

## What this repository is

This repo is a **practical companion** to our SIGGRAPH 2026 paper. The paper presents the *why* — a material-computational design method built around electrospinning. This repository is the *how* — the CAD files, firmware, and step-by-step guides you need to build a custom 3D electrospinning rig of your own.

It is intended for:

- **Researchers** who want to reproduce or extend our platform.
- **Designers, artists, and makers** curious about electrospinning as a fabrication method.
- **Students** picking up electrospinning for the first time.

> [!NOTE]
> This repo is meant to **grow over time**. As we publish further work and refine the platform, we will keep adding CAD revisions, firmware features, materials notes, and tutorials.

---

## ▶ Start here: the build tutorial

The fastest way to understand the full system is to watch our build tutorial. It walks through the mechanical, electrical, and software setup end-to-end.

<div align="center">

[![Watch the tutorial on YouTube](https://img.youtube.com/vi/paKA7W3roEg/maxresdefault.jpg)](https://youtu.be/paKA7W3roEg)

**[▶ Watch on YouTube](https://youtu.be/paKA7W3roEg)**

</div>

If you are new to electrospinning, we recommend reading the next two sections first, then watching the tutorial.

---

## What is electrospinning?

Electrospinning is a fabrication technique that uses **strong electric fields to draw a continuous jet of polymer solution into ultra-fine fibers** — typically between a few hundred nanometers and a few micrometers in diameter. The result is a nonwoven fibrous mat with extremely high surface area.

A traditional, fixed setup looks like this:

![Traditional fixed electrospinning setup — labeled diagram and lab photograph showing the metallic collector, metallic needle, syringe pump, polymer solution, machine controls, and high-voltage DC power supply](Images/01_Traditional%20Fixed%20Electrospinning.jpg)

The four functional elements are:

1. **A polymer solution** — a polymer dissolved in a suitable solvent.
2. **A syringe pump** — meters the solution at a controlled flow rate (typically 0.1 – 1 mL/hr).
3. **A high-voltage DC power supply** — connected to a metallic needle, charging the polymer jet (typically **15 – 25 kV**).
4. **A grounded conductive collector** — the substrate the fibers deposit onto.

When the electric field overcomes the surface tension of the droplet at the needle tip, a thin jet is drawn out, whips through the air (stretching as it travels), and the solvent evaporates before deposition — leaving solid nanofibers on the collector.

Electrospinning has been widely used in filtration, tissue engineering, and surface coatings. **What our work explores is its potential as a *spatial, three-dimensional* fabrication method.**

---

## Why a *custom 3D* electrospinning rig?

Conventional electrospinning is essentially a 2D process — fibers fall onto a flat collector below the needle. This makes it excellent for uniform fiber mats, but **fundamentally limits volumetric and topological control**.

In our paper we show that **scaffold curvature shapes the electric field**, and the field shapes where fibers deposit. Convex regions accumulate fibers consistently; concave regions experience *field shielding* and remain bare. To work around that and access complex 3D topologies, the emitter itself needs to move and reorient relative to the scaffold.

That is what this platform does. It mounts an electrospinning end-effector on a six-axis **Universal Robots UR20** arm, with a **localized syringe pump** so flow stays consistent regardless of orientation, and routes high voltage directly to the robot-mounted needle.

![Custom 3D Electrospinning platform — full setup with UR20, DC power supply, syringe pump, end effector, and conductive 3D-printed scaffolding, with detail callouts for the end effector and the syringe pump](Images/02_Custom%203D%20Electrospinning%20with%20UR20.jpg)

| Letter | Component |
|---|---|
| **A** | UR20 six-axis robotic arm |
| **B** | High-voltage DC power supply |
| **C** | Syringe pump (custom — see [`CAD Models/`](CAD%20Models) and [`Arduino Code/`](Arduino%20Code)) |
| **D** | End effector — routes polymer + high voltage to the needle |
| **E** | Conductive 3D-printed scaffolding (the substrate fibers deposit onto) |

> Before building or operating this platform, read the [Safety](#-safety) section below.

---

## What's in this repository

| Folder | What's inside |
|---|---|
| [`Images/`](Images) | Figures from the paper and additional photographs (hero shots, application images, microscopy) |
| [`CAD Models/`](CAD%20Models) | STEP files for every printable / fabricated part of the syringe pump |
| [`Arduino Code/`](Arduino%20Code) | ESP32 firmware for the syringe pump — exposes a Wi-Fi web UI for flow-rate control |
| [`Publications/`](Publications) | Links to publications associated with this platform |

---

## ⚠ Safety

> [!WARNING]
> **Electrospinning involves high-voltage DC and volatile solvents. Read this section before powering on or building anything.**

This platform operates at **up to 25 kV DC** and dispenses polymer solutions prepared with organic solvents. Both are serious hazards and must be respected. The same safety guidance that applies in our paper applies here.

### High voltage

- **Proper electrical insulation and grounding are strictly required.** All conductive paths intended to be at high potential must be sleeved in HV-rated insulation; all conductive paths intended to be at ground must be continuously, verifiably tied to earth ground.
- **Maintain a safe standoff distance.** Never make physical contact with the emitter, polymer jet, or scaffold while the system is energized.
- **De-energize and discharge before touching anything.** Switch off and disconnect the HV supply, then short the needle to ground (with an insulated grounding stick) before approaching the emitter — even a powered-off supply can leave residual charge.
- **One person at the controls.** When the supply is energized, only one trained operator should be near the rig.

### Solvents and ventilation

The polymer solutions used in our paper involve solvents such as **water, formic acid, and lithium bromide**, and may include **calcium chloride** as an additive. Solvent vapor accumulates near the emitter during electrospinning.

- **Operate inside a fume hood** or a dedicated, well-ventilated electrospinning enclosure.
- **No open flames, no sparks, no ignition sources** anywhere near the emitter — solvent vapor + a high-voltage corona is a fire hazard.
- **Wear appropriate PPE:** nitrile gloves, safety glasses, and a lab coat at minimum. Refer to the SDS for each specific solvent you use.

### Robotic arm

This platform integrates a **Universal Robots UR20** six-axis arm.

- **Follow your robot's standard safety protocol.** Define a safety-rated workspace, use the e-stop, and keep humans out of the cell during programmed motion.
- **Coordinate the HV and motion systems.** The robot should not be commanded to move when an operator is positioned to manually adjust the scaffold or the emitter — and HV must be off whenever the cell is entered.

### Bottom line

If you are unsure whether your setup is safe, **stop, ask, and verify** before proceeding. None of the contents of this repository should be treated as a substitute for proper lab training, institutional safety review, or supervision by a qualified electrical and chemical safety officer.

---

## Mechanical setup

The mechanical side of this platform is the **custom syringe pump** — a high-torque, hand-buildable design that mounts to the robot's end effector and pumps polymer solution at electrospinning-grade flow rates (typically 0.1 – 5 mL/hr).

| Element | Role |
|---|---|
| ① Stepper motor | Drives the leadscrew under microstepped pulses from the ESP32 |
| ② Threaded screw (leadscrew) | Converts rotation into linear translation of the moving frame |
| ③ Moving frame | Linear-bearing-guided carriage that pushes the syringe plunger |
| ④ Syringe | Disposable polymer reservoir with a metallic needle |

All mechanical parts are provided as STEP files in [`CAD Models/`](CAD%20Models). See the README in that folder for a part-by-part inventory and printing notes.

---

## Electrical setup

Three independent circuits coexist on this platform — keep them mentally separate:

| Wiring (matches the diagram above) | Carries | Voltage |
|---|---|---|
| **Blue** — polymer tube | Polymer solution from syringe pump → needle | n/a |
| **Red** — high-voltage line | DC power from supply → needle | **+15 to +25 kV** |
| **Green** — ground | DC power supply ground → conductive scaffold | 0 V (earth) |

Plus a low-voltage DC line (not shown in the simplified diagram) powering the ESP32 + stepper driver inside the syringe pump enclosure — covered in [`Arduino Code/README.md`](Arduino%20Code).

> [!CAUTION]
> The high-voltage red line and the polymer tubing both physically run alongside the robot arm to reach the end effector. Use HV-rated insulated tubing, secure all routing, and verify continuity to the grounded scaffold *before* energizing the supply.

---

## Programming setup

The syringe pump is controlled by an **ESP32** running the firmware in [`Arduino Code/`](Arduino%20Code). The firmware:

- Creates its own **Wi-Fi access point** (`ESP32-Syringe`) — no router needed.
- Serves a web UI at `http://192.168.4.1` for **flow rate (mL/hr)**, **direction**, and **syringe size**.
- Drives the stepper at microsecond precision via a hardware timer.

Quickstart:

1. Open `Arduino Code/KeratinStepperMotorControl.ino` in the Arduino IDE.
2. Install the ESP32 board package and upload to your ESP32.
3. Connect your phone or laptop to the `ESP32-Syringe` Wi-Fi network.
4. Open `http://192.168.4.1` in a browser, set your flow rate, press **Start**.

Full wiring map, syringe-size calibration, and the underlying flow-rate formula are in [`Arduino Code/README.md`](Arduino%20Code).

---

## Gallery

A small selection of artifacts produced with this platform — see the paper for the full discussion.

| | |
|---|---|
| ![Electrospun PVA on a wire garment, draped on a mannequin](Images/07_Applications_Electrospun%20PVA%20on%20Wire%20Garment.jpg) | ![Electrospun keratin face mask, close-up of fiber surface](Images/07_Applications_Keratin%20Mask.jpg) |
| **Electrospun PVA on a wire garment** | **Keratin–PEO face mask** |
| ![Programmable pin-matrix collector with selectively electrospun regions](Images/08_Applications_Programmable%20Pin%20Matrix.jpg) | ![Microscopic view of electrospun fibers showing nanoscale fiber morphology](Images/Extra_Microscopic%20View%20of%20Electrospun%20Fibers.jpg) |
| **Programmable pin-matrix collector** | **Microscopy of electrospun fibers** |

---

## Publications

Work from this platform is documented in the following publications. We host **links only** — please cite the canonical version of record at the publisher when available.

- **Wan, Mahmoud, Mutis, Velho, Xing, Farahi.** *Electrospun Fields: 3D Nano-Fiber Material Computation as Design Method.* SIGGRAPH '26 Art Papers. *ACM Digital Library link coming soon.*

A full list with citation entries lives in [`Publications/README.md`](Publications).

---

## Citation

This paper will appear in the ACM Digital Library shortly. The entry below will be updated with the DOI when available — for now, please use this preliminary BibTeX:

```bibtex
@inproceedings{wan2026electrospun,
  title     = {Electrospun Fields: 3D Nano-Fiber Material Computation as Design Method},
  author    = {Wan, Wai Lok and Mahmoud, Ayah and Mutis, Sergio and Velho, Avantika and Xing, Annie and Farahi, Behnaz},
  booktitle = {ACM SIGGRAPH 2026 Art Papers},
  year      = {2026},
  publisher = {ACM},
  address   = {New York, NY, USA},
  note      = {In press}
  % doi    = {TODO: add DOI when ACM DL link is live}
  % url    = {TODO: add ACM DL URL when available}
}
```

---

## Authors

| Author | Affiliation |
|---|---|
| Wai Lok Wan | MIT |
| Ayah Mahmoud | MIT |
| Sergio Mutis | MIT |
| Avantika Velho | Harvard University |
| Annie Xing | Harvard University |
| Behnaz Farahi (Director, [Critical Matter Group](https://www.media.mit.edu/groups/critical-matter/overview/)) | MIT Media Lab |

For questions, collaboration inquiries, or to flag issues with the documentation, please open a GitHub issue on this repository.

---

## License

© 2026 Critical Matter Group, MIT Media Lab.

This repository — including its source code, CAD files, images, and written documentation — is released under the [Creative Commons Attribution 4.0 International License (CC-BY-4.0)](LICENSE). You are free to share and adapt the contents for any purpose, including commercially, **provided you give appropriate credit to the Critical Matter Group**.
