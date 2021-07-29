#ifndef MESSAGES_H_
#define MESSAGES_H_

// Packet messages stored in flash memory
// Defines are gross, but we're not allowed to use F() outside of functions :(
#define DEFAULT_MSG         F("see how the brain plays around")
#define SETUP_MSG           F("Initialized system")
#define CANT_LOG_MSG        F("Can't log, stop calibrating first")
#define BEGIN_LOG_MSG       F("Began logging")
#define END_LOG_MSG         F("Ended logging")
#define RESET_MSG           F("Resetting system...")
#define UNRECOGNIZED_MSG    F("Unrecognized command")

#define CANT_CALIB_MSG      F("Can't calibrate, stop logging first")
#define BEGIN_CALIB_MSG     F("Began calibration")
#define END_CALIB_MSG       F("Ended calibration")

#endif