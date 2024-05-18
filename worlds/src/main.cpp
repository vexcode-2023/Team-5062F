#include "vex.h"
#include "cmath"
#include "objects.h"

using namespace vex;

robot Robot;

void autonomous(void) {
    // runs the auton
    Robot.select_auton(1);
}

void usercontrol(void) {
    // inits variables
    float forward_amount;
    float turn_amount;
    bool toggle;
    bool begin_ele = false;
    bool toggle_ele;
    while (true) {
        // raw joystick values
        forward_amount = Robot.controller1.Axis3.position();
        turn_amount = Robot.controller1.Axis1.position();

        // slopes the raw values to make the robot easier to control
        forward_amount = (forward_amount/float(100)*std::abs(forward_amount/float(100)))*100;
        turn_amount = (turn_amount/float(100)*std::abs(turn_amount/float(100)*float(3)/float(4)))*100;

        // tells the robot to move
        Robot.left_side.setVelocity((forward_amount+turn_amount), percent);
        Robot.right_side.setVelocity((forward_amount-turn_amount), percent);
        Robot.left_side.spin(forward);
        Robot.right_side.spin(forward);

        
        if (Robot.controller1.ButtonUp.pressing()){ //releases elevation
            begin_ele = true;
            Robot.release_motor.spinTo(210, degrees, false);
        }
        else if (Robot.controller1.ButtonX.pressing() && 
                 toggle_ele == false && toggle == false){ //activates PTO
            Robot.pto.set(true);
            toggle_ele = true;
            toggle = true;
        }
        else if (Robot.controller1.ButtonX.pressing() && 
                 toggle_ele == true && toggle == false){ //deactivates PTO (not usually needed)
            Robot.pto.set(false);
            toggle_ele = false;
            toggle = true;
        }
        else if (!Robot.controller1.ButtonX.pressing() && 
                 toggle == true){ //allows toggle to be activated again when all buttons are released
            toggle = false;
        }

        if (!begin_ele){
            Robot.release_motor.stop();
            Robot.release_motor.setStopping(hold);
        }

        //controls the intake by intaking with R1 and outake with R2
        Robot.intake.setVelocity((int(Robot.controller1.ButtonR2.pressing())*100 
                                  - int(Robot.controller1.ButtonR1.pressing())*100), percent);
        Robot.intake.spin(forward);

        // opens the side wings if L1 or the buttons that individually control the sides are pressed
        Robot.pneumatic_side_right.set(Robot.controller1.ButtonL1.pressing() || Robot.controller1.ButtonRight.pressing());
        Robot.pneumatic_side_left.set(Robot.controller1.ButtonL1.pressing() || Robot.controller1.ButtonY.pressing());

        // opens dropdown wings if L2 pressed
        Robot.pneumatic_dropdown_wings.set(Robot.controller1.ButtonL2.pressing());
    }
}


int main() {
    // gets the robot ready for befor
    Robot.pre_auton();
    Robot.Brain.Screen.print("rr");

    // Set up callbacks for autonomous and driver control periods.
    Robot.Competition.autonomous(autonomous);
    Robot.Competition.drivercontrol(usercontrol);

    // Prevent main from exiting with an infinite loop
    while (true) {
        if (Robot.Competition.isAutonomous()){ // controls PID in auton
            Robot.PIDcontroller.calculate_desired_distance_and_degrees();
            Robot.right_side.spin(forward, (Robot.PIDcontroller.forward_voltage() - Robot.PIDcontroller.turn_voltage()), volt);
            Robot.left_side.spin(forward, (Robot.PIDcontroller.forward_voltage() + Robot.PIDcontroller.turn_voltage()), volt);
        }
        wait(10, msec);
    }
    return 0;
}