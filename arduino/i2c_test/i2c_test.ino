#include<Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("I2C Begin");
}

void loop() {
  byte address = 0x3A;
  byte error;
  uint8_t cnt = 13;
  Serial.println("Begin transmission to 0x3A...");
  
  Wire.beginTransmission(address);
  for (int i = 0; i < cnt; i++) {
    Serial.print(i, HEX);
    Serial.print(" ");
    Wire.write((uint8_t) i);
  }
  Serial.println();
  error = Wire.endTransmission(false);
  Serial.print("Transmission end: ");
  Serial.println(error);

  Serial.println("Requesting data from 0x3A...");
  uint8_t rcv = Wire.requestFrom(address, cnt);
  Serial.print("Received ");
  Serial.print(rcv);
  Serial.println(" bytes");
  Serial.print("Data: ");
  while (Wire.available()) {
    Serial.print(Wire.read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
  delay(5000);
}
