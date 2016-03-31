#include <SoftwareSerial.h>
 
#define DEBUG 1
 
SoftwareSerial esp8266(1,0); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
                             
#define SSID "TELUS6815"
#define password "bde840375e"
String response = "";
void setup()
{
  esp8266.begin(115200); // your esp's baud rate might be different
  
  sendData("AT+RST\r\n",3000,DEBUG); // reset module
  //sendData("AT+CWJAP=\"Connectify-me\",\"123456789\"",5000,DEBUG);
  sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  sendData("AT+CIPSTART=0,\"TCP\",\"cookie4me.herokuapp.com\",80\r\n",5000,DEBUG);
  sendData("AT+CIPSEND=0,64\r\n",5000,DEBUG);
  sendData("GET /verify/123 HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n",5000,DEBUG);
  sendData("AT+CIPCLOSE=0\r\n",1000,DEBUG);
}
 
void loop()
{

}


 
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      //Serial.print(response);
    }
    
    return response;
}

