#include "CalibratedLIS2MDL.h"

CalibratedLIS2MDL::CalibratedLIS2MDL(): _lis2mdl(Adafruit_LIS2MDL()) {
  _magOffsets.xOff = _magOffsets.yOff = _magOffsets.zOff = 0;
}

bool CalibratedLIS2MDL::Initialize() {
  return _lis2mdl.begin();
}

bool CalibratedLIS2MDL::GetEvent(sensors_event_t *event, sensors_type_t type) {
  bool status = GetEventRaw(event, type);
  if (status) {
    Compensate(event, type);
  }
  return status;
}

bool CalibratedLIS2MDL::GetEventRaw(sensors_event_t *event, sensors_type_t type) {
  bool success = _lis2mdl->getEvent(event);
  
  /*
  Serial.print("Raw: (");
  Serial.print(event->mag.x, 4); Serial.print(", ");
  Serial.print(event->mag.y, 4); Serial.print(", ");
  Serial.print(event->mag.z, 4); Serial.println(")");
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
  if (_currCalibration) {
    // type is not 0, so we were calibrating
    _magOffsets.xOff = (_magLimits.xMin + _magLimits.xMax) / 2.0;
    _magOffsets.yOff = (_magLimits.yMin + _magLimits.yMax) / 2.0;
    _magOffsets.zOff = (_magLimits.zMin + _magLimits.zMax) / 2.0;
    _currCalibration = 0;
  }
}

void CalibratedLIS2MDL::AddCalibrationSample() {
  if (_currCalibration == SENSOR_TYPE_MAGNETIC_FIELD) {
    sensors_event_t event;
    if (GetEventRaw(&event, _currCalibration)) {
      if (_magToDiscard > 0) {
        _magToDiscard--;
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