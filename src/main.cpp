/*
MSE312 STEERING WHEEL SIMULATOR CODE

HARDWARE CONFIGURATION:
Motor: XD-3420
Motor Driver: L298N bi-channel config
PSU: 12V, 48W
Controller: ATmega32u4
Encoder: E6B2-CWZ6C
Software Emulation: X360CE.exe
*/

#include <Encoder.h>
#include <Motor.h>
#include "HID-Project.h"
#include "Joystick.h"

//SYSTEM VARIABLES
#define PID_DEBUG 0 //true of false - SET TO FALSE WHEN STEERING WHEEL SETUP IS COMPLETE AND READY FOR WINDOWS SOFTWARE
#define PWM_DEBUG 0
#define ENCODER_DEBUG 0
#define BAUD_RATE 115200 // Define the baud rate

//ENCODER SETUP
Encoder MSE312enc(2, 3); //interrupt pins on Atmega32u4
long newPosition = 0;
long oldPosition  = 0;
long centerPosition = 0;
double zeroPositionSensitivity = 5; //value of %, zero range +- % of the full rotation

//SETUP MAXIUMUM ANGLE OF THE STEERING WHEEL (from full left to full right)
//+-512 = 90DEG
//+-1024 = 180DEG 
#define STEERING_WHEEL_ANGLE 180

//DO NOT TOUCH
#define ENCODER_MAX (6*STEERING_WHEEL_ANGLE)
#define ENCODER_MIN (ENCODER_MAX * (-1))
//

//MOTOR VARIABLES
Motor FFBmotor(4, 8, 6); //IN1, IN2, PWM
int fullPower = 255;
double powerRatio = 0;
int PWM = 0;

//GAMEPAD SIMULATION VARIABLES
int steeringXaxis = 0; //steering axis only

//CONTROLLER VALUES ARE HARDCODED FOR DRIVER COMPATIBILITY
#define XBOX_MAX 32767
#define XBOX_MIN -32767

//PID FEEDBACK
#define analogInputFromMotor A0
int analogValue = 0;

// int calculatePWMforFFB() {
//   //CONTROLS PART OF THE PROJECT
//   // powerRatio = (double)newPosition/ENCODER_MAX;
//   // PWM = powerRatio*fullPower;

//   analogValue = analogRead(analogInputFromMotor);

//   if(PID_DEBUG) {
//     Serial.print("Analog Value from motor circuit");
//     Serial.println(analogValue);
//   }

//   return analogValue/4;
// }

void adjustSteeringPosition(long currentPosition) {

  //PWM = calculatePWMforFFB();
  PWM = 255;

  if(PWM_DEBUG) {
    Serial.print("PWM value sent to the motor");
    Serial.println(PWM);
  }

  // steering wheel is turned left -> motor goes to the right -> negative PWM
  if (currentPosition > centerPosition + (double)ENCODER_MAX*(zeroPositionSensitivity/100)) {
    FFBmotor.go(PWM);
  }

  // steering wheel is turned right -> motor goes to the left -> positive PWM
   else if (currentPosition < centerPosition + (double)ENCODER_MIN*(zeroPositionSensitivity/100)) {
    PWM = (-1)*PWM;
    FFBmotor.go(PWM);
  }

  //zero position of the steering wheel
  else if (currentPosition >= centerPosition + (double)ENCODER_MIN*(zeroPositionSensitivity/100.0) && currentPosition <= centerPosition + (double)ENCODER_MAX*(zeroPositionSensitivity/100.0)) {
    FFBmotor.go(0);
  }
}

void setup()
{
  pinMode(analogInputFromMotor, INPUT);
  analogReference(DEFAULT);
  newPosition = 0;
  MSE312enc.write(newPosition);
  Gamepad.begin();
}

void loop()
{
  //ENCODER GETS THE PRIORITY ACCESS TO COMPUTATION BECAUSE IT STARTSUP FEEDING TO USB PORT
  //encoder code to access steering wheel input
  newPosition = MSE312enc.read();

  //SETUP MAXIUMUM ANGLE OF T
  if (newPosition >= ENCODER_MAX){
    newPosition = ENCODER_MAX;
  } else if (newPosition <= ENCODER_MIN) {
    newPosition = ENCODER_MIN;
  }
    
  if (newPosition != oldPosition) {
    //perform encoder calculations
    oldPosition = newPosition;    
    steeringXaxis = map(newPosition, ENCODER_MIN, ENCODER_MAX, XBOX_MIN, XBOX_MAX);
    steeringXaxis = (-1)*steeringXaxis;

    Gamepad.xAxis(steeringXaxis);
    Gamepad.write();
    delay(10);
  }

  //delay(UPDATE_INTERVAL); //wait for time between data

  //DEBUG SHOULD NOT BE USED WHEN PLAYING
  if (ENCODER_DEBUG) {
    Serial.begin(BAUD_RATE); // setup serial
    Serial.print("Encoder Position: ");
    Serial.println(newPosition);
    Serial.print("Steering Value: ");
    Serial.println(steeringXaxis);
  }

  //adjust steering wheel load
  adjustSteeringPosition(newPosition);
}