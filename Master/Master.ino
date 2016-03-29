#include <Servo.h>
#include <myLCD.h>
#include <stdio.h>
#include <string.h>
#include "Rfid.h"

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
  //displays welcome message
  displayLCDWelcome();

  //constantly checks for ID
  while(last_card_read == 0) {
    checkID();
  }

  lcd.clear();
  lcd.cursorTo(0, 0);
  
  // send RFID tag with anh duc then to joey and confirm
  boolean accessGranted;
  
  if(accessGranted){
    printLCD("YOU GET A COOKIE!!11");
    dispenseCookie();       //dispense cookie
    delay(1000);
    last_card_read = 0;     //resets last card id
  }
  else{
    printLCD("RE-FUCKING-JECTED");
    delay(2000);
    last_card_read = 0;      //resets last card id
  }
  clear();                   //resets lcd
  lcd.cursorTo(0, 0);
}

// This function dispenses one cookie when called
void dispenseCookie(){
  myservo.write(0);
  delay(2000);
  myservo.write(60);
  delay(2000);
  myservo.write(0);
}

//Welcome message
void displayLCDWelcome(){
    lcd.printLCD("Ayyyy girl,");
    lcd.cursorTo(1,0);
    lcd.printLCD("wan sum cookie?");
    delay(1000);
}

//This function uses an RFID and checks for card and reads its' ID 
//returns ID
void checkID(){
  if (rfid.is_available()){
    last_card_read = rfid.get_rfid_id();
  }
}

