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

/**
 * Object representing the encoder on the horizontal wheel.
 */
Encoder horizontalenc;

/** 
 * Turns the robot right to a specified angle.
 * 
 * @param bodydegs the amount of degrees to turn the robot
 */
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

/** 
 * Turns the robot left to a specified angle.
 * 
 * @param bodydegs the amount of degrees to turn the robot
 */
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

/** 
 * Moves the robot forward a specified distance.
 * 
 * @param inches the amount of inches to move forward
 */
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

/** 
 * Moves the robot backward a specified distance.
 * 
 * @param inches the amount of inches to move forward
 */
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
    "gbusters:d=4,o=5,b=112:16b,16b,8d#6,8b,8c#6,8a,2p,16b,16b,16b,16b,8a,8b,2p,16b,16b,8d#6,8b,8c#6,8a,2p,16b,16b,16b,16b,8a,8c#6,8b",
    "Funky Town:d=8,o=4,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6",
    "Macarena:d=8,o=5,b=180:f,f,f,4f,f,f,f,f,f,f,f,a,c,c,4f,f,f,4f,f,f,f,f,f,f,d,c,4p,4f,f,f,4f,f,f,f,f,f,f,f,a,4p,2c.6,4a,c6,a,f,4p,2p",
    "Mission Impossible:d=16,o=5,b=100:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,p,a#4,c",
    "USA National Anthem:d=8,o=5,b=120:e.,d,4c,4e,4g,4c6.,p,e6.,d6,4c6,4e,4f#,4g.,p,4g,4e6.,d6,4c6,2b,a,4b,c6.,16p,4c6,4g,4e,32p,4c",
    "Bond:d=4,o=4,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6",
    "GoodBad:d=4,o=5,b=56:32p,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,d#,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,c#6,32a#,32d#6,32a#,32d#6,8a#.,16f#.,32f.,32d#.,c#,32a#,32d#6,32a#,32d#6,8a#.,16g#.,d#",
    "MetalGear:d=8,o=6,b=125:4e5,4d5,2c5,d5,e5,a4,4e5,2d5,c5,d5,4e.5,a5,g5,e5,4c5,2d5,e5,a5,2c,b5,c,d,4c,2a5,g5,a5,4b.5,c,4b5,a5,g5,1a5,4b5,4a5,2g5,a5,b5,e5,4b5,2a5,g5,a5,4b.5,e,d,b5,4g5,2a5,b5,e,2g,f#,g,a,4g,2e,d,e,4f#.,g,4f#,e,d,1e",
    "Jeopardy:d=4,o=6,b=125:c,f,c,f5,c,f,2c,c,f,c,f,a.,8g,8f,8e,8d,8c#,c,f,c,f5,c,f,2c,f.,8d,c,a#5,a5,g5,f5,p,d#,g#,d#,g#5,d#,g#,2d#,d#,g#,d#,g#,c.7,8a#,8g#,8g,8f,8e,d#,g#,d#,g#5,d#,g#,2d#,g#.,8f,d#,c#,c,p,a#5,p,g#.5,d#,g#",
    "Michael Jackson - Thriller:d=31,o=5,b=112:24b., 15p, 16d.6, p, 24b., 15p, 9e.6, p, 2d.6, 3p, 9d.6, p, 16c#.6, 10p, 6b., 3p, 24b., 15p, 16b., p, 49a., 10p, 16a., p, 49g., p, 9g., p, 24e., 15p, 12g., 153, 24a., 15p, 16b., p, 24a., 15p, 16a., p, 24g., 153, 16b., 1",
    "GunsNRoses_Welcome_To_The_Jungle:d=4,o=5,b=210:16c, 16p, 16c, 16p, c, 16c., 32p, 16c., 32p, 16c., 32p, 16c., 32p, c, 16a#, 16p, 16c., 32p, 8c, 16a#, 16p, 16a#, 16p, 8c, 16g, 16p, 16g, 16p, 8c, 16f, 16p, 16f, 16p, 8c, 16d#, 8p., 16c, 8p., 16a#, 16p, 8c, 8c, 16a#, 16p, 16a#, 16p, 8c, 16g, 16p, 16g, 16p, 8c, 16f, 16p, 16f, 16p, 8c, 1",
    "guns_n_roses_sweet_child_o_mine:d=4,o=5,b=120:8a#, 8a#, 8f, 8d#, 8d#6, 8f, 8d6, 8f, 8a#, 8a#, 8f, 8d#, 8d#6, 8f, 8d6, 8f, 8c, 8a#, 8f, 8d#, 8d#6, 8f, 8d6, 8f, 8c, 8a#, 8f, 8d#, 8d#6, 8f, 8d6, 8f, 8d#, 8a#, 8f, 8d#, 8d#6, 8f, 8d6, 8f, 8d#, 8a#, 8f, 8d#, 8d#6, 8f, 8d6, 8f, 8a#, 8a#, 8f, 8d#, 8d#6, 8f, 8d6, 8f, 8a#, 8a#, 8f, 8d#, 8d#",
    "MCHammer_UCantTouchThis:d=4,o=5,b=133:8d.6, 32p, 8c6, 8b, 8a, p, 16b., 32p, 8g, p, 16b., 32p, 16a, 16a, 32a, 16p., 32a, 16p., 16a, 16p, 8d.6, 16p, 8c6, 8b, 8a, p, 8e, 8g, p, 16b., 32p, 32a, 16p, 16a, 32a, 16p., 32a, 16p., 16a, 16p, 8d.6, 32p, 8c6, 8b, 8a, p, 16b., 32p, 8g, p, 16b., 32p, 16a, 16a, 32a, 16p., 32a, 16p., 16a, 16p, 8d ",
    "Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6",
    "Zelda1:d=4,o=5,b=125:a#,f.,8a#,16a#,16c6,16d6,16d#6,2f6,8p,8f6,16f.6,16f#6,16g#.6,2a#.6,16a#.6,16g#6,16f#.6,8g#.6,16f#.6,2f6,f6,8d#6,16d#6,16f6,2f#6,8f6,8d#6,8c#6,16c#6,16d#6,2f6,8d#6,8c#6,8c6,16c6,16d6,2e6,g6,8f6,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f,a#,f.,8a#,16a#,16c6,16d6,16d#6,2f6,8p,8f6,16f.6,16f#6,16g#.6,2a#.6,c#7,c7,2a6,f6,2f#.6,a#6,a6,2f6,f6,2f#.6,a#6,a6,2f6,d6,2d#.6,f#6,f6,2c#6,a#,c6,16d6,2e6,g6,8f6,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f",
    "smb:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6",
    "smb_under:d=4,o=6,b=100:32c,32p,32c7,32p,32a5,32p,32a,32p,32a#5,32p,32a#,2p,32c,32p,32c7,32p,32a5,32p,32a,32p,32a#5,32p,32a#,2p,32f5,32p,32f,32p,32d5,32p,32d,32p,32d#5,32p,32d#,2p,32f5,32p,32f,32p,32d5,32p,32d,32p,32d#5,32p,32d#",
    "smbdeath:d=4,o=5,b=90:32c6,32c6,32c6,8p,16b,16f6,16p,16f6,16f.6,16e.6,16d6,16c6,16p,16e,16p,16c",
    "BarryManilow_Copacabana:d=4,o=5,b=120:8a, 8c6, 8d6, 8f.6, 16a#., p., 8f6, 8e6, 8d6, 8e.6, 16a., p., 8a, 8c6, 16d6, 16p, 16e.6, 16f6, 32p, 16e.6, 16f6, 32p, 32e6, 32f6, 8e.6, 16p, 8c6, 8g#, 16b., 32p, 16b, 16p, 8b, 16a, 16b., 16p., 8a, 8c6, 8d6, 8f.6, 8a#, p., 8f6, 8e6, 8d6, 8e.6, 16a., p., 8a, 8c6, 16d.6, 32p, 8e6, 16f6, 16p, 16e6",
    "Imperial:d=4, o=5, b=100:e, e, e, 8c, 16p, 16g, e, 8c, 16p, 16g, e, p, b, b, b, 8c6, 16p, 16g, d#, 8c, 16p, 16g, e, 8p",
    "Rocky:d=4,o=5,b=100:16e,8g.,2a.,16a,8b.,2e.,1 6e,8g.,2a.,16a,8b.,1e,8p,16d,16c,8d.,16c,16d,2e,16p,16c6,16c6,8b,16b,8a,16a,g,8c6,1b"
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
