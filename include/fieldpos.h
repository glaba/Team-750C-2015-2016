/** @file fieldpos.h
 * @brief File for declarations relating to the field positioning system
 *
 * @see fieldpos.c
 */

#ifndef FIELDPOS_H_
#define FIELDPOS_H_

/**
 * Defines the starting position of the robot when it is placed on the field. 
 */
#define ROBOT_START_POSITION_X 0
#define ROBOT_START_POSITION_Y 0

/**
 * Defines the starting angle of the robot when it is placed on the field.
 */
#define ROBOT_START_ANGLE 90

/**
 * @brief Representation of a point.
 */
typedef struct point {
	/**
	 * X-coordinate
	 */
	double x;
	/**
	 * Y-coordinate
	 */
	double y;
} point;

/**
 * @brief Representation of a line segment, which is made up of a start point and an end point.
 */
typedef struct lineSegment {
	/**
	 * The first point in the line segment.
	 */
	point p1;
	/**
	 * The final point in the line segment. 
	 */
	point p2;
} lineSegment;

/**
 * Coordinates of each of the white lines on the field.
 */
extern const lineSegment LINE_COORDS[8];

/**
 * The robot's position.
 */
extern point position;

/**
 * Updates the robot's estimate of its position based on sensor input.
 */
void updatePosition();

/**
 * Resets the robot's position to the given coordinates.
 * 
 * @param x The X-coordinate to reset the robot's position to. 
 * @param y The Y-coordinate to reset the robot's position to. 
 */
void resetPosition(double x, double y);

#endif
