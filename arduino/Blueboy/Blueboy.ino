#include <AltSoftSerial.h>
#include <Wire.h>

#include "src/Blueboy.h"
#include "src/CommandProcessor.h"
#include "src/BlueboyTelemetry.h"
#include "src/sensor/CalibratedLSM6DS33.h"

const int RX_PIN = 8;
const int TX_PIN = 9;
const int RST_PIN = 4;

// packet messages
const char *DEFAULT_MSG = "see how the brain plays around";
const char *SETUP_MSG = "Initialized system";
const char *CANT_LOG_MSG = "Can't log, stop calibrating first";
const char *BEGIN_LOG_MSG = "Began logging";
const char *END_LOG_MSG = "Ended logging";
const char *RESET_MSG = "Resetting system...";
const char *UNRECOGNIZED_MSG = "Unrecognized command";

const char *CANT_CALIB_MSG = "Can't calibrate, stop logging first";
const char *BEGIN_CALIB_MSG = "Began calibration";
const char *END_CALIB_MSG = "Ended calibration";

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

  telemetry.SendMessage(BEGIN_CALIB_MSG);
  
  switch (cmd) {
    case CommandID::BeginCalibMag:
      break;
    case CommandID::BeginCalibAcc:
      break;
    case CommandID::BeginCalibGyro:
      peripherals.lsm6ds33.BeginCalibration(SENSOR_TYPE_GYROSCOPE);
      break;
  }
  return false;
}

bool EndCalibrateCommand(CommandID cmd, const char *data, uint16_t len) {
  telemetry.SendMessage(END_CALIB_MSG);
  
  switch (cmd) {
    case CommandID::EndCalibMag:
      break;
    case CommandID::EndCalibAcc:
      break;
    case CommandID::EndCalibGyro:
      peripherals.lsm6ds33.EndCalibration();

      struct AxisOffsets off;
      peripherals.lsm6ds33.GetCalibration(&off);
      
      Serial.print("x: ");
      Serial.println(off.xOff, 5);
      Serial.print("y: ");
      Serial.println(off.yOff, 5);
      Serial.print("z: ");
      Serial.println(off.zOff, 5);
      break;
  }
  return false;
}

bool ClearCalibrateCommand(CommandID cmd, const char *data, uint16_t len) {
  switch (cmd) {
    case CommandID::ClearCalibMag:
      break;
    case CommandID::ClearCalibAcc:
      break;
    case CommandID::ClearCalibGyro:
      break;
  }
  return false;
}

void setup() {
  digitalWrite(RST_PIN, HIGH);  // need to write high first, or else the system will reset immediately
  pinMode(RST_PIN, OUTPUT);
  
  Serial.begin(9600);
  bt.begin(57600);
  Wire.begin();
  
  strcpy(message, DEFAULT_MSG);

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
