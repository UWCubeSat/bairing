#include "SimpleCalibratedSensor.h"

bool SimpleCalibratedSensor::GetEvent(sensors_event_t *event, sensors_type_t type) {
  bool status = GetEventRaw(event, type);
  if (status) {
    Compensate(event, type);
  }
  return status;
}