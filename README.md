<p align="center">
  <img src="assets/logo-option1-joystick.png" alt="Embedded Doppler Visualizer Logo" width="120" />
</p>

<h1 align="center">Embedded Doppler Visualizer</h1>

<p align="center">
  <strong>Real-time Acoustic Wave Compression & Supersonic Mach Shockwave Simulation Driven by Physical Microcontroller Telemetry & Raylib C++17</strong>
</p>

<p align="center">
  <a href="https://isocpp.org/"><img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17"></a>
  <a href="https://www.arduino.cc/"><img src="https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"></a>
  <a href="https://www.raylib.com/"><img src="https://img.shields.io/badge/Graphics-Raylib-black?style=for-the-badge&logo=raylib&logoColor=white" alt="Raylib"></a>
  <a href="https://cmake.org/"><img src="https://img.shields.io/badge/Build-CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="License: MIT"></a>
</p>

<p align="center">
  <a href="#-classroom-presentation-cheat-sheet">Class Presentation Guide</a> •
  <a href="#-overview">Overview</a> •
  <a href="#-physics--mach-formulation">Physics Formulation</a> •
  <a href="#-object-oriented-architecture">OOP Architecture</a> •
  <a href="#-hardware-setup--pinout">Hardware Setup</a> •
  <a href="#-quickstart--build">Quickstart & Controls</a> •
  <a href="#-roadmap">Roadmap</a>
</p>

---

## 🎤 Classroom Presentation Cheat Sheet

> *Giving your presentation in front of the class? Here is a simple, no-stress script you can use to explain the project in 2 minutes:*

### 1. The Hook (30 seconds)
> *"In physics textbooks, the Doppler effect is usually taught with confusing static chalkboard formulas. We wanted to make wave mechanics tangible. We built a hardware-in-the-loop simulator connecting a physical Arduino joystick to a C++ graphics engine running on Raylib. You can steer an acoustic source, watch wavefronts compress, hit the sound barrier, and trigger supersonic Mach shockwaves in real time."*

### 2. The 3 Aerodynamic Regimes to Show Live (45 seconds)
1. **Subsonic ($M < 1.0$)**:
   - Deflect the stick or press `W/A/S/D` gently.
   - Point out how wavefronts bunch together ahead of the jet (higher observed pitch / blue shift) and stretch out behind it (lower pitch / red shift).
2. **The Sound Barrier ($M = 1.0$)**:
   - Accelerate until the top HUD Mach gauge hits `1.0`.
   - Show how the wave circles pile up into a flat, high-pressure perpendicular compression shockwall.
3. **Supersonic Mach Cone ($M > 1.0$)**:
   - Hold `Shift` (or push the analog stick all the way forward).
   - The jet outruns its own sound waves! Watch the V-shaped **Mach cone** form behind the aircraft with half-angle $\sin(\mu) = \frac{1}{M}$.
   - Fly past the green **Listener station** to demonstrate the sonic boom crack!

### 3. The OOP Architecture to Explain to the Professor (45 seconds)
- **Strategy Pattern (`IInputController`)**: Polymorphic base class. Allows hot-swapping between `JoystickController` (physical UART stream at 115200 baud) and `KeyboardController` (WASD fallback).
- **Object Pool Pattern (`WavefrontPool`)**: Pre-allocated ring buffer of 300 circular waves. Eliminates heap allocations (`new`/`delete`) inside the 60 FPS graphics loop so the simulation never drops frames.
- **Multithreading**: Dedicated POSIX background thread continuously reads serial bytes into lock-free atomic variables, so slow serial I/O never blocks OpenGL rendering.
- **Raylib Real-Time Audio**: 44.1 kHz stream synthesizing procedural sine waves modulated by the exact Doppler equation $f' = f_0 \frac{c}{c - \vec{v}_s \cdot \hat{r}}$.

---

## 🎯 Overview

**Embedded Doppler Visualizer** is an interactive, hardware-in-the-loop simulation that pairs physical embedded sensors with high-performance C++ graphics to demonstrate wave mechanics, the acoustic Doppler effect, and supersonic aerodynamics.

By mapping an analog dual-axis joystick (`KY-023`) to the kinematic state of a sound emitter, the visualizer reproduces:
- 🔊 **Subsonic Compression ($M < 1.0$)**: Dynamic bunching of acoustic wavefronts ahead of the moving source and expansion trailing behind.
- 🧱 **The Sound Barrier ($M = 1.0$)**: Critical wavefront pile-up forming the transonic barrier envelope.
- 💥 **Supersonic Mach Cone ($M > 1.0$)**: The conical shockwave wake and sonic boom envelope defined by $\sin(\mu) = \frac{1}{M}$.
- 🎧 **Interactive Doppler Acoustics**: Real-time Raylib procedural audio output that lets you hear the pitch shift and sonic boom crack as you fly past the listener.

> [!TIP]
> **No hardware? No problem!** The host engine includes a zero-configuration keyboard fallback mode (`WASD` / Arrow Keys with `Shift` Afterburner) when no microcontroller is attached.

---

## 📐 Physics & Mach Formulation

The simulator tracks individual wavefront circles $W_k$ emitted at discrete simulation timestamps $t_k$:

$$R_k(t) = c \cdot (t - t_k)$$

Where:
- $c$ is the ambient propagation speed of sound in the simulated medium ($180\text{ px/s}$).
- $\vec{x}_k = \vec{x}(t_k)$ is the emitter position at the moment of emission.
- $\vec{v}(t)$ is the source velocity vector driven by the joystick or keyboard.

### Perceived Doppler Frequency

For an observer located at $\vec{x}_{obs}$, the radial unit vector from source to observer is $\hat{r} = \frac{\vec{x}_{obs} - \vec{x}_s}{|\vec{x}_{obs} - \vec{x}_s|}$. The perceived pitch is:

$$f_{obs} = f_0 \left( \frac{c}{c - \vec{v}_s \cdot \hat{r}} \right)$$

- When approaching ($\vec{v}_s \cdot \hat{r} > 0$): $f_{obs} > f_0$ (higher pitch).
- When receding ($\vec{v}_s \cdot \hat{r} < 0$): $f_{obs} < f_0$ (lower pitch).

### Aerodynamic Regimes

| Regime | Condition | Physical Phenomenon |
|:---|:---|:---|
| **Stationary** | $M = 0$ | Concentric circular waves expanding radially outward. |
| **Subsonic** | $0 < M < 1$ | Wave compression in flight direction: $f' = f_0 \frac{c}{c - v}$. |
| **Transonic Barrier** | $M = 1.0$ | Wavefronts coalesce into a singular perpendicular high-pressure shockwall. |
| **Supersonic** | $M > 1.0$ | Conical Mach wake forms with half-angle $\mu = \arcsin\left(\frac{1}{M}\right)$. |

---

## 🏛️ Object-Oriented Architecture

The host engine is structured around clean OOP principles and proven software design patterns:

```text
                           ┌─────────────────────────┐
                           │    IInputController     │  <<interface>>
                           └────────────┬────────────┘
                                        │ (Polymorphic Strategy)
                       ┌────────────────┴────────────────┐
                       ▼                                 ▼
           ┌───────────────────────┐         ┌───────────────────────┐
           │  JoystickController   │         │  KeyboardController   │
           │  • POSIX Serial Thread│         │  • WASD / Arrow Keys  │
           │  • Deadband Filtering │         │  • Shift Afterburner  │
           │  • Atomic Coordinates │         │  • Aerodynamic Drag   │
           └───────────────────────┘         └───────────────────────┘
                                        │
                                        ▼
    ┌────────────────────────────────────────────────────────────────────────┐
    │                       VisualizerEngine (Facade)                        │
    │  • 60 FPS Raylib OpenGL Render Loop & Pastel Telemetry HUD             │
    │  • Euler Numerical Integration: x(t + dt) = x(t) + v(t) * dt           │
    │  • AudioSynthesizer: 44.1 kHz Procedural Doppler Pitch & Sonic Boom    │
    └──────────────────┬─────────────────────────────────┬───────────────────┘
                       │ (owns)                          │ (owns)
                       ▼                                 ▼
           ┌───────────────────────┐         ┌───────────────────────┐
           │    AcousticSource     │         │     WavefrontPool     │
           │  • Position & Heading │         │  • Pre-allocated Ring │
           │  • Mach Gauge & Mu    │         │  • O(1) Allocation    │
           │  • Mach Cone Lines    │         │  • Zero GC / Freeing  │
           └───────────────────────┘         └───────────────────────┘
```

### Class Responsibilities:
1. **`IInputController` (Strategy Pattern)**: Abstract base interface declaring pure virtual methods `update()`, `getVelocity()`, and `isResetTriggered()`.
2. **`JoystickController` (Hardware Adapter)**: Dedicated POSIX worker thread streaming UART data at 115200 baud. Employs analog deadband filtering (`±35`) to suppress potentiometer jitter and writes coordinates via `std::atomic<float>` for thread safety.
3. **`KeyboardController` (Fallback Strategy)**: Provides responsive keyboard steering with smooth acceleration and afterburner boost.
4. **`AcousticSource` (Kinematic Entity)**: Encapsulates vehicle position, Euler physics, heading trigonometry, Mach number ($M = v/c$), and the supersonic shock cone envelope.
5. **`WavefrontPool` (Object Pool Pattern)**: Fixed-size ring buffer managing 300 `Wavefront` structs with zero dynamic memory allocation inside the frame loop.
6. **`Observer` (Listener Station)**: Draggable receiver station computing radial line-of-sight velocity and Doppler pitch shift in real time.
7. **`AudioSynthesizer` (Raylib Audio Stream)**: Procedural sine wave sound generator with smooth per-sample frequency slewing and sonic boom impulse transients.
8. **`VisualizerEngine` (Facade)**: Top-level orchestrator coordinating inputs, physics, acoustics, and OpenGL telemetry rendering.

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
| **SW** | `D2` | Digital Input (`INPUT_PULLUP`) | Tactile push switch (active LOW reset) |

---

## 📡 Serial Telemetry Protocol

The Arduino polls the analog channels at **60 Hz** and transmits lightweight CSV packets:

```text
<RAW_X>,<RAW_Y>,<SWITCH_STATE>\n
```

- **Baud Rate**: `115200` baud (8-N-1)
- **Idle Payload**: `512,512,1` (Joystick centered, switch unpressed)
- **Pressed Switch**: `512,512,0` (Switch pulled low to ground)

---

## 🚀 Quickstart & Build

### 1. Build & Launch Host Application (macOS & Linux)

Make sure you have CMake and Raylib installed (e.g. `brew install raylib cmake` on macOS):

```bash
# Configure and build
cmake -B host/build -S host
cmake --build host/build

# Launch the visualizer!
./host/build/doppler_visualizer
```

*(You can also pass a custom serial port if desired: `./host/build/doppler_visualizer /dev/cu.usbmodem1101`).*

### 2. Flash Arduino Firmware (Optional)

Open `firmware/src/main.cpp` in the **Arduino IDE**, select your Arduino board and port, and click **Upload**.

Alternatively with PlatformIO:
```bash
cd firmware
pio run --target upload
```

---

## 🎮 Interactive Controls

| Action | Physical Joystick | Keyboard Fallback |
|:---|:---|:---|
| **Steer Emitter** | Move Analog Stick (`VRX`/`VRY`) | `W`, `A`, `S`, `D` or `Arrow Keys` |
| **Afterburner / Supersonic** | Full stick deflection | Hold `Shift` key |
| **Reset Position** | Click Joystick Button (`SW`) | `Space` or `R` key |
| **Toggle Input Mode** | — | `Tab` key (Joystick $\leftrightarrow$ Keyboard) |
| **Mute / Unmute Audio** | — | `M` key |
| **Move Microphone** | — | Click & Drag green **LISTENER** with mouse |
| **Exit** | Disconnect Cable | `ESC` key |

---

## 📑 Technical Report & Presentation

- 🌐 **Interactive Slide Deck**: Open [`presentation/index.html`](presentation/index.html) in any browser for live canvas simulations, Doppler diagrams, and meme slides!
- 📄 **Technical Report PDF**: [`Embedded_Doppler_Visualizer_Complete_Project_Report.pdf`](Embedded_Doppler_Visualizer_Complete_Project_Report.pdf)
- 📝 **Slide Markdown**: [`presentation/SLIDES.md`](presentation/SLIDES.md)

---

## 🗺️ Roadmap & Features

- [x] High-speed serial protocol design (`115200` baud CSV stream)
- [x] Mathematical Mach angle formulation ($\mu = \arcsin(1/M)$)
- [x] Raylib graphics window setup with keyboard fallback
- [x] Interactive classroom presentation deck with live physics canvas
- [x] Circular wavefront pool allocator (`WavefrontPool`) with screen boundary culling
- [x] Automatic USB serial port discovery (`/dev/cu.usbmodem*`, `/dev/ttyACM*`)
- [x] Real-time procedural Doppler pitch-shift synthesizer (Raylib 44.1 kHz stream)
- [x] Telemetry HUD (Mach speedometer gauge, velocity vectors, wavefront counter)
- [x] Supersonic Mach cone envelope rendering and shockwave passage detection
- [x] Draggable microphone observer with real-time radial line-of-sight frequency calculations
- [x] Real-time stick deflection gimbal widget (live analog stick position, tilt %, direction heading, & angle)

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
