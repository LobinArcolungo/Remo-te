/*
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/e3652e97-c5ac-451c-9a38-e3cbcd2b0591

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when chhttps://docs.arduino.cc/static/a94a462df840d87411906381f90fa390/29114/cloud-esp32-img-16.pnganges are made to the Thing

  int cups;
  bool start_tea_making;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"

const int LED = 16;

int change;
unsigned long last_millis;



void setup() {
  
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();
  
  //LED
  change =0;
  pinMode(LED, OUTPUT);
  last_millis =0;

  Serial.begin(74880);

}

void loop() {
  ArduinoCloud.update();
  
    if( millis()!= last_millis && millis()%2000==0 ){
        change++;
        if (change%2 == 0 ) digitalWrite(LED,HIGH);
        else digitalWrite(LED,LOW);
        last_millis=millis();
    }

  
}

/*
  Since these variables are READ_WRITE variable, these functions are
  executed every time a new value is received from IoT Cloud.
*/
void onCupsChange(){}
void onPushButtonChange(){
  if (push_button == true && start_tea_making==false){
    if (DEBUG) Serial.print("Let's make ");
    if (DEBUG) Serial.print(cups);
    if (DEBUG) Serial.println(" cups of tea!");
    
    if (!start_tea_making){
      if (cups>=2 && cups<=6){
        
        if (DEBUG) Serial.println("Change ");
        start_tea_making = true;            //Start of start_tea_making semaphore
        push_button = false;
        ArduinoCloud.update();
        prepareTea(cups*250);
      }
    }
  }
    
}