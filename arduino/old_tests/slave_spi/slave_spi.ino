#include <SPI.h>
#define BUZ 2
#define SS 10
char status;
void setup() {
pinMode(BUZ, OUTPUT);
pinMode(MOSI, INPUT);
pinMode(MISO, OUTPUT);
pinMode(SS, INPUT);
digitalWrite(MISO, LOW);
}

void loop() {
while(digitalRead(SS)); // Wait until LOW is received on SS
 // SPI on
 SPCR = (1<<SPE)|(0<<DORD)|(0<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(1<<SPR0);
while(!(SPSR & (1<<SPIF))); // Wait until the interrupt bit is freed
 status = SPDR;
 // SPI off
 SPCR = (0<<SPE)|(0<<DORD)|(0<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(1<<SPR0);
if (status == 'l'){
 digitalWrite(BUZ, HIGH);
 Serial.println("high\n");}
else
 digitalWrite(BUZ, LOW);
}
