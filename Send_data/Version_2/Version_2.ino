#include <SoftwareSerial.h>
 
#define DEBUG 1
 
SoftwareSerial esp8266(1,0); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
                             
#define SSID "TELUS6815"
#define PASSWORD "bde840375e"
#define BUFFER_SIZE 201
char *response;

void setup()
{
//  Serial.begin(115200);
//  Serial.println(response);
  esp8266.begin(115200); // your esp's baud rate might be different
  esp8266.println(response);
  sendData("AT+RST\r\n",1000,DEBUG); // reset module
  sendData("AT+CWJAP=\"Connectify-me\",\"anhduc123\"\r\n",10000,DEBUG);
  sendData("AT+CWMODE=3\r\n",2000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",2000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",2000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",2000,DEBUG); // turn on server on port 80
  sendData("AT+CIPSTART=0,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",3000,DEBUG);//10000
  sendData("AT+CIPSEND=0,61\r\n",1000,DEBUG);
  sendData("GET /add/abc HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n",3000,DEBUG);
  sendData("AT+CIPCLOSE=0\r\n",2000,DEBUG);
  sendData("AT+CIPCLOSE=0\r\n",2000,DEBUG);
  
  sendData("AT+CIPSTART=1,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",3000,DEBUG); //5000
  sendData("AT+CIPSEND=1,64\r\n",1000,DEBUG);
  sendData("GET /verify/abc HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n",3000,DEBUG);//5000
  parseData("AT+CIPCLOSE=1\r\n",2000,DEBUG);
}
 
void loop()
{

}

/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
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
    esp8266.println("Start of the response: ");
    esp8266.println(response[0]);
    
    if(debug)
    {
      Serial.print(response);
    }
}
