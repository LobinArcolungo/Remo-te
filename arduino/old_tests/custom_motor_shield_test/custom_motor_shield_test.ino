#include<Servo.h>
#define PUMP 0
#define MOTOR 1
const bool DEBUG= true;

/*  Test Script for testing Remo_te device. it takes from serial input at 9600 baud rate the following instructions
's' -> 12V 3A current cutoff

'e' -> 12V 3A current powerr on motor
'd' -> Stepper Motor turn right : clockwise
'a' -> Stepper Motor turn left : counterclockwise
'x' -> initialize stepnumber
'h' -> stepper go home
'b' -> step back
'n' -> step away
'g' -> pour all the water

'p' -> Pump activate
'o' -> Pump deactivate
'l' -> Pump one liter
'k' -> Pump half liter
'j' -> Pump one liter and a half

'w' -> Servo turn on boiler

'f' -> Servo start falling

't' -> Prepare 500 ml of tea

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
0.5 liters are needed 100 seconds and the "falling point" is not guardanteed
1.0 liters are needed 180 seconds and the "falling point" is not guaranteed
1.5 liters are needed 240 seconds and the "falling point" is not guardanteed

so 10 seconds before automatic turning off we 

*/

// Stepper motor
const int A1A = D1;
const int A1B = D3;
const int B1A = D2;
const int B1B = D4;

int stepnumber = 0;
double mf = 2*PI/16; // microstep factor

int Pa,Pb;

const int default_movement = 2048;
const int motor_power = 200;

// Stepper Motor relative position
int steps_from_home;
const int min_steps = 0;
const int max_steps = 9;


// Power supply Relay
const int POWER_ON = D5;

// Pump_Motor selection Relay
const int PUM0_MOT1 = D6;

// Servo motor
const int SERVO = D7;
Servo boiler_turner;
int boiler_pos;

const int SERVO_FALLER = D8;
Servo boiler_faller;
int faller_pos;

// Serial Input Parsing
const int BUF_SIZE=1;
char buf[1];
int rlen;

// Time management
unsigned long pump_seconds;
unsigned long boiling_timer;
unsigned long actualTime;
bool boiling;

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

// Motor movement !! each function leaves the power on!
void step_away(){
    if (steps_from_home<max_steps){
        powerOnMotor();
        for (int i = 0; i<default_movement;i++){
            stepnumber++;
            move(stepnumber,motor_power,1000);
        }
        steps_from_home += 1;
    }
}
void step_back(){
    if (steps_from_home>min_steps){
        powerOnMotor();
        for (int i = default_movement; i>0;i--){
            stepnumber--;
            move(stepnumber,200,1000);
        }
        steps_from_home -= 1;
    }
}
void stepper_go_home(){
    while(steps_from_home>0){
        step_back();
    }
}

void turn_on_boiler()
{
    int position;
    for(position=0;position<=180;position+=1){
        boiler_turner.write(position);
        Serial.print(".");
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
    powerOnMotor();
    for(position=0;position<=90;position+=1){
        boiler_faller.write(position);
        delay(30);
        }
    position = 0;
    boiler_faller.write(position);
    faller_pos = position; //to keep track of the position outside the function
}

void pumpMilliliters(int mill){
    pump_seconds = (int)(mill * 3.0/100);
    Serial.println(pump_seconds);
    if (pump_seconds >0 && pump_seconds<50){ //security limit
        powerOnPump();
        delay(pump_seconds*1000); 
        powerOff();
    }
}

unsigned long getFallingPointSeconds(int milliliters){
    
    unsigned long time =-1 ;
    if(milliliters>250){
        if (milliliters<=500){
            time = 100UL;
        }
        else if (milliliters<=1000){
            time = 180UL;
        }
        else if (milliliters<=1500){
            time = 240UL;
        }
        else{
            time = 250UL;
        }
    }
    return time;
}

void setup() {
    Serial.begin(115200);
    Serial.println("Setup initiated!");
    //Stepper motor
    pinMode(A1A,OUTPUT);
    pinMode(A1B,OUTPUT);
    pinMode(B1A,OUTPUT);
    pinMode(B1B,OUTPUT);

    if (DEBUG) Serial.println("Motor initiated!");
    // Power supply Relay
    pinMode(POWER_ON,OUTPUT);

    if (DEBUG) Serial.println("Power relay initiated!");

    // Pump_Motor selection Relay
    pinMode(PUM0_MOT1,OUTPUT);

    if (DEBUG) Serial.println("Selection relay initiated!");


    // Servo motor
    boiler_turner.attach(SERVO);
    boiler_pos=0;
    boiler_turner.write(boiler_pos);

    if (DEBUG) Serial.println("Servo turn on initiated!");

    boiler_faller.attach(SERVO_FALLER);
    faller_pos=0;
    boiler_faller.write(faller_pos);
    if (DEBUG) Serial.println("Servo faller initiated!");

    // Stepper motor position
    steps_from_home=0;

    //time management
    boiling = false;
    actualTime = 0;

    if (DEBUG) Serial.println("Setup terminated!");
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
    if (buf[0] == 'x') {
        stepnumber=0;
        powerOnMotor();
        move(stepnumber,motor_power,1000);
        powerOff();
    }
    if (buf[0] == 'h') {
        stepper_go_home();
    }
    if (buf[0] == 'b') {
        step_back();
    }
    if (buf[0] == 'n') {
        step_away();
    }
    if (buf[0] == 'g') {//pour all the water
        pourAllTheWater();
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

    //Prepare TEA

    if (buf[0] == 't') {//prepare 500ml
        prepareTea(1000);
    }


    //check if boiling time is up
    if(boiling && millis()-actualTime>=boiling_timer){
        boiling = false;
        pourAllTheWater();           
    }
}

void prepareTea(int milliliters){
    boiling_timer = getFallingPointSeconds(milliliters);

    if(milliliters>250 && milliliters<=1500 && boiling_timer>0){ //security parameters

        if (DEBUG) Serial.println("Start pumping");

        pumpMilliliters(milliliters);
        
        if (DEBUG) Serial.println("Finished pumping, turning on boiler");

        turn_on_boiler();
        
        //now wait until ten seconds from the turning off point

        if (milliliters>1000 && milliliters<=1500)
            boiling_timer -=10;

        if (DEBUG) {Serial.print("Boiler on, now we wait ");
                    Serial.print(boiling_timer);
                    Serial.print(" seconds. ");
                    Serial.print(boiling_timer *1000UL);
                    Serial.println(" milliseconds");}

        boiling_timer = boiling_timer *1000UL;
        actualTime = millis();
        boiling =true;
    }
        
}

void pourAllTheWater(){

    if (DEBUG) Serial.println("Now stepper goes home and takes one step away");

    //now check that the stepper motor is at home position

    if (steps_from_home != 0)
        stepper_go_home();
    
    step_away(); //give enough rope to fall

    if (DEBUG) Serial.println("Now boiler falls");

    fall();

    if (DEBUG) Serial.println("Boiler fell, now wait ten seconds and start pouring");

    delay(10*1000);
    //now the boiler should be turned off
    
    //pour hot water in three steps
    step_away();
    step_away();
    step_away();
    if (DEBUG) Serial.println("Positioning done, now pouring and wait 8 seconds each pouring");
    step_away();
    delay(8000);
    step_away();
    delay(8000);
    step_away();
    delay(8000);
    step_away();
    delay(8000);
    step_away();
    delay(8000);

    if (DEBUG) Serial.println("Now stepper go home");
    //reel the rope 
    stepper_go_home();

    if (DEBUG) Serial.println("finally shut power off");

    powerOff();
    
}