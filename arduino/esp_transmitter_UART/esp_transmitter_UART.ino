
// Serial Input Parsing
const int BUF_SIZE=10;
char buf[10];
int rlen;

String alphabet = "qwertyuiopasdfghjklzxcvbnm";
int index_in_alphabet;

void setup() {  
  pinMode(D2,OUTPUT);
  Serial.begin(74880);               // initialize serial communication at 74880 bits per second:
  Serial1.begin(74880);            // initialize UART with baud rate of 74880

}
void loop() {
  if (Serial.available()){
    rlen = Serial.readBytes(buf,BUF_SIZE);
    Serial.print("Received form Serial: ");
    Serial.println(buf);
    index_in_alphabet = alphabet.indexOf(buf[0]);
    if (index_in_alphabet>=0){
        String message = "command:";
        message.concat(buf[0]);
        Serial.println(index_in_alphabet);
        Serial.print("sending:");
        Serial.println(message);
        Serial1.print(message);
    }
  }
}