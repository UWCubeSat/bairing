#ifndef BLUEBOY_TELEMETRY_H_
#define BLUEBOY_TELEMETRY_H_

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "util/PacketSender.h"

#include "Blueboy.h"
#include "BlueboyPeripherals.h"

struct TelemetrySettings {
  AttitudeMode mode;          // attitude logging mode
  unsigned long sendDelay;    // time in milliseconds between sending data log packets
  unsigned long lastSent;     // time that the last data log packet was sent
  bool logging;               // true if currently logging data
};

// time in milliseconds between log data sending
constexpr unsigned long DEFAULT_LOG_DELAY = 200;

// attitude data mode
constexpr AttitudeMode DEFAULT_ATTITUDE_MODE = AttitudeMode::Raw;

class BlueboyTelemetry {
 public:
  // Initialize telemetry to use the given serial stream and sync pattern
  BlueboyTelemetry(AltSoftSerial& serial, uint32_t sync);

  // Initializes peripherals
  bool InitializePeripherals();

  // Update telemetry
  void Tick();

  // Sets the log period in milliseconds of the given device
  void SetLogPeriod(Device dev, unsigned long period);

  // Enable attitude logging on the device with the given mode
  void BeginLogging(Device dev, AttitudeMode mode);

  // Disable attitude logging on the device
  void EndLogging(Device dev);

  // Send a message packet with the given message
  void SendMessage(const char *str);

  // Send an attitude packet belonging to the given device with the given mode
  void SendAttitude(Device dev, AttitudeMode mode, const struct AttitudeData& data);
 private:
  AltSoftSerial& _serial;

  BlueboyPeripherals _peripherals;
  
  char _sendbuf[64];            // send packet buffer, used to build a packet
  PacketSender _sender;         // internal packet sender

  struct TelemetrySettings _settings[2];
};

#endif
