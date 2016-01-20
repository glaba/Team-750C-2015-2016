/** @file friendly.h
 * @brief Header file for human-readable definitions
 *
 * This file provides constant definitions for various sensory states that appear frequently.
 * Since the button pressed state is represented by an unintuitive LOW value,
 * these constants create definitions for the pressed and unpressed states of a button.
 *
 */

#ifndef FRIENDLY_H_
#define FRIENDLY_H_

/**
 * More readable definition for button pressed/unreleased state.
 */
#define PRESSED LOW

/**
 * More readable definition for button unpressed/released state.
 */
#define UNPRESSED HIGH

/**
 * More readable definition for button unreleased/pressed state.
 */
#define UNRELEASED LOW

/**
 * More readable definition for button released/unpressed state.
 */
#define RELEASED HIGH

#endif

