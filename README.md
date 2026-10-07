# Block Start Timing System

A compact timing system for sprint block starts built around two Arduino boards communicating over HC-12 radio modules. The goal is to detect when an athlete is set and measure the time until the athlete leaves the block, making it useful for testing and teaching sprint start timing.

## Overview

This project combines:

- a start module that monitors the starting block state
- a finish module that measures elapsed time after the start signal
- optional sensor and communication variants for different setups
- a small computer-vision experiment for tracking knee motion and movement timing

The repository contains multiple Arduino sketches and PlatformIO projects, so it is best thought of as a collection of experimental configurations rather than a single fixed build.

## Features

- Wireless start signal between master and finish units
- Simple block-state detection with a digital input
- Time measurement and display on the finish unit
- Multiple variants for accelerometer, amplifier, Bluetooth, ultrasound, and bust-detection experiments
- Audio cues for start commands and timing signals
- OpenCV / MediaPipe motion analysis utilities for additional investigation

## Hardware concept

The basic system works like this:

1. The start board watches the block / trigger condition.
2. When the athlete is ready and the block changes state, the master sends a start signal over radio.
3. The finish board receives the signal and starts timing.
4. When the athlete leaves the block, the finish board measures the total time and displays it.

This is a lightweight, low-cost timing arrangement that can be adapted for different sensors and testing conditions.

## Project structure

- `src/` – Arduino sketches for the main timing system and several variants
  - `startMaster_Base/`
  - `finishSlave/`
  - other experimental boards such as `startMaster_Amplifier`, `startMaster_Accelerometer`, and `finishSlave_BustDetection`
- `src_platformIO/` – PlatformIO versions of the start and finish modules
- `openCV_kneeTracking/` – Python-based pose-tracking experiments for analysing movement
- `sounds/` – audio files used for start sequence cues
- `utils/` – helper tools and conversion utilities for audio/wave processing
- root PDFs – HC-12 data sheet and IAAF guidance materials

## Getting started

Choose the sketch that matches your hardware configuration and upload it to the corresponding Arduino board.

Typical workflow:

1. Connect the start module and finish module to their respective sensors and radio modules.
2. Upload the preferred sketch from `src/` or the PlatformIO project under `src_platformIO/`.
3. Verify wiring for the block trigger input, radio communication, and display.
4. Run the system and check timing output on the finish unit.

## Notes

- This repository contains several experimental variants rather than one final production design.
- Some folders are for testing and comparison, not all are meant to be used together.
- The OpenCV scripts are separate from the Arduino timing logic and are intended for motion-analysis experiments.

## References

- `HC-12-Datasheet.pdf` – radio module reference
- `AccuracyAssessmentGuideIAAF.pdf` – guidance related to timing and accuracy assessment

## License

This project does not appear to include a repository license file. If you plan to reuse or distribute it, check with the original author before publishing a modified version.
