#include <Servo.h>

// Servo motor
const int SERVO = 5;
Servo boiler_turner;
int boiler_pos;

bool timer_on;
unsigned long timer_value;
unsigned long timer_start;
unsigned long mil_elapsed;

void setup(){
    Serial.begin(9600);

    // Servo motor
    boiler_turner.attach(SERVO);
    boiler_pos=0;
    boiler_turner.write(boiler_pos);

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
        boiler_pos = (boiler_pos + 90)%180;
        //boiler_turner.write(boiler_pos);
        turn_on_boiler();
        Serial.println("time_up");
        timer_on=false;
    }
    if (mil_elapsed%10000 ==0 ){
        Serial.print("Ten seconds passed: ");
        Serial.print(mil_elapsed);
        Serial.print(" - ");
        Serial.println(timer_value);
    }


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