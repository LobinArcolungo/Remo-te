#include<Servo.h>
#define PUMP 0
#define MOTOR 1

/*  Test Script for testing Remo_te device. it takes from serial input at 9600 baud rate the following instructions
's' -> 12V 3A current cutoff

'e' -> 12V 3A current powerr on motor
'd' -> Stepper Motor turn right : clockwise
'a' -> Stepper Motor turn left : counterclockwise
'x' -> initialize stepnumber

'p' -> Pump activate
'o' -> Pump deactivate
'l' -> Pump one liter
'k' -> Pump half liter
'j' -> Pump one liter and a half

'w' -> Servo turn on boiler

'f' -> Servo start falling

NOTES:
to pump 
0.5 liters are needed 15 seconds
1.0 liters are needed 27 seconds
1.5 liters are needed 42 seconds
we can agree that the pumps pump 2 liters per minute 
so the milliliters that need to be pumped are pumped 
in the time provided by the following formula

seconds = milliliters * 6/100

to boil
0.5 liters are needed 100 seconds and the "falling point" is not present
1.0 liters are needed 180 seconds and the "falling point" is at seconds
1.5 liters are needed 42 seconds and the "falling point" is at seconds


*/

// Stepper motor
const int A1A = 2;
const int A1B = 3;
const int B1A = 4;
const int B1B = 5;

int stepnumber = 0;
double mf = 2*PI/256; // microstep factor

int Pa,Pb;

const int default_movement = 2000;
const int motor_power = 150;

// Power supply Relay
const int POWER_ON = 7;

// Pump_Motor selection Relay
const int PUM0_MOT1 = 8;

// Servo motor
const int SERVO = 6;
Servo boiler_turner;
int boiler_pos;

const int SERVO_FALLER = 9;
Servo boiler_faller;
int faller_pos;

// Serial Input Parsing
const int BUF_SIZE=1;
char buf[1];
int rlen;

void move(int stepnumber, int MAXpower, int wait){//maxpower is 255 usually
  Pa = (sin(stepnumber*mf)*MAXpower);
  Pb = (cos(stepnumber*mf)*MAXpower);

  if (Pa>0){
    analogWrite(A1A,Pa);
    analogWrite(A1B,0);
  }
  else{
    analogWrite(A1A,0);
    analogWrite(A1B,abs(Pa));
  }
                                                                                                                                       
  if (Pb>0){
    analogWrite(B1A,Pb);
    analogWrite(B1B,0);
  }
  else{
    analogWrite(B1A,0);
    analogWrite(B1B,abs(Pb));
  }
        powerOnMotor();

}

// !!! always call powerOff after calling any powerOn
void powerOnMotor(){
    digitalWrite(PUM0_MOT1,HIGH); //select motor
    digitalWrite(POWER_ON,HIGH);
}
void powerOnPump(){
    digitalWrite(PUM0_MOT1,LOW); //select pump
    digitalWrite(POWER_ON,HIGH);
}
void powerOff(){
    digitalWrite(POWER_ON,LOW);
}
void turn_on_boiler()
{
    int position;
    for(position=0;position<=180;position+=1){
        boiler_turner.write(position);
        delay(30);
        }
    for(position=180;position>=0;position-=1){
        boiler_turner.write(position);
        delay(30);
        }
    boiler_pos = position; //to keep track of the position outside the function
}

void fall()
{
    int position;
    for(position=0;position<=90;position+=1){
        boiler_faller.write(position);
        delay(30);
        }
    for(position=90;position>=0;position-=1){
        boiler_faller.write(position);
        delay(30);
        }
    faller_pos = position; //to keep track of the position outside the function
}


void pumpMilliliters(int mill){
    int seconds = (int)(mill * 6/100);
    if (seconds >0 && seconds<50){ //security limit
        powerOnPump();
        delay(seconds*1000); 
        powerOff();
    }
}

int getFallingPointSeconds(int milliliters){
    //TODO: calculate seconds

    return 10;
}

void setup() {
    //Stepper motor
    pinMode(A1A,OUTPUT);
    pinMode(A1B,OUTPUT);
    pinMode(B1A,OUTPUT);
    pinMode(B1B,OUTPUT);

    // Power supply Relay
    pinMode(POWER_ON,OUTPUT);

    // Pump_Motor selection Relay
    pinMode(PUM0_MOT1,OUTPUT);

    // Servo motor
    boiler_turner.attach(SERVO);
    boiler_pos=0;
    boiler_turner.write(boiler_pos);

    boiler_faller.attach(SERVO_FALLER);
    faller_pos=0;
    boiler_faller.write(boiler_pos);

    Serial.begin(9600);
}

void loop() {
    //parse input
    if(Serial.available()){
        rlen = Serial.readBytes(buf,BUF_SIZE);

    }
    else buf[0]= 'i'; //idle: no commands

    // Current Cutoff
    if (buf[0] == 's') {//PowerOff
        powerOff();
    }

    //Stepper Motor

    if (buf[0] == 'd') { //Right: clockwise
        powerOnMotor();
        for (int i = 0; i<default_movement;i++){
            stepnumber++;
            move(stepnumber,motor_power,1000);
        }
    }
    if (buf[0] == 'a') {//Left: clockwise
        powerOnMotor();
        for (int i = default_movement; i>0;i--){
            stepnumber--;
            move(stepnumber,200,1000);
        }
    }
    if (buf[0] == 'e') {//power on
        powerOnMotor();
    }
    if (buf[0] == 'x') {//Left: clockwise
        stepnumber=0;
        powerOnMotor();
        move(stepnumber,motor_power,1000);
        powerOff();
    }

    //Pump

    if (buf[0] == 'p') {//PowerOn
        powerOnPump();
    }
    if (buf[0] == 'o') {//PowerOff
        powerOff();
    }
    if (buf[0] == 'l') {//Fill one liter
        pumpMilliliters(1000);
    }
    if (buf[0] == 'k') {//Fill half liter
        pumpMilliliters(500);
    }
    if (buf[0] == 'j') {//Fill one liter and a half
        pumpMilliliters(1500);
    }

    //Servo 

    if (buf[0] == 'w') {//PowerOn
        turn_on_boiler();
    }

    if (buf[0] == 'f') {//Start falling
        fall();
    }

}


void prepareTea(int milliliters){

    pumpMilliliters(milliliters);
    
    turn_on_boiler();
    
    delay(getFallingPointSeconds(milliliters)*1000);

    //TODO: turn off boiler!!!
    
    //pour hot water in three steps
    powerOnMotor();
    for (int i = 0; i<default_movement;i++){
        stepnumber++;
        move(stepnumber,motor_power,1000);
    }
    delay(1000);
    powerOnMotor();
    for (int i = 0; i<default_movement;i++){
        stepnumber++;
        move(stepnumber,motor_power,1000);
    }
    delay(1000);
    powerOnMotor();
    for (int i = 0; i<default_movement;i++){
        stepnumber++;
        move(stepnumber,motor_power,1000);
    }
    delay(1000);

    //reel the rope 
    for (int i = default_movement; i>0;i--){
        stepnumber--;
        move(stepnumber,200,1000);
    }
    for (int i = default_movement; i>0;i--){
        stepnumber--;
        move(stepnumber,200,1000);
    }
    for (int i = default_movement; i>0;i--){
        stepnumber--;
        move(stepnumber,200,1000);
    }

    powerOff();
        





}