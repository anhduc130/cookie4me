/*
  myLCD.h - myLCD library 
*/

// ensure this library description is only included once
#ifndef myLCD_h
#define myLCD_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include "string.h"

// library interface description
class myLCD
{
  // user-accessible "public" interface
  public:
    myLCD(int rs, int en, int db0, int db1, int db2, int db3);
    void initialize();
    void printCharLCD(int c);
    void commandLCD(int c);
    void writeLCD(int n);
    void cycleEn();
    void clear();
    void cursorHome();
    void cursorTo(int row, int column);
    void printLCD(char *message);
    void banner();

  // library-accessible "private" interface
  private:
    int CLEAR_DISPLAY = 0x01;
    int RETURN_HOME = 0x02;
    int db[4];
    int en;
    int rs;

};

#endif

  