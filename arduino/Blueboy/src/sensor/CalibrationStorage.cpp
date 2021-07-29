#include "CalibrationStorage.h"

StorageHandle CalibrationStorage::_registered = 0;

StorageHandle CalibrationStorage::Register() {
  StorageHandle handle = CalibrationStorage::_registered++;
  struct AxisOffsets offsets;
  Fetch(handle, &offsets);
  return handle;
}

void CalibrationStorage::Fetch(StorageHandle handle, struct AxisOffsets *offsets) {
  uint16_t address = _eepromAddress(handle);
  struct StoredCalibration stored;
  
  EEPROM.get(address, stored);
  
  if (stored.canary == STORAGE_CANARY) {
    *offsets = stored.offsets;
  } else {
    offsets->xOff = 0.0;
    offsets->yOff = 0.0;
    offsets->zOff = 0.0;
  }
}

void CalibrationStorage::Update(StorageHandle handle, struct AxisOffsets *offsets) {
  uint16_t address = _eepromAddress(handle);
  struct StoredCalibration stored;
  
  stored.canary = STORAGE_CANARY;
  stored.offsets = *offsets;
  
  EEPROM.put(address, stored);
}

void CalibrationStorage::Clear(StorageHandle handle) {
  uint16_t address = _eepromAddress(handle);
  struct StoredCalibration cleared;
  
  cleared.canary = 0x0000;
  cleared.offsets.xOff = cleared.offsets.yOff = cleared.offsets.zOff = 0.0;
  
  EEPROM.put(address, cleared);
}