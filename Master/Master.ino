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
SoftwareSerial esp8266(1,0); //Defines the SoftwareSerial ports to connect the RX and TX pins of the ESP8266

#define BUFFER_SIZE 195
                      
//Character array to store the current cookie count
volatile char cookieCount[2];

//Variable to determine if the user has been granted access (ie. the tag has been validated from our database)
boolean accessGranted = false;

Servo cookie_servo;  //Create a new servo object to control the cookie arm

// create lcd object with pins rs, en, db0, db1, db2, db3
myLCD lcd(LCD_RS, LCD_EN, LCD_DB0, LCD_DB1, LCD_DB2, LCD_DB3);

// create rfid object (digital IO pins 2 and 3)
RFID rfid;
 
// variable to hold rfid card number, 0 means no card scanned
unsigned long last_card_read = 0;
String rfid_tag = " ";

void setup() {
 
    // attach servo pin to servo object and put in default position
    cookie_servo.attach(COOKIE_SERVO);  
    cookie_servo.write(0);
  
    // initialize lcd display
    lcd.initialize();
    
    // display messages on LCD while WiFi is connecting
    lcd.clear();                   
    lcd.cursorTo(0, 0);
    lcd.printLCD("Connecting...");
    lcd.cursorTo(1,0);
    lcd.printLCD("Please wait.");
    initializeWifiModule();
  
    // initialize rfid reader
    rfid.initialize();

    // initialize piezo buzzer  
    pinMode(PIEZO_PIN, OUTPUT);

    // turn on blue LED near cookie opening 
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void loop() {
    //displays welcome message
    displayLCDWelcome();
  
    //constantly checks for ID
    while(last_card_read == 0) {
        checkID();
    }

    // beeps and displays message if RFID tag tapped 
    buzzRFIDTapped();
    lcd.clear();
    lcd.cursorHome();
    lcd.printLCD("Read successful!");
    delay(1000);    // reading delay
    
    // convert the tag number to a String
    rfid_tag = String(last_card_read);
     
    // verify RFID tag by sending it through the WiFi module to the web backend
    lcd.cursorTo(1,0);
    lcd.printLCD("Verifying...");
    verify(rfid_tag);

    lcd.clear();
    lcd.cursorTo(0, 0);
  
    // true if the RFID tag is located in the web database
    if(accessGranted){
        buzzAccessGranted();
        lcd.printLCD("Access Granted!");
                
        dispenseCookie();       // moves servo arm to push a cookie out 
        
        // increase the amount of cookies the user has eaten -> updates web database
        add(rfid_tag);              
        incrementCookieCount();
      
        // display their eaten cookie count on the LCD 
        lcd.clear();
        lcd.cursorTo(0,0);
        lcd.printLCD("You've eaten...");
        lcd.cursorTo(1, 0);
        lcd.printCharLCD(cookieCount[0]);
        lcd.printCharLCD(cookieCount[1]);
        if(cookieCount[0] == ' ' && cookieCount[1] == '1'){
          lcd.printLCD(" cookie!");
        }
        else {
          lcd.printLCD(" cookies!");
        }

        // reading delay
        delay(3000); 
      
        accessGranted = false;
    }
    else{
        lcd.printLCD("Access Denied!");
        buzzAccessDenied();
        delay(2000);
    }
  
    //resets last card id
    last_card_read = 0;      
  
    lcd.clear();                   //resets lcd
    lcd.cursorTo(0, 0);
    rfid.initialize();              // reset RFID scanner
}

//Increment the cookie count obtained from the verification process to show the user the updated cookie count on the LCD
void incrementCookieCount(){
    if (cookieCount[1] == '9'){
        cookieCount[0]++;
        cookieCount[1] = '0';
    }
    else {
        cookieCount[1]++; 
    }
}

//Initialize the ESP8266 WiFi Module
void initializeWifiModule(){
    esp8266.begin(115200); // default baud rate of the ESP8266 Module

    //Resets the WiFi module
    sendData("AT+RST\r\n",1000); 

    //Join the access point of the WiFi hotspot created, with the WiFi network "Connectify-me" and the password "anhduc123"
    sendData("AT+CWJAP=\"Connectify-me\",\"anhduc123\"\r\n",10000); 

    sendData("AT+CWMODE=3\r\n",2000); // configure as access point
    sendData("AT+CIFSR\r\n",2000); // get ip address
    sendData("AT+CIPMUX=1\r\n",2000); // configure for multiple connections
    sendData("AT+CIPSERVER=1,80\r\n",2000); // turn on server on port 80

    lcd.clear();                   //resets lcd
    lcd.cursorTo(0,0);
}

//Sends the given command through the SoftwareSerial port to the WiFi module so that the Module recognizes it as a command
void sendData(String command, const int timeout)
{   
    esp8266.print(command); // send the command to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    { 
    }
}

//Sends the given command through the SoftwareSerial port to the WiFi module and parse through the response from the server to get cookie count
void parseData(String command, const int timeout)
{
    //Initialize a character array of size buffer_size
    char response[BUFFER_SIZE];
    
    esp8266.print(command); // send the command to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
        while(esp8266.available())
        {
            // Read the characters from the Serial monitor into the character array 
            for(int i = 0; i < BUFFER_SIZE; i++){
                char c = (char) esp8266.read(); // read the next character.
                response[i] = c;
            }
        }  
    }
    //End the esp8266 SoftwareSerial port since commands are finished and start the Serial port to begin printing out the cookie count
    esp8266.end();
    Serial.begin(115200);

    // Find the comma after the cookie count 
    int comma_index = 0;
    while(response[comma_index] != ','){
        comma_index++;
    }

    //The two digits of the cookie count are indexed before the comma
    char tens_digit = response[comma_index-2];
    char ones_digit = response[comma_index-1];

    //Write the cookie count into the global character array
    cookieCount[0] = tens_digit;
    cookieCount[1] = ones_digit;

    //Print the cookie count to the Serial monitor
    Serial.write(cookieCount[0]);
    Serial.write(cookieCount[1]);

    //Check to see if the cookie count is a valid number, if so, then the website returned a JSON response and we can confirm access granted to the user.
    //If the cookie count is not a valid number, then the website returned an invalid HTML page that indicates we should deny access.
    if ((cookieCount[1] >= '0' && cookieCount[1] <= '9') && ( (cookieCount[0] >= '0' && cookieCount[0] <='9' ) || cookieCount[0] == ' ' )){
        accessGranted = true;
    }
    else {
        accessGranted = false;
    }

    //End Serial and start the SoftwareSerial port to be able to send AT commands afterwards.
    Serial.end();
    esp8266.begin(115200);
}

//Send a GET request to the server to increment the cookie count of the current user with the given key tag
void add(String key_tag){
    lcd.clear();
    lcd.cursorTo(0,0);
    lcd.printLCD("Updating your");
    lcd.cursorTo(1,0);
    lcd.printLCD("cookie count...");
    String command = "GET /add/";
    command += key_tag;
    command += " HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n";

    sendData("AT+CIPSTART=0,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",3000);
    sendData("AT+CIPSEND=0,65\r\n",1000);
    sendData(command,5000);
    sendData("AT+CIPCLOSE=0\r\n",2000);
    sendData("AT+CIPCLOSE=0\r\n",2000);
}

//Send a GET request to the server to verify the given key tag is valid
void verify(String key_tag){
    String command = "GET /verify/";
    command += key_tag;
    command += " HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n";

    sendData("AT+CIPSTART=1,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",2000);
    sendData("AT+CIPSEND=1,68\r\n",1000);
    sendData(command,5000);
    parseData("AT+CIPCLOSE=1\r\n",2000);
    sendData("AT+CIPCLOSE=1\r\n",2000);
}

// This function dispenses one cookie when called
void dispenseCookie(){
    cookie_servo.write(0);
    delay(2000);
    cookie_servo.write(60);
    delay(2000);
    cookie_servo.write(0);
}

// LCD welcome/default message for the cookie dispenser
void displayLCDWelcome(){
    lcd.printLCD("Please tap RFID.");
    lcd.cursorTo(1,0);
    lcd.printLCD("   cookie4.me   "); // make this scroll?
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

// Short beep fromo the piezo if RFID tag read successfully 
void buzzRFIDTapped(){
    analogWrite(PIEZO_PIN, 329.63);
    delay(300);
    analogWrite(PIEZO_PIN, 0);
}


