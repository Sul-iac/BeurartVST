# BeurartVST

**BeurartVST** is an audio plug‑in (VST3) written in C++ with the JUCE framework. It was built to control a tube pre‑amplifier over a serial (UART) link, merging the character of analog valves with the convenience of digital automation. The hardware is no longer available, so the code is released **as‑is** for anyone who wants to study or repurpose it.

## Introduction
BeurartVST began life as the software front‑end for a hybrid tube preamp. The plug‑in exposes the preamp’s analog parameters—power supply voltage and tube gain—so they can be automated, saved in presets and recalled directly from a DAW. Because the physical unit is discontinued, this repository now serves as a technical reference and a starting point for other hardware‑in‑the‑loop projects.
**Other files, such has the source code of the inital micro controller will not be shared in this repo**. You may ask it if you want by contacting me, and I will answer to all your questions with pleasure.
## Technical Overview
- **JUCE framework** – Provides the GUI, plug‑in shell and parameter management (Windows build).
- **VST3 format** – Loads in any VST3‑compatible DAW; no DSP processing is done in software—the plug‑in is purely a control surface.
- **Serial communication (UART)** – Commands are sent to the preamp’s microcontroller via USB‑to‑UART to adjust voltages, bias and gain.
- **C++ multithreading** – A dedicated thread handles serial I/O to keep the GUI and host audio thread responsive.
- **`juce_serialport` module** – Third‑party JUCE module bundled in the repo for cross‑platform COM‑port access on Windows.

## Who Is It For?
- Audio engineers and advanced musicians working with analog outboard gear.
- Developers looking for a concrete example of a plug‑in controlling external hardware.
- DIY builders who want to adapt the code to their own tube circuits, guitar pedals, or other analog devices.

## Installation
> **Heads‑up:** without the original preamp, the plug‑in opens fine but will not affect audio.

1. **Clone the repo**
   ```bash
   git clone https://github.com/Sul-iac/BeurartVST.git
   ```
2. **Open the JUCE project** – Launch `Beurart_V1.jucer` in Projucer (JUCE 6+ recommended).
3. **Check modules** – Ensure the bundled `juce_serialport` module and the VST3 SDK path are set in Projucer.
4. **Export & build** – Export to Visual Studio (or another IDE) and build the *VST3* or *Standalone* target.
5. **Install the plug‑in** – Copy the resulting `BeurartVST.vst3` to your VST3 plug‑in folder and rescan in your DAW.

*Default COM‑port* is hard‑coded to `COM3` (or the first detected port). Change this in the source or add a port selector if you use different hardware.

## Features
- **Tube power control** – Adjust the tube’s supply voltage in real time for different headroom/saturation profiles.
- **Tube gain control** – Drive the tube harder or softer from the GUI without touching the hardware knobs.
- **Full DAW integration** – Automate parameters, map them to MIDI controllers, and store them in session presets.
- **Custom UI** – A straightforward GUI with virtual knobs reflecting the hardware’s controls.

## Code Structure
```
Source/
 ├─ GUI/          // JUCE components for the editor window
 ├─ Serial/       // SerialDevice, command protocol, thread management
 ├─ Main.cpp      // JUCEApplication entry point (standalone) / plug‑in setup
 └─ JuceLibraryCode/ // Auto‑generated JUCE wrappers and modules (incl. juce_serialport)
```
GUI and hardware layers are kept separate: you can swap out the serial backend (e.g. MIDI, OSC) or redesign the interface without rewriting the whole project.

---

*Bridging analog tone and digital workflow—have fun!*

