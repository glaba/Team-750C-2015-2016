/** @file autonroutines.h
 * @brief Header file for hard-coded autonomous routines
 *
 * This file contains function declarations and definitions for use in the hard-coded autonomous routines.
 * These functions can be called from other files to run hard-coded autonomous routines.
 */

#ifndef AUTONROUTINES_H
#define AUTONROUTINES_H

/**
 * Angle that the robot must be from the vertical to shoot into the close goal while still being able to turn without hitting the wall.
 */
#define CLOSE_GOAL_ANGLE 5

/**
 * Distance that the robot must travel to reach the other tile to shoot preloads.
 */
#define DISTANCE_TO_OTHER_SIDE 26.162

/**
 * Runs a pre-written programming skills routine using sensors rather than the autonomous recorder.
 */
void runHardCodedProgrammingSkills();

#endif

