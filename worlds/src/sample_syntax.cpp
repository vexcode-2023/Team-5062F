/*#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;


// START V5 MACROS
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
// END V5 MACROS


// Robot configuration code.
controller Controller1 = controller(primary);
motor MotorGroup11MotorA = motor(PORT11, ratio6_1, false);
motor MotorGroup11MotorB = motor(PORT12, ratio6_1, true);
motor_group MotorGroup11 = motor_group(MotorGroup11MotorA, MotorGroup11MotorB);

digital_out DigitalOutA = digital_out(Brain.ThreeWirePort.A);
inertial Inertial3 = inertial(PORT3);

motor Motor4 = motor(PORT4, ratio36_1, false);




// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;
#pragma endregion VEXcode Generated Robot Configuration
// Include the V5 Library
#include "vex.h"
  
// Allows for easier use of the VEX Library
using namespace vex;

float myVariable;

float asd[1];

// "when started" hat block
int whenStarted1() {
  Inertial3.startCalibration();
  while (Inertial3.isCalibrating()) { task::sleep(50); }
  if (Controller1.ButtonUp.pressing()) {
    MotorGroup11.spin(forward);
    MotorGroup11.spin(forward, 10.0, volt);
    Motor4.spin(reverse);
    Motor4.spin(reverse, 10.0, volt);
    MotorGroup11.stop();
    Motor4.spinToPosition(1, degrees, true);
    DigitalOutA.set(true);
  } else if (Inertial3.rotation(degrees) == 50.0) {
    while (!(Controller1.Axis1.position() == 50.0)) {
      MotorGroup11.setVelocity(50.0, percent);
      MotorGroup11.setStopping(hold);
    wait(5, msec);
    }
  } else {
    DigitalOutA.set(false);
  }
  return 0;
}


int main() {
  whenStarted1();
}


int main(){
  int error;
  int previous_error;
  error = previous_error;
}*/

