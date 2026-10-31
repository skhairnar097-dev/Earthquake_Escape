# C++ OpenGL / FreeGLUT Implementation Guide

This directory contains the native desktop C++ implementation of **Earthquake Escape: Interactive 3D Simulation & Emergency Escape Game** using traditional Computer Graphics APIs (OpenGL, GLU, and FreeGLUT).

---

## What's New in this Version:
1. **Interactive Intensity Selection Menu**: Start tab allowing gamers to pick seismic intensity (`[1]` Low 5.5 Mw, `[2]` Medium 7.0 Mw, `[3]` High 8.2 Mw, `[4]` Extreme 9.5 Mw) with score multipliers up to 3.5x.
2. **20 Distinct Thematic Levels**: Level-by-level progression through 20 unique urban districts unlocked one-by-one with unique spawn points, destinations, atmospheric fog, and dynamic lighting:
   - **Level 1**: Downtown Plaza (Clear Daylight)
   - **Level 2**: Financial District (Golden Sunset)
   - **Level 3**: Industrial Zone (Red Alert Midnight Sirens)
   - **Level 4**: Metropolis Center (Storm Fog)
   - **Level 5**: Epicenter Ground Zero (Cataclysm)
   - **Level 6**: Neon Transit Hub (Cyber Indigo)
   - **Level 7**: Hillside Overlook (Alpine Mist)
   - **Level 8**: Bio-Tech Zone (Toxic Hazard)
   - **Level 9**: Coastal Sea Wall (Tempest Squall)
   - **Level 10**: The Apex Citadel (Inferno Megacity)
   - **Level 11**: Subway Concourse (Subterranean Dust)
   - **Level 12**: Aerospace Port (Gale Winds)
   - **Level 13**: Volcanic Rift Trench (Fault Rupture)
   - **Level 14**: Orbital Satellite Uplink (Polar Aurora)
   - **Level 15**: The Omega Sanctuary (10.5 Mw Cataclysm)
   - **Level 16**: Volcanic Ridge Caldera (Basalt Ash)
   - **Level 17**: Airport Runway Hub (Turbulent Gale)
   - **Level 18**: Old Port Canals (High Tide Surge)
   - **Level 19**: High-Tech Silicon Core (EMP Flash)
   - **Level 20**: Armageddon Epicenter (Omega Core 11.0 Mw Cataclysm)
3. **Interactive Survival Collectibles & Pickups**:
   - 🩹 **Medkits**: +350 pts & instantly restores full stamina.
   - ⚡ **Adrenaline Flasks**: +250 pts & +45% sprint speed rush for 7 seconds.
   - ⏳ **Chrono Extenders**: +200 pts & adds +8.0 seconds to evacuation countdown.
   - 🛡 **Shield Spheres**: +300 pts & deflects falling earthquake debris for 8.5 seconds.
4. **Dynamic Obstacle Kicking Physics**: Run into traffic hazard cones or safety drums to kick and nudge them across the asphalt with sliding momentum and score +15 bonus points!
5. **Interactive Sprint & Stamina Bar**: Hold `[F]` to sprint at high velocity (`12.2 m/s` or up to `16.5 m/s` with Adrenaline boost) with dynamic stamina drain and regeneration.
6. **Floating 3D Waypoint Compass Arrow**: A sleek neon-green directional arrow floating 2.8m above the player dynamically points directly to the safe zone in 3D world space.
7. **Real-time Alert Notification Banner**: Translucent in-game HUD popups notify you in real-time when you collect items, kick obstacles, or deflect debris.
8. **Real-time Scoring & High Score System**: Points awarded for remaining time, debris dodged, pickups gathered, and level completion bonuses scaled by your intensity multiplier.
9. **Detailed 3D Animated Player Avatar**: Gamers see their emergency responder character in **3rd Person by default** (with reflective rescue jacket, survival backpack, yellow helmet with cyan headlamp, and articulated walking/jump animations). Press `[V]` to toggle to 1st Person view anytime!

---

## Prerequisites & Compilation

### Option 1: On Windows with MinGW (Pre-configured & Self-contained)
The FreeGLUT headers (`include/GL/`), import libraries (`lib/`), and runtime DLL (`freeglut.dll`) are already bundled in this directory and configured in MinGW:

1. Open a terminal in `cpp_opengl` directory and run:
   ```bash
   mingw32-make
   ```
   *Or compile directly with g++:*
   ```bash
   g++ -O2 -Wall -I./include earthquake_escape.cpp -o earthquake_escape.exe -L./lib -lfreeglut -lopengl32 -lglu32
   ```

2. Run the executable:
   ```bash
   earthquake_escape.exe
   ```

*Or run from the root project folder using `.\run_cpp_game.bat`.*

### Option 2: On Linux (Ubuntu / Debian)
1. Install GLUT and OpenGL development packages:
   ```bash
   sudo apt-get update
   sudo apt-get install build-essential freeglut3-dev libglu1-mesa-dev
   ```
2. Compile and run:
   ```bash
   make
   ./earthquake_escape
   ```

### Option 3: Visual Studio
1. Create a new C++ Empty Project in Visual Studio.
2. Add `earthquake_escape.cpp` to the Source Files.
3. Install FreeGLUT via NuGet Package Manager (`Install-Package freeglut`).
4. Build and Run (F5).

---

## Controls
- **1, 2, 3, 4 / Mouse Click**: Select Seismic Intensity on Start Screen
- **1..9, 0, A..J / Mouse Click**: Select Mission Level (Levels 1 to 20)
- **W, A, S, D / Arrow Keys**: Move Forward / Left / Backward / Right
- **F / E**: Sprint / Rush (Uses Stamina)
- **Q, E / Mouse Left-Click & Drag**: Rotate Camera Angle
- **Spacebar**: Jump over low debris & obstacles (or launch mission)
- **V**: Toggle 3rd Person / 1st Person View
- **R**: Restart Current Level
- **L**: Level Selection Menu (20 Levels 4x5 Grid)
- **M**: Return to Intensity Selection Menu
- **ESC**: Exit Game
