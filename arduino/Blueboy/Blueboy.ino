/*!
 * @file Blueboy.ino
 * @author Sebastian S.
 * @brief Main sketch for the Blueboy 3-axis air bearing test stand diagnostics system. Receives commands, sends telemetry, and processes data.
 */

#include <AltSoftSerial.h>    // For UART communication with HC-06 (uses hardware timers instead of being purely software-based)
#include <Wire.h>             // For I2C communication with sensors and 1U
#include <avr/pgmspace.h>     // For storing constants in flash memory

#include "src/Blueboy.h"
#include "src/Messages.h"
#include "src/CommandProcessor.h"
#include "src/BlueboyTelemetry.h"

const int RX_PIN = 8;    // RX pin required by AltSoftSerial
const int TX_PIN = 9;    // TX pin required by AltSoftSerial
const int RST_PIN = 4;   // gpio pin tied to reset, pull low to reset

char strbuf[64];         // general-purpose string buffer
char message[32];        // stored message to be echoed on command

AltSoftSerial bt(RX_PIN, TX_PIN);

BlueboyPeripherals peripherals;

CommandProcessor commands(bt, SYNC_PATTERN);
BlueboyTelemetry telemetry(bt, peripherals, SYNC_PATTERN);

/*!
 * @brief Callback to be invoked on a reset command. Resets the system and sends a reset request to the connected test system.
 * 
 * Sends a message over telemetry reporting the beginning of the reset sequence.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return False (this callback should never be returned from)
 * 
 * @todo Request resets from peripherals and the test system
 */
bool ResetCommand(CommandID cmd, const char *data, uint16_t len) {
  telemetry.SendMessage(RESET_MSG);
  delay(100);   // delay to allow message to be sent asynchronously
  
  // TODO request reset from peripherals and 1U
  
  digitalWrite(RST_PIN, LOW);  // pull pin low for reset
  
  return false;  // this shouldn't happen
}

/*!
 * @brief Callback to be invoked on a message command. Sends a message over telemetry containing a stored
 * message, or updates the stored message with a received string and echoes it.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return True
 */
bool MessageCommand(CommandID cmd, const char *data, uint16_t len) {
  // send the stored message, or update and echo the message from a string sent
  if (len > 0) {
    // received a string to update stored message with
    // cap string to 31 characters long, then null-terminate
    len = min(len, 31);
    memcpy(message, data, len);
    message[len] = '\0';
  }
  
  // send stored message
  telemetry.SendMessage(message);
  return true;
}

/*!
 * @brief Callback to be invoked on a begin log command. Starts logging data from Blueboy or the test system
 * depending on the command ID, accepting an unsigned 16-bit short as the collection period and an optional
 * byte representing the attitude mode (orientation data as raw sensor data, euler angles, or a quaternion).
 * 
 * Sends messages over telemetry reporting successful beginning or failure due to current calibration.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return True iff the command was properly formed and Blueboy is not currently calibrating any sensors.
 */
bool BeginLogCommand(CommandID cmd, const char *data, uint16_t len) {
  if (peripherals.Calibrating()) {
    // don't start logging if we're calibrating
    telemetry.SendMessage(CANT_LOG_MSG);
    return false;
  }
  
  telemetry.SendMessage(BEGIN_LOG_MSG);
  uint8_t dev = ((uint8_t) cmd) >> 4;
  uint8_t mode = 0;
  uint16_t period;

  if (len >= 2) {
    period = *((uint16_t *) (data));  // interpret data as a pointer to a short, then dereference it
    telemetry.SetLogPeriod((Device) dev, (unsigned long) period);
  } else {
    return false;
  }

  if (len >= 2 + 1) {
    // optional mode
    mode = *((uint8_t *) (data + 2));  // interpret (data + 2) as a pointer to a byte, then dereference it
  }
  
  telemetry.BeginLogging((Device) dev, (AttitudeMode) mode);
  return true;
}

/*!
 * @brief Callback to be invoked on an end log command. Stops logging data from Blueboy or the test system depending on the command ID.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return True
 */
bool EndLogCommand(CommandID cmd, const char *data, uint16_t len) {
  telemetry.SendMessage(END_LOG_MSG);
  uint8_t dev = ((uint8_t) cmd) >> 4;
  
  telemetry.EndLogging((Device) dev);
  return true;
}

/*!
 * @brief Callback to be invoked when an invalid command is received.
 * 
 * Sends a message over telemetry reporting the command ID that was not recognized.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return True
 */
bool InvalidCommand(CommandID cmd, const char *data, uint16_t len) {
  sprintf(strbuf, "%s: %02x", UNRECOGNIZED_MSG, cmd);
  telemetry.SendMessage(strbuf);
  return true;
}

/*!
 * @brief Callback to be invoked on a begin calibrate command. Starts calibrating the sensor associated
 * with the command ID on Blueboy and the test system simultaneously.
 *
 * Sends a message over telemetry reporting that calibration has begun.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return True iff neither Blueboy nor the test system are currently logging.
 */
bool BeginCalibrateCommand(CommandID cmd, const char *data, uint16_t len) {
  if (telemetry.Logging(Device::Own) || telemetry.Logging(Device::Test)) {
    // don't start calibrating if we're currently logging
    telemetry.SendMessage(CANT_CALIB_MSG);
    return false;
  }
  
  switch (cmd) {
    case CommandID::BeginCalibMag:
      peripherals.lis2mdl.BeginCalibration(SENSOR_TYPE_MAGNETIC_FIELD);
      peripherals.oneU.BeginCalibration(SENSOR_TYPE_MAGNETIC_FIELD);
      break;
    case CommandID::BeginCalibAcc:
      peripherals.oneU.BeginCalibration(SENSOR_TYPE_ACCELEROMETER);
      break;
    case CommandID::BeginCalibGyro:
      peripherals.lsm6ds33.BeginCalibration(SENSOR_TYPE_GYROSCOPE);
      peripherals.oneU.BeginCalibration(SENSOR_TYPE_GYROSCOPE);
      break;
  }

  telemetry.SendMessage(BEGIN_CALIB_MSG);
  
  return true;
}

/*!
 * @brief Callback to be invoked on an end calibrate command. Stops calibrating the sensor associated
 * with the command ID on Blueboy and the test system simultaneously, stores new calibration constants
 * in persistent memory, and reports the new constants to a connected serial monitor.
 *
 * Sends a message over telemetry reporting that calibration has ended.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return True iff the command was properly formed and neither Blueboy nor the test system are currently logging.
 */
bool EndCalibrateCommand(CommandID cmd, const char *data, uint16_t len) {
  struct AxisOffsets off;
  switch (cmd) {
    case CommandID::EndCalibMag:
      peripherals.lis2mdl.EndCalibration();
      peripherals.oneU.EndCalibration();
      peripherals.lis2mdl.GetCalibration(&off);
      break;
    case CommandID::EndCalibAcc:
      peripherals.oneU.EndCalibration();
      break;
    case CommandID::EndCalibGyro:
      peripherals.lsm6ds33.EndCalibration();
      peripherals.oneU.EndCalibration();
      peripherals.lsm6ds33.GetCalibration(&off);
      break;
    default:
      return false;
  }
  telemetry.SendMessage(END_CALIB_MSG);
  
  Serial.println(F("Calibration complete! Offsets: "));
  Serial.print(F("  x: "));
  Serial.println(off.xOff, 5);
  Serial.print(F("  y: "));
  Serial.println(off.yOff, 5);
  Serial.print(F("  z: "));
  Serial.println(off.zOff, 5);
  return true;
}

/*!
 * @brief Callback to be invoked on a clear calibrate command. Clears the calbration constants of the sensor
 * associated with the command ID.
 *
 * Sends a message over telemetry reporting that calibration has been cleared.
 * @param cmd The ID of the command that invoked this callback
 * @param data Byte buffer containing data that may have been passed by the command
 * @param len Length of the byte buffer
 * @return True iff the command was properly formed.
 */
bool ClearCalibrateCommand(CommandID cmd, const char *data, uint16_t len) {  
  switch (cmd) {
    case CommandID::ClearCalibMag:
      peripherals.lis2mdl.ClearCalibration();
      break;
    case CommandID::ClearCalibAcc:
      break;
    case CommandID::ClearCalibGyro:
      peripherals.lsm6ds33.ClearCalibration();
      break;
    default:
      return false;
  }
  
  telemetry.SendMessage(CLEAR_CALIB_MSG);
  return true;
}

/*!
 * @brief Arduino Setup function
 */
void setup() {
  // need to write high first, or else the system will reset immediately
  digitalWrite(RST_PIN, HIGH);
  pinMode(RST_PIN, OUTPUT);
  
  // begin serial communications over serial monitor, bluetooth, and i2c bus
  Serial.begin(9600);
  bt.begin(57600);
  Wire.begin();
  
  // copy the default message from flash memory to a buffer, to be echoed on an empty message command
  strcpy_P(message, (const char *) DEFAULT_MSG);

  // bind command callbacks to ids in the command processor
  commands.Bind(CommandID::Reset,             &ResetCommand);
  commands.Bind(CommandID::BeginOwnAttitude,  &BeginLogCommand);
  commands.Bind(CommandID::EndOwnAttitude,    &EndLogCommand);
  commands.Bind(CommandID::BeginTestAttitude, &BeginLogCommand);
  commands.Bind(CommandID::EndTestAttitude,   &EndLogCommand);
  commands.Bind(CommandID::Echo,              &MessageCommand);
  commands.Bind(CommandID::BeginCalibMag,     &BeginCalibrateCommand);
  commands.Bind(CommandID::EndCalibMag,       &EndCalibrateCommand);
  commands.Bind(CommandID::ClearCalibMag,     &ClearCalibrateCommand);
  
  telemetry.InitializePeripherals();

  // report that we've started over telemetry
  telemetry.SendMessage(SETUP_MSG);
}

/*!
 * @brief Arduino Loop function
 */
void loop() {
  commands.Tick();
  telemetry.Tick();
}
