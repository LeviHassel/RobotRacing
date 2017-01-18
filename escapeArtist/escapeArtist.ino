#include <Servo.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define SWITCH_LEFT 5
#define SWITCH_RIGHT 3
#define SERVO_REAR_L 6
#define SERVO_REAR_R 9
#define SERVO_FRONT_R 10
#define SERVO_FRONT_L 11
#define IR_LEFT A0
#define IR_MIDDLE A1
#define IR_RIGHT A2
#define SENSOR_SDA A4
#define SENSOR_SCL A5
#define VCNL4000_ADDRESS 0x13
#define COMMAND_0 0x80
#define PRODUCT_ID 0x81
#define IR_CURRENT 0x83
#define AMBIENT_PARAMETER 0x84
#define AMBIENT_RESULT_MSB 0x85
#define AMBIENT_RESULT_LSB 0x86
#define PROXIMITY_RESULT_MSB 0x87
#define PROXIMITY_RESULT_LSB 0x88
#define PROXIMITY_FREQ 0x89
#define PROXIMITY_MOD 0x8A

Servo s_frontLeft;
Servo s_frontRight;
Servo s_rearLeft;
Servo s_rearRight;

unsigned int irLeft_val = 0;
unsigned int irRight_val = 0;
unsigned int irMiddle_val = 0;
unsigned int ambient_val = 0;
unsigned int proximity_val = 0;
unsigned int ambient_initial = 0;
unsigned int proximity_initial = 0;

int maxSpeed = 30;        //Fastest possible
int maxSpeed_RL = 150;
int midSpeed = 70;        //Mid-range
int midSpeed_RL = 110;
int reverse = 150;        //Turns backwards to help with turning
int reverse_RL = 30;
int reverseSlow = 100;
int reverseSlow_RL = 80;
int noSpeed = 90;         //Stops wheel entirely

int switchLeft_val = 0;
int switchRight_val = 0;
int leftCount = 0;
int rightCount = 0;
int lastSwitch = 0;       //0: left, 1: right
int objectDetected = 0;   //0: nope, 1: yeah

/* Initializes everything, runs automatically once uploaded to Arduino */
void setup() {
  //Serial.begin(9600);                         	 //TESTING ONLY, writing to serial frequently slows down other operations

  // Initialize sensors
  analogReference(DEFAULT);
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_MIDDLE, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(SWITCH_LEFT, INPUT_PULLUP);
  pinMode(SWITCH_RIGHT, INPUT_PULLUP);

  // Initialize servos
  s_frontLeft.attach(SERVO_FRONT_L, 0, 180);
  s_frontRight.attach(SERVO_FRONT_R, 0, 180);
  s_rearLeft.attach(SERVO_REAR_L, 0, 180);
  s_rearRight.attach(SERVO_REAR_R, 0, 180);
  s_frontLeft.write(noSpeed);
  s_frontRight.write(noSpeed);
  s_rearLeft.write(noSpeed);
  s_rearRight.write(noSpeed);

  Wire.begin();  // initialize I2C stuff
  
  writeByte(AMBIENT_PARAMETER, 0x0F);
  writeByte(IR_CURRENT, 20);
  writeByte(PROXIMITY_FREQ, 2);
  writeByte(PROXIMITY_MOD, 0x81);
  //Serial.println("Sensors ready");            	 //TESTING ONLY

  //Waits until both bump sensors are pressed simultaneously to begin
  while ((digitalRead(SWITCH_LEFT) == HIGH) || (digitalRead(SWITCH_RIGHT) == HIGH)) {
    //Serial.println("Waiting...");             	 //TESTING ONLY
    delay(300);
  }
  delay(200);

  calibrateProximity();
  //Serial.println("Sensors Calibrated");            //TESTING ONLY
  //Serial.println();                                //TESTING ONLY
  delay(200);
}

/* Loops through this continuously once running */
void loop() {
  readSensors();
  //printSensorValues();        					 //TESTING ONLY
  goFAST();
}

/* Decision-making code for turning and avoiding obstacles */
void goFAST() {
  // Turns away from wall if bump sensor detects wall  
  if (switchLeft_val == LOW) {
    turnRight();
  }
  else if (switchRight_val == LOW) {
    turnLeft();
  }
  // Executes obstacle avoidance if object is detected in front
  if (objectDetected == 1) {
    avoidObstacle();
  }
  // Follows line if nothing is detected
  else {
    followLine();
  }
}

/* Robot follows black line when it is directly under middle IR sensor or turns until this is the case */
void followLine() {
  if (irMiddle_val > 700) {
    goStraight();
  }
  else if (irLeft_val > 700) {
    turnLeft();
  }
  else if (irRight_val > 700) {
    turnRight();
  }
  else {
    goStraight();
  }
}

*/ Very precise turn around the right side of a wooden block of known dimensions */
void avoidObstacle() {
  turnRight();
  delay(1300);
  goStraight();
  delay(1200);
  turnLeft();
  delay(1200);
  goStraight();
  delay(1440);
  turnLeft();
  delay(1200);
  goStraight();
  delay(1450);
  turnRight();
  delay(700);
}

/* Read the bump sensors, IR sensors, proximity sensor and ambient sensor */
void readSensors() {
  switchLeft_val = digitalRead(SWITCH_LEFT);
  switchRight_val = digitalRead(SWITCH_RIGHT);
  irLeft_val = analogRead(IR_LEFT);
  irMiddle_val = analogRead(IR_MIDDLE);
  irRight_val = analogRead(IR_RIGHT);
  proximity_val = readProximity();
  ambient_val = readAmbient();

  // Checks if object is in front of robot and needs to be avoided
  if ((proximity_val > (proximity_initial + 55))) {
    objectDetected = 1;
  }
  else {
    objectDetected = 0;
  }
}

/* Print values of the bump sensors, IR sensors, proximity sensor and ambient sensor for testing purposes */
void printSensorValues() {
  
  Serial.print("Ambient: ");
  Serial.print("\t\t");
  Serial.print(ambient_val);
  Serial.print("\n");
  
  Serial.print("Proximity: ");
  Serial.print("\t\t");
  Serial.print(proximity_val);
  Serial.print("\n");
  
  Serial.print("Left IR: ");
  Serial.print("\t\t");
  Serial.print(irLeft_val);
  Serial.print("\n");

  Serial.print("Middle IR: ");
  Serial.print("\t\t");
  Serial.print(irMiddle_val);
  Serial.print("\n");

  Serial.print("Right IR: ");
  Serial.print("\t\t");
  Serial.print(irRight_val);
  Serial.print("\n");

  Serial.print("Left Switch: ");
  Serial.print("\t\t");
  Serial.print(switchLeft_val);
  Serial.print("\n");

  Serial.print("Right Switch: ");
  Serial.print("\t\t");
  Serial.print(switchRight_val);
  Serial.print("\n");

  delay(3000);
}

/* Calibrate the proximity sensor using an average of 10 readings based on its surroundings */
void calibrateProximity() {
  int prxCnt = 0;
  for (int i = 0; i < 10; i++) {
    prxCnt = prxCnt + readProximity();
  }
  proximity_initial = (prxCnt / 10);
}

/* Calibrate the ambient sensor using an average of 10 readings based on its surroundings */
void calibrateAmbient() {
  int ambCnt = 0;
  for (int i = 0; i < 10; i++) {
    ambCnt = ambCnt + readAmbient();
  }
  ambient_initial = (ambCnt / 10);
}

// The following are all wheel speed configurations that control the direction the robot travels in
// ------------------------------------------------------------------------------------------------

void goStraight() {
  s_frontLeft.write(maxSpeed);
  s_frontRight.write(maxSpeed);
  s_rearLeft.write(maxSpeed_RL);
  s_rearRight.write(maxSpeed);
}

void turnLeft() {
  s_frontLeft.write(reverse_RL);
  s_frontRight.write(maxSpeed_RL);
  s_rearLeft.write(reverse_RL);
  s_rearRight.write(maxSpeed);
}

void turnRight() {
  s_frontLeft.write(maxSpeed_RL);
  s_frontRight.write(reverse_RL);
  s_rearLeft.write(maxSpeed_RL);
  s_rearRight.write(reverse);
}

void stopMotion() {
  s_frontLeft.write(noSpeed);
  s_frontRight.write(noSpeed);
  s_rearLeft.write(noSpeed);
  s_rearRight.write(noSpeed);
}

void reverseStraight() {
  s_frontLeft.write(reverse);
  s_frontRight.write(reverse);
  s_rearLeft.write(reverse_RL);
  s_rearRight.write(reverse);
}

void reverseLeft() {
  s_frontLeft.write(reverse);
  s_frontRight.write(reverseSlow);
  s_rearLeft.write(reverse_RL);
  s_rearRight.write(reverseSlow);
}

void reverseRight() {
  s_frontLeft.write(reverseSlow);
  s_frontRight.write(reverse);
  s_rearLeft.write(reverseSlow_RL);
  s_rearRight.write(reverse);
}

// End section
// ------------------------------------------------------------------------------------------------

/* Read the values recorded by the proximity sensor */
unsigned int readProximity() {
  unsigned int data;
  byte temp;

  temp = readByte(COMMAND_0);
  writeByte(COMMAND_0, temp | 0x08);

  while (!(readByte(COMMAND_0) & 0x20))
    ;
  data = readByte(PROXIMITY_RESULT_MSB) << 8;
  data |= readByte(PROXIMITY_RESULT_LSB);

  return data;
}

/* Read the values recorded by the ambient light sensor */
unsigned int readAmbient() {
  unsigned int data;
  byte temp;

  temp = readByte(COMMAND_0);
  writeByte(COMMAND_0, temp | 0x10);

  while (!(readByte(COMMAND_0) & 0x40))
    ;
  data = readByte(AMBIENT_RESULT_MSB) << 8;
  data |= readByte(AMBIENT_RESULT_LSB);

  return data;
}

/* Used to read data from the sensors */
byte readByte(byte address) {
  byte data;

  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(VCNL4000_ADDRESS, 1);
  while (!Wire.available())
  ;
  data = Wire.read();

  return data;
}

/* Used to send data to the sensors */
void writeByte(byte address, byte data) {
  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

/* Robot follows very closely to wall using bump sensors (not used in final version) */
void followWall() {
  if (switchLeft_val == LOW) {
    lastSwitch = 0;
    if (objectDetected == 1) {
      turnRight();
      rightCount += 2;
    }
    else {
      goStraight();
    }
  }
  else if (switchRight_val == LOW) {
    lastSwitch = 1;
    if (objectDetected == 1) {
      turnLeft();
      leftCount += 2;
    }
    else {
      goStraight();
    }
  }
  else if (leftCount > 0) {
    turnLeft();
    leftCount--;
  }
  else if (rightCount > 0) {
    turnRight();
    rightCount--;
  }
  else if (lastSwitch == 0) {
    turnLeft();
  }
  else {
    turnRight();
  }
}