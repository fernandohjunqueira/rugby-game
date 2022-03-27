// Standard headers
#include <stdio.h>
#include <stdlib.h>

// Internal headers
#include "direction.h"
#include "position.h"
#include "spy.h"

// Main header
#include "defender.h"

// Macros
#define UNUSED(x) (void)(x) // Auxiliary to avoid error of unused parameter

// Tangents 
#define TAN1 0.4142135624 // Tangent of pi/8 radians (22.5°) 
#define TAN2 2.4142135624 // Tangent of 3*pi/8 radians (67.5°)

/*----------------------------------------------------------------------------*/
/*                              PRIVATE FUNCTIONS                             */
/*----------------------------------------------------------------------------*/

// Considering the defender position as the center of the unit circle,
// returns the quadrant in which the attacker is located.
unsigned int move_quadrant(
    position_t defender_position, position_t attacker_position) {

  if (attacker_position.j < defender_position.j){
    if (attacker_position.i < defender_position.i)
      return 2;
    else
      return 3;
  }
  else {
    if (attacker_position.i < defender_position.i)
      return 1;
    else
      return 4;
   }
}

// Returns the tangent from the angle between the defender and the attacker positions.
// Absolute values only, generalizing to the first quadrant of the unit circle.
float calculate_tan(position_t defender_position, position_t attacker_position) {
  unsigned int catA, catO;

  catO = abs((short int) attacker_position.i - (short int) defender_position.i); // opposite cateto.
  catA = abs((short int) attacker_position.j - (short int) defender_position.j); // adjacent cateto.

  if (catA == 0) return INT_MAX; // In case of an angle of pi/2 radians.
  return (float) catO / (float) catA; // Returns the tangent. Always positive.
}

// Based on the quadrant of the unit circle and the tangent of the angle,
// returns the corresponding direction.
direction_t attacker_direction(int quadrant, float tan) {
  if (tan < TAN1) {
    switch(quadrant) {
      case 1:
      case 4:
        return (direction_t) DIR_RIGHT;
      case 2:
      case 3:
        return (direction_t) DIR_LEFT;
    }
  }
  else if (tan > TAN2) {
    switch(quadrant) {
      case 1:
      case 2:
        return (direction_t) DIR_UP;
      case 3:
      case 4:
        return (direction_t) DIR_DOWN;
    }
  }
  else {
    switch(quadrant) {
      case 1:
        return (direction_t) DIR_UP_RIGHT;
      case 2:
        return (direction_t) DIR_UP_LEFT;
      case 3:
        return (direction_t) DIR_DOWN_LEFT;
      case 4:
        return (direction_t) DIR_DOWN_RIGHT;
    }
  }

  return (direction_t) DIR_STAY; // Something failed.
}

// Returns the direction to reach attacker's position from defender's position.
direction_t get_bias_direction(
    position_t defender_position, position_t attacker_position) {
  unsigned int quadrant = move_quadrant(defender_position, attacker_position);
  float tan = calculate_tan(defender_position, attacker_position);
  return attacker_direction(quadrant, tan);

}

// Returns a different direction based on the fail counter.
direction_t change_direction(unsigned int fail_counter) {
  switch(fail_counter) {
    case 1: 
      return (direction_t) DIR_UP_LEFT;
    case 2:
      return (direction_t) DIR_DOWN_LEFT;
    case 3:
      return (direction_t) DIR_UP;
    case 4:
      return (direction_t) DIR_DOWN;
    case 5:
      return (direction_t) DIR_LEFT;
    case 6:
      return (direction_t) DIR_RIGHT;
    default:
      return (direction_t) DIR_STAY; // The defender is trapped.
  }
}

/*----------------------------------------------------------------------------*/
/*                              PUBLIC FUNCTIONS                              */
/*----------------------------------------------------------------------------*/

direction_t execute_defender_strategy(
    position_t defender_position, Spy attacker_spy) {

  static unsigned int turn_counter = 0;
  static unsigned int fail_counter = 0; // Successive movement change fail counter.
  
  static position_t attacker_position = INVALID_POSITION;

  static position_t previous_position = INVALID_POSITION;
  static direction_t previous_move = DIR_STAY;

  static direction_t next_move = DIR_STAY;
  static direction_t bias_direction = DIR_STAY; // Will be used as a bias for the defense based on the initial position.

  // Gets executed as soon as spy is available:
  if (get_spy_number_uses(attacker_spy) < 1) {
    // Spot and store attacker's initial position.
    attacker_position = get_spy_position(attacker_spy); 
    
    // Store direction from defender's initial position to attacker's initial position.
    bias_direction = get_bias_direction(defender_position, attacker_position); 
  }

  // On the first turn, set movement direction to bias.
  if (!turn_counter) next_move = bias_direction; 

  // Changing direction logic below:
  // If failed going on a direction, choose another one.
  if (equal_positions(defender_position, previous_position)) {
    // The while-loop below avoid the case in which you lose another turn by choosing the previous direction by mistake.
    while(next_move.i == previous_move.i && next_move.j == previous_move.j) {
      next_move = change_direction(fail_counter); 
      fail_counter++;
    } 
  }
  else fail_counter = 0; // Succeeded changing direction, reset fail counter.
 
 // Increase turn counter and update previous position and move direction to next iteration.
  turn_counter++;
  previous_move = next_move;
  previous_position = defender_position;
  
  return next_move;
}

/*----------------------------------------------------------------------------*/
