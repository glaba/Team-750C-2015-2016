/** @file fieldpos.c
 * @brief File for field positiong system declarations and functions.
 *
 * @see fieldpos.h
 */

#include "main.h"

/**
 * The robot's position.
 */
point position;

/**
 * Coordinates of each of the white lines on the field.
 */
const lineSegment LINE_COORDS[8] = {{.p1 = {.x = 0, .y = 48}, .p2 = {.x = 48, .y = 0}},
									{.p1 = {.x = 0, .y = 48}, .p2 = {.x = 48, .y = 48}},
									{.p1 = {.x = 48, .y = 0}, .p2 = {.x = 48, .y = 48}},
									{.p1 = {.x = 96, .y = 0}, .p2 = {.x = 144, .y = 96}},
									{.p1 = {.x = 96, .y = 0}, .p2 = {.x = 96, .y = 96}},
									{.p1 = {.x = 96, .y = 96}, .p2 = {.x = 144, .y = 96}},
									{.p1 = {.x = 48, .y = 48}, .p2 = {.x = 120, .y = 120}},
									{.p1 = {.x = 96, .y = 96}, .p2 = {.x = 24, .y = 120}}};

/**
 * Updates the robot's estimate of its position based on sensor input.
 */
void updatePosition() {
	double gyroValue = (double)((gyroGet(gyro) % ROTATION_DEG) * MATH_PI / 180.0);
	double gyroVectorX = cos(gyroValue);
	double gyroVectorY = sin(gyroValue);
	double dr = (double) (0.5 * (INCHES_PER_ENC_TICK * (double) encoderGet(leftenc) + INCHES_PER_ENC_TICK * (double) encoderGet(rightenc)));
	double dh = (double) (INCHES_PER_ENC_TICK * (double) encoderGet(horizontalenc));
	
	position.x += dr * gyroVectorX + dh * cos(gyroValue + MATH_PI / 2);
	position.y += dr * gyroVectorY + dh * sin(gyroValue + MATH_PI / 2);

	if (analogRead(LINE_TRACKER_PORT) < LINE_THRESHOLD) {
		double lowestDist = 100000.0;
		double lowestX = -1;
		double lowestY = -1; // the coordinate of the closest distance
		for (int i = 0; i < 8; i++) {
			const lineSegment line = LINE_COORDS[i];
			double dist;
			//check if the point of intersection with the line is actually on the line segment
			//if the angle from each of the ends to the point is acute, then this is true (dot product)
			double angle1 = (position.x - line.p1.x) * (line.p2.x - line.p1.x) + (position.y - line.p1.y) * (line.p2.y - line.p1.y);
			double angle2 = (position.x - line.p2.x) * (line.p1.x - line.p2.x) + (position.y - line.p2.y) * (line.p1.y - line.p2.y);
			angle1 /= (sqrt((double)(pow(position.x-line.p1.x,2)+pow(position.y-line.p1.y,2)))*sqrt((double)(pow(line.p2.x-line.p1.x,2)+pow(line.p2.y-line.p1.y,2))));
			angle2 /= (sqrt((double)(pow(position.x-line.p2.x,2)+pow(position.y-line.p2.y,2)))*sqrt((double)(pow(line.p2.x-line.p1.x,2)+pow(line.p2.y-line.p1.y,2))));
			angle1 = acos(angle1);
			angle2 = acos(angle2);
			if (angle1 > MATH_PI / 2) {
				dist = sqrt((double)(pow(position.x-line.p1.x,2)+pow(position.y-line.p1.y,2)));
				lowestX = line.p1.x;
				lowestY = line.p1.y;
			} else if (angle2 > MATH_PI / 2) {
				dist = sqrt((double)(pow(position.x-line.p2.x,2)+pow(position.y-line.p2.y,2)));
				lowestX = line.p2.x;
				lowestY = line.p2.y;
			} else {
				dist = ((double) abs((line.p2.y-line.p1.y)*position.x-(line.p2.x-line.p1.x)*position.y+line.p2.x*line.p1.y-line.p2.y*line.p1.x)) / sqrt((double)(pow(line.p2.y-line.p1.y,2)+pow(line.p2.x-line.p1.y,2)));
				double r = (position.x-line.p1.x)*(line.p2.x-line.p1.x) + (position.y-line.p1.y)*(line.p2.y-line.p1.y);
				r /= (line.p2.x*line.p2.x+line.p2.y*line.p2.y+line.p1.x*line.p1.x+line.p1.y*line.p1.y-2*line.p1.x*line.p2.x-2*line.p1.y*line.p2.y);
				lowestX = line.p1.x+r*(line.p2.x-line.p1.x);
				lowestY = line.p1.y+r*(line.p2.y-line.p1.y);
			}

			if (dist < lowestDist) {
				lowestDist = dist;
			}
		}

		position.x = lowestX;
		position.y = lowestY;
	}
    clearDriveEncoders();
}

/**
 * Resets the robot's position to the given coordinates.
 * 
 * @param x The X-coordinate to reset the robot's position to. 
 * @param y The Y-coordinate to reset the robot's position to. 
 */
void resetPosition(double x, double y) {
	position.x = x;
	position.y = y;
}

