#!/usr/bin/env python
"""
Provides control for a dual-stepper-motor setup. Allows for two-directional 
movement and rotation at varying speeds. Allows for use of `move_forward()`, 
`move_backward()`, `turn_left()`, `turn_right()`, as well as other helpful 
functions.
"""

import math
import time
from enum import Enum

import RPi.GPIO as GPIO

# Enum for directions
class Direction(Enum):
    FORWARD = 1
    BACKWARD = -1

# Refered constants
STEPS_PER_ROTATION = 50
MINIMUM_MOTOR_DELAY = 0.001

# Half-step stepper motor sequence
HALFSTEP_SEQUENCE = (
    (1, 0, 0, 0),
    (1, 1, 0, 0),
    (0, 1, 0, 0),
    (0, 1, 1, 0),
    (0, 0, 1, 0),
    (0, 0, 1, 1),
    (0, 0, 0, 1),
    (1, 0, 0, 1),
)
# Defines a number of halfsteps in sequence
HALFSTEPS_COUNT = len(HALFSTEP_SEQUENCE)
# Defines the number of pins used in sequence
HALFSTEP_PINS_COUNT = len(HALFSTEP_SEQUENCE[0])

# MOTOR_PUSHER_PINS = (11, 13, 15, 16)
MOTOR_PUSHER_PINS = (31, 33, 35, 37)
MOTOR_LIFT_PINS = (11, 13, 15, 16)



def main() -> None:
    """Runs actions for testing sequence."""
    pin_setup()

    while True:
        try:
            print("Input step count: ")
            steps = input()
            if not isinstance(steps, int):
                raise TypeError('Number of steps must be an int')
            else:
                push(steps)
                
        except:
            break
    

    pin_cleanup()


def pin_setup() -> None:
    """
    Sets up board mode and motor pins.
    """
    # Sets board mode
    GPIO.setmode(GPIO.BOARD)  # type: ignore
    # Sets all motor pins to output and disengages them
    for pin in MOTOR_PUSHER_PINS + MOTOR_LIFT_PINS:
        GPIO.setup(pin, GPIO.OUT)  # type: ignore
        GPIO.output(pin, False)  # type: ignore


def pin_cleanup() -> None:
    """
    Turns off any pins left on.
    """
    GPIO.cleanup()  # type: ignore

def push(step_count: int) -> None:
    """
    Turns motors a number of rotations in an amount of time in a direction.
    """
    delay = 0.1

    # For as many steps as specified:
    if step_count < 0:
        direction = Direction.BACKWARD
    else:
        direction = Direction.FORWARD

    for _ in range(step_count):
        # Move one step in direction
        step(direction, delay)


def step(direction: Direction, delay: float = MINIMUM_MOTOR_DELAY) -> None:
    """
    Moves motors one step in direction. Optional: Step delay.
    """
    # Throws error if moving to quickly
    if delay < MINIMUM_MOTOR_DELAY:
        raise ValueError("Too fast to turn! Use a larger time!")

    # Defines the sequence for each motor from specified direction
    sequences = tuple(HALFSTEP_SEQUENCE[::i] for i in direction.value)
    # For each halfstep in sequence
    for halfstep in range(HALFSTEPS_COUNT):
        # For each pin value
        for pin in range(HALFSTEP_PINS_COUNT):
            # Assigns corresponding motor pins to action from designated sequence
            GPIO.output(MOTOR_PUSHER_PINS[pin], sequences[0][halfstep][pin])  # type: ignore
            GPIO.output(MOTOR_LIFT_PINS[pin], sequences[1][halfstep][pin])  # type: ignore
            # THIS TIMER WORKS BUT SHOULD IN BE WITHIN THIS LOOP OR THE ONE
            # BELOW, BECAUSE DO THE PINS NEED TIME BETWEEN EACH ONE ACTIVATING
            # OR JUST EACH HALFSTEP STAGE??? I'M SCARED TO TRY IT .･(>д<)･. -BK
            time.sleep(delay)

# Runs main only from command line call instead of library call
if __name__ == "__main__":
    main()