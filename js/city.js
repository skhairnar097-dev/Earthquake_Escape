// 100 Unique Level Layout Targets & Spawns (Collision-free, 100% Distinct Target Locations)
const LEVEL_TARGET_NAMES = [
    // Tier 1 (1-20)
    "South Park Refuge", "East Harbor Dock", "North Expressway Overpass", "West Coastal Stadium Bunker",
    "Central Underground Vault", "Northeast Helipad Evacuation Deck", "Southwest Mountain Tunnel Shelter", "Northwest Decontamination Dome",
    "Southeast Lighthouse Citadel", "Apex High-Altitude Bastion", "West Metro Ventilation Shaft", "East Terminal Cargo Hangar",
    "South Geothermal Shield Vault", "North Uplink Transmission Dome", "Grand Omega Sanctuary Core", "Mountain Geothermal Station",
    "Aircraft Hangar Blast Shelter", "Maritime Fort Outpost", "Faraday Electromagnetic Shield", "Titan Deep Core Sanctuary",

    // Tier 2 (21-40)
    "Catacomb Emergency Airlock", "Geothermal Coolant Vault", "Maglev Evacuation Shuttle", "Drainage Pump Station Bunker",
    "Refinery Fire Suppression Vault", "Maritime Coastguard Vessel", "Alpine Weather Radar Station", "Sub-Zero Cryo-Shield Chamber",
    "Geode Crystal Sanctuary", "Blast-Resistant Iron Vault", "Trestle Emergency Service Pad", "Bio-Dome Environmental Pod",
    "Heavy Gantry Crane Bastion", "Quarry Excavator Shelter", "Coastal Aqueduct Pump Gate", "Metro Traction Power Vault",
    "Seismic Early Warning Bunker", "Lead-Lined Radioactive Vault", "Sunken Plaza Evacuation Pod", "Under-City Fortress Core",

    // Tier 3 (41-60)
    "Tectonic Fault Pressure Valve", "Neutralization Containment Dome", "Ashfall Gorge Overhang", "Gamma Radiation Blast Shield",
    "Subterranean Missile Silo Cap", "Emergency Gas Shutoff Vault", "Sulfur Flats Extraction Pod", "Coolant Overflow Bastion",
    "Locomotive Roundhouse Shelter", "Centennial Megacity Vault", "Storm Surge Barrier Control", "Geyser Field Pressure Outpost",
    "Cargo Dispatch Center Bunker", "Lightning Arrester Terminal", "Chasm Bridge Anchor Station", "Blastwave Hardened Bunker",
    "Ore Crusher Steel Shelter", "Hazmat Quarantine Citadel", "Obsidian Ridge Lookout Post", "Titan Caldera Deep Vault",

    // Tier 4 (61-80)
    "Permafrost Station Alpha", "Glacier Tunnel Ice Shelter", "Tidal Wave Early Warning Spire", "Archipelago Causeway Tower",
    "Offshore Helipad Evacuation Hub", "Lighthouse Foghorn Sanctuary", "Skyway Suspension Pillar Base", "Cloudbreak Mountain Lodge",
    "Permafrost Research Shelter", "Abyssal Geothermal Core", "Sub-Sea Tube Anchor Gate", "Tramway Cable Anchor Bastion",
    "Iron Mine Excavation Shelter", "Turbine Substation Safe Room", "Frostfire Peak Summit Bastion", "Thunderhead Radar Sanctuary",
    "Avalanche Deflection Wall Bunker", "Tsunami Wall Command Outpost", "Zenith Observation Deck", "Apex Sanctum Grand Core",

    // Tier 5 (81-100)
    "Plasma Magnetic Containment Vault", "Quantum Stabilization Capsule", "Crustal Plate Anchor Shelter", "Void Edge Seismic Platform",
    "Antimatter Pulse Safe Core", "Orbital Silo Command Bunker", "Megastructure Foundation Bastion", "Hyper-Resonance Dampener Room",
    "Abyss Bridge Structural Pod", "Eclipse Solar Array Vault", "Magma Deflection Shield Dome", "Zero-G Inertia Chamber",
    "Event Horizon Safety Airpod", "Matrix Omega Core Sanctuary", "Gravitational Singularity Chamber", "World-Split Fault Bastion",
    "Celestial Beam Array Shelter", "Omega Bastion Prime Vault", "Continental Armageddon Outpost", "THE GRAND ZENITH SANCTUARY CORE"
];

const LEVEL_TARGETS = [
    { tx: 0.0, tz: 38.0, px: 0.0, pz: -38.0 }, // Level 1
    { tx: 38.0, tz: 0.0, px: -38.0, pz: 0.0 }, // Level 2
    { tx: 0.0, tz: -40.0, px: 0.0, pz: 38.0 }, // Level 3
    { tx: -40.0, tz: 0.0, px: 38.0, pz: 0.0 }, // Level 4
    { tx: 0.0, tz: 0.0, px: -35.0, pz: -44.0 }, // Level 5
    { tx: 44.0, tz: -44.0, px: -44.0, pz: 44.0 }, // Level 6
    { tx: -44.0, tz: 44.0, px: 44.0, pz: -44.0 }, // Level 7
    { tx: -44.0, tz: -44.0, px: 44.0, pz: 44.0 }, // Level 8
    { tx: 44.0, tz: 44.0, px: -44.0, pz: -44.0 }, // Level 9
    { tx: 6.0, tz: -44.0, px: -6.0, pz: 44.0 }, // Level 10
    { tx: -44.0, tz: 8.0, px: 44.0, pz: -8.0 }, // Level 11
    { tx: 44.0, tz: -8.0, px: -44.0, pz: 8.0 }, // Level 12
    { tx: -8.0, tz: 44.0, px: 8.0, pz: -44.0 }, // Level 13
    { tx: 26.5, tz: -44.0, px: -26.5, pz: 44.0 }, // Level 14
    { tx: -26.5, tz: 26.5, px: 26.5, pz: -26.5 }, // Level 15
    { tx: -26.5, tz: -44.0, px: 26.5, pz: 44.0 }, // Level 16
    { tx: 44.0, tz: 26.5, px: -44.0, pz: -26.5 }, // Level 17
    { tx: 26.5, tz: 44.0, px: -26.5, pz: -44.0 }, // Level 18
    { tx: -44.0, tz: -26.5, px: 44.0, pz: 26.5 }, // Level 19
    { tx: -6.0, tz: -6.0, px: 44.0, pz: 44.0 }, // Level 20
    { tx: -35.0, tz: -44.0, px: 35.0, pz: 44.0 }, // Level 21
    { tx: 0.0, tz: 44.0, px: -0.0, pz: -44.0 }, // Level 22
    { tx: 44.0, tz: -35.0, px: -44.0, pz: 35.0 }, // Level 23
    { tx: -44.0, tz: 9.0, px: 44.0, pz: -9.0 }, // Level 24
    { tx: 0.0, tz: -42.0, px: -0.0, pz: 42.0 }, // Level 25
    { tx: -5.0, tz: -26.5, px: 5.0, pz: 26.5 }, // Level 26
    { tx: 5.0, tz: -15.0, px: -5.0, pz: 15.0 }, // Level 27
    { tx: 0.0, tz: 26.5, px: -0.0, pz: -26.5 }, // Level 28
    { tx: 0.0, tz: 42.0, px: -0.0, pz: -42.0 }, // Level 29
    { tx: -32.0, tz: -5.0, px: 32.0, pz: 5.0 }, // Level 30
    { tx: -20.0, tz: 5.0, px: 20.0, pz: -5.0 }, // Level 31
    { tx: 20.0, tz: 0.0, px: -20.0, pz: -0.0 }, // Level 32
    { tx: 38.0, tz: -5.0, px: -38.0, pz: 5.0 }, // Level 33
    { tx: -26.5, tz: -20.0, px: 26.5, pz: 20.0 }, // Level 34
    { tx: 26.5, tz: -26.5, px: -26.5, pz: 26.5 }, // Level 35
    { tx: -40.0, tz: -26.5, px: 40.0, pz: 26.5 }, // Level 36
    { tx: -40.0, tz: 26.5, px: 40.0, pz: -26.5 }, // Level 37
    { tx: -6.0, tz: -3.0, px: 6.0, pz: 3.0 }, // Level 38
    { tx: -3.0, tz: 6.0, px: 3.0, pz: -6.0 }, // Level 39
    { tx: 3.0, tz: 3.0, px: -3.0, pz: -3.0 }, // Level 40
    { tx: -26.5, tz: 44.0, px: 26.5, pz: -44.0 }, // Level 41
    { tx: 18.0, tz: 44.0, px: -18.0, pz: -44.0 }, // Level 42
    { tx: -44.0, tz: 0.0, px: 44.0, pz: -0.0 }, // Level 43
    { tx: 5.0, tz: -42.0, px: -5.0, pz: 42.0 }, // Level 44
    { tx: -5.0, tz: -20.0, px: 5.0, pz: 20.0 }, // Level 45
    { tx: 0.0, tz: 20.0, px: -0.0, pz: -20.0 }, // Level 46
    { tx: -5.0, tz: 42.0, px: 5.0, pz: -42.0 }, // Level 47
    { tx: -32.0, tz: 5.0, px: 32.0, pz: -5.0 }, // Level 48
    { tx: 15.0, tz: -5.0, px: -15.0, pz: 5.0 }, // Level 49
    { tx: 32.0, tz: 0.0, px: -32.0, pz: -0.0 }, // Level 50
    { tx: -26.5, tz: -10.0, px: 26.5, pz: 10.0 }, // Level 51
    { tx: 26.5, tz: 10.0, px: -26.5, pz: -10.0 }, // Level 52
    { tx: 20.0, tz: -26.5, px: -20.0, pz: 26.5 }, // Level 53
    { tx: 40.0, tz: 26.5, px: -40.0, pz: -26.5 }, // Level 54
    { tx: 0.0, tz: -3.0, px: -0.0, pz: 3.0 }, // Level 55
    { tx: 6.0, tz: 0.0, px: -6.0, pz: -0.0 }, // Level 56
    { tx: 9.0, tz: -44.0, px: -9.0, pz: 44.0 }, // Level 57
    { tx: 44.0, tz: -9.0, px: -44.0, pz: 9.0 }, // Level 58
    { tx: 0.0, tz: -38.0, px: -0.0, pz: 38.0 }, // Level 59
    { tx: 0.0, tz: -15.0, px: -0.0, pz: 15.0 }, // Level 60
    { tx: 5.0, tz: 32.0, px: -5.0, pz: -32.0 }, // Level 61
    { tx: -32.0, tz: 0.0, px: 32.0, pz: -0.0 }, // Level 62
    { tx: 15.0, tz: 5.0, px: -15.0, pz: -5.0 }, // Level 63
    { tx: 42.0, tz: 0.0, px: -42.0, pz: -0.0 }, // Level 64
    { tx: 26.5, tz: -32.0, px: -26.5, pz: 32.0 }, // Level 65
    { tx: 10.0, tz: -26.5, px: -10.0, pz: 26.5 }, // Level 66
    { tx: -6.0, tz: 3.0, px: 6.0, pz: -3.0 }, // Level 67
    { tx: 3.0, tz: -3.0, px: -3.0, pz: 3.0 }, // Level 68
    { tx: -9.0, tz: -44.0, px: 9.0, pz: 44.0 }, // Level 69
    { tx: -44.0, tz: -9.0, px: 44.0, pz: 9.0 }, // Level 70
    { tx: -5.0, tz: -32.0, px: 5.0, pz: 32.0 }, // Level 71
    { tx: -5.0, tz: 20.0, px: 5.0, pz: -20.0 }, // Level 72
    { tx: -42.0, tz: 5.0, px: 42.0, pz: -5.0 }, // Level 73
    { tx: -15.0, tz: 5.0, px: 15.0, pz: -5.0 }, // Level 74
    { tx: 42.0, tz: 5.0, px: -42.0, pz: -5.0 }, // Level 75
    { tx: 26.5, tz: 20.0, px: -26.5, pz: -20.0 }, // Level 76
    { tx: -10.0, tz: 26.5, px: 10.0, pz: -26.5 }, // Level 77
    { tx: 0.0, tz: 3.0, px: -0.0, pz: -3.0 }, // Level 78
    { tx: -18.0, tz: 44.0, px: 18.0, pz: -44.0 }, // Level 79
    { tx: 44.0, tz: 9.0, px: -44.0, pz: -9.0 }, // Level 80
    { tx: 5.0, tz: -26.5, px: -5.0, pz: 26.5 }, // Level 81
    { tx: -5.0, tz: 38.0, px: 5.0, pz: -38.0 }, // Level 82
    { tx: -20.0, tz: 0.0, px: 20.0, pz: -0.0 }, // Level 83
    { tx: 42.0, tz: -5.0, px: -42.0, pz: 5.0 }, // Level 84
    { tx: 26.5, tz: 32.0, px: -26.5, pz: -32.0 }, // Level 85
    { tx: -6.0, tz: 0.0, px: 6.0, pz: -0.0 }, // Level 86
    { tx: 6.0, tz: -3.0, px: -6.0, pz: 3.0 }, // Level 87
    { tx: 44.0, tz: -26.5, px: -44.0, pz: 26.5 }, // Level 88
    { tx: 5.0, tz: -32.0, px: -5.0, pz: 32.0 }, // Level 89
    { tx: 5.0, tz: 38.0, px: -5.0, pz: -38.0 }, // Level 90
    { tx: 15.0, tz: 0.0, px: -15.0, pz: -0.0 }, // Level 91
    { tx: -26.5, tz: 20.0, px: 26.5, pz: -20.0 }, // Level 92
    { tx: -32.0, tz: 26.5, px: 32.0, pz: -26.5 }, // Level 93
    { tx: 3.0, tz: 0.0, px: -3.0, pz: -0.0 }, // Level 94
    { tx: -44.0, tz: -35.0, px: 44.0, pz: 35.0 }, // Level 95
    { tx: 0.0, tz: -20.0, px: -0.0, pz: 20.0 }, // Level 96
    { tx: -38.0, tz: -5.0, px: 38.0, pz: 5.0 }, // Level 97
    { tx: 32.0, tz: -5.0, px: -32.0, pz: 5.0 }, // Level 98
    { tx: -32.0, tz: -26.5, px: 32.0, pz: 26.5 }, // Level 99
    { tx: 0.0, tz: -6.0, px: -0.0, pz: 6.0 }, // Level 100
];

class CityBuilder {
    constructor(scene) {
        this.scene = scene;
        this.cityGroup = new THREE.Group();
        this.scene.add(this.cityGroup);

        this.buildings = [];
        this.staticColliders = []; // Array of THREE.Box3 for player collision
        this.hazardPoints = [];    // Locations where debris will crash
        this.safeZone = null;
        this.safeZoneBounds = null;
        this.streetLights = [];
        this.emergencyLights = [];
        this.citySize = 140;       // Default size in meters

        // Cached shared procedural materials & geometries
        this.textures = this.generateProceduralTextures();
        this.materials = this.createMaterials();
    }

    generateProceduralTextures() {
        // Procedural window facade canvas
        const windowCanvas = document.createElement('canvas');
        windowCanvas.width = 256;
        windowCanvas.height = 512;
        const ctx = windowCanvas.getContext('2d');

        // Dark concrete background
        ctx.fillStyle = '#222831';
        ctx.fillRect(0, 0, 256, 512);

        // Draw glowing office / apartment windows
        const rows = 16;
        const cols = 6;
        const padX = 8;
        const padY = 8;
        const winW = (256 - (cols + 1) * padX) / cols;
        const winH = (512 - (rows + 1) * padY) / rows;

        for (let r = 0; r < rows; r++) {
            for (let c = 0; c < cols; c++) {
                const x = padX + c * (winW + padX);
                const y = padY + r * (winH + padY);
                // Randomized warm yellow, cool cyan, or unlit
                const rand = Math.random();
                if (rand > 0.45) {
                    ctx.fillStyle = rand > 0.75 ? '#ffeaa7' : '#74b9ff';
                    ctx.shadowColor = ctx.fillStyle;
                    ctx.shadowBlur = 4;
                } else {
                    ctx.fillStyle = '#111827';
                    ctx.shadowBlur = 0;
                }
                ctx.fillRect(x, y, winW, winH);
            }
        }
        const buildingTex = new THREE.CanvasTexture(windowCanvas);
        buildingTex.wrapS = THREE.RepeatWrapping;
        buildingTex.wrapT = THREE.RepeatWrapping;

        // Asphalt road canvas with center dashed lines
        const roadCanvas = document.createElement('canvas');
        roadCanvas.width = 128;
        roadCanvas.height = 256;
        const rctx = roadCanvas.getContext('2d');
        rctx.fillStyle = '#1e232a';
        rctx.fillRect(0, 0, 128, 256);

        // Dashed yellow line in center
        rctx.strokeStyle = '#f1c40f';
        rctx.lineWidth = 4;
        rctx.setLineDash([20, 20]);
        rctx.beginPath();
        rctx.moveTo(64, 0);
        rctx.lineTo(64, 256);
        rctx.stroke();

        // White boundary solid lines
        rctx.strokeStyle = '#ecf0f1';
        rctx.lineWidth = 3;
        rctx.setLineDash([]);
        rctx.beginPath();
        rctx.moveTo(8, 0);
        rctx.lineTo(8, 256);
        rctx.moveTo(120, 0);
        rctx.lineTo(120, 256);
        rctx.stroke();

        const roadTex = new THREE.CanvasTexture(roadCanvas);
        roadTex.wrapS = THREE.RepeatWrapping;
        roadTex.wrapT = THREE.RepeatWrapping;

        // Safe zone ground chevron texture
        const safeCanvas = document.createElement('canvas');
        safeCanvas.width = 256;
        safeCanvas.height = 256;
        const sctx = safeCanvas.getContext('2d');
        sctx.fillStyle = '#052e16';
        sctx.fillRect(0, 0, 256, 256);
        sctx.strokeStyle = '#22c55e';
        sctx.lineWidth = 12;
        sctx.strokeRect(6, 6, 244, 244);
        sctx.font = 'bold 28px sans-serif';
        sctx.fillStyle = '#4ade80';
        sctx.textAlign = 'center';
        sctx.textBaseline = 'middle';
        sctx.fillText('EMERGENCY', 128, 90);
        sctx.fillText('SAFE ZONE', 128, 135);
        sctx.font = 'bold 44px sans-serif';
        sctx.fillText('⛑', 128, 190);

        const safeTex = new THREE.CanvasTexture(safeCanvas);

        return { buildingTex, roadTex, safeTex };
    }

    createMaterials() {
        return {
            road: new THREE.MeshStandardMaterial({
                map: this.textures.roadTex,
                roughness: 0.85,
                metalness: 0.1
            }),
            sidewalk: new THREE.MeshStandardMaterial({
                color: 0x94a3b8,
                roughness: 0.9,
                metalness: 0.05
            }),
            buildingFacade: new THREE.MeshStandardMaterial({
                map: this.textures.buildingTex,
                roughness: 0.6,
                metalness: 0.2
            }),
            buildingRoof: new THREE.MeshStandardMaterial({
                color: 0x334155,
                roughness: 0.9
            }),
            foliage: new THREE.MeshLambertMaterial({
                color: 0x166534
            }),
            wood: new THREE.MeshLambertMaterial({
                color: 0x582f0e
            }),
            vehicleRed: new THREE.MeshStandardMaterial({
                color: 0xd90429,
                roughness: 0.3,
                metalness: 0.8
            }),
            vehicleBlue: new THREE.MeshStandardMaterial({
                color: 0x0077b6,
                roughness: 0.3,
                metalness: 0.8
            }),
            vehicleWhite: new THREE.MeshStandardMaterial({
                color: 0xf8f9fa,
                roughness: 0.4,
                metalness: 0.7
            }),
            metal: new THREE.MeshStandardMaterial({
                color: 0x475569,
                roughness: 0.5,
                metalness: 0.7
            }),
            lampGlow: new THREE.MeshBasicMaterial({
                color: 0xfff3b0
            }),
            emergencyRed: new THREE.MeshBasicMaterial({
                color: 0xef4444
            }),
            safeZonePad: new THREE.MeshStandardMaterial({
                map: this.textures.safeTex,
                roughness: 0.5
            }),
            beaconCylinder: new THREE.MeshBasicMaterial({
                color: 0x22c55e,
                transparent: true,
                opacity: 0.28,
                side: THREE.DoubleSide
            }),
            beaconRing: new THREE.MeshBasicMaterial({
                color: 0x4ade80,
                wireframe: true
            }),
            debrisChunk: new THREE.MeshStandardMaterial({
                color: 0x64748b,
                roughness: 0.95,
                metalness: 0.1
            })
        };
    }

    // Build complete city based on selected level difficulty
    buildLevel(levelNumber = 1) {
        // Clear previous environment
        this.clear();

        // Level parameters
        let roadWidth = 16;
        let blockSize = 26;
        let gridX = 4;
        let gridZ = 4;

        if (levelNumber === 2) {
            blockSize = 28;
            gridZ = 5;
        } else if (levelNumber === 3) {
            blockSize = 28;
            gridZ = 6;
        } else if (levelNumber === 4) {
            gridX = 6;
            blockSize = 28;
            gridZ = 6;
        } else if (levelNumber === 5) {
            gridX = 6;
            blockSize = 28;
            gridZ = 7;
        } else if (levelNumber === 6) {
            gridX = 6;
            blockSize = 28;
            gridZ = 6;
        } else if (levelNumber === 7) {
            gridX = 6;
            blockSize = 28;
            gridZ = 6;
        } else if (levelNumber === 8) {
            gridX = 6;
            blockSize = 28;
            gridZ = 6;
        } else if (levelNumber === 9) {
            gridX = 6;
            blockSize = 28;
            gridZ = 6;
        } else if (levelNumber >= 10) {
            gridX = 6;
            blockSize = 30;
            gridZ = 7;
        }

        const pitchX = blockSize + roadWidth;
        const pitchZ = blockSize + roadWidth;

        const totalW = gridX * pitchX + roadWidth + 60;
        const spanZ = gridZ * pitchZ;
        const totalH = spanZ + roadWidth * 2 + 60;
        this.citySize = Math.max(totalW, totalH);

        // Distinct Target Location & Evacuation Route per Level (100% Unique across all 100 levels)
        const lvlIdx = Math.min(Math.max(1, levelNumber), 100) - 1;
        const coord = LEVEL_TARGETS[lvlIdx];
        const spawnPos = new THREE.Vector3(coord.px, 1.0, coord.pz);
        const safePos = new THREE.Vector3(coord.tx, 0, coord.tz);
        const targetName = LEVEL_TARGET_NAMES[lvlIdx] || "Emergency Safe Zone";

        const dx = safePos.x - spawnPos.x;
        const dz = safePos.z - spawnPos.z;
        const dist = Math.hypot(dx, dz);
        let angle = Math.atan2(dx, -dz) * (180 / Math.PI);
        if (angle < 0) angle += 360;

        let dirStr = "Northbound";
        if (angle >= 22.5 && angle < 67.5) dirStr = "Northeast";
        else if (angle >= 67.5 && angle < 112.5) dirStr = "Eastbound";
        else if (angle >= 112.5 && angle < 157.5) dirStr = "Southeast";
        else if (angle >= 157.5 && angle < 202.5) dirStr = "Southbound";
        else if (angle >= 202.5 && angle < 247.5) dirStr = "Southwest";
        else if (angle >= 247.5 && angle < 292.5) dirStr = "Westbound";
        else if (angle >= 292.5 && angle < 337.5) dirStr = "Northwest";

        const routeName = `${dirStr} -> ${targetName} (${Math.round(angle)}°, ${Math.round(dist)}m Evacuation)`;

        // 1. Build Ground, Main Streets, Corridors & Sidewalks with Directional Chevrons
        this.buildRoadNetwork(gridX, gridZ, blockSize, roadWidth, spawnPos, safePos);

        // 2. Build High-Rise and Mid-Rise Buildings (flanking left and right of open avenues)
        this.buildBuildingBlocks(gridX, gridZ, blockSize, roadWidth, levelNumber);

        // 3. Build Starting Evacuation Assembly Plaza at level spawn
        this.buildEvacuationPlaza(spawnPos.x, spawnPos.z, 22);

        // 4. Place Urban Props on Sidewalks
        this.placeProps(gridX, gridZ, blockSize, roadWidth, spawnPos, safePos);

        // 4b. Place Evacuation Path Obstacles & Vehicles along the road corridors
        this.placePathObstacles(gridX, gridZ, blockSize, roadWidth, spawnPos, safePos, levelNumber);

        // 5. Place Emergency Safe Zone at designated target location
        this.buildSafeZone(safePos.x, safePos.z);

        // 6. Build Boundary Collision Walls
        this.buildBoundaries();

        return {
            spawnPosition: spawnPos,
            safeZoneCenter: safePos,
            routeName: routeName,
            buildings: this.buildings,
            staticColliders: this.staticColliders,
            hazardPoints: this.hazardPoints
        };
    }

    buildRoadNetwork(gridX, gridZ, blockSize, roadWidth, spawnPos, safePos) {
        const pitchX = blockSize + roadWidth;
        const pitchZ = blockSize + roadWidth;
        const totalW = gridX * pitchX + roadWidth + 60;
        const totalH = gridZ * pitchZ + roadWidth + 60;

        // Ground base plane
        const groundGeo = new THREE.PlaneGeometry(totalW, totalH);
        const ground = new THREE.Mesh(groundGeo, this.materials.road);
        ground.rotation.x = -Math.PI / 2;
        ground.position.y = 0;
        ground.receiveShadow = true;
        this.cityGroup.add(ground);
        this.textures.roadTex.repeat.set(gridX * 2, gridZ * 4);

        // Sidewalk blocks on left and right of corridors
        const sidewalkMat = this.materials.sidewalk;
        for (let ix = -gridX / 2; ix < gridX / 2; ix++) {
            const cx = (ix + 0.5) * pitchX;
            for (let iz = -gridZ / 2; iz < gridZ / 2; iz++) {
                const cz = (iz + 0.5) * pitchZ;

                const swGeo = new THREE.BoxGeometry(blockSize + 3, 0.35, blockSize + 3);
                const swMesh = new THREE.Mesh(swGeo, sidewalkMat);
                swMesh.position.set(cx, 0.18, cz);
                swMesh.receiveShadow = true;
                this.cityGroup.add(swMesh);
            }
        }

        // Add Illuminated Green Road Guidance Decals along route pointing towards Safe Zone
        const dx = safePos.x - spawnPos.x;
        const dz = safePos.z - spawnPos.z;
        const dist = Math.hypot(dx, dz);
        const dirX = dx / dist;
        const dirZ = dz / dist;
        const arrowAngle = Math.atan2(dirX, dirZ); // Decal Y rotation

        const arrowGeo = new THREE.PlaneGeometry(3.4, 3.4);
        arrowGeo.rotateX(-Math.PI / 2);
        arrowGeo.rotateY(arrowAngle);

        const arrowCanvas = document.createElement('canvas');
        arrowCanvas.width = 128;
        arrowCanvas.height = 128;
        const actx = arrowCanvas.getContext('2d');
        actx.fillStyle = 'rgba(0,0,0,0)';
        actx.fillRect(0, 0, 128, 128);
        actx.strokeStyle = '#22c55e';
        actx.fillStyle = '#22c55e';
        actx.lineWidth = 14;
        actx.lineCap = 'round';
        actx.lineJoin = 'round';
        // Base chevron pointing along +Z (South)
        actx.beginPath();
        actx.moveTo(24, 34);
        actx.lineTo(64, 88);
        actx.lineTo(104, 34);
        actx.stroke();

        const arrowTex = new THREE.CanvasTexture(arrowCanvas);
        const arrowMat = new THREE.MeshBasicMaterial({
            map: arrowTex,
            transparent: true,
            opacity: 0.85
        });

        // Place arrows every 16 meters along line from spawn to safe
        const stepCount = Math.floor(dist / 16);
        for (let s = 1; s < stepCount; s++) {
            const px = spawnPos.x + dirX * (s * 16);
            const pz = spawnPos.z + dirZ * (s * 16);
            const decal = new THREE.Mesh(arrowGeo, arrowMat);
            decal.position.set(px, 0.05, pz);
            this.cityGroup.add(decal);
        }
    }

    buildEvacuationPlaza(cx, cz, blockSize) {
        const plazaGroup = new THREE.Group();
        plazaGroup.position.set(cx, 0, cz);

        // Ground assembly concrete square
        const padGeo = new THREE.BoxGeometry(blockSize - 4, 0.22, blockSize - 4);
        const padMat = new THREE.MeshStandardMaterial({ color: 0x1e293b, roughness: 0.8 });
        const pad = new THREE.Mesh(padGeo, padMat);
        pad.position.y = 0.11;
        pad.receiveShadow = true;
        plazaGroup.add(pad);

        // Bright yellow caution border
        const borderGeo = new THREE.BoxGeometry(blockSize - 2, 0.05, blockSize - 2);
        const borderMat = new THREE.MeshBasicMaterial({ color: 0xf59e0b });
        const border = new THREE.Mesh(borderGeo, borderMat);
        border.position.y = 0.13;
        plazaGroup.add(border);

        this.cityGroup.add(plazaGroup);
    }

    buildBuildingBlocks(gridX, gridZ, blockSize, roadWidth, levelNumber) {
        let colors = [0x2c3e50, 0x34495e, 0x1f2937, 0x374151, 0x1e293b];
        if (levelNumber === 6) colors = [0x1e1b4b, 0x312e81, 0x2e1065, 0x1e293b]; // Neon Cyber Indigo
        else if (levelNumber === 7) colors = [0x1c1917, 0x292524, 0x334155, 0x1e293b]; // Mountain Slate
        else if (levelNumber === 8) colors = [0x14532d, 0x1e293b, 0x334155, 0x064e3b]; // Bio-Hazard Olive/Charcoal
        else if (levelNumber === 9) colors = [0x0c4a6e, 0x1e293b, 0x0f172a, 0x1e3a8a]; // Ocean Maritime Navy
        else if (levelNumber === 10) colors = [0x450a0a, 0x18181b, 0x09090b, 0x7f1d1d]; // Obsidian & Molten Crimson
        else if (levelNumber === 11) colors = [0x172554, 0x1e3a8a, 0x1e293b, 0x0f172a]; // Subway Subterranean Cobalt & Steel
        else if (levelNumber === 12) colors = [0x3b0764, 0x581c87, 0x1e293b, 0x4c1d95]; // Skyline Promenade Twilight Violet
        else if (levelNumber === 13) colors = [0x713f12, 0x451a03, 0x27272a, 0x3f3f46]; // Rail Yards Rust & Charcoal
        else if (levelNumber === 14) colors = [0x365314, 0x1a2e05, 0x1f2937, 0x292524]; // Cathedral Ancient Stone
        else if (levelNumber === 15) colors = [0x500724, 0x701a75, 0x09090b, 0x18181b]; // Omega Abyssal Void Crimson
        else if (levelNumber === 16) colors = [0x431407, 0x7c2d12, 0x18181b, 0x27272a]; // Volcanic Basalt
        else if (levelNumber === 17) colors = [0x0f172a, 0x1e293b, 0x334155, 0x475569]; // Airport Slate & Steel
        else if (levelNumber === 18) colors = [0x064e3b, 0x0f766e, 0x134e4a, 0x1e293b]; // Ocean Port Aqueduct
        else if (levelNumber === 19) colors = [0x581c87, 0x1e1b4b, 0x0284c7, 0x0369a1]; // High-Tech Silicon EMP
        else if (levelNumber >= 20) colors = [0x701a75, 0x4c0519, 0x09090b, 0x831843]; // Armageddon Core Cataclysm

        const pitchX = blockSize + roadWidth;
        const pitchZ = blockSize + roadWidth;

        for (let ix = -gridX / 2; ix < gridX / 2; ix++) {
            const cx = (ix + 0.5) * pitchX;

            for (let iz = -gridZ / 2; iz < gridZ / 2; iz++) {
                const cz = (iz + 0.5) * pitchZ;

                // Heights vary for dramatic skyline
                const heightBonus = (levelNumber >= 10 ? 40 : (levelNumber >= 6 ? 32 : (levelNumber === 3 ? 35 : 25)));
                const height = 22 + Math.random() * heightBonus;
                const bWidth = blockSize - 4;
                const bDepth = blockSize - 4;

                const buildingGroup = new THREE.Group();
                buildingGroup.position.set(cx, 0, cz);

                // Main tower body
                const bodyGeo = new THREE.BoxGeometry(bWidth, height, bDepth);
                const bodyMat = this.materials.buildingFacade.clone();
                bodyMat.color.setHex(colors[Math.floor(Math.random() * colors.length)]);
                const bodyMesh = new THREE.Mesh(bodyGeo, bodyMat);
                bodyMesh.position.y = height / 2;
                bodyMesh.castShadow = true;
                bodyMesh.receiveShadow = true;
                buildingGroup.add(bodyMesh);

                // Roof structure (HVAC / Tank / Antenna)
                const roofGeo = new THREE.BoxGeometry(bWidth * 0.5, 4, bDepth * 0.5);
                const roofMesh = new THREE.Mesh(roofGeo, this.materials.buildingRoof);
                roofMesh.position.y = height + 2;
                buildingGroup.add(roofMesh);

                if (Math.random() > 0.4) {
                    const antennaGeo = new THREE.CylinderGeometry(0.15, 0.4, 8, 8);
                    const antennaMesh = new THREE.Mesh(antennaGeo, this.materials.metal);
                    antennaMesh.position.y = height + 8;
                    buildingGroup.add(antennaMesh);
                }

                // Add Entrance Lobby Canopy on street side
                const canopyGeo = new THREE.BoxGeometry(7, 0.4, 3.5);
                const canopy = new THREE.Mesh(canopyGeo, this.materials.metal);
                canopy.position.set(0, 3.6, bDepth / 2 + 1.5);
                buildingGroup.add(canopy);

                // Emergency Exit Sign over entrance
                const signGeo = new THREE.BoxGeometry(2.5, 0.8, 0.2);
                const signMat = new THREE.MeshBasicMaterial({ color: 0x22c55e });
                const sign = new THREE.Mesh(signGeo, signMat);
                sign.position.set(0, 4.3, bDepth / 2 + 1.8);
                buildingGroup.add(sign);

                this.cityGroup.add(buildingGroup);

                // Save building metadata for earthquake vibration & tilt
                const bData = {
                    group: buildingGroup,
                    originalPos: buildingGroup.position.clone(),
                    originalRot: buildingGroup.rotation.clone(),
                    height: height,
                    width: bWidth,
                    depth: bDepth,
                    tiltVulnerability: 0.6 + Math.random() * 0.8,
                    vibrationFreq: 3.0 + Math.random() * 2.0
                };
                this.buildings.push(bData);

                // Register collision bounding box (offset by building height/2)
                const box = new THREE.Box3();
                box.setFromCenterAndSize(
                    new THREE.Vector3(cx, height / 2, cz),
                    new THREE.Vector3(bWidth + 0.4, height, bDepth + 0.4)
                );
                this.staticColliders.push(box);

                // Designate hazard falling debris origin targeting boulevard and cross streets!
                if (Math.random() > 0.3) {
                    const toAvenue = (cx < 0 ? 1 : -1);
                    this.hazardPoints.push({
                        origin: new THREE.Vector3(
                            cx + (Math.random() - 0.5) * bWidth * 0.6,
                            height * 0.75,
                            cz + (Math.random() - 0.5) * bDepth * 0.6
                        ),
                        targetRoadPos: new THREE.Vector3(
                            cx + toAvenue * (bWidth / 2 + 5 + Math.random() * 5),
                            0.5,
                            cz + (Math.random() - 0.5) * 8
                        ),
                        buildingRef: bData
                    });
                }
            }
        }
    }

    placeProps(gridX, gridZ, blockSize, roadWidth, spawnPos, safePos) {
        const pitchX = blockSize + roadWidth;
        const pitchZ = blockSize + roadWidth;

        // Place Street Lamps, Trees, Parked Cars along sidewalks & cross streets
        for (let ix = -gridX / 2; ix <= gridX / 2; ix++) {
            const cornerX = ix * pitchX - roadWidth / 2;

            for (let iz = -gridZ / 2; iz <= gridZ / 2; iz++) {
                const cornerZ = iz * pitchZ - roadWidth / 2;

                // Keep both North-South Avenue (abs(X) < 9) and East-West Boulevard (abs(Z) < 9) 100% open
                if (Math.abs(cornerX) < 9 || Math.abs(cornerZ) < 9) continue;

                // Keep spawn and safe zone clear
                if (spawnPos && Math.hypot(cornerX - spawnPos.x, cornerZ - spawnPos.z) < 18) continue;
                if (safePos && Math.hypot(cornerX - safePos.x, cornerZ - safePos.z) < 18) continue;

                // 1. Street Lamp along sidewalk
                if (Math.random() > 0.35) {
                    this.buildStreetLamp(cornerX + 1.2, cornerZ + 1.2);
                }

                // 2. Tree on sidewalk
                if (Math.random() > 0.45) {
                    this.buildTree(cornerX - 2.5, cornerZ + (blockSize / 2));
                }

                // 3. Parked Car along curb on side streets
                if (Math.random() > 0.55 && Math.abs(cornerX) > 10) {
                    const carX = cornerX + roadWidth / 2 + 2;
                    const carZ = cornerZ + (Math.random() * (blockSize - 8));
                    this.buildVehicle(carX, carZ, 0);
                }

                // 4. Concrete Road Barrier on side streets
                if (Math.random() > 0.7 && Math.abs(cornerX) > 12) {
                    this.buildBarricade(cornerX + roadWidth / 2, cornerZ + 6);
                }
            }
        }
    }

    buildStreetLamp(x, z) {
        const lampGroup = new THREE.Group();
        lampGroup.position.set(x, 0, z);

        // Pole
        const poleGeo = new THREE.CylinderGeometry(0.12, 0.18, 5.5, 8);
        const pole = new THREE.Mesh(poleGeo, this.materials.metal);
        pole.position.y = 2.75;
        lampGroup.add(pole);

        // Arm
        const armGeo = new THREE.BoxGeometry(0.12, 0.12, 1.2);
        const arm = new THREE.Mesh(armGeo, this.materials.metal);
        arm.position.set(0, 5.5, 0.5);
        lampGroup.add(arm);

        // Glowing bulb fixture
        const bulbGeo = new THREE.SphereGeometry(0.28, 8, 8);
        const bulb = new THREE.Mesh(bulbGeo, this.materials.lampGlow);
        bulb.position.set(0, 5.3, 1.0);
        lampGroup.add(bulb);

        // Point Light
        const pLight = new THREE.PointLight(0xfff1b8, 0.9, 14, 2);
        pLight.position.set(0, 5.2, 1.0);
        pLight.castShadow = false;
        lampGroup.add(pLight);

        this.cityGroup.add(lampGroup);
        this.streetLights.push({ light: pLight, bulb: bulb });

        // Add small collider for lamp post
        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 2.5, z), new THREE.Vector3(0.5, 5, 0.5));
        this.staticColliders.push(box);
    }

    buildTree(x, z) {
        const treeGroup = new THREE.Group();
        treeGroup.position.set(x, 0, z);

        // Trunk
        const trunkGeo = new THREE.CylinderGeometry(0.3, 0.45, 3, 6);
        const trunk = new THREE.Mesh(trunkGeo, this.materials.wood);
        trunk.position.y = 1.5;
        treeGroup.add(trunk);

        // Foliage spheres (stacked pine/deciduous)
        const f1Geo = new THREE.ConeGeometry(2.2, 3.5, 7);
        const f1 = new THREE.Mesh(f1Geo, this.materials.foliage);
        f1.position.y = 3.6;
        treeGroup.add(f1);

        const f2Geo = new THREE.ConeGeometry(1.6, 2.8, 7);
        const f2 = new THREE.Mesh(f2Geo, this.materials.foliage);
        f2.position.y = 5.2;
        treeGroup.add(f2);

        this.cityGroup.add(treeGroup);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 2, z), new THREE.Vector3(1.0, 4, 1.0));
        this.staticColliders.push(box);
    }

    buildVehicle(x, z, rotY = 0) {
        const carGroup = new THREE.Group();
        carGroup.position.set(x, 0, z);
        carGroup.rotation.y = rotY;

        const mats = [this.materials.vehicleRed, this.materials.vehicleBlue, this.materials.vehicleWhite];
        const carMat = mats[Math.floor(Math.random() * mats.length)];

        // Lower chassis
        const chassisGeo = new THREE.BoxGeometry(2.1, 0.7, 4.4);
        const chassis = new THREE.Mesh(chassisGeo, carMat);
        chassis.position.y = 0.55;
        carGroup.add(chassis);

        // Cabin roof
        const cabinGeo = new THREE.BoxGeometry(1.8, 0.65, 2.3);
        const cabin = new THREE.Mesh(cabinGeo, this.materials.metal);
        cabin.position.set(0, 1.2, -0.2);
        carGroup.add(cabin);

        // 4 Wheels
        const wheelGeo = new THREE.CylinderGeometry(0.35, 0.35, 0.35, 10);
        wheelGeo.rotateZ(Math.PI / 2);
        const wheelMat = this.materials.metal;
        const wheelPositions = [
            [-1.0, 0.35, 1.3],
            [1.0, 0.35, 1.3],
            [-1.0, 0.35, -1.3],
            [1.0, 0.35, -1.3]
        ];
        wheelPositions.forEach(wp => {
            const w = new THREE.Mesh(wheelGeo, wheelMat);
            w.position.set(...wp);
            carGroup.add(w);
        });

        this.cityGroup.add(carGroup);

        // Vehicle collider
        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.8, z), new THREE.Vector3(2.3, 1.6, 4.6));
        this.staticColliders.push(box);
    }

    buildBarricade(x, z) {
        const bGroup = new THREE.Group();
        bGroup.position.set(x, 0, z);

        const bGeo = new THREE.BoxGeometry(3.5, 1.0, 0.5);
        const bMat = new THREE.MeshStandardMaterial({ color: 0xe67e22, roughness: 0.7 });
        const mesh = new THREE.Mesh(bGeo, bMat);
        mesh.position.y = 0.5;
        bGroup.add(mesh);

        // Warning stripes
        const stripeGeo = new THREE.BoxGeometry(3.6, 0.25, 0.52);
        const stripeMat = new THREE.MeshBasicMaterial({ color: 0xffffff });
        const stripe = new THREE.Mesh(stripeGeo, stripeMat);
        stripe.position.y = 0.5;
        bGroup.add(stripe);

        this.cityGroup.add(bGroup);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.5, z), new THREE.Vector3(3.6, 1.2, 0.7));
        this.staticColliders.push(box);
    }

    placePathObstacles(gridX, gridZ, blockSize, roadWidth, spawnPos, safePos, levelNumber) {
        const pitchX = blockSize + roadWidth;
        const pitchZ = blockSize + roadWidth;
        const spanX = (gridX / 2) * pitchX - 8;
        const spanZ = (gridZ / 2) * pitchZ - 8;

        const allCandidates = [];

        // Obstacles along central North-South Avenue (x around 0)
        for (let z = -spanZ + 12; z <= spanZ - 12; z += 12 + Math.random() * 4) {
            const lane = (Math.random() > 0.35) ? ((Math.random() > 0.5) ? -3.4 : 3.4) : 0.0;
            allCandidates.push({ x: lane, z: z, dir: 'NS' });
        }

        // Obstacles along central East-West Boulevard (z around 0)
        for (let x = -spanX + 12; x <= spanX - 12; x += 12 + Math.random() * 4) {
            const lane = (Math.random() > 0.35) ? ((Math.random() > 0.5) ? -3.4 : 3.4) : 0.0;
            allCandidates.push({ x: x, z: lane, dir: 'EW' });
        }

        allCandidates.forEach(pt => {
            // Keep safety breathing room around spawn and safe zone
            if (spawnPos && Math.hypot(pt.x - spawnPos.x, pt.z - spawnPos.z) < 16) return;
            if (safePos && Math.hypot(pt.x - safePos.x, pt.z - safePos.z) < 16) return;

            const r = Math.random();
            const rot = (pt.dir === 'NS') ? (Math.random() * 0.4 - 0.2) : (Math.PI / 2 + Math.random() * 0.4 - 0.2);

            if (r < 0.15) {
                this.buildPoliceCruiser(pt.x, pt.z, rot + (Math.random() - 0.5) * 0.6);
            } else if (r < 0.28) {
                this.buildAmbulance(pt.x, pt.z, rot + (Math.random() - 0.5) * 0.5);
            } else if (r < 0.44) {
                this.buildRoadblock(pt.x, pt.z, rot);
            } else if (r < 0.60) {
                this.buildJerseyBarrier(pt.x, pt.z, rot);
            } else if (r < 0.72) {
                this.buildTrafficConeCluster(pt.x, pt.z);
            } else if (r < 0.84) {
                this.buildSafetyDrum(pt.x, pt.z);
            } else if (r < 0.92) {
                this.buildFallenLamppost(pt.x, pt.z, rot + Math.PI / 4);
            } else {
                this.buildRubblePile(pt.x, pt.z);
            }
        });
    }

    buildPoliceCruiser(x, z, rotY = 0) {
        const car = new THREE.Group();
        car.position.set(x, 0, z);
        car.rotation.y = rotY;

        const bodyMat = new THREE.MeshStandardMaterial({ color: 0x111827, roughness: 0.4, metalness: 0.3 });
        const whiteMat = new THREE.MeshStandardMaterial({ color: 0xf8fafc, roughness: 0.4, metalness: 0.2 });

        const chassisGeo = new THREE.BoxGeometry(2.1, 0.7, 4.4);
        const chassis = new THREE.Mesh(chassisGeo, bodyMat);
        chassis.position.y = 0.55;
        car.add(chassis);

        const doorGeo = new THREE.BoxGeometry(2.14, 0.5, 2.0);
        const doorMesh = new THREE.Mesh(doorGeo, whiteMat);
        doorMesh.position.y = 0.58;
        car.add(doorMesh);

        const cabinGeo = new THREE.BoxGeometry(1.8, 0.65, 2.3);
        const cabin = new THREE.Mesh(cabinGeo, whiteMat);
        cabin.position.set(0, 1.2, -0.2);
        car.add(cabin);

        const winMat = new THREE.MeshStandardMaterial({ color: 0x1e293b, roughness: 0.1, metalness: 0.8 });
        const frontWinGeo = new THREE.BoxGeometry(1.7, 0.5, 0.1);
        const frontWin = new THREE.Mesh(frontWinGeo, winMat);
        frontWin.position.set(0, 1.2, -1.35);
        frontWin.rotation.x = 0.35;
        car.add(frontWin);

        const barBaseGeo = new THREE.BoxGeometry(1.3, 0.12, 0.35);
        const barBase = new THREE.Mesh(barBaseGeo, this.materials.metal);
        barBase.position.set(0, 1.58, -0.2);
        car.add(barBase);

        const blueStrobeGeo = new THREE.BoxGeometry(0.5, 0.18, 0.32);
        const blueMat = new THREE.MeshBasicMaterial({ color: 0x3b82f6 });
        const blueStrobe = new THREE.Mesh(blueStrobeGeo, blueMat);
        blueStrobe.position.set(-0.35, 1.68, -0.2);
        car.add(blueStrobe);

        const redStrobeGeo = new THREE.BoxGeometry(0.5, 0.18, 0.32);
        const redMat = new THREE.MeshBasicMaterial({ color: 0xef4444 });
        const redStrobe = new THREE.Mesh(redStrobeGeo, redMat);
        redStrobe.position.set(0.35, 1.68, -0.2);
        car.add(redStrobe);

        const blueLight = new THREE.PointLight(0x3b82f6, 1.6, 9, 2);
        blueLight.position.set(-0.35, 1.7, -0.2);
        car.add(blueLight);
        this.emergencyLights.push({ light: blueLight, type: 'blue' });

        const redLight = new THREE.PointLight(0xef4444, 1.6, 9, 2);
        redLight.position.set(0.35, 1.7, -0.2);
        car.add(redLight);
        this.emergencyLights.push({ light: redLight, type: 'red' });

        const wheelGeo = new THREE.CylinderGeometry(0.35, 0.35, 0.35, 8);
        wheelGeo.rotateZ(Math.PI / 2);
        const wMat = this.materials.metal;
        [[-1.0, 0.35, 1.3], [1.0, 0.35, 1.3], [-1.0, 0.35, -1.3], [1.0, 0.35, -1.3]].forEach(wp => {
            const w = new THREE.Mesh(wheelGeo, wMat);
            w.position.set(...wp);
            car.add(w);
        });

        this.cityGroup.add(car);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.8, z), new THREE.Vector3(2.3, 1.6, 4.6));
        this.staticColliders.push(box);
    }

    buildAmbulance(x, z, rotY = 0) {
        const amb = new THREE.Group();
        amb.position.set(x, 0, z);
        amb.rotation.y = rotY;

        const whiteMat = new THREE.MeshStandardMaterial({ color: 0xf8fafc, roughness: 0.3 });
        const redMat = new THREE.MeshStandardMaterial({ color: 0xdc2626, roughness: 0.4 });
        const greenMat = new THREE.MeshBasicMaterial({ color: 0x16a34a });

        const bodyGeo = new THREE.BoxGeometry(2.3, 1.7, 5.0);
        const body = new THREE.Mesh(bodyGeo, whiteMat);
        body.position.y = 1.1;
        amb.add(body);

        const stripeGeo = new THREE.BoxGeometry(2.34, 0.35, 4.9);
        const stripe = new THREE.Mesh(stripeGeo, redMat);
        stripe.position.y = 1.1;
        amb.add(stripe);

        const crossVGeo = new THREE.BoxGeometry(2.38, 0.7, 0.22);
        const crossHGeo = new THREE.BoxGeometry(2.38, 0.22, 0.7);
        const crossV = new THREE.Mesh(crossVGeo, greenMat);
        const crossH = new THREE.Mesh(crossHGeo, greenMat);
        crossV.position.set(0, 1.35, 0.5);
        crossH.position.set(0, 1.35, 0.5);
        amb.add(crossV);
        amb.add(crossH);

        const beaconGeo = new THREE.CylinderGeometry(0.25, 0.25, 0.25, 8);
        const beaconMat = new THREE.MeshBasicMaterial({ color: 0xf59e0b });
        const beacon = new THREE.Mesh(beaconGeo, beaconMat);
        beacon.position.set(0, 2.05, -1.2);
        amb.add(beacon);

        const ambLight = new THREE.PointLight(0xf59e0b, 1.8, 10, 2);
        ambLight.position.set(0, 2.2, -1.2);
        amb.add(ambLight);
        this.emergencyLights.push({ light: ambLight, type: 'amber' });

        const wheelGeo = new THREE.CylinderGeometry(0.38, 0.38, 0.35, 8);
        wheelGeo.rotateZ(Math.PI / 2);
        [[-1.05, 0.38, 1.5], [1.05, 0.38, 1.5], [-1.05, 0.38, -1.5], [1.05, 0.38, -1.5]].forEach(wp => {
            const w = new THREE.Mesh(wheelGeo, this.materials.metal);
            w.position.set(...wp);
            amb.add(w);
        });

        this.cityGroup.add(amb);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 1.0, z), new THREE.Vector3(2.5, 2.0, 5.2));
        this.staticColliders.push(box);
    }

    buildRoadblock(x, z, rotY = 0) {
        const rb = new THREE.Group();
        rb.position.set(x, 0, z);
        rb.rotation.y = rotY;

        const legMat = new THREE.MeshStandardMaterial({ color: 0x334155, roughness: 0.8 });
        const legGeo = new THREE.BoxGeometry(0.12, 0.95, 0.6);
        const leftLeg = new THREE.Mesh(legGeo, legMat);
        leftLeg.position.set(-1.4, 0.48, 0);
        const rightLeg = new THREE.Mesh(legGeo, legMat);
        rightLeg.position.set(1.4, 0.48, 0);
        rb.add(leftLeg);
        rb.add(rightLeg);

        const boardGeo = new THREE.BoxGeometry(3.2, 0.35, 0.12);
        const boardMat = new THREE.MeshStandardMaterial({ color: 0xf97316, roughness: 0.5 });
        const board = new THREE.Mesh(boardGeo, boardMat);
        board.position.y = 0.72;
        rb.add(board);

        for (let i = -3; i <= 3; i++) {
            const strGeo = new THREE.BoxGeometry(0.22, 0.36, 0.14);
            const strMat = new THREE.MeshBasicMaterial({ color: 0xffffff });
            const str = new THREE.Mesh(strGeo, strMat);
            str.position.set(i * 0.45, 0.72, 0);
            rb.add(str);
        }

        const lightGeo = new THREE.SphereGeometry(0.14, 6, 6);
        const lightMat = new THREE.MeshBasicMaterial({ color: 0xfbbf24 });
        const cautionLight = new THREE.Mesh(lightGeo, lightMat);
        cautionLight.position.set(0, 0.98, 0);
        rb.add(cautionLight);

        this.cityGroup.add(rb);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.48, z), new THREE.Vector3(3.3, 0.96, 0.7));
        this.staticColliders.push(box);
    }

    buildJerseyBarrier(x, z, rotY = 0) {
        const jb = new THREE.Group();
        jb.position.set(x, 0, z);
        jb.rotation.y = rotY;

        const concMat = new THREE.MeshStandardMaterial({ color: 0x94a3b8, roughness: 0.95 });
        const baseGeo = new THREE.BoxGeometry(3.0, 0.35, 0.75);
        const baseMesh = new THREE.Mesh(baseGeo, concMat);
        baseMesh.position.y = 0.18;
        jb.add(baseMesh);

        const topGeo = new THREE.BoxGeometry(3.0, 0.52, 0.4);
        const topMesh = new THREE.Mesh(topGeo, concMat);
        topMesh.position.y = 0.58;
        jb.add(topMesh);

        const stripeGeo = new THREE.BoxGeometry(3.02, 0.14, 0.42);
        const stripeMat = new THREE.MeshBasicMaterial({ color: 0xfacc15 });
        const stripe = new THREE.Mesh(stripeGeo, stripeMat);
        stripe.position.y = 0.65;
        jb.add(stripe);

        this.cityGroup.add(jb);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.44, z), new THREE.Vector3(3.1, 0.88, 0.8));
        this.staticColliders.push(box);
    }

    buildTrafficConeCluster(x, z) {
        const cluster = new THREE.Group();
        cluster.position.set(x, 0, z);

        const coneMat = new THREE.MeshStandardMaterial({ color: 0xf97316, roughness: 0.4 });
        const whiteMat = new THREE.MeshBasicMaterial({ color: 0xffffff });
        const baseMat = new THREE.MeshStandardMaterial({ color: 0x111827, roughness: 0.9 });

        const offsets = [[-0.4, -0.3], [0.4, -0.2], [0, 0.35]];
        offsets.forEach(off => {
            const single = new THREE.Group();
            single.position.set(off[0], 0, off[1]);

            const bGeo = new THREE.BoxGeometry(0.42, 0.05, 0.42);
            const bMesh = new THREE.Mesh(bGeo, baseMat);
            bMesh.position.y = 0.025;
            single.add(bMesh);

            const cGeo = new THREE.ConeGeometry(0.18, 0.65, 8);
            const cMesh = new THREE.Mesh(cGeo, coneMat);
            cMesh.position.y = 0.35;
            single.add(cMesh);

            const wGeo = new THREE.CylinderGeometry(0.12, 0.14, 0.15, 8);
            const wMesh = new THREE.Mesh(wGeo, whiteMat);
            wMesh.position.y = 0.34;
            single.add(wMesh);

            cluster.add(single);
        });

        this.cityGroup.add(cluster);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.35, z), new THREE.Vector3(1.4, 0.7, 1.4));
        this.staticColliders.push(box);
    }

    buildSafetyDrum(x, z) {
        const drum = new THREE.Group();
        drum.position.set(x, 0, z);

        const drumMat = new THREE.MeshStandardMaterial({ color: 0xf97316, roughness: 0.5 });
        const stripeMat = new THREE.MeshBasicMaterial({ color: 0xfef08a });

        const bGeo = new THREE.CylinderGeometry(0.42, 0.42, 0.92, 10);
        const bMesh = new THREE.Mesh(bGeo, drumMat);
        bMesh.position.y = 0.46;
        drum.add(bMesh);

        [0.32, 0.62].forEach(sy => {
            const sGeo = new THREE.CylinderGeometry(0.43, 0.43, 0.14, 10);
            const sMesh = new THREE.Mesh(sGeo, stripeMat);
            sMesh.position.y = sy;
            drum.add(sMesh);
        });

        this.cityGroup.add(drum);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.46, z), new THREE.Vector3(0.9, 0.92, 0.9));
        this.staticColliders.push(box);
    }

    buildFallenLamppost(x, z, rotY = 0) {
        const pole = new THREE.Group();
        pole.position.set(x, 0, z);
        pole.rotation.y = rotY;

        const pGeo = new THREE.CylinderGeometry(0.12, 0.15, 5.2, 8);
        pGeo.rotateZ(Math.PI / 2);
        const pMesh = new THREE.Mesh(pGeo, this.materials.metal);
        pMesh.position.set(0, 0.18, 0);
        pole.add(pMesh);

        const bGeo = new THREE.BoxGeometry(0.6, 0.35, 0.6);
        const bMesh = new THREE.Mesh(bGeo, this.materials.sidewalk);
        bMesh.position.set(-2.4, 0.18, 0);
        bMesh.rotation.z = 0.2;
        pole.add(bMesh);

        const lGeo = new THREE.BoxGeometry(0.35, 0.2, 0.4);
        const lMesh = new THREE.Mesh(lGeo, this.materials.metal);
        lMesh.position.set(2.6, 0.15, 0);
        pole.add(lMesh);

        const sparkLight = new THREE.PointLight(0x67e8f9, 0.8, 6, 2);
        sparkLight.position.set(2.6, 0.3, 0);
        pole.add(sparkLight);

        this.cityGroup.add(pole);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.25, z), new THREE.Vector3(5.6, 0.5, 0.8));
        this.staticColliders.push(box);
    }

    buildRubblePile(x, z) {
        const rubble = new THREE.Group();
        rubble.position.set(x, 0, z);

        const stoneMat = new THREE.MeshStandardMaterial({ color: 0x64748b, roughness: 0.95 });
        const darkMat = new THREE.MeshStandardMaterial({ color: 0x334155, roughness: 0.95 });

        const blocks = [
            { sz: [0.9, 0.45, 0.8], pos: [0, 0.22, 0], rot: 0.2 },
            { sz: [0.65, 0.38, 0.5], pos: [-0.5, 0.19, 0.3], rot: -0.4 },
            { sz: [0.7, 0.32, 0.6], pos: [0.5, 0.16, -0.2], rot: 0.6 },
            { sz: [0.45, 0.22, 0.45], pos: [0.1, 0.45, 0.1], rot: 0.3 }
        ];

        blocks.forEach((b, idx) => {
            const geo = new THREE.BoxGeometry(...b.sz);
            const mesh = new THREE.Mesh(geo, idx % 2 === 0 ? stoneMat : darkMat);
            mesh.position.set(...b.pos);
            mesh.rotation.y = b.rot;
            rubble.add(mesh);
        });

        this.cityGroup.add(rubble);

        const box = new THREE.Box3();
        box.setFromCenterAndSize(new THREE.Vector3(x, 0.3, z), new THREE.Vector3(1.8, 0.6, 1.6));
        this.staticColliders.push(box);
    }

    buildSafeZone(cx, cz) {
        const safeGroup = new THREE.Group();
        safeGroup.position.set(cx, 0, cz);

        // 1. Ground Safe Assembly Pad
        const padGeo = new THREE.BoxGeometry(20, 0.2, 20);
        const pad = new THREE.Mesh(padGeo, this.materials.safeZonePad);
        pad.position.y = 0.1;
        safeGroup.add(pad);

        // 2. High-Tech Holographic Beacon Beam (Vertical Pillar of Light)
        const beamGeo = new THREE.CylinderGeometry(3.0, 5.0, 80, 24, 1, true);
        const beam = new THREE.Mesh(beamGeo, this.materials.beaconCylinder);
        beam.position.y = 40;
        safeGroup.add(beam);

        // 3. Pulsing Neon Beacon Rings
        const rings = [];
        for (let i = 0; i < 3; i++) {
            const ringGeo = new THREE.TorusGeometry(3.5 + i * 0.8, 0.15, 8, 28);
            ringGeo.rotateX(Math.PI / 2);
            const ring = new THREE.Mesh(ringGeo, this.materials.beaconRing);
            ring.position.y = 2 + i * 5;
            safeGroup.add(ring);
            rings.push(ring);
        }

        // 4. Safe Zone Green Spotlight
        const spot = new THREE.SpotLight(0x22c55e, 3.5, 90, Math.PI / 6, 0.4);
        spot.position.set(0, 45, 0);
        spot.target = pad;
        safeGroup.add(spot);

        this.cityGroup.add(safeGroup);

        this.safeZone = {
            group: safeGroup,
            beam: beam,
            rings: rings,
            center: new THREE.Vector3(cx, 0, cz),
            radius: 9.5
        };

        this.safeZoneBounds = new THREE.Box3();
        this.safeZoneBounds.setFromCenterAndSize(
            new THREE.Vector3(cx, 2, cz),
            new THREE.Vector3(18, 5, 18)
        );
    }

    buildBoundaries() {
        const s = this.citySize * 0.98;
        const thickness = 4.0;
        const h = 14.0;

        // 4 boundary boxes to keep player inside city bounds
        const bounds = [
            { c: [0, h / 2, -s / 2], sz: [s, h, thickness] },
            { c: [0, h / 2, s / 2], sz: [s, h, thickness] },
            { c: [-s / 2, h / 2, 0], sz: [thickness, h, s] },
            { c: [s / 2, h / 2, 0], sz: [thickness, h, s] }
        ];

        bounds.forEach(b => {
            const box = new THREE.Box3();
            box.setFromCenterAndSize(
                new THREE.Vector3(...b.c),
                new THREE.Vector3(...b.sz)
            );
            this.staticColliders.push(box);
        });
    }

    // Animate safe zone beacon pulse
    update(time, delta) {
        if (!this.safeZone) return;

        // Rotate holographic beam
        this.safeZone.beam.rotation.y += delta * 0.6;

        // Pulse beacon rings up and down
        this.safeZone.rings.forEach((ring, idx) => {
            ring.position.y = 2 + ((time * 3 + idx * 3.5) % 18);
            const scale = 1.0 + Math.sin(time * 4 + idx) * 0.15;
            ring.scale.set(scale, scale, scale);
        });

        // Strobe emergency lights on stalled cruisers and ambulances
        if (this.emergencyLights && this.emergencyLights.length > 0) {
            const phase = Math.sin(time * 14.0) > 0;
            this.emergencyLights.forEach(el => {
                if (el.type === 'blue') {
                    el.light.intensity = phase ? 2.0 : 0.08;
                } else if (el.type === 'red') {
                    el.light.intensity = phase ? 0.08 : 2.0;
                } else if (el.type === 'amber') {
                    el.light.intensity = (Math.sin(time * 9.0) > 0.1) ? 1.6 : 0.15;
                }
            });
        }
    }

    // Emergency lights flicker & switch during quake
    setEmergencyMode(active) {
        this.streetLights.forEach(sl => {
            if (active) {
                // Dim or flicker streetlights
                sl.light.intensity = Math.random() > 0.3 ? 0.05 : 0.4;
                sl.bulb.material.color.setHex(0x555555);
            } else {
                sl.light.intensity = 0.9;
                sl.bulb.material.color.setHex(0xfff3b0);
            }
        });
    }

    clear() {
        if (this.cityGroup) {
            this.scene.remove(this.cityGroup);
            this.cityGroup.traverse(obj => {
                if (obj.geometry) obj.geometry.dispose();
            });
        }
        this.cityGroup = new THREE.Group();
        this.scene.add(this.cityGroup);

        this.buildings = [];
        this.staticColliders = [];
        this.hazardPoints = [];
        this.streetLights = [];
        this.emergencyLights = [];
        this.safeZone = null;
        this.safeZoneBounds = null;
    }
}

window.CityBuilder = CityBuilder;
