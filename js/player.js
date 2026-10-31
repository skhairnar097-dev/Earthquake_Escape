// Earthquake Escape - Player Controller & Camera System
// Supports First-Person & Third-Person perspectives, WASD movement, sprint, jump, mouse look, and swept AABB collision.

class PlayerController {
    constructor(camera, domElement, soundEngine) {
        this.camera = camera;
        this.domElement = domElement;
        this.soundEngine = soundEngine;

        // Player Physical State
        this.position = new THREE.Vector3(0, 1.0, 0);
        this.velocity = new THREE.Vector3();
        this.yaw = 0;   // Horizontal rotation (radians)
        this.pitch = 0; // Vertical rotation (radians)
        this.height = 1.75;
        this.radius = 0.55;

        // Movement settings
        this.walkSpeed = 6.5;
        this.sprintSpeed = 11.5;
        this.jumpVelocity = 7.5;
        this.gravity = 20.0;
        this.isGrounded = true;

        // Stamina
        this.stamina = 100;
        this.maxStamina = 100;
        this.isSprinting = false;

        // Camera mode: 'FPS' (First Person) or 'TPS' (Third Person) - Default: TPS for superior spatial navigation
        this.cameraMode = 'TPS';
        this.tpsDistance = 4.5;
        this.tpsHeight = 2.2;

        // Visual character model for Third-Person
        this.characterGroup = this.createCharacterMesh();
        
        // Input tracking
        this.keys = {
            forward: false,
            backward: false,
            left: false,
            right: false,
            sprint: false,
            jump: false,
            turnLeft: false,
            turnRight: false
        };

        this.mouseSensitivity = 0.0024;
        this.isPointerLocked = false;
        this.isMouseDown = false;
        this.lastMouseX = 0;
        this.lastMouseY = 0;
        this.stepTimer = 0;

        this.setupEventListeners();
    }

    createCharacterMesh() {
        const group = new THREE.Group();

        // Stylized survivor character
        const skinMat = new THREE.MeshLambertMaterial({ color: 0xffdbac });
        const jacketMat = new THREE.MeshStandardMaterial({ color: 0x2563eb, roughness: 0.6 }); // Emergency blue jacket
        const pantsMat = new THREE.MeshStandardMaterial({ color: 0x1e293b, roughness: 0.8 });
        const helmetMat = new THREE.MeshStandardMaterial({ color: 0xf59e0b, roughness: 0.4 }); // Safety yellow helmet
        const visorMat = new THREE.MeshBasicMaterial({ color: 0x38bdf8 }); // Glowing cyan eye visor / headlamp
        const packMat = new THREE.MeshStandardMaterial({ color: 0xe11d48, roughness: 0.5 }); // Emergency red survival pack

        // Torso
        const torsoGeo = new THREE.BoxGeometry(0.65, 0.75, 0.35);
        const torso = new THREE.Mesh(torsoGeo, jacketMat);
        torso.position.y = 1.0;
        torso.castShadow = true;
        group.add(torso);

        // Emergency Survival Pack (Backpack - on back at +Z)
        const packGeo = new THREE.BoxGeometry(0.45, 0.55, 0.20);
        const pack = new THREE.Mesh(packGeo, packMat);
        pack.position.set(0, 1.0, 0.24);
        group.add(pack);

        // Head
        const headGeo = new THREE.BoxGeometry(0.35, 0.35, 0.35);
        const head = new THREE.Mesh(headGeo, skinMat);
        head.position.y = 1.55;
        group.add(head);

        // Safety Helmet
        const helmetGeo = new THREE.CylinderGeometry(0.24, 0.26, 0.18, 8);
        const helmet = new THREE.Mesh(helmetGeo, helmetMat);
        helmet.position.y = 1.73;
        group.add(helmet);

        // Headlamp / Visor (Front at -Z)
        const visorGeo = new THREE.BoxGeometry(0.26, 0.08, 0.08);
        const visor = new THREE.Mesh(visorGeo, visorMat);
        visor.position.set(0, 1.66, -0.19);
        group.add(visor);

        // Left & Right Legs
        const legGeo = new THREE.BoxGeometry(0.24, 0.7, 0.24);
        this.leftLeg = new THREE.Mesh(legGeo, pantsMat);
        this.leftLeg.position.set(-0.16, 0.35, 0);
        this.rightLeg = new THREE.Mesh(legGeo, pantsMat);
        this.rightLeg.position.set(0.16, 0.35, 0);
        group.add(this.leftLeg);
        group.add(this.rightLeg);

        // Left & Right Arms
        const armGeo = new THREE.BoxGeometry(0.18, 0.65, 0.18);
        this.leftArm = new THREE.Mesh(armGeo, jacketMat);
        this.leftArm.position.set(-0.45, 0.95, 0);
        this.rightArm = new THREE.Mesh(armGeo, jacketMat);
        this.rightArm.position.set(0.45, 0.95, 0);
        group.add(this.leftArm);
        group.add(this.rightArm);

        group.visible = true; // Visible by default in TPS
        return group;
    }

    setupEventListeners() {
        window.addEventListener('keydown', (e) => this.onKeyDown(e));
        window.addEventListener('keyup', (e) => this.onKeyUp(e));

        // Optional Pointer Lock
        this.domElement.addEventListener('click', (e) => {
            // Only request pointer lock if clicking on the 3D scene (not UI buttons)
            if (e.target === this.domElement && !this.isPointerLocked) {
                this.domElement.requestPointerLock?.();
            }
        });

        document.addEventListener('pointerlockchange', () => {
            this.isPointerLocked = document.pointerLockElement === this.domElement;
        });

        // Mouse Drag Support (Works smoothly even without pointer lock!)
        this.domElement.addEventListener('mousedown', (e) => {
            if (e.button === 0) { // Left click drag
                this.isMouseDown = true;
                this.lastMouseX = e.clientX;
                this.lastMouseY = e.clientY;
            }
        });

        window.addEventListener('mouseup', () => {
            this.isMouseDown = false;
        });

        window.addEventListener('mousemove', (e) => {
            if (this.isPointerLocked) {
                this.yaw -= e.movementX * this.mouseSensitivity;
                this.pitch -= e.movementY * this.mouseSensitivity;
                this.pitch = Math.max(-1.48, Math.min(1.48, this.pitch));
            } else if (this.isMouseDown) {
                const dx = e.clientX - this.lastMouseX;
                const dy = e.clientY - this.lastMouseY;
                this.lastMouseX = e.clientX;
                this.lastMouseY = e.clientY;
                this.yaw -= dx * this.mouseSensitivity * 1.5;
                this.pitch -= dy * this.mouseSensitivity * 1.5;
                this.pitch = Math.max(-1.48, Math.min(1.48, this.pitch));
            }
        });

        // Touch drag support for mobile/touchpads
        let lastTouchX = 0, lastTouchY = 0;
        this.domElement.addEventListener('touchstart', (e) => {
            if (e.touches.length === 1) {
                lastTouchX = e.touches[0].clientX;
                lastTouchY = e.touches[0].clientY;
            }
        }, { passive: true });

        this.domElement.addEventListener('touchmove', (e) => {
            if (e.touches.length === 1) {
                const dx = e.touches[0].clientX - lastTouchX;
                const dy = e.touches[0].clientY - lastTouchY;
                lastTouchX = e.touches[0].clientX;
                lastTouchY = e.touches[0].clientY;
                this.yaw -= dx * this.mouseSensitivity * 1.8;
                this.pitch -= dy * this.mouseSensitivity * 1.8;
                this.pitch = Math.max(-1.48, Math.min(1.48, this.pitch));
            }
        }, { passive: true });
    }

    onKeyDown(e) {
        switch (e.code) {
            case 'KeyW':
            case 'ArrowUp':
                this.keys.forward = true;
                break;
            case 'KeyS':
            case 'ArrowDown':
                this.keys.backward = true;
                break;
            case 'KeyA':
                this.keys.left = true;
                break;
            case 'KeyD':
                this.keys.right = true;
                break;
            case 'ArrowLeft':
            case 'KeyQ':
                this.keys.turnLeft = true; // Natural steering with left arrow!
                break;
            case 'ArrowRight':
            case 'KeyE':
                this.keys.turnRight = true; // Natural steering with right arrow!
                break;
            case 'ShiftLeft':
            case 'ShiftRight':
                this.keys.sprint = true;
                break;
            case 'Space':
                if (this.isGrounded) {
                    this.velocity.y = this.jumpVelocity;
                    this.isGrounded = false;
                    if (this.soundEngine) this.soundEngine.playJump();
                }
                break;
            case 'KeyV':
                this.toggleCameraMode();
                break;
        }
    }

    onKeyUp(e) {
        switch (e.code) {
            case 'KeyW':
            case 'ArrowUp':
                this.keys.forward = false;
                break;
            case 'KeyS':
            case 'ArrowDown':
                this.keys.backward = false;
                break;
            case 'KeyA':
                this.keys.left = false;
                break;
            case 'KeyD':
                this.keys.right = false;
                break;
            case 'ArrowLeft':
            case 'KeyQ':
                this.keys.turnLeft = false;
                break;
            case 'ArrowRight':
            case 'KeyE':
                this.keys.turnRight = false;
                break;
            case 'ShiftLeft':
            case 'ShiftRight':
                this.keys.sprint = false;
                break;
        }
    }

    toggleCameraMode() {
        this.cameraMode = this.cameraMode === 'FPS' ? 'TPS' : 'FPS';
        this.characterGroup.visible = (this.cameraMode === 'TPS');
        return this.cameraMode;
    }

    spawnAt(pos, targetLookPos = null) {
        this.position.copy(pos);
        this.velocity.set(0, 0, 0);
        this.pitch = 0;
        this.stamina = 100;
        this.isGrounded = true;

        // Reset all key states
        this.keys.forward = false;
        this.keys.backward = false;
        this.keys.left = false;
        this.keys.right = false;
        this.keys.turnLeft = false;
        this.keys.turnRight = false;
        this.keys.sprint = false;

        // Orient player toward Safe Zone
        if (targetLookPos) {
            const dx = targetLookPos.x - pos.x;
            const dz = targetLookPos.z - pos.z;
            this.yaw = Math.atan2(-dx, -dz);
        } else {
            this.yaw = Math.PI; // Face straight South (+Z) down Central Evacuation Boulevard
        }

        this.characterGroup.position.copy(this.position);
        this.characterGroup.position.y -= 1.0;
        this.characterGroup.rotation.y = this.yaw;
    }

    update(delta, staticColliders = [], dynamicColliders = [], cameraShake = null) {
        // 1. Keyboard rotation steering support (Left/Right arrows or Q/E)
        if (this.keys.turnLeft) this.yaw += 2.8 * delta;
        if (this.keys.turnRight) this.yaw -= 2.8 * delta;

        // 2. Sprint & Stamina Mechanics
        const isMoving = this.keys.forward || this.keys.backward || this.keys.left || this.keys.right;
        if (this.keys.sprint && isMoving && this.stamina > 5) {
            this.isSprinting = true;
            this.stamina = Math.max(0, this.stamina - delta * 24);
        } else {
            this.isSprinting = false;
            this.stamina = Math.min(this.maxStamina, this.stamina + delta * 15);
        }

        const currentSpeed = this.isSprinting ? this.sprintSpeed : this.walkSpeed;

        // 3. Movement direction in world space relative to yaw
        const moveDir = new THREE.Vector3();
        if (this.keys.forward) moveDir.z -= 1;
        if (this.keys.backward) moveDir.z += 1;
        if (this.keys.left) moveDir.x -= 1;
        if (this.keys.right) moveDir.x += 1;

        if (moveDir.lengthSq() > 0) {
            moveDir.normalize();
            // Rotate direction vector by yaw angle
            moveDir.applyAxisAngle(new THREE.Vector3(0, 1, 0), this.yaw);

            // Step audio
            if (this.isGrounded) {
                this.stepTimer += delta * (this.isSprinting ? 1.6 : 1.0);
                if (this.stepTimer > 0.38) {
                    this.stepTimer = 0;
                    if (this.soundEngine) this.soundEngine.playFootstep();
                }
            }
        }

        // Horizontal velocity
        this.velocity.x = moveDir.x * currentSpeed;
        this.velocity.z = moveDir.z * currentSpeed;

        // 4. Gravity & Vertical Physics
        this.velocity.y -= this.gravity * delta;

        // 5. Sliding Collision Resolution along X and Z axes
        const nextPos = this.position.clone();

        // Test X axis movement
        nextPos.x += this.velocity.x * delta;
        if (!this.checkCollision(nextPos, staticColliders, dynamicColliders)) {
            this.position.x = nextPos.x;
        } else {
            this.velocity.x = 0; // Slide along wall
        }

        // Test Z axis movement
        nextPos.x = this.position.x;
        nextPos.z += this.velocity.z * delta;
        if (!this.checkCollision(nextPos, staticColliders, dynamicColliders)) {
            this.position.z = nextPos.z;
        } else {
            this.velocity.z = 0; // Slide along wall
        }

        // Apply Y vertical motion
        this.position.y += this.velocity.y * delta;
        if (this.position.y <= 1.0) { // Ground plane height
            this.position.y = 1.0;
            this.velocity.y = 0;
            this.isGrounded = true;
        }

        // 6. Update Third-Person Character Mesh position & leg swing animation
        this.characterGroup.position.copy(this.position);
        this.characterGroup.position.y -= 1.0; // Place feet on ground
        this.characterGroup.rotation.y = this.yaw;

        if (isMoving && this.isGrounded) {
            const swing = Math.sin(Date.now() * 0.012 * (this.isSprinting ? 1.5 : 1.0)) * 0.45;
            this.leftLeg.rotation.x = swing;
            this.rightLeg.rotation.x = -swing;
            this.leftArm.rotation.x = -swing;
            this.rightArm.rotation.x = swing;
        } else {
            this.leftLeg.rotation.x = 0;
            this.rightLeg.rotation.x = 0;
            this.leftArm.rotation.x = 0;
            this.rightArm.rotation.x = 0;
        }

        // 7. Update Camera View Transformation
        this.updateCamera(cameraShake);
    }

    checkCollision(testPos, staticColliders, dynamicColliders) {
        // Candidate player bounding box at proposed next position
        const pBox = new THREE.Box3();
        pBox.setFromCenterAndSize(
            new THREE.Vector3(testPos.x, testPos.y + 0.4, testPos.z),
            new THREE.Vector3(this.radius * 2, this.height, this.radius * 2)
        );

        // Current player bounding box
        const curBox = new THREE.Box3();
        curBox.setFromCenterAndSize(
            new THREE.Vector3(this.position.x, this.position.y + 0.4, this.position.z),
            new THREE.Vector3(this.radius * 2, this.height, this.radius * 2)
        );

        // Check against static city walls
        for (let i = 0; i < staticColliders.length; i++) {
            const collider = staticColliders[i];
            if (pBox.intersectsBox(collider)) {
                // If the player is already intersecting this collider, allow movement if moving AWAY from center (unstuck logic)
                if (curBox.intersectsBox(collider)) {
                    const curCenter = new THREE.Vector3();
                    const testCenter = new THREE.Vector3();
                    const colCenter = new THREE.Vector3();
                    curBox.getCenter(curCenter);
                    pBox.getCenter(testCenter);
                    collider.getCenter(colCenter);
                    if (testCenter.distanceToSquared(colCenter) > curCenter.distanceToSquared(colCenter)) {
                        continue; // Moving away from obstacle center, allow unstuck motion!
                    }
                }
                return true;
            }
        }

        // Check against dynamic fallen debris obstacles
        for (let i = 0; i < dynamicColliders.length; i++) {
            const collider = dynamicColliders[i].box;
            if (pBox.intersectsBox(collider)) {
                if (curBox.intersectsBox(collider)) {
                    const curCenter = new THREE.Vector3();
                    const testCenter = new THREE.Vector3();
                    const colCenter = new THREE.Vector3();
                    curBox.getCenter(curCenter);
                    pBox.getCenter(testCenter);
                    collider.getCenter(colCenter);
                    if (testCenter.distanceToSquared(colCenter) > curCenter.distanceToSquared(colCenter)) {
                        continue; // Moving away from debris
                    }
                }
                return true;
            }
        }

        return false;
    }

    updateCamera(cameraShake) {
        // Base camera rotation
        const euler = new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ');

        if (this.cameraMode === 'FPS') {
            // First Person: Camera placed at player's eye level (height ~1.65m)
            this.camera.position.set(this.position.x, this.position.y + 0.65, this.position.z);
            this.camera.quaternion.setFromEuler(euler);
        } else {
            // Third Person: Camera follows behind player at offset
            const offset = new THREE.Vector3(0, this.tpsHeight, this.tpsDistance);
            offset.applyEuler(new THREE.Euler(0, this.yaw, 0, 'YXZ'));

            this.camera.position.set(
                this.position.x + offset.x,
                this.position.y + offset.y,
                this.position.z + offset.z
            );
            // Look directly at player upper body
            this.camera.lookAt(this.position.x, this.position.y + 0.5, this.position.z);
        }

        // Apply 6-DOF Earthquake Camera Shake
        if (cameraShake) {
            this.camera.position.add(cameraShake.offsetPos);
            // Add shake euler rotation
            const shakeRot = cameraShake.offsetRot;
            this.camera.rotation.x += shakeRot.x;
            this.camera.rotation.y += shakeRot.y;
            this.camera.rotation.z += shakeRot.z;
        }
    }
}

window.PlayerController = PlayerController;
