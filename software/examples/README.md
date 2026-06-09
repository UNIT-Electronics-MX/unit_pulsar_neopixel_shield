# Examples

This directory contains example projects for testing and controlling the RGB LED matrix shield.

The examples are organized by programming language or development environment. Each subdirectory contains code focused on a specific implementation or testing approach.

---

## Directory Structure

```text
examples/
├── cpp_examples/
│   ├── neo_esp/
│   ├── neo_esp_offset/
│   ├── neo_esp_test_serial/
│   └── README.md
├── micropython/
└── README.md
```

---

## Available Example Categories

### `cpp_examples/`

Contains C++ examples for controlling the RGB LED matrix using Arduino-compatible development environments.

These examples are useful for:

* Testing individual LEDs.
* Verifying LED indexing.
* Checking matrix orientation.
* Running basic animations.
* Debugging wiring and data signal issues.

---

### `micropython/`


This folder include examples for boards compatible with MicroPython, allowing the matrix shield to be controlled using Python-based scripts.

---

## Recommended Usage

Start with the examples inside `cpp_examples/` if you want to validate the hardware and confirm that the RGB LED matrix is working correctly.

A recommended first test is to use a serial-based example to check:

* Power connection.
* Data pin configuration.
* LED count.
* Matrix direction.
* Individual LED addressing.

---

## Notes

Some examples may require external libraries, such as the Adafruit NeoPixel library.
