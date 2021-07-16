#include <AltSoftSerial.h>
#include <Wire.h>

#include "PacketReceiver.h"
#include "PacketSender.h"
#include "CommandProcessor.h"
#include "BlueboyTelemetry.h"

#include "OneUDriver.h"

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

char strbuf[128];        // general-purpose string buffer
char message[32];        // stored message to be echoed on command

AltSoftSerial bt(RX_PIN, TX_PIN);              // rx on pin 8, tx on pin 9

CommandProcessor commands(bt, SYNC_PATTERN);    // command processor
BlueboyTelemetry telemetry(bt, SYNC_PATTERN);   // telemetry sender

bool ResetCommand(const char *data, uint16_t len) {
  telemetry.SendMessage(RESET_MSG);
  delay(100);
  digitalWrite(RST_PIN, LOW);  // pull pin low for reset
  
  return false;  // this shouldn't happen
}

bool BeginLogCommand(const char *data, uint16_t len) {
  telemetry.SendMessage(BEGIN_LOG_MSG);
  telemetry.BeginLogging();
  return true;
}

bool EndLogCommand(const char *data, uint16_t len) {
  telemetry.SendMessage(END_LOG_MSG);
  telemetry.EndLogging();
  return true;
}

bool MessageCommand(const char *data, uint16_t len) {
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

bool InvalidCommand(const char *data, uint16_t cmdid) {
  sprintf(strbuf, "%s: %02x", UNRECOGNIZED_MSG, cmdid);
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

  commands.Bind(CommandID::Reset,     &ResetCommand);
  commands.Bind(CommandID::BeginLog,  &BeginLogCommand);
  commands.Bind(CommandID::EndLog,    &EndLogCommand);
  commands.Bind(CommandID::Message,   &MessageCommand);

  // TODO initialize peripherals here

  telemetry.SendMessage(SETUP_MSG);  // inform monitor we've started
}

void loop() {
  commands.Tick();
  telemetry.Tick();
}
