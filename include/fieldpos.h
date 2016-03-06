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
 * Coordinates of each of the white lines on the field (Index 0: Start X, Index 1: Start Y, Index 2: End X, Index 3: End Y)
 */
extern const int LINE_COORDS[8][4];

/**
 * X-coordinate of the robot's position.
 */
extern double xPosition;

/**
 * Y-coordinate of the robot's position.
 */
extern double yPosition;

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
