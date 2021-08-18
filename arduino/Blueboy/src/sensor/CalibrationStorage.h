/*!
 * @file CalibrationStorage.h
 * @author Sebastian S.
 * @brief Declaration for CalibrationStorage and adjacent types.
 */

#ifndef CALIBRATION_STORAGE_H_
#define CALIBRATION_STORAGE_H_

#include <Arduino.h>
#include <EEPROM.h>

/*!
 * @typedef StorageHandle
 * Handle to be used to access locations in EEPROM
 */
typedef uint8_t StorageHandle;

/*!
 * @var uint16_t STORAGE_CANARY
 * Value that must exist before any calibration for it to be considered valid
 */
constexpr uint16_t STORAGE_CANARY = 0xA5A5;

/*!
 * @struct AxisOffsets
 * Representation of offsets to a three-axis sensor calibration.
 */
struct AxisOffsets {
  float xOff;
  float yOff;
  float zOff;
};

/*!
 * @struct StoredCalibration
 * Representation of an AxisOffsets preceded by space for STORAGE_CANARY.
 */
struct StoredCalibration {
  uint16_t canary;              // equal to STORAGE_CANARY if a valid value has been written in this space
  struct AxisOffsets offsets;   // calibration offsets
};

/*!
 * @class CalibrationStorage
 * @brief Stores AxisOffsets of calibrated sensors in the EEPROM.
 */
class CalibrationStorage {
 public: 
  /*!
   * @brief Initializes the EEPROM for calibration storage.
   */
  static void Initialize() { _registered = 0; }
  
  /*!
   * @brief Registers space for an AxisOffsets
   * @return a StorageHandle corresponding with the newly allocated space
   */
  static StorageHandle Register();
  
  /*!
   * @brief Fetches the AxisOffsets stored at the space allocated by the given storage handle
   * @param handle StorageHandle to access the AxisOffsets of
   * @param offsets Pointer to an AxisOffsets to fill with data
   */
  static void Fetch(StorageHandle handle, struct AxisOffsets *offsets);
  
  /*!
   * @brief Stores the given AxisOffsets at the space allocated by the given storage handle
   * @param handle StorageHandle of the space the new AxisOffsets should be placed at
   * @param offsets Pointer to the AxisOffsets to store in the EEPROM
   */
  static void Update(StorageHandle handle, struct AxisOffsets *offsets);
  
  /*!
   * @brief Clears the stored AxisOffsets at the space allocated by the given storage handle
   * @param handle StorageHandle of the space that should be cleared
   */
  static void Clear(StorageHandle handle);
 
  CalibrationStorage() = delete;
  CalibrationStorage(const CalibrationStorage &) = delete;
  CalibrationStorage &operator=(const CalibrationStorage &) = delete;
 private:
  static StorageHandle _registered;

  /*!
   * @var uint16_t AddressOffset
   * The location in the EEPROM to start storing calibration offsets.
   */
  static constexpr uint16_t AddressOffset = 0x100;
  
  /*!
   * @brief Translates a StorageHandle to its corresponding EEPROM address
   * @return The address in the EEPROM corresponding with the handle
   */
  static uint16_t _eepromAddress(StorageHandle handle) {
    return AddressOffset + handle * sizeof(struct StoredCalibration);
  }
};

#endif