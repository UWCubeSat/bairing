#ifndef CALIBRATION_STORAGE_H_
#define CALIBRATION_STORAGE_H_

#include <Arduino.h>
#include <EEPROM.h>

typedef uint8_t StorageHandle;

// Value the two bytes before a calibration must be to be valid
constexpr uint16_t STORAGE_CANARY = 0xA5A5;

struct AxisOffsets {
  float xOff;
  float yOff;
  float zOff;
};

struct StoredCalibration {
  uint16_t canary;              // equal to STORAGE_CANARY if a valid value has been written in this space
  struct AxisOffsets offsets;   // calibration offsets
};

// Stores AxisOffsets of calibrated sensors in the EEPROM
class CalibrationStorage {
 public: 
  static void Initialize() { _registered = 0; }
  
  // Registers a space for an axis offset, returning a handle to be used to access that space in the future
  static StorageHandle Register();
  
  // Fetches the AxisOffsets stored in the space registered for handle and returns it through offsets
  static void Fetch(StorageHandle handle, struct AxisOffsets *offsets);
  
  // Stores the AxisOffsets pointed to by offsets in the space registered for handle
  static void Update(StorageHandle handle, struct AxisOffsets *offsets);
  
  // Clears the AxisOffsets in the space registered for handle
  static void Clear(StorageHandle handle);
 
  CalibrationStorage() = delete;
  CalibrationStorage(const CalibrationStorage &) = delete;
  CalibrationStorage &operator=(const CalibrationStorage &) = delete;
 private:
  static StorageHandle _registered;

  static constexpr uint16_t AddressOffset = 0x100;
  
  static uint16_t _eepromAddress(StorageHandle handle) {
    return AddressOffset + handle * sizeof(struct StoredCalibration);
  }
};

#endif