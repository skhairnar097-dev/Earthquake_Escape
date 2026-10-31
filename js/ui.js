// Earthquake Escape - User Interface, HUD, Minimap Radar & Viva Guide

class UIManager {
    constructor() {
        this.selectedIntensity = 'medium'; // 'low', 'medium', 'high'
        this.selectedLevel = 1;
        this.unlockedLevel = parseInt(localStorage.getItem('earthquake_unlocked_level') || '1', 10);
        this.completedLevels = new Set(JSON.parse(localStorage.getItem('earthquake_completed_levels') || '[]'));

        this.intensityData = {
            low:     { name: 'Low',     richter: 5.5, mult: 1.0 },
            medium:  { name: 'Medium',  richter: 7.0, mult: 1.5 },
            high:    { name: 'High',    richter: 8.2, mult: 2.2 }
        };

        this.levelNames = {
            1: 'Downtown Plaza',
            2: 'Financial District',
            3: 'Industrial Sector',
            4: 'Metropolis Center',
            5: 'Ground Zero',
            6: 'Neon Transit Hub',
            7: 'Hillside Overlook',
            8: 'Bio-Tech Zone',
            9: 'Coastal Sea Wall',
            10: 'The Apex Citadel',
            11: 'Subway Concourse',
            12: 'Skyline Promenade',
            13: 'Industrial Rail Yards',
            14: 'Cathedral District',
            15: 'Omega Epicenter',
            16: 'Volcanic Ridge Caldera',
            17: 'Airport Runway Hub',
            18: 'Old Port Canals',
            19: 'High-Tech Silicon Core',
            20: 'Armageddon Epicenter'
        };

        this.minimapCanvas = null;
        this.minimapCtx = null;
        this.onStartGameCallback = null;
        this.onRestartCallback = null;
        this.soundEngine = window.soundEngine;
    }

    getLevelName(lvl) {
        if (this.levelNames[lvl]) return this.levelNames[lvl];
        const prefixes = [
            "Neo Metro", "Cyber", "Underground", "Obsidian", "Quake Fault", "Cryo", 
            "Titan", "Vortex", "Apex", "Solar", "Magma", "Zenith", "Abyss", "Quantum", "Nexus", "Aero"
        ];
        const suffixes = [
            "Station", "Terminal", "Bunker", "Skyway", "Vault", "Avenue", 
            "Sector", "Runway", "Bastion", "Core", "Sanctuary", "Depot", "Grid", "Bridge"
        ];
        const p = prefixes[(lvl * 3) % prefixes.length];
        const s = suffixes[(lvl * 7) % suffixes.length];
        return `${p} ${s} #${lvl}`;
    }

    init() {
        this.minimapCanvas = document.getElementById('minimap-canvas');
        if (this.minimapCanvas) {
            this.minimapCtx = this.minimapCanvas.getContext('2d');
        }

        this.updateLevelProgressionUI();
        this.updateStartButtonLabel();
        this.bindEvents();
    }

    bindEvents() {
        // Step 1: Intensity card selection
        const intensityCards = document.querySelectorAll('.intensity-card');
        intensityCards.forEach(card => {
            card.addEventListener('click', () => {
                intensityCards.forEach(c => c.classList.remove('selected'));
                card.classList.add('selected');
                this.selectedIntensity = card.dataset.intensity;
                this.updateStartButtonLabel();
            });
        });

        // Step 2: Level card selection (one-by-one with delegation for up to 100 levels)
        const levelsGrid = document.querySelector('.levels-grid');
        if (levelsGrid) {
            levelsGrid.addEventListener('click', (e) => {
                const card = e.target.closest('.level-card');
                if (!card) return;
                const lvl = parseInt(card.dataset.level, 10);
                if (lvl > this.unlockedLevel) {
                    this.showWarningBanner(`Level ${lvl} is locked! Clear Level ${lvl - 1} first.`);
                    return;
                }
                this.selectLevel(lvl);
            });
        }

        // Start game button
        const startBtn = document.getElementById('btn-start-game');
        if (startBtn) {
            startBtn.addEventListener('click', () => {
                if (this.soundEngine) this.soundEngine.resume();
                document.getElementById('start-screen').classList.add('hidden');
                document.getElementById('game-hud').classList.remove('hidden');
                document.activeElement?.blur?.();
                window.focus();
                if (this.onStartGameCallback) {
                    this.onStartGameCallback(this.selectedLevel, this.selectedIntensity);
                }
            });
        }

        // HUD Menu button (return to Level Select)
        const menuBtn = document.getElementById('btn-hud-menu');
        if (menuBtn) {
            menuBtn.addEventListener('click', () => {
                this.openLevelSelectMenu();
            });
        }

        // Restart button in HUD
        const restartBtn = document.getElementById('btn-hud-restart');
        if (restartBtn) {
            restartBtn.addEventListener('click', () => {
                document.activeElement?.blur?.();
                window.focus();
                if (this.onRestartCallback) {
                    this.onRestartCallback(this.selectedLevel, this.selectedIntensity);
                }
            });
        }

        // Modal Action: Select Level Menu
        const modalMenu = document.getElementById('btn-modal-menu');
        if (modalMenu) {
            modalMenu.addEventListener('click', () => {
                document.getElementById('result-modal').classList.add('hidden');
                this.openLevelSelectMenu();
            });
        }

        // Modal Action: Replay Level
        const modalRetry = document.getElementById('btn-modal-retry');
        if (modalRetry) {
            modalRetry.addEventListener('click', () => {
                document.getElementById('result-modal').classList.add('hidden');
                document.activeElement?.blur?.();
                window.focus();
                if (this.onRestartCallback) {
                    this.onRestartCallback(this.selectedLevel, this.selectedIntensity);
                }
            });
        }

        // Modal Action: Next Level
        const modalNext = document.getElementById('btn-modal-next');
        if (modalNext) {
            modalNext.addEventListener('click', () => {
                document.getElementById('result-modal').classList.add('hidden');
                document.activeElement?.blur?.();
                window.focus();
                let nextLvl = this.selectedLevel + 1;
                if (nextLvl > 100) nextLvl = 1;
                this.selectLevel(nextLvl);
                if (this.onRestartCallback) {
                    this.onRestartCallback(this.selectedLevel, this.selectedIntensity);
                }
            });
        }

        // Sound toggle
        const soundBtn = document.getElementById('btn-hud-sound');
        if (soundBtn) {
            soundBtn.addEventListener('click', () => {
                if (this.soundEngine) {
                    const isMuted = this.soundEngine.toggleMute();
                    soundBtn.textContent = isMuted ? '🔇 Audio OFF' : '🔊 Audio ON';
                }
            });
        }

        // Viva Study Guide Modal Toggles
        const vivaBtn = document.getElementById('btn-hud-viva');
        const vivaModal = document.getElementById('viva-modal');
        const vivaClose = document.getElementById('btn-close-viva');
        if (vivaBtn && vivaModal) {
            vivaBtn.addEventListener('click', () => vivaModal.classList.remove('hidden'));
        }
        if (vivaClose && vivaModal) {
            vivaClose.addEventListener('click', () => vivaModal.classList.add('hidden'));
        }
    }

    ensureExtendedLevels() {
        const grid = document.querySelector('.levels-grid');
        if (!grid) return;

        // When all 20 levels are completed, dynamically add next levels (21, 22, 23... up to 100)
        const targetMax = Math.min(100, Math.max(20, this.unlockedLevel));
        for (let lvl = 21; lvl <= targetMax; lvl++) {
            if (!document.querySelector(`.level-card[data-level="${lvl}"]`)) {
                const card = document.createElement('div');
                card.className = 'level-card locked';
                card.dataset.level = lvl;
                const name = this.getLevelName(lvl);
                const tier = Math.ceil(lvl / 20);
                const desc = `Tier ${tier} Seismic Gauntlet - Evacuate to high-altitude shelter under intense magnitude shocks.`;
                card.innerHTML = `
                    <div class="lvl-card-top">
                        <span class="lvl-badge">LEVEL ${lvl}</span>
                        <span class="lvl-status-badge locked">🔒 LOCKED</span>
                    </div>
                    <h3 class="lvl-title">${name}</h3>
                    <p class="lvl-desc">${desc}</p>
                `;
                grid.appendChild(card);
            }
        }
    }

    selectLevel(lvl) {
        this.selectedLevel = lvl;
        const levelCards = document.querySelectorAll('.level-card');
        levelCards.forEach(c => {
            if (parseInt(c.dataset.level, 10) === lvl) {
                c.classList.add('selected');
            } else {
                c.classList.remove('selected');
            }
        });
        this.updateStartButtonLabel();
    }

    updateLevelProgressionUI() {
        this.ensureExtendedLevels();

        const levelCards = document.querySelectorAll('.level-card');
        levelCards.forEach(card => {
            const lvl = parseInt(card.dataset.level, 10);
            const statusBadge = card.querySelector('.lvl-status-badge');

            if (lvl <= this.unlockedLevel) {
                card.classList.remove('locked');
                if (this.completedLevels.has(lvl)) {
                    if (statusBadge) {
                        statusBadge.className = 'lvl-status-badge completed';
                        statusBadge.textContent = '✔ CLEARED';
                    }
                } else {
                    if (statusBadge) {
                        statusBadge.className = 'lvl-status-badge unlocked';
                        statusBadge.textContent = 'UNLOCKED';
                    }
                }
            } else {
                card.classList.add('locked');
                if (statusBadge) {
                    statusBadge.className = 'lvl-status-badge locked';
                    statusBadge.textContent = '🔒 LOCKED';
                }
            }
        });

        const progressPill = document.getElementById('progress-summary-badge');
        if (progressPill) {
            progressPill.textContent = `Progression: Level ${this.unlockedLevel} / 100 Unlocked`;
        }
    }

    updateStartButtonLabel() {
        const startLabel = document.getElementById('btn-start-label');
        if (startLabel) {
            const intObj = this.intensityData[this.selectedIntensity] || this.intensityData.medium;
            const lvlName = this.getLevelName(this.selectedLevel);
            startLabel.textContent = `START ${lvlName.toUpperCase()} (${intObj.name.toUpperCase()} ${intObj.richter.toFixed(1)} Mw)`;
        }
    }

    openLevelSelectMenu() {
        document.getElementById('game-hud').classList.add('hidden');
        document.getElementById('start-screen').classList.remove('hidden');
        this.updateLevelProgressionUI();
        this.selectLevel(this.selectedLevel);
        this.updateStartButtonLabel();
    }

    setStartCallback(cb) { this.onStartGameCallback = cb; }
    setRestartCallback(cb) { this.onRestartCallback = cb; }

    showWarningBanner(text = "EARTHQUAKE IMMINENT - EVACUATE TO SAFE ZONE!") {
        const banner = document.getElementById('warning-banner');
        if (banner) {
            banner.textContent = `⚠️ ${text} ⚠️`;
            banner.classList.remove('hidden');
            banner.classList.add('flash-animation');
        }
    }

    showAlertNotification(msg) {
        const banner = document.getElementById('hud-alert-banner');
        if (banner) {
            banner.textContent = msg;
            banner.classList.remove('hidden');
            clearTimeout(this.alertTimeout);
            this.alertTimeout = setTimeout(() => {
                banner.classList.add('hidden');
            }, 3200);
        }
    }

    hideWarningBanner() {
        const banner = document.getElementById('warning-banner');
        if (banner) {
            banner.classList.add('hidden');
            banner.classList.remove('flash-animation');
        }
    }

    updateHUD(timeLeft, maxTime, stage, richter, distanceToSafe, stamina, cameraMode, player = null, safeZoneCenter = null, routeName = null) {
        // Timer display
        const timerEl = document.getElementById('hud-timer-val');
        const timerBar = document.getElementById('hud-timer-bar');
        if (timerEl) {
            const secs = Math.max(0, Math.ceil(timeLeft));
            timerEl.textContent = `${secs}s`;
            if (secs <= 10) {
                timerEl.classList.add('urgent');
            } else {
                timerEl.classList.remove('urgent');
            }
        }
        if (timerBar) {
            const pct = Math.max(0, Math.min(100, (timeLeft / maxTime) * 100));
            timerBar.style.width = `${pct}%`;
            if (pct < 25) {
                timerBar.style.background = '#ef4444';
            } else if (pct < 50) {
                timerBar.style.background = '#f59e0b';
            } else {
                timerBar.style.background = '#22c55e';
            }
        }

        // Stage indicator
        const stageEl = document.getElementById('hud-stage-val');
        if (stageEl) {
            stageEl.textContent = stage;
            if (stage.includes('QUAKE') || stage.includes('ESCAPE')) {
                stageEl.style.color = '#ef4444';
            } else if (stage.includes('WARNING')) {
                stageEl.style.color = '#f59e0b';
            } else {
                stageEl.style.color = '#38bdf8';
            }
        }

        // Richter intensity
        const richterEl = document.getElementById('hud-richter-val');
        if (richterEl) {
            richterEl.textContent = `${richter.toFixed(1)} Mw`;
        }

        // Distance to safe zone
        const distEl = document.getElementById('hud-distance-val');
        if (distEl) {
            distEl.textContent = `${Math.round(distanceToSafe)}m`;
        }

        // Stamina bar
        const stamBar = document.getElementById('hud-stamina-bar');
        if (stamBar) {
            stamBar.style.width = `${stamina}%`;
        }

        // Camera mode indicator
        const camBtn = document.getElementById('btn-hud-cam');
        if (camBtn) {
            camBtn.textContent = `📷 View: ${cameraMode} [V]`;
        }

        // Floating Waypoint Pill Direction Guidance
        if (player && safeZoneCenter) {
            this.updateWaypointPill(player, safeZoneCenter, distanceToSafe, routeName);
        }
    }

    updateWaypointPill(player, safeZoneCenter, distanceToSafe, routeName = null) {
        const pill = document.getElementById('hud-waypoint-pill');
        const textEl = document.getElementById('hud-waypoint-text');
        const arrowEl = document.getElementById('hud-waypoint-arrow');
        if (!pill || !player || !safeZoneCenter) return;

        const dist = Math.round(distanceToSafe);
        if (textEl) {
            textEl.textContent = routeName ? `${routeName.toUpperCase()}: ${dist}m` : `SAFE ZONE: ${dist}m`;
        }

        // Calculate relative yaw angle from player forward to safe zone
        const dx = safeZoneCenter.x - player.position.x;
        const dz = safeZoneCenter.z - player.position.z;
        const targetYaw = Math.atan2(-dx, -dz);
        let diff = targetYaw - player.yaw;

        // Normalize diff to [-PI, PI]
        while (diff > Math.PI) diff -= Math.PI * 2;
        while (diff < -Math.PI) diff += Math.PI * 2;

        const deg = (diff * 180 / Math.PI);
        let arrowSymbol = '⬆';
        if (Math.abs(deg) < 22.5) {
            arrowSymbol = '⬆'; // Straight ahead
        } else if (deg >= 22.5 && deg < 67.5) {
            arrowSymbol = '↖'; // Ahead Left
        } else if (deg >= 67.5 && deg < 112.5) {
            arrowSymbol = '⬅'; // Turn Left
        } else if (deg >= 112.5 && deg < 157.5) {
            arrowSymbol = '↙'; // Behind Left
        } else if (deg <= -22.5 && deg > -67.5) {
            arrowSymbol = '↗'; // Ahead Right
        } else if (deg <= -67.5 && deg > -112.5) {
            arrowSymbol = '➡'; // Turn Right
        } else if (deg <= -112.5 && deg > -157.5) {
            arrowSymbol = '↘'; // Behind Right
        } else {
            arrowSymbol = '⬇'; // Turn Around
        }

        if (arrowEl) {
            arrowEl.textContent = arrowSymbol;
        }

        if (dist <= 15) {
            pill.classList.add('imminent');
        } else {
            pill.classList.remove('imminent');
        }
    }

    renderMinimap(player, safeZoneCenter, obstacles = []) {
        if (!this.minimapCtx || !this.minimapCanvas || !player) return;
        const ctx = this.minimapCtx;
        const w = this.minimapCanvas.width;
        const h = this.minimapCanvas.height;
        const cx = w / 2;
        const cy = h / 2;
        const radarRange = 80; // Distance in meters shown on radar
        const scale = (w / 2) / radarRange;

        // Clear radar
        ctx.clearRect(0, 0, w, h);

        // Circular background
        ctx.fillStyle = 'rgba(15, 23, 42, 0.90)';
        ctx.beginPath();
        ctx.arc(cx, cy, cx - 2, 0, Math.PI * 2);
        ctx.fill();
        ctx.strokeStyle = '#38bdf8';
        ctx.lineWidth = 1.5;
        ctx.stroke();

        // Compass Cardinal Ticks (N, S, E, W)
        ctx.fillStyle = 'rgba(148, 163, 184, 0.7)';
        ctx.font = 'bold 8px monospace';
        ctx.textAlign = 'center';
        ctx.fillText('N', cx, 11);
        ctx.fillText('S', cx, h - 4);
        ctx.fillText('W', 8, cy + 3);
        ctx.fillText('E', w - 8, cy + 3);

        // Range rings (25m, 50m, 75m)
        [25, 50, 75].forEach(r => {
            ctx.beginPath();
            ctx.arc(cx, cy, r * scale, 0, Math.PI * 2);
            ctx.strokeStyle = 'rgba(71, 85, 105, 0.4)';
            ctx.lineWidth = 1;
            ctx.stroke();
        });

        // Safe Zone Waypoint & Guiding Dashed Path
        if (safeZoneCenter) {
            const dx = (safeZoneCenter.x - player.position.x) * scale;
            const dz = (safeZoneCenter.z - player.position.z) * scale;
            const distFromCenter = Math.sqrt(dx * dx + dz * dz);

            // Clamp to edge if outside radar view
            let drawX = cx + dx;
            let drawY = cy + dz;
            if (distFromCenter > (cx - 10)) {
                const angle = Math.atan2(dz, dx);
                drawX = cx + Math.cos(angle) * (cx - 10);
                drawY = cy + Math.sin(angle) * (cy - 10);
            }

            // Green dashed guidance line from player to safe zone
            ctx.strokeStyle = 'rgba(74, 222, 128, 0.45)';
            ctx.lineWidth = 1.5;
            ctx.setLineDash([3, 3]);
            ctx.beginPath();
            ctx.moveTo(cx, cy);
            ctx.lineTo(drawX, drawY);
            ctx.stroke();
            ctx.setLineDash([]);

            // Draw glowing green safe zone icon
            ctx.fillStyle = '#22c55e';
            ctx.shadowColor = '#4ade80';
            ctx.shadowBlur = 8;
            ctx.beginPath();
            ctx.arc(drawX, drawY, 6, 0, Math.PI * 2);
            ctx.fill();
            ctx.shadowBlur = 0;

            ctx.fillStyle = '#ffffff';
            ctx.font = 'bold 9px sans-serif';
            ctx.textAlign = 'center';
            ctx.fillText('EXIT', drawX, drawY - 8);
        }

        // Active Fallen Debris Hazard Markers (Red Dots)
        if (obstacles && obstacles.length > 0) {
            ctx.fillStyle = '#ef4444';
            obstacles.forEach(obs => {
                const ox = cx + (obs.position.x - player.position.x) * scale;
                const oz = cy + (obs.position.z - player.position.z) * scale;
                if (ox >= 0 && ox <= w && oz >= 0 && oz <= h) {
                    ctx.beginPath();
                    ctx.arc(ox, oz, 3.5, 0, Math.PI * 2);
                    ctx.fill();
                }
            });
        }

        // Player icon in center (Cyan arrow pointing in player yaw direction)
        // Correct alignment: -player.yaw points arrow exactly where player looks!
        ctx.save();
        ctx.translate(cx, cy);
        ctx.rotate(-player.yaw);

        ctx.fillStyle = '#38bdf8';
        ctx.beginPath();
        ctx.moveTo(0, -7);
        ctx.lineTo(5, 6);
        ctx.lineTo(0, 3);
        ctx.lineTo(-5, 6);
        ctx.closePath();
        ctx.fill();
        ctx.restore();
    }

    _computeStars(score, won) {
        if (!won) return 0;
        if (score >= 8000) return 3;
        if (score >= 4500) return 2;
        return 1;
    }

    _applyStars(starCount) {
        // star-box-1, star-box-2, star-box-3
        for (let i = 1; i <= 3; i++) {
            const box = document.getElementById(`star-box-${i}`);
            if (!box) continue;
            const svg = box.querySelector('.star-svg');
            if (i <= starCount) {
                box.classList.remove('star-empty');
                box.classList.add('star-earned');
                // Re-trigger animation by briefly removing and re-adding class
                if (svg) {
                    svg.style.animationDelay = `${(i - 1) * 0.22}s`;
                    svg.style.animation = 'none';
                    svg.offsetHeight; // reflow
                    svg.style.animation = '';
                }
            } else {
                box.classList.remove('star-earned');
                box.classList.add('star-empty');
                if (svg) {
                    svg.style.animationDelay = '0s';
                    svg.style.animation = 'none';
                }
            }
        }

        // Label
        const labelEl = document.getElementById('modal-stars-label');
        const tableStarsEl = document.getElementById('modal-table-stars-val');
        const criteriaEl = document.getElementById('modal-stars-criteria');
        const scoreContainer = document.getElementById('modal-stars-container');

        const labelMap = {
            0: '💀 0 STARS — MISSION FAILED',
            1: '⭐ 1 STAR — MISSION CLEARED',
            2: '⭐⭐ 2 STARS — GREAT PERFORMANCE!',
            3: '⭐⭐⭐ 3 STARS — HIGH SCORE EXCELLENCE!'
        };
        const criteriaMap = {
            0: 'Reach the safe zone to earn stars',
            1: 'Score ≥ 1 earns 1 Star — aim for 4,500+ for 2 Stars',
            2: 'Score ≥ 4,500 earns 2 Stars — reach 8,000+ for 3 Stars',
            3: 'Score ≥ 8,000 — Maximum excellence achieved!'
        };

        if (labelEl)    labelEl.textContent   = labelMap[starCount];
        if (tableStarsEl) tableStarsEl.textContent = `${'⭐'.repeat(starCount) || '—'} (${starCount} / 3 Stars)`;
        if (criteriaEl) criteriaEl.textContent = criteriaMap[starCount];
        if (scoreContainer) {
            scoreContainer.classList.remove('hidden');
        }
    }

    _applyScoreBadge(score, won) {
        const badgeEl = document.getElementById('modal-score-badge');
        if (!badgeEl) return;
        if (!won) {
            badgeEl.textContent = '💀 MISSION FAILED';
            badgeEl.className = 'score-tier-badge badge-fail';
            return;
        }
        if (score >= 8000) {
            badgeEl.textContent = '🏆 HIGH SCORE ACHIEVED';
            badgeEl.className = 'score-tier-badge badge-gold';
        } else if (score >= 4500) {
            badgeEl.textContent = '🥈 GREAT SCORE';
            badgeEl.className = 'score-tier-badge badge-silver';
        } else {
            badgeEl.textContent = '🥉 LEVEL CLEARED';
            badgeEl.className = 'score-tier-badge badge-bronze';
        }
    }

    showResultModal(won, timeSpent, timeLeft, level, finalScore, richterVal = null) {
        const modal = document.getElementById('result-modal');
        const titleEl = document.getElementById('modal-title');
        const subtitleEl = document.getElementById('modal-subtitle');
        const badgeEl = document.getElementById('modal-badge');
        const unlockBanner = document.getElementById('unlock-alert-banner');
        const scoreEl = document.getElementById('modal-score-val');
        const timeEl = document.getElementById('modal-time-val');
        const intensityEl = document.getElementById('modal-intensity-val');
        const rankEl = document.getElementById('modal-rank-val');
        const nextBtn = document.getElementById('btn-modal-next');

        if (won) {
            // Register level completion
            this.completedLevels.add(level);
            localStorage.setItem('earthquake_completed_levels', JSON.stringify([...this.completedLevels]));

            // Unlock next level if not already unlocked
            let newlyUnlocked = false;
            if (level >= this.unlockedLevel && this.unlockedLevel < 100) {
                this.unlockedLevel = level + 1;
                localStorage.setItem('earthquake_unlocked_level', this.unlockedLevel);
                newlyUnlocked = true;
            }

            titleEl.textContent = 'SAFE ZONE REACHED!';
            titleEl.style.color = '#22c55e';
            if (subtitleEl) {
                const lvlName = this.getLevelName(level);
                subtitleEl.textContent = `Level ${level} (${lvlName}) Completed Successfully`;
            }

            badgeEl.textContent = '🎉 MISSION SUCCESSFUL';
            badgeEl.className = 'result-badge success';

            if (unlockBanner) {
                if (level < 100) {
                    if (level === 20) {
                        unlockBanner.textContent = '✨ ALL 20 INITIAL LEVELS COMPLETED! LEVEL 21 UNLOCKED (TIER 2 ACTIVE UP TO LEVEL 100)! ✨';
                    } else {
                        const nextName = this.getLevelName(level + 1);
                        unlockBanner.textContent = `✨ LEVEL ${level + 1}: ${nextName.toUpperCase()} UNLOCKED! ✨`;
                    }
                    unlockBanner.classList.remove('hidden');
                } else {
                    unlockBanner.textContent = '🏆 ALL 100 MISSIONS CONQUERED! GRAND TITAN SURVIVOR! 🏆';
                    unlockBanner.classList.remove('hidden');
                }
            }

            if (nextBtn) {
                nextBtn.classList.remove('hidden');
                if (level < 100) {
                    nextBtn.innerHTML = `<span>Select Level ${level + 1}</span> <span>➔</span>`;
                } else {
                    nextBtn.innerHTML = `<span>Replay Campaign</span> <span>➔</span>`;
                }
            }

            let rank = 'C';
            if (timeLeft > 25) rank = 'S';
            else if (timeLeft > 15) rank = 'A';
            else rank = 'B';
            rankEl.textContent = rank;
        } else {
            titleEl.textContent = 'EVACUATION FAILED';
            titleEl.style.color = '#ef4444';
            if (subtitleEl) {
                subtitleEl.textContent = `Could not reach safe zone before building collapse`;
            }
            badgeEl.textContent = '💀 TIME EXPIRED / CRUSHED';
            badgeEl.className = 'result-badge defeat';
            rankEl.textContent = 'F';

            if (unlockBanner) {
                unlockBanner.classList.add('hidden');
            }
            if (nextBtn) {
                nextBtn.classList.add('hidden');
            }
        }

        const intObj = this.intensityData[this.selectedIntensity] || this.intensityData.medium;
        const actualRichter = richterVal !== null ? richterVal : intObj.richter;
        if (intensityEl) {
            intensityEl.textContent = `${actualRichter.toFixed(1)} Mw (${intObj.name})`;
        }

        timeEl.textContent = `${timeSpent.toFixed(1)}s (Bonus Left: ${timeLeft.toFixed(1)}s)`;
        scoreEl.textContent = `${Math.round(finalScore).toLocaleString()} PTS`;

        // ---- Stars & Score Badge (score-based) ----
        const starCount = this._computeStars(Math.round(finalScore), won);
        this._applyStars(starCount);
        this._applyScoreBadge(Math.round(finalScore), won);

        // Hide stars container on defeat
        const starsContainer = document.getElementById('modal-stars-container');
        if (starsContainer) {
            if (won) {
                starsContainer.classList.remove('hidden');
            } else {
                starsContainer.classList.add('hidden');
            }
        }

        this.updateLevelProgressionUI();
        this.updateStartButtonLabel();

        modal.classList.remove('hidden');
    }
}

window.UIManager = UIManager;
