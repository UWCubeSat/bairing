/*!
 * @file CalibratedLIS2MDL.h
 * @author Sebastian S.
 * @brief Implementation of CalibratedLIS2MDL.h
 */

#include "CalibratedLIS2MDL.h"

CalibratedLIS2MDL::CalibratedLIS2MDL(): _lis2mdl(Adafruit_LIS2MDL()) {
  _handle = CalibrationStorage::Register();
  FetchCalibration();
}

bool CalibratedLIS2MDL::Initialize() {
  bool began = _lis2mdl.begin();
  if (began) {
    Serial.println(F("Stored magnetometer calibration offsets: "));
    Serial.print(F("  x: ")); Serial.println(_magOffsets.xOff);
    Serial.print(F("  y: ")); Serial.println(_magOffsets.yOff);
    Serial.print(F("  z: ")); Serial.println(_magOffsets.zOff);
  }
  return began;
}

bool CalibratedLIS2MDL::GetEventRaw(sensors_event_t *event, sensors_type_t type) {
  bool success = _lis2mdl.getEvent(event);
  
  // swap x and y axes and invert them to match axes on the LSM6DS33
  float tmp = event->magnetic.x;
  event->magnetic.x = -event->magnetic.y;
  event->magnetic.y = -tmp;
  
  /*
  Serial.print("Raw: (");
  Serial.print(event->magnetic.x, 4); Serial.print(", ");
  Serial.print(event->magnetic.y, 4); Serial.print(", ");
  Serial.print(event->magnetic.z, 4); Serial.println(")");
  //*/
  
  return success;
}

void CalibratedLIS2MDL::BeginCalibration(sensors_type_t type) {
  if (type == SENSOR_TYPE_MAGNETIC_FIELD) {
    _currCalibration = type;
    
    // just initialize to BIG values, assuming we read at least one sample and this doesn't become real data
    // if we ever read larger than 1000 or smaller than -1000 something went REALLY wrong
    _magLimits.xMin = _magLimits.yMin = _magLimits.zMin = 1000;
    _magLimits.xMax = _magLimits.yMax = _magLimits.zMax = -1000;
    _magToDiscard = 10;
  }
}

void CalibratedLIS2MDL::EndCalibration() {
  if (_currCalibration) {  // type is not 0, so we were calibrating
    // Set offsets to be the midpoints of axis limits
    _magOffsets.xOff = (_magLimits.xMin + _magLimits.xMax) / 2.0;
    _magOffsets.yOff = (_magLimits.yMin + _magLimits.yMax) / 2.0;
    _magOffsets.zOff = (_magLimits.zMin + _magLimits.zMax) / 2.0;
    _currCalibration = 0;
    
    UpdateCalibration();
  }
}

void CalibratedLIS2MDL::AddCalibrationSample() {
  if (_currCalibration) {
    sensors_event_t event;
    if (GetEventRaw(&event, _currCalibration)) {
      if (_magToDiscard > 0) {
        _magToDiscard--;  // Discard the first few samples
      } else {
        _magLimits.xMin = min(_magLimits.xMin, event.magnetic.x);
        _magLimits.yMin = min(_magLimits.yMin, event.magnetic.y);
        _magLimits.zMin = min(_magLimits.zMin, event.magnetic.z);
        
        /*
        Serial.print("min: (");
        Serial.print(_magLimits.xMin, 4); Serial.print(", ");
        Serial.print(_magLimits.yMin, 4); Serial.print(", ");
        Serial.print(_magLimits.zMin, 4); Serial.print(")\t");
        //*/
        
        _magLimits.xMax = max(_magLimits.xMax, event.magnetic.x);
        _magLimits.yMax = max(_magLimits.yMax, event.magnetic.y);
        _magLimits.zMax = max(_magLimits.zMax, event.magnetic.z);
        
        /*
        Serial.print("max: (");
        Serial.print(_magLimits.xMax, 4); Serial.print(", ");
        Serial.print(_magLimits.yMax, 4); Serial.print(", ");
        Serial.print(_magLimits.zMax, 4); Serial.println(")");
        //*/
      }
    }
  }
}

void CalibratedLIS2MDL::Compensate(sensors_event_t *reading, sensors_type_t type) {
  reading->magnetic.x -= _magOffsets.xOff;
  reading->magnetic.y -= _magOffsets.yOff;
  reading->magnetic.z -= _magOffsets.zOff;
}
