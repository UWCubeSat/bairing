/*!
 * @file CalibratedBNO080.h
 * @author Sebastian S.
 * @brief Implementation of CalibratedBNO080.h
 */

#include "CalibratedBNO080.h"

// CalibratedBNO080::CalibratedBNO080() { }

bool CalibratedBNO080::Initialize() {
  return false;
}

bool CalibratedBNO080::GetEventRaw(sensors_event_t *event, sensors_type_t type) {
  return false;
}

bool CalibratedBNO080::GetOrientationQuaternion(struct Quaternion *quaternion) {
  return false;
}

bool CalibratedBNO080::GetOrientationEulers(struct Vector *eulers) {
  return false;
}

void CalibratedBNO080::BeginCalibration(sensors_type_t type) {
  
}

void CalibratedBNO080::GetCalibration(struct AxisOffsets *offsets, sensors_type_t type) {
}

void CalibratedBNO080::EndCalibration() {
  
}

void CalibratedBNO080::ClearCalibration(sensors_type_t type) {
  
}