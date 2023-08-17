const int pwmMotorA = D1;
const int pwmMotorB = D2;
const int dirMotorA = D3;
const int dirMotorB = D4;

int delayBtwnStep = 3;

void setup() {
    Serial.begin ( 115200 );
    Serial.println();

    pinMode(pwmMotorA, OUTPUT);
    pinMode(pwmMotorB, OUTPUT);
    pinMode(dirMotorA, OUTPUT);
    pinMode(dirMotorB, OUTPUT);
    Serial.println("Motor SHield 12E Initialized");
}

void loop() {
    stepperRotate(1000, 0);
    delay(100);
    stepperRotate(1000, 1);
    delay(100);
}


void stepperRotate(int nbStep, int dirStep) {
    for (int i = 0; i <= nbStep; i++) {
        if (dirStep == 0) {      // sens horaire
                nextStep(i % 4);
        }
        if (dirStep == 1) {      // sens antihoraire
                nextStep(3 - (i % 4));
        }
        delay(100);
    }
}


void nextStep(int index) {
    if (index == 0) {
        digitalWrite(pwmMotorA, true);
        digitalWrite(pwmMotorB, false);
        digitalWrite(dirMotorA, true);
        digitalWrite(dirMotorB, false);
    }
    if (index == 1) {
        digitalWrite(pwmMotorA, false);
        digitalWrite(pwmMotorB, true);
        digitalWrite(dirMotorA, true);
        digitalWrite(dirMotorB, false);
    }
    if (index == 2) {
        digitalWrite(pwmMotorA, false);
        digitalWrite(pwmMotorB, true);
        digitalWrite(dirMotorA, false);
        digitalWrite(dirMotorB, true);
    }
    if (index == 3) {
        digitalWrite(pwmMotorA, true);
        digitalWrite(pwmMotorB, false);
        digitalWrite(dirMotorA, false);
        digitalWrite(dirMotorB, true);
    }
}