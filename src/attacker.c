// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Internal headers
#include "direction.h"
#include "position.h"
#include "spy.h"

// Main header
#include "attacker.h"

// Macros
#define UNUSED(x) (void)(x) // Auxiliary to avoid error of unused parameter

/*----------------------------------------------------------------------------*/
/*                              PRIVATE FUNCTIONS                             */
/*----------------------------------------------------------------------------*/

direction_t initial_diagonal() {
  srand(time(NULL));
  if (rand() % 2)
    return (direction_t) DIR_UP_RIGHT;
  else
    return (direction_t) DIR_DOWN_RIGHT;
}

direction_t invert_diagonal(direction_t next_move) {
  if (next_move.i == 1)
    return (direction_t) DIR_UP_RIGHT;
  else
    return (direction_t) DIR_DOWN_RIGHT;
}

direction_t move_straight(unsigned int fail_counter) {
  switch(fail_counter) {
    case 1:
      return (direction_t) DIR_RIGHT;
    case 2:
      return (direction_t) DIR_DOWN;
    case 3:
      return (direction_t) DIR_UP;
    case 4:
      return (direction_t) DIR_LEFT;
    default:
      return (direction_t) DIR_STAY;
  }
}

/*----------------------------------------------------------------------------*/
/*                              PUBLIC FUNCTIONS                              */
/*----------------------------------------------------------------------------*/

direction_t execute_attacker_strategy(
    position_t attacker_position, Spy defender_spy) {

  // I didn't find useful to use spy on my attacker strategy.
  UNUSED(defender_spy);

  static unsigned int turn_counter = 0; 
  static unsigned int fail_counter = 0;  // Successive movement change fail counter
  
  static position_t previous_position = INVALID_POSITION;
  static direction_t next_move = DIR_STAY;

  // On the first move, choose one of the diagonals to the right.
  if (!turn_counter) next_move = initial_diagonal();

  // Changing direction logic below:
  // If failed going on the first diagonal, try the other one to the right.
  if (equal_positions(attacker_position, previous_position) && fail_counter < 1) {
    next_move = invert_diagonal(next_move);
    fail_counter++;
  } // If both failed, try moving straight on the 4 different directions
  else if (equal_positions(attacker_position, previous_position)) {
    next_move = move_straight(fail_counter);
    fail_counter++;
  } // Succeeded changing direction, reset fail counter.
  else
    fail_counter = 0;
    
  // Increase turn counter and update previous position to next iteration.
  turn_counter++;
  previous_position = attacker_position;

  return next_move; 
}

/*----------------------------------------------------------------------------*/
