#include <Arduino.h>

void readSerialAndRespond() {
   String s("");

    do {
        s = Serial.readString();
        Serial.println("readSerialAndRespond getting exceuted");
        Serial.println(s);
        Serial.flush();
    } while( s.indexOf("[@START_TEST@]") == -1 );

    Serial.println(s);
    Serial.flush();
}