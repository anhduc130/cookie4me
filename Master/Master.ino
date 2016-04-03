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

#include <SoftwareSerial.h>
 
#define DEBUG 1
 
SoftwareSerial esp8266(1,0); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
                             
#define SSID "TELUS6815"
#define PASSWORD "bde840375e"
#define BUFFER_SIZE 201
char cookieCount[10];

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

  initializeWifiModule();
  
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
    lcd.printChar(cookieCount[0]);
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
  last_card_read = 0;      
  
  lcd.clear();                   //resets lcd
  lcd.cursorTo(0, 0);
}

void initializeWifiModule(){
  pinMode(2,OUTPUT);
  esp8266.begin(115200); // your esp's baud rate might be different

  sendData("AT+RST\r\n",1000,DEBUG); // reset module
  sendData("AT+CWJAP=\"Connectify-me\",\"anhduc123\"\r\n",10000,DEBUG);
  sendData("AT+CWMODE=3\r\n",2000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",2000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",2000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",2000,DEBUG); // turn on server on port 80

  verify("123");
}

void sendData(String command, const int timeout, boolean debug)
{
    char response[BUFFER_SIZE];
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    { 
    }
}

void parseData(String command, const int timeout, boolean debug)
{
    char response[BUFFER_SIZE];
    int count = 0;
    int test = 0;
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window 
        for(int i = 0; i < BUFFER_SIZE; i++){
          char c = (char) esp8266.read(); // read the next character.
          response[i] = c;
        }
       }  
    }

    esp8266.end();
    Serial.begin(115200);
    while (response[count] != ','){
      cookieCount[count] = response[count];
      count++;
    }

    if (cookieCount[0] == '7'){
      digitalWrite(2,HIGH);
    }
}

void add(String key_tag){
  String command = "GET /add/";
  command += key_tag;
  command += " HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n";

  sendData("AT+CIPSTART=0,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",3000,DEBUG);
  sendData("AT+CIPSEND=0,61\r\n",1000,DEBUG);
  sendData(command,5000,DEBUG);
  sendData("AT+CIPCLOSE=0\r\n",2000,DEBUG);
  sendData("AT+CIPCLOSE=0\r\n",2000,DEBUG);
}

void verify(String key_tag){
  String command = "GET /verify/";
  command += key_tag;
  command += " HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n";

  sendData("AT+CIPSTART=1,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",2000,DEBUG);
  sendData("AT+CIPSEND=1,64\r\n",1000,DEBUG);
  sendData(command,5000,DEBUG);
  parseData("AT+CIPCLOSE=1\r\n",2000,DEBUG);
  sendData("AT+CIPCLOSE=1\r\n",2000,DEBUG);
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
    lcd.printLCD("(: cookie4.me ;)"); // make this scroll?
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

