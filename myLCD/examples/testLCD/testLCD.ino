#include <myLCD.h>

#include <stdio.h>
#include <string.h>

// rs, en, db0, db1, db2, db3
myLCD lcd(0, 1, 2, 3, 4, 5);
boolean displayTitle;

void setup() {
  lcd.initialize();
  displayTitle = true;
}

void loop() {
 if(displayTitle){
    lcd.printLCD("Ayyyy girl,");
    lcd.cursorTo(1,0);
    lcd.printLCD("wan sum cookie?");
    delay(1000);
 }
  displayTitle = false;
  
}
