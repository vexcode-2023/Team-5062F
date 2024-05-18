#include "objects.h"

void robot::pre_auton(void) {
    //prepares the robot before a match starts
    calibrating = true;
    Inertial3.calibrate();
    wait(3,seconds);
    calibrating = false;
}

void robot::select_auton(int index){
    //
    while (!(Competition.isEnabled()) && calibrating){
        wait(10, msec);
    }
    // inits the robot for autonomous
    PIDcontroller.init();
    intake.stop();
    intake.setVelocity(100, percent);
    //checks which auton to run
    if (index == 1){
        close_qual();
    }
    else if (index == 2){
        far_qual();
    }
    else if (index == 3){safe_far();}
    else if (index == 4){program_skills();}
    else if (index == 5){three_ball();}
    
    
}

void robot::close_qual(void){ // close side AWP
    // descores triball
    PIDcontroller.drive(12);
    pneumatic_dropdown_wings.set(true);
    PIDcontroller.drive(-10.5,0,12);
    wait(500, msec);
    pneumatic_dropdown_wings.set(false);
    //heads towards and touches elevation bar
    PIDcontroller.turn(90+60-360,1.5);
    PIDcontroller.drive(40,0,4,100);
    //opens a wing to push away a triball
    pneumatic_side_right.set(true);
    //outtakes the preload
    intake.spin(forward);
    wait(1, sec);
    pneumatic_side_right.set(false);
}

void robot::far_qual(void){ // far side 5 ball
    // ball under elevation bar
    intake.spin(reverse);
    PIDcontroller.drive(11, 0,5,600,1);
    wait(500, msec);
    PIDcontroller.drive(-32, 0,4,600,1.7);
    intake.stop();
    // descores
    pneumatic_dropdown_wings.set(true);
    PIDcontroller.turn(-45, 0.1);
    PIDcontroller.drive(-17,0,12,600,1.05);
    PIDcontroller.turn(-45, 0.1);
    PIDcontroller.drive(-40, 0,12,600,.6);
    pneumatic_dropdown_wings.set(false);
    // pushes the three balls into side of goal
    PIDcontroller.drive(15,0,20,600,.5);
    PIDcontroller.turn(-190, 0.5);
    intake.spin(forward, 9, volt);
    pneumatic_side_right.set(true);
    PIDcontroller.drive(1000, 0,80,600, 2);
    pneumatic_side_right.set(false);
    // backs out of goal
    PIDcontroller.drive(-18, 0,8,600,1.1);
    PIDcontroller.turn(-130, .3);
    //curved trajectory
    PIDcontroller.final_rotation += 110;
    //goes to intake the triball in the middle by the barrier
    intake.spin(reverse, 9, volt);
    PIDcontroller.drive(65, 0,20,600,2.1);
    PIDcontroller.turn(200, .5);
    intake.spin(forward, 9, volt);
    //pushes the triball in the intake and the triball in the middle into the goal
    pneumatic_side_right.set(true);
    pneumatic_side_left.set(true);
    PIDcontroller.drive(500000,0,800,800);
}

void robot::safe_far(void){
    PIDcontroller.drive(-500000,0,800,40,8);
    PIDcontroller.drive(20,0,12,600,3);
    PIDcontroller.drive(-40,0,12,600,3);
    PIDcontroller.drive(20,0,12,600,3);
    PIDcontroller.drive(-40,0,12,600,3);
}

void robot::program_skills(void){
    wait(50, seconds);
    pneumatic_side_left.set(true);
    pneumatic_side_right.set(true);
    intake.spin(forward);
    PIDcontroller.drive(66,0,12,600,7);
    PIDcontroller.drive(-50);
}

void robot::three_ball(void){
    intake.spin(reverse);
    PIDcontroller.drive(11, 0,5,600,1);
    wait(500, msec);
    PIDcontroller.drive(-32, 0,4,600,1.6);
    intake.stop();
    // descores
    PIDcontroller.turn(-45, 0.2);
    PIDcontroller.drive(-18,0,24,600,1.15);
    PIDcontroller.turn(-45, 0.1);
    PIDcontroller.drive(-40, 0,12,600,.6);
    // pushes the three balls into side of goal
    PIDcontroller.drive(15,0,20,600,.5);
    PIDcontroller.turn(-165);
    intake.spin(forward, 9, volt);
    pneumatic_side_right.set(true);
    PIDcontroller.drive(1000, 0,80,600, 2);
    pneumatic_side_right.set(false);
    // backs out of goal
    PIDcontroller.drive(-18, 0,8,600,1.1);

}