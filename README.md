# 🏢 Earthquake Escape — Interactive 3D Simulation & Evacuation Game

[![Three.js](https://img.shields.io/badge/Three.js-r128-black?logo=three.js)](https://threejs.org/)
[![WebGL](https://img.shields.io/badge/WebGL-2.0-red?logo=webgl)](https://www.khronos.org/webgl/)
[![OpenGL](https://img.shields.io/badge/C%2B%2B-OpenGL%20%2F%20FreeGLUT-blue?logo=c%2B%2B)](https://www.opengl.org/)
[![License](https://img.shields.io/badge/Academic-Computer%20Graphics-green)](#)

An interactive 3D disaster simulation and emergency evacuation game developed for the **Computer Graphics Mini-Project**. Features real-time 3D affine transformations, procedural seismic wave synthesis, AABB sliding collision detection, dynamic falling rubble, an 8-cardinal waypoint compass, dual-frequency radar minimap, and a score-based 3-star mission evaluation system with 20+ progression levels.

---

## 🎮 How to Play

### Option 1: Web Browser (WebGL / Three.js) — Recommended
1. Clone or download this repository:
   ```bash
   git clone https://github.com/Shubham-Khairnar/Earthquake-Escape.git
   cd Earthquake-Escape
   ```
2. Start a local server:
   ```powershell
   python -m http.server 8080
   ```
3. Open in your browser:
   👉 **[http://localhost:8080](http://localhost:8080)**

*(Alternatively, you can directly double-click `index.html` or use VS Code Live Server).*

---

### Option 2: Native C++ Desktop (OpenGL / FreeGLUT)
1. Go to `cpp_opengl/` directory:
   ```powershell
   cd cpp_opengl
   mingw32-make
   .\earthquake_escape.exe
   ```
   *Or run the pre-configured launcher:*
   ```powershell
   .\run_cpp_game.bat
   ```

---

## 🕹️ Controls

| Action | Key / Control |
|---|---|
| **Move** | `W`, `A`, `S`, `D` / Arrow Keys |
| **Sprint** | `Shift` (Uses Stamina) |
| **Jump** | `Spacebar` |
| **Look Around** | Mouse Drag / Arrow Keys |
| **Switch View (1st / 3rd Person)** | `V` |
| **CG Concepts Inspector** | `C` |
| **Restart Level** | `R` |

---

## ✨ Key Features
- **3D Affine Transformations:** Height-proportional structural sway matrix calculations and continuous ground displacements.
- **Seismic Wave Synthesis:** Multi-harmonic sinusoidal wave equations modeling Richter magnitude scale ($5.5\text{ Mw}$ to $8.2+\text{ Mw}$).
- **AABB Collision Physics:** Axis-Aligned Bounding Box intersection detection with velocity sliding response along walls and obstacles.
- **Dynamic 6-DOF Camera Shake:** Trauma-decay camera perturbation system simulating earthquake tremor intensity.
- **Waypoint & Radar Navigation:** 8-cardinal floating waypoint pill and a dual-frequency circular radar minimap showing exit distance.
- **Mission Completed Evaluation:** Score-based 3-star rating mechanism with unlockable campaign levels (20 handcrafted levels).
- **Zero-Dependency Web Audio:** Real-time procedural audio synthesis using HTML5 Web Audio API (rumble, sirens, collapse).

---

## 📁 Project Structure
```text
├── index.html               # Main WebGL Application Viewport
├── styles.css               # Modern Glassmorphism & Emergency HUD Styling
├── js/
│   ├── main.js              # Game Orchestrator & State Management
│   ├── city.js              # Procedural City Generator & Safe Zones
│   ├── earthquake.js        # Seismic Waveforms, Building Sway & Falling Rubble
│   ├── player.js            # Kinematics, AABB Sliding Collisions & Camera
│   ├── audio.js             # Web Audio API Procedural Sound Synthesizer
│   ├── ui.js                # HUD, Radar Minimap, 3-Star Rating & Mission Modals
│   └── cg_inspector.js      # On-Screen CG Matrix & Waveform Inspector
├── cpp_opengl/              # Native C++ OpenGL / FreeGLUT Implementation
├── PROJECT_REPORT.md        # Comprehensive Academic Project Report
├── VIVA_QUESTIONS.md        # Computer Graphics Viva Preparation Guide
└── libs/                    # Bundled Three.js (Offline Ready)
```

---

## 👨‍💻 Author
- **Shubham Khairnar** — *Computer Graphics Mini-Project (2026)*

