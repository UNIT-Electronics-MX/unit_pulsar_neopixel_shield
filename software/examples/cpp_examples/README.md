# Interactive 8x32 RGB LED Matrix Test

This project provides an interactive serial test program for an 8x32 RGB LED matrix based on WS2812B-compatible addressable LEDs. It uses the `Adafruit_NeoPixel` library to control a total of 257 LEDs: one indicator LED and 256 matrix LEDs.

The main purpose of this code is to verify the correct operation, wiring, indexing, and physical LED arrangement of an 8x32 RGB LED matrix. Through the Serial Monitor, the user can turn on individual LEDs, complete rows, ranges of LEDs, run sequences, and perform visual verification tests.

---

## Features

* Controls an 8x32 RGB LED matrix.
* Supports 256 independently addressable matrix LEDs.
* Uses LED 0 as a status indicator.
* Allows interactive control through the Serial Monitor.
* Provides commands to test individual LEDs, rows, ranges, and full-matrix lighting.
* Includes a verification routine for checking the physical position of LEDs 9 to 16.
* Helps identify LED indexing issues and matrix wiring direction.
* Keeps the indicator LED enabled while testing the matrix.

---

## Hardware Configuration

The system is configured for the following LED layout:

| Element             | Description                 |
| ------------------- | --------------------------- |
| Indicator LED       | LED 0                       |
| Matrix LEDs         | LEDs 1 to 256               |
| Matrix size         | 8 columns x 32 rows         |
| Data pin            | Pin 8                       |
| LED type            | WS2812B-compatible RGB LEDs |
| Color order         | GRB                         |
| Communication speed | 800 kHz                     |

The matrix is treated as a linear LED strip, where LED 0 is reserved as an indicator and the actual matrix starts at LED 1.

---

## Required Library

This project requires the following Arduino library:

```cpp
#include <Adafruit_NeoPixel.h>
```

You can install it from the Arduino IDE:

1. Open the Arduino IDE.
2. Go to **Sketch > Include Library > Manage Libraries**.
3. Search for **Adafruit NeoPixel**.
4. Install the library by Adafruit.

---

## Main Configuration

```cpp
#define PIN        8
#define NUM_LEDS   257
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 32
```

Where:

* `PIN` defines the microcontroller pin connected to the LED matrix data input.
* `NUM_LEDS` is set to `257` because the system uses 1 indicator LED plus 256 matrix LEDs.
* `MATRIX_WIDTH` defines the number of columns in the matrix.
* `MATRIX_HEIGHT` defines the number of rows in the matrix.

---

## LED Indexing

The LED indexing is organized as follows:

* `LED 0`: status indicator.
* `LEDs 1-256`: RGB LED matrix.

The first row of the matrix uses LEDs 1 to 8.

The second row uses LEDs 9 to 16.

The third row uses LEDs 17 to 24.

This pattern continues until LED 256.

| Matrix Row | LED Range    |
| ---------- | ------------ |
| Row 0      | LEDs 1-8     |
| Row 1      | LEDs 9-16    |
| Row 2      | LEDs 17-24   |
| ...        | ...          |
| Row 31     | LEDs 249-256 |

---

## Serial Monitor Configuration

To use the program, open the Serial Monitor with the following settings:

```text
Baud rate: 115200
Line ending: Newline
```

After uploading the code, the Serial Monitor will display an interactive menu with the available commands.

---

## Available Commands

| Command          | Description                              | Example             |
| ---------------- | ---------------------------------------- | ------------------- |
| `L<num>`         | Turns on an individual LED               | `L1`, `L64`, `L256` |
| `F<num>`         | Turns on a full row                      | `F0`, `F1`, `F31`   |
| `C`              | Clears all matrix LEDs                   | `C`                 |
| `T`              | Runs a four-point reference test         | `T`                 |
| `A`              | Turns on all matrix LEDs in red          | `A`                 |
| `S`              | Runs a sequential LED test from 1 to 256 | `S`                 |
| `R<start>-<end>` | Turns on a range of LEDs                 | `R1-8`, `R20-40`    |
| `V`              | Verifies LEDs 9 to 16 one by one         | `V`                 |
| `M`              | Displays the command menu again          | `M`                 |

---

## Command Descriptions

### Individual LED Test

The `L<num>` command turns on a single LED in red.

Example:

```text
L64
```

This turns on physical LED 64.

Valid range:

```text
0 to 256
```

Although LED 0 can be addressed, it is mainly used as the indicator LED.

---

### Row Test

The `F<num>` command turns on an entire matrix row.

Example:

```text
F0
```

This turns on row 0, corresponding to LEDs 1 to 8.

The row uses different colors to identify the beginning and end of the row:

* First LED: red.
* Middle LEDs: blue.
* Last LED: green.

This helps verify the physical orientation of each row.

Valid row range:

```text
0 to 31
```

---

### Clear LEDs

The `C` command turns off all matrix LEDs while keeping LED 0 enabled as a green indicator.

Example:

```text
C
```

---

### Four-Point Reference Test

The `T` command turns on four reference LEDs:

| LED    | Color  | Purpose        |
| ------ | ------ | -------------- |
| LED 1  | Red    | Start of row 0 |
| LED 8  | Green  | End of row 0   |
| LED 9  | Blue   | Start of row 1 |
| LED 16 | Yellow | End of row 1   |

This test is useful for checking the wiring direction and identifying whether the matrix follows a straight or zigzag arrangement.

Expected reference pattern:

```text
Row 0: LED 1 to LED 8
Row 1: LED 9 to LED 16
```

If the physical position does not match the expected pattern, the matrix may be wired in a zigzag layout or have a different physical LED order.

---

### Turn On All LEDs

The `A` command turns on all 256 matrix LEDs in red.

Example:

```text
A
```

This is useful for checking whether all LEDs are working correctly.

LED 0 remains enabled as a green indicator.

---

### Sequential Test

The `S` command runs a sequence from LED 1 to LED 256.

Example:

```text
S
```

Each LED turns on individually in red for 200 milliseconds.

This test helps verify:

* LED order.
* Missing LEDs.
* Incorrect wiring.
* Physical matrix direction.
* Damaged or disconnected LEDs.

To stop the sequence, send any character through the Serial Monitor.

---

### Range Test

The `R<start>-<end>` command turns on a range of LEDs in red.

Example:

```text
R1-8
```

This turns on LEDs 1 through 8.

Another example:

```text
R129-256
```

This turns on the second half of the matrix.

Valid range:

```text
1 to 256
```

---

### LED Position Verification

The `V` command verifies LEDs 9 to 16 one by one.

Example:

```text
V
```

The program turns on each LED individually and waits for the user to press Enter before continuing.

This test is useful when checking whether the second row of the matrix is physically arranged as expected.

The expected LED order for row 1 may depend on the matrix wiring. In a zigzag layout, the physical direction of every other row may be reversed.

---

## Indicator LED

LED 0 is used as a status indicator.

During setup, LED 0 is turned on with a low-intensity green color:

```cpp
strip.setPixelColor(0, strip.Color(0, 50, 0));
```

This allows the user to verify that the strip is powered and that the program is running.

The indicator LED remains on during most test operations.

---

## Brightness

The brightness is configured with:

```cpp
strip.setBrightness(64);
```

This sets the brightness to a moderate level to reduce current consumption and avoid excessive brightness during testing.

The brightness value can be adjusted from:

```text
0 to 255
```

A higher value increases brightness but also increases current consumption.

---

## Recommended Power Considerations

When turning on all 256 RGB LEDs, especially at high brightness, the matrix may require significant current. For testing purposes, the brightness is limited to 64.

For stable operation:

* Use an external power supply if required.
* Make sure the LED matrix ground and microcontroller ground are connected.
* Avoid powering a large LED matrix directly from a weak USB port.
* Use proper wire thickness for the LED power lines.
* Consider adding a capacitor across the LED power input.

---

## Troubleshooting

### Nothing turns on

Check the following:

* The data pin is connected to pin 8.
* The LED matrix is powered correctly.
* The ground of the microcontroller and the matrix are connected together.
* The correct LED type and color order are selected.
* The Adafruit NeoPixel library is installed.

---

### Colors appear incorrect

The code uses:

```cpp
NEO_GRB + NEO_KHZ800
```

If the colors do not match, the matrix may use a different color order such as RGB instead of GRB.

Try changing:

```cpp
NEO_GRB
```

to:

```cpp
NEO_RGB
```

---

### LED order does not match the expected position

If the LEDs turn on in a different physical order, the matrix may use a zigzag arrangement.

Use the following commands to diagnose the layout:

```text
T
V
S
F0
F1
```

These commands help identify the real physical order of the LEDs.

---

### Some LEDs do not turn on

Possible causes:

* Incorrect soldering.
* Damaged LED.
* Insufficient power.
* Data signal interruption.
* Incorrect LED count.
* Loose connection.

Use the sequential test:

```text
S
```

to identify the first LED where the problem appears.

---

## Code Purpose

This code is mainly intended for hardware validation and debugging of an 8x32 RGB LED matrix. It is especially useful during the development of custom LED shields, display modules, interactive panels, and embedded visual interfaces.

It can be used to verify:

* LED addressing.
* Matrix orientation.
* Data input wiring.
* Row mapping.
* Power stability.
* Individual LED functionality.

---

## Possible Improvements

Future versions of this program could include:

* XY coordinate control.
* Zigzag mapping correction.
* Text scrolling.
* Color selection through serial commands.
* Brightness adjustment through serial commands.
* Animation modes.
* Sprite rendering.
* Matrix graphics library integration.
* Support for multiple data inputs.

---

## Example Workflow

A recommended test sequence is:

```text
C
L1
L8
F0
F1
T
V
R1-16
A
S
```

This sequence allows the user to verify individual LEDs, rows, reference points, LED ranges, full matrix operation, and the complete LED order.

---

## License

This project can be used, modified, and adapted for educational, prototyping, and hardware testing purposes.
