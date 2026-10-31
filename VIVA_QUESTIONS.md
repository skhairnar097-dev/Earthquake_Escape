# Earthquake Escape: Viva & Presentation Q&A Guide
### Interactive 3D Earthquake Simulation & Emergency Escape Game
**Course:** Computer Graphics Mini-Project  
**Author:** Shubham Khairnar  

---

## 1. Why was this project selected?
**Answer:**  
Traditional Computer Graphics mini-projects often showcase static 3D models or simple non-interactive simulations. **Earthquake Escape** was selected because it combines:
1. **Interactive Real-Time Graphics:** A fully controllable player exploring a dynamic 3D urban environment.
2. **Procedural Physics & Animation:** Mathematical synthesis of seismic waves (Primary and Secondary shear waves) displacing ground and structures in real time.
3. **Core Curriculum Application:** Direct demonstration of all fundamental Computer Graphics syllabus topics—3D Affine Transformations (Translation, Rotation, Scaling), View & Projection Matrices, Lighting models, and Collision Detection.
4. **Educational & Societal Value:** It serves as a disaster management escape simulation, emphasizing prompt evacuation toward designated open safe zones during natural catastrophes.

---

## 2. Which Computer Graphics transformations are used?
**Answer:**  
All three primary 3D affine transformations are extensively used, implemented via $4 \times 4$ homogeneous transformation matrices:

### A. Translation ($T$)
Used to update the player's 3D position vector, the kinematics of falling concrete debris, and the high-frequency displacement of the ground plane during tremors.
$$\mathbf{T}(t_x, t_y, t_z) = \begin{bmatrix} 1 & 0 & 0 & t_x \\ 0 & 1 & 0 & t_y \\ 0 & 0 & 1 & t_z \\ 0 & 0 & 0 & 1 \end{bmatrix}$$
- **Falling Debris Kinematics:** $\vec{p}(t) = \vec{p}_0 + \vec{v}_0 t + \frac{1}{2}\vec{g}t^2$
- **Ground Seismic Displacement:** $\vec{d}(t) = (d_x(t), d_y(t), d_z(t))^T$

### B. Rotation ($R$)
Applied around multiple axes:
1. **Building Swaying:** Buildings rotate slightly along the $X$ and $Z$ axes during seismic resonance:
   $$\mathbf{R}_x(\theta) = \begin{bmatrix} 1 & 0 & 0 & 0 \\ 0 & \cos\theta & -\sin\theta & 0 \\ 0 & \sin\theta & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}, \quad \mathbf{R}_z(\psi) = \begin{bmatrix} \cos\psi & -\sin\psi & 0 & 0 \\ \sin\psi & \cos\psi & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}$$
2. **Camera Shake & Mouse Look:** Camera yaw (rotation about $Y$) and pitch (rotation about $X$), combined with rotational camera trauma (roll about $Z$).
3. **Tumbling Rubble:** Fallen debris rotates angularly while falling.

### C. Scaling ($S$)
Applied to non-rigid objects and visual indicators:
$$\mathbf{S}(s_x, s_y, s_z) = \begin{bmatrix} s_x & 0 & 0 & 0 \\ 0 & s_y & 0 & 0 \\ 0 & 0 & s_z & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}$$
- **Safe Zone Beacon Rings:** Pulsing rings scaling periodically using $s(t) = 1.0 + A \sin(\omega t)$.
- **HUD Alarm & Minimap Pulse:** Warning elements dynamically scale up and down to alert the player.

---

## 3. How is earthquake shaking generated?
**Answer:**  
Earthquake ground motion and building resonance are mathematically modeled using **Multi-Harmonic Sinusoidal Wave Synthesis** combined with **Trauma Decay**:

1. **Wave Equations:**
   - **Horizontal P & S Waves ($X$-axis):**
     $$d_x(t) = \left( A_1 \sin(\omega_1 t) + A_2 \sin(\omega_2 t + \phi) \right) \cdot I$$
   - **Vertical Heave ($Y$-axis):**
     $$d_y(t) = A_{\text{vert}} \cos(\omega_3 t) \cdot I$$
   - **Transverse Waves ($Z$-axis):**
     $$d_z(t) = \left( A_1 \cos(\omega_1 t \cdot 0.8) + A_2 \sin(\omega_4 t) \right) \cdot I$$
   *(Where $I$ is the Richter intensity scaling factor, $\omega$ are angular frequencies, and $A$ are wave amplitudes).*

2. **Building Height Amplification:**
   In real civil engineering, high-rise buildings experience greater deflection at the top than at the base. We model this by scaling the sway angle proportionally with building height:
   $$\theta_{\text{sway}} = \text{swayOffset} \times \sin(t \cdot \omega_{\text{building}}) \times \left(\frac{h}{H_{\text{total}}}\right) \times I$$

3. **Trauma-Based Camera Shake:**
   Camera jitter utilizes an exponential trauma model:
   $$\text{Shake} = \text{Trauma}^2 \times I$$
   Every frame, trauma decays continuously ($\text{Trauma} \leftarrow \max(0, \text{Trauma} - \lambda \Delta t)$), while debris impacts and violent tremors inject fresh trauma spikes.

---

## 4. How is camera movement implemented?
**Answer:**  
The camera system supports both **First-Person (FPS)** and **Third-Person (TPS)** view modes:

1. **View Matrix Transformation:**  
   In OpenGL, this is evaluated using `gluLookAt(eye, center, up)`. In Three.js, it is handled via the camera's inverse world matrix $\mathbf{V} = (\mathbf{M}_{\text{cam}})^{-1}$.
2. **First-Person Perspective:**  
   - Position: Placed directly at the player's eye coordinates: $(x, y + 1.65, z)$.
   - Orientation: Euler angles (Pitch $\theta$ for looking up/down clamped between $-85^\circ$ and $+85^\circ$, and Yaw $\psi$ for turning $360^\circ$).
3. **Third-Person Perspective:**  
   - Camera eye is positioned at a backward distance offset $\vec{d} = (0, 2.2, 4.5)$ rotated by the player's yaw angle $\psi$:
     $$\vec{P}_{\text{cam}} = \vec{P}_{\text{player}} + \mathbf{R}_y(\psi) \begin{bmatrix} 0 \\ 2.2 \\ 4.5 \end{bmatrix}$$
   - The camera looks at $\vec{P}_{\text{player}} + (0, 0.8, 0)$.
4. **Earthquake Shake Injection:**  
   The evaluated translational and rotational shake offsets $(\Delta x, \Delta y, \Delta z)$ and $(\Delta \text{pitch}, \Delta \text{yaw}, \Delta \text{roll})$ are added on top of the camera matrix before rendering each frame.

---

## 5. How does collision detection work?
**Answer:**  
The project implements **Axis-Aligned Bounding Box (AABB)** collision detection with **Sliding Response Resolution**:

1. **Mathematical Condition for Overlap:**  
   Two bounding boxes $A$ and $B$ intersect if and only if they overlap along all three orthogonal Cartesian axes ($X$, $Y$, and $Z$):
   $$\text{Overlap}_x = (A_{x,\min} \le B_{x,\max}) \land (A_{x,\max} \ge B_{x,\min})$$
   $$\text{Overlap}_y = (A_{y,\min} \le B_{y,\max}) \land (A_{y,\max} \ge B_{y,\min})$$
   $$\text{Overlap}_z = (A_{z,\min} \le B_{z,\max}) \land (A_{z,\max} \ge B_{z,\min})$$
   $$\text{Intersect}(A, B) = \text{Overlap}_x \land \text{Overlap}_y \land \text{Overlap}_z$$

2. **Sliding Collision Response:**  
   Instead of halting the player completely upon contact, each coordinate axis ($X$ and $Z$) is tested independently:
   - If moving along $X$ causes an intersection, only velocity along $X$ is cancelled ($v_x = 0$), allowing velocity along $Z$ to proceed.
   - This produces smooth wall-sliding around building corners and fallen obstacles without jittering.

3. **Dynamic Obstacles:**  
   When concrete debris hits the street, its bounding box is dynamically registered into the active collision list, physically blocking routes and requiring the player to navigate around them.

---

## 6. How are earthquake intensities and level progression structured?
**Answer:**  
The project decouples **Seismic Intensity Selection (Step 1)** from **Sequential Level Progression (Step 2)**:

1. **Step 1 - Intensity Selection (Richter Scale $M_w$):**  
   The player first chooses the earthquake intensity, which dynamically tunes the mathematical simulation parameters across all levels:
   - **Low (5.5 $M_w$):** Mild ground waves, light debris, extended evacuation window (Score Multiplier: 1.0x).
   - **Medium (7.0 $M_w$):** Active structural resonance, falling masonry, balanced evacuation window (Score Multiplier: 1.5x).
   - **High (8.2 $M_w$):** Violent ground heave, dense roadblocks, urgent evacuation window (Score Multiplier: 2.2x).

2. **Step 2 - Sequential Level Progression (1 to 100 Levels in 5 Tiers):**  
   The campaign features **100 progressive urban evacuation levels** organized into 5 Tiers (20 levels per tier), unlocked sequentially one-by-one:
   - **Tier 1 (Levels 1 to 20 - Metropolitan Core):** Downtown Plaza, Financial District, Industrial Sector, Neon Transit Hub, up to Level 20 Armageddon Epicenter.
   - **Tier 2 (Levels 21 to 40 - Sub-Surface & Fault Lines):** Sub-Level Stations, Fracture Corridors, Industrial Trenches, and Transit Tunnels.
   - **Tier 3 (Levels 41 to 60 - Skyway & Alpine Zones):** High-Altitude Overpasses, Skyway Runways, and Alpine Fault Escapes.
   - **Tier 4 (Levels 61 to 80 - Catacomb & Extreme Trenches):** Subterranean Aqueducts, Catacomb Lines, and Rupture Basins.
   - **Tier 5 (Levels 81 to 100 - Planetary Armageddon):** Deep Core Sanctuaries, Continental Faults, culminating in Level 100 Grand Zenith Sanctuary.

3. **Continuous Progression Loop & Level 20 -> 21 Unlock:**  
   Upon reaching the safe zone in any level, a prominent **"MISSION SUCCESSFUL"** screen displays performance statistics (time left bonus, debris dodged, difficulty multiplier, total score) and automatically unlocks the next level.  
   When **all initial 20 levels are completed**, the game automatically unlocks **Tier 2 (Level 21)** and displays a special announcement banner. The player seamlessly proceeds through levels 21, 22, 23, 24 up to 100.  
   The Level Selection menu features a **5-Tier Paginated Bar** (`[P]` Prev Tier, `[N]` Next Tier, 5 Tier tabs) and a clean 4-column × 5-row responsive grid (20 levels per tier) with direct hotkeys (`1..9, 0, A..J`), arrow navigation, and mouse selection.

---

## 7. What are the game's win and lose conditions?
**Answer:**  
- **Win Condition (Victory):**  
  The player successfully reaches the green **Emergency Safe Zone** before the countdown timer hits zero:
  $$\|\vec{P}_{\text{player}} - \vec{P}_{\text{safeZone}}\| \le R_{\text{safeZone}}$$
- **Lose Condition (Game Over):**  
  The countdown timer reaches $0.0$ seconds before reaching the safe assembly area, representing failure to evacuate during structural collapse.
- **Scoring System:**  
  $$\text{Score} = \text{Base} + (\text{TimeRemaining} \times 110) + (\text{LevelMultiplier} \times 1000) + \text{StaminaBonus}$$

---

## 8. Which parts are simulation and which parts are game mechanics?
**Answer:**  

| Component | Category | Description |
|---|---|---|
| **Seismic Wave Synthesis** | *Simulation* | Harmonic sinusoidal oscillator displacing vertices and ground coordinates. |
| **Building Sway Dynamics** | *Simulation* | Structural resonance bending building groups based on height-moment ratios. |
| **Debris Kinematics** | *Simulation* | Numerical integration of Newtonian gravity and rotational angular velocity: $\vec{a} = -g\hat{j}$. |
| **Atmospheric Dust Haze** | *Simulation* | Particle system simulation with turbulence drift and opacity shifts. |
| **Trauma Camera Shake** | *Simulation* | 6-DOF non-linear camera displacement simulating mechanical seismograph motion. |
| **Countdown Timer** | *Game Mechanic* | Urgency-inducing time constraint for game loop progression. |
| **Score & Performance Rank** | *Game Mechanic* | Rewards fast escape times and hazard avoidance (S, A, B, C rank). |
| **Radar / Minimap** | *Game Mechanic* | Spatial HUD radar guiding navigation towards the beacon waypoint. |
| **Sprint Stamina System** | *Game Mechanic* | Resource management limiting continuous high-speed movement. |
| **Safe Zone Assembly Area** | *Game Mechanic* | Target objective defining the game's win condition. |

---

## 9. How does the game achieve the modern Unity 3D engine visual look and aesthetic?
**Answer:**  
To match the high visual fidelity of contemporary 3D engines like Unity (URP / Standard Lit Pipeline), the graphics pipeline incorporates eight specialized rendering and shading techniques:

1. **Unity-Style Dual-Light Rig:**  
   Unlike basic OpenGL programs that only use a single light source, the game implements a dual-light setup:  
   - `GL_LIGHT0`: Directional Key Light (warm high-angle sunlight/moonlight creating crisp shadows).  
   - `GL_LIGHT1`: Hemisphere Sky Bounce Fill Light (soft cool ambient fill from the opposite angle), ensuring architectural crevices and shadowed building facades retain color and ambient depth instead of appearing pitch black.

2. **PBR-Style Specular Highlights & Local Viewer:**  
   Enabled `glShadeModel(GL_SMOOTH)` (Gouraud interpolation) and `glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE)`. Configured surface material properties (`GL_SPECULAR` and `GL_SHININESS = 48.0f`), granting glossy reflections on vehicle lacquer, building windows, asphalt moisture, and safety helmets.

3. **Procedural Atmospheric Sky Dome & Parallax Skyline:**  
   Implemented `drawSkyBackdrop()` rendered at infinite depth buffer distance (`glDepthMask(GL_FALSE)`). Features a celestial sky gradient (deep zenith space navy transitioning down to horizon twilight), distant city skyscraper silhouettes, blinking red/white aviation obstruction hazard beacons, and a radiant celestial sun/moon disk with an atmospheric soft corona halo.

4. **Detailed Road Geometry & Urban Infrastructure:**  
   - **Zebra Crosswalks:** Pedestrian striped crossing zones (`drawZebraCrosswalk()`) at all four intersection approaches.  
   - **Double Solid Amber Medians:** High-visibility road divider lanes.  
   - **Cast-Iron Manhole Covers:** Sewer covers with concentric traction rings (`drawManholeCover()`).  
   - **3D Raised Concrete Curbs:** Beveled curb borders with 45° chamfers separating street asphalt from pedestrian sidewalks.

5. **Grounded Contact Shadows (Ambient Decals):**  
   `drawContactShadow()` projects soft, alpha-blended radial ambient occlusion disks beneath the player avatar, parked vehicles, and fallen debris blocks, solidly anchoring all 3D objects onto the asphalt.

6. **Volumetric Illumination Pools & Headlights:**  
   - Forward-projecting volumetric headlight cones from vehicle grilles onto the roadway.  
   - Police cars and ambulances cast pulsating red and blue emergency siren light pools onto the pavement.  
   - Street lampposts project warm sodium circular light pools on the sidewalks.

7. **Dynamic FX Particle Simulation System:**  
   High-performance particle engine simulating multiple particle classes:  
   - *Tremor Dust Billows:* Expanding ground dust clouds rising from vibrating foundations.  
   - *Electrical Wire Sparks:* High-voltage blue/yellow sparks showering and bouncing off pavement from severed lamppost wiring.  
   - *Footstep Dust Bursts:* Rapid dust puffs emitted behind the player's boots while walking or sprinting.  
   - *Atmospheric Motes:* Ambient ash and dust motes gently drifting with turbulent wind vectors.

8. **Cinematic Post-Processing Stack & 6-DOF Camera Trauma:**  
   - *Screen-Space Vignette:* Dark radial falloff drawing focus to the viewport center, with a pulsing red danger perimeter when evacuation time is critical.  
   - *6-DOF Camera Trauma:* Camera shake includes rotational pitch wobble (`camShakePitch`) and physical roll tilt (`camShakeRoll`) during heavy seismic quakes.  
   - *3D Floating Holographic Waypoint:* A 3D emerald diamond billboard rotating high above the safe zone, always facing the camera.  
   - *Radar Minimap:* Features concentric range rings (25m, 50m) and a continuous 360° rotating radar sweep line.

---

## 10. How are Target Locations and Evacuation Routes managed across the 100 levels?
**Answer:**  
Rather than reusing generic cardinal points or repeating the same center vault, **all 100 levels feature 100% unique, distinct target locations and player spawn coordinates**:
1. **Zero Repeated Target Coordinates:**  
   Each of the 100 levels has a dedicated, verified `(safeZoneX, safeZoneZ)` coordinate paired with an opposing or diagonal `(playerX, playerZ)` spawn location.
2. **Diverse Urban Architectural Refuges:**  
   Targets span across all urban sectors—North/South/East/West avenue ends, outer corner helipads, subterranean transit shafts, elevated skyway decks, industrial rail hangars, coastal maritime outposts, and inner cross-avenue intersections.
3. **Guaranteed Collision-Free Open Paths:**  
   All 100 targets and spawns are mathematically validated against the city building collision grid (`checkCollision()`) to ensure the safe zone and starting plaza are situated in wide-open avenues and plazas.
4. **Dynamic Synchronized HUD & 3D Waypoints:**  
   - The **3D Waypoint Compass Arrow** calculates the exact heading vector $\theta = \operatorname{atan2}(\Delta X, -\Delta Z)$ toward the level's specific target.
   - The **Radar Minimap** maps the safe zone's exact spatial position with a glowing emerald blip and real-time range rings.
   - **Roadway Surface Chevrons** dynamically project luminous green directional arrows along the asphalt connecting the player directly to their objective.
   - The **HUD Route Banner** dynamically displays the cardinal direction, unique target name, heading in degrees, and precise evacuation distance in meters.
