# Outdoor Navigation Device

An EiE project using C, Zephyr RTOS, and the nRF52840 DK. The planned MVP receives phone position and a destination over BLE, calculates waypoint guidance, and displays direction and distance. The current firmware only prints a startup message and a heartbeat approximately once per second.

## Current development setup

This application currently reuses the existing EiE workspace. It is not yet a self-contained, pinned workspace. Keep this layout:

```text
zephyr-projects/
├── .west/
├── zephyr/
├── modules/hal/cmsis_6/
├── modules/hal/nordic/
├── eie_nrf52840/.venv/
└── outdoor-navigation-device/
```

The successful build reported macOS, Python 3.12.1, West 1.5.0, Zephyr 4.4.0, Zephyr SDK 1.0.1, GNU C 14.3.0, and CMake 4.4.3. These are observed versions, not dependency pins. A separate reproducible workspace is a future milestone; do not update the lesson workspace just to build this application.

## Build in VS Code

Open this repository folder. Press Cmd+Shift+B and choose **Navigation: Build** if prompted. The task directly invokes the existing EiE Python executable, so it does not depend on Conda activation or the selected terminal interpreter.

Tasks belong in `.vscode/tasks.json`. Editor settings belong in `.vscode/settings.json`. Use the Navigation task rather than the generic CMake build button.

The equivalent command, from this folder, is:

```sh
../eie_nrf52840/.venv/bin/python -m west build -b nrf52840dk/nrf52840 . -- "-DZEPHYR_MODULES=$PWD/../modules/hal/cmsis_6;$PWD/../modules/hal/nordic" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

If changing the board or clearing stale build configuration, add `-p always` after `build`. Routine builds can reuse the build directory.

## Flash and verify on hardware

Connect the DK through its debug/J-Link USB port and turn it on. Install the SEGGER J-Link tools if they are not already available. Run **Tasks: Run Task → Navigation: Flash**. This builds first and replaces the firmware currently on the board.

Equivalent flash command after a successful build:

```sh
../eie_nrf52840/.venv/bin/python -m west flash --runner jlink
```

Open the board's SEGGER serial port at 115200 baud, 8 data bits, no parity, 1 stop bit, and no flow control. Start monitoring and reset the board. Expected output:

```text
Outdoor navigation device started
Application running
Application running
```

Check several heartbeat messages and repeat after reset. Hardware flashing and this runtime test have not yet been verified in this project's recorded baseline.

## Editor configuration

The Microsoft C/C++ extension reads `build/compile_commands.json` to obtain the actual compiler flags and Zephyr include paths. Build first, then use **C/C++: Reset IntelliSense Database** if header diagnostics remain. The generated build directory stays out of Git.

The Python interpreter default is a convenience for the local layout. Existing interpreter selections can override it, and terminal activation remains unverified. Build and Flash tasks avoid that dependency by using an explicit executable path.

## Known setup issues

- The EiE workspace automatically discovers its lesson module, whose LED driver requires lesson-specific hardware definitions. Explicitly selecting CMSIS and Nordic modules fixed the starter application's build. Extend this selection deliberately if later features need more modules.
- Successful compilation does not establish successful flashing or runtime behavior.

## Next checkpoints

1. Verify flashing and repeatable serial output.
2. Record the result and commit the working baseline.
3. Pin dependency revisions and verify a separate clean workspace.
4. Implement and test simulated navigation before sensor and BLE integration.

Keep credentials, sensitive location data, and restricted third-party material out of the repository. Preserve repository privacy.
