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
#define TRANSMISSION_POT 1

/**
 * Defines potentiometer values for each transmission gearing.
 */
#define GEAR_DRIVE 1860
#define GEAR_LIFT 4095

#endif
