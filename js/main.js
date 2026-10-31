// Earthquake Escape - Main Game Loop & State Orchestrator

class EarthquakeGame {
    constructor() {
        this.container = document.getElementById('canvas-container');
        this.scene = null;
        this.camera = null;
        this.renderer = null;
        this.clock = new THREE.Clock();

        // Subsystems
        this.soundEngine = window.soundEngine;
        this.city = null;
        this.earthquake = null;
        this.player = null;
        this.cgInspector = null;
        this.ui = new UIManager();

        // Game State
        this.currentLevel = 1;
        this.gameState = 'MENU'; // 'MENU', 'PLAYING', 'GAMEOVER', 'VICTORY'
        this.gameTime = 0;       // Elapsed play time in seconds
        this.totalDuration = 45;
        this.timeLeft = 45;
        this.quakeTriggerTime = 7.0; // Seconds before quake begins
        this.warningTriggerTime = 4.0;

        this.sunLight = null;
        this.ambientLight = null;

        this.init();
    }

    init() {
        // 1. Create Scene & Atmospheric Fog
        this.scene = new THREE.Scene();
        this.scene.background = new THREE.Color(0x0f172a);
        this.scene.fog = new THREE.FogExp2(0x1e293b, 0.008);

        // 2. Camera Setup (Perspective Projection)
        const aspect = window.innerWidth / window.innerHeight;
        this.camera = new THREE.PerspectiveCamera(65, aspect, 0.1, 800);
        this.camera.position.set(0, 1.6, 0);

        // 3. WebGL Renderer with Shadows
        this.renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: 'high-performance' });
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
        this.renderer.shadowMap.enabled = true;
        this.renderer.shadowMap.type = THREE.PCFSoftShadowMap;
        this.container.appendChild(this.renderer.domElement);

        // 4. Lighting System
        this.setupLighting();

        // 5. Initialize Subsystems
        this.city = new CityBuilder(this.scene);
        this.earthquake = new EarthquakeController(this.scene, this.soundEngine);
        this.player = new PlayerController(this.camera, this.renderer.domElement, this.soundEngine);
        this.scene.add(this.player.characterGroup);

        this.cgInspector = new CGInspector(this.scene, this.camera, this.renderer);
        this.ui.init();

        // Hook up UI callbacks
        this.ui.setStartCallback((lvl, intens) => this.startLevel(lvl, intens));
        this.ui.setRestartCallback((lvl, intens) => this.startLevel(lvl, intens));

        // Connect HUD buttons
        const btnCam = document.getElementById('btn-hud-cam');
        if (btnCam) {
            btnCam.addEventListener('click', () => {
                const newMode = this.player.toggleCameraMode();
                btnCam.textContent = `📷 View: ${newMode} [V]`;
            });
        }

        const btnInsp = document.getElementById('btn-hud-inspector');
        if (btnInsp) {
            btnInsp.addEventListener('click', () => this.cgInspector.toggle());
        }

        // Window resize
        window.addEventListener('resize', () => this.onWindowResize());

        // Start render loop
        this.animate();
    }

    setupLighting() {
        // Natural sunlight
        this.sunLight = new THREE.DirectionalLight(0xfff5eb, 1.3);
        this.sunLight.position.set(60, 110, 45);
        this.sunLight.castShadow = true;
        this.sunLight.shadow.mapSize.width = 2048;
        this.sunLight.shadow.mapSize.height = 2048;
        this.sunLight.shadow.camera.near = 10;
        this.sunLight.shadow.camera.far = 280;
        const d = 90;
        this.sunLight.shadow.camera.left = -d;
        this.sunLight.shadow.camera.right = d;
        this.sunLight.shadow.camera.top = d;
        this.sunLight.shadow.camera.bottom = -d;
        this.sunLight.shadow.bias = -0.0005;
        this.scene.add(this.sunLight);

        // Ambient Fill
        this.ambientLight = new THREE.AmbientLight(0x38bdf8, 0.35);
        this.scene.add(this.ambientLight);

        // Sky & Ground Hemisphere Light
        const hemiLight = new THREE.HemisphereLight(0x93c5fd, 0x1e293b, 0.4);
        this.scene.add(hemiLight);
    }

    startLevel(levelNumber = 1, intensityPreset = 'medium') {
        this.currentLevel = levelNumber;
        this.currentIntensity = intensityPreset;
        this.gameState = 'PLAYING';
        this.gameTime = 0;

        // Reset Quake & City
        this.earthquake.clear();
        this.earthquake.setDifficulty(levelNumber, intensityPreset);
        this.totalDuration = this.earthquake.duration;
        this.timeLeft = this.totalDuration;

        // Generate City for Level
        const levelData = this.city.buildLevel(levelNumber);
        this.levelData = levelData;

        // Spawn Player
        this.player.spawnAt(levelData.spawnPosition, levelData.safeZoneCenter);

        // Update CG Inspector colliders
        if (this.cgInspector.showColliders) {
            this.cgInspector.buildColliderHelpers(this.city.staticColliders);
        }

        // Update Level indicator in HUD
        const lvlInd = document.getElementById('hud-level-indicator');
        if (lvlInd) {
            lvlInd.textContent = `LEVEL ${levelNumber} •`;
        }

        // Apply environmental lighting based on level theme
        if (levelNumber === 2) {
            // Sunset Golden Hour
            this.sunLight.intensity = 1.1;
            this.sunLight.color.setHex(0xfbbf24);
            this.ambientLight.color.setHex(0xf97316);
            this.scene.fog.color.setHex(0x3b1d11);
            this.scene.fog.density = 0.008;
        } else if (levelNumber === 3) {
            // Industrial Twilight Alert
            this.sunLight.intensity = 0.9;
            this.sunLight.color.setHex(0xf87171);
            this.ambientLight.color.setHex(0xef4444);
            this.scene.fog.color.setHex(0x1e1515);
            this.scene.fog.density = 0.008;
        } else if (levelNumber === 4) {
            // Rainstorm Mist
            this.sunLight.intensity = 0.7;
            this.sunLight.color.setHex(0x93c5fd);
            this.ambientLight.color.setHex(0x38bdf8);
            this.scene.fog.color.setHex(0x0f172a);
            this.scene.fog.density = 0.009;
        } else if (levelNumber === 5) {
            // Ground Zero Crimson Smoke
            this.sunLight.intensity = 0.6;
            this.sunLight.color.setHex(0xf43f5e);
            this.ambientLight.color.setHex(0x881337);
            this.scene.fog.color.setHex(0x18050e);
            this.scene.fog.density = 0.009;
        } else if (levelNumber === 6) {
            // Neon Cyber Indigo
            this.sunLight.intensity = 0.85;
            this.sunLight.color.setHex(0x818cf8);
            this.ambientLight.color.setHex(0xa855f7);
            this.scene.fog.color.setHex(0x0f0b1e);
            this.scene.fog.density = 0.009;
        } else if (levelNumber === 7) {
            // Mountain Alpine Dawn
            this.sunLight.intensity = 1.0;
            this.sunLight.color.setHex(0x2dd4bf);
            this.ambientLight.color.setHex(0x0d9488);
            this.scene.fog.color.setHex(0x062e28);
            this.scene.fog.density = 0.009;
        } else if (levelNumber === 8) {
            // Bio-Hazard Toxic Green Alert
            this.sunLight.intensity = 0.75;
            this.sunLight.color.setHex(0xa3e635);
            this.ambientLight.color.setHex(0x4d7c0f);
            this.scene.fog.color.setHex(0x0d1a08);
            this.scene.fog.density = 0.010;
        } else if (levelNumber === 9) {
            // Ocean Tempest Slate Navy
            this.sunLight.intensity = 0.65;
            this.sunLight.color.setHex(0x38bdf8);
            this.ambientLight.color.setHex(0x0369a1);
            this.scene.fog.color.setHex(0x081726);
            this.scene.fog.density = 0.011;
        } else if (levelNumber === 10) {
            // Level 10: The Apex Citadel - Amber Inferno
            this.sunLight.intensity = 0.95;
            this.sunLight.color.setHex(0xf59e0b);
            this.ambientLight.color.setHex(0x991b1b);
            this.scene.fog.color.setHex(0x1a0505);
            this.scene.fog.density = 0.012;
        } else if (levelNumber === 11) {
            // Level 11: Subway Concourse - Subterranean Cobalt Emergency Lights
            this.sunLight.intensity = 0.55;
            this.sunLight.color.setHex(0x60a5fa);
            this.ambientLight.color.setHex(0x1e3a8a);
            this.scene.fog.color.setHex(0x050c1a);
            this.scene.fog.density = 0.013;
        } else if (levelNumber === 12) {
            // Level 12: Skyline Promenade - High-Altitude Twilight Violet
            this.sunLight.intensity = 0.85;
            this.sunLight.color.setHex(0xc084fc);
            this.ambientLight.color.setHex(0x6b21a8);
            this.scene.fog.color.setHex(0x140526);
            this.scene.fog.density = 0.010;
        } else if (levelNumber === 13) {
            // Level 13: Industrial Rail Yards - Smoldering Rust & Ash
            this.sunLight.intensity = 0.80;
            this.sunLight.color.setHex(0xfb923c);
            this.ambientLight.color.setHex(0x7c2d12);
            this.scene.fog.color.setHex(0x1a0c05);
            this.scene.fog.density = 0.012;
        } else if (levelNumber === 14) {
            // Level 14: Cathedral District - Deep Emerald Cloister Twilight
            this.sunLight.intensity = 0.85;
            this.sunLight.color.setHex(0xa3e635);
            this.ambientLight.color.setHex(0x3f6212);
            this.scene.fog.color.setHex(0x0a1705);
            this.scene.fog.density = 0.011;
        } else if (levelNumber === 15) {
            // Level 15: Omega Epicenter - 10.5 Mw Apocalyptic Void Crimson
            this.sunLight.intensity = 0.65;
            this.sunLight.color.setHex(0xf43f5e);
            this.ambientLight.color.setHex(0x881337);
            this.scene.fog.color.setHex(0x180208);
            this.scene.fog.density = 0.014;
        } else if (levelNumber === 16) {
            // Level 16: Volcanic Ridge Caldera - Basalt Ash & Fiery Magma Glow
            this.sunLight.intensity = 0.75;
            this.sunLight.color.setHex(0xf97316);
            this.ambientLight.color.setHex(0x7c2d12);
            this.scene.fog.color.setHex(0x1c0c06);
            this.scene.fog.density = 0.013;
        } else if (levelNumber === 17) {
            // Level 17: Airport Runway Hub - Turbulent Gale & Cold Runway Slate
            this.sunLight.intensity = 0.90;
            this.sunLight.color.setHex(0xe2e8f0);
            this.ambientLight.color.setHex(0x334155);
            this.scene.fog.color.setHex(0x0f172a);
            this.scene.fog.density = 0.012;
        } else if (levelNumber === 18) {
            // Level 18: Old Port Canals - High Tide Oceanic Teal Mist
            this.sunLight.intensity = 0.80;
            this.sunLight.color.setHex(0x38bdf8);
            this.ambientLight.color.setHex(0x0f766e);
            this.scene.fog.color.setHex(0x041d24);
            this.scene.fog.density = 0.013;
        } else if (levelNumber === 19) {
            // Level 19: High-Tech Silicon Core - EMP Electric Neon Aurora
            this.sunLight.intensity = 0.70;
            this.sunLight.color.setHex(0xa855f7);
            this.ambientLight.color.setHex(0x0284c7);
            this.scene.fog.color.setHex(0x110726);
            this.scene.fog.density = 0.012;
        } else if (levelNumber >= 20) {
            // Level 20: Armageddon Epicenter - 11.0 Mw Planetary Titan Cataclysm
            this.sunLight.intensity = 0.55;
            this.sunLight.color.setHex(0xf43f5e);
            this.ambientLight.color.setHex(0xbe123c);
            this.scene.fog.color.setHex(0x21020d);
            this.scene.fog.density = 0.015;
        } else {
            // Level 1 Daylight
            this.sunLight.intensity = 1.3;
            this.sunLight.color.setHex(0xfff5eb);
            this.ambientLight.color.setHex(0x38bdf8);
            this.scene.fog.color.setHex(0x1e293b);
            this.scene.fog.density = 0.008;
        }
        this.city.setEmergencyMode(false);
        this.ui.hideWarningBanner();

        // Sound resume
        this.soundEngine.resume();
        this.soundEngine.stopEarthquakeRumble();
        this.soundEngine.stopSiren();
    }

    update(delta) {
        if (this.gameState !== 'PLAYING') return;

        this.gameTime += delta;
        this.timeLeft = Math.max(0, this.totalDuration - this.gameTime);

        // 1. Earthquake Timeline Stages
        let stageName = "1. EXPLORING CITY";

        if (this.gameTime >= this.warningTriggerTime && this.gameTime < this.quakeTriggerTime) {
            stageName = "2. ⚠️ QUAKE WARNING";
            if (this.earthquake.state !== 'WARNING') {
                this.earthquake.startWarning();
                this.ui.showWarningBanner("EARTHQUAKE IMMINENT - TREMORS DETECTED!");
            }
        } else if (this.gameTime >= this.quakeTriggerTime) {
            stageName = "3. 🚨 QUAKE ACTIVE - ESCAPE!";
            if (this.earthquake.state !== 'ACTIVE') {
                this.earthquake.startQuake(this.levelData.hazardPoints);
                this.ui.showWarningBanner("VIOLENT EARTHQUAKE! HEAD TO GREEN SAFE ZONE!");
                
                // Emergency lighting switch
                this.sunLight.intensity = 0.45;
                this.sunLight.color.setHex(0xd97706); // Murky amber sky
                this.scene.fog.density = 0.022;       // Thick dust haze
                this.city.setEmergencyMode(true);
            }
        }

        // 2. Update Earthquake Physics, Building Sway & Debris
        this.earthquake.update(delta, this.levelData.buildings, this.player.position);

        // 3. Get 6-DOF Camera Shake
        const camShake = this.earthquake.getCameraShake();

        // 4. Update Player Physics, Movement & Collisions
        this.player.update(
            delta,
            this.city.staticColliders,
            this.earthquake.activeObstacleColliders,
            camShake
        );

        // 5. Update Safe Zone Pulsing Animation
        this.city.update(this.gameTime, delta);

        // 6. Check Distance to Safe Zone
        const distToSafe = this.player.position.distanceTo(this.levelData.safeZoneCenter);

        // 7. Update HUD & Minimap
        this.ui.updateHUD(
            this.timeLeft,
            this.totalDuration,
            stageName,
            this.earthquake.richterScale,
            distToSafe,
            this.player.stamina,
            this.player.cameraMode,
            this.player,
            this.levelData.safeZoneCenter,
            this.levelData.routeName
        );

        this.ui.renderMinimap(
            this.player,
            this.levelData.safeZoneCenter,
            this.earthquake.activeObstacleColliders
        );

        // 8. Update CG Concept Inspector
        this.cgInspector.update(this.player, this.earthquake, this.city.safeZone);

        // 9. Win / Lose Conditions Check
        // Win Condition: Reached inside Safe Zone radius
        if (distToSafe <= this.city.safeZone.radius) {
            this.handleVictory();
        }
        // Lose Condition: Timer expired
        else if (this.timeLeft <= 0) {
            this.handleGameOver("TIME EXPIRED");
        }
    }

    handleVictory() {
        this.gameState = 'VICTORY';
        this.earthquake.stopQuake();
        this.soundEngine.playVictory();
        this.ui.hideWarningBanner();

        // Calculate score with intensity multiplier
        const baseScore = 2000;
        const timeBonus = this.timeLeft * 110;
        const diffBonus = this.currentLevel * 1000;
        const intMult = (this.ui.intensityData[this.currentIntensity] && this.ui.intensityData[this.currentIntensity].mult) || 1.0;
        const finalScore = (baseScore + timeBonus + diffBonus) * intMult;

        this.ui.showResultModal(true, this.gameTime, this.timeLeft, this.currentLevel, finalScore, this.earthquake.richterScale);
    }

    handleGameOver(reason = "FAILED") {
        this.gameState = 'GAMEOVER';
        this.earthquake.stopQuake();
        this.soundEngine.playGameOver();
        this.ui.hideWarningBanner();

        const finalScore = Math.max(100, (this.totalDuration - this.timeLeft) * 35);
        this.ui.showResultModal(false, this.gameTime, 0, this.currentLevel, finalScore, this.earthquake.richterScale);
    }

    onWindowResize() {
        if (!this.camera || !this.renderer) return;
        this.camera.aspect = window.innerWidth / window.innerHeight;
        this.camera.updateProjectionMatrix();
        this.renderer.setSize(window.innerWidth, window.innerHeight);
    }

    animate() {
        requestAnimationFrame(() => this.animate());
        const delta = Math.min(0.08, this.clock.getDelta());
        this.update(delta);
        this.renderer.render(this.scene, this.camera);
    }
}

// Instantiate on DOM load
window.addEventListener('DOMContentLoaded', () => {
    window.game = new EarthquakeGame();
});
