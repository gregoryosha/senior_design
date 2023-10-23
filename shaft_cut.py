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
MINIMUM_MOTOR_DELAY = 0.0001

# Half-step stepper motor sequence
HALFSTEP_SEQUENCE = (
    (1, 0, 1, 0),
    (0, 1, 1, 0),
    (0, 1, 0, 1),
    (1, 0, 0, 1),
)
# Defines a number of halfsteps in sequence
HALFSTEPS_COUNT = len(HALFSTEP_SEQUENCE)
STEP_PER_MM = 6.25
# Defines the number of pins used in sequence
HALFSTEP_PINS_COUNT = len(HALFSTEP_SEQUENCE[0])

# MOTOR_PUSHER_PINS = (31, 33, 35, 37)
MOTOR_PUSHER_PINS = (29, 31, 33, 35)
MOTOR_R_LIFT_PINS = (32, 36, 38, 40)
MOTOR_L_LIFT_PINS = (11, 13, 15, 16)

CUTTER_PIN = 7



def main() -> None:
    """Runs actions for testing sequence."""
    option = 'n'
    pin_setup()
    try: 
        while True:
            print("\nInput pushing distance in mm (negatives are accepted): ")
            dist = int(input())
            move_motor(dist, 0.001, 'push')

            print("\nrun cutting motor? [y/n]")
            option = input()
            if (option == 'y'):
                print("\n Are you sure? [y/n]")
                option == input()
                if (option == 'y'):
                    run_cutter(True)
                else:
                    print("not running motor...")
            elif (option == 'n'):
                print("not running motor...")
            else:
                print("Not an input")


            print("\nInput desired height in mm: ")
            h = int(input())
            move_motor(h, 0.001, 'lift')
    
    except:
        print("Shutting down...")
        pin_cleanup()

        

    

def pin_setup() -> None:
    """
    Sets up board mode and motor pins.
    """
    # Sets board mode
    GPIO.setmode(GPIO.BOARD)  # type: ignore
    # Sets all motor pins to output and disengages them
    for pin in MOTOR_PUSHER_PINS + MOTOR_R_LIFT_PINS + MOTOR_L_LIFT_PINS:
        GPIO.setup(pin, GPIO.OUT)  # type: ignore
        GPIO.output(pin, False)  # type: ignore
    
    #Set up cutting motor
    GPIO.setup(CUTTER_PIN, GPIO.OUT)
    GPIO.output(pin, False)  # type: ignore



def pin_cleanup() -> None:
    """
    Turns off any pins left on.
    """
    GPIO.cleanup()  # type: ignore

def run_cutter(input: bool):
    GPIO.output(CUTTER_PIN, input)


def move_motor(dist: int, delay: float, move_type: str) -> None:
    """
    Turns motors a number of rotations in an amount of time in a direction.
    """
    step_count = int(6.25 * dist)
    if step_count < 0:
        direction = Direction.BACKWARD
    else:
        direction = Direction.FORWARD


    for _ in range(abs(step_count)):
        # Move one step in direction
        step(direction, move_type, delay)



def step(direction: Direction, move_type: str, delay: float = MINIMUM_MOTOR_DELAY) -> None:
    """
    Moves motors one step in direction. Optional: Step delay.
    """
    # Defines the sequence for each motor from specified direction
    sequence = HALFSTEP_SEQUENCE[::direction.value]
    # For each halfstep in sequence
    for halfstep in range(HALFSTEPS_COUNT):
        # For each pin value
        for pin in range(HALFSTEP_PINS_COUNT):
            if (move_type == 'lift'):
                GPIO.output(MOTOR_L_LIFT_PINS[pin], sequence[halfstep][pin])  # type: ignore
                GPIO.output(MOTOR_R_LIFT_PINS[pin], sequence[halfstep][pin])  # type: ignore
            elif (move_type == 'push'):
                GPIO.output(MOTOR_PUSHER_PINS[pin], sequence[halfstep][pin])  # type: ignore
            # Assigns corresponding motor pins to action from designated sequenc
            time.sleep(0.001)
        time.sleep(delay)

    #Set to low
    for pin in range(HALFSTEP_PINS_COUNT):
            if (move_type == 'lift'):
                GPIO.output(MOTOR_L_LIFT_PINS[pin], False)  # type: ignore
                GPIO.output(MOTOR_R_LIFT_PINS[pin], False)  # type: ignore
            elif (move_type == 'push'):
                GPIO.output(MOTOR_PUSHER_PINS[pin], False)  # type: ignore
            # Assigns corresponding motor pins to action from designated sequenc
            time.sleep(0.001)

    
    

# Runs main only from command line call instead of library call
if __name__ == "__main__":
    main()