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
 * Defines the encoder ports for the horizontal wheel.
 */
#define HORIZONTAL_ENC_TOP -1
#define HORIZONTAL_ENC_BOT -1

/**
 * Object representing the encoder on the horizontal wheel. 
 */
extern Encoder horizontalenc;

/** 
 * Clears the drive encoders by resetting their value to zero.
 */
inline void clearDriveEncoders(){
    encoderReset(leftenc);
    encoderReset(rightenc);
    encoderReset(horizontalenc);
}

void resetEncoderVariables();

/** 
 * Turns the robot left to a specified angle.
 * 
 * @param bodydegs the amount of degrees to turn the robot
 */
void lturn(int bodydegs);

/** 
 * Turns the robot right to a specified angle.
 * 
 * @param bodydegs the amount of degrees to turn the robot
 */
void rturn(int bodydegs);

/** 
 * Moves the robot forward a specified distance.
 * 
 * @param inches the amount of inches to move forward
 */
void goForward(int inches);

/** 
 * Moves the robot backward a specified distance.
 * 
 * @param inches the amount of inches to move backward
 */
void goBackward(int inches);

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
 * This varies by hardware revision. This value is for hardware revision A2.
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
 * Defines the proportional error-correction term for the gyroscope alignment velocity control loop.
 */
#define GYRO_KP 6

/**
 * Defines the integral (accumulated) error-correction term for the gyroscope alignment velocity control loop.
 */
#define GYRO_KI 0

/**
 * Defines the derivative (change) error-correction term for the gyroscope alignment velocity control loop.
 */
#define GYRO_KD /*7050*/ 3825

/**
 * Defines the proportional error-correction term for the encoder alignment control loop.
 */
#define ENCODER_KP 6

/**
 * Defines the integral (accumulated) error-correction term for the encoder alignment control loop.
 */
#define ENCODER_KI 0

/**
 * Defines the derivative (change) error-correction term for the encoder alignment control loop.
 */
#define ENCODER_KD 0

/**
 * Defines the number of inches per each encoder tick. 
 */
#define INCHES_PER_ENC_TICK ((DRIVE_DIA * MATH_PI * DRIVE_GEARRATIO) / 360.0)

/**
 * Object representing the gyroscope.
 */
extern Gyro gyro;

/**
 * Defines the port for the limit switch that is triggered when the shooter fires.
 */
#define SHOOTER_LIMIT 10

/**
 * Defines the port for the ultrasonic echo wire (orange).
 */
#define ULTRASONIC_ECHO_PORT 11

/**
 * Defines the port for the ultrasonic ping wire (yellow).
 */
#define ULTRASONIC_PING_PORT 12

/**
 * Object representing the ultrasonic sensor.
 */
extern Ultrasonic sonar;

/**
 * Defines the port that the line tracker is plugged into. 
 */
#define LINE_TRACKER_PORT -1

/**
 * Defines the threshold line tracker value for a white line. 
 */
#define LINE_THRESHOLD -1

/**
 * Defines the amount of songs in the master list.
 */
#define SONG_COUNT 26

/** 
 * Master list of all songs.
 */
extern char* songs[];

/**
 * Object representing the speaker song task.
 * The speaker song task runs in a separate thread from the operator control code.
 * This prevents it from blocking the driving code from executing.
 */
TaskHandle speakerTask;

/** 
 * Plays a song over the speaker.
 * This task plays a random song from the array of songs.
 *
 * @param ignore does nothing - required by task definition
 */
void playSpeaker(void *ignore);

/** 
 * Returns a random song from the master list.
 * 
 * @return a pointer to the song
 */
char* randsong();

#endif

