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
 * Defines potentiometer values for each transmission gearing.
 */
#define GEAR_DRIVE 1860
#define GEAR_LIFT 4095

#define POWER_EXPANDER_STATUS 3
#define POWER_EXPANDER_VOLTAGE_DIVISOR 280 //Hardware revision A2

/** 
 * Returns the electric potential of the power expander battery in millivolts.
 * 
 * @return the power expander battery voltage, in millivolts
 */
inline unsigned int powerLevelExpander(){
    return analogRead(POWER_EXPANDER_STATUS)*1000/POWER_EXPANDER_VOLTAGE_DIVISOR;
}

#define NUM_BATTS 3

#define BATT_MAIN 0
#define BATT_BKUP 1
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
#define GYRO_NET_TARGET 38

/**
 * Object representing the gyroscope.
 */
extern Gyro gyro;

#endif
