/*!
 * @file Messages.h
 * @author Sebastian S.
 * @brief Defines for messages stored in flash memory.
 *
 * The F() macro, which stores strings in flash memory, cannot be used outside of functions, so to
 * use it to save on dynamic memory we can't store them as variables
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

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
#define CLEAR_CALIB_MSG     F("Cleared calibration")

#endif