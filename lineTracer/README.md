# Robot Racing, Project 1: Line Tracer

### Contributors
Levi Hassel

## Overview
This project uses an Arduino-based robot I designed in my Embedded Systems class. The robot is tasked with using an IR sensor to follow a black line on a variety of white surface courses. Some courses have varying line widths, some have sharp turns and some have long curvy turns - they are rather unpredictable. Speed and precision were the main goals here and I chose to focus on precision with my robot.

## Robot Design
Through the use of excessive amounts of tape, I was able to secure the sensors only one centimeter above the ground, ensuring the most accurate readings possible. Mounting them so close together enables precise bouncing around the edge of lines, but decreases overall speed since so much time will be spent correcting the tiny distance between the sensors.
[design1.jpg]
[design3.png]

## Control Program
Below is a state machine representing my robot’s approach to line-tracing. This approach ensures maximum precision, but tends to be lacking in speed, since it never actually reaches the “Drive Straight” state in practice. Regardless, this state machine shows how my code was designed to perform.
[states.jpg]

## IR Sensor Characterization
### Performance Summary
The graph below shows the readings of each sensor as they’re raised higher from the paper. The sensors seem to be most accurate from 3-4 cm for black readings and 1-2 cm for white readings, though you can clearly see the variance between them anywhere in the 1-5 cm range. 7 cm appears to be the maximum distance where you can differentiate white from black with certainty and once you move up to 9-10 cm, it becomes impossible to tell them apart. The relationship between distance and readings, while not entirely linear, can most easily be represented with a simple linear approximation (dotted line in graph) until 𝑥≥9, where a constant line would suffice. While there isn’t a large discrepancy between the two sensors, the left sensor is consistently more accurate (like I found previously), especially at 1 cm (where my sensors are actually attached). These slight differences could be due to many factors, such as the angle of the lighting or even wear/damage/dust on the right sensor.
[graph1.png]

### Sample Readings
This is a small sample of the sensor readings while following a black line. It turns right when both sensors detect white and turns left whenever the left sensor detects black. As you can see, my robot literally doesn’t go straight. Though it’s designed to go straight when right sees black and left sees white, you’ll notice that this never occurs; the right sensor is on white the entire time, probably because of how I programmed my wheels to turn. While this wasn’t my original intention, I discovered that this approach yielded the best results I’d had and decided to leave the code alone.
[graph2.png]

## Requirements
This code is not going to be very useful unless you have an Arduino-based robot built exactly the way mine was. However, it works great when all you do is look at it. With a correctly built robot, all you need to do is download the Arduino IDE, connect the Arduino Uno to your PC using the USB plug and compile/run the code.

## References
https://www.arduino.cc/en/Main/ArduinoBoardUno
