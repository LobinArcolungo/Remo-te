#include <Servo.h>
Servo myservo;

int pos=0;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(9);
  
    myservo.write(0);
    delay(2000);
}

void loop() {
  for(pos=0;pos<=180;pos+=1){
    myservo.write(pos);
    delay(50);
    }
  for(pos=180;pos>=0;pos-=1){
    myservo.write(pos);
    delay(50);
    }
}
