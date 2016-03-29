#include "Arduino.h"
#include "myLCD.h"

// constructor
myLCD::myLCD(int r, int e, int db0, int db1, int db2, int db3){
	rs = r;
	en = e;

	db[0] = db0;
	db[1] = db1;
	db[2] = db2;
	db[3] = db3;

	// set data pins
	int i;
	for(i=0; i<4; i++){
		pinMode(db[i], OUTPUT);
	}

	// set the rs and en pins
	pinMode(en, OUTPUT);
	pinMode(rs, OUTPUT);

	// initialize LCD to 4 bit data mode
	//initialize();
}

// initialize LCD for 4-bit interface, ignore R/W since it's grounded
void myLCD::initialize() {
  digitalWrite(rs,LOW);
  digitalWrite(en,LOW);
  delay(50); //wait > 40 ms
  writeLCD(0x03);
  delay(5); //wait > 4.1 ms
  writeLCD(0x03);
  delayMicroseconds(150); //wait > 100us
  commandLCD(0x32);

  commandLCD(0x2C); //Sets it to two lines and 5x10 dots

  commandLCD(0x0C); //Display ON

  clear(); //Display clear

  commandLCD(0x06); //Entry mode set
}

//Takes an integer and takes the lower 8 bits and splits it up into 4 bits
void myLCD::printCharLCD(int c) {
  digitalWrite(rs, HIGH); //Have to set rs bit high in order to write to RAM
  //Write the top 4 bits
  writeLCD(c>>4);
  //Write the bottom 4 bits
  digitalWrite(rs, HIGH); //Have to set rs bit high in order to write to RAM
  writeLCD(c);
}

//Write instructions to the LCD, sets rs to LOW indicating instruction
void myLCD::commandLCD(int c) {
  digitalWrite(rs, LOW);
  writeLCD(c >> 4);
  writeLCD(c);
}

//Given an integer the lower 4 bits will be written to the pins
//MSB through LSB of lower 4 bits mapped to pins 7-4 respectively
//parameters:
  //n the integer value to convert to 4 bit output
void myLCD::writeLCD(int n) {
  for(int i = 0; i < 4; i++){
    digitalWrite(db[i], (n>>i) & 0x01);
  }
  cycleEn(); //Cycle the enable after each write
}

//Switches enable from low to high to low
//cycle time has to be greater than 400ns
//wait 1000us since it's a safe bet for execution time
//Call this each time data register is written to
void myLCD::cycleEn() {
  digitalWrite(en,LOW);
  delayMicroseconds(2);
  digitalWrite(en,HIGH);
  delayMicroseconds(2);
  digitalWrite(en,LOW);
  delayMicroseconds(200);
}

//Clears the LCD
void myLCD::clear() {
  digitalWrite(rs, LOW);
  writeLCD(CLEAR_DISPLAY >> 4);
  writeLCD(CLEAR_DISPLAY);
    delay(4); //Wait 4 milliseconds
}

//Sets the cursor to base address 0x80
void myLCD::cursorHome() {
  digitalWrite(rs,LOW);
  writeLCD(RETURN_HOME >> 4);
  writeLCD(RETURN_HOME);
  delay(4);
}

//Sets the row and column of the lcd with (0,0) being the top left. 
//If row > 1 and column > 64 then it will not change the cursor
//parameters:
  //row: zero indexed, so top row is row zero
  //column: zero indexed, so left column is row zero
void myLCD::cursorTo(int row, int column) {
  int baseAddress = 0x80;
  if(row < 2 && column <= 0x40){
    int address = baseAddress + column + 0x40*row;
    //Since base address is 0x80, all we have to do is bitwise or the offset
    commandLCD(baseAddress|address);
  }
}

//Prints a message (string) to the lcd, set the cursor prior to using
//parameter
  //message: String to print
void myLCD::printLCD(char *message) {
  int i;
  for (i = 0; message[i] != 0; i++){
    printCharLCD(message[i]);
  }
}

//Banner output scrolls the message ELEC 291-20C Team L2C-4A
void myLCD::banner(){
  char *message1 = "ELEC 291-20C    Team L2C-4A";
  char *message2 = "Team L2C-4A";
  for(int i = 0; i <= strlen(message1); i++){
    clear();
    char *subString1 = &message1[i];
    cursorTo(0,0);
    printLCD(subString1);

    if(i < strlen(message2)) {
      char *subString2 = &message2[i];
      cursorTo(1,0);
      printLCD(subString2);
    }
    delay(500);
  }
}