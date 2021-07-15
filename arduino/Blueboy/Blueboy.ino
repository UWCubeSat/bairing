#include <SoftwareSerial.h>
#include "PacketReceiver.h"
#include "PacketSender.h"
#include "CommandProcessor.h"

const uint32_t SYNC_PATTERN = 0xDEADBEEF;

// packet messages
const char *DEFAULT_MSG = "see how the brain plays around";
const char *BEGIN_LOG_MSG = "Began logging";
const char *END_LOG_MSG = "Ended logging";
const char *RESET_MSG = "Resetting system";
const char *UNRECOGNIZED_MSG = "Unrecognized command";

const unsigned long DELAY = 200;	// time in milliseconds between packet sending

bool logging;            // whether the system is actively sending data telemetry or not

char sendbuf[256];		   // send packet buffer, used to build a packet
char rcvbuf[256];		     // buffer containing data received from packets (without sync, length, id, etc.)

char message[32];        // stored message to be echoed on command
uint8_t messageLength;   // stored message length

uint32_t pattern;        // last 4 bytes received, most recent in the most significant byte and vice versa

unsigned long lastTime;	 // time since last packet was sent

PacketReceiver packetReceiver = PacketReceiver(rcvbuf, SYNC_PATTERN);
PacketSender packetSender = PacketSender(sendbuf);

CommandProcessor commands;

// Telemetry IDs
enum class Telemetry {
  Attitude = 0x01,
  Message = 0x69,
};

// Attitude data from one source
struct AttitudeData {
  float magX, magY, magZ;     // orientation
  float gyroX, gyroY, gyroZ;  // angular velocity
  float accX, accY, accZ;     // linear acceleration
};

SoftwareSerial bt = SoftwareSerial(2, 3);  // rx on pin 2, tx on pin 3

// Sends a string response packet, with a sync pattern followed by a length and string
void SendMessagePacket(const char *str) {
  // PacketSender packet = PacketSender(sendbuf, (uint8_t) Telemetry::Message);
  packetSender.Begin((uint8_t) Telemetry::Message);
  packetSender.AddStr(str);
  
  bt.write((char *) &SYNC_PATTERN, 4);
  packetSender.Send(bt);
}

// Sends an attitude data packet
void SendAttitudePacket() {
  // PacketSender packet = PacketSender(sendbuf, (uint8_t) Telemetry::Attitude);
  packetSender.Begin((uint8_t) Telemetry::Attitude);
  
  struct AttitudeData hemisphere;
  hemisphere.magX = hemisphere.magY = hemisphere.magZ = 0;
  hemisphere.gyroX = hemisphere.gyroY = hemisphere.gyroZ = 1.5;
  hemisphere.accX = hemisphere.accY = hemisphere.accZ = 6;
  packetSender.AddBuf((char *) &hemisphere, sizeof(hemisphere));

  bt.write((char *) &SYNC_PATTERN, 4);
  packetSender.Send(bt);
}

// Check if we received a byte, and if we did process it
void CheckReceiveByte() {
  
}

bool ResetCommand(const char *data, uint16_t len) {
  Serial.println("Pretend we reset everything here");
  return true;  // in the real command, this won't even happen
}

bool BeginLogCommand(const char *data, uint16_t len) {
  logging = true;
  return true;
}

bool EndLogCommand(const char *data, uint16_t len) {
  logging = false;
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
  SendMessagePacket(message);
  
  return true;
}

void setup() {
  Serial.begin(9600);
  bt.begin(57600);
  
  logging = false;
  pattern = 0;
  lastTime = millis();
  
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
  
  if (logging && (millis() - lastTime >= DELAY)) {
    SendAttitudePacket();
    lastTime = millis();
  }
}
