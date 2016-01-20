/** @file sensors.h
 * @brief File for important sensor declarations and functions
 *
 * This file contains the code for declarations and functions regarding sensors.
 * The definitions contained herein define sensor ports.
 * The functions contained herein process certain sensor values for later use.
 *
 * Some functions defined herein are too complex to be defined as inline functions in the sensors.h file.
 * Additionally, some sensors must be instantiated as object types.
 * See the sensors.c file for these more object instantiations and function definitions
 *
 * @see sensors.c
 */

#ifndef SENSORS_H_
#define SENSORS_H_

/**
 * Defines the encoder ports on the left side of the drivetrain.
 */
#define LEFT_ENC_TOP 1
#define LEFT_ENC_BOT 2

/** 
 * Object representing the encoder on the left side of the drivetrain.
 */
extern Encoder leftenc;

/**
 * Defines the encoder ports on the right side of the drivetrain.
 */
#define RIGHT_ENC_TOP 3
#define RIGHT_ENC_BOT 4

/** 
 * Object representing the encoder on the right side of the drivetrain.
 */
extern Encoder rightenc;

/**
 * Defines the transmission potentiometer for position determination.
 */
#define TRANSMISSION_POT 2

/**
 * Defines potentiometer values for drive gearing.
 */
#define GEAR_DRIVE 1860

/**
 * Defines potentiometer values for lift gearing.
 */
#define GEAR_LIFT 4095

/**
 * Defines power expander status port.
 * This is used to get the power expander battery voltage.
 */
#define POWER_EXPANDER_STATUS 3

/**
 * Defines power expander divisor.
 * The sensor's value is divided by this to get the battery voltage.
 */
#define POWER_EXPANDER_VOLTAGE_DIVISOR 280 //Hardware revision A2

/** 
 * Returns the electric potential of the power expander battery in millivolts.
 * 
 * @return the power expander battery voltage, in millivolts
 */
inline unsigned int powerLevelExpander(){
    return analogRead(POWER_EXPANDER_STATUS)*1000/POWER_EXPANDER_VOLTAGE_DIVISOR;
}

/**
 * The number of batteries present on the robot.
 */
#define NUM_BATTS 3

/**
 * Battery ID number of the robot's main battery.
 */
#define BATT_MAIN 0

/**
 * Battery ID number of the robot's backup battery.
 */
#define BATT_BKUP 1

/**
 * Battery ID number of the power expander's battery.
 */
#define BATT_PEXP 2

/**
 * Defines the port for the gyroscope.
 */
#define GYRO_PORT 4

/**
 * Defines the gyroscope sensitivity. A value of zero represents the default sensitivity.
 */
#define GYRO_SENSITIVITY 0

/**
 * Defines gyroscope target angle for the opposite corner net.
 */
#define GYRO_NET_TARGET 0

/**
 * Object representing the gyroscope.
 */
extern Gyro gyro;

#endif
