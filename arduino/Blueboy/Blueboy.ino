#include <AltSoftSerial.h>
#include <Wire.h>

#include "PacketReceiver.h"
#include "PacketSender.h"
#include "Blueboy.h"
#include "CommandProcessor.h"
#include "BlueboyTelemetry.h"

const int RX_PIN = 8;
const int TX_PIN = 9;
const int RST_PIN = 4;

const uint32_t SYNC_PATTERN = 0xDEADBEEF;

// packet messages
const char *DEFAULT_MSG = "see how the brain plays around";
const char *SETUP_MSG = "Initialized system";
const char *BEGIN_LOG_MSG = "Began logging";
const char *END_LOG_MSG = "Ended logging";
const char *RESET_MSG = "Resetting system...";
const char *UNRECOGNIZED_MSG = "Unrecognized command";

char strbuf[64];        // general-purpose string buffer
char message[32];        // stored message to be echoed on command

AltSoftSerial bt(RX_PIN, TX_PIN);              // rx on pin 8, tx on pin 9

CommandProcessor commands(bt, SYNC_PATTERN);    // command processor
BlueboyTelemetry telemetry(bt, SYNC_PATTERN);   // telemetry sender

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
  telemetry.SendMessage(BEGIN_LOG_MSG);
  uint8_t dev = ((uint8_t) cmd) >> 4;
  uint8_t mode = 0;
  uint16_t period;

  Serial.println("Received begin log: ");
  Serial.print("  dev: ");
  Serial.println(dev);

  if (len >= 2) {
    period = *((uint16_t *) (data));  // interpret data as a pointer to a short, then dereference it
    telemetry.SetLogPeriod((Device) dev, (unsigned long) period);
    Serial.print("  period: ");
    Serial.println(period);
  } else {
    return false;
  }

  if (len >= 2 + 1) {
    // optional mode
    mode = *((uint8_t *) (data + 2));  // interpret (data + 2) as a pointer to a byte, then dereference it
    Serial.print("  mode: ");
    Serial.println(mode);
  }
  
  telemetry.BeginLogging((Device) dev, (AttitudeMode) mode);
  return true;
}

bool EndLogCommand(CommandID cmd, const char *data, uint16_t len) {
  telemetry.SendMessage(END_LOG_MSG);
  uint8_t dev = ((uint8_t) cmd) >> 4;

  Serial.println("Received end log: ");
  Serial.print("  dev: ");
  Serial.println(dev);
  
  telemetry.EndLogging((Device) dev);
  return true;
}

bool InvalidCommand(CommandID cmd, const char *data, uint16_t len) {
  sprintf(strbuf, "%s: %02x", UNRECOGNIZED_MSG, cmd);
  telemetry.SendMessage(strbuf);
  return true;
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

  telemetry.InitializePeripherals();

  telemetry.SendMessage(SETUP_MSG);  // inform monitor we've started
}

void loop() {
  commands.Tick();
  telemetry.Tick();
}
