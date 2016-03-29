#include <Servo.h>
#include <myLCD.h>
#include <stdio.h>
#include <string.h>

Servo myservo;  //Create a new servo object to control the cookie arm

// rs, en, db0, db1, db2, db3
myLCD lcd(0, 1, 2, 3, 4, 5);
boolean displayTitle;

RFID rfid();
unsigned long last_card_read = 0;

void setup() {
  myservo.attach(3);  // attaches the servo on pin 3 to the servo object
  lcd.initialize();
  displayTitle = true;
  rfid.initialize();
}

void loop() {
  checkID();
}

void dispenseCooke(){
  myservo.write(0);
  delay(2000);
  myservo.write(60);
  delay(2000);
  myservo.write(0);
}

void displayLCD(){
 if(displayTitle){
    lcd.printLCD("Ayyyy girl,");
    lcd.cursorTo(1,0);
    lcd.printLCD("wan sum cookie?");
    delay(1000);
 }
  displayTitle = false;
}

void checkID(){
  if (rfid.is_available()){
    last_card_read = rfid.get_rfid_id();
  }
}

