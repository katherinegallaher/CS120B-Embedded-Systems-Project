**High Level Description of custom lab:**

My lab is a rhythm game reminiscent of the game of Guitar Hero. It uses a SNES controller as input and an LCD screen along with LED lights to indicate which arrow(s) to press. The game begins with a start screen stating the name of the game and to press start to begin. When the user presses the start button on the SNES controller the game will begin. A timer at the top right of the LCD screen indicates how much time is left for the level below that tells you your score. There are four LED lights below the first four columns of the LCD screen. Each of the first four columns belongs to an arrow. Arrows will scroll up to the top of the LCD screen and the user will input the combination.


**User Guide:**

**-Rules: **

The level is 60 seconds long and your score begins at 10 points.
For every correct arrow combination that is input from the controller you will receive 2 
additional points, however if you input the wrong combination you will lose 3 points of your 
current score. If your score drops below 0 the level will end 	and you will see a game over 
screen. If you complete the level (do not drop below  0 points) you will encounter a level 
complete screen that will display your final score.

**-Controls:**

Up(directional pad): corresponds with the up arrow
Down(directional pad): corresponds with the down arrow
Left(directional pad): corresponds with the left arrow
Right(directional pad): corresponds with the right arrow
Start button: Is used to start the game

**Technologies and components used:**

-Atmel Studio 6.2
-Atmega1284P
-SNES Controller
-LCM-S01602DTR/M


**Demo Video Link: **

http://youtu.be/yDwu2wxXmJE

**Source Files:**

finalproj.c: contains SM code and runs SMs.

**Resource Links:**

LCD Custom Characters:
http://www.circuitvalley.com/2012/02/lcd-custom-character-hd44780-16x2.html
http://www.8051projects.net/lcd-interfacing/introduction.php

SNES Interfacing
http://uzebox.org/files/NES-controller-Hydra-Ch6All-v1.0.pdf

io.h: contains code for the LCD
io.c: contains code for the LCD, LCD_DisplayString() was modified slightly
timer.h: contains timer code for avr
scheduler.h: contains code to implement the scheduler
bit.h: contains bit access functions

**Representations of State Machines Used(.sm files):**

arrows SM
Clock & Final Display SM
Current Pattern SM
Read SNES Buttons SM
Points SM
Start Screen SM
