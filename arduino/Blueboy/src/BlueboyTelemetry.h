/*!
 * @file BlueboyTelemetry.h
 * @author Sebastian S.
 * @brief Definition for BlueboyTelemetry and adjacent utility types.
 */
#ifndef BLUEBOY_TELEMETRY_H_
#define BLUEBOY_TELEMETRY_H_

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "util/PacketSender.h"

#include "Blueboy.h"
#include "BlueboyPeripherals.h"

/*!
 * @struct TelemetrySettings
 * @brief Representation of a device's telemtry settings.
 */
struct TelemetrySettings {
  AttitudeMode mode;          //!< attitude logging mode
  unsigned long sendDelay;    //!< time in milliseconds between sending data log packets
  unsigned long lastSent;     //!< time that the last data log packet was sent
  bool logging;               //!< true if currently logging data
};

/*!
 * @var long Default time in milliseconds between data packets being sent
 */
constexpr unsigned long DEFAULT_LOG_DELAY = 200;

/*!
 * @var AttitudeMode Default attitude mode for data to be sent in
 */
constexpr AttitudeMode DEFAULT_ATTITUDE_MODE = AttitudeMode::Raw;

/*!
 * @class BlueboyTelemetry
 * @brief Collects data from Blueboy and the test system and sends telemetry.
 */
class BlueboyTelemetry {
 public:
  /*!
   * @brief Initialize telemetry to use the given serial stream and sync pattern
   * @param serial Reference to the AltSoftSerial stream to write bytes to
   * @param sync Sync pattern in little endian to send before each packet
   */
  BlueboyTelemetry(AltSoftSerial& serial, BlueboyPeripherals& peripherals, uint32_t sync);

  /*!
   * @brief Initializes connected peripherals, including onboard sensors and the mounted test system
   * @return True if all peripherals were successfully initialized
   */
  bool InitializePeripherals();

  /*!
   * @brief Updates the telemetry handler
   */
  void Tick();

  /*!
   * @brief Sets the log period of the given device
   * @param dev Device to change the log period of
   * @param period logging period in milliseconds
   */
  void SetLogPeriod(Device dev, unsigned long period);

  /*!
   * @brief Enables attitude logging on the given device with the given mode
   * @param dev Device to begin logging from
   * @param mode Attitude mode data should be sent in
   */
  void BeginLogging(Device dev, AttitudeMode mode);

  /*!
   * @brief Disables attitude logging on the given device
   * @param dev Device to end logging from
   */
  void EndLogging(Device dev);

  /*!
   * @brief Sends a message packet with the given message
   * @param str Null-terminated string to send
   */
  void SendMessage(const char *str);
  
  /*!
   * @brief Sends a message packet with the given message
   * @param str Null-terminated string in flash memory to send
   */
  void SendMessage(const __FlashStringHelper *str);

  /*!
   * @brief Sends an attitude packet belonging to the given device with the given mode
   * @param dev Device to send attitude data from
   * @param mode Mode to send attitude data in
   * @param data Attitude data to send
   */
  void SendAttitude(Device dev, AttitudeMode mode, const struct AttitudeData& data);
  
  /*!
   * @param dev The device to check for logging status
   * @return True if the given device is currently logging
   */
  bool Logging(Device dev);
 private:
  AltSoftSerial& _serial;

  BlueboyPeripherals& _peripherals;
  
  char _sendbuf[64];            // send packet buffer, used to build a packet
  PacketSender _sender;         // internal packet sender

  struct TelemetrySettings _settings[2];
};

#endif
