# Robot Racing, Project 2: Escape Artist

### Contributors
Levi Hassel and Riley Smith

## Overview
This project uses an Arduino-based robot we designed in our Embedded Systems class. The goal was to build a robot capable of following black lines, detecting objects in its environment and avoiding obstacles, whilst moving very fast. It was to face off against another robot in an arena by following a line and avoiding obstacles (as well as the opponent) in a race to get to the opposite side first. The robot is fully autonomous.
![alt text](https://cloud.githubusercontent.com/assets/9669653/22046556/11dbd766-dce7-11e6-9a4f-c97d38055161.png "Project Overview")

## Robot Design
Though it may look messy, it’s all in the name of streamlining, lacking even a single breadboard and containing an additional Arduino Uno (for the sole purpose of powering two of the servos). We used multiple servos, wheels, IR sensors, proximity sensors and bump sensors in combination with two Arduino Unos in our build. Everything is connected pin-to-pin or by securing wires together, either by tying them around each other or clamping them in bundles using alligator clips. While this minimalist approach made troubleshooting much simpler, it also drastically increased the amount of troubleshooting required (the connections were very unstable and bumping anything usually resulted in a non-functioning robot). To resolve this issue, all of the connections were coated in a generous volume of a conductive glue.
[design1.png]
[design2.png]

## Control Program
Here’s a general outline of the program:
1. Initializes all variables/connections and begins the servo/sensor processes
2 Calibrates the sensors by moving a small amount to detect even incremental changes in the readings, and then averages those readings out to create a threshold
3. Endlessly follows a black line using IR sensors unless…
   a. One of the bump sensors is pressed in by a wall, in which it turns the opposite direction to move away from the wall
   b. The proximity sensor detects an object in front of the robot, which triggers a function for precisely turning around a wooden block
4. After either of this trigger events have been completed, it returns to Step 3

## Sensor Characterizations
### Range Finder
#### Initialization & Calibration
We ending up using the default settings for all variables (frequency, current etc.) set during the initialization stage of the VCNL4000 proximity sensor. After an excessive amount of experimenting with different values for the variables, we deduced that the performance gains weren’t worth the troubles and unpredictability that came with them. We conducted only a very simplistic calibration for our proximity sensor, which consisted of taking the average of ten measurements during the setup stage. It was more or less in line with the datasheet’s measurements, though it did not remain accurate at the distances described, as you’ll see in the next section.

#### Performance Summary
This plot shows proximity readings as the distance between it and the object facing it varies:
[plot.png]
As you can see, it did not give very useful readings (at least with our setup). We tried to compensate for this by designing the obstacle avoidance function in a way that would allow for it to quickly turn very tightly away from the object it detected a centimeter or two away. For the most part, we were able to get around this shortcoming and still found a way to best utilize our (rather unimpressive) proximity sensor.

#### Interference
Though we noticed marginally different readings based on the lighting of the room, they never seemed substantial enough to affect our performance much. We performed a few rather basic experiments to test this and despite seeing clear changes in the readings, they were always relative to our initial measurements (which is all our code compared subsequent readings to), so lighting didn’t end up becoming much of a problem with our approach. We didn’t test in environments with other robot's proximity sensors enough to fully measure their effect on our range finder, but it’s possible they had a negative effect on our performance.

### Line Sensor
We used three line sensors, which were all placed under the front of the robot. With the addition of a third IR sensor, and the knowledge that we only needed to follow a straight line, we were able to radically simplify the basic line following code. We designed it to go straight if the middle sensor detects black (regardless of the other sensors’ readings), turn right if only the right sensor detects black and turn left if only the left sensor detects black. Rather than using a complex calibration technique like in Project 1, it was adequate to simply compare the readings to a predetermined constant. If the sensors read a value higher than 700, it was counted as black. While this method might seem too straightforward to work well, testing actually showed far better performance and accuracy than we saw in Project 1.

### Bump Sensors
The bump sensors, as digital pins, are pretty self-explanatory. They return 1 when not pressed and 0 when pressed in. The idea was to use these to alter the control flow based on which side of the robot was touching something, a concept that worked very reliably for us.

## Requirements
This code is not going to be very useful unless you have an Arduino-based robot built exactly the way mine was. However, it works great when all you do is look at it. With a correctly built robot, all you need to do is download the Arduino IDE, connect the Arduino Uno to your PC using the USB plug and compile/run the code.

## References
https://www.arduino.cc/en/Main/ArduinoBoardUno
https://www.sparkfun.com/products/retired/10901




