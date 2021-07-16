#ifndef BLUEBOY_TELEMETRY_H_
#define BLUEBOY_TELEMETRY_H_

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "PacketSender.h"
#include "OneUDriver.h"

constexpr uint8_t ONEU_ADDRESS = 0x3A;

// Telemetry IDs
enum class TelemetryID {
  AttitudeRaw = 0x01,
  AttitudeQuaternion = 0x02,
  AttitudeEuler = 0x03,
  Message = 0x69,
};

// Raw attitude data in 3 ways:
//   - Access as array of 9 floats with data
//   - Access each component as an array of 3 floats with mag/gyro/acc
//   - Access each individual float by name
struct AttitudeDataRaw {
  union {
    float data[9];
    struct {
      union {
        float mag[3];
        struct {
          float magX, magY, magZ;     // magnetometor, microteslas
        };
      };
      union {
        float gyro[3];
        struct {
          float gyroX, gyroY, gyroZ;  // gyroscope, rad/s
        };
      };
      union {
        float acc[3];
        struct {
          float accX, accY, accZ;     // accelerometer, m/s^2
        };
      };
    };
  };
};

// time in milliseconds between log data sending
constexpr unsigned long DEFAULT_LOG_DELAY = 200;

class BlueboyTelemetry {
 public:
  // Initialize telemetry to use the given serial stream and sync pattern
  BlueboyTelemetry(AltSoftSerial& serial, uint32_t sync);

  // Update telemetry
  void Tick();

  // Enable attitude logging
  void BeginLogging();

  // Disable attitude logging
  void EndLogging();

  // Send a message packet with the given message
  void SendMessage(const char *str);

  // Send a raw attitude data packet with the given attitude
  void SendAttitudeRaw(const struct AttitudeDataRaw& data);
 private:
  AltSoftSerial& _serial;
  
  char _sendbuf[128];           // send packet buffer, used to build a packet
  PacketSender _sender;         // internal packet sender

  OneUDriver _oneU;             // driver for attached 1U system
  
  unsigned long _sendDelay;     // time in milliseconds between sending data log packets
  unsigned long _lastSent;      // time that the last data log packet was sent
  bool _logging;                // true if Blueboy is currently logging data
};

#endif
