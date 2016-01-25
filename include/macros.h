/** @file macros.h
 * @brief Header file for macro definitions 
 *
 * This file provides macro definitions for various basic functions that come about frequently.
 */

#ifndef MACROS_H_
#define MACROS_H_

/** 
 * Returns the minimum of the two values.
 * 
 * @param a the first input value
 * @param b the second input value
 * 
 * @return the lesser of the two values
 */
#define min(a,b) ((a)<(b)?(a):(b))

/** 
 * Returns the minimum of the two values.
 * 
 * @param a the first input value
 * @param b the second input value
 * 
 * @return the lesser of the two values
 */
#define MIN(a,b) ((a)<(b)?(a):(b))

/** 
 * Returns the maximum of the two values.
 * 
 * @param a the first input value 
 * @param b the second input value
 * 
 * @return the greater of the two values
 */
#define max(a,b) ((a)>(b)?(a):(b))

/** 
 * Returns the maximum of the two values.
 * 
 * @param a the first input value 
 * @param b the second input value
 * 
 * @return the greater of the two values
 */
#define MAX(a,b) ((a)>(b)?(a):(b))

/** 
 * Returns the absolute value of the input value.
 * 
 * @param x the input value
 * 
 * @return the absolute value of x
 */
#define abs(x) ((x)>0?(x):-(x))

/** 
 * Constrains a value to a set of boundaries.
 * 
 * @param amt the value to constrain
 * @param low the lower bound
 * @param high the higher bound
 * 
 * @return high, amt, or low if amt is higher, in between, or lower than the range specified, respectively
 */
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

/**
 * Rounds a value to the nearest integer.
 *
 * @param x the value to round
 * 
 * @return the rounded value
 */
#define round(x) (((x) >=0) ?(long)((x)+0.5):(long)((x)-0.5))

/** 
 * Returns the sign of the input value.
 * 
 * @param x the value to determine the sign of
 * 
 * @return -1, 0, or 1 if the value is negative, zero, or positive, respectively
 */
#define sign(x) ((x)>0?1:((x)<0?-1:0))

/** 
 * Converts an angle measure in degrees to radians.
 * 
 * @param deg the angle measure in degrees
 * 
 * @return the angle measure in radians
 */
#define radians(deg) ((deg)*DEG_TO_RAD)

/** 
 * Converts an angle measure in degrees to radians.
 * 
 * @param rad the angle measure in radians
 * 
 * @return the angle measure in degrees
 */
#define degrees(rad) ((rad)*RAD_TO_DEG)

/** 
 * Squares the input value.
 * 
 * @param x the value to square
 * 
 * @return the square of the input value
 */
#define sq(x) ((x)*(x))

/** 
 * Squares the input value.
 * 
 * @param x the value to square
 * 
 * @return the square of the input value
 */
#define SQ(x) ((x)*(x))

#endif

