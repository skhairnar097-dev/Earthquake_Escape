// Earthquake Escape - Procedural Audio Engine
// Uses Web Audio API for 100% self-contained sound effects without external audio files.

class SoundEngine {
    constructor() {
        this.ctx = null;
        this.initialized = false;
        this.muted = false;
        this.masterGain = null;
        
        // Quake rumble audio nodes
        this.rumbleSource = null;
        this.rumbleGain = null;
        this.rumbleFilter = null;
        this.rumbleOsc = null;
        this.isRumbling = false;

        // Siren nodes
        this.sirenOsc = null;
        this.sirenGain = null;
        this.sirenLfo = null;
        this.isSirening = false;
    }

    init() {
        if (this.initialized) return;
        try {
            const AudioContext = window.AudioContext || window.webkitAudioContext;
            this.ctx = new AudioContext();
            this.masterGain = this.ctx.createGain();
            this.masterGain.gain.setValueAtTime(0.7, this.ctx.currentTime);
            this.masterGain.connect(this.ctx.destination);
            this.initialized = true;
        } catch (e) {
            console.warn("Web Audio API not supported:", e);
        }
    }

    resume() {
        if (!this.initialized) this.init();
        if (this.ctx && this.ctx.state === 'suspended') {
            this.ctx.resume();
        }
    }

    toggleMute() {
        this.muted = !this.muted;
        if (this.masterGain && this.ctx) {
            this.masterGain.gain.setValueAtTime(this.muted ? 0 : 0.7, this.ctx.currentTime);
        }
        return this.muted;
    }

    // Helper: Create Brownian/Pink Noise buffer for seismic rumble and debris impacts
    createNoiseBuffer(seconds = 3) {
        if (!this.ctx) return null;
        const bufferSize = this.ctx.sampleRate * seconds;
        const buffer = this.ctx.createBuffer(1, bufferSize, this.ctx.sampleRate);
        const output = buffer.getChannelData(0);
        let lastOut = 0.0;
        for (let i = 0; i < bufferSize; i++) {
            const white = Math.random() * 2 - 1;
            // Brown noise integration
            output[i] = (lastOut + (0.02 * white)) / 1.02;
            lastOut = output[i];
            output[i] *= 3.5; // Gain boost
        }
        return buffer;
    }

    // Continuous Earthquake Ground Rumble with Sub-bass
    startEarthquakeRumble(intensity = 1.0) {
        if (!this.ctx || this.isRumbling || this.muted) return;
        this.resume();

        try {
            const noiseBuffer = this.createNoiseBuffer(4);
            if (!noiseBuffer) return;

            this.rumbleSource = this.ctx.createBufferSource();
            this.rumbleSource.buffer = noiseBuffer;
            this.rumbleSource.loop = true;

            // Lowpass filter for heavy ground tremor
            this.rumbleFilter = this.ctx.createBiquadFilter();
            this.rumbleFilter.type = 'lowpass';
            this.rumbleFilter.frequency.setValueAtTime(65 * intensity, this.ctx.currentTime);
            this.rumbleFilter.Q.setValueAtTime(4.0, this.ctx.currentTime);

            // Sub-bass sine oscillator for tactile deep vibration
            this.rumbleOsc = this.ctx.createOscillator();
            this.rumbleOsc.type = 'triangle';
            this.rumbleOsc.frequency.setValueAtTime(32, this.ctx.currentTime);

            // Sub-bass gain
            const subGain = this.ctx.createGain();
            subGain.gain.setValueAtTime(0.35 * intensity, this.ctx.currentTime);
            this.rumbleOsc.connect(subGain);

            this.rumbleGain = this.ctx.createGain();
            this.rumbleGain.gain.setValueAtTime(0.01, this.ctx.currentTime);
            this.rumbleGain.gain.exponentialRampToValueAtTime(Math.min(0.9, 0.5 * intensity), this.ctx.currentTime + 1.2);

            this.rumbleSource.connect(this.rumbleFilter);
            this.rumbleFilter.connect(this.rumbleGain);
            subGain.connect(this.rumbleGain);
            this.rumbleGain.connect(this.masterGain);

            this.rumbleSource.start();
            this.rumbleOsc.start();
            this.isRumbling = true;
        } catch (e) {
            console.warn("Rumble audio error:", e);
        }
    }

    updateEarthquakeIntensity(intensity) {
        if (!this.isRumbling || !this.rumbleGain || !this.rumbleFilter || !this.ctx) return;
        const targetGain = Math.min(1.0, 0.45 * intensity);
        const targetFreq = Math.min(160, 50 + intensity * 40);
        this.rumbleGain.gain.setTargetAtTime(targetGain, this.ctx.currentTime, 0.1);
        this.rumbleFilter.frequency.setTargetAtTime(targetFreq, this.ctx.currentTime, 0.1);
    }

    stopEarthquakeRumble() {
        if (!this.isRumbling) return;
        try {
            if (this.rumbleGain && this.ctx) {
                this.rumbleGain.gain.setTargetAtTime(0.001, this.ctx.currentTime, 0.5);
                setTimeout(() => {
                    if (this.rumbleSource) {
                        try { this.rumbleSource.stop(); } catch(e){}
                        this.rumbleSource.disconnect();
                    }
                    if (this.rumbleOsc) {
                        try { this.rumbleOsc.stop(); } catch(e){}
                        this.rumbleOsc.disconnect();
                    }
                    this.isRumbling = false;
                }, 600);
            } else {
                this.isRumbling = false;
            }
        } catch (e) {
            this.isRumbling = false;
        }
    }

    // Emergency Siren Alarm
    startSiren() {
        if (!this.ctx || this.isSirening || this.muted) return;
        this.resume();

        try {
            this.sirenOsc = this.ctx.createOscillator();
            this.sirenGain = this.ctx.createGain();
            this.sirenLfo = this.ctx.createOscillator();
            const lfoGain = this.ctx.createGain();

            this.sirenOsc.type = 'sawtooth';
            this.sirenOsc.frequency.setValueAtTime(650, this.ctx.currentTime);

            // Modulate pitch between 500Hz and 850Hz every 1.5 seconds
            this.sirenLfo.frequency.setValueAtTime(0.65, this.ctx.currentTime);
            lfoGain.gain.setValueAtTime(200, this.ctx.currentTime);

            this.sirenLfo.connect(lfoGain);
            lfoGain.connect(this.sirenOsc.frequency);

            // Bandpass filter to sound like an outdoor loud siren speaker
            const sirenFilter = this.ctx.createBiquadFilter();
            sirenFilter.type = 'bandpass';
            sirenFilter.frequency.setValueAtTime(700, this.ctx.currentTime);
            sirenFilter.Q.setValueAtTime(3.0, this.ctx.currentTime);

            this.sirenGain.gain.setValueAtTime(0.01, this.ctx.currentTime);
            this.sirenGain.gain.linearRampToValueAtTime(0.25, this.ctx.currentTime + 1.0);

            this.sirenOsc.connect(sirenFilter);
            sirenFilter.connect(this.sirenGain);
            this.sirenGain.connect(this.masterGain);

            this.sirenOsc.start();
            this.sirenLfo.start();
            this.isSirening = true;
        } catch (e) {
            console.warn("Siren error:", e);
        }
    }

    stopSiren() {
        if (!this.isSirening) return;
        try {
            if (this.sirenGain && this.ctx) {
                this.sirenGain.gain.setTargetAtTime(0.001, this.ctx.currentTime, 0.4);
                setTimeout(() => {
                    if (this.sirenOsc) {
                        try { this.sirenOsc.stop(); } catch(e){}
                        this.sirenOsc.disconnect();
                    }
                    if (this.sirenLfo) {
                        try { this.sirenLfo.stop(); } catch(e){}
                        this.sirenLfo.disconnect();
                    }
                    this.isSirening = false;
                }, 500);
            } else {
                this.isSirening = false;
            }
        } catch (e) {
            this.isSirening = false;
        }
    }

    // Concrete Debris Crash on Road
    playDebrisCrash(volume = 0.7) {
        if (!this.ctx || this.muted) return;
        this.resume();

        try {
            const buffer = this.createNoiseBuffer(0.8);
            if (!buffer) return;

            const noise = this.ctx.createBufferSource();
            noise.buffer = buffer;

            const filter = this.ctx.createBiquadFilter();
            filter.type = 'bandpass';
            filter.frequency.setValueAtTime(220, this.ctx.currentTime);
            filter.frequency.exponentialRampToValueAtTime(50, this.ctx.currentTime + 0.6);
            filter.Q.setValueAtTime(1.5, this.ctx.currentTime);

            const gain = this.ctx.createGain();
            gain.gain.setValueAtTime(0.6 * volume, this.ctx.currentTime);
            gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.7);

            // Sub-thud sine
            const thud = this.ctx.createOscillator();
            const thudGain = this.ctx.createGain();
            thud.frequency.setValueAtTime(85, this.ctx.currentTime);
            thud.frequency.exponentialRampToValueAtTime(28, this.ctx.currentTime + 0.4);
            thudGain.gain.setValueAtTime(0.8 * volume, this.ctx.currentTime);
            thudGain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.45);

            thud.connect(thudGain);
            thudGain.connect(this.masterGain);

            noise.connect(filter);
            filter.connect(gain);
            gain.connect(this.masterGain);

            noise.start();
            thud.start();
            thud.stop(this.ctx.currentTime + 0.5);
            noise.stop(this.ctx.currentTime + 0.8);
        } catch(e) {}
    }

    // Emergency Warning Beep
    playWarningBeep() {
        if (!this.ctx || this.muted) return;
        this.resume();

        try {
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();
            osc.type = 'square';
            osc.frequency.setValueAtTime(880, this.ctx.currentTime); // High alert A5
            gain.gain.setValueAtTime(0.2, this.ctx.currentTime);
            gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.22);

            osc.connect(gain);
            gain.connect(this.masterGain);
            osc.start();
            osc.stop(this.ctx.currentTime + 0.25);
        } catch (e) {}
    }

    // Footstep Sound
    playFootstep() {
        if (!this.ctx || this.muted) return;
        try {
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();
            osc.type = 'triangle';
            osc.frequency.setValueAtTime(90 + Math.random() * 30, this.ctx.currentTime);
            gain.gain.setValueAtTime(0.08, this.ctx.currentTime);
            gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.08);

            osc.connect(gain);
            gain.connect(this.masterGain);
            osc.start();
            osc.stop(this.ctx.currentTime + 0.09);
        } catch(e) {}
    }

    // Jump Whoosh
    playJump() {
        if (!this.ctx || this.muted) return;
        try {
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();
            osc.type = 'sine';
            osc.frequency.setValueAtTime(140, this.ctx.currentTime);
            osc.frequency.exponentialRampToValueAtTime(320, this.ctx.currentTime + 0.18);
            gain.gain.setValueAtTime(0.18, this.ctx.currentTime);
            gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.2);

            osc.connect(gain);
            gain.connect(this.masterGain);
            osc.start();
            osc.stop(this.ctx.currentTime + 0.22);
        } catch(e) {}
    }

    // Victory Fanfare
    playVictory() {
        if (!this.ctx || this.muted) return;
        this.resume();
        const notes = [261.63, 329.63, 392.00, 523.25, 659.25]; // C, E, G, C, E
        const now = this.ctx.currentTime;
        notes.forEach((freq, idx) => {
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();
            osc.type = 'triangle';
            osc.frequency.setValueAtTime(freq, now + idx * 0.14);
            gain.gain.setValueAtTime(0, now + idx * 0.14);
            gain.gain.linearRampToValueAtTime(0.3, now + idx * 0.14 + 0.04);
            gain.gain.exponentialRampToValueAtTime(0.001, now + idx * 0.14 + 0.6);

            osc.connect(gain);
            gain.connect(this.masterGain);
            osc.start(now + idx * 0.14);
            osc.stop(now + idx * 0.14 + 0.65);
        });
    }

    // Game Over Sound
    playGameOver() {
        if (!this.ctx || this.muted) return;
        this.resume();
        const now = this.ctx.currentTime;
        const osc = this.ctx.createOscillator();
        const gain = this.ctx.createGain();
        osc.type = 'sawtooth';
        osc.frequency.setValueAtTime(180, now);
        osc.frequency.exponentialRampToValueAtTime(45, now + 1.2);
        gain.gain.setValueAtTime(0.3, now);
        gain.gain.exponentialRampToValueAtTime(0.001, now + 1.3);

        const filter = this.ctx.createBiquadFilter();
        filter.type = 'lowpass';
        filter.frequency.setValueAtTime(250, now);

        osc.connect(filter);
        filter.connect(gain);
        gain.connect(this.masterGain);
        osc.start(now);
        osc.stop(now + 1.4);
    }

    // Rain Weather Ambiance (for stormy levels like Industrial Port)
    startRainAmbiance() {
        if (!this.ctx || this.isRaining || this.muted) return;
        this.resume();
        try {
            const buffer = this.createNoiseBuffer(3);
            if (!buffer) return;

            this.rainSource = this.ctx.createBufferSource();
            this.rainSource.buffer = buffer;
            this.rainSource.loop = true;

            this.rainFilter = this.ctx.createBiquadFilter();
            this.rainFilter.type = 'bandpass';
            this.rainFilter.frequency.setValueAtTime(1200, this.ctx.currentTime);
            this.rainFilter.Q.setValueAtTime(0.8, this.ctx.currentTime);

            this.rainGain = this.ctx.createGain();
            this.rainGain.gain.setValueAtTime(0.01, this.ctx.currentTime);
            this.rainGain.gain.linearRampToValueAtTime(0.18, this.ctx.currentTime + 1.0);

            this.rainSource.connect(this.rainFilter);
            this.rainFilter.connect(this.rainGain);
            this.rainGain.connect(this.masterGain);

            this.rainSource.start();
            this.isRaining = true;
        } catch(e) {}
    }

    stopRainAmbiance() {
        if (!this.isRaining) return;
        try {
            if (this.rainGain && this.ctx) {
                this.rainGain.gain.linearRampToValueAtTime(0.001, this.ctx.currentTime + 0.5);
                setTimeout(() => {
                    if (this.rainSource) {
                        try { this.rainSource.stop(); } catch(e){}
                        this.rainSource.disconnect();
                    }
                    this.isRaining = false;
                }, 600);
            }
        } catch(e) {
            this.isRaining = false;
        }
    }

    // Distant Thunder Rumble
    playThunder() {
        if (!this.ctx || this.muted) return;
        this.resume();
        try {
            const buffer = this.createNoiseBuffer(3.5);
            if (!buffer) return;

            const now = this.ctx.currentTime;
            const src = this.ctx.createBufferSource();
            src.buffer = buffer;

            const filter = this.ctx.createBiquadFilter();
            filter.type = 'lowpass';
            filter.frequency.setValueAtTime(180, now);
            filter.frequency.exponentialRampToValueAtTime(45, now + 2.8);

            const gain = this.ctx.createGain();
            gain.gain.setValueAtTime(0.01, now);
            gain.gain.linearRampToValueAtTime(0.4, now + 0.15);
            gain.gain.exponentialRampToValueAtTime(0.001, now + 3.0);

            src.connect(filter);
            filter.connect(gain);
            gain.connect(this.masterGain);

            src.start(now);
            src.stop(now + 3.2);
        } catch(e) {}
    }
}

window.soundEngine = new SoundEngine();
