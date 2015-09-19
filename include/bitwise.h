#ifndef BITWISE_H_
#define BITWISE_H_

/**
 * Reads the value of a bit (1 or 0) from a variable.
 *
 * @param value the variable to read from
 * @param bit the bit number to read (0 being the rightmost)
 *
 * @return the value of the bit (1 or 0)
 */
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

/**
 * Sets the value of a bit in a variable to 1.
 *
 * @param value the variable to set in
 * @param bit the bit number to set (0 being the rightmost)
 */
#define bitSet(value, bit) ((value) |= (1UL << (bit)))

/**
 * Clears the value of a bit in a variable to 0.
 *
 * @param value the variable to clear in
 * @param bit the bit number to clear (0 being the rightmost)
 */
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))

/** 
 * Writes a value (1 or 0) to a bit in a variable.
 * 
 * @param value the variable to write in
 * @param bit the bit number to set (0 being the rightmost)
 * @param bitvalue the value to write (1 or 0)
 */
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#endif
