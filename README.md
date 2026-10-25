<p align="center">
  <img src="assets/logo-option1-joystick.png" alt="Embedded Doppler Visualizer Logo" width="120" />
</p>

<h1 align="center">Embedded Doppler Visualizer</h1>

<p align="center">
  <strong>Real-time Acoustic Wave Compression & Supersonic Mach Shockwave Simulation Driven by Physical Microcontroller Telemetry</strong>
</p>

<p align="center">
  <a href="https://isocpp.org/"><img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17"></a>
  <a href="https://www.arduino.cc/"><img src="https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"></a>
  <a href="https://www.raylib.com/"><img src="https://img.shields.io/badge/Graphics-Raylib-black?style=for-the-badge&logo=raylib&logoColor=white" alt="Raylib"></a>
  <a href="https://cmake.org/"><img src="https://img.shields.io/badge/Build-CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="License: MIT"></a>
</p>

<p align="center">
  <a href="#-overview">Overview</a> •
  <a href="#-interactive-classroom-presentation">Class Presentation</a> •
  <a href="#-technical-report--documentation">Technical Report</a> •
  <a href="#-physics--mach-formulation">Physics Formulation</a> •
  <a href="#-hardware-setup--pinout">Hardware Setup</a> •
  <a href="#-system-architecture">Architecture</a> •
  <a href="#-quickstart--build">Quickstart</a> •
  <a href="#-roadmap">Roadmap</a>
</p>

---

## 🎯 Overview

**Embedded Doppler Visualizer** is an interactive, hardware-in-the-loop simulation that pairs physical embedded sensors with high-performance C++ graphics to demonstrate wave mechanics, the acoustic Doppler effect, and supersonic aerodynamics.

By mapping an analog dual-axis joystick (`KY-023`) to the kinematic state of a sound emitter, the visualizer reproduces:
- 🔊 **Subsonic Compression ($M < 1.0$)**: Dynamic bunching of acoustic wavefronts ahead of the moving source and expansion trailing behind.
- 🧱 **The Sound Barrier ($M = 1.0$)**: Critical wavefront pile-up forming the transonic barrier envelope.
- 💥 **Supersonic Mach Cone ($M > 1.0$)**: The conical shockwave wake and sonic boom envelope defined by $\sin(\mu) = \frac{1}{M}$.

> [!TIP]
> **No hardware? No problem!** The host engine includes a zero-configuration keyboard fallback mode (`WASD` / Arrow Keys) when no microcontroller is attached.

---

## 🐱 Interactive Classroom Presentation

Need to explain this project to classmates or professors? An interactive, animated slide deck with live Doppler physics simulation and meme humor is included in [`presentation/`](presentation/):

- 🌐 **Interactive Slide Deck**: Open [`presentation/index.html`](presentation/index.html) in any browser (Safari, Chrome, Firefox, Edge).
  - Built-in live interactive canvas simulator right inside the presentation.
  - Full keyboard navigation (`Space` / `Arrow Keys` for slides, `F` for fullscreen, `8-bit` audio toggle).
  - Real dev engineering bloopers, cat memes, and Doppler physics explained simply.
- 📝 **Markdown Version**: View [`presentation/SLIDES.md`](presentation/SLIDES.md) for quick reading or Marp / Slidev export.

---

## 📑 Technical Report & Documentation

A publication-quality 6-page comprehensive project report covering theoretical acoustic physics, circuit schematics, multi-threaded C++17 architecture, and performance benchmarks is included:

- 📄 **Complete PDF Report**: [`Embedded_Doppler_Visualizer_Complete_Project_Report.pdf`](Embedded_Doppler_Visualizer_Complete_Project_Report.pdf)
- 📝 **Typst Source Document**: [`docs/embedded_doppler_comprehensive_report.typ`](docs/embedded_doppler_comprehensive_report.typ)

---

## 📐 Physics & Mach Formulation

The simulator tracks individual wavefront circles $W_k$ emitted at discrete simulation timestamps $t_k$:

$$R_k(t) = c \cdot (t - t_k)$$

Where:
- $c$ is the ambient propagation speed of sound in the simulated medium.
- $\vec{x}_k = \vec{x}(t_k)$ is the emitter position at the moment of emission.
- $\vec{v}(t)$ is the source velocity vector driven by the joystick displacement.

### Aerodynamic Regimes

| Regime | Condition | Physical Phenomenon |
|:---|:---|:---|
| **Stationary** | $M = 0$ | Concentric circular waves expanding radially outward. |
| **Subsonic** | $0 < M < 1$ | Wave compression in flight direction: $f' = f_0 \frac{c}{c - v}$. |
| **Transonic Barrier** | $M = 1.0$ | Wavefronts coalesce into a singular perpendicular high-pressure shockwall. |
| **Supersonic** | $M > 1.0$ | Conical Mach wake forms with half-angle $\mu = \arcsin\left(\frac{1}{M}\right)$. |

---

## 🔌 Hardware Setup & Pinout

```text
┌──────────────────────────────────────────────┐
│        KY-023 2-Axis Joystick Module         │
└──────────────────────────────────────────────┘
       │        │        │        │        │
      GND      +5V      VRX      VRY       SW
       │        │        │        │        │
       ▼        ▼        ▼        ▼        ▼
      GND       5V       A0       A1       D2
┌──────────────────────────────────────────────┐
│           Arduino Uno / Nano Board           │
└──────────────────────────────────────────────┘
```

| Joystick Pin | Arduino Pin | Electrical Configuration | Description |
|:---|:---|:---|:---|
| **VCC** | `5V` | $+5\text{V}$ Power Rail | Powers internal potentiometers |
| **GND** | `GND` | Ground | Common reference ground |
| **VRX** | `A0` | Analog Input (`0–1023`) | Horizontal axis deflection |
| **VRY** | `A1` | Analog Input (`0–1023`) | Vertical axis deflection |
| **SW** | `D2` | Digital Input (`INPUT_PULLUP`) | Tactile push switch (Reset / Pulse trigger) |

---

## 📡 Serial Telemetry Protocol

The ATmega328P microcontroller firmware polls the analog ADC channels at **60 Hz** and transmits lightweight, newline-terminated CSV packets:

```text
<RAW_X>,<RAW_Y>,<SWITCH_STATE>\n
```

- **Baud Rate**: `115200` baud (8-N-1)
- **Idle Payload**: `512,512,1` (Joystick centered, switch unpressed)
- **Pressed Switch**: `512,512,0` (Switch pulled low to ground)

---

## 🏛️ System Architecture

```text
  [Hardware: KY-023 Joystick]
               │ (Analog voltages)
               ▼
   [Firmware: Arduino Uno]
               │ (115200 Baud UART / CSV @ 60 Hz)
               ▼
   [Host: Serial Worker Thread] ──(std::atomic ring state)──┐
                                                            ▼
                                                [Host: Raylib Render Loop]
                                                • 60 FPS Anti-Aliased Graphics
                                                • Circular Wavefront Allocator
                                                • Supersonic Mach Cone Envelope
```

1. **Serial Worker Thread**: Dedicated POSIX background thread continuously drains the serial buffer so that UART I/O latency never blocks graphics rendering.
2. **Lock-Free State**: Thread-safe communication via atomic coordinates ensures zero mutex contention between telemetry polling and frame drawing.
3. **Raylib Graphics Engine**: GPU-accelerated OpenGL pipeline rendering circular wave rings, boundary culling, and velocity vector overlays.

---

## 🚀 Quickstart & Build

### 1. Flash Arduino Firmware

```bash
cd firmware
pio run --target upload
```

*(Alternatively, open `firmware/src/main.cpp` in the Arduino IDE, select your port, and click **Upload**).*

### 2. Build & Launch Host Application

```bash
# Configure and build
cmake -B host/build -S host
cmake --build host/build

# Run simulator
./host/build/EmbeddedDopplerVisualizer
```

### Controls

| Action | Physical Hardware | Keyboard Fallback |
|:---|:---|:---|
| Move Source | Deflect Analog Stick (`VRX`/`VRY`) | `Arrow Keys` or `WASD` |
| Reset Position | Click Joystick Button (`SW`) | `R` Key |
| Exit | Disconnect Cable | `ESC` Key |

---

## 🗺️ Development Roadmap

- [x] High-speed serial protocol design (`115200` baud CSV stream)
- [x] Mathematical Mach angle formulation ($\mu = \arcsin(1/M)$)
- [x] Raylib graphics window setup with keyboard fallback
- [x] Interactive classroom presentation deck with live physics canvas
- [ ] Circular wavefront pool allocator with screen boundary culling
- [ ] Automatic USB serial port discovery (`/dev/cu.usbmodem*`)
- [ ] Real-time Doppler pitch-shift synthesizer (WebAudio / Miniaudio)
- [ ] Telemetry HUD (Mach gauge, velocity vectors, wavefront counter)

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
