
// Stepper motor
const int A1A = 2;
const int A1B = 3;
const int B1A = 4;
const int B1B = 5;

int stepnumber = 0;
double mf = 2*PI/128; // microstep factor

int Pa,Pb;

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

  delayMicroseconds(wait);

}


void setup() {
    pinMode(A1A,OUTPUT);
    pinMode(A1B,OUTPUT);
    pinMode(B1A,OUTPUT);
    pinMode(B1B,OUTPUT);
    Serial.begin(9600);

    pinMode(13,OUTPUT);
}

void loop() {
  for (int i = 0; i<2048;i++){
    stepnumber++;
    move(stepnumber,150,750);
  }
  digitalWrite(13,HIGH);
  delay(500);
  digitalWrite(13,LOW);
  for (int i = 1024; i>512;i--){
    stepnumber--;
    move(stepnumber,150,1000);
  }
  delay(200);

}