#include <AltSoftSerial.h>    // For UART communication with HC-06
#include <Wire.h>             // For I2C communication with sensors and 1U
#include <avr/pgmspace.h>     // For storing constants in flash memory

#include "src/Blueboy.h"
#include "src/Messages.h"
#include "src/CommandProcessor.h"
#include "src/BlueboyTelemetry.h"
#include "src/sensor/CalibratedLSM6DS33.h"
#include "src/sensor/CalibrationStorage.h"

const int RX_PIN = 8;
const int TX_PIN = 9;
const int RST_PIN = 4;

char strbuf[64];         // general-purpose string buffer
char message[32];        // stored message to be echoed on command

AltSoftSerial bt(RX_PIN, TX_PIN);              // rx on pin 8, tx on pin 9

BlueboyPeripherals peripherals;

CommandProcessor commands(bt, SYNC_PATTERN);    // command processor
BlueboyTelemetry telemetry(bt, peripherals, SYNC_PATTERN);   // telemetry sender

bool ResetCommand(CommandID cmd, const char *data, uint16_t len) {
  telemetry.SendMessage(RESET_MSG);
  delay(100);
  digitalWrite(RST_PIN, LOW);  // pull pin low for reset
  
  return false;  // this shouldn't happen
}

bool MessageCommand(CommandID cmd, const char *data, uint16_t len) {
  // send the stored message, or update and echo the message from a string sent
  if (len > 0) {
    // received a string to update stored message with
    // cap string to 31 characters long, then null-terminate in case rcvbuf isn't already
    len = min(len, 31);
    memcpy(message, data, len);
    message[len] = '\0';
  }
  
  // send stored message
  telemetry.SendMessage(message);
  return true;
}

bool BeginLogCommand(CommandID cmd, const char *data, uint16_t len) {
  if (peripherals.lsm6ds33.Calibrating()) {
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

bool EndLogCommand(CommandID cmd, const char *data, uint16_t len) {
  telemetry.SendMessage(END_LOG_MSG);
  uint8_t dev = ((uint8_t) cmd) >> 4;
  
  telemetry.EndLogging((Device) dev);
  return true;
}

bool InvalidCommand(CommandID cmd, const char *data, uint16_t len) {
  sprintf(strbuf, "%s: %02x", UNRECOGNIZED_MSG, cmd);
  telemetry.SendMessage(strbuf);
  return true;
}

bool BeginCalibrateCommand(CommandID cmd, const char *data, uint16_t len) {
  if (telemetry.Logging(Device::Own) || telemetry.Logging(Device::Test)) {
    // don't start calibrating if we're currently logging
    telemetry.SendMessage(CANT_CALIB_MSG);
    return false;
  }
  
  switch (cmd) {
    case CommandID::BeginCalibMag:
      peripherals.lis2mdl.BeginCalibration(SENSOR_TYPE_MAGNETIC_FIELD);
      break;
    case CommandID::BeginCalibAcc:
      break;
    case CommandID::BeginCalibGyro:
      peripherals.lsm6ds33.BeginCalibration(SENSOR_TYPE_GYROSCOPE);
      break;
  }

  telemetry.SendMessage(BEGIN_CALIB_MSG);
  
  return true;
}

bool EndCalibrateCommand(CommandID cmd, const char *data, uint16_t len) {
  struct AxisOffsets off;
  switch (cmd) {
    case CommandID::EndCalibMag:
      peripherals.lis2mdl.EndCalibration();
      peripherals.lis2mdl.GetCalibration(&off);
      break;
    case CommandID::EndCalibAcc:
      break;
    case CommandID::EndCalibGyro:
      peripherals.lsm6ds33.EndCalibration();
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

void setup() {
  digitalWrite(RST_PIN, HIGH);  // need to write high first, or else the system will reset immediately
  pinMode(RST_PIN, OUTPUT);
  
  Serial.begin(9600);
  bt.begin(57600);
  Wire.begin();
  
  strcpy_P(message, (const char *) DEFAULT_MSG);

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

  telemetry.SendMessage(SETUP_MSG);  // inform monitor we've started
}

void loop() {
  commands.Tick();
  telemetry.Tick();
}
