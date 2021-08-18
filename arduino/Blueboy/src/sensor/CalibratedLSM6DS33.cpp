#include "CalibratedLSM6DS33.h"

CalibratedLSM6DS33::CalibratedLSM6DS33(): _lsm6ds33(Adafruit_LSM6DS33()) {
  CalibrationStorage::Register();  // dummmy, save a space for accelerometer
  _handle = CalibrationStorage::Register();
  FetchCalibration();
}

bool CalibratedLSM6DS33::Initialize() {
  bool began = _lsm6ds33.begin_I2C();
  if (began) {
    Serial.println(F("Stored gyroscope calibration offsets: "));
    Serial.print(F("  x: ")); Serial.println(_gyroOffsets.xOff);
    Serial.print(F("  y: ")); Serial.println(_gyroOffsets.yOff);
    Serial.print(F("  z: ")); Serial.println(_gyroOffsets.zOff);
  }
  return began;
}

bool CalibratedLSM6DS33::GetEventRaw(sensors_event_t *event, sensors_type_t type) {
  bool success;
  float tmp;
  switch (type) {
    case SENSOR_TYPE_GYROSCOPE:
      success = _lsm6ds33.getGyroSensor()->getEvent(event);
      
      // x = -y
      // y = -z
      // z = -x
      /*
      tmp = event->gyro.x;
      event->gyro.x = -event->gyro.y;
      event->gyro.y = -event->gyro.z;
      event->gyro.z = -tmp;
      */
      
      /*
      // modify vector s.t. we get readings with right hand axes
      event->gyro.z = -event->gyro.z;
      &/
      
      /*
      Serial.print("Raw: (");
      Serial.print(event->gyro.x, 4); Serial.print(", ");
      Serial.print(event->gyro.y, 4); Serial.print(", ");
      Serial.print(event->gyro.z, 4); Serial.println(")");
      //*/
      
      return success;
    case SENSOR_TYPE_ACCELEROMETER:
      success = _lsm6ds33.getAccelerometerSensor()->getEvent(event);
      
      tmp = event->acceleration.x;
      event->acceleration.x = -event->acceleration.y;
      event->acceleration.y = -event->acceleration.z;
      event->acceleration.z = -tmp;
      
      /*
      event->acceleration.z = -event->acceleration.z;
      */
      
      /*
      Serial.print("Raw: (");
      Serial.print(event->acceleration.x, 4); Serial.print(", ");
      Serial.print(event->acceleration.y, 4); Serial.print(", ");
      Serial.print(event->acceleration.z, 4); Serial.println(")");
      //*/
      
      return success;
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
      return _lsm6ds33.getTemperatureSensor()->getEvent(event);
    default:
      return false;
  }
}

void CalibratedLSM6DS33::BeginCalibration(sensors_type_t type) {
  if (type == SENSOR_TYPE_GYROSCOPE) {
    _currCalibration = type;
    
    // just initialize to BIG values, assuming we read at least one sample and this doesn't become real data
    // if we ever read larger than 1000 or smaller than -1000 something went REALLY wrong
    _gyroLimits.xMin = _gyroLimits.yMin = _gyroLimits.zMin = 1000;
    _gyroLimits.xMax = _gyroLimits.yMax = _gyroLimits.zMax = -1000;
    _gyroToDiscard = 10;
  }
}

void CalibratedLSM6DS33::EndCalibration() {
  if (_currCalibration) {  // type is not 0, so we were calibrating
    _gyroOffsets.xOff = (_gyroLimits.xMin + _gyroLimits.xMax) / 2.0;
    _gyroOffsets.yOff = (_gyroLimits.yMin + _gyroLimits.yMax) / 2.0;
    _gyroOffsets.zOff = (_gyroLimits.zMin + _gyroLimits.zMax) / 2.0;
    _currCalibration = 0;
    
    UpdateCalibration();
  }
}

void CalibratedLSM6DS33::AddCalibrationSample() {
  if (_currCalibration == SENSOR_TYPE_GYROSCOPE) {
    sensors_event_t event;
    if (GetEventRaw(&event, _currCalibration)) {
      if (_gyroToDiscard > 0) {
        _gyroToDiscard--;
      } else {
        _gyroLimits.xMin = min(_gyroLimits.xMin, event.gyro.x);
        _gyroLimits.yMin = min(_gyroLimits.yMin, event.gyro.y);
        _gyroLimits.zMin = min(_gyroLimits.zMin, event.gyro.z);
        
        /*
        Serial.print("min: (");
        Serial.print(_gyroLimits.xMin, 4); Serial.print(", ");
        Serial.print(_gyroLimits.yMin, 4); Serial.print(", ");
        Serial.print(_gyroLimits.zMin, 4); Serial.print(")\t");
        //*/
        
        _gyroLimits.xMax = max(_gyroLimits.xMax, event.gyro.x);
        _gyroLimits.yMax = max(_gyroLimits.yMax, event.gyro.y);
        _gyroLimits.zMax = max(_gyroLimits.zMax, event.gyro.z);
        
        /*
        Serial.print("max: (");
        Serial.print(_gyroLimits.xMax, 4); Serial.print(", ");
        Serial.print(_gyroLimits.yMax, 4); Serial.print(", ");
        Serial.print(_gyroLimits.zMax, 4); Serial.println(")");
        //*/
      }
    }
  }
}

void CalibratedLSM6DS33::Compensate(sensors_event_t *reading, sensors_type_t type) {
  if (type == SENSOR_TYPE_GYROSCOPE) {
    reading->gyro.x -= _gyroOffsets.xOff;
    reading->gyro.y -= _gyroOffsets.yOff;
    reading->gyro.z -= _gyroOffsets.zOff;
  }
}