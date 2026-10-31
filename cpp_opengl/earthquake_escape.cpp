/*
 ============================================================================
  Project Title : Earthquake Escape - 3D Simulation & Emergency Escape Game
  Subject       : Computer Graphics (CG) Mini-Project
  Technology    : C++ with OpenGL & FreeGLUT
 ============================================================================
  Features Implemented:
  1. Real Architectural Buildings with:
     - Multi-floor Window Grids with Frames, Mullions, & Warm/Daylight Lighting
     - Ground Floor Main Entrances with Double Glass Doors, Brass Handles,
       Overhang Marquee Canopy, Porch Lights & Storefront Display Windows
     - Rear Steel Emergency Exit Doors with Glowing Red EXIT Signs
     - Rooftop Parapet Ledges, Stairwell Penthouses, Industrial Water Towers,
       HVAC Air Conditioning Compressors & Communication Antennae with Blinking Beacons
     - Concrete Pedestrian Sidewalks and Street Lampposts
  2. Intensity Selection Start Menu (Magnitude 5.5 to 9.5) with Score Multipliers
  3. Multi-Level Progression (Levels 1 to 5 with distinct themes & layouts)
  4. Real-time Scoring & High Score System
  5. Fully Rendered & Animated 3D Player Avatar (Default 3rd Person View)
  6. Camera Transformation & View Matrix (gluLookAt with Seismic Camera Shake)
  7. 3D Geometric Transformations (Translation, Rotation, Scaling)
  8. Collision Detection with Debris Trapping Protection
 ============================================================================
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#if defined(__has_include)
  #if __has_include(<GL/freeglut.h>)
    #include <GL/freeglut.h>
  #elif __has_include(<GL/glut.h>)
    #include <GL/glut.h>
  #elif __has_include("GL/freeglut.h")
    #include "GL/freeglut.h"
  #elif __has_include("GL/glut.h")
    #include "GL/glut.h"
  #else
    #include <GL/glut.h>
  #endif
#else
  #include <GL/glut.h>
#endif
#endif

// Constants & Screen Dimensions
const float PI = 3.14159265358979323846f;
int windowWidth = 1024;
int windowHeight = 768;

// Game State Enum
enum GameState {
    STATE_INTENSITY_SELECT, // Step 1: Intensity / Difficulty selection
    STATE_LEVEL_SELECT,     // Step 2: Level selection (unlocked one-by-one)
    STATE_EXPLORE,          // Exploring before tremors start
    STATE_WARNING,          // Early tremor warnings
    STATE_EARTHQUAKE,       // Active seismic event & falling debris
    STATE_LEVEL_CLEAR,      // Reached safe zone, mission successful & next level prompt
    STATE_VICTORY,          // All 5 levels completed
    STATE_GAMEOVER          // Time expired
};

// Quake Intensity Presets
enum QuakeIntensityPreset {
    INTENSITY_LOW = 1,
    INTENSITY_MEDIUM = 2,
    INTENSITY_HIGH = 3
};

// Debris struct for falling obstacles
struct Debris {
    float x, y, z;
    float vx, vy, vz;
    float rotX, rotY, rotZ;
    float rotSpeedX, rotSpeedY;
    float size;
    bool landed;
};

// Architectural Building struct
struct Building {
    float x, z;
    float width, depth, height;
    float r, g, b;
    float swayOffset;
    int style;      // 0: Modern Office, 1: Brick Residential, 2: Commercial Tower
    bool hasAntenna;
    int seed;
};

// Street Obstacle / Prop Types
enum PropType {
    PROP_CAR,
    PROP_POLICE_CAR,
    PROP_AMBULANCE,
    PROP_BARRICADE,
    PROP_CONCRETE_BARRIER,
    PROP_TRAFFIC_CONE,
    PROP_HAZARD_DRUM,
    PROP_FALLEN_LAMPPOST,
    PROP_RUBBLE_PILE,
    PROP_TREE,
    PROP_FIRE_HYDRANT,
    PROP_FISSURE
};

// 3D Object placed on streets and evacuation paths
struct StreetProp {
    PropType type;
    float x, y, z;
    float rotY;
    float width, height, depth;
    float r, g, b;
    bool jumpable;
    float vx, vz; // dynamic kicking physics velocity
};

// Interactive Survival Collectibles on Evacuation Path
enum PickupType {
    PICKUP_MEDKIT,     // First Aid: Restores Stamina & +350 pts
    PICKUP_ADRENALINE, // Adrenaline: 1.45x Sprint Speed Boost for 7s
    PICKUP_CHRONO,     // Chrono: Adds +8.0s Evacuation Countdown
    PICKUP_SHIELD      // Seismic Shield: Hazard Debris Immunity for 8.5s
};

struct SurvivalPickup {
    PickupType type;
    float x, y, z;
    float rotY;
    float hoverOffset;
    bool collected;
};

// Global Variables
GameState gameState = STATE_INTENSITY_SELECT;
int selectedIntensity = INTENSITY_MEDIUM; // Default: Medium (Mag 7.0)
int level = 1;
int highestUnlockedLevel = 1;             // Progression: starts at 1, unlocked one-by-one up to 100
const int MAX_LEVELS = 100;
const int LEVELS_PER_PAGE = 20;
const int TOTAL_PAGES = 5;
int levelSelectPage = 0;                  // 0: Lvls 1-20, 1: 21-40, 2: 41-60, 3: 61-80, 4: 81-100

// Scoring System
int currentScore = 0;
int highScore = 12500;
int levelScore = 0;
int debrisDodged = 0;
float scoreMultiplier = 1.5f;

// Timers & Intensity
float timer = 48.0f;
float totalTime = 48.0f;
float gameElapsed = 0.0f;
float quakeIntensity = 1.0f;

// Interactive Mechanics & Player State
float stamina = 100.0f;
float maxStamina = 100.0f;
float speedBoostTimer = 0.0f;
float shieldTimer = 0.0f;
bool isSprinting = false;
std::string hudAlertText = "";
float hudAlertTimer = 0.0f;
int pickupsCollected = 0;

// Player Variables
float playerX = -35.0f;
float playerY = 1.0f;
float playerZ = -35.0f;
float playerYaw = 45.0f; // Degrees
float playerVelY = 0.0f;
bool isGrounded = true;
bool isFirstPerson = false; // Default: Third-person view so avatar is visible!
bool isMoving = false;
float walkAnimTime = 0.0f;

// Key States
bool keys[256] = { false };
bool specialKeys[256] = { false };

// Mouse drag state for looking around
bool isMouseDragging = false;
int lastMouseX = 0;

// Safe Zone Coordinates
float safeZoneX = 35.0f;
float safeZoneZ = 35.0f;
float safeZoneRadius = 6.0f;
float beaconPulse = 1.0f;

// World objects
std::vector<Building> buildings;
std::vector<Debris> debrisList;
std::vector<StreetProp> streetProps;
std::vector<SurvivalPickup> survivalPickups;

// Camera Shake & 6-DOF Camera Trauma
float camShakeX = 0.0f;
float camShakeY = 0.0f;
float camShakeZ = 0.0f;
float camShakePitch = 0.0f;
float camShakeRoll  = 0.0f;
float radarSweepAngle = 0.0f;

// Unity-Style Particle Simulation System
struct VisualParticle {
    float x, y, z;
    float vx, vy, vz;
    float r, g, b, a;
    float size;
    float life;
    float maxLife;
    int type; // 0 = dust puff, 1 = electrical spark, 2 = ambient atmospheric ash/mote, 3 = footstep dust
};
std::vector<VisualParticle> particles;
const size_t MAX_PARTICLES = 160;

// --------------------------------------------------------------------------
// Function Prototypes
// --------------------------------------------------------------------------
void initGame();
void initCity();
void setupLevelLayout();
void populateStreetProps();
void populateSurvivalPickups();
void addProp(PropType type, float x, float z, float rotY, float width, float height, float depth, float r, float g, float b, bool jumpable);
void applyLevelEnvironment();
void startNewGame(int selectedLvl = 1);
void update(int value);
void display();
void reshape(int w, int h);
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void drawCity();
void drawDetailedBuilding(const Building& b);
void drawWindowOnWall(float cx, float cy, float cz, float w, float h, float nx, float nz, int litState);
void drawEntranceDoor(float halfW, float halfD);
void drawEmergencyExitDoor(float halfW, float halfD);
void drawRooftop(const Building& b);
void drawStreetLamppost(float lx, float lz);
void drawSafeZone();
void drawDebris();
void drawPlayerAvatar();
void drawSurvivalPickups();
void draw3DWaypointArrow();
void drawHUD();
void drawIntensityMenu();
void drawLevelSelectMenu();
void drawLevelClearScreen();
void drawVictoryScreen();
void drawGameOverScreen();
void drawCard(float x, float y, float w, float h, float r, float g, float b, float a, bool highlighted);
void renderBitmapString(float x, float y, void *font, const char *string);
void setAlertNotification(const std::string& msg);
bool checkCollision(float newX, float newZ);
const char* getLevelName(int lvl);
const char* getIntensityName(int intens);
const char* getTargetName(int lvl);
const char* getRouteName(int lvl);

// Unity Visual Enhancement Functions
void spawnParticle(float x, float y, float z, float vx, float vy, float vz, float r, float g, float b, float a, float size, float life, int type);
void updateParticles(float dt);
void drawParticles();
void drawContactShadow(float x, float z, float radiusX, float radiusZ, float opacity);
void drawSkyBackdrop();
void drawZebraCrosswalk(float cx, float cz, float width, float length, bool horizontal);
void drawManholeCover(float mx, float mz, float radius);
void draw3DObjectiveMarker();
void drawScreenVignette();

// Prop Renderers
void drawStreetProps();
void drawVehicleProp(const StreetProp& p);
void drawBarricadeProp(const StreetProp& p);
void drawConcreteBarrierProp(const StreetProp& p);
void drawTrafficConeProp(const StreetProp& p);
void drawHazardDrumProp(const StreetProp& p);
void drawFallenLamppostProp(const StreetProp& p);
void drawRubblePileProp(const StreetProp& p);
void drawStreetTreeProp(const StreetProp& p);
void drawFireHydrantProp(const StreetProp& p);
void drawFissureProp(const StreetProp& p);

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// Helper Names (All 100 Urban Districts & Extreme Survival Sectors)
// --------------------------------------------------------------------------
const char* getLevelName(int lvl) {
    if (lvl < 1 || lvl > MAX_LEVELS) return "Urban District";
    static const char* names[100] = {
        // Tier 1: Urban Core Districts (Levels 1-20)
        "Downtown Plaza (Daylight)",
        "Financial District (Sunset)",
        "Industrial Zone (Red Alert)",
        "Metropolis Center (Storm Fog)",
        "Epicenter Ground Zero (Cataclysm)",
        "Neon Transit Hub (Cyber Indigo)",
        "Hillside Overlook (Alpine Mist)",
        "Bio-Tech Zone (Toxic Hazard)",
        "Coastal Sea Wall (Tempest Squall)",
        "The Apex Citadel (Inferno Megacity)",
        "Subway Concourse (Subterranean Dust)",
        "Aerospace Port (Gale Winds)",
        "Volcanic Rift Trench (Fault Rupture)",
        "Orbital Satellite Uplink (Polar Aurora)",
        "The Omega Sanctuary (10.5 Mw Cataclysm)",
        "Volcanic Ridge Caldera (Basalt Ash)",
        "Airport Runway Hub (Turbulent Gale)",
        "Old Port Canals (High Tide Surge)",
        "High-Tech Silicon Core (EMP Flash)",
        "Armageddon Epicenter (Omega Core 11.0 Mw)",

        // Tier 2: Subterranean Megacity & Perimeter (Levels 21-40)
        "Subterranean Catacombs (Dust Fog)",
        "Geothermal Power Vault (Steam)",
        "Central Maglev Terminus (Neon Grid)",
        "Sunken Highway Aqueduct (Rising Flood)",
        "Petrochemical Refinery (Amber Flare)",
        "Coastal Shipping Docks (Tempest Tide)",
        "Deep Mountain Radar (Alpine Fog)",
        "Cryo-Storage Facility (Sub-Zero Frost)",
        "Crystal Geode Cavern (Phosphor Blue)",
        "Blast Furnace Foundry (Molten Glow)",
        "Elevated Monorail Trestle (High Winds)",
        "Bio-Sphere Habitat Dome (Toxic Green)",
        "Heavy Cargo Crane Yard (Steel Echo)",
        "Granite Quarry Canyon (Falling Boulders)",
        "Desalination Aqueduct (Sea Spray)",
        "Metro Express Conduit (Dark Spark)",
        "Seismic Monitoring Base (Red Alert)",
        "Nuclear Storage Cavern (Radiation Shield)",
        "Sunken Plaza Ruins (Ash Drift)",
        "Underground Bastion Core (Deep Tremors)",

        // Tier 3: Tectonic & Industrial Wastelands (Levels 41-60)
        "Tectonic Fault Breach (Fault Line)",
        "Acid Sump Reservoir (Chemical Mist)",
        "Ashfall Gorge Pass (Obsidian Ash)",
        "Reactor Debris Field (Gamma Flare)",
        "Ordnance Depot Silo (Blast Hazard)",
        "High-Pressure Gas Run (Vapor Jet)",
        "Sulfur Caldera Flats (Yellow Smoke)",
        "Molten Slag Canal (Crimson Surge)",
        "Crushed Railway Depot (Twisted Rail)",
        "Centennial Megacity Ruins (11.2 Mw)",
        "Storm Surge Floodwall (Sea Storm)",
        "Hydrothermal Geyser Plain (Boiling Mist)",
        "Freight Rail Terminal (Rubble Storm)",
        "High-Voltage Grid Spire (Electric Arc)",
        "Obsidian Canyon Chasm (Crust Split)",
        "Blastwave Epicenter (Zero Point)",
        "Scrap Mountain Ridge (Metal Avalanche)",
        "Hazard Isolation Zone (Bio Shield)",
        "Dark Obsidian Bastion (Fault Line)",
        "Titan Caldera Trench (11.5 Mw Quake)",

        // Tier 4: Extreme Mountain & Oceanic Outposts (Levels 61-80)
        "Permafrost Alpine Ridge (Gale Blizzard)",
        "Glacier Crevasse Chasm (Ice Spires)",
        "Tidal Wave Overlook (Ocean Tempest)",
        "Stormy Archipelago Bridge (High Tide)",
        "Offshore Platform Rig (Sea Spray)",
        "Tempest Lighthouse Spire (Beacon Flash)",
        "Skyway Viaduct Overpass (Gale Winds)",
        "Cloudbreak Mountain Pass (Alpine Mist)",
        "Frostbite Fault Line (Frozen Fracture)",
        "Hydrothermal Abyssal Basin (Boiling Steam)",
        "Deep Oceanic Causeway (Sub-Sea Surge)",
        "High Tramway Terminal (Cable Tension)",
        "Iron Ore Chasm Run (Iron Dust)",
        "Wind Turbine Plateau (Vortex Winds)",
        "Frostfire Mountain Peak (Dual Elements)",
        "Thunderhead Ridge Bastion (Lightning Fog)",
        "Snow Avalanche Gully (Powder Rush)",
        "Tsunami Sea Barrier (Crash Surge)",
        "Zenith Overlook Bastion (High Altitude)",
        "Apex Spire Sanctum (11.8 Mw Super-Quake)",

        // Tier 5: Cataclysm Core & The Final Frontier (Levels 81-100)
        "Plasma Reactor Core (Energy Flare)",
        "Quantum Research Sector (Zero Flux)",
        "Deep Crust Trench (Mantle Rupture)",
        "Tectonic Chasm Void (Earth Tear)",
        "Antimatter Relay Station (Pulse Flash)",
        "Orbital Defense Silo (Laser Sweep)",
        "Megastructure Pylon (Steel Spires)",
        "Hyper-Resonance Trench (Vibration Wave)",
        "Titan Seismic Abyss (Deep Drop)",
        "Eclipse Skyway Corridor (Dark Horizon)",
        "Magma Chamber Nexus (Liquid Fire)",
        "Zero-Gravity Anchor (Grav-Lift)",
        "Rift Horizon Causeway (Event Boundary)",
        "Cataclysm Matrix Core (Omega Shock)",
        "Singularity Sub-Crust (Gravity Bend)",
        "Super-Tectonic Trench (World Split)",
        "Celestial Relay Spire (Sky Beam)",
        "Omega Bastion Sanctum (Final Defense)",
        "Armageddon Edge (12.0 Mw Continental Quake)",
        "THE GRAND SURVIVAL ZENITH (Ultimate 100 Mw)"
    };
    return names[lvl - 1];
}

const char* getTargetName(int lvl) {
    if (lvl < 1 || lvl > MAX_LEVELS) return "Emergency Safe Zone";
    static const char* targets[100] = {
        // Tier 1 (1-20)
        "South Park Refuge",
        "East Harbor Dock",
        "North Expressway Overpass",
        "West Coastal Stadium Bunker",
        "Central Underground Vault",
        "Northeast Helipad Evacuation Deck",
        "Southwest Mountain Tunnel Shelter",
        "Northwest Decontamination Dome",
        "Southeast Lighthouse Citadel",
        "Apex High-Altitude Bastion",
        "West Metro Ventilation Shaft",
        "East Terminal Cargo Hangar",
        "South Geothermal Shield Vault",
        "North Uplink Transmission Dome",
        "Grand Omega Sanctuary Core",
        "Mountain Geothermal Station",
        "Aircraft Hangar Blast Shelter",
        "Maritime Fort Outpost",
        "Faraday Electromagnetic Shield",
        "Titan Deep Core Sanctuary",

        // Tier 2 (21-40)
        "Catacomb Emergency Airlock",
        "Geothermal Coolant Vault",
        "Maglev Evacuation Shuttle",
        "Drainage Pump Station Bunker",
        "Refinery Fire Suppression Vault",
        "Maritime Coastguard Vessel",
        "Alpine Weather Radar Station",
        "Sub-Zero Cryo-Shield Chamber",
        "Geode Crystal Sanctuary",
        "Blast-Resistant Iron Vault",
        "Trestle Emergency Service Pad",
        "Bio-Dome Environmental Pod",
        "Heavy Gantry Crane Bastion",
        "Quarry Excavator Shelter",
        "Coastal Aqueduct Pump Gate",
        "Metro Traction Power Vault",
        "Seismic Early Warning Bunker",
        "Lead-Lined Radioactive Vault",
        "Sunken Plaza Evacuation Pod",
        "Under-City Fortress Core",

        // Tier 3 (41-60)
        "Tectonic Fault Pressure Valve",
        "Neutralization Containment Dome",
        "Ashfall Gorge Overhang",
        "Gamma Radiation Blast Shield",
        "Subterranean Missile Silo Cap",
        "Emergency Gas Shutoff Vault",
        "Sulfur Flats Extraction Pod",
        "Coolant Overflow Bastion",
        "Locomotive Roundhouse Shelter",
        "Centennial Megacity Vault",
        "Storm Surge Barrier Control",
        "Geyser Field Pressure Outpost",
        "Cargo Dispatch Center Bunker",
        "Lightning Arrester Terminal",
        "Chasm Bridge Anchor Station",
        "Blastwave Hardened Bunker",
        "Ore Crusher Steel Shelter",
        "Hazmat Quarantine Citadel",
        "Obsidian Ridge Lookout Post",
        "Titan Caldera Deep Vault",

        // Tier 4 (61-80)
        "Permafrost Station Alpha",
        "Glacier Tunnel Ice Shelter",
        "Tidal Wave Early Warning Spire",
        "Archipelago Causeway Tower",
        "Offshore Helipad Evacuation Hub",
        "Lighthouse Foghorn Sanctuary",
        "Skyway Suspension Pillar Base",
        "Cloudbreak Mountain Lodge",
        "Permafrost Research Shelter",
        "Abyssal Geothermal Core",
        "Sub-Sea Tube Anchor Gate",
        "Tramway Cable Anchor Bastion",
        "Iron Mine Excavation Shelter",
        "Turbine Substation Safe Room",
        "Frostfire Peak Summit Bastion",
        "Thunderhead Radar Sanctuary",
        "Avalanche Deflection Wall Bunker",
        "Tsunami Wall Command Outpost",
        "Zenith Observation Deck",
        "Apex Sanctum Grand Core",

        // Tier 5 (81-100)
        "Plasma Magnetic Containment Vault",
        "Quantum Stabilization Capsule",
        "Crustal Plate Anchor Shelter",
        "Void Edge Seismic Platform",
        "Antimatter Pulse Safe Core",
        "Orbital Silo Command Bunker",
        "Megastructure Foundation Bastion",
        "Hyper-Resonance Dampener Room",
        "Abyss Bridge Structural Pod",
        "Eclipse Solar Array Vault",
        "Magma Deflection Shield Dome",
        "Zero-G Inertia Chamber",
        "Event Horizon Safety Airpod",
        "Matrix Omega Core Sanctuary",
        "Gravitational Singularity Chamber",
        "World-Split Fault Bastion",
        "Celestial Beam Array Shelter",
        "Omega Bastion Prime Vault",
        "Continental Armageddon Outpost",
        "THE GRAND ZENITH SANCTUARY CORE"
    };
    return targets[lvl - 1];
}

// 100 Unique Level Layout Targets & Spawns (Collision-free, 100% Distinct Target Locations)
struct LevelTargetCoord {
    float safeX, safeZ;
    float spawnX, spawnZ;
    float spawnYaw;
};

static const LevelTargetCoord g_levelTargets[100] = {
    {    0.0f,   38.0f,    0.0f,  -38.0f,    0.0f }, // Level 1
    {   38.0f,    0.0f,  -38.0f,    0.0f,   90.0f }, // Level 2
    {    0.0f,  -40.0f,    0.0f,   38.0f,  180.0f }, // Level 3
    {  -40.0f,    0.0f,   38.0f,    0.0f,  270.0f }, // Level 4
    {    0.0f,    0.0f,  -35.0f,  -44.0f,   38.5f }, // Level 5
    {   44.0f,  -44.0f,  -44.0f,   44.0f,  135.0f }, // Level 6
    {  -44.0f,   44.0f,   44.0f,  -44.0f,  315.0f }, // Level 7
    {  -44.0f,  -44.0f,   44.0f,   44.0f,  225.0f }, // Level 8
    {   44.0f,   44.0f,  -44.0f,  -44.0f,   45.0f }, // Level 9
    {    6.0f,  -44.0f,   -6.0f,   44.0f,  172.2f }, // Level 10
    {  -44.0f,    8.0f,   44.0f,   -8.0f,  280.3f }, // Level 11
    {   44.0f,   -8.0f,  -44.0f,    8.0f,  100.3f }, // Level 12
    {   -8.0f,   44.0f,    8.0f,  -44.0f,  349.7f }, // Level 13
    {   26.5f,  -44.0f,  -26.5f,   44.0f,  148.9f }, // Level 14
    {  -26.5f,   26.5f,   26.5f,  -26.5f,  315.0f }, // Level 15
    {  -26.5f,  -44.0f,   26.5f,   44.0f,  211.1f }, // Level 16
    {   44.0f,   26.5f,  -44.0f,  -26.5f,   58.9f }, // Level 17
    {   26.5f,   44.0f,  -26.5f,  -44.0f,   31.1f }, // Level 18
    {  -44.0f,  -26.5f,   44.0f,   26.5f,  238.9f }, // Level 19
    {   -6.0f,   -6.0f,   44.0f,   44.0f,  225.0f }, // Level 20
    {  -35.0f,  -44.0f,   35.0f,   44.0f,  218.5f }, // Level 21
    {    0.0f,   44.0f,   -0.0f,  -44.0f,    0.0f }, // Level 22
    {   44.0f,  -35.0f,  -44.0f,   35.0f,  128.5f }, // Level 23
    {  -44.0f,    9.0f,   44.0f,   -9.0f,  281.6f }, // Level 24
    {    0.0f,  -42.0f,   -0.0f,   42.0f,  180.0f }, // Level 25
    {   -5.0f,  -26.5f,    5.0f,   26.5f,  190.7f }, // Level 26
    {    5.0f,  -15.0f,   -5.0f,   15.0f,  161.6f }, // Level 27
    {    0.0f,   26.5f,   -0.0f,  -26.5f,    0.0f }, // Level 28
    {    0.0f,   42.0f,   -0.0f,  -42.0f,    0.0f }, // Level 29
    {  -32.0f,   -5.0f,   32.0f,    5.0f,  261.1f }, // Level 30
    {  -20.0f,    5.0f,   20.0f,   -5.0f,  284.0f }, // Level 31
    {   20.0f,    0.0f,  -20.0f,   -0.0f,   90.0f }, // Level 32
    {   38.0f,   -5.0f,  -38.0f,    5.0f,   97.5f }, // Level 33
    {  -26.5f,  -20.0f,   26.5f,   20.0f,  233.0f }, // Level 34
    {   26.5f,  -26.5f,  -26.5f,   26.5f,  135.0f }, // Level 35
    {  -40.0f,  -26.5f,   40.0f,   26.5f,  236.5f }, // Level 36
    {  -40.0f,   26.5f,   40.0f,  -26.5f,  303.5f }, // Level 37
    {   -6.0f,   -3.0f,    6.0f,    3.0f,  243.4f }, // Level 38
    {   -3.0f,    6.0f,    3.0f,   -6.0f,  333.4f }, // Level 39
    {    3.0f,    3.0f,   -3.0f,   -3.0f,   45.0f }, // Level 40
    {  -26.5f,   44.0f,   26.5f,  -44.0f,  328.9f }, // Level 41
    {   18.0f,   44.0f,  -18.0f,  -44.0f,   22.2f }, // Level 42
    {  -44.0f,    0.0f,   44.0f,   -0.0f,  270.0f }, // Level 43
    {    5.0f,  -42.0f,   -5.0f,   42.0f,  173.2f }, // Level 44
    {   -5.0f,  -20.0f,    5.0f,   20.0f,  194.0f }, // Level 45
    {    0.0f,   20.0f,   -0.0f,  -20.0f,    0.0f }, // Level 46
    {   -5.0f,   42.0f,    5.0f,  -42.0f,  353.2f }, // Level 47
    {  -32.0f,    5.0f,   32.0f,   -5.0f,  278.9f }, // Level 48
    {   15.0f,   -5.0f,  -15.0f,    5.0f,  108.4f }, // Level 49
    {   32.0f,    0.0f,  -32.0f,   -0.0f,   90.0f }, // Level 50
    {  -26.5f,  -10.0f,   26.5f,   10.0f,  249.3f }, // Level 51
    {   26.5f,   10.0f,  -26.5f,  -10.0f,   69.3f }, // Level 52
    {   20.0f,  -26.5f,  -20.0f,   26.5f,  143.0f }, // Level 53
    {   40.0f,   26.5f,  -40.0f,  -26.5f,   56.5f }, // Level 54
    {    0.0f,   -3.0f,   -0.0f,    3.0f,  180.0f }, // Level 55
    {    6.0f,    0.0f,   -6.0f,   -0.0f,   90.0f }, // Level 56
    {    9.0f,  -44.0f,   -9.0f,   44.0f,  168.4f }, // Level 57
    {   44.0f,   -9.0f,  -44.0f,    9.0f,  101.6f }, // Level 58
    {    0.0f,  -38.0f,   -0.0f,   38.0f,  180.0f }, // Level 59
    {    0.0f,  -15.0f,   -0.0f,   15.0f,  180.0f }, // Level 60
    {    5.0f,   32.0f,   -5.0f,  -32.0f,    8.9f }, // Level 61
    {  -32.0f,    0.0f,   32.0f,   -0.0f,  270.0f }, // Level 62
    {   15.0f,    5.0f,  -15.0f,   -5.0f,   71.6f }, // Level 63
    {   42.0f,    0.0f,  -42.0f,   -0.0f,   90.0f }, // Level 64
    {   26.5f,  -32.0f,  -26.5f,   32.0f,  140.4f }, // Level 65
    {   10.0f,  -26.5f,  -10.0f,   26.5f,  159.3f }, // Level 66
    {   -6.0f,    3.0f,    6.0f,   -3.0f,  296.6f }, // Level 67
    {    3.0f,   -3.0f,   -3.0f,    3.0f,  135.0f }, // Level 68
    {   -9.0f,  -44.0f,    9.0f,   44.0f,  191.6f }, // Level 69
    {  -44.0f,   -9.0f,   44.0f,    9.0f,  258.4f }, // Level 70
    {   -5.0f,  -32.0f,    5.0f,   32.0f,  188.9f }, // Level 71
    {   -5.0f,   20.0f,    5.0f,  -20.0f,  346.0f }, // Level 72
    {  -42.0f,    5.0f,   42.0f,   -5.0f,  276.8f }, // Level 73
    {  -15.0f,    5.0f,   15.0f,   -5.0f,  288.4f }, // Level 74
    {   42.0f,    5.0f,  -42.0f,   -5.0f,   83.2f }, // Level 75
    {   26.5f,   20.0f,  -26.5f,  -20.0f,   53.0f }, // Level 76
    {  -10.0f,   26.5f,   10.0f,  -26.5f,  339.3f }, // Level 77
    {    0.0f,    3.0f,   -0.0f,   -3.0f,    0.0f }, // Level 78
    {  -18.0f,   44.0f,   18.0f,  -44.0f,  337.8f }, // Level 79
    {   44.0f,    9.0f,  -44.0f,   -9.0f,   78.4f }, // Level 80
    {    5.0f,  -26.5f,   -5.0f,   26.5f,  169.3f }, // Level 81
    {   -5.0f,   38.0f,    5.0f,  -38.0f,  352.5f }, // Level 82
    {  -20.0f,    0.0f,   20.0f,   -0.0f,  270.0f }, // Level 83
    {   42.0f,   -5.0f,  -42.0f,    5.0f,   96.8f }, // Level 84
    {   26.5f,   32.0f,  -26.5f,  -32.0f,   39.6f }, // Level 85
    {   -6.0f,    0.0f,    6.0f,   -0.0f,  270.0f }, // Level 86
    {    6.0f,   -3.0f,   -6.0f,    3.0f,  116.6f }, // Level 87
    {   44.0f,  -26.5f,  -44.0f,   26.5f,  121.1f }, // Level 88
    {    5.0f,  -32.0f,   -5.0f,   32.0f,  171.1f }, // Level 89
    {    5.0f,   38.0f,   -5.0f,  -38.0f,    7.5f }, // Level 90
    {   15.0f,    0.0f,  -15.0f,   -0.0f,   90.0f }, // Level 91
    {  -26.5f,   20.0f,   26.5f,  -20.0f,  307.0f }, // Level 92
    {  -32.0f,   26.5f,   32.0f,  -26.5f,  309.6f }, // Level 93
    {    3.0f,    0.0f,   -3.0f,   -0.0f,   90.0f }, // Level 94
    {  -44.0f,  -35.0f,   44.0f,   35.0f,  231.5f }, // Level 95
    {    0.0f,  -20.0f,   -0.0f,   20.0f,  180.0f }, // Level 96
    {  -38.0f,   -5.0f,   38.0f,    5.0f,  262.5f }, // Level 97
    {   32.0f,   -5.0f,  -32.0f,    5.0f,   98.9f }, // Level 98
    {  -32.0f,  -26.5f,   32.0f,   26.5f,  230.4f }, // Level 99
    {    0.0f,   -6.0f,   -0.0f,    6.0f,  180.0f }, // Level 100
};

const char* getRouteName(int lvl) {
    if (lvl < 1 || lvl > MAX_LEVELS) return "Evacuation Route";
    static char routeBuf[128];
    int idx = lvl - 1;
    float dx = g_levelTargets[idx].safeX - g_levelTargets[idx].spawnX;
    float dz = g_levelTargets[idx].safeZ - g_levelTargets[idx].spawnZ;
    float dist = sqrt(dx * dx + dz * dz);
    float angle = atan2(dx, -dz) * 180.0f / PI; // 0 = North, 90 = East, 180 = South, 270 = West
    if (angle < 0.0f) angle += 360.0f;

    const char* dirStr = "Northbound";
    if (angle >= 22.5f && angle < 67.5f) dirStr = "Northeast";
    else if (angle >= 67.5f && angle < 112.5f) dirStr = "Eastbound";
    else if (angle >= 112.5f && angle < 157.5f) dirStr = "Southeast";
    else if (angle >= 157.5f && angle < 202.5f) dirStr = "Southbound";
    else if (angle >= 202.5f && angle < 247.5f) dirStr = "Southwest";
    else if (angle >= 247.5f && angle < 292.5f) dirStr = "Westbound";
    else if (angle >= 292.5f && angle < 337.5f) dirStr = "Northwest";

    snprintf(routeBuf, sizeof(routeBuf), "%s -> %s (%d deg, %dm)", dirStr, getTargetName(lvl), (int)angle, (int)dist);
    return routeBuf;
}

const char* getIntensityName(int intens) {
    switch (intens) {
        case INTENSITY_LOW:     return "Low (Mag 5.5)";
        case INTENSITY_MEDIUM:  return "Medium (Mag 7.0)";
        case INTENSITY_HIGH:    return "High (Mag 8.2)";
        default: return "Medium (Mag 7.0)";
    }
}

// --------------------------------------------------------------------------
// Unity Visual Enhancement Systems
// --------------------------------------------------------------------------
void spawnParticle(float x, float y, float z, float vx, float vy, float vz, float r, float g, float b, float a, float size, float life, int type) {
    if (particles.size() >= MAX_PARTICLES) {
        particles.erase(particles.begin());
    }
    VisualParticle p;
    p.x = x; p.y = y; p.z = z;
    p.vx = vx; p.vy = vy; p.vz = vz;
    p.r = r; p.g = g; p.b = b; p.a = a;
    p.size = size;
    p.life = life;
    p.maxLife = life;
    p.type = type;
    particles.push_back(p);
}

void updateParticles(float dt) {
    for (size_t i = 0; i < particles.size(); ) {
        VisualParticle& p = particles[i];
        p.life -= dt;
        if (p.life <= 0.0f) {
            particles.erase(particles.begin() + i);
            continue;
        }

        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;

        if (p.type == 0) {
            // Ground dust billows: expands, slows down, rises gently
            p.vx *= 0.94f;
            p.vz *= 0.94f;
            p.vy += 0.45f * dt;
            p.size += 0.85f * dt;
            p.a = (p.life / p.maxLife) * 0.38f;
        } else if (p.type == 1) {
            // Electrical sparks: gravity, rapid bounce
            p.vy -= 12.0f * dt;
            if (p.y <= 0.05f) {
                p.y = 0.05f;
                p.vy = -p.vy * 0.45f;
                p.vx *= 0.7f;
                p.vz *= 0.7f;
            }
            p.a = (p.life / p.maxLife) * 0.95f;
        } else if (p.type == 2) {
            // Ambient atmospheric motes / ash: gentle floating drift
            p.vx += sin(gameElapsed * 2.0f + p.y) * 0.2f * dt;
            p.vz += cos(gameElapsed * 1.5f + p.x) * 0.2f * dt;
            p.a = (p.life / p.maxLife) * 0.45f;
        } else if (p.type == 3) {
            // Footstep dust: rapid fade, slight rise
            p.vy += 0.5f * dt;
            p.size += 0.4f * dt;
            p.a = (p.life / p.maxLife) * 0.32f;
        }

        ++i;
    }
}

void drawParticles() {
    if (particles.empty()) return;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Luminous additive blending

    for (const auto& p : particles) {
        glColor4f(p.r, p.g, p.b, p.a);
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        if (p.type == 1) {
            glScalef(p.size * 0.5f, p.size * 1.8f, p.size * 0.5f);
            glutSolidCube(1.0);
        } else {
            glutSolidSphere(p.size * 0.5f, 6, 6);
        }
        glPopMatrix();
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawContactShadow(float x, float z, float radiusX, float radiusZ, float opacity) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.02f, 0.03f, 0.05f, opacity);
    glVertex3f(x, 0.015f, z);
    glColor4f(0.02f, 0.03f, 0.05f, 0.0f);
    for (int i = 0; i <= 24; ++i) {
        float ang = i * 2.0f * PI / 24.0f;
        glVertex3f(x + cos(ang) * radiusX, 0.015f, z + sin(ang) * radiusZ);
    }
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawSkyBackdrop() {
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDepthMask(GL_FALSE);

    glPushMatrix();
    glTranslatef(playerX, 0.0f, playerZ);

    // 1. Celestial Atmospheric Sky Dome (Space navy to horizon gradient)
    float domeRadius = 240.0f;
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= 36; ++i) {
        float ang = i * 2.0f * PI / 36.0f;
        float nx = cos(ang);
        float nz = sin(ang);

        // Zenith
        glColor3f(0.03f, 0.06f, 0.14f);
        glVertex3f(nx * domeRadius * 0.4f, 130.0f, nz * domeRadius * 0.4f);

        // Horizon
        glColor3f(0.14f, 0.22f, 0.35f);
        glVertex3f(nx * domeRadius, 0.0f, nz * domeRadius);
    }
    glEnd();

    // 2. Distant Metropolitan Skyline Silhouettes
    for (int i = 0; i < 28; ++i) {
        float ang = i * 2.0f * PI / 28.0f;
        float dist = 200.0f;
        float bx = cos(ang) * dist;
        float bz = sin(ang) * dist;
        float bw = 18.0f + ((i * 13) % 16);
        float bh = 48.0f + ((i * 31) % 60);

        glColor3f(0.05f, 0.08f, 0.15f);
        glPushMatrix();
        glTranslatef(bx, bh * 0.5f, bz);
        glScalef(bw, bh, bw);
        glutSolidCube(1.0);
        glPopMatrix();

        // Red/white aviation obstruction hazard beacons
        if (i % 2 == 0) {
            bool beaconOn = (fmod(gameElapsed + i * 0.35f, 1.0f) < 0.5f);
            if (beaconOn) {
                glColor3f(1.0f, 0.15f, 0.15f);
                glPushMatrix();
                glTranslatef(bx, bh + 3.0f, bz);
                glutSolidSphere(1.4, 6, 6);
                glPopMatrix();
            }
        }
    }

    // 3. Radiant Atmospheric Celestial Sun / Moon Disk
    glPushMatrix();
    glTranslatef(120.0f, 85.0f, -140.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 0.90f, 0.65f, 0.22f);
    glutSolidSphere(18.0, 16, 16);
    glColor4f(1.0f, 0.98f, 0.92f, 0.88f);
    glutSolidSphere(8.0, 16, 16);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glPopMatrix();

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_FOG);
    glEnable(GL_LIGHTING);
}

void drawZebraCrosswalk(float cx, float cz, float width, float length, bool horizontal) {
    glDisable(GL_LIGHTING);
    glColor3f(0.92f, 0.93f, 0.96f);
    int stripes = 7;
    float stripeW = 0.65f;
    float gap = 0.55f;

    for (int s = 0; s < stripes; ++s) {
        float offset = -((stripes - 1) * (stripeW + gap)) * 0.5f + s * (stripeW + gap);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        if (horizontal) {
            glVertex3f(cx - width * 0.5f, 0.025f, cz + offset - stripeW * 0.5f);
            glVertex3f(cx + width * 0.5f, 0.025f, cz + offset - stripeW * 0.5f);
            glVertex3f(cx + width * 0.5f, 0.025f, cz + offset + stripeW * 0.5f);
            glVertex3f(cx - width * 0.5f, 0.025f, cz + offset + stripeW * 0.5f);
        } else {
            glVertex3f(cx + offset - stripeW * 0.5f, 0.025f, cz - length * 0.5f);
            glVertex3f(cx + offset + stripeW * 0.5f, 0.025f, cz - length * 0.5f);
            glVertex3f(cx + offset + stripeW * 0.5f, 0.025f, cz + length * 0.5f);
            glVertex3f(cx + offset - stripeW * 0.5f, 0.025f, cz + length * 0.5f);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
}

void drawManholeCover(float mx, float mz, float radius) {
    glPushMatrix();
    glTranslatef(mx, 0.025f, mz);

    glColor3f(0.18f, 0.20f, 0.23f);
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < 20; ++i) {
        float a = i * 2.0f * PI / 20.0f;
        glVertex3f(cos(a) * radius, 0.0f, sin(a) * radius);
    }
    glEnd();

    glDisable(GL_LIGHTING);
    glColor3f(0.28f, 0.32f, 0.36f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 20; ++i) {
        float a = i * 2.0f * PI / 20.0f;
        glVertex3f(cos(a) * radius * 0.65f, 0.005f, sin(a) * radius * 0.65f);
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void draw3DObjectiveMarker() {
    float dist = sqrt((playerX - safeZoneX) * (playerX - safeZoneX) + (playerZ - safeZoneZ) * (playerZ - safeZoneZ));
    float markerY = 10.2f + sin(gameElapsed * 3.5f) * 0.8f;

    glPushMatrix();
    glTranslatef(safeZoneX, markerY, safeZoneZ);

    float camDirX = playerX - safeZoneX;
    float camDirZ = playerZ - safeZoneZ;
    float billAngle = atan2(camDirX, camDirZ) * 180.0f / PI;
    glRotatef(billAngle, 0.0f, 1.0f, 0.0f);

    glDisable(GL_LIGHTING);

    // Floating rotating 3D emerald diamond
    glPushMatrix();
    glRotatef(gameElapsed * 80.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(45.0f, 1.0f, 0.0f, 1.0f);
    glColor3f(0.20f, 1.0f, 0.50f);
    glutSolidCube(1.5);
    glColor3f(0.95f, 1.0f, 0.95f);
    glutWireCube(1.52);
    glPopMatrix();

    // Downward guidance arrow
    glColor3f(0.25f, 0.95f, 0.45f);
    glBegin(GL_TRIANGLES);
    glVertex3f( 0.0f, -0.7f, 0.0f);
    glVertex3f(-0.75f, 0.4f, 0.0f);
    glVertex3f( 0.75f, 0.4f, 0.0f);
    glEnd();

    // Floating text label
    std::stringstream labelSS;
    labelSS << "▼ EVACUATION POINT (" << static_cast<int>(dist) << "m) ▼";
    glColor3f(0.95f, 1.0f, 0.95f);
    glRasterPos3f(-3.2f, 1.5f, 0.0f);
    for (char c : labelSS.str()) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawScreenVignette() {
    glBegin(GL_QUADS);
    // Top strip
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glVertex2f(0.0f, windowHeight);
    glVertex2f(windowWidth, windowHeight);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(windowWidth - 95.0f, windowHeight - 95.0f);
    glVertex2f(95.0f, windowHeight - 95.0f);

    // Bottom strip
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(windowWidth, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(windowWidth - 95.0f, 95.0f);
    glVertex2f(95.0f, 95.0f);

    // Left strip
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, windowHeight);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(95.0f, windowHeight - 95.0f);
    glVertex2f(95.0f, 95.0f);

    // Right strip
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glVertex2f(windowWidth, 0.0f);
    glVertex2f(windowWidth, windowHeight);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(windowWidth - 95.0f, windowHeight - 95.0f);
    glVertex2f(windowWidth - 95.0f, 95.0f);
    glEnd();

    // Critical low-time red danger pulse
    if (timer <= 10.0f && (gameState == STATE_EXPLORE || gameState == STATE_WARNING || gameState == STATE_EARTHQUAKE)) {
        float pulse = (sin(gameElapsed * 8.0f) * 0.5f + 0.5f) * 0.35f;
        glColor4f(0.85f, 0.05f, 0.05f, pulse);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(windowWidth, 0.0f);
        glVertex2f(windowWidth, windowHeight);
        glVertex2f(0.0f, windowHeight);
        glEnd();
    }
}

// --------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------
void initGame() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); // Dual-Light Rig (Key Sun + Sky Fill)
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glShadeModel(GL_SMOOTH); // Gouraud vertex smooth shading
    glEnable(GL_NORMALIZE);   // Preserve unit normals after scaling

    // Unity-style PBR Specular Material Highlights
    GLfloat matSpecular[] = { 0.45f, 0.45f, 0.45f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 48.0f);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Setup Directional Key Light (Sun)
    GLfloat lightPos0[] = { 45.0f, 90.0f, 35.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    // Setup Secondary Sky Fill Light (Hemisphere Bounce)
    GLfloat lightPos1[] = { -45.0f, 40.0f, -35.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

    // Atmospheric Fog
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.010f);

    applyLevelEnvironment();
    setupLevelLayout();
}

void applyLevelEnvironment() {
    GLfloat fogColor[4];
    GLfloat lightDiffuse[4];
    GLfloat lightAmbient[4] = { 0.24f, 0.24f, 0.30f, 1.0f };

    int envPreset = ((level - 1) % 20) + 1;
    switch (envPreset) {
        case 1: // Clear Daylight / Afternoon
            fogColor[0] = 0.08f; fogColor[1] = 0.12f; fogColor[2] = 0.20f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.95f; lightDiffuse[1] = 0.90f; lightDiffuse[2] = 0.85f; lightDiffuse[3] = 1.0f;
            break;
        case 2: // Sunset Golden Amber
            fogColor[0] = 0.22f; fogColor[1] = 0.12f; fogColor[2] = 0.07f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 1.00f; lightDiffuse[1] = 0.70f; lightDiffuse[2] = 0.42f; lightDiffuse[3] = 1.0f;
            break;
        case 3: // Midnight Emergency Sirens (Red Alert)
            fogColor[0] = 0.18f; fogColor[1] = 0.05f; fogColor[2] = 0.06f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.95f; lightDiffuse[1] = 0.38f; lightDiffuse[2] = 0.35f; lightDiffuse[3] = 1.0f;
            break;
        case 4: // Stormy Dense Mist / Deep Cyan
            fogColor[0] = 0.06f; fogColor[1] = 0.12f; fogColor[2] = 0.18f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.65f; lightDiffuse[1] = 0.80f; lightDiffuse[2] = 0.95f; lightDiffuse[3] = 1.0f;
            break;
        case 5: // Epicenter Ground Zero (Apocalyptic Violet)
            fogColor[0] = 0.16f; fogColor[1] = 0.05f; fogColor[2] = 0.12f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 1.00f; lightDiffuse[1] = 0.50f; lightDiffuse[2] = 0.65f; lightDiffuse[3] = 1.0f;
            break;
        case 6: // Neon Transit Hub (Cyber Indigo / Electric Violet)
            fogColor[0] = 0.10f; fogColor[1] = 0.07f; fogColor[2] = 0.20f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.75f; lightDiffuse[1] = 0.55f; lightDiffuse[2] = 1.00f; lightDiffuse[3] = 1.0f;
            break;
        case 7: // Hillside Overlook (Alpine Emerald Mist)
            fogColor[0] = 0.05f; fogColor[1] = 0.18f; fogColor[2] = 0.14f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.70f; lightDiffuse[1] = 0.95f; lightDiffuse[2] = 0.85f; lightDiffuse[3] = 1.0f;
            break;
        case 8: // Bio-Tech Zone (Toxic Olive & Amber Haze)
            fogColor[0] = 0.12f; fogColor[1] = 0.16f; fogColor[2] = 0.05f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.85f; lightDiffuse[1] = 0.95f; lightDiffuse[2] = 0.35f; lightDiffuse[3] = 1.0f;
            break;
        case 9: // Coastal Sea Wall (Tempest Slate & Ocean Navy)
            fogColor[0] = 0.05f; fogColor[1] = 0.10f; fogColor[2] = 0.18f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.45f; lightDiffuse[1] = 0.75f; lightDiffuse[2] = 0.98f; lightDiffuse[3] = 1.0f;
            break;
        case 10: // The Apex Citadel (Molten Crimson & Sulfur Orange)
            fogColor[0] = 0.22f; fogColor[1] = 0.06f; fogColor[2] = 0.04f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 1.00f; lightDiffuse[1] = 0.45f; lightDiffuse[2] = 0.20f; lightDiffuse[3] = 1.0f;
            break;
        case 11: // Subway Concourse (Subterranean Phosphor Glow & Dust)
            fogColor[0] = 0.06f; fogColor[1] = 0.14f; fogColor[2] = 0.10f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.55f; lightDiffuse[1] = 0.90f; lightDiffuse[2] = 0.65f; lightDiffuse[3] = 1.0f;
            break;
        case 12: // Aerospace Port (Aviation Amber Fog & Gale Winds)
            fogColor[0] = 0.20f; fogColor[1] = 0.14f; fogColor[2] = 0.06f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.98f; lightDiffuse[1] = 0.82f; lightDiffuse[2] = 0.45f; lightDiffuse[3] = 1.0f;
            break;
        case 13: // Volcanic Rift Trench (Magma Blaze & Thick Ash)
            fogColor[0] = 0.25f; fogColor[1] = 0.08f; fogColor[2] = 0.05f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 1.00f; lightDiffuse[1] = 0.35f; lightDiffuse[2] = 0.15f; lightDiffuse[3] = 1.0f;
            break;
        case 14: // Orbital Satellite Uplink (Arctic Polar Shimmer)
            fogColor[0] = 0.04f; fogColor[1] = 0.10f; fogColor[2] = 0.22f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.50f; lightDiffuse[1] = 0.85f; lightDiffuse[2] = 1.00f; lightDiffuse[3] = 1.0f;
            break;
        case 15: // The Omega Sanctuary (Chromatic Cataclysm Void)
            fogColor[0] = 0.18f; fogColor[1] = 0.04f; fogColor[2] = 0.16f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.95f; lightDiffuse[1] = 0.45f; lightDiffuse[2] = 0.90f; lightDiffuse[3] = 1.0f;
            break;
        case 16: // Volcanic Ridge Caldera (Basalt Ash & Fiery Amber)
            fogColor[0] = 0.19f; fogColor[1] = 0.09f; fogColor[2] = 0.07f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.95f; lightDiffuse[1] = 0.40f; lightDiffuse[2] = 0.22f; lightDiffuse[3] = 1.0f;
            break;
        case 17: // Airport Runway Hub (Turbulent Gale & Storm Slate)
            fogColor[0] = 0.12f; fogColor[1] = 0.16f; fogColor[2] = 0.22f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.85f; lightDiffuse[1] = 0.90f; lightDiffuse[2] = 0.98f; lightDiffuse[3] = 1.0f;
            break;
        case 18: // Old Port Canals (High Tide Surge & Oceanic Teal)
            fogColor[0] = 0.04f; fogColor[1] = 0.14f; fogColor[2] = 0.18f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.50f; lightDiffuse[1] = 0.85f; lightDiffuse[2] = 0.95f; lightDiffuse[3] = 1.0f;
            break;
        case 19: // High-Tech Silicon Core (EMP Neon Aurora)
            fogColor[0] = 0.14f; fogColor[1] = 0.05f; fogColor[2] = 0.24f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 0.70f; lightDiffuse[1] = 0.95f; lightDiffuse[2] = 1.00f; lightDiffuse[3] = 1.0f;
            break;
        case 20: // Armageddon Epicenter (Omega Core 11.0 Mw Cataclysm)
        default:
            fogColor[0] = 0.24f; fogColor[1] = 0.03f; fogColor[2] = 0.12f; fogColor[3] = 1.0f;
            lightDiffuse[0] = 1.00f; lightDiffuse[1] = 0.30f; lightDiffuse[2] = 0.50f; lightDiffuse[3] = 1.0f;
            break;
    }

    glClearColor(fogColor[0], fogColor[1], fogColor[2], 1.0f);
    glFogfv(GL_FOG_COLOR, fogColor);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    // Dual-Light Rig: Secondary sky fill light
    GLfloat skyFillDiffuse[4] = {
        lightDiffuse[0] * 0.38f,
        lightDiffuse[1] * 0.44f,
        lightDiffuse[2] * 0.62f,
        1.0f
    };
    GLfloat skyFillAmbient[4] = {
        lightAmbient[0] * 0.8f,
        lightAmbient[1] * 0.85f,
        lightAmbient[2] * 1.1f,
        1.0f
    };
    glLightfv(GL_LIGHT1, GL_AMBIENT, skyFillAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, skyFillDiffuse);
}

void addProp(PropType type, float x, float z, float rotY, float width, float height, float depth, float r, float g, float b, bool jumpable) {
    // Keep player spawn position clear
    float dxSpawn = x - playerX;
    float dzSpawn = z - playerZ;
    if (sqrt(dxSpawn * dxSpawn + dzSpawn * dzSpawn) < 7.5f) return;

    // Keep safe zone beacon pad clear
    float dxSafe = x - safeZoneX;
    float dzSafe = z - safeZoneZ;
    if (sqrt(dxSafe * dxSafe + dzSafe * dzSafe) < (safeZoneRadius + 3.0f)) return;

    StreetProp p;
    p.type = type;
    p.x = x;
    p.y = 0.0f;
    p.z = z;
    p.rotY = rotY;
    p.width = width;
    p.height = height;
    p.depth = depth;
    p.r = r; p.g = g; p.b = b;
    p.jumpable = jumpable;
    p.vx = 0.0f;
    p.vz = 0.0f;
    streetProps.push_back(p);
}

void populateStreetProps() {
    streetProps.clear();

    // 1. Street Trees in Planters at Sidewalk Intersections
    float treeCoords[4] = { -28.0f, -11.5f, 11.5f, 28.0f };
    for (int i = 0; i < 4; ++i) {
        addProp(PROP_TREE, -11.5f, treeCoords[i], 0.0f, 1.4f, 4.5f, 1.4f, 0.12f, 0.45f, 0.18f, false);
        addProp(PROP_TREE,  11.5f, treeCoords[i], 0.0f, 1.4f, 4.5f, 1.4f, 0.12f, 0.45f, 0.18f, false);
        addProp(PROP_TREE, treeCoords[i], -11.5f, 0.0f, 1.4f, 4.5f, 1.4f, 0.12f, 0.45f, 0.18f, false);
        addProp(PROP_TREE, treeCoords[i],  11.5f, 0.0f, 1.4f, 4.5f, 1.4f, 0.12f, 0.45f, 0.18f, false);
    }

    // 2. Fire Hydrants at Sidewalk Curbs
    addProp(PROP_FIRE_HYDRANT, -10.8f, -22.0f, 0.0f, 0.6f, 0.85f, 0.6f, 0.9f, 0.15f, 0.15f, true);
    addProp(PROP_FIRE_HYDRANT,  10.8f,  22.0f, 0.0f, 0.6f, 0.85f, 0.6f, 0.9f, 0.15f, 0.15f, true);
    addProp(PROP_FIRE_HYDRANT, -22.0f,  10.8f, 0.0f, 0.6f, 0.85f, 0.6f, 0.9f, 0.15f, 0.15f, true);
    addProp(PROP_FIRE_HYDRANT,  22.0f, -10.8f, 0.0f, 0.6f, 0.85f, 0.6f, 0.9f, 0.15f, 0.15f, true);

    // 3. Vehicles along North-South Avenue (|X| <= 7.0f)
    // Red Sedan (West lane)
    addProp(PROP_CAR, -3.4f, -25.0f, 18.0f, 2.2f, 1.4f, 4.4f, 0.85f, 0.18f, 0.18f, false);
    // Police Cruiser with Flashing Lights (East lane)
    addProp(PROP_POLICE_CAR, 3.2f, -13.0f, -22.0f, 2.2f, 1.55f, 4.4f, 0.95f, 0.95f, 0.98f, false);
    // Ambulance with Flashing Beacons (West lane)
    addProp(PROP_AMBULANCE, -3.6f, 8.0f, 28.0f, 2.3f, 1.6f, 4.6f, 0.95f, 0.95f, 0.95f, false);
    // Yellow Taxi (East lane)
    addProp(PROP_CAR, 3.4f, 23.0f, -15.0f, 2.2f, 1.4f, 4.4f, 0.95f, 0.78f, 0.12f, false);

    // 4. Vehicles along East-West Boulevard (|Z| <= 7.0f)
    // Blue Civilian Sedan
    addProp(PROP_CAR, -26.0f, -3.2f, 82.0f, 2.2f, 1.4f, 4.4f, 0.15f, 0.45f, 0.85f, false);
    // Silver SUV
    addProp(PROP_CAR, -12.0f, 3.0f, 102.0f, 2.3f, 1.5f, 4.5f, 0.72f, 0.75f, 0.80f, false);
    // Police Cruiser
    addProp(PROP_POLICE_CAR, 12.0f, -3.4f, 76.0f, 2.2f, 1.55f, 4.4f, 0.95f, 0.95f, 0.98f, false);
    // Red Sports Coupe
    addProp(PROP_CAR, 25.0f, 3.2f, 96.0f, 2.2f, 1.35f, 4.4f, 0.88f, 0.12f, 0.15f, false);

    // 5. Roadblocks & Barricades (Orange/White Construction Barriers)
    addProp(PROP_BARRICADE, 3.0f, -19.5f, 0.0f, 3.4f, 1.05f, 0.6f, 0.95f, 0.45f, 0.05f, true);
    addProp(PROP_BARRICADE, -3.0f, 16.0f, 0.0f, 3.4f, 1.05f, 0.6f, 0.95f, 0.45f, 0.05f, true);
    addProp(PROP_BARRICADE, -18.5f, 3.2f, 90.0f, 3.4f, 1.05f, 0.6f, 0.95f, 0.45f, 0.05f, true);
    addProp(PROP_BARRICADE, 18.0f, -3.0f, 90.0f, 3.4f, 1.05f, 0.6f, 0.95f, 0.45f, 0.05f, true);

    // 6. Concrete Jersey Barriers
    addProp(PROP_CONCRETE_BARRIER, -2.6f, -3.5f, 15.0f, 3.6f, 1.0f, 0.8f, 0.6f, 0.62f, 0.65f, true);
    addProp(PROP_CONCRETE_BARRIER, -4.0f, -2.5f, 85.0f, 3.6f, 1.0f, 0.8f, 0.6f, 0.62f, 0.65f, true);

    // 7. Fallen Lampposts across Lanes with Electric Sparks!
    addProp(PROP_FALLEN_LAMPPOST, 2.6f, -7.5f, 70.0f, 4.8f, 0.5f, 0.8f, 0.25f, 0.27f, 0.30f, true);
    addProp(PROP_FALLEN_LAMPPOST, -2.2f, 19.0f, -72.0f, 4.8f, 0.5f, 0.8f, 0.25f, 0.27f, 0.30f, true);
    addProp(PROP_FALLEN_LAMPPOST, -8.0f, 2.5f, 18.0f, 4.8f, 0.5f, 0.8f, 0.25f, 0.27f, 0.30f, true);
    addProp(PROP_FALLEN_LAMPPOST, 22.0f, -2.2f, -18.0f, 4.8f, 0.5f, 0.8f, 0.25f, 0.27f, 0.30f, true);

    // 8. Traffic Hazard Cones & Safety Drums
    addProp(PROP_TRAFFIC_CONE, -1.2f, -20.0f, 0.0f, 0.7f, 0.85f, 0.7f, 0.98f, 0.38f, 0.05f, true);
    addProp(PROP_TRAFFIC_CONE, -0.4f, -19.5f, 0.0f, 0.7f, 0.85f, 0.7f, 0.98f, 0.38f, 0.05f, true);
    addProp(PROP_HAZARD_DRUM, -3.0f, 14.8f, 0.0f, 0.8f, 1.0f, 0.8f, 0.92f, 0.76f, 0.10f, true);
    addProp(PROP_HAZARD_DRUM, -2.1f, 15.0f, 0.0f, 0.8f, 1.0f, 0.8f, 0.92f, 0.76f, 0.10f, true);
    addProp(PROP_TRAFFIC_CONE, 1.5f, 28.0f, 0.0f, 0.7f, 0.85f, 0.7f, 0.98f, 0.38f, 0.05f, true);
    addProp(PROP_TRAFFIC_CONE, 2.4f, 28.2f, 0.0f, 0.7f, 0.85f, 0.7f, 0.98f, 0.38f, 0.05f, true);
    addProp(PROP_HAZARD_DRUM, 16.5f, 2.5f, 0.0f, 0.8f, 1.0f, 0.8f, 0.92f, 0.76f, 0.10f, true);

    // 9. Rubble Piles (Masonry and Brick Chunks on Roadway)
    addProp(PROP_RUBBLE_PILE, -1.2f, -31.0f, 25.0f, 1.8f, 0.7f, 1.6f, 0.50f, 0.46f, 0.42f, true);
    addProp(PROP_RUBBLE_PILE,  1.5f,   3.0f, -18.0f, 1.8f, 0.7f, 1.6f, 0.50f, 0.46f, 0.42f, true);
    addProp(PROP_RUBBLE_PILE, -1.8f,   1.2f,  45.0f, 1.8f, 0.7f, 1.6f, 0.50f, 0.46f, 0.42f, true);
    addProp(PROP_RUBBLE_PILE, 28.0f,  -1.5f, -30.0f, 1.8f, 0.7f, 1.6f, 0.50f, 0.46f, 0.42f, true);

    // 10. Seismic Surface Fissures / Ground Cracks
    addProp(PROP_FISSURE, 0.0f, -16.0f, 12.0f, 5.5f, 0.1f, 1.2f, 0.08f, 0.08f, 0.10f, true);
    addProp(PROP_FISSURE, 0.0f,  12.0f, -15.0f, 5.5f, 0.1f, 1.2f, 0.08f, 0.08f, 0.10f, true);
    addProp(PROP_FISSURE, -16.0f, 0.0f, 75.0f, 5.5f, 0.1f, 1.2f, 0.08f, 0.08f, 0.10f, true);
    addProp(PROP_FISSURE,  14.0f, 0.0f, 105.0f, 5.5f, 0.1f, 1.2f, 0.08f, 0.08f, 0.10f, true);

    // 11. Additional Scaling Level-Specific Obstacles (Intensifies with level!)
    if (level >= 4) {
        addProp(PROP_BARRICADE, -14.0f, -14.0f, 45.0f, 3.4f, 1.05f, 0.6f, 0.95f, 0.45f, 0.05f, true);
        addProp(PROP_BARRICADE,  14.0f,  14.0f, 45.0f, 3.4f, 1.05f, 0.6f, 0.95f, 0.45f, 0.05f, true);
        addProp(PROP_RUBBLE_PILE, 15.0f, -15.0f, 30.0f, 1.9f, 0.75f, 1.7f, 0.48f, 0.44f, 0.40f, true);
        addProp(PROP_CAR, -15.0f, 15.0f, 40.0f, 2.2f, 1.4f, 4.4f, 0.25f, 0.25f, 0.28f, false);
    }
    if (level >= 7) {
        addProp(PROP_FALLEN_LAMPPOST, -15.0f, -6.0f, 60.0f, 4.8f, 0.5f, 0.8f, 0.24f, 0.26f, 0.28f, true);
        addProp(PROP_FALLEN_LAMPPOST,  6.0f,  16.0f, -30.0f, 4.8f, 0.5f, 0.8f, 0.24f, 0.26f, 0.28f, true);
        addProp(PROP_HAZARD_DRUM, -6.0f, -18.0f, 0.0f, 0.8f, 1.0f, 0.8f, 0.92f, 0.76f, 0.10f, true);
        addProp(PROP_HAZARD_DRUM,  18.0f,  6.0f, 0.0f, 0.8f, 1.0f, 0.8f, 0.92f, 0.76f, 0.10f, true);
        addProp(PROP_CONCRETE_BARRIER, 6.0f, -15.0f, 45.0f, 3.6f, 1.0f, 0.8f, 0.6f, 0.62f, 0.65f, true);
    }
    if (level >= 10) {
        addProp(PROP_POLICE_CAR, -6.0f, 25.0f, 20.0f, 2.2f, 1.55f, 4.4f, 0.95f, 0.95f, 0.98f, false);
        addProp(PROP_AMBULANCE,  25.0f, -6.0f, 110.0f, 2.3f, 1.6f, 4.6f, 0.95f, 0.95f, 0.95f, false);
        addProp(PROP_RUBBLE_PILE, -25.0f, -5.0f, 15.0f, 2.0f, 0.8f, 1.8f, 0.52f, 0.48f, 0.44f, true);
        addProp(PROP_FISSURE, -5.0f, -25.0f, 35.0f, 6.0f, 0.1f, 1.3f, 0.08f, 0.08f, 0.10f, true);
        addProp(PROP_FISSURE,  25.0f,  5.0f, -35.0f, 6.0f, 0.1f, 1.3f, 0.08f, 0.08f, 0.10f, true);
    }
    if (level >= 13) {
        addProp(PROP_CONCRETE_BARRIER, -20.0f, 20.0f, 135.0f, 3.6f, 1.0f, 0.8f, 0.6f, 0.62f, 0.65f, true);
        addProp(PROP_CONCRETE_BARRIER,  20.0f, -20.0f, 135.0f, 3.6f, 1.0f, 0.8f, 0.6f, 0.62f, 0.65f, true);
        addProp(PROP_RUBBLE_PILE, 0.0f, -35.0f, 0.0f, 2.2f, 0.85f, 1.9f, 0.46f, 0.42f, 0.38f, true);
        addProp(PROP_RUBBLE_PILE, 0.0f,  35.0f, 0.0f, 2.2f, 0.85f, 1.9f, 0.46f, 0.42f, 0.38f, true);
    }
}

void setAlertNotification(const std::string& msg) {
    hudAlertText = msg;
    hudAlertTimer = 3.2f;
}

void populateSurvivalPickups() {
    survivalPickups.clear();

    struct PickupDef {
        PickupType type;
        float x, z;
    };

    std::vector<PickupDef> defs = {
        { PICKUP_MEDKIT,      0.0f, -22.0f },
        { PICKUP_ADRENALINE,  2.5f,  -8.0f },
        { PICKUP_CHRONO,     -2.5f,  18.0f },
        { PICKUP_SHIELD,      0.0f,  26.0f },
        { PICKUP_ADRENALINE, -22.0f,  0.0f },
        { PICKUP_MEDKIT,     -8.0f,   2.5f },
        { PICKUP_CHRONO,     18.0f,  -2.5f },
        { PICKUP_SHIELD,     24.0f,   0.0f }
    };

    for (const auto& d : defs) {
        // Keep clear of player spawn and safe zone
        float dxSpawn = d.x - playerX;
        float dzSpawn = d.z - playerZ;
        if (sqrt(dxSpawn * dxSpawn + dzSpawn * dzSpawn) < 6.5f) continue;

        float dxSafe = d.x - safeZoneX;
        float dzSafe = d.z - safeZoneZ;
        if (sqrt(dxSafe * dxSafe + dzSafe * dzSafe) < (safeZoneRadius + 3.0f)) continue;

        SurvivalPickup sp;
        sp.type = d.type;
        sp.x = d.x;
        sp.y = 1.15f;
        sp.z = d.z;
        sp.rotY = static_cast<float>(rand() % 360);
        sp.hoverOffset = 0.0f;
        sp.collected = false;
        survivalPickups.push_back(sp);
    }
}

void setupLevelLayout() {
    buildings.clear();
    debrisList.clear();

    // Scale intensity multipliers
    float intensityBase = 1.0f;
    if (selectedIntensity == INTENSITY_LOW)     { intensityBase = 0.70f; scoreMultiplier = 1.0f; }
    if (selectedIntensity == INTENSITY_MEDIUM)  { intensityBase = 1.00f; scoreMultiplier = 1.5f; }
    if (selectedIntensity == INTENSITY_HIGH)    { intensityBase = 1.45f; scoreMultiplier = 2.2f; }

    quakeIntensity = intensityBase * (1.0f + std::min(2.2f, (level - 1) * 0.035f));

    int lvlIdx = std::max(1, std::min(100, level)) - 1;
    safeZoneX = g_levelTargets[lvlIdx].safeX;
    safeZoneZ = g_levelTargets[lvlIdx].safeZ;
    playerX = g_levelTargets[lvlIdx].spawnX;
    playerY = 1.0f;
    playerZ = g_levelTargets[lvlIdx].spawnZ;
    playerYaw = g_levelTargets[lvlIdx].spawnYaw;
    safeZoneRadius = (level % 5 == 0) ? 7.5f : 6.5f;

    // Generate Architectural Buildings in 4 Quadrants
    float xCols[] = { -35.0f, -18.0f, 18.0f, 35.0f };
    float zRows[] = { -35.0f, -18.0f, 18.0f, 35.0f };

    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            Building b;
            b.x = xCols[c];
            b.z = zRows[r];
            b.width = 14.0f;
            b.depth = 14.0f;
            b.height = (16.0f + std::min(45.0f, level * 0.40f)) + (rand() % 16);
            b.style = rand() % 3;
            b.hasAntenna = (b.height > 24.0f);
            b.seed = rand();
            if (b.style == 0) {
                b.r = 0.22f + (rand() % 15) / 100.0f;
                b.g = 0.26f + (rand() % 15) / 100.0f;
                b.b = 0.33f + (rand() % 15) / 100.0f;
            } else if (b.style == 1) {
                b.r = 0.48f + (rand() % 15) / 100.0f;
                b.g = 0.24f + (rand() % 10) / 100.0f;
                b.b = 0.18f + (rand() % 10) / 100.0f;
            } else {
                b.r = 0.32f + (rand() % 15) / 100.0f;
                b.g = 0.33f + (rand() % 15) / 100.0f;
                b.b = 0.36f + (rand() % 15) / 100.0f;
            }
            b.swayOffset = (rand() % 100) / 20.0f;
            buildings.push_back(b);
        }
    }

    // Populate Dynamic 3D Objects and Survival Pickups on the Path
    populateStreetProps();
    populateSurvivalPickups();
}

void initCity() {
    playerY = 1.0f;
    playerVelY = 0.0f;
    isGrounded = true;
    debrisDodged = 0;
    walkAnimTime = 0.0f;
    isMoving = false;

    // Interactive stats reset
    stamina = maxStamina;
    speedBoostTimer = 0.0f;
    shieldTimer = 0.0f;
    isSprinting = false;
    hudAlertTimer = 0.0f;
    hudAlertText = "";
    pickupsCollected = 0;

    // Reset camera shake and key inputs
    camShakeX = 0.0f;
    camShakeY = 0.0f;
    camShakeZ = 0.0f;
    for (int i = 0; i < 256; ++i) {
        keys[i] = false;
        specialKeys[i] = false;
    }

    // Base Time allocation per intensity
    float baseTime = 48.0f;
    if (selectedIntensity == INTENSITY_LOW)     baseTime = 56.0f;
    if (selectedIntensity == INTENSITY_MEDIUM)  baseTime = 48.0f;
    if (selectedIntensity == INTENSITY_HIGH)    baseTime = 40.0f;

    totalTime = std::max(18.0f, baseTime - (level - 1) * 0.32f);
    timer = totalTime;
    gameElapsed = 0.0f;
    gameState = STATE_EXPLORE;

    applyLevelEnvironment();
    setupLevelLayout();
}

void startNewGame(int selectedLvl) {
    level = selectedLvl;
    if (selectedLvl == 1) currentScore = 0;
    initCity();
}

// --------------------------------------------------------------------------
// Collision Detection (Axis-Aligned Bounding Box)
// --------------------------------------------------------------------------
bool checkCollision(float newX, float newZ) {
    float pRadius = 0.6f;

    // 1. Boundary Check
    if (newX < -48.0f || newX > 48.0f || newZ < -48.0f || newZ > 48.0f) {
        return true;
    }

    // 2. Building Collisions
    for (const auto& b : buildings) {
        float minX = b.x - b.width / 2.0f - pRadius;
        float maxX = b.x + b.width / 2.0f + pRadius;
        float minZ = b.z - b.depth / 2.0f - pRadius;
        float maxZ = b.z + b.depth / 2.0f + pRadius;

        if (newX >= minX && newX <= maxX && newZ >= minZ && newZ <= maxZ) {
            float curDistSq = (playerX - b.x) * (playerX - b.x) + (playerZ - b.z) * (playerZ - b.z);
            float newDistSq = (newX - b.x) * (newX - b.x) + (newZ - b.z) * (newZ - b.z);
            // Allow stepping away if already overlapping
            if (newDistSq <= curDistSq) {
                return true;
            }
        }
    }

    // 3. Fallen Debris Collisions
    for (const auto& d : debrisList) {
        if (d.landed) {
            float distSq = (newX - d.x) * (newX - d.x) + (newZ - d.z) * (newZ - d.z);
            float threshSq = (d.size + pRadius) * (d.size + pRadius);
            if (distSq < threshSq) {
                float curDistSq = (playerX - d.x) * (playerX - d.x) + (playerZ - d.z) * (playerZ - d.z);
                if (distSq <= curDistSq) {
                    return true;
                }
            }
        }
    }

    // 4. Street Prop Collisions (Vehicles, Barriers, Cones, Fallen Poles, Rubble)
    for (size_t i = 0; i < streetProps.size(); ++i) {
        auto& p = streetProps[i];
        if (p.type == PROP_FISSURE) continue; // Surface fissure is walkable
        // If player jumped high enough to clear jumpable obstacles, allow jumping over!
        if (p.jumpable && playerY > (p.height + 0.15f)) continue;

        float minX = p.x - p.width * 0.5f - pRadius;
        float maxX = p.x + p.width * 0.5f + pRadius;
        float minZ = p.z - p.depth * 0.5f - pRadius;
        float maxZ = p.z + p.depth * 0.5f + pRadius;

        if (newX >= minX && newX <= maxX && newZ >= minZ && newZ <= maxZ) {
            // Interactive dynamic obstacle physics:
            // Cones and Hazard drums can be kicked out of the way!
            if (p.type == PROP_TRAFFIC_CONE || p.type == PROP_HAZARD_DRUM) {
                float kickDirX = newX - playerX;
                float kickDirZ = newZ - playerZ;
                float klen = sqrt(kickDirX * kickDirX + kickDirZ * kickDirZ);
                if (klen > 0.001f) {
                    float pushSpeed = (isSprinting ? 9.5f : 6.0f);
                    p.vx = (kickDirX / klen) * pushSpeed;
                    p.vz = (kickDirZ / klen) * pushSpeed;
                    currentScore += 15;
                    setAlertNotification(p.type == PROP_TRAFFIC_CONE ? "Kicked Traffic Cone! +15 PTS" : "Nudged Safety Drum! +15 PTS");
                }
                continue; // Permeable kickable object
            }

            float curDistSq = (playerX - p.x) * (playerX - p.x) + (playerZ - p.z) * (playerZ - p.z);
            float newDistSq = (newX - p.x) * (newX - p.x) + (newZ - p.z) * (newZ - p.z);
            if (newDistSq <= curDistSq) {
                return true;
            }
        }
    }

    return false;
}

// --------------------------------------------------------------------------
// Game Loop Update
// --------------------------------------------------------------------------
void update(int value) {
    float dt = 0.016f; // ~60 FPS

    if (gameState == STATE_EXPLORE || gameState == STATE_WARNING || gameState == STATE_EARTHQUAKE) {
        gameElapsed += dt;
        timer -= dt;

        // Interactive HUD alert countdown
        if (hudAlertTimer > 0.0f) hudAlertTimer -= dt;
        if (speedBoostTimer > 0.0f) speedBoostTimer -= dt;
        if (shieldTimer > 0.0f) shieldTimer -= dt;

        // Stage Transitions
        if (gameElapsed >= 3.5f && gameElapsed < 7.0f) {
            gameState = STATE_WARNING;
        } else if (gameElapsed >= 7.0f) {
            gameState = STATE_EARTHQUAKE;
        }

        // Prop Dynamic Sliding Physics (Friction and Momentum)
        for (auto& p : streetProps) {
            if (fabs(p.vx) > 0.02f || fabs(p.vz) > 0.02f) {
                p.x += p.vx * dt;
                p.z += p.vz * dt;
                p.vx *= 0.88f;
                p.vz *= 0.88f;
                p.rotY += (p.vx + p.vz) * 14.0f;
            }
        }

        // Spawn falling debris during earthquake
        int spawnRate = 42;
        if (selectedIntensity == INTENSITY_LOW)     spawnRate = 58;
        if (selectedIntensity == INTENSITY_MEDIUM)  spawnRate = 38;
        if (selectedIntensity == INTENSITY_HIGH)    spawnRate = 26;
        spawnRate = std::max(12, spawnRate - (level - 1) * 2);

        if (gameState == STATE_EARTHQUAKE && (rand() % spawnRate == 0) && debrisList.size() < 35 && !buildings.empty()) {
            Debris d;
            int bIdx = rand() % buildings.size();
            d.x = buildings[bIdx].x + ((rand() % 100) / 50.0f - 1.0f) * 8.0f;
            d.y = buildings[bIdx].height * 0.85f;
            d.z = buildings[bIdx].z + ((rand() % 100) / 50.0f - 1.0f) * 8.0f;
            d.vx = ((rand() % 100) / 50.0f - 1.0f) * 2.5f;
            d.vy = -1.0f;
            d.vz = ((rand() % 100) / 50.0f - 1.0f) * 2.5f;
            d.rotX = 0; d.rotY = 0; d.rotZ = 0;
            d.rotSpeedX = static_cast<float>(rand() % 180);
            d.rotSpeedY = static_cast<float>(rand() % 180);
            d.size = 1.0f + (rand() % 12) / 10.0f;
            d.landed = false;
            debrisList.push_back(d);
        }

        // Update Debris Falling with Shield Protection
        for (auto& d : debrisList) {
            if (!d.landed) {
                d.vy -= 19.0f * dt; // Gravity
                d.x += d.vx * dt;
                d.y += d.vy * dt;
                d.z += d.vz * dt;
                d.rotX += d.rotSpeedX * dt;
                d.rotY += d.rotSpeedY * dt;

                // Interactive Shield Deflection
                if (shieldTimer > 0.0f) {
                    float distToPlayerSq = (d.x - playerX) * (d.x - playerX) + (d.z - playerZ) * (d.z - playerZ);
                    if (distToPlayerSq < 3.2f * 3.2f && fabs(d.y - playerY) < 2.5f) {
                        d.vy = 8.5f; // Bounce debris up and away
                        d.vx = ((rand() % 100) / 50.0f - 1.0f) * 6.0f;
                        d.vz = ((rand() % 100) / 50.0f - 1.0f) * 6.0f;
                        currentScore += 50;
                        setAlertNotification("SHIELD DEFLECTION! Hazard Deflected! +50 PTS");
                    }
                }

                if (d.y <= d.size / 2.0f) {
                    d.y = d.size / 2.0f;
                    d.landed = true;
                    debrisDodged++;
                    currentScore += static_cast<int>(35 * scoreMultiplier);

                    // Impact dust puff on pavement
                    for (int pCount = 0; pCount < 5; ++pCount) {
                        spawnParticle(d.x, 0.1f, d.z,
                                      ((rand() % 40) - 20) * 0.08f,
                                      0.6f + (rand() % 20) * 0.04f,
                                      ((rand() % 40) - 20) * 0.08f,
                                      0.62f, 0.58f, 0.52f, 0.45f, 0.75f, 0.9f, 0);
                    }
                }
            }
        }

        // Interactive Sprint & Stamina Mechanics
        bool wantsSprint = (keys['f'] || keys['F'] || keys['e'] || keys['E']);
        float moveSpeed = 8.2f;

        if (wantsSprint && isMoving && stamina > 3.0f) {
            isSprinting = true;
            stamina = std::max(0.0f, stamina - 28.0f * dt);
            moveSpeed = 12.2f;
        } else {
            isSprinting = false;
            stamina = std::min(maxStamina, stamina + 20.0f * dt);
        }

        // Adrenaline Speed Boost Multiplier
        if (speedBoostTimer > 0.0f) {
            moveSpeed *= 1.35f;
        }

        // Player Movement (WASD + Arrow Keys)
        float rad = playerYaw * PI / 180.0f;
        float forwardX = sin(rad);
        float forwardZ = -cos(rad);
        float rightX = cos(rad);
        float rightZ = sin(rad);

        float dx = 0.0f, dz = 0.0f;
        if (keys['w'] || keys['W'] || specialKeys[GLUT_KEY_UP])    { dx += forwardX; dz += forwardZ; }
        if (keys['s'] || keys['S'] || specialKeys[GLUT_KEY_DOWN])  { dx -= forwardX; dz -= forwardZ; }
        if (keys['a'] || keys['A'])                               { dx -= rightX;   dz -= rightZ; }
        if (keys['d'] || keys['D'])                               { dx += rightX;   dz += rightZ; }

        // Rotation (Q or Left/Right Arrow keys)
        if (keys['q'] || keys['Q'] || specialKeys[GLUT_KEY_LEFT])  playerYaw -= 95.0f * dt;
        if (specialKeys[GLUT_KEY_RIGHT])                           playerYaw += 95.0f * dt;

        float len = sqrt(dx * dx + dz * dz);
        if (len > 0.001f) {
            isMoving = true;
            walkAnimTime += dt * (isSprinting ? 16.0f : 10.5f);

            dx = (dx / len) * moveSpeed * dt;
            dz = (dz / len) * moveSpeed * dt;

            // X slide
            if (!checkCollision(playerX + dx, playerZ)) playerX += dx;
            // Z slide
            if (!checkCollision(playerX, playerZ + dz)) playerZ += dz;
        } else {
            isMoving = false;
        }

        // Jump Physics
        if (keys[' '] && isGrounded) {
            playerVelY = 7.2f;
            isGrounded = false;
        }
        playerVelY -= 20.0f * dt;
        playerY += playerVelY * dt;
        if (playerY <= 1.0f) {
            playerY = 1.0f;
            playerVelY = 0.0f;
            isGrounded = true;
        }

        // Interactive Survival Pickups Collection
        for (auto& sp : survivalPickups) {
            if (sp.collected) continue;
            sp.rotY += 120.0f * dt;
            sp.hoverOffset = sin(gameElapsed * 4.0f + sp.x) * 0.25f;

            float pdistSq = (playerX - sp.x) * (playerX - sp.x) + (playerZ - sp.z) * (playerZ - sp.z);
            if (pdistSq < 2.0f * 2.0f && fabs(playerY - sp.y) < 2.2f) {
                sp.collected = true;
                pickupsCollected++;
                if (sp.type == PICKUP_MEDKIT) {
                    currentScore += static_cast<int>(350 * scoreMultiplier);
                    stamina = maxStamina;
                    setAlertNotification("+350 PTS: Medkit Secured! Full Stamina Restored!");
                } else if (sp.type == PICKUP_ADRENALINE) {
                    speedBoostTimer = 7.0f;
                    currentScore += static_cast<int>(250 * scoreMultiplier);
                    setAlertNotification("+45% SPEED: Adrenaline Sprint Rush Active! (7s)");
                } else if (sp.type == PICKUP_CHRONO) {
                    timer += 8.0f;
                    currentScore += static_cast<int>(200 * scoreMultiplier);
                    setAlertNotification("+8.0s: Chrono Extender Time Added!");
                } else if (sp.type == PICKUP_SHIELD) {
                    shieldTimer = 8.5f;
                    currentScore += static_cast<int>(300 * scoreMultiplier);
                    setAlertNotification("SHIELD ON: Debris Deflection Active! (8.5s)");
                }
            }
        }

        // Update Particle Simulation
        updateParticles(dt);

        // Rotating 360 Radar Minimap Sweep
        radarSweepAngle += 180.0f * dt;
        if (radarSweepAngle >= 360.0f) radarSweepAngle -= 360.0f;

        // Footstep dust when player walks or sprints
        if (isMoving && isGrounded && fmod(walkAnimTime, 3.14159f) < 0.25f) {
            float footOffset = (sin(walkAnimTime) > 0 ? 0.22f : -0.22f);
            float footX = playerX + cos(rad) * footOffset - sin(rad) * 0.3f;
            float footZ = playerZ + sin(rad) * footOffset + cos(rad) * 0.3f;
            spawnParticle(footX, 0.05f, footZ,
                          -sin(rad) * 0.4f + ((rand() % 20) - 10) * 0.02f,
                          0.35f + (rand() % 15) * 0.02f,
                          cos(rad) * 0.4f + ((rand() % 20) - 10) * 0.02f,
                          0.55f, 0.52f, 0.48f, 0.35f, 0.45f, 0.6f, 3);
        }

        // Ambient atmospheric floating motes / ash drifting on wind
        if (rand() % 6 == 0 && particles.size() < MAX_PARTICLES) {
            float ax = playerX + ((rand() % 100) - 50) * 0.6f;
            float ay = 1.0f + (rand() % 60) * 0.1f;
            float az = playerZ + ((rand() % 100) - 50) * 0.6f;
            spawnParticle(ax, ay, az,
                          ((rand() % 20) - 10) * 0.04f,
                          -0.1f + ((rand() % 20) - 10) * 0.02f,
                          ((rand() % 20) - 10) * 0.04f,
                          0.85f, 0.88f, 0.95f, 0.4f, 0.18f, 2.5f, 2);
        }

        // Ground tremor dust billows during active earthquake
        if (gameState == STATE_EARTHQUAKE && (rand() % 3 == 0) && !buildings.empty()) {
            int bIdx = rand() % buildings.size();
            float bx = buildings[bIdx].x + ((rand() % 100) - 50) * 0.25f;
            float bz = buildings[bIdx].z + ((rand() % 100) - 50) * 0.25f;
            spawnParticle(bx, 0.1f, bz,
                          ((rand() % 40) - 20) * 0.06f,
                          0.8f + (rand() % 20) * 0.05f,
                          ((rand() % 40) - 20) * 0.06f,
                          0.62f, 0.58f, 0.52f, 0.45f, 0.9f, 1.4f, 0);
        }

        // Camera Shake & 6-DOF Trauma calculation during earthquake
        if (gameState == STATE_EARTHQUAKE) {
            float t = gameElapsed * 20.0f;
            camShakeX = (sin(t * 1.3f) + sin(t * 2.1f) * 0.5f) * 0.38f * quakeIntensity;
            camShakeY = (cos(t * 1.7f)) * 0.22f * quakeIntensity;
            camShakeZ = (sin(t * 1.1f)) * 0.38f * quakeIntensity;
            camShakePitch = sin(t * 1.4f) * 1.8f * quakeIntensity;
            camShakeRoll  = cos(t * 1.9f) * 2.4f * quakeIntensity;
        } else if (gameState == STATE_WARNING) {
            camShakeX = sin(gameElapsed * 15.0f) * 0.09f;
            camShakeY = 0.0f;
            camShakeZ = 0.0f;
            camShakePitch = sin(gameElapsed * 12.0f) * 0.4f;
            camShakeRoll  = 0.0f;
        } else {
            camShakeX = 0.0f; camShakeY = 0.0f; camShakeZ = 0.0f;
            camShakePitch = 0.0f; camShakeRoll = 0.0f;
        }

        // Pulsing Safe Zone Beacon
        beaconPulse = 1.0f + 0.20f * sin(gameElapsed * 4.5f);

        // Win Condition: Within safe zone radius
        float distToSafe = sqrt((playerX - safeZoneX) * (playerX - safeZoneX) + (playerZ - safeZoneZ) * (playerZ - safeZoneZ));
        if (distToSafe <= safeZoneRadius) {
            // Calculate Level Score
            int timePoints = static_cast<int>(timer * 110.0f);
            int dodgePoints = debrisDodged * 40;
            int baseLevelBonus = level * 1200;
            int pickupBonus = pickupsCollected * 150;
            levelScore = static_cast<int>((timePoints + dodgePoints + baseLevelBonus + pickupBonus) * scoreMultiplier);
            currentScore += levelScore;
            if (currentScore > highScore) highScore = currentScore;

            // Unlock next level
            if (level >= highestUnlockedLevel && highestUnlockedLevel < MAX_LEVELS) {
                highestUnlockedLevel = level + 1;
            }

            if (level < MAX_LEVELS) {
                gameState = STATE_LEVEL_CLEAR;
            } else {
                gameState = STATE_VICTORY;
            }
        } else if (timer <= 0.0f) {
            gameState = STATE_GAMEOVER;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// --------------------------------------------------------------------------
// Rendering Functions
// --------------------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Setup Camera Matrix (gluLookAt) with 6-DOF Trauma
    float rad = playerYaw * PI / 180.0f;
    float lookX = sin(rad);
    float lookZ = -cos(rad);
    float upX = sin(camShakeRoll * PI / 180.0f);
    float upY = cos(camShakeRoll * PI / 180.0f);

    if (isFirstPerson) {
        gluLookAt(
            playerX + camShakeX, playerY + 0.65f + camShakeY, playerZ + camShakeZ,
            playerX + lookX + camShakeX, playerY + 0.65f + camShakeY + (camShakePitch * 0.02f), playerZ + lookZ + camShakeZ,
            upX, upY, 0.0f
        );
    } else {
        // Third Person Camera: positioned behind & elevated above avatar
        float camDist = 5.8f;
        float camEyeX = playerX - lookX * camDist + camShakeX;
        float camEyeY = playerY + 3.2f + camShakeY;
        float camEyeZ = playerZ - lookZ * camDist + camShakeZ;

        gluLookAt(
            camEyeX, camEyeY, camEyeZ,
            playerX, playerY + 1.2f + (camShakePitch * 0.04f), playerZ,
            upX, upY, 0.0f
        );
        drawPlayerAvatar();
    }

    // 1. Procedural Sky Dome & Skyline Backdrop
    drawSkyBackdrop();

    // 2. Contact Shadows (Player, Vehicles, Debris)
    drawContactShadow(playerX, playerZ, 0.65f, 0.65f, 0.45f);
    for (const auto& p : streetProps) {
        if (p.type == PROP_CAR || p.type == PROP_POLICE_CAR || p.type == PROP_AMBULANCE) {
            drawContactShadow(p.x, p.z, 1.4f, 2.5f, 0.55f);
        }
    }
    for (const auto& d : debrisList) {
        if (d.landed) {
            drawContactShadow(d.x, d.z, d.size * 0.65f, d.size * 0.65f, 0.42f);
        }
    }

    // 3. Render 3D World Scene Components
    drawCity();
    drawSafeZone();
    draw3DObjectiveMarker();
    drawDebris();
    drawParticles();

    // 4. Render 2D UI / HUD on top
    drawHUD();

    glutSwapBuffers();
}

// --------------------------------------------------------------------------
// Architectural Window Renderer
// --------------------------------------------------------------------------
void drawWindowOnWall(float cx, float cy, float cz, float w, float h, float nx, float nz, int litState) {
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    float border = 0.16f;

    // 1. Window Outer Dark Frame
    glColor3f(0.12f, 0.13f, 0.16f);
    glBegin(GL_QUADS);
    glNormal3f(nx, 0.0f, nz);
    if (nz != 0.0f) {
        float z = cz + nz * 0.04f;
        glVertex3f(cx - hw - border, cy - hh - border, z);
        glVertex3f(cx + hw + border, cy - hh - border, z);
        glVertex3f(cx + hw + border, cy + hh + border, z);
        glVertex3f(cx - hw - border, cy + hh + border, z);
    } else {
        float x = cx + nx * 0.04f;
        glVertex3f(x, cy - hh - border, cz - hw - border);
        glVertex3f(x, cy - hh - border, cz + hw + border);
        glVertex3f(x, cy + hh + border, cz + hw + border);
        glVertex3f(x, cy + hh + border, cz - hw - border);
    }
    glEnd();

    // 2. Glass Pane
    if (litState == 2) {
        glColor3f(0.98f, 0.88f, 0.42f); // Warm illuminated interior light
    } else if (litState == 1) {
        glColor3f(0.42f, 0.68f, 0.88f); // Daylight reflective sky glass
    } else {
        glColor3f(0.18f, 0.24f, 0.32f); // Tinted dark glass
    }

    glBegin(GL_QUADS);
    glNormal3f(nx, 0.0f, nz);
    if (nz != 0.0f) {
        float z = cz + nz * 0.06f;
        glVertex3f(cx - hw, cy - hh, z);
        glVertex3f(cx + hw, cy - hh, z);
        glVertex3f(cx + hw, cy + hh, z);
        glVertex3f(cx - hw, cy + hh, z);
    } else {
        float x = cx + nx * 0.06f;
        glVertex3f(x, cy - hh, cz - hw);
        glVertex3f(x, cy - hh, cz + hw);
        glVertex3f(x, cy + hh, cz + hw);
        glVertex3f(x, cy + hh, cz - hw);
    }
    glEnd();

    // 3. Mullions (Architectural Cross-bar dividing window into 4 panes)
    glColor3f(0.12f, 0.13f, 0.16f);
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    glNormal3f(nx, 0.0f, nz);
    if (nz != 0.0f) {
        float z = cz + nz * 0.07f;
        glVertex3f(cx - hw, cy, z);
        glVertex3f(cx + hw, cy, z);
        glVertex3f(cx, cy - hh, z);
        glVertex3f(cx, cy + hh, z);
    } else {
        float x = cx + nx * 0.07f;
        glVertex3f(x, cy, cz - hw);
        glVertex3f(x, cy, cz + hw);
        glVertex3f(x, cy - hh, cz);
        glVertex3f(x, cy + hh, cz);
    }
    glEnd();
    glLineWidth(1.0f);
}

// --------------------------------------------------------------------------
// Architectural Entrance Door & Porch Canopy (Front Facade +Z)
// --------------------------------------------------------------------------
void drawEntranceDoor(float halfW, float halfD) {
    float z = halfD;

    // 1. Entrance Portal Stone Surround Frame
    glColor3f(0.18f, 0.20f, 0.24f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-2.4f, 0.0f, z + 0.04f);
    glVertex3f( 2.4f, 0.0f, z + 0.04f);
    glVertex3f( 2.4f, 3.8f, z + 0.04f);
    glVertex3f(-2.4f, 3.8f, z + 0.04f);
    glEnd();

    // 2. Double Doors (Left & Right Leaves)
    // Left Door Leaf
    glColor3f(0.16f, 0.17f, 0.20f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.9f, 0.0f, z + 0.06f);
    glVertex3f(-0.1f, 0.0f, z + 0.06f);
    glVertex3f(-0.1f, 3.3f, z + 0.06f);
    glVertex3f(-1.9f, 3.3f, z + 0.06f);
    glEnd();

    // Left Door Glass Panel
    glColor3f(0.35f, 0.60f, 0.80f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.6f, 1.1f, z + 0.08f);
    glVertex3f(-0.4f, 1.1f, z + 0.08f);
    glVertex3f(-0.4f, 3.0f, z + 0.08f);
    glVertex3f(-1.6f, 3.0f, z + 0.08f);
    glEnd();

    // Right Door Leaf
    glColor3f(0.16f, 0.17f, 0.20f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.1f, 0.0f, z + 0.06f);
    glVertex3f(1.9f, 0.0f, z + 0.06f);
    glVertex3f(1.9f, 3.3f, z + 0.06f);
    glVertex3f(0.1f, 3.3f, z + 0.06f);
    glEnd();

    // Right Door Glass Panel
    glColor3f(0.35f, 0.60f, 0.80f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.4f, 1.1f, z + 0.08f);
    glVertex3f(1.6f, 1.1f, z + 0.08f);
    glVertex3f(1.6f, 3.0f, z + 0.08f);
    glVertex3f(0.4f, 3.0f, z + 0.08f);
    glEnd();

    // Brass / Chrome Push Handles
    glColor3f(0.85f, 0.75f, 0.35f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.35f, 1.2f, z + 0.12f);
    glVertex3f(-0.35f, 1.8f, z + 0.12f);
    glVertex3f( 0.35f, 1.2f, z + 0.12f);
    glVertex3f( 0.35f, 1.8f, z + 0.12f);
    glEnd();
    glLineWidth(1.0f);

    // 3. Ground Floor Flanking Storefront Display Windows
    // Left Storefront Window
    glColor3f(0.30f, 0.55f, 0.75f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-7.6f, 0.6f, z + 0.05f);
    glVertex3f(-3.0f, 0.6f, z + 0.05f);
    glVertex3f(-3.0f, 3.2f, z + 0.05f);
    glVertex3f(-7.6f, 3.2f, z + 0.05f);
    glEnd();

    // Right Storefront Window
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(3.0f, 0.6f, z + 0.05f);
    glVertex3f(7.6f, 0.6f, z + 0.05f);
    glVertex3f(7.6f, 3.2f, z + 0.05f);
    glVertex3f(3.0f, 3.2f, z + 0.05f);
    glEnd();

    // 4. Entrance Marquee / Overhang Canopy
    glColor3f(0.22f, 0.24f, 0.28f);
    glPushMatrix();
    glTranslatef(0.0f, 3.7f, z + 1.1f);
    glScalef(5.6f, 0.26f, 2.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Under-canopy Warm Entrance Porch Light
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.95f, 0.72f);
    glPushMatrix();
    glTranslatef(0.0f, 3.55f, z + 1.0f);
    glScalef(0.6f, 0.08f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    // Diagonal Metal Support Struts
    glColor3f(0.15f, 0.15f, 0.18f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex3f(-2.4f, 4.4f, z + 0.05f);
    glVertex3f(-2.4f, 3.7f, z + 2.0f);
    glVertex3f( 2.4f, 4.4f, z + 0.05f);
    glVertex3f( 2.4f, 3.7f, z + 2.0f);
    glEnd();
    glLineWidth(1.0f);
}

// --------------------------------------------------------------------------
// Rear Emergency Steel Door with Illuminated EXIT Sign (-Z)
// --------------------------------------------------------------------------
void drawEmergencyExitDoor(float halfW, float halfD) {
    float z = -halfD;

    // Metal Door Frame
    glColor3f(0.18f, 0.19f, 0.22f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-1.4f, 0.0f, z - 0.04f);
    glVertex3f( 1.4f, 0.0f, z - 0.04f);
    glVertex3f( 1.4f, 2.8f, z - 0.04f);
    glVertex3f(-1.4f, 2.8f, z - 0.04f);
    glEnd();

    // Heavy Industrial Steel Door
    glColor3f(0.35f, 0.38f, 0.42f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-1.1f, 0.0f, z - 0.06f);
    glVertex3f( 1.1f, 0.0f, z - 0.06f);
    glVertex3f( 1.1f, 2.6f, z - 0.06f);
    glVertex3f(-1.1f, 2.6f, z - 0.06f);
    glEnd();

    // Red Emergency EXIT Sign above door
    glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.8f, 2.9f, z - 0.07f);
    glVertex3f( 0.8f, 2.9f, z - 0.07f);
    glVertex3f( 0.8f, 3.25f, z - 0.07f);
    glVertex3f(-0.8f, 3.25f, z - 0.07f);
    glEnd();
    glEnable(GL_LIGHTING);
}

// --------------------------------------------------------------------------
// Rooftop Architectural Elements
// --------------------------------------------------------------------------
void drawRooftop(const Building& b) {
    float halfW = b.width * 0.5f;
    float halfD = b.depth * 0.5f;
    float roofY = b.height;

    // 1. Sunken Dark Gravel Roof Surface
    glColor3f(0.14f, 0.15f, 0.17f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfW + 0.3f, roofY + 0.02f, -halfD + 0.3f);
    glVertex3f( halfW - 0.3f, roofY + 0.02f, -halfD + 0.3f);
    glVertex3f( halfW - 0.3f, roofY + 0.02f,  halfD - 0.3f);
    glVertex3f(-halfW + 0.3f, roofY + 0.02f,  halfD - 0.3f);
    glEnd();

    // 2. Parapet Perimeter Ledges (Border Wall around roof edge)
    glColor3f(b.r * 0.85f, b.g * 0.85f, b.b * 0.85f);
    // North wall
    glPushMatrix();
    glTranslatef(0.0f, roofY + 0.45f, -halfD + 0.25f);
    glScalef(b.width, 0.9f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();
    // South wall
    glPushMatrix();
    glTranslatef(0.0f, roofY + 0.45f, halfD - 0.25f);
    glScalef(b.width, 0.9f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();
    // West wall
    glPushMatrix();
    glTranslatef(-halfW + 0.25f, roofY + 0.45f, 0.0f);
    glScalef(0.5f, 0.9f, b.depth - 1.0f);
    glutSolidCube(1.0);
    glPopMatrix();
    // East wall
    glPushMatrix();
    glTranslatef(halfW - 0.25f, roofY + 0.45f, 0.0f);
    glScalef(0.5f, 0.9f, b.depth - 1.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 3. Rooftop Stairwell Penthouse (Access Shed)
    glColor3f(0.28f, 0.30f, 0.35f);
    glPushMatrix();
    glTranslatef(-3.2f, roofY + 1.4f, -3.2f);
    glScalef(4.0f, 2.8f, 4.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Roof Access Metal Door
    glColor3f(0.20f, 0.22f, 0.26f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-4.0f, roofY + 0.05f, -1.15f);
    glVertex3f(-2.4f, roofY + 0.05f, -1.15f);
    glVertex3f(-2.4f, roofY + 2.2f,  -1.15f);
    glVertex3f(-4.0f, roofY + 2.2f,  -1.15f);
    glEnd();

    // 4. Rooftop HVAC Air Conditioning Compressors
    glColor3f(0.40f, 0.42f, 0.46f);
    glPushMatrix();
    glTranslatef(3.5f, roofY + 0.8f, 3.0f);
    glScalef(2.6f, 1.6f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // HVAC Circular Fan Grill
    glDisable(GL_LIGHTING);
    glColor3f(0.15f, 0.16f, 0.18f);
    glPushMatrix();
    glTranslatef(3.5f, roofY + 1.62f, 3.0f);
    glScalef(1.2f, 0.05f, 1.2f);
    glutSolidCube(1.0);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    // 5. Water Tower / Tank (Industrial Barrel on Stilts)
    // 4 Steel Legs
    glColor3f(0.25f, 0.26f, 0.30f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex3f(2.0f, roofY, -4.5f); glVertex3f(2.0f, roofY + 1.5f, -4.5f);
    glVertex3f(4.8f, roofY, -4.5f); glVertex3f(4.8f, roofY + 1.5f, -4.5f);
    glVertex3f(2.0f, roofY, -1.7f); glVertex3f(2.0f, roofY + 1.5f, -1.7f);
    glVertex3f(4.8f, roofY, -1.7f); glVertex3f(4.8f, roofY + 1.5f, -1.7f);
    glEnd();
    glLineWidth(1.0f);

    // Water Tank Barrel
    glColor3f(0.45f, 0.40f, 0.35f);
    glPushMatrix();
    glTranslatef(3.4f, roofY + 2.6f, -3.1f);
    glScalef(2.8f, 2.2f, 2.8f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Conical Tank Roof
    glColor3f(0.32f, 0.30f, 0.28f);
    glPushMatrix();
    glTranslatef(3.4f, roofY + 3.9f, -3.1f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(1.8, 1.2, 12, 4);
    glPopMatrix();

    // 6. Tall Communication Antenna Mast with Blinking Beacon Light
    if (b.hasAntenna) {
        float antBaseY = roofY + 2.8f;
        glColor3f(0.35f, 0.38f, 0.42f);
        glLineWidth(2.5f);
        glBegin(GL_LINES);
        glVertex3f(-3.2f, antBaseY, -3.2f);
        glVertex3f(-3.2f, antBaseY + 8.5f, -3.2f);
        glEnd();
        glLineWidth(1.0f);

        // Blinking Red Aviation Obstruction Warning Light
        glDisable(GL_LIGHTING);
        bool blink = (fmod(gameElapsed, 0.8f) < 0.4f);
        if (blink) glColor3f(1.0f, 0.15f, 0.15f);
        else       glColor3f(0.3f, 0.05f, 0.05f);

        glPushMatrix();
        glTranslatef(-3.2f, antBaseY + 8.6f, -3.2f);
        glutSolidSphere(0.24, 8, 8);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }
}

// --------------------------------------------------------------------------
// Full Detailed Architectural Building
// --------------------------------------------------------------------------
void drawDetailedBuilding(const Building& b) {
    glPushMatrix();
    glTranslatef(b.x, 0.0f, b.z);

    // Dynamic Earthquake Sway Rotation around the base
    if (gameState == STATE_EARTHQUAKE) {
        float sway = sin(gameElapsed * 6.2f + b.swayOffset) * 2.3f * quakeIntensity;
        glRotatef(sway, 0.0f, 0.0f, 1.0f);
    }

    float halfW = b.width * 0.5f;
    float halfD = b.depth * 0.5f;

    // 1. Concrete Sidewalk Curb
    glColor3f(0.60f, 0.62f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, 0.09f, 0.0f);
    glScalef(b.width + 3.2f, 0.18f, b.depth + 3.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 2. Foundation Stone Base Trim (Dark granite)
    glColor3f(0.18f, 0.20f, 0.24f);
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f);
    glScalef(b.width + 0.15f, 1.2f, b.depth + 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 3. Main Building Core Walls
    glColor3f(b.r, b.g, b.b);
    glPushMatrix();
    glTranslatef(0.0f, b.height * 0.5f, 0.0f);
    glScalef(b.width, b.height, b.depth);
    glutSolidCube(1.0);
    glPopMatrix();

    // 4. Ground Floor Entrances & Storefronts
    drawEntranceDoor(halfW, halfD);
    drawEmergencyExitDoor(halfW, halfD);

    // 5. Windows on All 4 Facades (Front, Back, Left, Right)
    float colOffsets[4] = { -5.6f, -1.9f, 1.9f, 5.6f };
    float floorHeight = 3.3f;
    int maxFloors = static_cast<int>((b.height - 4.5f) / floorHeight);

    for (int fl = 1; fl <= maxFloors; ++fl) {
        float cy = 4.4f + (fl - 1) * floorHeight;
        if (cy + 1.2f >= b.height) break;

        // Architectural horizontal floor division ledge / cornice
        glColor3f(b.r * 0.9f, b.g * 0.9f, b.b * 0.9f);
        glPushMatrix();
        glTranslatef(0.0f, cy - 1.2f, 0.0f);
        glScalef(b.width + 0.2f, 0.25f, b.depth + 0.2f);
        glutSolidCube(1.0);
        glPopMatrix();

        for (int c = 0; c < 4; ++c) {
            float u = colOffsets[c];
            // Deterministic lighting variation per window
            int litKey = (b.seed + fl * 7 + c * 13) % 10;
            int litState = 1; // 1 = sky reflective
            if (litKey < 3) litState = 2; // Warm interior office light
            else if (litKey > 7) litState = 0; // Dark tinted window

            // Front (+Z)
            drawWindowOnWall(u, cy, halfD, 2.1f, 1.7f, 0.0f, 1.0f, litState);
            // Back (-Z)
            drawWindowOnWall(u, cy, -halfD, 2.1f, 1.7f, 0.0f, -1.0f, (litState + 1) % 3);
            // Right (+X)
            drawWindowOnWall(halfW, cy, u, 2.1f, 1.7f, 1.0f, 0.0f, (litState + 2) % 3);
            // Left (-X)
            drawWindowOnWall(-halfW, cy, u, 2.1f, 1.7f, -1.0f, 0.0f, litState);
        }
    }

    // 6. Rooftop Architecture (Parapet, Penthouse, Water Tower, AC, Antenna)
    drawRooftop(b);

    glPopMatrix();
}

// --------------------------------------------------------------------------
// Street Lamppost Renderer
// --------------------------------------------------------------------------
void drawStreetLamppost(float lx, float lz) {
    glPushMatrix();
    glTranslatef(lx, 0.0f, lz);

    // Concrete base
    glColor3f(0.4f, 0.42f, 0.45f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    glScalef(0.6f, 0.6f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Metal pole
    glColor3f(0.22f, 0.25f, 0.28f);
    glPushMatrix();
    glTranslatef(0.0f, 2.8f, 0.0f);
    glScalef(0.16f, 5.0f, 0.16f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Horizontal arm
    glPushMatrix();
    glTranslatef(0.6f, 5.2f, 0.0f);
    glScalef(1.4f, 0.12f, 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Glowing Lantern head
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.92f, 0.60f); // Warm sodium street light
    glPushMatrix();
    glTranslatef(1.2f, 4.95f, 0.0f);
    glScalef(0.45f, 0.35f, 0.45f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Warm street lamppost ground light pool (Unity-style volumetric illumination)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.88f, 0.50f, 0.22f);
    glVertex3f(1.2f, 0.02f, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 18; ++i) {
        float a = i * 2.0f * PI / 18.0f;
        glVertex3f(1.2f + cos(a) * 5.2f, 0.02f, sin(a) * 5.2f);
    }
    glEnd();
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

// --------------------------------------------------------------------------
// Street Prop 3D Renderers (Objects on the Path)
// --------------------------------------------------------------------------
void drawVehicleProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);
    glRotatef(p.rotY, 0.0f, 1.0f, 0.0f);

    // 1. Lower Chassis
    glColor3f(p.r, p.g, p.b);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    glScalef(2.1f, 0.65f, 4.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 2. Bumpers (Front and Rear)
    glColor3f(0.16f, 0.18f, 0.20f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 2.22f);
    glScalef(2.05f, 0.35f, 0.25f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, -2.22f);
    glScalef(2.05f, 0.35f, 0.25f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 3. Cabin & Passenger Roof
    glColor3f(p.r * 0.88f, p.g * 0.88f, p.b * 0.88f);
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, -0.2f);
    glScalef(1.85f, 0.65f, 2.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 4. Tinted Windshield & Rear Window
    glColor3f(0.12f, 0.18f, 0.26f);
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, 1.02f);
    glScalef(1.7f, 0.55f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, -1.42f);
    glScalef(1.7f, 0.55f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 5. Four Rubber Tires with Silver Wheel Hubs
    float tireX[4] = { -1.05f, 1.05f, -1.05f, 1.05f };
    float tireZ[4] = {  1.35f, 1.35f, -1.35f, -1.35f };
    for (int w = 0; w < 4; ++w) {
        glPushMatrix();
        glTranslatef(tireX[w], 0.35f, tireZ[w]);
        glColor3f(0.12f, 0.12f, 0.14f);
        glScalef(0.32f, 0.7f, 0.7f);
        glutSolidCube(1.0);
        // Silver Hubcap
        glColor3f(0.7f, 0.72f, 0.75f);
        glPushMatrix();
        glTranslatef((tireX[w] > 0 ? 0.15f : -0.15f), 0.0f, 0.0f);
        glScalef(0.08f, 0.4f, 0.4f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPopMatrix();
    }

    // 6. Glowing Headlights & Red Taillights
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.98f, 0.82f); // Headlights
    glPushMatrix();
    glTranslatef(-0.7f, 0.6f, 2.22f);
    glScalef(0.4f, 0.22f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.7f, 0.6f, 2.22f);
    glScalef(0.4f, 0.22f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Volumetric Headlight Beams projecting forward onto the road
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glBegin(GL_TRIANGLES);
    // Left beam
    glColor4f(1.0f, 0.98f, 0.75f, 0.26f);
    glVertex3f(-0.7f, 0.6f, 2.22f);
    glColor4f(1.0f, 0.98f, 0.75f, 0.0f);
    glVertex3f(-1.9f, 0.02f, 10.5f);
    glVertex3f( 0.2f, 0.02f, 10.5f);
    // Right beam
    glColor4f(1.0f, 0.98f, 0.75f, 0.26f);
    glVertex3f(0.7f, 0.6f, 2.22f);
    glColor4f(1.0f, 0.98f, 0.75f, 0.0f);
    glVertex3f(-0.2f, 0.02f, 10.5f);
    glVertex3f( 1.9f, 0.02f, 10.5f);
    glEnd();
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    glColor3f(0.95f, 0.15f, 0.15f); // Taillights
    glPushMatrix();
    glTranslatef(-0.7f, 0.6f, -2.22f);
    glScalef(0.4f, 0.22f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.7f, 0.6f, -2.22f);
    glScalef(0.4f, 0.22f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 7. Police & Ambulance Flashing Emergency Beacons & Dynamic Siren Ground Pools
    if (p.type == PROP_POLICE_CAR) {
        bool blink = (fmod(gameElapsed, 0.4f) < 0.2f);
        // Left Red Beacon
        if (blink) glColor3f(1.0f, 0.15f, 0.15f); else glColor3f(0.25f, 0.02f, 0.02f);
        glPushMatrix();
        glTranslatef(-0.45f, 1.55f, -0.2f);
        glScalef(0.5f, 0.2f, 0.3f);
        glutSolidCube(1.0);
        glPopMatrix();
        // Right Blue Beacon
        if (!blink) glColor3f(0.15f, 0.45f, 1.0f); else glColor3f(0.02f, 0.05f, 0.25f);
        glPushMatrix();
        glTranslatef(0.45f, 1.55f, -0.2f);
        glScalef(0.5f, 0.2f, 0.3f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Pulsating Ground Siren Reflection
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        glBegin(GL_TRIANGLE_FAN);
        if (blink) glColor4f(0.85f, 0.12f, 0.12f, 0.32f);
        else       glColor4f(0.12f, 0.45f, 0.95f, 0.32f);
        glVertex3f(0.0f, 0.02f, 0.0f);
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i <= 20; ++i) {
            float a = i * 2.0f * PI / 20.0f;
            glVertex3f(cos(a) * 6.5f, 0.02f, sin(a) * 6.5f);
        }
        glEnd();
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
    } else if (p.type == PROP_AMBULANCE) {
        bool blink = (fmod(gameElapsed, 0.35f) < 0.18f);
        if (blink) glColor3f(1.0f, 0.25f, 0.1f); else glColor3f(0.3f, 0.05f, 0.02f);
        glPushMatrix();
        glTranslatef(0.0f, 1.58f, -0.2f);
        glScalef(1.2f, 0.22f, 0.35f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Pulsating Ambulance Amber/Red Ground Reflection
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(0.95f, 0.25f, 0.08f, blink ? 0.35f : 0.08f);
        glVertex3f(0.0f, 0.02f, 0.0f);
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i <= 20; ++i) {
            float a = i * 2.0f * PI / 20.0f;
            glVertex3f(cos(a) * 6.5f, 0.02f, sin(a) * 6.5f);
        }
        glEnd();
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawBarricadeProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);
    glRotatef(p.rotY, 0.0f, 1.0f, 0.0f);

    // A-Frame Metal Legs
    glColor3f(0.25f, 0.27f, 0.30f);
    glPushMatrix();
    glTranslatef(-p.width * 0.45f, 0.5f, 0.0f);
    glScalef(0.12f, 1.0f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(p.width * 0.45f, 0.5f, 0.0f);
    glScalef(0.12f, 1.0f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Horizontal Orange Roadblock Plank
    glColor3f(0.95f, 0.45f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, 0.72f, 0.0f);
    glScalef(p.width, 0.36f, 0.14f);
    glutSolidCube(1.0);
    glPopMatrix();

    // White Warning Chevrons / Stripes on Plank
    glColor3f(0.98f, 0.98f, 0.98f);
    for (float sx = -p.width * 0.35f; sx <= p.width * 0.35f; sx += 0.7f) {
        glPushMatrix();
        glTranslatef(sx, 0.72f, 0.08f);
        glScalef(0.24f, 0.34f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(sx, 0.72f, -0.08f);
        glScalef(0.24f, 0.34f, 0.02f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // Flashing Amber Hazard Beacons on top of legs
    glDisable(GL_LIGHTING);
    bool blink = (fmod(gameElapsed + p.x, 0.7f) < 0.35f);
    if (blink) glColor3f(1.0f, 0.85f, 0.1f);
    else       glColor3f(0.35f, 0.25f, 0.02f);

    glPushMatrix();
    glTranslatef(-p.width * 0.45f, 1.08f, 0.0f);
    glutSolidSphere(0.14, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(p.width * 0.45f, 1.08f, 0.0f);
    glutSolidSphere(0.14, 8, 8);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawConcreteBarrierProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);
    glRotatef(p.rotY, 0.0f, 1.0f, 0.0f);

    // Concrete Base
    glColor3f(0.55f, 0.56f, 0.58f);
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);
    glScalef(p.width, 0.5f, p.depth);
    glutSolidCube(1.0);
    glPopMatrix();

    // Upper Sloped Section
    glColor3f(0.62f, 0.63f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 0.0f);
    glScalef(p.width, 0.5f, p.depth * 0.55f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Reflective Hazard Stripe
    glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.8f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, p.depth * 0.5f + 0.02f);
    glScalef(p.width * 0.9f, 0.15f, 0.02f);
    glutSolidCube(1.0);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawTrafficConeProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);

    // Black square base pad
    glColor3f(0.12f, 0.12f, 0.14f);
    glPushMatrix();
    glTranslatef(0.0f, 0.03f, 0.0f);
    glScalef(0.7f, 0.06f, 0.7f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Fluorescent Orange Cone
    glColor3f(0.98f, 0.38f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, 0.06f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.28, 0.85, 12, 4);
    glPopMatrix();

    // White reflective collar around cone
    glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.95f, 0.95f);
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.20, 0.22, 12, 2);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawHazardDrumProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);

    // Yellow barrel body
    glColor3f(0.92f, 0.76f, 0.10f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(0.75f, 1.0f, 0.75f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Black warning ring bands
    glColor3f(0.14f, 0.14f, 0.16f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    glScalef(0.77f, 0.15f, 0.77f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.68f, 0.0f);
    glScalef(0.77f, 0.15f, 0.77f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawFallenLamppostProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);
    glRotatef(p.rotY, 0.0f, 1.0f, 0.0f);

    // Cracked concrete stump on curb
    glColor3f(0.42f, 0.44f, 0.46f);
    glPushMatrix();
    glTranslatef(-2.2f, 0.25f, 0.0f);
    glScalef(0.65f, 0.5f, 0.65f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Fallen metal pole lying across roadway
    glColor3f(0.24f, 0.26f, 0.28f);
    glPushMatrix();
    glTranslatef(0.2f, 0.16f, 0.0f);
    glScalef(4.4f, 0.16f, 0.16f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Smashed lantern head on pavement
    glColor3f(0.18f, 0.20f, 0.22f);
    glPushMatrix();
    glTranslatef(2.5f, 0.22f, 0.0f);
    glScalef(0.55f, 0.35f, 0.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Electrical spark arcs crackling from damaged wiring!
    glDisable(GL_LIGHTING);
    if (fmod(gameElapsed + p.x * 2.0f, 0.8f) < 0.25f) {
        glColor3f(0.4f, 0.95f, 1.0f); // Electric cyan spark
        glLineWidth(2.2f);
        glBegin(GL_LINES);
        glVertex3f(2.5f, 0.22f, 0.0f);
        glVertex3f(2.8f, 0.45f, 0.25f);
        glVertex3f(2.8f, 0.45f, 0.25f);
        glVertex3f(3.0f, 0.15f, 0.4f);
        glVertex3f(2.5f, 0.22f, 0.0f);
        glVertex3f(2.7f, 0.50f, -0.3f);
        glEnd();
        glLineWidth(1.0f);

        // Small glowing spark point
        glColor3f(1.0f, 1.0f, 0.4f);
        glPushMatrix();
        glTranslatef(2.7f, 0.35f, 0.1f);
        glutSolidSphere(0.08, 6, 6);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawRubblePileProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);
    glRotatef(p.rotY, 0.0f, 1.0f, 0.0f);

    // Central masonry block
    glColor3f(0.48f, 0.45f, 0.42f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    glRotatef(18.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.2f, 0.7f, 0.9f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Smaller fragments
    glColor3f(0.42f, 0.38f, 0.35f);
    glPushMatrix();
    glTranslatef(0.6f, 0.2f, 0.4f);
    glRotatef(35.0f, 0.0f, 1.0f, 0.0f);
    glScalef(0.65f, 0.4f, 0.55f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.55f, 0.50f, 0.46f);
    glPushMatrix();
    glTranslatef(-0.55f, 0.18f, -0.35f);
    glRotatef(-22.0f, 0.0f, 1.0f, 0.0f);
    glScalef(0.7f, 0.35f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawStreetTreeProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);

    // Concrete Planter Box
    glColor3f(0.55f, 0.58f, 0.60f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    glScalef(1.4f, 0.44f, 1.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Planter Soil
    glColor3f(0.22f, 0.16f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    glScalef(1.2f, 0.05f, 1.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Wood Trunk
    glColor3f(0.38f, 0.25f, 0.16f);
    glPushMatrix();
    glTranslatef(0.0f, 1.6f, 0.0f);
    glScalef(0.32f, 2.4f, 0.32f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Layered Foliage Canopy
    glColor3f(0.12f, 0.42f, 0.18f);
    glPushMatrix();
    glTranslatef(0.0f, 2.8f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(1.6, 2.4, 8, 3);
    glPopMatrix();

    glColor3f(0.16f, 0.52f, 0.22f);
    glPushMatrix();
    glTranslatef(0.0f, 4.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(1.1, 2.0, 8, 3);
    glPopMatrix();

    glPopMatrix();
}

void drawFireHydrantProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.0f, p.z);

    // Red Cast Iron Body
    glColor3f(0.88f, 0.12f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glScalef(0.35f, 0.8f, 0.35f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Brass Side Hose Nozzles
    glColor3f(0.85f, 0.72f, 0.25f);
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    glScalef(0.55f, 0.18f, 0.18f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Top Bonnet Dome
    glColor3f(0.75f, 0.10f, 0.10f);
    glPushMatrix();
    glTranslatef(0.0f, 0.82f, 0.0f);
    glutSolidSphere(0.18, 8, 8);
    glPopMatrix();

    glPopMatrix();
}

void drawFissureProp(const StreetProp& p) {
    glPushMatrix();
    glTranslatef(p.x, 0.02f, p.z);
    glRotatef(p.rotY, 0.0f, 1.0f, 0.0f);

    glDisable(GL_LIGHTING);
    // Dark Jagged Chasm Base
    glColor3f(0.04f, 0.04f, 0.06f);
    glBegin(GL_POLYGON);
    glVertex3f(-p.width * 0.5f, 0.0f, -0.4f);
    glVertex3f(-p.width * 0.2f, 0.0f, -0.7f);
    glVertex3f( p.width * 0.1f, 0.0f, -0.3f);
    glVertex3f( p.width * 0.5f, 0.0f, -0.6f);
    glVertex3f( p.width * 0.4f, 0.0f,  0.5f);
    glVertex3f(-0.1f,           0.0f,  0.7f);
    glVertex3f(-p.width * 0.3f, 0.0f,  0.4f);
    glEnd();

    // Glowing Molten Core / Seismic Heat interior
    float glow = 0.5f + 0.3f * sin(gameElapsed * 5.0f);
    glColor3f(glow, glow * 0.35f, 0.05f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex3f(-p.width * 0.45f, 0.005f, -0.2f);
    glVertex3f(-p.width * 0.15f, 0.005f, -0.4f);
    glVertex3f(-p.width * 0.15f, 0.005f, -0.4f);
    glVertex3f( 0.05f,           0.005f,  0.2f);
    glVertex3f( 0.05f,           0.005f,  0.2f);
    glVertex3f( p.width * 0.42f, 0.005f, -0.1f);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawStreetProps() {
    for (const auto& p : streetProps) {
        switch (p.type) {
            case PROP_CAR:
            case PROP_POLICE_CAR:
            case PROP_AMBULANCE:
                drawVehicleProp(p);
                break;
            case PROP_BARRICADE:
                drawBarricadeProp(p);
                break;
            case PROP_CONCRETE_BARRIER:
                drawConcreteBarrierProp(p);
                break;
            case PROP_TRAFFIC_CONE:
                drawTrafficConeProp(p);
                break;
            case PROP_HAZARD_DRUM:
                drawHazardDrumProp(p);
                break;
            case PROP_FALLEN_LAMPPOST:
                drawFallenLamppostProp(p);
                break;
            case PROP_RUBBLE_PILE:
                drawRubblePileProp(p);
                break;
            case PROP_TREE:
                drawStreetTreeProp(p);
                break;
            case PROP_FIRE_HYDRANT:
                drawFireHydrantProp(p);
                break;
            case PROP_FISSURE:
                drawFissureProp(p);
                break;
        }
    }
}

void drawCity() {
    // 1. Asphalt Ground
    glColor3f(0.12f, 0.14f, 0.18f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-50.0f, 0.0f, -50.0f);
    glVertex3f(-50.0f, 0.0f,  50.0f);
    glVertex3f( 50.0f, 0.0f,  50.0f);
    glVertex3f( 50.0f, 0.0f, -50.0f);
    glEnd();

    // 2. Double Solid Yellow Road Medians & Dashed White Lane Dividers
    glColor3f(0.95f, 0.78f, 0.12f); // Reflective highway traffic yellow
    for (float pos = -46.0f; pos < 46.0f; pos += 6.0f) {
        if (pos > -13.0f && pos < 13.0f) continue; // Clear central intersection box
        // North-South Avenue Double Center Line
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-0.35f, 0.02f, pos);
        glVertex3f(-0.35f, 0.02f, pos + 3.8f);
        glVertex3f(-0.15f, 0.02f, pos + 3.8f);
        glVertex3f(-0.15f, 0.02f, pos);

        glVertex3f( 0.15f, 0.02f, pos);
        glVertex3f( 0.15f, 0.02f, pos + 3.8f);
        glVertex3f( 0.35f, 0.02f, pos + 3.8f);
        glVertex3f( 0.35f, 0.02f, pos);
        glEnd();

        // East-West Boulevard Double Center Line
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(pos,        0.02f, -0.35f);
        glVertex3f(pos + 3.8f, 0.02f, -0.35f);
        glVertex3f(pos + 3.8f, 0.02f, -0.15f);
        glVertex3f(pos,        0.02f, -0.15f);

        glVertex3f(pos,        0.02f,  0.15f);
        glVertex3f(pos + 3.8f, 0.02f,  0.15f);
        glVertex3f(pos + 3.8f, 0.02f,  0.35f);
        glVertex3f(pos,        0.02f,  0.35f);
        glEnd();
    }

    // 3. Pedestrian Zebra Crosswalks at 4 Central Intersection Approaches
    drawZebraCrosswalk(  0.0f, -11.8f, 13.0f, 3.8f, true);  // North approach
    drawZebraCrosswalk(  0.0f,  11.8f, 13.0f, 3.8f, true);  // South approach
    drawZebraCrosswalk(-11.8f,   0.0f,  3.8f, 13.0f, false); // West approach
    drawZebraCrosswalk( 11.8f,   0.0f,  3.8f, 13.0f, false); // East approach

    // 4. Heavy Cast-Iron Sewer Manhole Covers & Drainage Grates
    drawManholeCover(-4.5f, -14.5f, 0.95f);
    drawManholeCover( 4.5f,  14.5f, 0.95f);
    drawManholeCover( 14.5f, -4.5f, 0.95f);
    drawManholeCover(-14.5f,  4.5f, 0.95f);

    // 5. 3D Raised Concrete Curbs separating roadways and sidewalks
    glColor3f(0.48f, 0.50f, 0.54f);
    for (float s = -48.0f; s < 48.0f; s += 24.0f) {
        if (s > -14.0f && s < 14.0f) continue; // Keep central intersection clear
        glPushMatrix();
        glTranslatef(-8.2f, 0.12f, s + 10.0f);
        glScalef(0.35f, 0.24f, 20.0f);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(8.2f, 0.12f, s + 10.0f);
        glScalef(0.35f, 0.24f, 20.0f);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(s + 10.0f, 0.12f, -8.2f);
        glScalef(20.0f, 0.24f, 0.35f);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(s + 10.0f, 0.12f, 8.2f);
        glScalef(20.0f, 0.24f, 0.35f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // 6. Glowing Green Evacuation Chevrons oriented dynamically towards Safe Zone
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.95f, 0.45f);
    float toSafeX = safeZoneX - playerX;
    float toSafeZ = safeZoneZ - playerZ;
    float distSafe = sqrt(toSafeX * toSafeX + toSafeZ * toSafeZ);
    if (distSafe > 0.01f) {
        float dirX = toSafeX / distSafe;
        float dirZ = toSafeZ / distSafe;
        float normX = -dirZ;
        float normZ = dirX;

        // Draw multiple guiding chevrons along the primary route
        for (float d = 6.0f; d < distSafe - 4.0f; d += 9.0f) {
            float midX = playerX + dirX * d;
            float midZ = playerZ + dirZ * d;
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(midX + dirX * 1.8f, 0.03f, midZ + dirZ * 1.8f);
            glVertex3f(midX - dirX * 0.9f + normX * 1.2f, 0.03f, midZ - dirZ * 0.9f + normZ * 1.2f);
            glVertex3f(midX - dirX * 0.9f - normX * 1.2f, 0.03f, midZ - dirZ * 0.9f - normZ * 1.2f);
            glEnd();
        }
    }
    glEnable(GL_LIGHTING);

    // 7. Detailed Architectural Buildings with Windows, Doors, & Rooftops
    for (const auto& b : buildings) {
        drawDetailedBuilding(b);
    }

    // 8. Street Lampposts lining the Sidewalk Curbs
    float lampPos[] = { -36.0f, -20.0f, 20.0f, 36.0f };
    for (int i = 0; i < 4; ++i) {
        drawStreetLamppost(-11.5f, lampPos[i]);
        drawStreetLamppost( 11.5f, lampPos[i]);
        drawStreetLamppost(lampPos[i], -11.5f);
        drawStreetLamppost(lampPos[i],  11.5f);
    }

    // 9. 3D Obstacles & Objects on the Path (Vehicles, Barricades, Fallen Lampposts, Trees, etc.)
    drawStreetProps();

    // 10. Interactive Survival Pickups along the evacuation routes
    drawSurvivalPickups();

    // 11. Interactive 3D Waypoint Compass Arrow floating above player pointing to Safety
    draw3DWaypointArrow();
}

void drawSurvivalPickups() {
    for (const auto& sp : survivalPickups) {
        if (sp.collected) continue;
        glPushMatrix();
        glTranslatef(sp.x, sp.y + sp.hoverOffset, sp.z);
        glRotatef(sp.rotY, 0.0f, 1.0f, 0.0f);

        if (sp.type == PICKUP_MEDKIT) {
            // White medical case
            glColor3f(0.95f, 0.95f, 0.95f);
            glPushMatrix();
            glScalef(0.9f, 0.65f, 0.5f);
            glutSolidCube(1.0);
            glPopMatrix();

            // Red Cross on Front and Back faces
            glDisable(GL_LIGHTING);
            glColor3f(0.95f, 0.15f, 0.15f);
            for (float side = -0.26f; side <= 0.26f; side += 0.52f) {
                glBegin(GL_QUADS);
                // Vertical bar
                glVertex3f(-0.08f, -0.22f, side);
                glVertex3f( 0.08f, -0.22f, side);
                glVertex3f( 0.08f,  0.22f, side);
                glVertex3f(-0.08f,  0.22f, side);
                // Horizontal bar
                glVertex3f(-0.24f, -0.07f, side);
                glVertex3f( 0.24f, -0.07f, side);
                glVertex3f( 0.24f,  0.07f, side);
                glVertex3f(-0.24f,  0.07f, side);
                glEnd();
            }
            glEnable(GL_LIGHTING);
        } else if (sp.type == PICKUP_ADRENALINE) {
            // High-octane golden amber canister with orange caps
            glColor3f(1.0f, 0.85f, 0.10f);
            glPushMatrix();
            glScalef(0.5f, 0.85f, 0.5f);
            glutSolidCube(1.0);
            glPopMatrix();

            glColor3f(1.0f, 0.45f, 0.05f);
            glPushMatrix();
            glTranslatef(0.0f, 0.48f, 0.0f);
            glScalef(0.3f, 0.25f, 0.3f);
            glutSolidCube(1.0);
            glPopMatrix();
        } else if (sp.type == PICKUP_CHRONO) {
            // Cyan glowing hourglass diamond
            glColor3f(0.15f, 0.92f, 0.98f);
            glPushMatrix();
            glRotatef(45.0f, 1.0f, 0.0f, 1.0f);
            glutSolidCube(0.65);
            glPopMatrix();
        } else if (sp.type == PICKUP_SHIELD) {
            // Electric cobalt blue sphere with outer orbital ring
            glColor3f(0.25f, 0.65f, 1.0f);
            glutSolidSphere(0.46, 16, 16);
            glColor3f(0.65f, 0.85f, 1.0f);
            glPushMatrix();
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidTorus(0.06, 0.60, 8, 20);
            glPopMatrix();
        }

        // Glowing Pulsing Halo beneath pickup
        glDisable(GL_LIGHTING);
        glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
        glBegin(GL_LINE_LOOP);
        for (int a = 0; a < 16; ++a) {
            float ang = a * 2.0f * PI / 16.0f;
            glVertex3f(cos(ang) * 0.72f, -0.55f, sin(ang) * 0.72f);
        }
        glEnd();
        glEnable(GL_LIGHTING);

        glPopMatrix();
    }
}

void draw3DWaypointArrow() {
    float toSafeX = safeZoneX - playerX;
    float toSafeZ = safeZoneZ - playerZ;
    float distSafe = sqrt(toSafeX * toSafeX + toSafeZ * toSafeZ);
    if (distSafe < 0.8f) return;

    float angleYaw = atan2(toSafeX, -toSafeZ) * 180.0f / PI;

    glPushMatrix();
    glTranslatef(playerX, playerY + 2.7f + sin(gameElapsed * 5.0f) * 0.15f, playerZ);
    glRotatef(angleYaw, 0.0f, 1.0f, 0.0f);

    glDisable(GL_LIGHTING);
    // Vibrant neon green floating 3D arrow
    glColor3f(0.15f, 1.0f, 0.45f);
    glBegin(GL_TRIANGLES);
    glVertex3f( 0.0f, 0.1f, -1.1f);
    glVertex3f(-0.5f, 0.1f,  0.4f);
    glVertex3f( 0.5f, 0.1f,  0.4f);

    glVertex3f(-0.18f, 0.1f,  0.4f);
    glVertex3f(-0.18f, 0.1f,  0.9f);
    glVertex3f( 0.18f, 0.1f,  0.9f);

    glVertex3f(-0.18f, 0.1f,  0.4f);
    glVertex3f( 0.18f, 0.1f,  0.9f);
    glVertex3f( 0.18f, 0.1f,  0.4f);
    glEnd();

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawSafeZone() {
    glPushMatrix();
    glTranslatef(safeZoneX, 0.05f, safeZoneZ);

    // Green Ground Pad
    glColor3f(0.1f, 0.75f, 0.35f);
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < 32; ++i) {
        float angle = i * 2.0f * PI / 32.0f;
        glVertex3f(cos(angle) * safeZoneRadius, 0.0f, sin(angle) * safeZoneRadius);
    }
    glEnd();

    // 75m Tall Glowing Skyward Light Beacon Column
    glDisable(GL_LIGHTING);
    glColor4f(0.2f, 0.98f, 0.45f, 0.35f);
    glBegin(GL_QUADS);
    for (int i = 0; i < 8; ++i) {
        float a1 = i * 2.0f * PI / 8.0f;
        float a2 = (i + 1) * 2.0f * PI / 8.0f;
        float r = 0.9f;
        glVertex3f(cos(a1) * r, 0.0f, sin(a1) * r);
        glVertex3f(cos(a2) * r, 0.0f, sin(a2) * r);
        glVertex3f(cos(a2) * r * 1.5f, 75.0f, sin(a2) * r * 1.5f);
        glVertex3f(cos(a1) * r * 1.5f, 75.0f, sin(a1) * r * 1.5f);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // Holographic Pulsing Rings (Computer Graphics Scaling)
    glDisable(GL_LIGHTING); // Disabled so wireframe glows brightly
    glColor3f(0.2f, 0.95f, 0.45f);
    for (int ring = 1; ring <= 3; ++ring) {
        glPushMatrix();
        glTranslatef(0.0f, ring * 3.8f, 0.0f);
        glScalef(beaconPulse, 1.0f, beaconPulse);
        glutWireTorus(0.12, safeZoneRadius * 0.72f, 8, 24);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawDebris() {
    for (const auto& d : debrisList) {
        glPushMatrix();
        glTranslatef(d.x, d.y, d.z);
        glRotatef(d.rotX, 1.0f, 0.0f, 0.0f);
        glRotatef(d.rotY, 0.0f, 1.0f, 0.0f);
        glColor3f(0.52f, 0.48f, 0.45f);
        glutSolidCube(d.size);
        glPopMatrix();
    }
}

// --------------------------------------------------------------------------
// 3D Player Character Model (Fully Rendered & Animated for Gamers)
// --------------------------------------------------------------------------
void drawPlayerAvatar() {
    glPushMatrix();
    glTranslatef(playerX, playerY, playerZ);
    glRotatef(-playerYaw, 0.0f, 1.0f, 0.0f);

    float legSwing = isMoving ? sin(walkAnimTime) * 28.0f : 0.0f;
    float armSwing = isMoving ? sin(walkAnimTime) * 25.0f : 0.0f;
    if (!isGrounded) {
        legSwing = -18.0f; // Airborne jump pose
        armSwing = 35.0f;
    }

    // 1. Torso: High-Visibility Emergency Jacket
    glColor3f(1.0f, 0.48f, 0.12f); // Vibrant Rescue Orange
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(0.65f, 0.75f, 0.35f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Reflective Silver Safety Stripe
    glColor3f(0.92f, 0.94f, 0.98f);
    glPushMatrix();
    glTranslatef(0.0f, 0.04f, 0.01f);
    glScalef(0.66f, 0.12f, 0.36f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 2. Emergency Survival Backpack
    glColor3f(0.18f, 0.22f, 0.32f); // Dark tactical navy
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.24f);
    glScalef(0.45f, 0.55f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 3. Head (Skin Tone)
    glColor3f(0.96f, 0.78f, 0.65f);
    glPushMatrix();
    glTranslatef(0.0f, 0.58f, 0.0f);
    glutSolidSphere(0.20, 12, 12);
    glPopMatrix();

    // 4. Hard Safety Helmet
    glColor3f(0.98f, 0.85f, 0.15f); // High-Vis Construction Yellow
    glPushMatrix();
    glTranslatef(0.0f, 0.70f, 0.0f);
    glutSolidSphere(0.24, 14, 14);
    glPopMatrix();

    // Helmet Headlamp Beacon
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 1.0f, 1.0f); // Glowing cyan headlamp
    glPushMatrix();
    glTranslatef(0.0f, 0.68f, -0.22f);
    glScalef(0.12f, 0.08f, 0.08f);
    glutSolidCube(1.0);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    // 5. Left Leg (Pants + Boots)
    glColor3f(0.15f, 0.20f, 0.32f); // Tactical pants
    glPushMatrix();
    glTranslatef(-0.18f, -0.38f, 0.0f);
    glRotatef(legSwing, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.28f, 0.0f);
    glScalef(0.22f, 0.58f, 0.22f);
    glutSolidCube(1.0);
    // Boot
    glTranslatef(0.0f, -0.52f, -0.12f);
    glColor3f(0.1f, 0.1f, 0.12f);
    glScalef(1.05f, 0.35f, 1.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 6. Right Leg (Pants + Boots)
    glColor3f(0.15f, 0.20f, 0.32f);
    glPushMatrix();
    glTranslatef(0.18f, -0.38f, 0.0f);
    glRotatef(-legSwing, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.28f, 0.0f);
    glScalef(0.22f, 0.58f, 0.22f);
    glutSolidCube(1.0);
    // Boot
    glTranslatef(0.0f, -0.52f, -0.12f);
    glColor3f(0.1f, 0.1f, 0.12f);
    glScalef(1.05f, 0.35f, 1.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 7. Left Arm (Swinging + Gloves)
    glColor3f(1.0f, 0.48f, 0.12f);
    glPushMatrix();
    glTranslatef(-0.44f, 0.22f, 0.0f);
    glRotatef(-armSwing, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.24f, 0.0f);
    glScalef(0.18f, 0.52f, 0.18f);
    glutSolidCube(1.0);
    // Glove
    glTranslatef(0.0f, -0.52f, 0.0f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glutSolidCube(1.1);
    glPopMatrix();

    // 8. Right Arm (Swinging + Gloves)
    glColor3f(1.0f, 0.48f, 0.12f);
    glPushMatrix();
    glTranslatef(0.44f, 0.22f, 0.0f);
    glRotatef(armSwing, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.24f, 0.0f);
    glScalef(0.18f, 0.52f, 0.18f);
    glutSolidCube(1.0);
    // Glove
    glTranslatef(0.0f, -0.52f, 0.0f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glutSolidCube(1.1);
    glPopMatrix();

    glPopMatrix();
}

// --------------------------------------------------------------------------
// 2D Card Drawer Helper
// --------------------------------------------------------------------------
void drawCard(float x, float y, float w, float h, float r, float g, float b, float a, bool highlighted) {
    if (highlighted) {
        glColor4f(0.2f, 0.95f, 1.0f, 0.95f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x - 2, y - 2);
        glVertex2f(x + w + 2, y - 2);
        glVertex2f(x + w + 2, y + h + 2);
        glVertex2f(x - 2, y + h + 2);
        glEnd();
        glLineWidth(1.0f);
    }

    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

// --------------------------------------------------------------------------
// 2D Orthographic HUD & Menus
// --------------------------------------------------------------------------
void drawHUD() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG); // Ensure 2D HUD text and graphics are never obscured by fog

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (gameState == STATE_INTENSITY_SELECT) {
        drawIntensityMenu();
    } else if (gameState == STATE_LEVEL_SELECT) {
        drawLevelSelectMenu();
    } else if (gameState == STATE_LEVEL_CLEAR) {
        drawLevelClearScreen();
    } else if (gameState == STATE_VICTORY) {
        drawVictoryScreen();
    } else if (gameState == STATE_GAMEOVER) {
        drawGameOverScreen();
    } else {
        // In-Game Active HUD
        // 0. Cinematic Post-Processing Screen-Space Vignette
        drawScreenVignette();

        // 1. Top Header Bar
        glColor4f(0.05f, 0.08f, 0.15f, 0.85f);
        glBegin(GL_QUADS);
        glVertex2f(0, windowHeight - 65);
        glVertex2f(windowWidth, windowHeight - 65);
        glVertex2f(windowWidth, windowHeight);
        glVertex2f(0, windowHeight);
        glEnd();

        // Level & Title
        std::stringstream titleSS;
        titleSS << "EARTHQUAKE ESCAPE | LVL " << level << "/" << MAX_LEVELS << ": " << getLevelName(level);
        glColor3f(0.2f, 0.85f, 1.0f);
        renderBitmapString(20, windowHeight - 26, GLUT_BITMAP_HELVETICA_18, titleSS.str().c_str());

        // Intensity / Magnitude Badge
        std::stringstream magSS;
        magSS << "INTENSITY: " << getIntensityName(selectedIntensity);
        if (selectedIntensity == INTENSITY_LOW)          glColor3f(0.3f, 0.9f, 0.4f);
        else if (selectedIntensity == INTENSITY_MEDIUM) glColor3f(1.0f, 0.85f, 0.2f);
        else                                            glColor3f(1.0f, 0.5f, 0.1f);
        renderBitmapString(20, windowHeight - 48, GLUT_BITMAP_HELVETICA_12, magSS.str().c_str());

        // Score & Timer
        std::stringstream scoreSS;
        scoreSS << "SCORE: " << currentScore << "  |  HIGH: " << highScore;
        glColor3f(1.0f, 0.95f, 0.3f);
        renderBitmapString(windowWidth / 2 - 80, windowHeight - 26, GLUT_BITMAP_HELVETICA_18, scoreSS.str().c_str());

        std::stringstream timerSS;
        timerSS << "TIME LEFT: " << std::fixed << std::setprecision(1) << (timer > 0 ? timer : 0.0f) << "s";
        if (timer <= 10.0f) glColor3f(1.0f, 0.25f, 0.25f);
        else glColor3f(0.25f, 1.0f, 0.4f);
        renderBitmapString(windowWidth - 210, windowHeight - 26, GLUT_BITMAP_HELVETICA_18, timerSS.str().c_str());

        // Earthquake Stage Status Warning
        if (gameState == STATE_EXPLORE) {
            glColor3f(0.4f, 0.8f, 1.0f);
            renderBitmapString(windowWidth / 2 - 80, windowHeight - 48, GLUT_BITMAP_HELVETICA_12, "[Status: Exploring City - Find Escape Route]");
        } else if (gameState == STATE_WARNING) {
            glColor3f(1.0f, 0.8f, 0.2f);
            renderBitmapString(windowWidth / 2 - 95, windowHeight - 48, GLUT_BITMAP_HELVETICA_12, "[Status: WARNING! Pre-Shock Tremors Detected!]");
        } else if (gameState == STATE_EARTHQUAKE) {
            glColor3f(1.0f, 0.25f, 0.25f);
            renderBitmapString(windowWidth / 2 - 110, windowHeight - 48, GLUT_BITMAP_HELVETICA_12, "[Status: SEISMIC QUAKE! ESCAPE TO GREEN ZONE!]");
        }

        // Distance & View Hint
        float dist = sqrt((playerX - safeZoneX)*(playerX - safeZoneX) + (playerZ - safeZoneZ)*(playerZ - safeZoneZ));
        std::stringstream distSS;
        distSS << "Target: " << getTargetName(level) << " (" << (int)dist << "m) | View: " << (isFirstPerson ? "1st Person [V]" : "3rd Person [V]")
               << " | Dodged: " << debrisDodged << " | Menu: [M]";
        glColor3f(0.85f, 0.88f, 0.92f);
        renderBitmapString(20, 20, GLUT_BITMAP_HELVETICA_12, distSS.str().c_str());

        // Controls hint
        renderBitmapString(windowWidth - 560, 20, GLUT_BITMAP_HELVETICA_12, "Controls: W/S move | LEFT/RIGHT/Q/E steer | [F] SPRINT | Space jump | V view");

        // 2. Floating Waypoint Compass Pill (Top Center)
        float pillW = 380.0f;
        float pillH = 32.0f;
        float pillX = (windowWidth - pillW) / 2.0f;
        float pillY = windowHeight - 95.0f;
        drawCard(pillX, pillY, pillW, pillH, 0.05f, 0.10f, 0.16f, 0.90f, (dist <= 15.0f));

        // Waypoint relative angle calculation
        float dx = safeZoneX - playerX;
        float dz = safeZoneZ - playerZ;
        float targetRad = atan2(-dx, -dz);
        float pRad = playerYaw * PI / 180.0f;
        float diff = targetRad - pRad;
        while (diff > PI) diff -= 2.0f * PI;
        while (diff < -PI) diff += 2.0f * PI;
        float deg = diff * 180.0f / PI;

        std::string arrow = " [ ⬆ AHEAD ]";
        if (fabs(deg) < 22.5f) arrow = " [ ⬆ AHEAD ]";
        else if (deg >= 22.5f && deg < 67.5f) arrow = " [ ↖ AHEAD-LEFT ]";
        else if (deg >= 67.5f && deg < 112.5f) arrow = " [ ⬅ TURN LEFT ]";
        else if (deg >= 112.5f && deg < 157.5f) arrow = " [ ↙ BEHIND-LEFT ]";
        else if (deg <= -22.5f && deg > -67.5f) arrow = " [ ↗ AHEAD-RIGHT ]";
        else if (deg <= -67.5f && deg > -112.5f) arrow = " [ ➡ TURN RIGHT ]";
        else if (deg <= -112.5f && deg > -157.5f) arrow = " [ ↘ BEHIND-RIGHT ]";
        else arrow = " [ ⬇ BEHIND ]";

        std::stringstream wpSS;
        wpSS << getTargetName(level) << ": " << (int)dist << "m " << arrow;
        if (dist <= 15.0f) glColor3f(0.3f, 1.0f, 0.5f);
        else glColor3f(0.2f, 0.95f, 0.55f);
        renderBitmapString(pillX + 16, pillY + 10, GLUT_BITMAP_HELVETICA_12, wpSS.str().c_str());

        // 3. Circular Radar Minimap (Top Right)
        float rX = windowWidth - 75.0f;
        float rY = windowHeight - 115.0f;
        float rRad = 45.0f;

        // Radar background circle
        glColor4f(0.06f, 0.09f, 0.16f, 0.92f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 32; ++i) {
            float ang = i * 2.0f * PI / 32.0f;
            glVertex2f(rX + cos(ang) * rRad, rY + sin(ang) * rRad);
        }
        glEnd();

        // Cyan Border Ring
        glColor3f(0.2f, 0.75f, 0.95f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 32; ++i) {
            float ang = i * 2.0f * PI / 32.0f;
            glVertex2f(rX + cos(ang) * rRad, rY + sin(ang) * rRad);
        }
        glEnd();
        glLineWidth(1.0f);

        // Concentric Distance Range Rings (25m and 50m)
        glColor4f(0.2f, 0.75f, 0.95f, 0.25f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 24; ++i) {
            float ang = i * 2.0f * PI / 24.0f;
            glVertex2f(rX + cos(ang) * (rRad * 0.45f), rY + sin(ang) * (rRad * 0.45f));
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 24; ++i) {
            float ang = i * 2.0f * PI / 24.0f;
            glVertex2f(rX + cos(ang) * (rRad * 0.75f), rY + sin(ang) * (rRad * 0.75f));
        }
        glEnd();

        // Dynamic 360 Rotating Radar Sweep Beam
        float sweepRad = radarSweepAngle * PI / 180.0f;
        glColor4f(0.35f, 0.95f, 1.0f, 0.55f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        glVertex2f(rX, rY);
        glVertex2f(rX + cos(sweepRad) * (rRad - 2.0f), rY + sin(sweepRad) * (rRad - 2.0f));
        glEnd();
        glLineWidth(1.0f);

        // Cardinal Ticks N, S, W, E
        glColor3f(0.6f, 0.7f, 0.8f);
        renderBitmapString(rX - 4, rY + rRad - 11, GLUT_BITMAP_HELVETICA_10, "N");
        renderBitmapString(rX - 4, rY - rRad + 3, GLUT_BITMAP_HELVETICA_10, "S");
        renderBitmapString(rX - rRad + 3, rY - 4, GLUT_BITMAP_HELVETICA_10, "W");
        renderBitmapString(rX + rRad - 10, rY - 4, GLUT_BITMAP_HELVETICA_10, "E");

        // Radar Range Scale (80m radius)
        float radarScale = rRad / 80.0f;

        // Safe zone waypoint dot on radar
        float sRelX = (safeZoneX - playerX) * radarScale;
        float sRelZ = (safeZoneZ - playerZ) * radarScale;
        float sDistFromC = sqrt(sRelX * sRelX + sRelZ * sRelZ);
        float sDrawX = rX + sRelX;
        float sDrawY = rY - sRelZ; // OpenGL Y increases upwards, in 3D North is -Z
        if (sDistFromC > (rRad - 6.0f)) {
            float ang = atan2(-sRelZ, sRelX);
            sDrawX = rX + cos(ang) * (rRad - 6.0f);
            sDrawY = rY + sin(ang) * (rRad - 6.0f);
        }

        // Guiding green dashed line from player to safe zone
        glColor4f(0.2f, 0.95f, 0.45f, 0.45f);
        glBegin(GL_LINES);
        glVertex2f(rX, rY);
        glVertex2f(sDrawX, sDrawY);
        glEnd();

        // Safe Zone Glowing Dot
        glColor3f(0.2f, 1.0f, 0.45f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 16; ++i) {
            float ang = i * 2.0f * PI / 16.0f;
            glVertex2f(sDrawX + cos(ang) * 4.5f, sDrawY + sin(ang) * 4.5f);
        }
        glEnd();

        // Player Avatar Arrow in Center of Radar
        glPushMatrix();
        glTranslatef(rX, rY, 0.0f);
        glRotatef(playerYaw - 180.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(0.25f, 0.90f, 1.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, -6.0f);
        glVertex2f(-4.0f, 5.0f);
        glVertex2f(4.0f, 5.0f);
        glEnd();
        glPopMatrix();
    }

    // 4. Interactive Sprint & Stamina Bar (Bottom Left)
    if (gameState == STATE_EXPLORE || gameState == STATE_WARNING || gameState == STATE_EARTHQUAKE) {
        float stamBarX = 20.0f;
        float stamBarY = 46.0f;
        float stamBarW = 220.0f;
        float stamBarH = 20.0f;

        // Background box
        drawCard(stamBarX, stamBarY, stamBarW, stamBarH, 0.06f, 0.10f, 0.16f, 0.88f, isSprinting);

        // Fill bar based on stamina ratio
        float stamRatio = std::max(0.0f, std::min(1.0f, stamina / maxStamina));
        if (isSprinting) {
            glColor3f(1.0f, 0.75f, 0.15f); // Amber sprint drain
        } else if (stamRatio > 0.35f) {
            glColor3f(0.25f, 0.95f, 0.45f); // Vibrant green
        } else {
            glColor3f(0.95f, 0.35f, 0.25f); // Low stamina orange/red
        }
        glBegin(GL_QUADS);
        glVertex2f(stamBarX + 2.0f, stamBarY + 2.0f);
        glVertex2f(stamBarX + 2.0f + (stamBarW - 4.0f) * stamRatio, stamBarY + 2.0f);
        glVertex2f(stamBarX + 2.0f + (stamBarW - 4.0f) * stamRatio, stamBarY + stamBarH - 2.0f);
        glVertex2f(stamBarX + 2.0f, stamBarY + stamBarH - 2.0f);
        glEnd();

        std::stringstream stamSS;
        if (speedBoostTimer > 0.0f) stamSS << "⚡ ADRENALINE SPRINT: " << (int)stamina << "%";
        else if (isSprinting) stamSS << "🏃 SPRINTING [F]: " << (int)stamina << "%";
        else stamSS << "STAMINA [Hold F to Sprint]: " << (int)stamina << "%";
        glColor3f(1.0f, 1.0f, 1.0f);
        renderBitmapString(stamBarX + 6.0f, stamBarY + 5.0f, GLUT_BITMAP_HELVETICA_10, stamSS.str().c_str());

        // 5. Active Powerup Badges (Top Center below compass)
        float pillW = 380.0f;
        float pillX = (windowWidth - pillW) / 2.0f;
        float buffX = pillX;
        float buffY = windowHeight - 122.0f;

        if (speedBoostTimer > 0.0f) {
            std::stringstream boostSS;
            boostSS << "⚡ SPEED SURGE (+45% SPEED): " << std::fixed << std::setprecision(1) << speedBoostTimer << "s";
            drawCard(buffX, buffY, pillW, 20.0f, 0.24f, 0.18f, 0.04f, 0.90f, true);
            glColor3f(1.0f, 0.85f, 0.15f);
            renderBitmapString(buffX + 12.0f, buffY + 5.0f, GLUT_BITMAP_HELVETICA_10, boostSS.str().c_str());
            buffY -= 24.0f;
        }
        if (shieldTimer > 0.0f) {
            std::stringstream shieldSS;
            shieldSS << "🛡 HAZARD SHIELD (DEBRIS IMMUNITY): " << std::fixed << std::setprecision(1) << shieldTimer << "s";
            drawCard(buffX, buffY, pillW, 20.0f, 0.04f, 0.16f, 0.26f, 0.90f, true);
            glColor3f(0.35f, 0.85f, 1.0f);
            renderBitmapString(buffX + 12.0f, buffY + 5.0f, GLUT_BITMAP_HELVETICA_10, shieldSS.str().c_str());
            buffY -= 24.0f;
        }

        // 6. Real-Time Interactive Alert Notification Banner (Mid-Top Popup)
        if (hudAlertTimer > 0.0f && !hudAlertText.empty()) {
            float alertW = 480.0f;
            float alertH = 34.0f;
            float alertX = (windowWidth - alertW) / 2.0f;
            float alertY = windowHeight - 170.0f;
            float alertAlpha = std::min(1.0f, hudAlertTimer * 1.5f);
            drawCard(alertX, alertY, alertW, alertH, 0.08f, 0.18f, 0.28f, 0.94f * alertAlpha, true);
            glColor4f(1.0f, 0.95f, 0.35f, alertAlpha);
            renderBitmapString(alertX + 18.0f, alertY + 11.0f, GLUT_BITMAP_HELVETICA_12, hudAlertText.c_str());
        }
    }

    // Restore matrices and states
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
}

// --------------------------------------------------------------------------
// Start Tab: Intensity Selection Menu (Step 1)
// --------------------------------------------------------------------------
void drawIntensityMenu() {
    // Frosted dark background overlay
    glColor4f(0.04f, 0.06f, 0.12f, 0.94f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Title Banner
    glColor3f(0.2f, 0.9f, 1.0f);
    renderBitmapString(windowWidth / 2 - 240, windowHeight - 75, GLUT_BITMAP_HELVETICA_18, "EARTHQUAKE ESCAPE: 3D SEISMIC SURVIVAL");

    glColor3f(0.9f, 0.9f, 0.95f);
    renderBitmapString(windowWidth / 2 - 225, windowHeight - 110, GLUT_BITMAP_HELVETICA_18, "STEP 1: SELECT SEISMIC INTENSITY & DIFFICULTY");

    glColor3f(0.6f, 0.7f, 0.85f);
    renderBitmapString(windowWidth / 2 - 205, windowHeight - 140, GLUT_BITMAP_HELVETICA_12, "Choose intensity first, then proceed to select your mission level:");

    // 3 Intensity Option Cards (Centered Vertically and Horizontally)
    float cardW = 660.0f;
    float cardH = 82.0f;
    float gapY = 24.0f;
    float cardX = (windowWidth - cardW) / 2.0f;
    float startY = windowHeight / 2.0f + 65.0f;

    // Card 1: Low (Mag 5.5)
    bool isC1 = (selectedIntensity == INTENSITY_LOW);
    drawCard(cardX, startY, cardW, cardH, 0.08f, 0.16f, 0.22f, 0.9f, isC1);
    glColor3f(0.35f, 0.95f, 0.45f);
    renderBitmapString(cardX + 28, startY + 48, GLUT_BITMAP_HELVETICA_18, "[1] LOW INTENSITY (Magnitude 5.5)");
    glColor3f(0.8f, 0.85f, 0.9f);
    renderBitmapString(cardX + 28, startY + 20, GLUT_BITMAP_HELVETICA_12, "Mild tremors, longer escape countdown, light debris. (Score Multiplier: 1.0x)");

    // Card 2: Medium (Mag 7.0)
    bool isC2 = (selectedIntensity == INTENSITY_MEDIUM);
    float y2 = startY - (cardH + gapY);
    drawCard(cardX, y2, cardW, cardH, 0.12f, 0.16f, 0.26f, 0.9f, isC2);
    glColor3f(1.0f, 0.88f, 0.25f);
    renderBitmapString(cardX + 28, y2 + 48, GLUT_BITMAP_HELVETICA_18, "[2] MEDIUM INTENSITY (Magnitude 7.0)  *RECOMMENDED*");
    glColor3f(0.8f, 0.85f, 0.9f);
    renderBitmapString(cardX + 28, y2 + 20, GLUT_BITMAP_HELVETICA_12, "Strong quakes, active building sway, standard debris. (Score Multiplier: 1.5x)");

    // Card 3: High (Mag 8.2)
    bool isC3 = (selectedIntensity == INTENSITY_HIGH);
    float y3 = startY - 2.0f * (cardH + gapY);
    drawCard(cardX, y3, cardW, cardH, 0.18f, 0.14f, 0.24f, 0.9f, isC3);
    glColor3f(1.0f, 0.55f, 0.2f);
    renderBitmapString(cardX + 28, y3 + 48, GLUT_BITMAP_HELVETICA_18, "[3] HIGH INTENSITY (Magnitude 8.2)");
    glColor3f(0.8f, 0.85f, 0.9f);
    renderBitmapString(cardX + 28, y3 + 20, GLUT_BITMAP_HELVETICA_12, "Severe ground resonance, rapid countdown, aggressive debris. (Score Multiplier: 2.2x)");

    // Navigation and Start Prompt
    glColor3f(0.2f, 0.95f, 0.5f);
    renderBitmapString(windowWidth / 2 - 240, 75, GLUT_BITMAP_HELVETICA_18, "PRESS [1, 2, 3] OR CLICK A CARD TO PROCEED TO STEP 2");

    std::stringstream promptSS;
    promptSS << "Press [UP/DOWN] to Cycle | [ENTER] for Step 2: Level Selection | High Score: " << highScore << " pts";
    glColor3f(0.7f, 0.75f, 0.85f);
    renderBitmapString(windowWidth / 2 - 260, 42, GLUT_BITMAP_HELVETICA_12, promptSS.str().c_str());
}

// --------------------------------------------------------------------------
// Level Selection Menu (Step 2: Unlocked One-by-One)
// --------------------------------------------------------------------------
void drawLevelSelectMenu() {
    // Frosted dark background overlay
    glColor4f(0.04f, 0.06f, 0.12f, 0.94f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Ensure valid page
    if (levelSelectPage < 0 || levelSelectPage >= TOTAL_PAGES) {
        levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
        if (levelSelectPage < 0) levelSelectPage = 0;
        if (levelSelectPage >= TOTAL_PAGES) levelSelectPage = TOTAL_PAGES - 1;
    }

    // Title Banner
    glColor3f(0.2f, 0.9f, 1.0f);
    renderBitmapString(windowWidth / 2 - 250, windowHeight - 40, GLUT_BITMAP_HELVETICA_18, "EARTHQUAKE ESCAPE: 3D SEISMIC SURVIVAL (100 LEVELS)");

    std::stringstream subSS;
    subSS << "STEP 2: SELECT MISSION LEVEL  |  TIER " << (levelSelectPage + 1) << " OF " << TOTAL_PAGES 
          << " (LEVELS " << (levelSelectPage * LEVELS_PER_PAGE + 1) << " - " << ((levelSelectPage + 1) * LEVELS_PER_PAGE) << ")";
    glColor3f(0.9f, 0.9f, 0.95f);
    renderBitmapString(windowWidth / 2 - 275, windowHeight - 66, GLUT_BITMAP_HELVETICA_18, subSS.str().c_str());

    std::stringstream progSS;
    progSS << "Chosen Intensity: " << getIntensityName(selectedIntensity)
           << "  |  Progression: Level " << highestUnlockedLevel << " / " << MAX_LEVELS << " Unlocked";
    glColor3f(0.35f, 0.95f, 0.55f);
    renderBitmapString(windowWidth / 2 - 180, windowHeight - 90, GLUT_BITMAP_HELVETICA_12, progSS.str().c_str());

    // Tier Pagination Bar
    const float marginX = 20.0f;
    const float barY = windowHeight - 126.0f;
    const float barH = 26.0f;
    const float navBtnW = 125.0f;

    // Prev Tier Button
    drawCard(marginX, barY, navBtnW, barH, 0.10f, 0.18f, 0.28f, 0.9f, false);
    glColor3f(0.3f, 0.85f, 1.0f);
    renderBitmapString(marginX + 10, barY + 7, GLUT_BITMAP_HELVETICA_12, "[P] ◀ PREV TIER");

    // Next Tier Button
    drawCard(windowWidth - marginX - navBtnW, barY, navBtnW, barH, 0.10f, 0.18f, 0.28f, 0.9f, false);
    glColor3f(0.3f, 0.85f, 1.0f);
    renderBitmapString(windowWidth - marginX - navBtnW + 10, barY + 7, GLUT_BITMAP_HELVETICA_12, "NEXT TIER ▶ [N]");

    // 5 Tier Tab Chips (Tiers 1 to 5)
    const float tabW = 126.0f;
    const float tabGap = 8.0f;
    const float tabsStartX = (windowWidth - (5 * tabW + 4 * tabGap)) / 2.0f;

    for (int p = 0; p < TOTAL_PAGES; ++p) {
        float tx = tabsStartX + p * (tabW + tabGap);
        bool isCurrentPage = (p == levelSelectPage);
        int pageFirstLvl = p * LEVELS_PER_PAGE + 1;
        int pageLastLvl = (p + 1) * LEVELS_PER_PAGE;
        bool isTierUnlocked = (pageFirstLvl <= highestUnlockedLevel);

        if (isCurrentPage) {
            drawCard(tx, barY, tabW, barH, 0.15f, 0.38f, 0.25f, 0.95f, true);
            glColor3f(0.25f, 1.0f, 0.5f);
        } else if (isTierUnlocked) {
            drawCard(tx, barY, tabW, barH, 0.08f, 0.14f, 0.22f, 0.85f, false);
            glColor3f(0.75f, 0.85f, 0.95f);
        } else {
            drawCard(tx, barY, tabW, barH, 0.05f, 0.07f, 0.10f, 0.6f, false);
            glColor3f(0.45f, 0.48f, 0.55f);
        }

        std::stringstream tabSS;
        tabSS << "T" << (p + 1) << " (" << pageFirstLvl << "-" << pageLastLvl << ")";
        if (!isTierUnlocked) tabSS << " [🔒]";
        renderBitmapString(tx + 12, barY + 7, GLUT_BITMAP_HELVETICA_12, tabSS.str().c_str());
    }

    // 20 Level Option Cards for the active Tier page (4 Cols x 5 Rows)
    const int cols = 4;
    const float gapX = 14.0f;
    const float cardW = (windowWidth - 2.0f * marginX - (cols - 1) * gapX) / cols;
    const float cardH = 76.0f;
    const float gapY = 10.0f;
    const float topOfGrid = windowHeight - 138.0f;
    const float startY = topOfGrid - cardH;

    for (int k = 0; k < LEVELS_PER_PAGE; ++k) {
        int i = levelSelectPage * LEVELS_PER_PAGE + k + 1;
        int row = k / cols;
        int col = k % cols;
        float cx = marginX + col * (cardW + gapX);
        float cy = startY - row * (cardH + gapY);

        bool isSelected = (level == i);
        bool isUnlocked = (i <= highestUnlockedLevel);
        bool isCleared = (i < highestUnlockedLevel);

        // Shortcut key character for this slot on the active page
        char keyChar;
        if (k >= 0 && k <= 8) keyChar = '1' + k;
        else if (k == 9) keyChar = '0';
        else keyChar = 'A' + (k - 10);

        if (isUnlocked) {
            drawCard(cx, cy, cardW, cardH, 0.08f, 0.16f, 0.24f, 0.9f, isSelected);

            if (isSelected) glColor3f(0.2f, 0.95f, 0.5f);
            else if (isCleared) glColor3f(0.35f, 0.85f, 1.0f);
            else glColor3f(0.9f, 0.95f, 1.0f);

            std::stringstream lvlHeaderSS;
            lvlHeaderSS << "[" << keyChar << "] LVL " << i << ": " << getLevelName(i);
            renderBitmapString(cx + 10, cy + 54, GLUT_BITMAP_HELVETICA_12, lvlHeaderSS.str().c_str());

            if (isCleared) {
                glColor3f(0.2f, 0.95f, 0.45f);
                renderBitmapString(cx + cardW - 74, cy + 54, GLUT_BITMAP_HELVETICA_10, "[CLEARED]");
            } else if (isSelected) {
                glColor3f(0.2f, 0.95f, 0.45f);
                renderBitmapString(cx + cardW - 68, cy + 54, GLUT_BITMAP_HELVETICA_10, "[READY]");
            } else {
                glColor3f(0.4f, 0.8f, 1.0f);
                renderBitmapString(cx + cardW - 74, cy + 54, GLUT_BITMAP_HELVETICA_10, "[UNLOCKED]");
            }

            glColor3f(0.75f, 0.82f, 0.9f);
            std::stringstream targetSS;
            targetSS << "Target: " << getTargetName(i);
            renderBitmapString(cx + 10, cy + 32, GLUT_BITMAP_HELVETICA_10, targetSS.str().c_str());

            glColor3f(0.60f, 0.68f, 0.78f);
            std::stringstream routeSS;
            routeSS << getRouteName(i);
            renderBitmapString(cx + 10, cy + 12, GLUT_BITMAP_HELVETICA_10, routeSS.str().c_str());
        } else {
            // Locked Card
            drawCard(cx, cy, cardW, cardH, 0.05f, 0.06f, 0.09f, 0.6f, false);
            glColor3f(0.50f, 0.55f, 0.62f);
            std::stringstream lockedSS;
            lockedSS << "[" << keyChar << "] LVL " << i << ": LOCKED [🔒]";
            renderBitmapString(cx + 10, cy + 46, GLUT_BITMAP_HELVETICA_12, lockedSS.str().c_str());

            glColor3f(0.40f, 0.44f, 0.50f);
            std::stringstream needSS;
            if (i == 21) {
                needSS << "Complete all 20 Tier 1 levels to unlock Tier 2";
            } else {
                needSS << "Clear Level " << (i - 1) << " to unlock";
            }
            renderBitmapString(cx + 10, cy + 20, GLUT_BITMAP_HELVETICA_10, needSS.str().c_str());
        }
    }

    // Navigation and Start Prompt
    glColor3f(0.2f, 0.95f, 0.5f);
    renderBitmapString(windowWidth / 2 - 270, 130, GLUT_BITMAP_HELVETICA_18, "PRESS [1..9, 0, A..J] OR CLICK CARD TO START MISSION");

    glColor3f(0.35f, 0.85f, 1.0f);
    renderBitmapString(windowWidth / 2 - 275, 95, GLUT_BITMAP_HELVETICA_12, "[P / N] or [PgUp / PgDn] Switch Tiers (20 Levels each) | [Arrows] Navigate Grid");

    glColor3f(0.7f, 0.75f, 0.85f);
    renderBitmapString(windowWidth / 2 - 250, 65, GLUT_BITMAP_HELVETICA_12, "[ENTER / SPACE] Launch Selected Level | [M] Intensity Presets Menu");
}

// --------------------------------------------------------------------------
// Level Cleared Screen (Mission Successful)
// --------------------------------------------------------------------------
void drawLevelClearScreen() {
    glColor4f(0.04f, 0.08f, 0.14f, 0.90f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    float cardW = 580.0f;
    float cardH = 370.0f;
    float cardX = (windowWidth - cardW) / 2.0f;
    float cardY = (windowHeight - cardH) / 2.0f;
    drawCard(cardX, cardY, cardW, cardH, 0.08f, 0.14f, 0.24f, 0.95f, true);

    // Mission Successful Header Bar
    glColor3f(0.25f, 1.0f, 0.45f);
    renderBitmapString(cardX + 160, cardY + cardH - 40, GLUT_BITMAP_HELVETICA_18, "🎉 MISSION SUCCESSFUL! 🎉");

    std::stringstream lvlSS;
    lvlSS << "Safe Zone Reached: " << getTargetName(level) << "! Level " << level << " (" << getLevelName(level) << ") Cleared";
    glColor3f(0.9f, 0.95f, 1.0f);
    renderBitmapString(cardX + 45, cardY + cardH - 72, GLUT_BITMAP_HELVETICA_18, lvlSS.str().c_str());

    // Unlocked Next Level Announcement
    if (level < MAX_LEVELS) {
        std::stringstream unlockSS;
        if (level == 20) {
            unlockSS << "✨ TIER 1 COMPLETE! TIER 2 ACTIVE (LVL 21 UNLOCKED) -> " << getTargetName(21) << " ✨";
        } else {
            unlockSS << "✨ LEVEL " << (level + 1) << " UNLOCKED -> Next Target: " << getTargetName(level + 1) << " ✨";
        }
        glColor3f(0.2f, 0.95f, 0.55f);
        renderBitmapString(cardX + 30, cardY + cardH - 100, GLUT_BITMAP_HELVETICA_18, unlockSS.str().c_str());
    } else {
        glColor3f(1.0f, 0.85f, 0.2f);
        renderBitmapString(cardX + 70, cardY + cardH - 100, GLUT_BITMAP_HELVETICA_18, "🏆 ALL 100 DISTRICT MISSIONS CONQUERED! 🏆");
    }

    std::stringstream stats1, stats2, stats3, stats4, stats5;
    stats1 << "Time Left Bonus: " << std::fixed << std::setprecision(1) << timer << "s (+ " << static_cast<int>(timer * 110.0f) << " pts)";
    stats2 << "Debris Dodged: " << debrisDodged << " (+ " << (debrisDodged * 40) << " pts)";
    stats3 << "Difficulty Multiplier: " << std::fixed << std::setprecision(1) << scoreMultiplier << "x (" << getIntensityName(selectedIntensity) << ")";
    stats4 << "Points Earned This Level: +" << levelScore << " pts";
    stats5 << "Total Cumulative Score: " << currentScore << " pts  |  High Score: " << highScore;

    glColor3f(0.8f, 0.85f, 0.9f);
    renderBitmapString(cardX + 50, cardY + cardH - 135, GLUT_BITMAP_HELVETICA_12, stats1.str().c_str());
    renderBitmapString(cardX + 50, cardY + cardH - 160, GLUT_BITMAP_HELVETICA_12, stats2.str().c_str());
    renderBitmapString(cardX + 50, cardY + cardH - 185, GLUT_BITMAP_HELVETICA_12, stats3.str().c_str());

    glColor3f(1.0f, 0.88f, 0.2f);
    renderBitmapString(cardX + 50, cardY + cardH - 220, GLUT_BITMAP_HELVETICA_18, stats4.str().c_str());

    glColor3f(0.3f, 0.9f, 1.0f);
    renderBitmapString(cardX + 50, cardY + cardH - 250, GLUT_BITMAP_HELVETICA_12, stats5.str().c_str());

    if (level < MAX_LEVELS) {
        std::stringstream nextSS;
        nextSS << "Press [SPACE / ENTER] -> Next Level " << (level + 1) << " | [L] -> Select Level Menu";
        glColor3f(0.2f, 1.0f, 0.5f);
        renderBitmapString(cardX + 40, cardY + 45, GLUT_BITMAP_HELVETICA_18, nextSS.str().c_str());
    } else {
        glColor3f(0.2f, 1.0f, 0.5f);
        renderBitmapString(cardX + 90, cardY + 45, GLUT_BITMAP_HELVETICA_18, "Press [ENTER] -> Replay Campaign | [M] -> Menu");
    }

    glColor3f(0.7f, 0.75f, 0.85f);
    renderBitmapString(cardX + 80, cardY + 20, GLUT_BITMAP_HELVETICA_12, "Press [R] to Replay Level | [M] for Intensity Menu");
}

// --------------------------------------------------------------------------
// Victory Screen (All Levels Completed)
// --------------------------------------------------------------------------
void drawVictoryScreen() {
    glColor4f(0.04f, 0.08f, 0.14f, 0.92f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    float cardW = 580.0f;
    float cardH = 340.0f;
    float cardX = (windowWidth - cardW) / 2.0f;
    float cardY = (windowHeight - cardH) / 2.0f;
    drawCard(cardX, cardY, cardW, cardH, 0.08f, 0.16f, 0.28f, 0.95f, true);

    glColor3f(0.2f, 1.0f, 0.45f);
    renderBitmapString(cardX + 50, cardY + cardH - 45, GLUT_BITMAP_HELVETICA_18, "VICTORY! YOU ESCAPED ALL 100 SEISMIC ZONES!");

    glColor3f(1.0f, 0.85f, 0.2f);
    renderBitmapString(cardX + 130, cardY + cardH - 85, GLUT_BITMAP_HELVETICA_18, "*** CITY SURVIVOR MASTER ***");

    std::stringstream fscore, hscore, intss;
    fscore << "Final Escape Score: " << currentScore << " Points";
    hscore << "All-Time High Score: " << highScore << " Points";
    intss  << "Conquered on: " << getIntensityName(selectedIntensity);

    glColor3f(0.9f, 0.95f, 1.0f);
    renderBitmapString(cardX + 80, cardY + cardH - 130, GLUT_BITMAP_HELVETICA_18, fscore.str().c_str());
    glColor3f(0.3f, 0.85f, 1.0f);
    renderBitmapString(cardX + 80, cardY + cardH - 165, GLUT_BITMAP_HELVETICA_12, hscore.str().c_str());
    renderBitmapString(cardX + 80, cardY + cardH - 190, GLUT_BITMAP_HELVETICA_12, intss.str().c_str());

    glColor3f(0.25f, 1.0f, 0.5f);
    renderBitmapString(cardX + 60, cardY + 50, GLUT_BITMAP_HELVETICA_18, "Press [ENTER] to Play Again  |  [M] for Menu");
}

// --------------------------------------------------------------------------
// Game Over Screen
// --------------------------------------------------------------------------
void drawGameOverScreen() {
    glColor4f(0.12f, 0.04f, 0.06f, 0.92f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    float cardW = 540.0f;
    float cardH = 300.0f;
    float cardX = (windowWidth - cardW) / 2.0f;
    float cardY = (windowHeight - cardH) / 2.0f;
    drawCard(cardX, cardY, cardW, cardH, 0.22f, 0.08f, 0.10f, 0.95f, true);

    glColor3f(1.0f, 0.25f, 0.25f);
    renderBitmapString(cardX + 110, cardY + cardH - 50, GLUT_BITMAP_HELVETICA_18, "MISSION FAILED: TIME EXPIRED!");

    std::stringstream failSS, scoreSS;
    failSS << "Trapped in Level " << level << " (" << getLevelName(level) << ")";
    scoreSS << "Score Achieved: " << currentScore << " pts  |  High Score: " << highScore << " pts";

    glColor3f(0.9f, 0.9f, 0.95f);
    renderBitmapString(cardX + 80, cardY + cardH - 100, GLUT_BITMAP_HELVETICA_12, failSS.str().c_str());
    glColor3f(1.0f, 0.85f, 0.3f);
    renderBitmapString(cardX + 80, cardY + cardH - 135, GLUT_BITMAP_HELVETICA_18, scoreSS.str().c_str());

    glColor3f(0.3f, 0.9f, 1.0f);
    renderBitmapString(cardX + 60, cardY + 55, GLUT_BITMAP_HELVETICA_18, "Press [R] to Retry Level  |  [M] for Intensity Menu");
}

void renderBitmapString(float x, float y, void *font, const char *string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string);
        string++;
    }
}

// --------------------------------------------------------------------------
// Window Reshape
// --------------------------------------------------------------------------
void reshape(int w, int h) {
    if (h == 0) h = 1;
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

// --------------------------------------------------------------------------
// Input Callbacks
// --------------------------------------------------------------------------
void keyDown(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC

    if (gameState == STATE_INTENSITY_SELECT) {
        if (key == '1') { selectedIntensity = INTENSITY_LOW;     gameState = STATE_LEVEL_SELECT; return; }
        if (key == '2') { selectedIntensity = INTENSITY_MEDIUM;  gameState = STATE_LEVEL_SELECT; return; }
        if (key == '3') { selectedIntensity = INTENSITY_HIGH;    gameState = STATE_LEVEL_SELECT; return; }
        if (key == 13 || key == ' ') { gameState = STATE_LEVEL_SELECT; return; }
    } else if (gameState == STATE_LEVEL_SELECT) {
        if (key == 'p' || key == 'P' || key == '[') {
            levelSelectPage = (levelSelectPage > 0) ? (levelSelectPage - 1) : (TOTAL_PAGES - 1);
            level = levelSelectPage * LEVELS_PER_PAGE + 1;
            if (level > highestUnlockedLevel) level = highestUnlockedLevel;
            return;
        }
        if (key == 'n' || key == 'N' || key == ']' || key == 9) { // 9 is Tab
            levelSelectPage = (levelSelectPage + 1) % TOTAL_PAGES;
            level = levelSelectPage * LEVELS_PER_PAGE + 1;
            if (level > highestUnlockedLevel) level = highestUnlockedLevel;
            return;
        }

        int k = -1;
        if (key >= '1' && key <= '9') {
            k = key - '1';
        } else if (key == '0') {
            k = 9;
        } else if (key >= 'a' && key <= 'j') {
            k = 10 + (key - 'a');
        } else if (key >= 'A' && key <= 'J') {
            k = 10 + (key - 'A');
        }

        if (k >= 0 && k < LEVELS_PER_PAGE) {
            int req = levelSelectPage * LEVELS_PER_PAGE + k + 1;
            if (req <= highestUnlockedLevel) {
                level = req;
                startNewGame(level);
                return;
            }
        }
        if (key == 13 || key == ' ') {
            startNewGame(level);
            return;
        }
        if (key == 'm' || key == 'M') {
            gameState = STATE_INTENSITY_SELECT;
            return;
        }
    } else if (gameState == STATE_LEVEL_CLEAR) {
        if (key == 13 || key == ' ') {
            if (level < MAX_LEVELS) {
                level++;
                initCity();
            } else {
                gameState = STATE_VICTORY;
            }
            return;
        }
        if (key == 'l' || key == 'L') {
            gameState = STATE_LEVEL_SELECT;
            levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
            return;
        }
        if (key == 'r' || key == 'R') {
            initCity();
            return;
        }
        if (key == 'm' || key == 'M') {
            gameState = STATE_INTENSITY_SELECT;
            return;
        }
    } else if (gameState == STATE_VICTORY) {
        if (key == 13 || key == ' ' || key == 'r' || key == 'R') {
            startNewGame(1);
            return;
        }
        if (key == 'l' || key == 'L') {
            gameState = STATE_LEVEL_SELECT;
            levelSelectPage = 0;
            return;
        }
        if (key == 'm' || key == 'M') {
            gameState = STATE_INTENSITY_SELECT;
            return;
        }
    } else if (gameState == STATE_GAMEOVER) {
        if (key == 'r' || key == 'R') {
            initCity();
            return;
        }
        if (key == 'l' || key == 'L') {
            gameState = STATE_LEVEL_SELECT;
            levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
            return;
        }
        if (key == 'm' || key == 'M') {
            gameState = STATE_INTENSITY_SELECT;
            return;
        }
    }

    if ((key == 'v' || key == 'V') && !keys[key]) isFirstPerson = !isFirstPerson;
    if ((key == 'r' || key == 'R') && !keys[key]) initCity();
    if ((key == 'l' || key == 'L') && !keys[key]) {
        gameState = STATE_LEVEL_SELECT;
        levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
    }
    if ((key == 'm' || key == 'M') && !keys[key]) gameState = STATE_INTENSITY_SELECT;

    keys[key] = true;
}

void keyUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void specialKeyDown(int key, int x, int y) {
    if (gameState == STATE_INTENSITY_SELECT) {
        if (key == GLUT_KEY_UP) {
            selectedIntensity = (selectedIntensity == INTENSITY_LOW) ? INTENSITY_HIGH : (selectedIntensity - 1);
            return;
        }
        if (key == GLUT_KEY_DOWN) {
            selectedIntensity = (selectedIntensity == INTENSITY_HIGH) ? INTENSITY_LOW : (selectedIntensity + 1);
            return;
        }
    } else if (gameState == STATE_LEVEL_SELECT) {
        if (key == GLUT_KEY_PAGE_UP) {
            levelSelectPage = (levelSelectPage > 0) ? (levelSelectPage - 1) : (TOTAL_PAGES - 1);
            level = levelSelectPage * LEVELS_PER_PAGE + 1;
            if (level > highestUnlockedLevel) level = highestUnlockedLevel;
            return;
        }
        if (key == GLUT_KEY_PAGE_DOWN) {
            levelSelectPage = (levelSelectPage + 1) % TOTAL_PAGES;
            level = levelSelectPage * LEVELS_PER_PAGE + 1;
            if (level > highestUnlockedLevel) level = highestUnlockedLevel;
            return;
        }

        int target = level;
        if (key == GLUT_KEY_UP) {
            if (target > 4) target -= 4;
            else target = (target + 16 <= MAX_LEVELS) ? (target + 16) : target;
        } else if (key == GLUT_KEY_DOWN) {
            if (target + 4 <= MAX_LEVELS) target += 4;
            else target = (target - 1) % 4 + 1;
        } else if (key == GLUT_KEY_LEFT) {
            target = (target > 1) ? (target - 1) : highestUnlockedLevel;
        } else if (key == GLUT_KEY_RIGHT) {
            target = (target < highestUnlockedLevel) ? (target + 1) : 1;
        }

        if (target >= 1 && target <= highestUnlockedLevel) {
            level = target;
            levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
        }
        return;
    }

    if (key >= 0 && key < 256) specialKeys[key] = true;
}

void specialKeyUp(int key, int x, int y) {
    if (key >= 0 && key < 256) specialKeys[key] = false;
}

void mouse(int button, int state, int x, int y) {
    if (gameState == STATE_INTENSITY_SELECT) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            float glX = static_cast<float>(x);
            float glY = static_cast<float>(windowHeight - y);
            float cardW = 660.0f;
            float cardH = 82.0f;
            float gapY = 24.0f;
            float cardX = (windowWidth - cardW) / 2.0f;
            float startY = windowHeight / 2.0f + 65.0f;
            float y2 = startY - (cardH + gapY);
            float y3 = startY - 2.0f * (cardH + gapY);

            if (glX >= cardX && glX <= (cardX + cardW)) {
                if (glY >= startY && glY <= (startY + cardH)) {
                    selectedIntensity = INTENSITY_LOW;
                    gameState = STATE_LEVEL_SELECT;
                    levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
                } else if (glY >= y2 && glY <= (y2 + cardH)) {
                    selectedIntensity = INTENSITY_MEDIUM;
                    gameState = STATE_LEVEL_SELECT;
                    levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
                } else if (glY >= y3 && glY <= (y3 + cardH)) {
                    selectedIntensity = INTENSITY_HIGH;
                    gameState = STATE_LEVEL_SELECT;
                    levelSelectPage = (level - 1) / LEVELS_PER_PAGE;
                }
            }
        }
        return;
    }

    if (gameState == STATE_LEVEL_SELECT) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            float glX = static_cast<float>(x);
            float glY = static_cast<float>(windowHeight - y);

            const float marginX = 20.0f;
            const float barY = windowHeight - 126.0f;
            const float barH = 26.0f;
            const float navBtnW = 125.0f;

            // Click Prev Tier Button
            if (glX >= marginX && glX <= (marginX + navBtnW) && glY >= barY && glY <= (barY + barH)) {
                levelSelectPage = (levelSelectPage > 0) ? (levelSelectPage - 1) : (TOTAL_PAGES - 1);
                level = levelSelectPage * LEVELS_PER_PAGE + 1;
                if (level > highestUnlockedLevel) level = highestUnlockedLevel;
                return;
            }

            // Click Next Tier Button
            if (glX >= (windowWidth - marginX - navBtnW) && glX <= (windowWidth - marginX) && glY >= barY && glY <= (barY + barH)) {
                levelSelectPage = (levelSelectPage + 1) % TOTAL_PAGES;
                level = levelSelectPage * LEVELS_PER_PAGE + 1;
                if (level > highestUnlockedLevel) level = highestUnlockedLevel;
                return;
            }

            // Click on 5 Tier Tabs
            const float tabW = 126.0f;
            const float tabGap = 8.0f;
            const float tabsStartX = (windowWidth - (5 * tabW + 4 * tabGap)) / 2.0f;
            for (int p = 0; p < TOTAL_PAGES; ++p) {
                float tx = tabsStartX + p * (tabW + tabGap);
                if (glX >= tx && glX <= (tx + tabW) && glY >= barY && glY <= (barY + barH)) {
                    levelSelectPage = p;
                    level = levelSelectPage * LEVELS_PER_PAGE + 1;
                    if (level > highestUnlockedLevel) level = highestUnlockedLevel;
                    return;
                }
            }

            // Click on 20 Level Cards on the current Tier page
            const int cols = 4;
            const float gapX = 14.0f;
            const float cardW = (windowWidth - 2.0f * marginX - (cols - 1) * gapX) / cols;
            const float cardH = 76.0f;
            const float gapY = 10.0f;
            const float topOfGrid = windowHeight - 138.0f;
            const float startY = topOfGrid - cardH;

            for (int k = 0; k < LEVELS_PER_PAGE; ++k) {
                int row = k / cols;
                int col = k % cols;
                float cx = marginX + col * (cardW + gapX);
                float cy = startY - row * (cardH + gapY);

                if (glX >= cx && glX <= (cx + cardW) && glY >= cy && glY <= (cy + cardH)) {
                    int i = levelSelectPage * LEVELS_PER_PAGE + k + 1;
                    if (i <= highestUnlockedLevel) {
                        level = i;
                        startNewGame(level);
                    }
                    return;
                }
            }
        }
        return;
    }

    if (gameState == STATE_LEVEL_CLEAR) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (level < MAX_LEVELS) {
                level++;
                initCity();
            } else {
                gameState = STATE_LEVEL_SELECT;
            }
            return;
        }
    }

    if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMouseDragging = true;
            lastMouseX = x;
        } else {
            isMouseDragging = false;
        }
    }
}

void motion(int x, int y) {
    if (isMouseDragging && gameState != STATE_INTENSITY_SELECT && gameState != STATE_LEVEL_SELECT && gameState != STATE_LEVEL_CLEAR) {
        int dx = x - lastMouseX;
        playerYaw += dx * 0.35f;
        lastMouseX = x;
    }
}

// --------------------------------------------------------------------------
// Main Entry Point
// --------------------------------------------------------------------------
int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(NULL)));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Earthquake Escape - Interactive 3D OpenGL Simulation");

    initGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(16, update, 0);

    std::cout << "========================================================\n";
    std::cout << "  EARTHQUAKE ESCAPE - 3D Multi-Level OpenGL Game (20 Levels)\n";
    std::cout << "========================================================\n";
    std::cout << "  Real Architecture: Windows, Entrances, Doors, Rooftops & Tanks\n";
    std::cout << "  Start Screen     : 4 Intensity Presets (Low, Med, High, Extreme)\n";
    std::cout << "  Levels           : 20 Unique Urban Districts with Dynamic Lighting\n";
    std::cout << "  Interactive Items: Medkits (+Stamina), Adrenaline (+Speed),\n";
    std::cout << "                     Chrono (+Time), Shield Sphere (Debris Immunity)\n";
    std::cout << "  Dynamic Physics  : Kickable Cones & Safety Drums with Momentum\n";
    std::cout << "  Controls:\n";
    std::cout << "    [W, A, S, D] / [Arrows] : Walk Forward / Left / Back / Right\n";
    std::cout << "    [F]                     : Sprint / Rush (Uses Stamina)\n";
    std::cout << "    [Q, E] / [Mouse Drag]   : Rotate Camera Left / Right\n";
    std::cout << "    [Space]                 : Jump over low debris & barricades\n";
    std::cout << "    [V]                     : Toggle 3rd / 1st Person View\n";
    std::cout << "    [R]                     : Restart Current Level\n";
    std::cout << "    [L]                     : Level Select Menu (20 Levels Grid)\n";
    std::cout << "    [M]                     : Return to Intensity Menu\n";
    std::cout << "    [ESC]                   : Exit Game\n";
    std::cout << "========================================================\n";

    glutMainLoop();
    return 0;
}
