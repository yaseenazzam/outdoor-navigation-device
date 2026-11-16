# Outdoor Navigation Device

An embedded navigation project developed through the Embedded in Embedded (EiE) program using the Nordic nRF52840 DK, C, and Zephyr RTOS.

The goal is a device that receives current position and a destination from a phone over Bluetooth Low Energy, calculates waypoint guidance, and displays direction and distance.

## Current Status

The project currently runs navigation calculations and a serial test harness on the nRF52840 DK using fixed, simulated inputs.

Implemented functionality includes:

- Coordinate validation.
- Shortest-turn heading error.
- Great-circle distance using the haversine formula.
- Initial bearing relative to true north.
- Input-error reporting for distance and bearing calculations.
- On-board tests with results printed over serial.

Build, flashing, and serial output have been verified.

## Planned MVP

- Receive live phone position and a destination over BLE.
- Measure and calibrate heading using a magnetometer, potentially combined with an IMU.
- Calculate navigation guidance on the nRF52840.
- Display direction, distance, and navigation status.
- Handle stale or invalid inputs, disconnection, and arrival.
- Demonstrate repeatable outdoor waypoint navigation.

The MVP provides straight-line waypoint guidance. Onboard GNSS, ANT/ANT+, a custom PCB, and additional feedback mechanisms are optional future enhancements.

## Hardware and Tools

| Component | Current selection |
|---|---|
| Development board | Nordic nRF52840 DK |
| Firmware language | C |
| RTOS | Zephyr |
| Editor | Visual Studio Code |
| Build orchestration | West and CMake |
| Flashing | SEGGER J-Link |
| Planned companion phone | iPhone 16 Pro Max |
| Heading sensor and display | To be selected |

Python runs the development tools on the computer. The compiled C firmware runs on the nRF52840.

## Project Structure

```text
outdoor-navigation-device/
├── .vscode/
│   ├── settings.json
│   └── tasks.json
├── src/
│   ├── main.c
│   ├── navigation.c
│   └── navigation.h
├── .gitignore
├── CMakeLists.txt
├── prj.conf
└── README.md
```

- `main.c` runs the current test harness.
- `navigation.c` implements validation and navigation calculations. It also currently contains a temporary validation test entry point.
- `navigation.h` declares the coordinate structure and public functions.
- `CMakeLists.txt` selects source files and integrates Zephyr.
- `prj.conf` configures Zephyr features.
- `.vscode/tasks.json` defines Build and Flash tasks.
- `.vscode/settings.json` configures editor support.

## Development Environment

The application currently reuses an existing EiE-managed Zephyr workspace:

```text
zephyr-projects/
├── .west/
├── zephyr/
├── modules/
│   └── hal/
│       ├── cmsis_6/
│       └── nordic/
├── eie_nrf52840/
│   └── .venv/
└── outdoor-navigation-device/
```

The successful development build reported:

| Tool | Version |
|---|---|
| Python | 3.12.1 |
| West | 1.5.0 |
| Zephyr | 4.4.0 |
| Zephyr SDK | 1.0.1 |
| GNU C compiler | 14.3.0 |
| CMake | 4.4.3 |

These are observed working versions. Dependency revisions are not yet pinned, and cloning this repository alone does not recreate the environment. Establishing a separate reproducible workspace is an outstanding milestone.

## Build

Open the repository folder in VS Code and press **Cmd+Shift+B**. Select **Navigation: Build** if prompted.

The task invokes the EiE virtual environment’s Python executable directly, avoiding dependence on terminal activation.

Alternatively, run this from the repository root:

```sh
../eie_nrf52840/.venv/bin/python -m west build \
  -b nrf52840dk/nrf52840 . -- \
  "-DZEPHYR_MODULES=$PWD/../modules/hal/cmsis_6;$PWD/../modules/hal/nordic" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

The explicit module selection prevents the shared workspace from importing EiE lesson drivers that require lesson-specific hardware definitions.

For a pristine rebuild, add `-p always` after `build`.

## Flash

Connect and power the DK through its debug/J-Link USB port.

In VS Code, select **Tasks: Run Task → Navigation: Flash**. This builds first and then programs the board.

Alternatively, after a successful build:

```sh
../eie_nrf52840/.venv/bin/python -m west flash --runner jlink
```

Flashing replaces the firmware currently on the board.

## Run the Tests

1. Flash the current firmware.
2. Open the board’s SEGGER serial port in Serial Monitor.
3. Configure 115200 baud, 8 data bits, no parity, 1 stop bit, and no flow control.
4. Start monitoring and press the board’s Reset button.
5. Check each test’s PASS/FAIL output.

Tests run once per reset. Inputs are hardcoded, and the board executes the calculations and compares results with expected values.

### Verified Checks

| Group | Checks | Coverage |
|---|---:|---|
| Coordinate validation | 3 | Valid origin, invalid latitude, invalid longitude |
| Distance | 3 | Identical positions, one degree east at the equator, invalid destination with unchanged output |
| Initial bearing | 3 | East at 90°, west at 270°, identical-position rejection with unchanged output |
| Heading error | 10 | Ordinary turns, north crossing, alignment, opposite headings, fractional angles |

The distance test expects approximately **111,195 metres** for one degree of longitude at the equator, with a tolerance of 1 metre.

The final `Result: 10/10 passed` message counts only the heading tests. The other checks print individual results.

## Navigation Conventions

- Coordinates use decimal degrees.
- Latitude must be finite and within `[-90, 90]`.
- Longitude must be finite and within `[-180, 180]`.
- Distance uses a spherical Earth model with radius **6,371,000 metres**.
- Initial bearing is measured clockwise from true north in `[0, 360)`.
- Positive heading error means turn right.
- Negative heading error means turn left.
- Exactly opposite headings consistently select a right turn of 180°.
- Distance and bearing functions return a Boolean success result and write through an output pointer.
- On reported failure, the output is left unchanged.

## Limitations and Outstanding Work

- Tests currently use simulated inputs. Live BLE location, heading sensors, and display integration are pending.
- The heading-error function assumes finite input angles in `[0, 360)`.
- Bearing rejects identical coordinate pairs. Equivalent geographic positions with different representations, polar cases, and antipodal ambiguity require further handling and tests.
- Non-finite coordinates, boundary values, and null output pointers need additional explicit runtime coverage.
- Magnetic heading must be reconciled with true bearing before sensor-based guidance.
- Arrival behavior, input freshness, position accuracy, and navigation states remain to be implemented.
- The temporary test harness will need a clearer separation from application behavior as the project grows.

## Development Approach

Development proceeds through small, testable increments with code review, on-board verification, and documentation of observed results.

AI assistance is used for architectural guidance, explanations, debugging, and review. The developer implements core navigation logic and verifies behavior on hardware.

The next milestone combines distance, initial bearing, and a simulated true heading into a single guidance result. Later milestones introduce application states, live phone data, calibrated heading, a display, and outdoor testing.
