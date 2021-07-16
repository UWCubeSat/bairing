#include "BlueboyTelemetry.h"

BlueboyTelemetry::BlueboyTelemetry(SoftwareSerial& serial, uint32_t sync): _serial(serial),
                                                                           _sender(PacketSender(_sendbuf, sync)) {
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
    for (int i = 0; i < 9; i++) {
      // TODO get real data from a peripheral
      raw.data[i] = (float) (millis() * (i + 1)) / 1000.0;
    }
    SendAttitudeRaw(raw);
    _lastSent = millis();
  }
}
