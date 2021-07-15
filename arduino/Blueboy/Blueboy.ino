#include <SoftwareSerial.h>
#include "PacketReceiver.h"
#include "PacketSender.h"
#include "CommandProcessor.h"
#include "BlueboyTelemetry.h"

const uint32_t SYNC_PATTERN = 0xDEADBEEF;

// packet messages
const char *DEFAULT_MSG = "see how the brain plays around";
const char *BEGIN_LOG_MSG = "Began logging";
const char *END_LOG_MSG = "Ended logging";
const char *RESET_MSG = "Resetting system";
const char *UNRECOGNIZED_MSG = "Unrecognized command";

char sendbuf[256];		   // send packet buffer, used to build a packet
char rcvbuf[256];		     // buffer containing data received from packets (without sync, length, id, etc.)
char message[32];        // stored message to be echoed on command

SoftwareSerial bt = SoftwareSerial(2, 3);  // rx on pin 2, tx on pin 3

PacketReceiver packetReceiver = PacketReceiver(rcvbuf, SYNC_PATTERN);
PacketSender packetSender = PacketSender(sendbuf, SYNC_PATTERN);

CommandProcessor commands;
BlueboyTelemetry telemetry = BlueboyTelemetry(bt, packetSender);

bool ResetCommand(const char *data, uint16_t len) {
  telemetry.SendMessage(RESET_MSG);
  Serial.println("Pretend we reset everything here");
  return true;  // in the real command, this won't even happen
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
    memcpy(message, rcvbuf, len);
    message[len] = '\0';
  }
  
  // send stored message
  telemetry.SendMessage(message);
  return true;
}

void setup() {
  Serial.begin(9600);
  bt.begin(57600);
  
  strcpy(message, DEFAULT_MSG);

  commands.Bind(CommandID::Reset,     &ResetCommand);
  commands.Bind(CommandID::BeginLog,  &BeginLogCommand);
  commands.Bind(CommandID::EndLog,    &EndLogCommand);
  commands.Bind(CommandID::Message,   &MessageCommand);

  packetReceiver.Begin();  // receive first packet
}

void loop() {
  while (bt.available()) {
    // a byte is available to read over bluetooth
    char readbyte = bt.read();

    // register byte in the packet receiver
    if (packetReceiver.AddByte(readbyte)) {
      // packetReceiver.PrintPacketInfo();
      
      CommandID command = (CommandID) packetReceiver.GetPacketID();
      uint16_t datalen = packetReceiver.GetPacketDataLength();
      commands.Dispatch(command, rcvbuf, datalen);
      
      packetReceiver.Begin();  // restart packet receiver
    }
  }

  telemetry.Tick();
}
