// Include the Arduino Stepper Library
#include <Stepper.h>
 
// Define Constants
 
// Number of steps per output rotation
const int STEPS_PER_REV = 2000;
const int SPEED_CONTROL = A0;

const int BUF_SIZE=1;
char buf[1];
int rlen;


int incr_speed_by = 20;
int speed = 0;
int direction = 1;

int motorSpeed;
int steps;

String line;

// The pins used are 1,2,3,4 
// Connected to L298N Motor Driver In1, In2, In3, In4 
// Pins entered in sequence 1-2-3-4 for proper step sequencing
 
Stepper stepper_NEMA17(STEPS_PER_REV, D1, D2, D3, D4);

void motorSpeedRight(){
  speed += incr_speed_by;
}
void motorSpeedLeft(){
  speed -= incr_speed_by;
}
void motorStepUp(){
  steps+=10;
}
void motorStepDown(){
  steps-=10;
}
void motorStop(){
  speed = 0;
}
void move(int motorspeed,int steps){

  // map it to a range from 0 to 100: = map(speed, 0, 1023, 0, 100);
  // set the motor speed:
  if (abs(motorSpeed) > 0) {
    stepper_NEMA17.setSpeed(abs(motorSpeed));
    // step 1/100 of a revolution:

    direction = motorSpeed/abs(motorSpeed);
    stepper_NEMA17.step(direction*steps);//STEPS_PER_REV / 100);
  }
}




void setup() {
  // nothing to do inside the void setup
  Serial.begin(9600);
  buf[0] = 'i';
  steps = 10;
}
 
void loop() {

  if(Serial.available()){
    rlen = Serial.readBytes(buf,BUF_SIZE);

  }
  else buf[0]= 'i';


  if (buf[0] == 'd') {
    
    motorSpeedRight();
  }
  if (buf[0] == 'a') {
    
    motorSpeedLeft();
  }
  if (buf[0] == 'w') {
    
    motorStepUp();
  }
  if (buf[0] == 's') {
    
    motorStepDown();
  }
  if (buf[0] == 'x') {
    
    motorStop();
  }

  if (buf[0]=='g'){ //g go makes one move
    

    //Serial.print("Speed: ");
    //Serial.print(speed);
    //Serial.print(", Steps: ");
    //Serial.print(steps);
    //Serial.print(", command:  ");
    //Serial.println(buf[0]);

    motorSpeed = map(speed, 0, 1000, 0, 100);
    if (steps>0)  move(motorSpeed,steps);
  }
  line  = "Speed: "+String(speed)+ ", Steps: "+String(steps)+ ", command:  "+String(buf[0]);
    Serial.println(line);
}