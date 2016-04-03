#include <SoftwareSerial.h>
 
#define DEBUG 1
 
SoftwareSerial esp8266(1,0); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
                             
#define SSID "TELUS6815"
#define PASSWORD "bde840375e"
#define BUFFER_SIZE 201
char cookies[2];

void setup()
{
  pinMode(2,OUTPUT);
  esp8266.begin(115200); // your esp's baud rate might be different

  sendData("AT+RST\r\n",1000,DEBUG); // reset module
  sendData("AT+CWJAP=\"Connectify-me\",\"anhduc123\"\r\n",10000,DEBUG);
  sendData("AT+CWMODE=3\r\n",2000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",2000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",2000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",2000,DEBUG); // turn on server on port 80

  
//  sendData("AT+CIPSTART=0,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",3000,DEBUG);//10000
//  sendData("AT+CIPSEND=0,61\r\n",1000,DEBUG);
//  sendData("GET /add/abc HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n",3000,DEBUG);
//  add("abc");
//  sendData("AT+CIPCLOSE=0\r\n",2000,DEBUG);
//  sendData("AT+CIPCLOSE=0\r\n",2000,DEBUG);
  
//  sendData("AT+CIPSTART=1,\"TCP\",\"www.cookie4me.herokuapp.com\",80\r\n",2000,DEBUG); //5000
//  sendData("AT+CIPSEND=1,64\r\n",1000,DEBUG);
//  sendData("GET /verify/abc HTTP/1.1\r\nHost:cookie4me.herokuapp.com\r\n\r\n",5000,DEBUG);//5000
//  parseData("AT+CIPCLOSE=1\r\n",2000,DEBUG);
//  sendData("AT+CIPCLOSE=1\r\n",2000,DEBUG);
//  verify("abc");

  //add("123");
  //verify("hello");
  verify("123");
}
 
void loop()
{
//  add("abc");
//  verify("abc");
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
    char cookieCount[10];
    int count = 0;
    int test = 0;
    
    esp8266.print(command); // send the read character to the esp8266
    
    //esp8266.end();
    
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
//    Serial.print("\nThe cookie count is: ");
//
//    for (int i = 0; i < count; i++){
//      Serial.write(cookieCount[i]);
//    }

    if (cookieCount[0] == '7'){
      digitalWrite(2,HIGH);
    }


//    while (test < BUFFER_SIZE){
//      Serial.print("The character at index ");
//      Serial.print(test);
//      Serial.print(" is ");
//      Serial.println(response[test]);
//      test++;
//    }

    


    // Start here
//    while(response[0] == '3' || response[0] == '4' || response[0] == '5'){
//    if (response[1] != ','){
//      cookies[0] = response[0];
//      cookies[1] = response[1];
//      break;
//    }
//    else{
//      cookies[0] == response[0];
//      break;
//    }
//    }
    

//    esp8266.println(cookies[0]);
//    esp8266.println(cookies[1]);
    
   // My thing goes here
//    esp8266.println(response[0]);
//    esp8266.println(response[1]);
//
//    char cookies[2];
//    cookies[0] = response[0];
//    esp8266.println(cookies);
//    int i = 0;
//    while(true){
//      if(response[i+1] != ','){
//        cookies[i+1] = response[i+1];
//        i++; 
//      }
//      else
//        break;
//    }
//    esp8266.println(cookies);
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

void findCookies(void){
  
}




