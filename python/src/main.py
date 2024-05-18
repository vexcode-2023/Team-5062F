from vex import *

brain=Brain()
right_motors_motor_a = Motor(Ports.PORT1, GearSetting.RATIO_6_1, False)
right_motors_motor_b = Motor(Ports.PORT10, GearSetting.RATIO_6_1, False)
right_motors_motor_c = Motor(Ports.PORT11, GearSetting.RATIO_6_1, False)
right_motors = MotorGroup(right_motors_motor_a, right_motors_motor_b, right_motors_motor_c)
left_motors_motor_a = Motor(Ports.PORT13, GearSetting.RATIO_6_1, True)
left_motors_motor_b = Motor(Ports.PORT15, GearSetting.RATIO_6_1, True)
left_motors_motor_c = Motor(Ports.PORT4, GearSetting.RATIO_6_1, True)
left_motors = MotorGroup(left_motors_motor_a, left_motors_motor_b, left_motors_motor_c)
controller = Controller(PRIMARY)
pneumatic_fan = DigitalOut(brain.three_wire_port.a)
flywheel = Motor(Ports.PORT9, GearSetting.RATIO_6_1, False)
flywheel_extender = Motor(Ports.PORT16, GearSetting.RATIO_36_1, False)
elevation = DigitalOut(brain.three_wire_port.b)
inertial = Inertial(Ports.PORT8)
intake = Motor(Ports.PORT3, GearSetting.RATIO_6_1, False)

# wait for rotation sensor to fully initialize
wait(30, MSEC)


inertial.calibrate()
# Library imports
import math, random
reverse_drive = 1 #a variable that if activated will reverse the direction the drivetrain drives
autons = ["4 triball" , "corner remove", "skills", "Defensive push", "NO ele bar", "None", "6 triball"]
auton_index = 0
goal_rotation = 0
disable_PID = False

# Begin project code
def driver_control_0(): 
    flywheel.set_velocity(100, PERCENT)
    flywheel_extender.set_velocity(100, PERCENT)
    global reverse_drive 
    extended = False
    spinning = False
    toggling_flywheel = False
    elevated = False
    toggling_elevate = False
    toggling_raise_flywheel = False
    while True:
        # causes the drive motors to spin forever at whatever speed wanted
        right_motors.spin(FORWARD)
        left_motors.spin(FORWARD)
        #calculates
        forward_amount = controller.axis3.position()*reverse_drive
        turn_amount = reverse_drive * controller.axis1.position()
        turn_amount = reverse_drive * (turn_amount/100)*abs(turn_amount/100)*3/4
        right_motors.set_velocity(forward_amount - 100*turn_amount, PERCENT)
        left_motors.set_velocity(forward_amount + 100*turn_amount, PERCENT)
        if controller.buttonL1.pressing():
            pneumatic_fan.set(True)
            reverse_drive = -1
        else:
            pneumatic_fan.set(False)
            reverse_drive = 1
        if controller.buttonLeft.pressing():
            if not toggling_elevate:
                if not elevated:
                    elevation.set(True)
                    elevated = True
                else:
                    elevation.set(False)
                    elevated = False
            toggling_elevate = True
        else:
            toggling_elevate = False
        while controller.buttonL2.pressing():
            left_motors.set_stopping(HOLD)
            right_motors.set_stopping(HOLD)
            left_motors.stop()
            right_motors.stop()
            wait(5)
        left_motors.set_stopping(COAST)
        right_motors.set_stopping(COAST)
        flywheel.set_stopping(COAST)
        if controller.buttonX.pressing():
            flywheel.set_velocity(100, PERCENT)
            if not toggling_flywheel:
                if spinning:
                    flywheel.stop()
                else:
                    flywheel.spin(FORWARD)
                spinning = not spinning
                toggling_flywheel = True
        elif controller.buttonY.pressing():
            flywheel.set_velocity(-100, PERCENT)
            if not toggling_flywheel:
                if spinning:
                    flywheel.stop()
                else:
                    flywheel.spin(FORWARD)
                spinning = not spinning
            toggling_flywheel = True
        else:
            toggling_flywheel = False
        if controller.buttonRight.pressing():
            if not toggling_raise_flywheel:
                if extended:
                    flywheel_lower.broadcast()
                else:
                    flywheel_extender.spin_to_position(.95, TURNS, wait=False) 
                extended = not extended
            toggling_raise_flywheel = True
        else:
            toggling_raise_flywheel = False
        if controller.buttonR2.pressing():
            intake.set_velocity(-100, PERCENT)
            intake.spin(FORWARD)
            flywheel_extender.spin_to_position(0.42, TURNS, wait=False)
            extended = True
        elif controller.buttonR1.pressing():
            intake.set_velocity(-100, PERCENT)
            intake.spin(REVERSE)
            flywheel_extender.spin_to_position(0.42, TURNS, wait=False)
            extended = True
        else:
            intake.stop()
        flywheel_extender.set_stopping(HOLD)

def lower_flywheel():
    flywheel_extender.spin(REVERSE)
    while flywheel_extender.velocity(PERCENT) == 0:
        pass
    while flywheel_extender.velocity(PERCENT) != 0:
        wait(10)
    flywheel_extender.reset_position()
    flywheel_extender.stop()

flywheel_lower = Event(lower_flywheel)

def when_started():
    global autons, auton_index
    colors = [Color.WHITE, Color.GREEN, Color.ORANGE, Color.ORANGE, Color.ORANGE ,Color.WHITE, Color.ORANGE, Color.ORANGE, Color.ORANGE]
    color_index = 0
    for row in range(20):
        brain.screen.print("___")
        for times in range(6):
            color_index += 1
            brain.screen.set_pen_color(random.choice(colors))
            if random.randint(1,3) == 1:
                brain.screen.print("Fenneks")
            elif random.randint(1,2)==1:
                brain.screen.print("5062_F_")
            else:
                if random.randint(1,5) == 1:
                    brain.screen.print("_Adam__")
                elif random.randint(1,4) == 1:
                    brain.screen.print("Nathan_")
                elif random.randint(1,3) == 1:
                    brain.screen.print("Bethany")
                elif random.randint(1,2) == 1:
                    brain.screen.print("_Ella__")
                else:
                    brain.screen.print("_Leah__")
        brain.screen.print("___")
        brain.screen.set_cursor(row, 1)
    controller.screen.print("Auton =", str(autons[auton_index]))
    while not controller.buttonB.pressing():
        controller.rumble(".")
        if controller.buttonA.pressing():
            controller.screen.clear_row(3)
            auton_index += 1
            auton_index = auton_index % len(autons)
            controller.screen.print("Auton =", str(autons[auton_index]))
            while controller.buttonA.pressing():
                wait(1)
    controller.rumble("")

def autonomous_0():
    #autonomous code
    global autons, auton_index, disable_PID
    intake.set_velocity(100, PERCENT)
    if str(autons[auton_index]) == "4 triball":
        #scoring offensive and touch elevation bar
        flywheel_extender.spin_to_position(.3, TURNS, wait=True)
        intake.spin(FORWARD)
        PID_drive_amount(40)
        PID_turn_amount(90)
        intake.spin(REVERSE)
        wait(500)
        intake.spin(FORWARD)
        PID_turn_amount(-110)
        PID_drive_amount(10)
        PID_turn_amount(110)
        intake.spin(REVERSE)
        wait(500)
        PID_turn_amount(200)
        intake.spin(FORWARD)
        PID_drive_amount(16)
        PID_turn_amount(160)
        intake.spin(REVERSE)
        PID_drive_amount(10)
        intake.spin(FORWARD)
        PID_turn_amount(115)
        PID_drive_amount(16)
        PID_drive_amount(-16)
        PID_turn_amount(-115)
        intake.spin(REVERSE)
        wait(500)
        PID_turn_amount(170)
        pneumatic_fan.set(True)
        PID_drive_amount(-25)
        PID_drive_amount(15)
    elif str(autons[auton_index]) == "NO ele bar":
        #scoring offensive without touching the elevation bar
        PID_drive_amount(-6)
        pneumatic_fan.set(True)
        wait(500)
        PID_drive_amount(-6)
        pneumatic_fan.set(False)
        PID_drive_amount(-6)
        PID_turn_amount(-45)
        PID_drive_amount(-12)
        PID_drive_amount(12)
        PID_drive_amount(-12)
        PID_drive_amount(12)
        PID_drive_amount(-12)
        PID_drive_amount(12)        
    elif str(autons[auton_index]) == "corner remove":
        #removal from corner defensive
        flywheel_extender.spin_to_position(0, TURNS, wait=False)
        PID_drive_amount(9)
        pneumatic_fan.set(True)
        wait(0.5, SECONDS)
        PID_drive_amount(-3)
        pneumatic_fan.set(False)
        wait(0.5, SECONDS)
        PID_turn_amount(-42)
        PID_drive_amount(-33)
        PID_turn_amount(-12)
        flywheel_extender.spin_to_position(.95, TURNS)
        PID_drive_amount(-3)
    elif str(autons[auton_index]) == "skills":
        # skills scoring autonomous
        '''flywheel.set_velocity(100, PERCENT)
        left_motors.set_stopping(HOLD)
        right_motors.set_stopping(HOLD)
        PID_drive_amount(-22)
        PID_turn_amount(30)
        PID_drive_amount(-15)
        PID_drive_amount(15)
        PID_turn_amount(-30)
        PID_drive_amount(15)
        PID_turn_amount(110)
        flywheel.spin(FORWARD)
        PID_drive_amount(12)
        disable_PID = True
        flywheel_extender.spin_to_position(1.4, TURNS, wait=False)
        left_motors.stop()
        right_motors.stop()
        flywheel.spin(FORWARD)
        wait(30, SECONDS)
        flywheel.stop()
        flywheel_extender.spin_to_position(0)
        disable_PID = False
        PID_drive_amount(-10)
        PID_turn_amount(60)
        PID_drive_amount(-20)
        PID_turn_amount(-43)
        PID_drive_amount(-70)
        PID_turn_amount(-49)
        PID_drive_amount(-20)
        PID_turn_amount(-105)
        PID_drive_amount(-50)
        PID_turn_amount(65)
        PID_drive_amount(-10)
        PID_turn_amount(90)
        pneumatic_fan.set(True)
        wait(50)
        PID_drive_amount(-25)
        PID_drive_amount(25)
        pneumatic_fan.set(False)
        PID_turn_amount(-45)
        PID_drive_amount(-25)
        PID_turn_amount(70)
        pneumatic_fan.set(True)
        wait(50)
        PID_drive_amount(-25)
        PID_drive_amount(40)
        pneumatic_fan.set(False)
        PID_turn_amount(60)
        PID_drive_amount(-30)
        PID_turn_amount(-60)
        PID_drive_amount(-36)
        PID_turn_amount(-100)
        #pneumatic_fan.set(True)
        #wait(50)
        PID_drive_amount(-20)
        PID_drive_amount(36)
        pneumatic_fan.set(False)'''
        flywheel.set_velocity(100, PERCENT)
        left_motors.set_stopping(HOLD)
        right_motors.set_stopping(HOLD)
        disable_PID = True
        flywheel_extender.spin_to_position(1.4, TURNS, wait=False)
        left_motors.stop()
        right_motors.stop()
        flywheel.spin(FORWARD)
        wait(33, SECONDS)
        flywheel.stop()
        flywheel_extender.spin_to_position(0)
        disable_PID = False
        PID_drive_amount(-15)
        PID_turn_amount(60)
        PID_drive_amount(-12)
        PID_turn_amount(-41)
        PID_drive_amount(-70)
        PID_turn_amount(-49)
        PID_drive_amount(-40)
        PID_drive_amount(20)
        PID_turn_amount(-95)
        PID_drive_amount(-40)
        PID_turn_amount(55)
        PID_drive_amount(-15)
        PID_turn_amount(90)
        pneumatic_fan.set(True)
        wait(50)
        PID_drive_amount(-25)
        PID_drive_amount(35)
        pneumatic_fan.set(False)
        PID_turn_amount(-90)
        PID_drive_amount(-30)
        pneumatic_fan.set(True)
        PID_turn_amount(140)
        PID_drive_amount(-40)
        pneumatic_fan.set(False)
        PID_drive_amount(20)
        '''PID_turn_amount(90)
        PID_drive_amount(-45)
        PID_drive_amount(5)
        PID_turn_amount(90)
        PID_drive_amount(-25)
        pneumatic_fan.set(False)'''
        '''PID_turn_amount(60)
        PID_drive_amount(-30)
        PID_turn_amount(-100)
        pneumatic_fan.set(True)
        wait(50)
        PID_drive_amount(-25)
        PID_drive_amount(20)
        pneumatic_fan.set(False)
        PID_turn_amount(80)
        PID_drive_amount(-36)
        PID_turn_amount(-120)
        pneumatic_fan.set(True)
        wait(50)
        PID_drive_amount(-36)
        PID_drive_amount(10)
        pneumatic_fan.set(False)'''
    elif str(autons[auton_index]) == "Defensive push":
        flywheel_extender.spin_to_position(.3, TURNS, wait=False)
        PID_turn_amount(30)
        intake.spin(FORWARD)
        PID_drive_amount(47)
        PID_turn_amount(-120)
        pneumatic_fan.set(True)
        PID_drive_amount(-30)
    else:
        pass
    
# PID Auton functions:
# PID code initialization

desired_distance = 0
desired_turn = 0
positions = []

forward_error = 0 # sets to desired value - sensor's value
forward_previous_error = 0 # position previously checked
forward_derivative = 0 # difference between the error and the error previously, which is the speed

turn_error = 0 # sets to desired value - sensor's value
turn_previous_error = 0 # position previously checked
turn_derivative = 0 # difference between the error and the error previously, which is the speed

def PID_check_distance():
    # init
    global autons, auton_index
    global forward_error, forward_previous_error, forward_derivative, desired_distance
    global turn_error, turn_previous_error, turn_derivative, desired_turn
    
    forward_kp = 0.015
    forward_kd = 0.008
    
    turn_kp = -0.08
    turn_kd = -0.08

    # grabs the previous error values for the derivatives
    forward_previous_error = forward_error
    turn_previous_error = turn_error

    # calculates the positions in the motor
    global positions
    positions = [right_motors_motor_a.position(DEGREES), right_motors_motor_b.position(DEGREES), right_motors_motor_c.position(DEGREES), left_motors_motor_a.position(DEGREES), left_motors_motor_b.position(DEGREES), left_motors_motor_c.position(DEGREES)]
    average_pos = sum(positions)/len(positions)
    turn_difference = inertial.rotation()

    # forward error and derivative calculations
    forward_error = desired_distance - average_pos
    forward_derivative = forward_previous_error - forward_error

    forward_motor_power = forward_error * forward_kp - forward_derivative * forward_kd

    # turning
    turn_error = desired_turn - turn_difference
    turn_derivative =  turn_previous_error - turn_error

    turning_motor_power = turn_error * turn_kp - turn_derivative * turn_kd
    
    # motor controls
    right_motors_motor_a.spin(FORWARD, forward_motor_power + turning_motor_power, VOLT)
    right_motors_motor_b.spin(FORWARD, forward_motor_power + turning_motor_power, VOLT)
    right_motors_motor_c.spin(FORWARD, forward_motor_power + turning_motor_power, VOLT)
    left_motors_motor_a.spin(FORWARD, forward_motor_power - turning_motor_power, VOLT)
    left_motors_motor_b.spin(FORWARD, forward_motor_power - turning_motor_power, VOLT)
    left_motors_motor_c.spin(FORWARD, forward_motor_power - turning_motor_power, VOLT)

def PID_drive_amount(change_distance):
    # returns a value for the PID loop to use
    global desired_distance, forward_error
    gear_ratio = 60/36
    wheel_size = 3.25*math.pi
    degrees_per_inch = (360 * gear_ratio)/(wheel_size)
    desired_distance += change_distance * degrees_per_inch
    wait(math.sqrt(abs(change_distance))/4, SECONDS)

def PID_turn_amount(change_turn):
    global desired_turn, turn_error
    desired_turn += change_turn
    wait(math.sqrt(abs(change_turn))/18, SECONDS)

def PID_run():
    global disable_PID
    disable_PID = False
    while True:
        #if not disable_PID:
        PID_check_distance()
        wait(10, MSEC)

#runs the program
def run_autonomous():
    global auton_index, autons, disable_PID
    #starts the autonomous functions
    autonomous = Thread(autonomous_0)
    pidcontrol = Thread(PID_run)
    #waits for the driver control period to end
    while(competition.is_autonomous() and competition.is_enabled()):
        wait(10, MSEC)
    #stops the autonomous control tasks
    if autons[auton_index]  ==  "corner remove":
        pneumatic_fan.set(False)
    autonomous.stop()
    pidcontrol.stop()

def run_driver_control():
    #starts the driver control functions
    run_driver_control_0 = Thread(driver_control_0)
    #waits for the driver control period to end
    while(competition.is_driver_control() and competition.is_enabled()):
        wait(10, MSEC)
    #stops the driver control tasks
    run_driver_control_0.stop()

#registers the competition functions
when_started()
competition = Competition(run_driver_control, run_autonomous)