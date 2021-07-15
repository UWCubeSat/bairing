#include <SoftwareSerial.h>
#include "PacketReceiver.h"
#include "PacketSender.h"

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

// Command IDs
enum class Command {
  Reset = 0x01,
  BeginLog = 0x02,
  EndLog = 0x04,
  Message = 0x69,
};

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

// process a packet that's just been received
void ProcessPacket(PacketReceiver packet, char *data) {
  Command id = (Command) packetReceiver.GetPacketID();
  uint16_t datalen = packetReceiver.GetPacketDataLength();

  switch (id) {
    case Command::Reset:
      // reset, then send reset signals to peripherals
      // TODO
      SendMessagePacket(RESET_MSG);
      Serial.println("pretend we sent reset signals");
      Serial.println("pretend this reset the arduino");
      Serial.println("pretend we don't see this, since the arduino will have reset");
      break;
    case Command::BeginLog:
      // start telemetry
      logging = true;
      SendMessagePacket(BEGIN_LOG_MSG);
      Serial.println(BEGIN_LOG_MSG);
      break;
    case Command::EndLog:
      // end telemetry
      logging = false;
      SendMessagePacket(END_LOG_MSG);
      Serial.println(END_LOG_MSG);
      break;
    case Command::Message:
      // send the stored message, or update and echo the message from a string sent
      if (datalen > 0) {
        // received a string to update stored message with
        // cap string to 31 characters long, then null-terminate in case rcvbuf isn't already
        datalen = min(datalen, 31);
        memcpy(message, rcvbuf, datalen);
        message[datalen] = '\0';
      }
      // send stored message
      SendMessagePacket(message);
      Serial.println(message);
      break;
    default:
      // command not recognized
      SendMessagePacket(UNRECOGNIZED_MSG);
      Serial.println(UNRECOGNIZED_MSG);
      break;
  }
}

// Check if we received a byte, and if we did process it
void CheckReceiveByte() {
  if (bt.available()) {
    char readbyte = bt.read();

    if (packetReceiver.AddByte(readbyte)) {
      uint16_t rcvbuflen = packetReceiver.GetPacketDataLength();
      uint8_t id = packetReceiver.GetPacketID();

      Serial.println("Packet received");
      Serial.print("  Length: ");
      Serial.println(rcvbuflen);
      
      Serial.print("  ID: ");
      Serial.println(id);
      
      Serial.print("  Data: ");
      for (int i = 0; i < rcvbuflen; i++) {
        Serial.print(rcvbuf[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      ProcessPacket(packetReceiver, rcvbuf);

      packetReceiver.Begin();  // restart packet receiver
    }
    
    /*
    if (packetReceiver.Completed()) {
     // we are not currently receiving a packet
      pattern = ((uint32_t) readbyte << 24) | (pattern >> 8);
      if (pattern == SYNC_PATTERN) {
        // Serial.println("Packet begin");
        packetReceiver.Begin();
      }
    } else {
      if (packetReceiver.AddByte(readbyte)) {
        // finished packet
        uint16_t rcvbuflen = packetReceiver.GetPacketDataLength();
        uint8_t id = packetReceiver.GetPacketID();

        Serial.println("Packet received");
        Serial.print("  Length: ");
        Serial.println(rcvbuflen);
        
        Serial.print("  ID: ");
        Serial.println(id);
        
        Serial.print("  Data: ");
        for (int i = 0; i < rcvbuflen; i++) {
          Serial.print(rcvbuf[i], HEX);
          Serial.print(" ");
        }
        Serial.println();

        ProcessPacket(packetReceiver, rcvbuf);
    
        // reset sync pattern to prepare for the next pattern
        pattern = 0;
      }
    }
    */
  }
}

void setup() {
  Serial.begin(9600);
  bt.begin(57600);
  
  logging = false;
  pattern = 0;
  lastTime = millis();
  
  strcpy(message, DEFAULT_MSG);

  packetReceiver.Begin();
}

void loop() {
  CheckReceiveByte();
  
  if (logging && (millis() - lastTime >= DELAY)) {
    SendAttitudePacket();
    lastTime = millis();
  }
}
