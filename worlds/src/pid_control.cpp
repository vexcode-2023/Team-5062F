#include "pid_functions.h"
#include "objects.h"
#include "cmath"

extern robot Robot;

void time(void){
    // if the robot takes too long to do an action this makes the robot assume it finished
    int a = Robot.PIDcontroller.moves;
    wait(Robot.PIDcontroller.time_to_wait, seconds);
    //checks to see if another task already started
    if (a == Robot.PIDcontroller.moves){ 
        Robot.PIDcontroller.waiting = !Robot.PIDcontroller.waiting;
    }
}


void pidcontroller::init(){
    //inits the pid controller at the beginning of auton
    Robot.right_side_1.resetPosition();
    Robot.right_side_2.resetPosition();
    Robot.right_side_3.resetPosition();
    Robot.left_side_1.resetPosition();
    Robot.left_side_2.resetPosition();
    Robot.left_side_3.resetPosition();
    final_distance = 0;
    forward_error = 0;
    forward_previous_error = 0;
    forward_integral = 0;
    final_rotation = 0;
    turning_error = 0;
    turning_previous_error = 0;
    turning_integral = 0;
    Robot.Inertial3.resetRotation();
}

void pidcontroller::init_vars_in_between_functions(){
    // resets everything except turning values
    Robot.right_side_1.resetPosition();
    Robot.right_side_2.resetPosition();
    Robot.right_side_3.resetPosition();
    Robot.left_side_1.resetPosition();
    Robot.left_side_2.resetPosition();
    Robot.left_side_3.resetPosition();
    final_distance = 0;
    forward_error = 0;
    forward_previous_error = 0;
    forward_integral = 0;
    forward_target = 0;
    waiting = true;
    forward_target_reached = false;
}

void pidcontroller::sensors(){
    // updates the sensor values for the pid controller
        forward_sensor = (
            Robot.right_side_1.position(degrees) +
            Robot.right_side_2.position(degrees) +
            Robot.right_side_3.position(degrees) +
            Robot.left_side_1.position(degrees) +
            Robot.left_side_2.position(degrees) +
            Robot.left_side_3.position(degrees)) / 6;
        turning_sensor = Robot.Inertial3.rotation(degrees);
    };

float pidcontroller::forward_voltage(float k_p, float k_i, float k_d){
	//Save the previous error for the derivative
	forward_previous_error = forward_error;

	//Calculate a new error by finding the difference between the current position
	//and the desired position.
	forward_error = forward_target - forward_sensor;

	//Begin summing the errors into the integral term if the error is below a threshold,
	//and reset it if not. This is to prevent the integral from growing too large.
	if(std::abs(forward_error) > forward_integral_threshold) {
		forward_integral += forward_error;
	}
	else {
		forward_integral = 0;
	}

	//Calculate the derivative by finding the change between the current error and
	//last update's error
	forward_derivative = forward_error - forward_previous_error;

	return (k_p*forward_error + k_i*forward_integral + k_d*forward_derivative);
};

float pidcontroller::turn_voltage(float k_p, float k_i, float k_d){
	//Save the previous error for the derivative
	turning_previous_error = turning_error;

	//Calculate a new error by finding the difference between the current position
	//and the desired position.
	turning_error = turning_target - turning_sensor;

	//Begin summing the errors into the integral term if the error is below a threshold,
	//and reset it if not. This is to prevent the integral from growing too large.
	if(std::abs(turning_error) > turning_integral_threshold) {
		turning_integral += turning_error;
	}
	else {
		turning_integral = 0;
	};

	//Calculate the derivative by finding the change between the current error and
	//last update's error
	turning_derivative = turning_error - turning_previous_error;

	//Combine all the parts of the PID function into the PID algorithm and return the value.
	return k_p*turning_error + k_i*turning_integral + k_d*turning_derivative;
};

void pidcontroller::init_acceleration(){
    // calculates how long (distance-wise) the robot will take to stop
    distance_to_reach_max_forward_velocity = 0;
    forward_current_ideal_speed = 0;
    if (std::abs(forward_acceleration_rate) > 0){
        while (std::abs(forward_current_ideal_speed) < max_speed){
            forward_current_ideal_speed += forward_acceleration_rate;
            distance_to_reach_max_forward_velocity += forward_current_ideal_speed;
        };
    }
    forward_current_ideal_speed = 0;
};

void pidcontroller::drive(float distance, float stop_speed, float acceleration, float max_velocity, float time){
    // drives a certain amount
    forward_acceleration_rate = acceleration;
    max_speed = max_velocity;
    init_acceleration();
    init_vars_in_between_functions();
    // converts inches to degrees of motor rotations
    float gear_ratio = float(36)/float(48);
    float diameter = float(3.25);
    float circumference = diameter*M_PI;
    final_distance = (distance*360.0f)/(circumference*gear_ratio);
    // sets the time the robot waits till it starts next task
    if (time == -1){
        time_to_wait = sqrt(std::abs(final_distance))/float(16);
    }
    else {time_to_wait = time;}
    // waits the timer without taking up this thread
    wait_event.broadcast();
    //sees if the timer ended or if the task physically finished
    while (!((std::abs(forward_sensor - final_distance) < 10.0 && std::abs(turning_sensor - final_rotation) < 5.0) || !waiting))
    {
        wait(30, msec);
    }
    
};

void pidcontroller::turn(float degrees, float time){
    // turns a certain amount
    init_acceleration();
    init_vars_in_between_functions();
    final_rotation += degrees;
    // sets the time the robot waits till it starts next task
    if (time == -1){
        time_to_wait = sqrt(std::abs(degrees))/float(18);
    }
    else{time_to_wait = time;}
    // waits the timer without using up this thread
    wait_event.broadcast();
    //sees if the timer ended or if the task finished by reaching its goal
    while (!((std::abs(forward_sensor - final_distance) < 10.0 && std::abs(turning_sensor - final_rotation) < 5.0)|| !waiting))
    {
        wait(30, msec);
    }
};

void pidcontroller::calculate_desired_distance_and_degrees(){
    // calculates the speeds the robot should be moving at
    // makes speed vars positive
    forward_acceleration_rate = std::abs(forward_acceleration_rate);
    max_speed = std::abs(max_speed);
    if (final_distance < 0.0f){ // going backwards
        // accelerates
        if (forward_current_ideal_speed > -max_speed && forward_sensor > final_distance/2){
            forward_current_ideal_speed -= forward_acceleration_rate;   
        } // decelerates
        else if (forward_sensor <= final_distance - distance_to_reach_max_forward_velocity && forward_sensor >= final_distance){
            forward_current_ideal_speed += forward_acceleration_rate;
        }
        // caps the speed to a maximum
        if (forward_current_ideal_speed < -max_speed){
            forward_current_ideal_speed = -max_speed;
        }
        //checks if the robot thinks it reached its target
        if (!forward_target_reached){
            forward_target += forward_current_ideal_speed;
        }
        if (forward_target + forward_current_ideal_speed < final_distance){
            forward_target_reached = true;
            forward_target = final_distance;
        }
    }
    else{ // going forwards
        // accelerates
        if (forward_current_ideal_speed < max_speed && forward_sensor < final_distance/2){
            forward_current_ideal_speed += forward_acceleration_rate;   
        } // decelerates
        else if (forward_sensor >= final_distance - distance_to_reach_max_forward_velocity && forward_sensor <= final_distance){
            forward_current_ideal_speed -= forward_acceleration_rate;
        }
        // caps the max speed
        if (forward_current_ideal_speed > max_speed){
            forward_current_ideal_speed = max_speed;
        }
        // checks if the target reached its goal
        if (!forward_target_reached){
            forward_target += forward_current_ideal_speed;
        }
        if (forward_target + forward_current_ideal_speed > final_distance){
            forward_target_reached = true;
            forward_target = final_distance;
        }
    }
    
    //turning doesn't need acceleration control to be accurate
    turning_target = final_rotation;

    //updates the sensors
    sensors();
};