#ifndef OBJECTS_H
#define OBJECTS_H

#include "vex.h"
#include "pid_functions.h"
using namespace vex;

class robot{
    public:
    // an instance of competition
    competition Competition;

    // brain
    brain Brain;

    //controller
    controller controller1 = controller{primary};

    //drivetrain
    motor left_side_1{PORT2, ratio6_1, true};
    motor left_side_2{PORT3, ratio6_1, false};
    motor left_side_3{PORT1, ratio6_1, true};
    motor right_side_1{PORT8, ratio6_1, false};
    motor right_side_2{PORT7, ratio6_1, true};
    motor right_side_3{PORT6, ratio6_1, false};
    motor_group right_side{right_side_1, right_side_2, right_side_3};
    motor_group left_side{left_side_1, left_side_2, left_side_3};

    //pneumatics
    digital_out pneumatic_dropdown_wings{Brain.ThreeWirePort.C};
    digital_out pneumatic_side_right{Brain.ThreeWirePort.D};
    digital_out pneumatic_side_left{Brain.ThreeWirePort.B};
    digital_out pto{Brain.ThreeWirePort.A};

    //inertial
    inertial Inertial3{PORT20};

    //other motors
    motor intake{PORT18};
    motor release_motor{PORT15};

    //pidcontroller
    pidcontroller PIDcontroller;

    // other variables/functions
    void pre_auton(void);
    void close_qual(void);
    void far_qual(void);
    void safe_far(void);
    void program_skills(void);
    void three_ball(void);
    void select_auton(int index);
    bool calibrating = true;
};

#endif