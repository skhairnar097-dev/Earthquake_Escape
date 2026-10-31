// Earthquake Escape - Computer Graphics Concept Inspector
// Educational debugging tool demonstrating 3D transformations, projection matrices, wireframes, and AABB collision boxes.

class CGInspector {
    constructor(scene, camera, renderer) {
        this.scene = scene;
        this.camera = camera;
        this.renderer = renderer;

        this.enabled = false;
        this.wireframeMode = false;
        this.showColliders = false;

        // Visual helper objects
        this.colliderHelperGroup = new THREE.Group();
        this.scene.add(this.colliderHelperGroup);
        this.colliderHelpers = [];

        // Oscilloscope canvas context
        this.oscCanvas = null;
        this.oscCtx = null;
        this.waveHistory = [];

        this.createInspectorUI();
    }

    createInspectorUI() {
        const panel = document.createElement('div');
        panel.id = 'cg-inspector-panel';
        panel.className = 'cg-panel hidden';
        panel.innerHTML = `
            <div class="cg-panel-header">
                <div class="cg-title">
                    <span class="icon">📐</span>
                    <span>CG Concept & Pipeline Inspector</span>
                </div>
                <button id="cg-close-btn" class="cg-icon-btn">✕</button>
            </div>

            <div class="cg-panel-body">
                <!-- Toggles -->
                <div class="cg-section">
                    <div class="cg-section-title">Visual Modes</div>
                    <div class="cg-btn-row">
                        <button id="btn-toggle-wireframe" class="cg-toggle-btn">Wireframe Mode: OFF</button>
                        <button id="btn-toggle-aabb" class="cg-toggle-btn">AABB Bounding Boxes: OFF</button>
                    </div>
                </div>

                <!-- Live CG Transformations -->
                <div class="cg-section">
                    <div class="cg-section-title">Active Transformations (Player & World)</div>
                    <div class="cg-stat-grid">
                        <div class="cg-stat-box">
                            <span class="lbl">Translation [T]:</span>
                            <span id="cg-stat-pos" class="val">X: 0.0, Y: 0.0, Z: 0.0</span>
                        </div>
                        <div class="cg-stat-box">
                            <span class="lbl">Rotation [R] (Euler):</span>
                            <span id="cg-stat-rot" class="val">Pitch: 0.0°, Yaw: 0.0°</span>
                        </div>
                        <div class="cg-stat-box">
                            <span class="lbl">Scaling [S] (Safe Beacon):</span>
                            <span id="cg-stat-scale" class="val">1.00x</span>
                        </div>
                        <div class="cg-stat-box">
                            <span class="lbl">Triangles / Draw Calls:</span>
                            <span id="cg-stat-perf" class="val">0 / 0</span>
                        </div>
                    </div>
                </div>

                <!-- Seismic Wave Oscilloscope (Mathematical Wave Synthesis) -->
                <div class="cg-section">
                    <div class="cg-section-title">Seismic Wave Harmonics: f(t) = A₁·sin(ω₁t) + A₂·sin(ω₂t)</div>
                    <canvas id="cg-oscilloscope" width="340" height="75"></canvas>
                </div>

                <!-- 4x4 View & Projection Matrices -->
                <div class="cg-section">
                    <div class="cg-section-title">Camera 4x4 Perspective Projection Matrix</div>
                    <div id="cg-matrix-display" class="cg-matrix-mono">Loading matrix...</div>
                </div>

                <!-- Viva Explanations -->
                <div class="cg-section">
                    <div class="cg-section-title">CG Concepts Applied in this Project</div>
                    <ul class="cg-concepts-list">
                        <li><strong>Translation:</strong> Matrix multiplication for player movement and falling debris trajectory.</li>
                        <li><strong>Rotation:</strong> Real-time Euler/quaternion building swaying and trauma camera roll.</li>
                        <li><strong>Scaling:</strong> Dynamic matrix scaling of safe-zone visual beacon and HUD warning alarms.</li>
                        <li><strong>Perspective Projection:</strong> Frustum transformation mapping 3D eye space into 2D screen coordinates.</li>
                        <li><strong>AABB Collision:</strong> Axis-Aligned Bounding Box overlap tests $[x_{min}, x_{max}] \cap [y_{min}, y_{max}] \cap [z_{min}, z_{max}]$.</li>
                    </ul>
                </div>
            </div>
        `;
        document.body.appendChild(panel);

        // Bind events
        document.getElementById('cg-close-btn').addEventListener('click', () => this.toggle());
        document.getElementById('btn-toggle-wireframe').addEventListener('click', () => this.toggleWireframe());
        document.getElementById('btn-toggle-aabb').addEventListener('click', () => this.toggleColliders());

        this.oscCanvas = document.getElementById('cg-oscilloscope');
        if (this.oscCanvas) {
            this.oscCtx = this.oscCanvas.getContext('2d');
        }

        // Global hotkey 'C' to toggle inspector
        window.addEventListener('keydown', (e) => {
            if (e.code === 'KeyC' && !e.target.matches('input, textarea')) {
                this.toggle();
            }
        });
    }

    toggle() {
        this.enabled = !this.enabled;
        const panel = document.getElementById('cg-inspector-panel');
        if (panel) {
            panel.classList.toggle('hidden', !this.enabled);
        }
        const hudBtn = document.getElementById('btn-hud-inspector');
        if (hudBtn) {
            hudBtn.classList.toggle('active', this.enabled);
        }
    }

    toggleWireframe() {
        this.wireframeMode = !this.wireframeMode;
        const btn = document.getElementById('btn-toggle-wireframe');
        if (btn) {
            btn.textContent = `Wireframe Mode: ${this.wireframeMode ? 'ON' : 'OFF'}`;
            btn.classList.toggle('active', this.wireframeMode);
        }

        this.scene.traverse((child) => {
            if (child.isMesh && child.material) {
                if (Array.isArray(child.material)) {
                    child.material.forEach(m => m.wireframe = this.wireframeMode);
                } else {
                    child.material.wireframe = this.wireframeMode;
                }
            }
        });
    }

    toggleColliders(staticColliders = []) {
        this.showColliders = !this.showColliders;
        const btn = document.getElementById('btn-toggle-aabb');
        if (btn) {
            btn.textContent = `AABB Bounding Boxes: ${this.showColliders ? 'ON' : 'OFF'}`;
            btn.classList.toggle('active', this.showColliders);
        }

        if (this.showColliders) {
            this.buildColliderHelpers(staticColliders);
        } else {
            this.clearColliderHelpers();
        }
    }

    buildColliderHelpers(colliders) {
        this.clearColliderHelpers();
        const boxGeo = new THREE.BoxGeometry(1, 1, 1);
        const wireGeo = new THREE.WireframeGeometry(boxGeo);
        const greenMat = new THREE.LineBasicMaterial({ color: 0x00ff88, linewidth: 1 });

        colliders.forEach(box => {
            const center = new THREE.Vector3();
            const size = new THREE.Vector3();
            box.getCenter(center);
            box.getSize(size);

            const line = new THREE.LineSegments(wireGeo, greenMat);
            line.position.copy(center);
            line.scale.copy(size);
            this.colliderHelperGroup.add(line);
            this.colliderHelpers.push(line);
        });
    }

    clearColliderHelpers() {
        while (this.colliderHelperGroup.children.length > 0) {
            const obj = this.colliderHelperGroup.children[0];
            this.colliderHelperGroup.remove(obj);
            if (obj.geometry) obj.geometry.dispose();
        }
        this.colliderHelpers = [];
    }

    update(player, earthquake, safeZone) {
        if (!this.enabled) return;

        // 1. Update Player Translation & Rotation
        if (player) {
            const posEl = document.getElementById('cg-stat-pos');
            if (posEl) {
                posEl.textContent = `X: ${player.position.x.toFixed(1)}, Y: ${player.position.y.toFixed(1)}, Z: ${player.position.z.toFixed(1)}`;
            }
            const rotEl = document.getElementById('cg-stat-rot');
            if (rotEl) {
                const pitchDeg = (player.pitch * 180 / Math.PI).toFixed(0);
                const yawDeg = (player.yaw * 180 / Math.PI).toFixed(0);
                rotEl.textContent = `Pitch: ${pitchDeg}°, Yaw: ${yawDeg}°`;
            }
        }

        // 2. Update Scaling
        if (safeZone && safeZone.rings && safeZone.rings[0]) {
            const scaleEl = document.getElementById('cg-stat-scale');
            if (scaleEl) {
                scaleEl.textContent = `${safeZone.rings[0].scale.x.toFixed(2)}x (Pulsing)`;
            }
        }

        // 3. Update Renderer Performance Info
        if (this.renderer && this.renderer.info) {
            const perfEl = document.getElementById('cg-stat-perf');
            if (perfEl) {
                const tris = this.renderer.info.render.triangles || 0;
                const calls = this.renderer.info.render.calls || 0;
                perfEl.textContent = `${tris.toLocaleString()} Tris / ${calls} Calls`;
            }
        }

        // 4. Update 4x4 Perspective Projection Matrix
        if (this.camera) {
            const matEl = document.getElementById('cg-matrix-display');
            if (matEl) {
                const el = this.camera.projectionMatrix.elements;
                matEl.innerHTML = `
                    | ${el[0].toFixed(2)}  ${el[4].toFixed(2)}  ${el[8].toFixed(2)}   ${el[12].toFixed(2)} |<br>
                    | ${el[1].toFixed(2)}  ${el[5].toFixed(2)}  ${el[9].toFixed(2)}   ${el[13].toFixed(2)} |<br>
                    | ${el[2].toFixed(2)}  ${el[6].toFixed(2)}  ${el[10].toFixed(2)}  ${el[14].toFixed(2)} |<br>
                    | ${el[3].toFixed(2)}  ${el[7].toFixed(2)}  ${el[11].toFixed(2)}  ${el[15].toFixed(2)} |
                `;
            }
        }

        // 5. Draw Seismic Wave Oscilloscope
        this.renderOscilloscope(earthquake);
    }

    renderOscilloscope(earthquake) {
        if (!this.oscCtx || !this.oscCanvas) return;
        const ctx = this.oscCtx;
        const w = this.oscCanvas.width;
        const h = this.oscCanvas.height;

        let dispY = 0;
        if (earthquake && (earthquake.state === 'ACTIVE' || earthquake.state === 'WARNING')) {
            dispY = earthquake.currentDisplacement.x * 70;
        }

        this.waveHistory.push(dispY);
        if (this.waveHistory.length > w) {
            this.waveHistory.shift();
        }

        // Clear canvas
        ctx.fillStyle = '#0f172a';
        ctx.fillRect(0, 0, w, h);

        // Center line
        ctx.strokeStyle = '#334155';
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(0, h / 2);
        ctx.lineTo(w, h / 2);
        ctx.stroke();

        // Draw Waveform
        ctx.strokeStyle = (earthquake && earthquake.state === 'ACTIVE') ? '#ef4444' : '#22c55e';
        ctx.lineWidth = 2;
        ctx.beginPath();
        for (let i = 0; i < this.waveHistory.length; i++) {
            const x = i;
            const y = h / 2 - this.waveHistory[i];
            if (i === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        }
        ctx.stroke();
    }
}

window.CGInspector = CGInspector;
