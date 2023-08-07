#include<Servo.h>
#define SERVO_BOILER 8
#define RELAY_PUMP 7
#define BUTTON_DEBUG 2

Servo servo9;
int pos9;


int button_debug_raw;
int button_debug_debounce;
int button_debug_value;

int getMillisFromCups(int cups){
 // TODO: millis calculator algorithm
    return 5000;
}

void turn_on_boiler()
{
    for(pos9=0;pos9<=180;pos9+=1){
        servo9.write(pos9);
        delay(50);
        }
    for(pos9=180;pos9>=0;pos9-=1){
        servo9.write(pos9);
        delay(50);
        }
}

void fill_boiler(int cups){

    digitalWrite(RELAY_PUMP,HIGH);
    delay(1000 * getMillisFromCups(cups));
    digitalWrite(RELAY_PUMP,LOW);
    delay(1000);
}

void setup() {
    delay(100);
    Serial.begin(9600);
    //specify the pinout
    pinMode(BUTTON_DEBUG,INPUT);
    pinMode(RELAY_PUMP,OUTPUT);

    //servo_boiler
    servo9.attach(SERVO_BOILER);
    pos9=0;
    servo9.write(pos9);

    //relay pump
    // no further setup needed

}

void loop() {
    button_debug_raw = digitalRead(BUTTON_DEBUG);

    //debouncing
    if (button_debug_raw == HIGH){
        button_debug_debounce++;
        if (button_debug_debounce >=10){
            button_debug_value = 1;
        }
        else{
            button_debug_value=0;
        }
    }
    else{
        button_debug_debounce = 0;
        button_debug_value=0;
    }

    //here button_debug_value is 0 or 1 if pressed
    if (button_debug_value==1) {
        Serial.print("------------------------------- START BOILER -------------------------------");
        Serial.println("");

        turn_on_boiler();
    
    }
    
    Serial.print("button_debug_value: ");
    Serial.print(button_debug_value);
    Serial.println("");

}  