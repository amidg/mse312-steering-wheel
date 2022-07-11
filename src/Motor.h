  /*
  Motor.h - library created to support motor operation. Created for iANTS by Dmitrii Gusev, Electronic Systems Engineer
*/

/*MOTOR INTERFACE DESCRIPTION
IN1/3---IN2/4----PWM--------OUTPUT1/2
0         0       -         BRAKE
1         1       -         FLOATING
1         0      PWM        FORWARD W/ SPEED
0         1      PWM        REVERSE W/ SPEED
1         0       1         FULL FORWARD (PWM IS 255) //special case
0         1       1         FULL REVERSE (PWM IS 255) //special case 
 */

#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Motor
{
    public:
    Motor(int control1, int control2, int pwmPin);
    void go(int directionAndPower);
    void stop();

    private:
    int IN1;
    int IN2;
    int PWM;
};

#endif