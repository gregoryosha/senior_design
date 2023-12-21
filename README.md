# senior_design
The shaft cutter runs the electronics off a single Arduino integrated with a cnc shield. 

## Electrical Components
List of electrical systems:
* Microcontrollers: Arduino R3 UNO with CNC sheild
* Motors: Stepper motors
* Sensors: Limit switches on each axis 
* Input: LCD screen and rotary encoder

## File Descriptions
`headless_shaft.ino` uses no LCD output and is run directly from the computer serial input. `shaft_LCDmenu.ino` is the fully integrated version, which requires no computer to run the system. All control is input through the LCD menu with the rotary encoder. 

## Prototype 
The earlier prototype runs off a raspberry Pi and has limited cutting ability. It has no ability to discern absolute distance since it has no limit switches. It allows for movement from the current stepper position, which is enough for a full cutting pass. 


