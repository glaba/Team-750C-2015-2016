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

/** 
 * Master list of all songs.
 */
char* songs[] = {
    "Batman:d=8,o=5,b=180:d,d,c#,c#,c,c,c#,c#,d,d,c#,c#,c,c,c#,c#,d,d#,c,c#,c,c,c#,c#,f,p,4f",
    "Spiderman:d=4,o=6,b=200:c,8d#,g.,p,f#,8d#,c.,p,c,8d#,g,8g#,g,f#,8d#,c.,p,f,8g#,c.7,p,a#,8g#,f.,p,c,8d#,g.,p,f#,8d#,c,p,8g#,2g,p,8f#,f#,8d#,f,8d#,2c",
    "Star Wars:d=8,o=6,b=180:f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c,4p,f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c",
    "Final Countdown:d=16,o=5,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b",
    "Deep Purple-Smoke on the Water:d=4,o=4,b=112:c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c,2p,8p,c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c",
    "Ghost Busters:d=8,o=5,b=145:16c6,32p,16c6,e6,c6,d6,a#,2p,32c6,32p,32c6,32p,c6,a#,c6",
    "Fresh Prince Of Bel Air Theme:d=4,o=5,b=112:16a, 16p, 16a, 8p, 8a, 32d7, 32d#7, 16e7, 8p, 8e7, 8p, 32c7, 32c#7, 8d7, 16d#7, 16d7, 16p, 16c7, 16d6, 16p, 16c6, 16p, 8b, 8p, 16a, 16p, 16a, 8p, 8a6, 32d7, 32d#7, 16e7, 8p, 8e7, 8p, 32c7, 32c#7, 8d7, 16d#7, 16d7, 16p, 16c7, 16a, 16p, 16e6, 16p, 16a, 8p, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6, 32b, 32c6",
    "Funky Town:d=8,o=4,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6",
    "Macarena:d=8,o=5,b=180:f,f,f,4f,f,f,f,f,f,f,f,a,c,c,4f,f,f,4f,f,f,f,f,f,f,d,c,4p,4f,f,f,4f,f,f,f,f,f,f,f,a,4p,2c.6,4a,c6,a,f,4p,2p",
    "Mission Impossible:d=16,o=5,b=100:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,p,a#4,c",
    "USA National Anthem:d=8,o=5,b=120:e.,d,4c,4e,4g,4c6.,p,e6.,d6,4c6,4e,4f#,4g.,p,4g,4e6.,d6,4c6,2b,a,4b,c6.,16p,4c6,4g,4e,32p,4c",
    "Bond:d=4,o=5,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6",
    "GoodBad:d=4,o=5,b=56:32p,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,d#,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,c#6,32a#,32d#6,32a#,32d#6,8a#.,16f#.,32f.,32d#.,c#,32a#,32d#6,32a#,32d#6,8a#.,16g#.,d#",
    "MetalGear:d=8,o=6,b=125:4e5,4d5,2c5,d5,e5,a4,4e5,2d5,c5,d5,4e.5,a5,g5,e5,4c5,2d5,e5,a5,2c,b5,c,d,4c,2a5,g5,a5,4b.5,c,4b5,a5,g5,1a5,4b5,4a5,2g5,a5,b5,e5,4b5,2a5,g5,a5,4b.5,e,d,b5,4g5,2a5,b5,e,2g,f#,g,a,4g,2e,d,e,4f#.,g,4f#,e,d,1e",
    "Jeopardy:d=4,o=6,b=125:c,f,c,f5,c,f,2c,c,f,c,f,a.,8g,8f,8e,8d,8c#,c,f,c,f5,c,f,2c,f.,8d,c,a#5,a5,g5,f5,p,d#,g#,d#,g#5,d#,g#,2d#,d#,g#,d#,g#,c.7,8a#,8g#,8g,8f,8e,d#,g#,d#,g#5,d#,g#,2d#,g#.,8f,d#,c#,c,p,a#5,p,g#.5,d#,g#"
};

/** 
 * Returns a random song from the master list.
 * 
 * @return a pointer to the song
 */
char* randsong() {
    int index = rand() % SONG_COUNT;
    return songs[index];
}

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
void playSpeaker(void *ignore){
    char *rtttl = randsong();
    speakerPlayRtttl(rtttl);
    speakerTask = NULL;
}
