#include "BlueboyTelemetry.h"

BlueboyTelemetry::BlueboyTelemetry(AltSoftSerial& serial, uint32_t sync): _serial(serial),
                                                                           _sender(PacketSender(_sendbuf, sync)),
                                                                           _oneU(ONEU_ADDRESS) {
  _sendDelay = DEFAULT_LOG_DELAY;
  _lastSent = 0;
  _logging = false;
}

void BlueboyTelemetry::BeginLogging() {
  _logging = true;
}

void BlueboyTelemetry::EndLogging() {
  _logging = false;
}

void BlueboyTelemetry::SendMessage(const char *str) {
  Serial.println(str);
  _sender.Begin((uint8_t) TelemetryID::Message);
  _sender.AddStr(str);
  _sender.Send(_serial);
}

void BlueboyTelemetry::SendAttitudeRaw(const struct AttitudeDataRaw& attitude) {
  _sender.Begin((uint8_t) TelemetryID::AttitudeRaw);
  
  for (int i = 0; i < 9; i++) {
    _sender.AddFloat(attitude.data[i]);
  }
  
  _sender.Send(_serial);
}

void BlueboyTelemetry::Tick() {
  if (_logging && (millis() - _lastSent >= _sendDelay)) {
    struct AttitudeDataRaw raw;
    struct OneUData data;
    bool success = true;
    if (success && (success = _oneU.ReadData(OneUDataType::Mag, &data))) {
      raw.magX = data.magnetic_field.x;
      raw.magY = data.magnetic_field.y;
      raw.magZ = data.magnetic_field.z;
    }
    if (success && (success = _oneU.ReadData(OneUDataType::Acc, &data))) {
      raw.accX = data.acceleration.x;
      raw.accY = data.acceleration.y;
      raw.accZ = data.acceleration.z;
    }
    if (success && (success = _oneU.ReadData(OneUDataType::Gyro, &data))) {
      raw.gyroX = data.angular_velocity.x;
      raw.gyroY = data.angular_velocity.y;
      raw.gyroZ = data.angular_velocity.z;
    }
    if (success) {
      SendAttitudeRaw(raw);
    }
    _lastSent = millis();
  }
}
