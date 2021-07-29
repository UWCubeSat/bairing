#include "BlueboyTelemetry.h"

BlueboyTelemetry::BlueboyTelemetry(AltSoftSerial& serial,
                                   BlueboyPeripherals& peripherals,
                                   uint32_t sync): _serial(serial),
                                                   _sender(PacketSender(_sendbuf, sync)),
                                                   _peripherals(peripherals) {
  for (int i = 0; i < 2; i++) {
    _settings[i].sendDelay = DEFAULT_LOG_DELAY;
    _settings[i].lastSent = 0;
    _settings[i].logging = false;
    _settings[i].mode = AttitudeMode::Raw;
  }
}

bool BlueboyTelemetry::InitializePeripherals() {
  return _peripherals.Initialize();
}

void BlueboyTelemetry::SetLogPeriod(Device dev, unsigned long period) {
  int index = (int) dev - 1;
  _settings[index].sendDelay = period;
}

void BlueboyTelemetry::BeginLogging(Device dev, AttitudeMode mode) {
  int index = (int) dev - 1;
  _settings[index].logging = true;
  _settings[index].mode = mode;
}

void BlueboyTelemetry::EndLogging(Device dev) {
  int index = (int) dev - 1;
  _settings[index].logging = false;
}

bool BlueboyTelemetry::Logging(Device dev) {
  int index = (int) dev - 1;
  return _settings[index].logging;
}

static char msgbuf[64];

void BlueboyTelemetry::SendMessage(const __FlashStringHelper *str) {
  strcpy_P(msgbuf, (const char *)str);
  SendMessage(msgbuf);
}

void BlueboyTelemetry::SendMessage(const char *str) {
  Serial.println(str);
  _sender.Begin((uint8_t) TelemetryID::Message);
  _sender.AddStr(str);
  _sender.Send(_serial);
}

void BlueboyTelemetry::SendAttitude(Device dev, AttitudeMode mode, const struct AttitudeData& data) {
  _sender.Begin(((uint8_t) dev << 4) | (uint8_t) mode);   // dev as high 4 bits, mode as low

  switch (mode) {
    case AttitudeMode::Raw:
      _sender.AddFloat(data.raw.magnetic.x);
      _sender.AddFloat(data.raw.magnetic.y);
      _sender.AddFloat(data.raw.magnetic.z);
    
      _sender.AddFloat(data.raw.acceleration.x);
      _sender.AddFloat(data.raw.acceleration.y);
      _sender.AddFloat(data.raw.acceleration.z);
      
      _sender.AddFloat(data.raw.gyro.x);
      _sender.AddFloat(data.raw.gyro.y);
      _sender.AddFloat(data.raw.gyro.z);
      break;
    case AttitudeMode::Euler:
      _sender.AddFloat(data.orientation.euler.pitch);
      _sender.AddFloat(data.orientation.euler.roll);
      _sender.AddFloat(data.orientation.euler.heading);
      break;
    case AttitudeMode::Quaternion:
      _sender.AddFloat(data.orientation.quaternion.x);
      _sender.AddFloat(data.orientation.quaternion.y);
      _sender.AddFloat(data.orientation.quaternion.z);
      _sender.AddFloat(data.orientation.quaternion.w);
      break;
  }
  
  _sender.Send(_serial);
}

void BlueboyTelemetry::Tick() {
  if (_peripherals.lsm6ds33.Calibrating()) {
    _peripherals.lsm6ds33.AddCalibrationSample();
    delay(1);
  }
  
  if (_peripherals.lis2mdl.Calibrating()) {
    _peripherals.lis2mdl.AddCalibrationSample();
    delay(1);
  }
  
  for (int i = 0; i < 2; i++) {
    if (_settings[i].logging && (millis() - _settings[i].lastSent >= _settings[i].sendDelay)) {
      struct AttitudeData data;

      Device dev = (Device) (i + 1);
  
      // send attitude
      if (_settings[i].mode == AttitudeMode::Raw) {
        
        _peripherals.ReadRaw(dev, &data);
      }
  
      SendAttitude(dev, _settings[i].mode, data);
      
      _settings[i].lastSent = millis();
    }
  }
}
