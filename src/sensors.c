/** @file sensors.c
 * @brief File for important sensor declarations and functions
 *
 * This file contains the code for declarations and functions regarding sensors.
 * The definitions contained herein define objects representing more complex sensors.
 * The functions contained herein are too complex to be defined as inline functions in the sensors.h file.
 *
 * See the sensors.h file for the basic sensory definitions and functions.
 *
 * @see sensors.h
 */

#include "main.h"

/** 
 * Object representing the encoder on the left side of the drivetrain.
 */
Encoder leftenc;

/** 
 * Object representing the encoder on the right side of the drivetrain.
 */
Encoder rightenc;

void rturn(int bodydegs) {
	clearDriveEncoders();
	float turndeg;
	float encdegperbodydeg = DRIVE_WHEELBASE / (DRIVE_DIA * DRIVE_GEARRATIO);
	turndeg = encdegperbodydeg * bodydegs;

	while(abs(encoderGet(rightenc)) < abs(turndeg)) {
		move(0, MOTOR_MAX);
        delay(20);
	}

	move(0, 0);
	clearDriveEncoders();
}

void lturn(int bodydegs) {
	clearDriveEncoders();
	float turndeg;
	float encdegperbodydeg = DRIVE_WHEELBASE / (DRIVE_DIA * DRIVE_GEARRATIO);
	turndeg = encdegperbodydeg * bodydegs;

	while(abs(encoderGet(leftenc)) < abs(turndeg)) {
		move(0, -MOTOR_MAX);
        delay(20);
	}

	move(0, 0);
	clearDriveEncoders();
}

void goForward(int inches) {
	int deg;
	float encperinch;
	encperinch = 360/(DRIVE_DIA * PI * DRIVE_GEARRATIO);
	deg = encperinch * (float)inches;

	clearDriveEncoders();
	while(abs(encoderGet(rightenc)) <  abs(deg))
	{
		move(sign(inches) * 127, 0);
		delay(20);
	}
	move(0,0);
	clearDriveEncoders();
}

void goBackward(int inches) {
	goForward(-inches);
}

/**
 * Object representing the gyroscope.
 */
Gyro gyro;

/**
 * Object representing the ultrasonic sensor.
 */
Ultrasonic sonar;

