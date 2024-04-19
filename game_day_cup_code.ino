#include <Servo.h>
#include <Stepper.h>

bool generate_random_open_close() { 
  long random_num = random(0, 2); // exclusive for upper, thus 1+1=2
  if (random_num == 0) return false; 
  else return true;
}

int generate_random_base_rotation() { 
  long random_num = random(0, 4); 
  if (random_num == 0) return 90;
  else if (random_num == 1) return 180; 
  else if (random_num == 2) return 270;
  else return 0;  
}

int find_fastest_rotation(int target_degree, int prev) { 
  if (prev - target_degree < 0 && abs(prev - (target_degree - 360)) < abs(prev - target_degree)) return (target_degree - 360); // negative case
  if (abs(prev - (target_degree + 360)) < abs(prev - target_degree)) return (target_degree + 360); 
  else return (target_degree); 
}

bool current_state = false; // False is closed, True is open
int MOTOR_1_PIN = 13;
int pos = 0; 

int num_motors = 2;
Servo servo_one; 
Servo servo_two; 
Servo servo_three;

Servo servo_ball_drop; 

int BALL_DROP_MOTOR_PIN = 11;
int BALL_DROP_BUTTON_PIN = 2;
Servo servo_list[] = {servo_one, servo_two, servo_three}; 
int all_pins[] = {13, 12}; 
int all_pos[] = {0, 0}; 
bool all_states[] = {true, true, true}; 

int button_pressed = 0; 
bool open_drop = false; 
int drop_pos = 0;  
int count = 0; 


int STEPPER_STEPS_PER_REV = 2038; // 1/64 gear ratio; 32 steps per rev on motor
Stepper stepperOne(STEPPER_STEPS_PER_REV, 1, 2, 3, 4); // 4 pins 
int stepper_pos = 0; 
int desired_stepper_pos = 0; 

Stepper stepperLid(STEPPER_STEPS_PER_REV, 5, 6, 7, 8); 
int stepper_lid_pos = 0; 
int target_lid_pos = 0; 

void process_motor(int index) { 
  if (all_states[index] && all_pos[index] < 180) { 
    // Spin to open
    all_pos[index] += 1; 
    servo_list[index].write(all_pos[index]);
  }
  else if (!all_states[index] && all_pos[index] > 0) { 
    all_pos[index] -= 1; 
    servo_list[index].write(all_pos[index]);
  }
}

void move_stepper() { 
  if (stepper_pos < desired_stepper_pos) { 
    stepperOne.step(STEPPER_STEPS_PER_REV / 360); // 1 degree stepped
    stepper_pos += 1; 
  }
  else if (stepper_pos > desired_stepper_pos) { 
    stepperOne.step(-(STEPPER_STEPS_PER_REV / 360)); // 1 degree stepped
    stepper_pos -= 1; 
  }
}

void move_stepper_lid() { 
  if (stepper_lid_pos < target_lid_pos) { 
    stepperLid.step(STEPPER_STEPS_PER_REV / 360); // 1 degree stepped
    stepper_lid_pos += 1; 
  }
  else if (stepper_lid_pos > target_lid_pos) { 
    stepperLid.step(-(STEPPER_STEPS_PER_REV / 360)); // 1 degree stepped
    stepper_lid_pos -= 1; 
  }
}

void move_ball_drop() { 
  if (open_drop && drop_pos < 160) {
    drop_pos += 1;  
  }
  else if (!open_drop && drop_pos > 30) { 
    drop_pos -= 1; 
  }
  servo_ball_drop.write(drop_pos);
}

void setup() {
  // put your setup code here, to run once:
  Serial.println("Game Day project started"); 
  //servo_one.attach(MOTOR_1_PIN); 
  for (int i = 0; i < num_motors; i++) { 
    servo_list[i].attach(all_pins[i]);
  }
  servo_ball_drop.attach(BALL_DROP_MOTOR_PIN);

  pinMode(BALL_DROP_BUTTON_PIN, INPUT); 
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  count++; 
  for (int index = 0; index < num_motors; index++) {
      if (count % 150 == 0) { // to cause a wait time
      //if ((all_states[index] && all_pos[index] == 180) || (!all_states[index] && all_pos[index] == 0)) { 
        all_states[index] = generate_random_open_close();
        delay(200);
      //}
      }
      process_motor(index);
  }
  if (count % 150 == 0) { 
    target_lid_pos = generate_random_open_close() ? 180 : 0; 
  }
  button_pressed = digitalRead(BALL_DROP_BUTTON_PIN); 
  //Serial.println(button_pressed); 

  if (button_pressed == HIGH) { 
    open_drop = true; 
  }
  else { 
    open_drop = false; 
  }
  move_ball_drop(); 

  if (count % 300 == 0) { 
    desired_stepper_pos = find_fastest_rotation(generate_random_base_rotation(), stepper_pos % 360);
  }
  move_stepper(); 
  move_stepper_lid(); 
}
