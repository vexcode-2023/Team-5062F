#ifndef PID_FUNCTIONS_H
#define PID_FUNCTIONS_H
#include "vex.h"

using namespace vex;

// function for the timer at each task
void time(void);

class pidcontroller{
    public:
    //defines the init functions
    void init();
    void init_acceleration();
    void init_vars_in_between_functions();

    //inits acceleration control variables
    float final_rotation = 0;
    float final_distance = 0;
    float forward_current_ideal_speed = 0;
    float forward_acceleration_rate = 5;
    float max_speed = 550;
    int distance_to_reach_max_forward_velocity = 0;
    bool forward_target_reached = false;

    // variables for the timer
    int moves = 0;
    event wait_event{time};
    bool waiting = true;
    float time_to_wait = 0;
    
    //defines driving and turning functions
    void drive(float distance, float stop_speed = 0, float acceleration = 4, float max_velocity = 600, float time = -1);
    void turn(float degrees, float time = -1);

    //defines update functions
    void sensors();
    void calculate_desired_distance_and_degrees();

    // defines the actual pid controller
    float forward_voltage(float k_p = 0.0095, float k_i = 0, float k_d = 0.0015);
    float turn_voltage(float k_p = 0.057, float k_i = 0, float k_d = 0.012); 

    // inits pid controller variables
    float forward_previous_error = 0;
    float forward_error = 0;
    float forward_integral_threshold = 0;
    float forward_sensor = 0;
    float forward_target = 0;
    float forward_integral = 0;
    float forward_derivative = 0;
    float turning_previous_error = 0;
    float turning_error = 0;
    float turning_integral_threshold = 0;
    float turning_sensor = 0;
    float turning_target = 0;
    float turning_integral = 0;
    float turning_derivative = 0;
};


#endif