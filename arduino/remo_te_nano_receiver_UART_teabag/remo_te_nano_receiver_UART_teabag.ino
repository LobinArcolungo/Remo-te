#include<Servo.h>
#define PUMP 0
#define MOTOR 1
const bool DEBUG= true;

/*  Test Script for testing Remo_te device. it is loaded on an arduino nano or any other 5V 
    devices and it receives from serial uart protocol commands for example from an esp 8266
    or from keyboard
// All input from ESP must be in the form "command:*" where * is the char command

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

'y' -> End tea making signal received from esp. shutting down TE_READY signal

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
const int A1A = 2;
const int A1B = 3;
const int B1A = 4;
const int B1B = 5;

int stepnumber = 0;
double mf = 2*PI/256; // microstep factor

int Pa,Pb;

const int default_movement = 2048;
const int motor_power = 150;

// Stepper Motor relative position
int steps_from_home;
const int min_steps = 0;
const int max_steps = 9;


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
const int BUF_SIZE=11;
// char buf[11]; declared in loop
int rlen;

String alphabet = "qwertyuiopasdfghjklzxcvbnm";
int index_in_alphabet;
char command;

// te_ready notification pin
int te_ready;
const int TE_READY=10;

// Time management
unsigned long pump_seconds;
unsigned long boiling_timer;
unsigned long actualTime;
bool boiling;

const unsigned long INFUSION_TIME = 3UL*60UL*1000UL;
bool infusing;
unsigned long start_infusion_time;

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
    for(position=90;position>=0;position-=1){
        boiler_turner.write(position);
        delay(30);
        }
    for(position=0;position<=90;position+=1){
        boiler_turner.write(position);
        Serial.print(".");
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
    boiler_pos=90;
    boiler_turner.write(boiler_pos);

    boiler_faller.attach(SERVO_FALLER);
    faller_pos=0;
    boiler_faller.write(faller_pos);

    // Stepper motor position
    steps_from_home=0;

    //time management
    boiling = false;
    actualTime = 0;

    infusing = false;
    start_infusion_time = 0;

    //TE_READY notification pin
    pinMode(TE_READY,OUTPUT);
    te_ready=LOW;
    digitalWrite(TE_READY,te_ready);

    Serial.begin(74880);
    if (DEBUG) Serial.print("Arduino nano setup ended! ");
    
}

void loop() {
    if (Serial.available()){
        char buf[11];
        // Start of input parsing verification. this should get rid of the garbage input coming from the esp
        // All input from ESP must be in the form "command:*" where * is the char command
        rlen = Serial.readBytes(buf,BUF_SIZE);
        String comm = buf;

        if(rlen == 9){
            String sub = comm.substring(0,8);
            if (sub.equals("command:"))
                command = buf[rlen-1];
                index_in_alphabet = alphabet.indexOf(command);
        }
        else{
            index_in_alphabet = -1;
        }
        // End of input parsing verification
        
        if (index_in_alphabet>=0){
            Serial.print("Received form Serial: '");
            Serial.print(command);
            Serial.println("'");
        }

        if(rlen == 11){
            String te = comm.substring(8,10);
            String sub = comm.substring(0,8);
            if (sub.equals("command:") && te.equals("TE")){
                char c_cups [1];
                c_cups[0]= buf[rlen-1];
                int n_cups = atoi(c_cups);
                if (DEBUG) Serial.print("Arduino nano is going to prepare ");
                if (DEBUG) Serial.print(n_cups);
                if (DEBUG) Serial.println(" cups of tea");
                prepareTea(n_cups*250);

            }
            else{
                if (DEBUG) Serial.print("11:");
                if (DEBUG) Serial.println(comm);
            }
        }

        
        memset(buf,0, sizeof(buf));
        Serial.flush();
        rlen=0;

    }
    else command= 'i'; //idle: no commands

    // Current Cutoff
    if (command == 's') {//PowerOff
        powerOff();
    }

    //Stepper Motor

    if (command == 'd') { //Right: clockwise
        powerOnMotor();
        for (int i = 0; i<default_movement;i++){
            stepnumber++;
            move(stepnumber,motor_power,1000);
        }
    }
    if (command == 'a') {//Left: clockwise
        powerOnMotor();
        for (int i = default_movement; i>0;i--){
            stepnumber--;
            move(stepnumber,200,1000);
        }
    }
    if (command == 'e') {//power on
        powerOnMotor();
    }
    if (command == 'x') {
        stepnumber=0;
        powerOnMotor();
        move(stepnumber,motor_power,1000);
        powerOff();
    }
    if (command == 'h') {
        stepper_go_home();
    }
    if (command == 'b') {
        step_back();
    }
    if (command == 'n') {
        step_away();
    }
    if (command == 'g') {//pour all the water
        pourAllTheWater();
    }

    //Pump

    if (command == 'p') {//PowerOn
        powerOnPump();
    }
    if (command == 'o') {//PowerOff
        powerOff();
    }
    if (command == 'l') {//Fill one liter
        pumpMilliliters(1000);
    }
    if (command == 'k') {//Fill half liter
        pumpMilliliters(500);
    }
    if (command == 'j') {//Fill one liter and a half
        pumpMilliliters(1500);
    }

    //Servo 

    if (command == 'w') {//PowerOn
        turn_on_boiler();
    }

    if (command == 'f') {//Start falling
        fall();
    }

    //Prepare TEA

    if (command == 't') {//prepare 500ml
        prepareTea(1000);
    }

    //End tea making
    if (command == 'y') {

        if (DEBUG) Serial.println("'y' signal received, now checking if te_ready is high'");
        if(te_ready == HIGH){
            if (DEBUG) Serial.println("it is.");
            te_ready = LOW;
            digitalWrite(TE_READY,te_ready);
            if (DEBUG) Serial.print("now it is: ");
            if (DEBUG) Serial.println(te_ready);

        }else{
            if (DEBUG) Serial.println("it isn't.");
        }
    }


    //check if boiling time is up
    if(boiling && millis()-actualTime>=boiling_timer){
        boiling = false;
        pourAllTheWater();           
    }
    //check if infusion time is up
    if(infusing && millis()-start_infusion_time>=INFUSION_TIME){
        infusing = false;

        if (DEBUG) Serial.println("Infusion terminated!");
        endTeaPreparation();           
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

    powerOff(); //always power off motor

    //now start infusing process

    if (DEBUG) Serial.println("Now start infusing process");

    start_infusion_time = millis();
    infusing =true;
}
void endTeaPreparation(){

    if (DEBUG) Serial.println("Now stepper go home");
    //reel the rope 
    stepper_go_home();

    //reel the last bit
    steps_from_home+=1;
    stepper_go_home();

    if (DEBUG) Serial.println("finally shut power off");

    powerOff();

    //send TE_READY signal to ESP
    Serial.println("TE_READY");
    te_ready=HIGH;
    digitalWrite(TE_READY,te_ready);

    return;
    
}