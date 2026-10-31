// Earthquake Escape - Earthquake Physics & Simulation Controller
// Implements sinusoidal seismic wave transformations, building sway, debris physics, and camera shake.

class EarthquakeController {
    constructor(scene, soundEngine) {
        this.scene = scene;
        this.soundEngine = soundEngine;

        // Quake State
        this.state = 'IDLE'; // 'IDLE', 'WARNING', 'ACTIVE', 'AFTERSHOCK'
        this.intensity = 1.0;
        this.richterScale = 5.2;
        this.timer = 0;
        this.duration = 45;
        this.trauma = 0.0; // Camera shake trauma (0 to 1)

        // Wave parameters
        this.waveTime = 0;
        this.currentDisplacement = new THREE.Vector3();
        this.currentTilt = new THREE.Euler();

        // Active falling & fallen debris
        this.fallingDebris = [];
        this.activeObstacleColliders = []; // Dynamic AABBs blocking escape routes

        // Dust Particle System
        this.dustSystem = null;
        this.dustPositions = null;
        this.initDustParticles();
    }

    setDifficulty(levelNumber = 1, intensityPreset = 'medium') {
        this.levelNumber = levelNumber;

        // Intensity Presets
        const presets = {
            low:     { richter: 5.5, mult: 0.75, timeBonus: 8 },
            medium:  { richter: 7.0, mult: 1.15, timeBonus: 0 },
            high:    { richter: 8.2, mult: 1.65, timeBonus: -6 }
        };

        const presetKey = (typeof intensityPreset === 'string' && presets[intensityPreset]) 
            ? intensityPreset 
            : 'medium';
        const preset = presets[presetKey];

        // Level Base Settings
        const levelSettings = {
            1: { baseDuration: 52, weather: 'dust' },
            2: { baseDuration: 46, weather: 'dust' },
            3: { baseDuration: 40, weather: 'dust' },
            4: { baseDuration: 35, weather: 'rain' },
            5: { baseDuration: 30, weather: 'ash' },
            6: { baseDuration: 42, weather: 'spark' },
            7: { baseDuration: 38, weather: 'mist' },
            8: { baseDuration: 34, weather: 'toxic' },
            9: { baseDuration: 30, weather: 'rain' },
            10: { baseDuration: 28, weather: 'ash' }
        };
        const lvlSetting = levelSettings[levelNumber] || {
            baseDuration: Math.max(20, 48 - (levelNumber - 1) * 0.3),
            weather: ['dust', 'ash', 'rain', 'mist', 'spark', 'toxic'][(levelNumber - 1) % 6]
        };

        this.intensity = preset.mult * (1.0 + Math.min(2.0, (levelNumber - 1) * 0.035));
        this.richterScale = Math.min(9.9, preset.richter + Math.min(2.5, (levelNumber - 1) * 0.04));
        this.duration = Math.max(18, lvlSetting.baseDuration + preset.timeBonus);
        this.weatherType = lvlSetting.weather;

        // Configure weather particle system for this level
        this.configureWeatherSystem(this.weatherType);
    }

    initDustParticles() {
        // Alias for backwards compatibility, defaults to dust
        this.configureWeatherSystem('dust');
    }

    configureWeatherSystem(weatherType = 'dust') {
        if (this.dustSystem) {
            this.scene.remove(this.dustSystem);
            if (this.dustSystem.geometry) this.dustSystem.geometry.dispose();
            if (this.dustSystem.material) this.dustSystem.material.dispose();
            this.dustSystem = null;
        }

        this.weatherType = weatherType;
        const particleCount = (weatherType === 'rain' || weatherType === 'mist') ? 3200 : 1800;
        const geometry = new THREE.BufferGeometry();
        const positions = new Float32Array(particleCount * 3);
        const velocities = [];

        const range = 180;
        for (let i = 0; i < particleCount; i++) {
            positions[i * 3] = (Math.random() - 0.5) * range;
            positions[i * 3 + 1] = Math.random() * (weatherType === 'rain' ? 45 : 28);
            positions[i * 3 + 2] = (Math.random() - 0.5) * range;

            if (weatherType === 'rain') {
                // High-speed falling rain streaks with slight wind drift
                velocities.push({
                    vx: -2.0 + Math.random() * 0.5,
                    vy: -32.0 - Math.random() * 10.0,
                    vz: -1.0 + Math.random() * 0.5
                });
            } else if (weatherType === 'ash') {
                // Swirling apocalyptic embers & ash
                velocities.push({
                    vx: (Math.random() - 0.5) * 3.0,
                    vy: Math.random() * 1.5 - 0.8,
                    vz: (Math.random() - 0.5) * 3.0
                });
            } else if (weatherType === 'spark') {
                // High-speed electrical flickering sparks
                velocities.push({
                    vx: (Math.random() - 0.5) * 4.5,
                    vy: -Math.random() * 8.0 - 2.0,
                    vz: (Math.random() - 0.5) * 4.5
                });
            } else if (weatherType === 'toxic') {
                // Rising toxic gas vapor plumes
                velocities.push({
                    vx: (Math.random() - 0.5) * 1.8,
                    vy: Math.random() * 1.8 + 0.4,
                    vz: (Math.random() - 0.5) * 1.8
                });
            } else if (weatherType === 'mist') {
                // Alpine mountain fog drift
                velocities.push({
                    vx: 1.2 + (Math.random() - 0.5) * 0.6,
                    vy: -0.2 - Math.random() * 0.4,
                    vz: 0.8 + (Math.random() - 0.5) * 0.6
                });
            } else {
                // Ground dust particles
                velocities.push({
                    vx: (Math.random() - 0.5) * 1.5,
                    vy: -Math.random() * 0.8 - 0.2,
                    vz: (Math.random() - 0.5) * 1.5
                });
            }
        }

        geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));

        // Create canvas texture suited to the weather type
        const pCanvas = document.createElement('canvas');
        pCanvas.width = 32;
        pCanvas.height = 32;
        const pctx = pCanvas.getContext('2d');

        if (weatherType === 'rain') {
            // Elongated semi-transparent rain streak
            pctx.fillStyle = 'rgba(186, 230, 253, 0.9)';
            pctx.fillRect(14, 2, 4, 28);
        } else if (weatherType === 'ash') {
            // Glowing fiery ember particle
            const grad = pctx.createRadialGradient(16, 16, 0, 16, 16, 16);
            grad.addColorStop(0, 'rgba(251, 146, 60, 0.95)');
            grad.addColorStop(0.4, 'rgba(239, 68, 68, 0.7)');
            grad.addColorStop(1, 'rgba(69, 10, 10, 0)');
            pctx.fillStyle = grad;
            pctx.fillRect(0, 0, 32, 32);
        } else if (weatherType === 'spark') {
            // High-voltage electric neon spark
            const grad = pctx.createRadialGradient(16, 16, 0, 16, 16, 16);
            grad.addColorStop(0, 'rgba(192, 132, 252, 0.95)');
            grad.addColorStop(0.5, 'rgba(56, 189, 248, 0.75)');
            grad.addColorStop(1, 'rgba(15, 23, 42, 0)');
            pctx.fillStyle = grad;
            pctx.fillRect(0, 0, 32, 32);
        } else if (weatherType === 'toxic') {
            // Noxious chemical green fume puff
            const grad = pctx.createRadialGradient(16, 16, 0, 16, 16, 16);
            grad.addColorStop(0, 'rgba(132, 204, 22, 0.9)');
            grad.addColorStop(0.5, 'rgba(34, 197, 94, 0.4)');
            grad.addColorStop(1, 'rgba(20, 83, 45, 0)');
            pctx.fillStyle = grad;
            pctx.fillRect(0, 0, 32, 32);
        } else if (weatherType === 'mist') {
            // Alpine teal mist droplet
            const grad = pctx.createRadialGradient(16, 16, 0, 16, 16, 16);
            grad.addColorStop(0, 'rgba(204, 251, 241, 0.85)');
            grad.addColorStop(0.5, 'rgba(94, 234, 212, 0.35)');
            grad.addColorStop(1, 'rgba(15, 118, 110, 0)');
            pctx.fillStyle = grad;
            pctx.fillRect(0, 0, 32, 32);
        } else {
            // Soft ground dust puff
            const grad = pctx.createRadialGradient(16, 16, 0, 16, 16, 16);
            grad.addColorStop(0, 'rgba(215, 204, 185, 0.8)');
            grad.addColorStop(0.5, 'rgba(180, 165, 140, 0.3)');
            grad.addColorStop(1, 'rgba(150, 140, 120, 0)');
            pctx.fillStyle = grad;
            pctx.fillRect(0, 0, 32, 32);
        }
        const pTex = new THREE.CanvasTexture(pCanvas);

        const material = new THREE.PointsMaterial({
            size: weatherType === 'rain' ? 0.9 : ((weatherType === 'ash' || weatherType === 'spark') ? 1.6 : 1.2),
            map: pTex,
            transparent: true,
            opacity: weatherType === 'rain' ? 0.75 : 0.0,
            depthWrite: false,
            blending: (weatherType === 'ash' || weatherType === 'spark') ? THREE.AdditiveBlending : THREE.NormalBlending
        });

        this.dustSystem = new THREE.Points(geometry, material);
        this.dustPositions = positions;
        this.dustVelocities = velocities;
        this.scene.add(this.dustSystem);
    }

    startWarning() {
        this.state = 'WARNING';
        this.trauma = 0.2;
        if (this.soundEngine) {
            this.soundEngine.startSiren();
            this.soundEngine.playWarningBeep();
        }
    }

    startQuake(hazardPoints = []) {
        this.state = 'ACTIVE';
        this.trauma = 0.95;
        if (this.soundEngine) {
            this.soundEngine.startEarthquakeRumble(this.intensity);
            if (this.weatherType === 'rain') {
                this.soundEngine.playThunder();
            }
        }

        // Spawn falling debris waves from designated hazard points
        this.scheduleDebrisSpawns(hazardPoints);
    }

    scheduleDebrisSpawns(hazardPoints) {
        if (!hazardPoints || hazardPoints.length === 0) return;

        // Stagger debris collapses at intervals
        hazardPoints.forEach((hp, idx) => {
            const delay = (1.2 + idx * 1.8 + Math.random() * 1.6) * 1000;
            setTimeout(() => {
                if (this.state === 'ACTIVE' || this.state === 'AFTERSHOCK') {
                    this.spawnDebrisChunk(hp);
                }
            }, delay);
        });
    }

    spawnDebrisChunk(hazardPoint) {
        const debrisGroup = new THREE.Group();
        debrisGroup.position.copy(hazardPoint.origin);

        const hazardType = hazardPoint.type || 'concrete';

        if (hazardType === 'container') {
            // Level 4: Tumbling shipping container block
            const cGeo = new THREE.BoxGeometry(3.0, 3.0, 7.5);
            const containerColors = [0xb91c1c, 0x1d4ed8, 0xd97706, 0x047857];
            const cMat = new THREE.MeshStandardMaterial({
                color: containerColors[Math.floor(Math.random() * containerColors.length)],
                roughness: 0.7,
                metalness: 0.5
            });
            const cMesh = new THREE.Mesh(cGeo, cMat);
            cMesh.castShadow = true;
            debrisGroup.add(cMesh);
        } else if (hazardType === 'brick' || hazardType === 'arch') {
            // Level 3: Brick and stone masonry rubble
            const count = 5 + Math.floor(Math.random() * 4);
            const brickGeo = new THREE.BoxGeometry(1.6, 0.9, 1.2);
            const brickMat = new THREE.MeshStandardMaterial({
                color: 0x991b1b,
                roughness: 0.9
            });
            for (let i = 0; i < count; i++) {
                const mesh = new THREE.Mesh(brickGeo, brickMat);
                mesh.position.set(
                    (Math.random() - 0.5) * 3.5,
                    (Math.random() - 0.5) * 2.0,
                    (Math.random() - 0.5) * 3.5
                );
                mesh.rotation.set(Math.random() * Math.PI, Math.random() * Math.PI, 0);
                mesh.castShadow = true;
                debrisGroup.add(mesh);
            }
        } else if (hazardType === 'skybridge') {
            // Level 5: Large structural skybridge girder slab
            const sGeo = new THREE.BoxGeometry(5.0, 1.5, 4.0);
            const sMat = new THREE.MeshStandardMaterial({
                color: 0x334155,
                roughness: 0.6,
                metalness: 0.8
            });
            const sMesh = new THREE.Mesh(sGeo, sMat);
            sMesh.castShadow = true;
            debrisGroup.add(sMesh);
        } else {
            // Concrete rubble chunks
            const count = 3 + Math.floor(Math.random() * 4);
            const debrisGeo = new THREE.DodecahedronGeometry(1.2 + Math.random() * 1.0, 0);
            const debrisMat = new THREE.MeshStandardMaterial({
                color: 0x78716c,
                roughness: 0.95,
                metalness: 0.05
            });

            for (let i = 0; i < count; i++) {
                const mesh = new THREE.Mesh(debrisGeo, debrisMat);
                mesh.position.set(
                    (Math.random() - 0.5) * 2.5,
                    (Math.random() - 0.5) * 1.5,
                    (Math.random() - 0.5) * 2.5
                );
                mesh.rotation.set(Math.random() * Math.PI, Math.random() * Math.PI, 0);
                mesh.castShadow = true;
                debrisGroup.add(mesh);
            }
        }

        this.scene.add(debrisGroup);

        // Physics trajectory towards street target
        const startPos = hazardPoint.origin.clone();
        const targetPos = hazardPoint.targetRoadPos.clone();
        const fallTime = Math.sqrt((2 * Math.max(5, startPos.y)) / 18); // Approximate gravity time

        const velX = (targetPos.x - startPos.x) / Math.max(0.8, fallTime);
        const velZ = (targetPos.z - startPos.z) / Math.max(0.8, fallTime);

        this.fallingDebris.push({
            group: debrisGroup,
            velocity: new THREE.Vector3(velX, 0, velZ),
            rotSpeed: new THREE.Vector3(
                (Math.random() - 0.5) * 3,
                (Math.random() - 0.5) * 2,
                (Math.random() - 0.5) * 3
            ),
            targetPos: targetPos,
            landed: false,
            groundY: hazardType === 'container' ? 1.5 : 0.6
        });

        // Add extra shake burst
        this.addTrauma(0.35);
    }

    addTrauma(amount) {
        this.trauma = Math.min(1.0, this.trauma + amount);
    }

    update(delta, buildings, playerPos) {
        this.waveTime += delta;

        // 1. Quake Dynamics & Ground Waves
        if (this.state === 'ACTIVE' || this.state === 'WARNING') {
            const currentAmp = this.state === 'WARNING' ? 0.08 : (0.28 * this.intensity);
            const freq1 = 14.0;
            const freq2 = 22.0;

            // Synthesis of primary P-waves and S-shear waves
            this.currentDisplacement.x = (Math.sin(this.waveTime * freq1) * 0.7 + Math.sin(this.waveTime * freq2 * 1.3) * 0.3) * currentAmp;
            this.currentDisplacement.y = Math.cos(this.waveTime * 18.0) * currentAmp * 0.4; // Vertical heave
            this.currentDisplacement.z = (Math.cos(this.waveTime * freq1 * 0.9) * 0.6 + Math.sin(this.waveTime * 28.0) * 0.4) * currentAmp;

            // 2. Building Sway and Tilt (Amplified along building height)
            if (buildings && buildings.length > 0) {
                const tiltMult = (this.state === 'WARNING' ? 0.005 : 0.024) * this.intensity;
                buildings.forEach(b => {
                    const swayX = Math.sin(this.waveTime * b.vibrationFreq) * b.tiltVulnerability * tiltMult;
                    const swayZ = Math.cos(this.waveTime * (b.vibrationFreq * 0.8)) * b.tiltVulnerability * tiltMult;
                    b.group.rotation.x = b.originalRot.x + swayX;
                    b.group.rotation.z = b.originalRot.z + swayZ;

                    // Micro-displacement at ground base
                    b.group.position.x = b.originalPos.x + this.currentDisplacement.x * 0.5;
                    b.group.position.z = b.originalPos.z + this.currentDisplacement.z * 0.5;
                });
            }

            // 3. Dust / Weather Particle Cloud
            if (this.dustSystem) {
                if (this.weatherType === 'rain') {
                    this.dustSystem.material.opacity = Math.min(0.92, this.dustSystem.material.opacity + delta * 0.4);
                } else if (this.weatherType === 'ash') {
                    this.dustSystem.material.opacity = Math.min(0.85, this.dustSystem.material.opacity + delta * 0.35);
                } else {
                    this.dustSystem.material.opacity = Math.min(0.65, this.dustSystem.material.opacity + delta * 0.35);
                }
            }
        } else {
            // Restore building rotations
            if (buildings) {
                buildings.forEach(b => {
                    b.group.rotation.x = THREE.MathUtils.lerp(b.group.rotation.x, b.originalRot.x, delta * 3);
                    b.group.rotation.z = THREE.MathUtils.lerp(b.group.rotation.z, b.originalRot.z, delta * 3);
                });
            }
            if (this.dustSystem) {
                if (this.weatherType === 'rain') {
                    this.dustSystem.material.opacity = 0.75; // Persistent rain in storm level
                } else if (this.weatherType === 'ash' || this.weatherType === 'spark' || this.weatherType === 'mist') {
                    this.dustSystem.material.opacity = 0.45;
                } else if (this.weatherType === 'toxic') {
                    this.dustSystem.material.opacity = 0.55;
                } else {
                    this.dustSystem.material.opacity = Math.max(0.0, this.dustSystem.material.opacity - delta * 0.2);
                }
            }
        }

        // Animate weather & dust particles
        if (this.dustSystem && this.dustSystem.material.opacity > 0.01) {
            const p = this.dustPositions;
            const count = p.length / 3;
            const isRain = this.weatherType === 'rain';

            for (let i = 0; i < count; i++) {
                const idx = i * 3;
                p[idx] += this.dustVelocities[i].vx * delta * (isRain ? 1.0 : (1 + this.intensity * 2));
                p[idx + 1] += this.dustVelocities[i].vy * delta;
                p[idx + 2] += this.dustVelocities[i].vz * delta * (isRain ? 1.0 : (1 + this.intensity * 2));

                // Respawn above when hitting ground or ceiling
                if (p[idx + 1] < 0.2 || p[idx + 1] > 40) {
                    p[idx + 1] = (this.weatherType === 'toxic') ? 0.3 : (isRain ? (30 + Math.random() * 15) : (20 + Math.random() * 10));
                    if (playerPos) {
                        p[idx] = playerPos.x + (Math.random() - 0.5) * (isRain ? 110 : 80);
                        p[idx + 2] = playerPos.z + (Math.random() - 0.5) * (isRain ? 110 : 80);
                    }
                }
            }
            this.dustSystem.geometry.attributes.position.needsUpdate = true;
        }

        // 4. Update Falling Debris Physics
        this.updateDebris(delta);

        // 5. Decay Camera Trauma exponentially
        this.trauma = Math.max(0.0, this.trauma - delta * 0.45);
    }

    updateDebris(delta) {
        const gravity = 22.0;

        for (let i = this.fallingDebris.length - 1; i >= 0; i--) {
            const d = this.fallingDebris[i];
            if (d.landed) continue;

            // Apply gravity and trajectory
            d.group.position.x += d.velocity.x * delta;
            d.group.position.z += d.velocity.z * delta;
            d.velocity.y -= gravity * delta;
            d.group.position.y += d.velocity.y * delta;

            // Tumbling rotation
            d.group.rotation.x += d.rotSpeed.x * delta;
            d.group.rotation.y += d.rotSpeed.y * delta;
            d.group.rotation.z += d.rotSpeed.z * delta;

            // Check ground collision
            if (d.group.position.y <= d.groundY) {
                d.group.position.y = d.groundY;
                d.landed = true;

                // Play concrete crash sound
                if (this.soundEngine) {
                    this.soundEngine.playDebrisCrash(0.85);
                }

                // Add trauma spike
                this.addTrauma(0.4);

                // Create permanent obstacle AABB bounding box on the street!
                const box = new THREE.Box3();
                box.setFromCenterAndSize(
                    new THREE.Vector3(d.group.position.x, 1.2, d.group.position.z),
                    new THREE.Vector3(3.6, 2.4, 3.6)
                );
                this.activeObstacleColliders.push({
                    box: box,
                    position: d.group.position.clone()
                });
            }
        }
    }

    // Get 6-DOF Camera Shake (Roll, Pitch, Yaw, X, Y, Z offsets) based on trauma^2
    getCameraShake() {
        if (this.trauma <= 0.001) {
            return {
                offsetPos: new THREE.Vector3(),
                offsetRot: new THREE.Euler()
            };
        }

        const shake = this.trauma * this.trauma; // Non-linear feel
        const maxTranslation = 0.45 * this.intensity;
        const maxRotation = 0.06 * this.intensity;

        const time = this.waveTime * 35;
        const ox = (Math.sin(time * 1.1) + Math.sin(time * 2.3) * 0.5) * maxTranslation * shake;
        const oy = (Math.cos(time * 1.4) + Math.sin(time * 3.1) * 0.4) * maxTranslation * shake;
        const oz = (Math.sin(time * 0.9) + Math.cos(time * 1.8) * 0.5) * maxTranslation * shake;

        const rx = (Math.sin(time * 1.3)) * maxRotation * shake;
        const ry = (Math.cos(time * 1.5)) * maxRotation * shake;
        const rz = (Math.sin(time * 2.1)) * maxRotation * shake * 1.5;

        return {
            offsetPos: new THREE.Vector3(ox, oy, oz),
            offsetRot: new THREE.Euler(rx, ry, rz)
        };
    }

    stopQuake() {
        this.state = 'IDLE';
        this.trauma = 0.0;
        if (this.soundEngine) {
            this.soundEngine.stopEarthquakeRumble();
            this.soundEngine.stopSiren();
        }
    }

    clear() {
        this.stopQuake();
        this.fallingDebris.forEach(d => {
            this.scene.remove(d.group);
        });
        this.fallingDebris = [];
        this.activeObstacleColliders = [];
        if (this.dustSystem) {
            this.scene.remove(this.dustSystem);
            if (this.dustSystem.geometry) this.dustSystem.geometry.dispose();
            if (this.dustSystem.material) this.dustSystem.material.dispose();
            this.dustSystem = null;
        }
    }
}

window.EarthquakeController = EarthquakeController;
