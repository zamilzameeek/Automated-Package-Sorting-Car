#include <AccelStepper.h>
#include <Servo.h>

#define HALFSTEP 8  // Define the step mode (HALFSTEP) for smoother movement

// Motor pin definitions
#define motorPin1  4     // Define motor pin 1, connected to IN1 on the ULN2003 driver
#define motorPin2  3     // Define motor pin 2, connected to IN2 on the ULN2003 driver
#define motorPin3  6    // Define motor pin 3, connected to IN3 on the ULN2003 driver
#define motorPin4  7    // Define motor pin 4, connected to IN4 on the ULN2003 driver

//Color sensor pin definition
#define s0 A3       
#define s1 A4
#define s2 A0
#define s3 A1
#define out 2

//Buzzer pin definition
#define buzzer 5
#define volume 25

// Initialize the stepper motor with the defined pin sequence (IN1, IN3, IN2, IN4)
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// Servo pin definitions
Servo servo0;
Servo servo1;

const int servoPin0 = 0;
const int servoPin1 = 1;

// Define steps for the stepper motor
const int stepsToLift = 0;    // Adjust this value based on your stepper motor setup
const int stepsToLower = 600; // Initial position

// Motor control pins
const int IN1 = 9;
const int IN2 = 8;
const int ENA = 10; // PWM pin for speed control of Left Motor (Motor A)

const int IN3 = 13;
const int IN4 = 12;
const int ENB = 11; // PWM pin for speed control of Right Motor (Motor B)

int red = 0;  
int green = 0;  
int blue = 0; //RGB initial value

bool boxGrabbed = false;

void setup() { 
  //Buzzer
  pinMode(buzzer,OUTPUT);

  //Set color sensor pins as outputs
   pinMode(s0, OUTPUT);   
   pinMode(s1, OUTPUT);
   pinMode(s2, OUTPUT);
   pinMode(s3, OUTPUT);
   pinMode(out, INPUT);
   digitalWrite(s0, HIGH); //Putting S0/S1 on HIGH/HIGH levels means the output frequency scaling is at 100% 
   digitalWrite(s1, HIGH); 

  // Set the motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Initialize the servos
  servo0.attach(servoPin0);
  servo1.attach(servoPin1);

  // Initialize the stepper motor
  stepper1.setMaxSpeed(1000.0);       // Set the maximum speed for the stepper motor (steps per second)
  stepper1.setAcceleration(1000.0);    // Set the acceleration rate for the stepper motor (steps per second^2)
  stepper1.setSpeed(1000);             // Set the initial speed for the stepper motor (steps per second)

  // Set initial position for stepper motor and servos
  stepper1.setCurrentPosition(0);
  servo0.write(190);  // Initialize servos to release position
  servo1.write(80);   // Initialize servos to release position
}

void loop() {
  delay(5000);
  detectcolor();
  if (red > green && red > blue){

    beep(3);  //three beeps to signal red box

    if (!boxGrabbed) {
      pickup();
      boxGrabbed = true; // Set flag to indicate box is grabbed
      delay(1000);
    }

    moveleft(0, 150);
    delay(1500);
    stopMotors();

    dropoff();
    boxGrabbed = false;
    delay(2500);

    moveleft(0, 150);
    delay(1200);
    stopMotors();
  }
  else if (blue > red && blue > green) {

    beep(2);  //two beeps to signal blue box

    if (!boxGrabbed) {
      pickup();
      boxGrabbed = true; // Set flag to indicate box is grabbed
      delay(1000);
    }

    moveleft(0, 150);
    delay(2000);
    stopMotors();

    dropoff();
    boxGrabbed = false;
    delay(2000);

    moveleft(0, 150);
    delay(1200);
    stopMotors();
  }
  else if (green > red || green > blue) {

    beep(1);  //one beep to signal green box

    if (!boxGrabbed) {
      pickup();
      boxGrabbed = true; // Set flag to indicate box is grabbed
      delay(1000);
    }

    moveleft(0, 150);
    delay(2500);
    stopMotors();

    dropoff();
    boxGrabbed = false;
    delay(2500);

    moveleft(0, 150);
    delay(1500);
    stopMotors();
  }
  else {
    analogWrite(buzzer, 50); //long beep to signal no box
    delay(2000);
    analogWrite(buzzer, 0);
  }
}

void moveleft(int speedLeft, int speedRight) {
  analogWrite(ENA, speedLeft);
  analogWrite(ENB, speedRight);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}



void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void pickup() {
  // Open the servos to release position
  servo0.write(190);
  servo1.write(80);
  delay(500);
  
  // Lower the stepper motor
  stepper1.moveTo(stepsToLower);
  while (stepper1.distanceToGo() != 0) {
    stepper1.run();
  }

  // Wait for 1 second
  delay(1000);

  // Close the servos to grab the box
  servo0.write(170);
  servo1.write(95);
  delay(500);

  // Lift the stepper motor
  stepper1.moveTo(stepsToLift);
  while (stepper1.distanceToGo() != 0) {
    stepper1.run();
  }
}

void dropoff() {
  // Lower the stepper motor
  stepper1.moveTo(stepsToLower);
  while (stepper1.distanceToGo() != 0) {
    stepper1.run();
  }

  // Open the servos to release the box
  servo0.write(190);
  servo1.write(80);

  // Wait for 1 second
  delay(1000);

  // Lift the stepper motor
  stepper1.moveTo(-50);
  while (stepper1.distanceToGo() != 0) {
    stepper1.run();
  }

  // Close the servos
  servo0.write(170);
  servo1.write(95);
  delay(500);
}

void detectcolor(){
  // Read Red
  digitalWrite(s2, LOW);        //S2/S3 levels define which set of photodiodes we are using LOW/LOW is for RED LOW/HIGH is for Blue and HIGH/HIGH is for green
  digitalWrite(s3, LOW);
  red = pulseIn(out, LOW);  //here we wait until "out" goes LOW, we start measuring the duration and stops when "out" is HIGH again
  red = map(red, 60, 15, 0, 80);        
  delay(20);
                    
  // Read Blue
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  blue = pulseIn(out, LOW);  //here we wait until "out" goes LOW, we start measuring the duration and stops when "out" is HIGH again
  blue = map(blue, 80, 11, 0, 100);          
  delay(20);

  // Read Green
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  green = pulseIn(out, LOW);  //here we wait until "out" goes LOW, we start measuring the duration and stops when "out" is HIGH again
  green = map(green, 80, 11, 0, 100);          
  delay(20);
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    analogWrite(buzzer, volume);
    delay(250);
    analogWrite(buzzer, 0);
    delay(250);
  }
}