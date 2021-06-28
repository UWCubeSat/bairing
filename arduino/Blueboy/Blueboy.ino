#include <SoftwareSerial.h>
#include "ReceivePacket.h"
#include "SendPacket.h"

const uint8_t  STRING_PACKET_ID = 0x69;
const uint32_t SYNC_PATTERN = 0xDEADBEEF;
const char *DEFAULT_STRING = "see how the brain plays around";

const unsigned long DELAY = 200;	// time in milliseconds between packet sending

char sendbuf[256];		  // send packet buffer, used to build a packet
char rcvbuf[256];		    // buffer containing data received from packets (without sync, length, id, etc.)
uint16_t rcvbuflen;

uint32_t pattern;       // last 4 bytes received, most recent in the most significant byte and vice versa

unsigned long lastTime;	// time since last packet was sent

ReceivePacket receivePacket = ReceivePacket(rcvbuf);

SoftwareSerial bt = SoftwareSerial(2, 3);  // rx on pin 2, tx on pin 3

// Check if we received a byte, and if we did process it
void CheckReceiveByte() {
  if (bt.available()) {
    char readbyte = bt.read();
    if (receivePacket.completed()) {
	    // we are not currently receiving a packet
      pattern = ((uint32_t) readbyte << 24) | (pattern >> 8);
      if (pattern == SYNC_PATTERN) {
        receivePacket.begin();
        // Serial.println("begin packet");
      }
    } else {
      // Serial.print(readbyte, HEX);
      // Serial.print(" ");
      if (receivePacket.addByte(readbyte)) {
        // finished packet
        rcvbuflen = receivePacket.getPacketDataLength();
        uint16_t id = receivePacket.getPacketID();
		
		    // data in rcvbuf
        
        // Serial.println();
        Serial.print("Length: ");
        Serial.println(rcvbuflen);
		    
        Serial.print("ID: ");
        Serial.println(id);
		    
        Serial.print("Data: ");
        Serial.write(rcvbuf, rcvbuflen);
        Serial.println();
    
        // reset sync pattern to prepare for the next pattern
        pattern = 0;
      }
    }
  }
}

// Sends a string response packet, with a sync pattern followed by a length and string
void SendStringPacket(const char *str, uint8_t len) {
  SendPacket packet = SendPacket(sendbuf, STRING_PACKET_ID);
  packet.addStr(str, (int) len);
  
  bt.write((char *) &SYNC_PATTERN, 4);
  packet.send(bt);
}

void setup() {
  Serial.begin(9600);
  bt.begin(57600);
  
  pattern = 0;
  lastTime = millis();

  strcpy(rcvbuf, DEFAULT_STRING);
  rcvbuflen = strlen(DEFAULT_STRING);
}

void loop() {
  CheckReceiveByte();
  
  if (millis() - lastTime >= DELAY) {
    SendStringPacket(rcvbuf, rcvbuflen);
    lastTime = millis();
  }
}
