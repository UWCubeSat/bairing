
# bairing
Source code for the Blueboy air bearing diagnostics system, COSMOS project defining target definitions for monitoring, and a bare-bones real-time visual orientation monitoring program.

See: [Using the Air Bearing Test Stand](https://wiki.huskysat.org/wiki/index.php/Guide:_Using_the_Air_Bearing_Test_Stand) for more information on use.

# Project Structure
  - `arduino/`: Arduino sketches
    - `Blueboy/`: Blueboy Arduino sketch and source files
    - `Calibration/`: Testing sketch for persistent calibration
    - `bno085/`: Testing sketch for the BNO085 driver
  - `config/`: COSMOS project configuration
    - `targets/BLUEBOY/cmd_tlm`: Configuration for telemetry and command packets
    - `targets/BLUEBOY/screens`: Definitions for COSMOS displays
  - `procedures/`: COSMOS procedures defined in Ruby
  - `monitor/`: 3d live orientation monitoring program in Python
  - `msp430interface/`: CCS project containing an example implementation of the I2C test interface
  - `fritzing/`: Fritzing project modeling the system on a breadboard

# Arduino Libraries
Libraries required to compile Blueboy:
  - Adafruit Unified Sensor
  - Adafruit BusIO
  - AltSoftSerial
  - SparkFun BNO080 Cortex Based IMU
  
# Python Libraries
Libraries required to run the visual orientation monitoring program:
  - PyGame
  - AHRS

# Hardware
See the breadboard schematic in `fritzing/` for wiring.
  - BNO085 9-DOF IMU
  - HC-06 Bluetooth module
  - 4-channel I2C bi-directional 3.3-5V logic level converter
  - TP4056 Li-ion battery charger + 5V boost converter with 18650
  - Resistors: 2 10kOhm, 1 200Ohm