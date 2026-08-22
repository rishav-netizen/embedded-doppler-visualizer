# 🕹️ Embedded Doppler Visualizer

[![Status: In Development](https://img.shields.io/badge/Status-Under_Development-orange?style=flat-square)]()
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Arduino](https://img.shields.io/badge/Arduino-Uno-00979D?style=flat-square&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Raylib](https://img.shields.io/badge/Graphics-Raylib-black?style=flat-square&logo=raylib&logoColor=white)](https://www.raylib.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue?style=flat-square)](LICENSE)

An interactive, hardware-driven acoustic wave visualizer in **C++17** and **Raylib** that uses an **Arduino dual-axis analog joystick** to simulate Doppler compression, the sound barrier, and supersonic Mach cones in real time.

> 🚧 **Work in Progress:** This project is currently under active development.

---

## 🎯 Overview & Concept

Unlike purely software-based simulations, **Embedded Doppler Visualizer** integrates physical hardware with high-performance C++ graphics:
- **Hardware-in-the-Loop Interaction**: A physical 2-axis analog joystick (KY-023) directly drives the kinematic state of the sound source.
- **High-Speed Serial Telemetry**: Low-latency sensor readings stream from an ATmega328P microcontroller at 60 Hz over high-speed USB-UART (115200 baud).
- **Asynchronous Telemetry Polling**: An isolated background POSIX worker thread reads incoming serial packets without lagging the 60 FPS graphics loop.
- **Wavefront & Mach Physics**: Visualizes wave compression (subsonic), wavefront pileup (transonic barrier), and conical shock waves (supersonic $M > 1.0$) as you manipulate the joystick.
- **Keyboard Fallback**: Seamlessly falls back to Arrow keys / WASD when no physical microcontroller is plugged in.

---

## 🔌 Hardware Setup & Pinout

```
+------------------------------------+
|  KY-023 Dual-Axis Joystick Module  |
+------------------------------------+
       |     |     |     |     |
      GND   +5V   VRX   VRY   SW
       |     |     |     |     |
       v     v     v     v     v
      GND    5V    A0    A1    D2
+------------------------------------+
|          Arduino Uno               |
+------------------------------------+
```

| Joystick Pin | Arduino Pin | Description |
|:---|:---|:---|
| **VCC** | `5V` | Power supply (+5V) |
| **GND** | `GND` | Common ground |
| **VRX** | `A0` | Horizontal analog axis (`0–1023`) |
| **VRY** | `A1` | Vertical analog axis (`0–1023`) |
| **SW**  | `D2` | Pushbutton tactile switch (Reset / Pulse) |

---

## 📡 Serial Protocol

The Arduino firmware samples the analog joystick and streams compact CSV frames at 60 Hz:

```text
<RAW_X>,<RAW_Y>,<SWITCH_STATE>\n
```
*Example: `512,512,1` (Center, Idle)*

---

## 🛠️ Tech Stack

- **Host Application**: C++17 (POSIX Multithreading, `std::atomic` lock-free synchronization)
- **Graphics Pipeline**: [Raylib](https://www.raylib.com/) (60 FPS, MSAA anti-aliased 2D rendering)
- **Embedded Platform**: Arduino Uno / Nano (C++ / PlatformIO)
- **Build System**: CMake

---

## 🗺️ Roadmap

- [x] Protocol design & mathematical Mach formulation
- [ ] Core C++ circular wavefront allocator and boundary culling
- [ ] Multithreaded serial reader with automatic port discovery (`/dev/cu.usbmodem*`)
- [ ] Raylib telemetry HUD (Mach number, velocity vectors, active wave count)
- [ ] Audio pitch-shift synthesis (Doppler acoustic effect)

---

## 📄 License
This project is licensed under the [MIT License](LICENSE).
