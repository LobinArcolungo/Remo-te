#include <Servo.h>

// Servo motor
const int POWER_ON = 4;
int choice;

bool timer_on;
unsigned long timer_value;
unsigned long timer_start;
unsigned long mil_elapsed;

void setup(){
    Serial.begin(9600);

    // Servo motor
    pinMode(POWER_ON,OUTPUT);
    choice=0;

    timer_on=false;
}

void loop(){
    mil_elapsed = (millis()-timer_start);
    if(!timer_on){
        timer_on=true;
        timer_value=6UL*1000UL;
        timer_start = millis();
    }
    else if (mil_elapsed >= timer_value){
        choice ++;
        if (choice%2 == 0) {powerOn();}
        else {powerOff();}
        Serial.print(choice);
        Serial.println(" - time_up");
        timer_on=false;
    }
    if (mil_elapsed%10000 ==0 ){
        Serial.print("Ten seconds passed: ");
        Serial.print(mil_elapsed);
        Serial.print(" - ");
        Serial.println(timer_value);
    }


}


void powerOn(){
    digitalWrite(POWER_ON,HIGH);
}
void powerOff(){
    digitalWrite(POWER_ON,LOW);
}