// ===========
// Line Tracer
// ===========

#include <Servo.h>

#define BUTTON 2
#define SPEAKER 3
#define RED1 6
#define RED2 5
#define RED3 10
#define GREEN 12    
#define RIGHT_IR A2
#define LEFT_IR A3

Servo leftWheel;
Servo rightWheel;

int leftIR_val = 0;
int rightIR_val = 0;   
int basicSpeed = 55;
int rotateSpeed = 50;
int slowSpeed = 110;
int leftSpeed = basicSpeed;
int rightSpeed = basicSpeed;
int prevDirection = 0;         // 0 (Straight), 1 (Left) or 2 (Right)

/* Initializes everything, runs automatically once uploaded to Arduino */
void setup() {
  analogReference(DEFAULT);
  pinMode(RED1, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(RED3, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(LEFT_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);

  //Minimum Speed = 90, Maximum Speed = 50
  leftWheel.attach(9,0,180);
  rightWheel.attach(11,0,180);    
  leftWheel.write(90);
  rightWheel.write(90);
  
  digitalWrite(RED1, HIGH);
  tone(SPEAKER, 400, 400);
  delay(100);
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
    analogWrite(RED1, fadeValue);             
    delay(13);                            
  }     
  delay(200);
  
  digitalWrite(RED2, HIGH);
  tone(SPEAKER, 400, 400);
  delay(100);
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
    analogWrite(RED2, fadeValue);             
    delay(13);                            
  }
  delay(200);
  
  digitalWrite(RED3, HIGH);
  tone(SPEAKER, 400, 400);
  delay(100);
  for(int fadedValue = 255 ; fadedValue >= 0; fadedValue -=5) { 
    analogWrite(RED3, fadedValue);             
    delay(13);                         
  }
  delay(200);
  
  digitalWrite(GREEN, HIGH);
  tone(SPEAKER, 600, 500);
  leftWheel.write(rotateSpeed);
  rightWheel.write(rotateSpeed);
}

/* Loops through this continuously once setup is complete */
void loop() {
  readSensors();
  
  if ((rightIR_val < 200)&&(leftIR_val < 200)) {
    if(prevDirection == 0) {
      straight();
    }
    else if(prevDirection == 1) {
      turnRight();
    }
    else {
      turnLeft();
    }
  }
  else if ((rightIR_val > 200)) {
    straight();
    prevDirection = 2;
  }
  else if ((leftIR_val > 200)&&(rightIR_val < 200)) {
    turnRight();
    prevDirection = 1;
  }
  else {
    turnLeft();
    prevDirection = 2;
  }     
  
  // Send the speed values to the motors
  leftWheel.write(leftSpeed);
  rightWheel.write(rightSpeed);
}

/* Read the values of the IR sensors */
void readSensors() {
  leftIR_val = analogRead(LEFT_IR);
  rightIR_val = analogRead(RIGHT_IR);
}

/* Sets the robot to go straight */
void straight() {
  leftSpeed = basicSpeed;
  rightSpeed = basicSpeed;
}

/* Sets the robot to turn left */
void turnLeft() {
  leftSpeed = slowSpeed;
  rightSpeed = rotateSpeed;
}

/* Sets the robot to turn right */
void turnRight() {
  leftSpeed = rotateSpeed;
  rightSpeed = slowSpeed;
}
