/** @file fieldpos.c
 * @brief File for field positiong system declarations and functions.
 *
 * @see fieldpos.h
 */

#include "main.h"

/**
 * X-coordinate of the robot's position.
 */
double xPosition;

/**
 * Y-coordinate of the robot's position.
 */
double yPosition;

/**
 * Coordinates of each of the white lines on the field (Index 0: Start X, Index 1: Start Y, Index 2: End X, Index 3: End Y)
 */
const int LINE_COORDS[8][4] = {{0, 48, 48, 0}, {0, 48, 48, 48}, {48, 0, 48, 48}, {96, 0, 144, 96}, {96, 0, 96, 96}, {96, 96, 144, 96}, {48, 48, 120, 120}, {96, 96, 24, 120}};

/**
 * Updates the robot's estimate of its position based on sensor input.
 */
void updatePosition() {
	double gyroValue = (double)(gyroGet(gyro) % ROTATION_DEG * MATH_PI / 180.0);
	double gyroVectorX = cos(gyroValue);
	double gyroVectorY = sin(gyroValue);
	double dr = (double) (0.5 * (INCHES_PER_ENC_TICK * encoderGet(leftenc) + INCHES_PER_ENC_TICK * encoderGet(rightenc)));
	double dh = (double) (INCHES_PER_ENC_TICK * encoderGet(horizontalenc));
	
	xPosition += dr * gyroVectorX + dh * cos(gyroValue + MATH_PI / 2);
	yPosition += dr * gyroVectorY + dh * sin(gyroValue + MATH_PI / 2);

	if (analogRead(LINE_TRACKER_PORT) < LINE_THRESHOLD) {
		double lowestDist = 100000.0;
		double lowestX = -1;
		double lowestY = -1; // the coordinate of the closest distance
		int lowestIndex = -1;
		for (int i = 0; i < 8; i++) {
			const int *coords = LINE_COORDS[i];
			double dist;
			//check if the point of intersection with the line is actually on the line segment
			//if the angle from each of the ends to the point is acute, then this is true (dot product)
			double angle1 = (xPosition - coords[0]) * (coords[2] - coords[0]) + (yPosition - coords[1]) * (coords[3] - coords[1]);
			double angle2 = (xPosition - coords[2]) * (coords[0] - coords[2]) + (yPosition - coords[3]) * (coords[1] - coords[3]);
			angle1 /= (sqrt((double)(pow(xPosition-coords[0],2)+pow(yPosition-coords[1],2)))*sqrt((double)(pow(coords[2]-coords[0],2)+pow(coords[3]-coords[1],2))));
			angle2 /= (sqrt((double)(pow(xPosition-coords[2],2)+pow(yPosition-coords[3],2)))*sqrt((double)(pow(coords[2]-coords[0],2)+pow(coords[3]-coords[1],2))));
			angle1 = acos(angle1);
			angle2 = acos(angle2);
			if (angle1 > MATH_PI / 2) {
				dist = sqrt((double)(pow(xPosition-coords[0],2)+pow(yPosition-coords[1],2)));
				lowestX = coords[0];
				lowestY = coords[1];
			} else if (angle2 > MATH_PI / 2) {
				dist = sqrt((double)(pow(xPosition-coords[2],2)+pow(yPosition-coords[3],2)));
				lowestX = coords[2];
				lowestY = coords[3];
			} else {
				dist = ((double) abs((coords[3]-coords[1])*xPosition-(coords[2]-coords[0])*yPosition+coords[2]*coords[1]-coords[3]*coords[0])) / sqrt((double)(pow(coords[3]-coords[1],2)+pow(coords[2]-coords[1],2)));
				
			}

			if (dist < lowestDist) {
				lowestDist = dist;
				lowestIndex = i;
			}
		}

		xPosition = lowestX;
		yPosition = lowestY;
	}

	//implement line tracker code if needed
}

/**
 * Resets the robot's position to the given coordinates.
 * 
 * @param x The X-coordinate to reset the robot's position to. 
 * @param y The Y-coordinate to reset the robot's position to. 
 */
void resetPosition(double x, double y) {
	xPosition = x;
	yPosition = y;
}