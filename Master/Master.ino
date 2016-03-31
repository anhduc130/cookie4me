/*
 *  cookie4.me - ELEC 291 Group 4 - Project 2
 *  An Internet-enabled, diet-tracking cookie dispenser!
 */

#include "Master_constants.h"

#include <myLCD.h>
#include <Rfid.h>

#include <Servo.h>
#include <stdio.h>
#include <string.h>

Servo cookie_servo;  //Create a new servo object to control the cookie arm

// create lcd object with pins rs, en, db0, db1, db2, db3
myLCD lcd(LCD_RS, LCD_EN, LCD_DB0, LCD_DB1, LCD_DB2, LCD_DB3);

// create rfid object (digital IO pins 2 and 3)
RFID rfid;
// variable to hold rfid card number, 0 means no card scanned
unsigned long last_card_read = 0;

void setup() {
 
 // attach servo pin to servo object and put in default position
  cookie_servo.attach(COOKIE_SERVO);  
  cookie_servo.write(0);
  
  lcd.initialize();
  
  rfid.initialize();
  
  pinMode(PIEZO_PIN, OUTPUT);
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
  char *name = "Bob";
  char *numCookies = "25";
  
  //if(accessGranted){
  if(last_card_read == 9410488){
    buzzAccessGranted();
    lcd.printLCD("Enjoy, ");
    lcd.printLCD(name);
    lcd.printLCD("!");
    lcd.cursorTo(1,0);
    lcd.printLCD("Total: ");
    lcd.printLCD(numCookies);
    lcd.cursorTo(0, 0);
        
    dispenseCookie();       //dispense cookie   
    delay(1000);
  }
  else{
    lcd.printLCD("Access Denied.");
    buzzAccessDenied();
    delay(2000);
  }
  
  //resets last card id
  rfid.set_last_card_zero();
  last_card_read = 0;      
  
  lcd.clear();                   //resets lcd
  lcd.cursorTo(0, 0);
}

// This function dispenses one cookie when called
void dispenseCookie(){
  cookie_servo.write(0);
  delay(2000);
  cookie_servo.write(60);
  delay(2000);
  cookie_servo.write(0);
}

// Welcome message
void displayLCDWelcome(){
    lcd.printLCD("Please tap RFID.");
    lcd.cursorTo(1,0);
    lcd.printLCD("~~~cookie4.me~~~"); // make this scroll?
}

// This function uses an RFID and checks for card and reads its ID 
// returns ID
void checkID(){
  if (rfid.is_available()){
    last_card_read = rfid.get_rfid_id();
  }
}

// Briefly buzzes the piezo if RFID access granted 
void buzzAccessGranted(){
     analogWrite(PIEZO_PIN, 440);
     delay(300);
     analogWrite(PIEZO_PIN, 0);
}

// Rapidly buzzes the piezo if RFID access denied
void buzzAccessDenied(){
  for (int i = 0 ; i < 8 ; i++){
    analogWrite(PIEZO_PIN, 75);
    delay(100);
    analogWrite(PIEZO_PIN, 0);
    delay(100);
  }
}

