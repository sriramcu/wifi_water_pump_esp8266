
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266SMTP.h>
int val = 1; //boot relay as high



#ifndef STASSID
#define STASSID "YOUR_NETWORK_NAME"
#define STAPSK  "NETWORK_PASSWORD"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiClient espClient;
;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

int   toggle = 0,off = 0, on = 0;

const int relay_pin = 4;//D2
int motor_state;
int aval;

void setup() {
  Serial.begin(115200);
  pinMode(relay_pin,OUTPUT);
  digitalWrite(relay_pin,val);
  pinMode(16,OUTPUT);
  digitalWrite(16,HIGH);
  pinMode(13,INPUT);
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);
 
  // prepare LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());


  //TURN OFF MOTOR WHEN ESP IS POWERED ON, NO MATTER WHAT
  int repeat0 = 0, repeat1 = 0;
  aval = analogRead(A0);
  if(aval<500)
  {   
      delay(50);
      while(repeat0<4)
      {
        aval = analogRead(A0);
        if(aval<500)
          repeat0++;
        else
          break;

         delay(50);
        
        }
    
   }

   
  else if(aval>500)
  {   
      delay(50);
      while(repeat1<4)
      {
        aval = analogRead(A0);
        if(aval>500)
          repeat1++;
        else
          break;

         delay(50);
        
        }
    
   }
  

  if(repeat0>repeat1)
    motor_state = 0;//do nothing

  else{
    val = !(val);
    digitalWrite(relay_pin,val); //toggle relay to turn off motor
    motor_state = 0;
    
  }
    
  
}

void loop() 
{
  int tmr=0;
  while(1)
  {
    //if((millis()-tmr)>5000) //speaker on for 5 seconds
      //digitalWrite(12,LOW);
    int qval=0;
    
      
    
  int delay_flag = 0;
   

  int repeat0 = 0, repeat1 = 0;
  aval = analogRead(A0);
  if(aval<500)
  {   
      delay(50);
      while(repeat0<4)
      {
        aval = analogRead(A0);
        if(aval<500)
          repeat0++;
        else
          break;

         delay(50);
        
        }
    
   }

   
  else if(aval>500)
  {   
      delay(50);
      while(repeat1<4)
      {
        aval = analogRead(A0);
        if(aval>500)
          repeat1++;
        else
          break;

         delay(50);
        
        }
    
   }
  if(repeat0!=4 && repeat1!=4)
    continue;

  else if(repeat0==4)
    motor_state = 0;

  else
    motor_state = 1;


  //BUZZ
  if(motor_state == 1 && ((millis()-tmr)>6000))
    {
    while(digitalRead(13)==HIGH)
    {
      if((qval%100 == 0)||qval>180)
        digitalWrite(12,HIGH);
      else
        digitalWrite(12,LOW);
      qval++;
      delay(20);
      if(qval==400)
        break;
        
      }
      digitalWrite(12,LOW);
    }

    
    if(qval==400)
    {
      //recalculate motor_state
      if(analogRead(A0)>500)
        motor_state = 1;
       else
        motor_state = 0;
       if(motor_state == 1)
      {
        val = !(val);
        
      }
        motor_state = 0; 
        digitalWrite(relay_pin,val);
       tmr = millis();
       
        

        continue;
        
      }

  
  ArduinoOTA.handle();
  delay(1000);
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    continue;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  
  
  if (req.indexOf(F("/motor/0")) != -1) {
    if(motor_state == 1)
    {
      val = !(val);
      delay_flag = 1;
    }
      motor_state = 0;
  } else if (req.indexOf(F("/motor/1")) != -1) {
    if(motor_state == 0)
      val = !(val);
     motor_state = 1;
  } else {
    Serial.println(F("invalid request"));
   
  }


  digitalWrite(relay_pin,val);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nMotor is now "));
  client.print((motor_state) ? F("on") : F("off"));
  client.print(F("<br><br>Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/motor/1'>here</a> to switch motor on, or <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/motor/0'>here</a> to switch motor off(5 second delay) or <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/motor/state'>here</a> to check current state of the motor.</html>"));

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}
}


