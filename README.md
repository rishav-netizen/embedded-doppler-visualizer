<p align="center">
  <img src="docs/logo_joystick.png" alt="Embedded Doppler Visualizer Logo" width="130" />
</p>

<h1 align="center">Embedded Doppler Visualizer</h1>

<p align="center">
  <strong>Real-time Acoustic Wave Compression &amp; Supersonic Mach Shockwave Simulation Driven by Physical Microcontroller Telemetry &amp; Raylib C++17</strong>
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
  <a href="#-physics--mach-formulation">Physics Formulation</a> •
  <a href="#-system-architecture">Architecture</a> •
  <a href="#-hardware-setup--pinout">Hardware Setup</a> •
  <a href="#-quickstart--build">Quickstart</a> •
  <a href="#-interactive-controls">Controls</a> •
  <a href="#-roadmap">Roadmap</a>
</p>

---

## 🎯 Overview

**Embedded Doppler Visualizer** is an interactive, hardware-in-the-loop simulation that pairs physical embedded sensors with a high-performance C++ graphics engine to model wave mechanics, the acoustic Doppler effect, and supersonic aerodynamics in real time.

By mapping a physical analog dual-axis thumbstick (`KY-023`) to the kinematic state of an acoustic sound emitter, the visualizer reproduces:
- 🔊 **Subsonic Compression ($M < 1.0$)**: Dynamic bunching of acoustic wavefronts ahead of the moving source and expansion trailing behind.
- 🧱 **The Sound Barrier ($M = 1.0$)**: Critical wavefront pile-up coalescing into a high-pressure perpendicular shockwall.
- 💥 **Supersonic Mach Cone ($M > 1.0$)**: The conical shockwave envelope defined by $\sin(\mu) = \frac{1}{M}$ trailing behind the source.
- 🎧 **Real-Time Doppler Audio**: A 44.1 kHz procedural audio stream synthesizing the perceived line-of-sight pitch shift and sonic boom impulses as the sound barrier is broken.

> [!TIP]
> **No hardware attached?** The visualizer includes an automatic keyboard fallback mode (`WASD` / Arrow Keys with `Shift` Afterburner) when no microcontroller is detected.

---

## 📐 Physics & Mach Formulation

The simulation models sound propagation as discrete circular wavefronts $W_k$ expanding outward from emission coordinates $\vec{x}_k = \vec{x}(t_k)$:

$$R_k(t) = c \cdot (t - t_k)$$

Where:
- $c$ is the propagation speed of sound in the simulated medium ($180\text{ px/s}$).
- $\vec{x}(t)$ and $\vec{v}(t)$ represent the emitter's kinematic position and velocity.

### Perceived Doppler Frequency

For an observer located at $\vec{x}_{obs}$, the radial unit vector from source to observer is $\hat{r} = \frac{\vec{x}_{obs} - \vec{x}_s}{|\vec{x}_{obs} - \vec{x}_s|}$. The perceived pitch is governed by:

$$f_{obs} = f_0 \left( \frac{c}{c - \vec{v}_s \cdot \hat{r}} \right)$$

- **Approaching** ($\vec{v}_s \cdot \hat{r} > 0$): Denominator shrinks $\to f_{obs} > f_0$ (higher observed pitch).
- **Receding** ($\vec{v}_s \cdot \hat{r} < 0$): Denominator grows $\to f_{obs} < f_0$ (lower observed pitch).

### Aerodynamic Regimes

| Regime | Condition | Wavefront Behavior | Acoustic Effect |
|:---|:---|:---|:---|
| **Stationary** | $M = 0$ | Concentric symmetric circles expanding outward | Resting pitch ($f_{obs} = f_0$) |
| **Subsonic** | $0 < M < 1.0$ | Wavefronts compress ahead and stretch behind | Audible Doppler pitch shift |
| **Sound Barrier** | $M = 1.0$ | Circles pile up at the nose into a flat shockwall | Transonic compression barrier |
| **Supersonic** | $M > 1.0$ | Source outruns waves; conical shock wake with $\sin(\mu) = \frac{1}{M}$ | Sonic boom crack across listener |

---

## 🏛️ System Architecture

The simulation is built around modular Object-Oriented Programming (OOP) principles in C++17:

```text
                           ┌─────────────────────────┐
                           │    IInputController     │  <<interface>>
                           └────────────┬────────────┘
                                        │ (Polymorphic Strategy)
                       ┌────────────────┴────────────────┐
                       ▼                                 ▼
           ┌───────────────────────┐         ┌───────────────────────┐
           │  JoystickController   │         │  KeyboardController   │
           │  • Non-blocking poll()│         │  • WASD / Arrow Keys  │
           │  • ±35 ADC Deadzone   │         │  • Shift Afterburner  │
           │  • std::atomic Floats │         │  • Aerodynamic Drag   │
           └───────────────────────┘         └───────────────────────┘
                                        │
                                        ▼
    ┌────────────────────────────────────────────────────────────────────────┐
    │              main.cpp (60 FPS Simulation & HUD Orchestrator)           │
    │  • 60 FPS Raylib OpenGL Render Loop & Telemetry HUD                   │
    │  • Euler Kinematic Integration: x(t + dt) = x(t) + v(t) * dt           │
    │  • AudioSynthesizer: 44.1 kHz Procedural Doppler Sine & Sonic Boom     │
    └──────────────────┬─────────────────────────────────┬───────────────────┘
                       │ (owns)                          │ (owns)
                       ▼                                 ▼
           ┌───────────────────────┐         ┌───────────────────────┐
           │    AcousticSource     │         │     WavefrontPool     │
           │  • Kinematics & Mach  │         │  • Pre-allocated Array│
           │  • Heading Trig       │         │  • O(1) Modulo Reuse  │
           │  • Mach Cone Geometry │         │  • Zero Heap Stutter  │
           └───────────────────────┘         └───────────────────────┘
```

### Core Components
1. **`IInputController` (Strategy Pattern)**: Abstract interface declaring `update()`, `getVelocity()`, `getDeflection()`, and `isResetTriggered()`.
2. **`JoystickController` (Hardware Adapter)**: Dedicated POSIX worker thread streaming serial bytes at 115200 baud. Uses non-blocking `poll()` and a `±35` deadzone filter around `512` to eliminate potentiometer resting jitter, passing coordinates via `std::atomic<float>` for thread safety.
3. **`KeyboardController` (Fallback Strategy)**: Responsive keyboard steering with acceleration and an afterburner boost ceiling.
4. **`AcousticSource` (Kinematic Entity)**: Encapsulates source position, forward Euler physics, heading trigonometry, Mach number ($M = v/c$), and the supersonic shock cone envelope.
5. **`WavefrontPool` (Object Pool Pattern)**: Fixed circular array `Wavefront waves[300]` with zero dynamic heap allocations (`new`/`delete`) inside the 60 FPS render loop.
6. **`Observer` (Listener Station)**: Draggable receiver station computing radial line-of-sight velocity and Doppler pitch shift in real time.
7. **`AudioSynthesizer` (Raylib Audio Stream)**: Procedural sine wave sound generator with smooth frequency slewing and lock-free noise bursts for sonic boom transients.

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
| **VCC** | `5V` | $+5\text{V}$ Power Rail | Biases internal potentiometers |
| **GND** | `GND` | Ground | Common reference ground |
| **VRX** | `A0` | Analog Input (`0–1023`) | Horizontal axis deflection ($X$) |
| **VRY** | `A1` | Analog Input (`0–1023`) | Vertical axis deflection ($Y$) |
| **SW** | `D2` | Digital Input (`INPUT_PULLUP`) | Tactile push switch (active LOW reset) |

### Serial Telemetry Protocol

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

Ensure you have CMake and Raylib installed (e.g. `brew install raylib cmake` on macOS):

```bash
# Configure and build in Release mode
cmake -B host/build -S host -DCMAKE_BUILD_TYPE=Release
cmake --build host/build

# Launch the visualizer
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
| **Supersonic Boost** | Full stick deflection | Hold `Shift` key |
| **Reset Position** | Click Joystick Button (`SW`) | `Space` or `R` key |
| **Pause Simulation** | — | `P` key (freeze waves to inspect geometry) |
| **Tune Sound Speed ($c$)** | — | `[` (slower) / `]` (faster) |
| **Toggle Input Mode** | — | `Tab` key (Joystick $\leftrightarrow$ Keyboard) |
| **Re-Scan USB Serial** | — | `C` key (detects Arduino plugged in after launch) |
| **Mute / Unmute Audio** | — | `M` key |
| **Move Listener** | — | Click &amp; Drag green **LISTENER** with mouse |
| **Exit** | — | `ESC` key |

---

## 📑 Technical Documentation

Detailed architectural reports, circuit schematics, and Typst source documents are located in the [`docs/`](docs) directory:
- 📄 **Comprehensive Project Report**: [`docs/embedded_doppler_comprehensive_report.typ`](docs/embedded_doppler_comprehensive_report.typ)
- 🔌 **Circuit & Wiring Diagrams**: [`docs/circuit_diagram.svg`](docs/circuit_diagram.svg) and [`docs/hardware_schematic.svg`](docs/hardware_schematic.svg)
- 📐 **Wave Physics & System Workflow**: [`docs/doppler_physics.svg`](docs/doppler_physics.svg) and [`docs/system_workflow.svg`](docs/system_workflow.svg)
- 🏛️ **OOP Architecture Diagram**: [`docs/oop_architecture.svg`](docs/oop_architecture.svg)

---

## 🗺️ Roadmap & Completed Features

- [x] High-speed serial protocol design (`115200` baud CSV stream)
- [x] Mathematical Mach angle formulation ($\mu = \arcsin(1/M)$)
- [x] Raylib graphics window setup with keyboard fallback
- [x] Circular wavefront pool allocator (`WavefrontPool`) with screen boundary culling
- [x] Automatic USB serial port discovery (`/dev/cu.usbmodem*`, `/dev/ttyACM*`)
- [x] Real-time procedural Doppler pitch-shift synthesizer (Raylib 44.1 kHz stream)
- [x] Telemetry HUD (Mach speedometer gauge, velocity vectors, wavefront counter, live Doppler oscilloscope)
- [x] Supersonic Mach cone envelope rendering and shockwave passage detection
- [x] Draggable microphone observer with real-time radial line-of-sight frequency calculations
- [x] Real-time stick deflection gimbal widget (live analog stick position, tilt %, direction heading, & angle)
- [x] Simulation pause toggle (`P` key) and dynamic sound speed controls (`[` / `]`)

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
