# Gesture Glove – IMU-Based Motion Mouse

This project turns hand motion into mouse movement. An IMU (gyroscope) is mounted on a glove, and tilting or rotating your hand moves the cursor on your computer. Three physical buttons handle left, right, and middle clicks.

It runs on an Arduino Leonardo (ATmega32U4) and uses the board's native USB HID support to act as an actual mouse, no drivers or companion software needed on the computer side.

This was my first project combining sensor data, filtering, and USB HID output into a working physical device.

## Current Status

The project is complete and working. Tilting the glove moves the cursor, and all three buttons register clicks reliably.

It currently supports:

- Reading gyroscope data over I2C
- Converting raw sensor readings into smooth, usable motion
- Filtering out hand tremor so the cursor doesn't jitter at rest
- Scaling sensitivity so small movements are precise and fast movements are still fast
- Debounced button input for left, right, and middle click
- Serial output for debugging sensor values in real time

## Hardware and Tools

- Arduino Leonardo (ATmega32U4)
- MPU6050 IMU (gyroscope)
- 3 momentary pushbuttons (left / right / middle click)
- PlatformIO
- Visual Studio Code
- C++
- [Mouse.h](https://www.arduino.cc/reference/en/language/functions/usb/mouse/) (Arduino's built-in USB HID mouse library)
- [BareI2C](https://github.com/Yamin-Osman/BareMetalI2C.git) — my own register-level I2C driver, included locally in `lib/` and used here instead of `Wire.h`

## Why I Built It

I wanted a project that combined a few different things I was learning at the same time: reading real sensor data, cleaning up noisy signals, and making a microcontroller act as an actual USB input device instead of just printing numbers to a serial monitor.

I also wanted a real piece of hardware to test my BareI2C driver on, since that project was built and tested somewhat in isolation. Getting real gyroscope data out of the MPU6050 using my own driver instead of `Wire.h` was a good way to confirm it actually worked under real conditions.

## How It Works

The motion pipeline runs every loop:

```text
Read raw gyro data over I2C (via BareI2C)
→ Subtract per-axis offset (raw sensor bias)
→ Subtract neutral calibration value (resting drift)
→ Low-pass filter (smooths out jitter)
→ Deadzone filter (ignores tiny unintentional movement)
→ Rotation correction (compensates for how the sensor is mounted)
→ Dynamic sensitivity scaling (slow = precise, fast = responsive)
→ Mouse.move()
```

Buttons are read every loop too, with a simple time-based debounce (50 ms) so a single press doesn't register as multiple clicks.

## Calibration

Because every IMU has slightly different manufacturing bias, and every glove/mount is slightly misaligned, a few constants need tuning per physical build:

- **Axis offsets** — raw sensor bias, read directly from the IMU at rest
- **Neutral values** — leftover drift after offset correction, tuned so the cursor doesn't creep when the hand is still
- **Deadzone threshold** — how much motion is ignored as noise vs. treated as intentional
- **Rotation correction angle** — compensates for the sensor not being mounted perfectly flat/straight on the glove

These live as `constexpr` values at the top of `imu.cpp` and `cursor.cpp` so they're easy to find and re-tune.

## Project Structure

```
IMU/
├── include/
│   ├── imu.h
│   └── cursor.h
├── lib/
│   └── BareI2C/
│       ├── BareI2C.h
│       └── BareI2C.cpp
├── src/
│   ├── main.cpp       # setup/loop, button handling, ties everything together
│   ├── imu.cpp         # reads and filters gyro data
│   └── cursor.cpp       # converts filtered gyro data into mouse movement
├── test/
├── platformio.ini
├── .gitignore
└── README.md
```

This follows PlatformIO's standard project layout: shared headers live in `include/`, project-specific libraries live in `lib/`, and application source lives in `src/`.

The BareI2C driver source is included directly in this repo, under `lib/BareI2C/`. It started out as its own standalone project — the original repo (linked above) has the full write-up of how the driver itself works. A copy of the source lives here too, following PlatformIO's convention of keeping local libraries inside the project that uses them, so this repo builds on its own without needing an external dependency reference.

## What I Learned

- How to combine a physical sensor, a filtering pipeline, and USB HID output into one working device
- Complementary use of offset correction, low-pass filtering, and deadzones to turn noisy raw data into something usable
- Why sensitivity often needs to scale dynamically with input speed, rather than being one fixed number
- How small mounting misalignments show up as systematic error, and how a simple rotation correction can fix that
- Debouncing physical button input in a polling loop
- Using a driver I wrote myself as a real dependency in another project, instead of just testing it in isolation

## Limitations

- Sensitivity and calibration values are currently hardcoded and need to be manually retuned if the sensor is remounted
- No accelerometer fusion yet
- Only tested on ATmega32U4-based boards

## Future Work

- Add accelerometer fusion (complementary or Kalman filter) to reduce long-term drift
- Store calibration values in EEPROM instead of hardcoding them
- Add a small on/off or "click-to-recenter" gesture

## Author

Built by an incoming first-year engineering student as part of learning embedded systems, C++, sensor filtering, and USB HID device design.