#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <String.h>
#define ACCEPT_TYPE "text/csv"
WiFiClient client;

const char ssid[] = "CSED_IIOT";
const char password[] = "csed1234";
const char put_Thing[] = "Gorup1_thing_downstream";
const char get_Property3[] = "valve";
const char host[] = "172.16.78.20";
const int httpsPort = 7080;
const char appKey[]= "9cf53b78-adcf-455b-9f39-caa7b9c76d47";

const char Thing[] = "Gorup1_thing_downstream";
const char Property[] = "flow2";

const char Thing1[] = "Gorup1_thing_downstream";
const char Property1[] = "flow1";

const char Thing2[] = "Gorup1_thing_downstream";
const char Property2[] = "distance";

const char Thing3[] = "Gorup1_thing_downstream";
const char Property3[] = "temperature";

const char Thing4[] = "Gorup1_thing_downstream";
const char Property4[] = "volume1";

const char Thing5[] = "Gorup1_thing_downstream";
const char Property5[] = "volume2";


void setup()
{
  Wire.begin(4,5);  
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
{
    delay(500);
    Serial.print(".");
 }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}


void loop()
{
  Wire.requestFrom(8, 64);
 char array[64];
  int i=0;
  while (0 < Wire.available())
  {
    char c = Wire.read();
    array[i] = c;
    Serial.println(array[i]);
    i++;
    
  }
 
  char *strings[7]; 
  char *ptr = NULL;   
  byte index = 0;
  ptr = strtok(array, ",");
  while (ptr != NULL)
  {
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");
  }
  Serial.println(strings[0]);   //Distance
  Serial.println(strings[1]);   //temperature
  Serial.println(strings[2]);   //flow1
  Serial.println(strings[3]);   //flow2
  Serial.println(strings[4]);   //Volume1
  Serial.println(strings[5]);   //volume2
  // Serial.println(strings[6]);   //Noise

  Put(Thing,Property,strings[3]);
  Put(Thing4,Property4,strings[4]);
  Put(Thing1,Property1,strings[2]);
  Put(Thing5,Property5,strings[5]);
  Put(Thing2,Property2,strings[0]);
  Put(Thing3,Property3,strings[1]);  
  
 
  String res = Get(put_Thing,get_Property3);
 // Serial.print(res);
  Serial.println(res[9]);
 
  Wire.beginTransmission(8);
  Wire.write(res[9]);
  Wire.endTransmission();

  delay(1000);
}

void Put(String ThingName, String ThingProperty, String Value)
  {

    Serial.println(host);
  if (!client.connect(host, httpsPort))
 {
    Serial.println("connection failed");
    return;
  } else

{
    Serial.println("Connected to ThingWorx.");
  }
  String url = "/Thingworx/Things/" + ThingName + "/Properties/" + ThingProperty;
  Serial.print("requesting URL: ");
  Serial.println(url);

  String strPUTReqVal = "{\"" + ThingProperty + "\":\"" + Value + "\"}";  
  Serial.print("PUT Value: ");
  Serial.println(strPUTReqVal);

  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "appKey:"+ appKey + "\r\n" +
               "x-thingworx-session: false" + "\r\n" +
               "Accept: application/json" + "\r\n" +
               "Connection: close" + "\r\n" +
               "Content-Type: application/json" + "\r\n" +
               "Content-Length: " + String(strPUTReqVal.length()) + "\r\n\r\n" +
               strPUTReqVal + "\r\n\r\n");  

  while (client.connected())
{
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  client.stop();
}

String Get(String get_Thing, String get_Property)
{              
        HTTPClient http;
        int httpCode = -1;
        String fullRequestURL = "http://" + String(host) +":"+ String(httpsPort)+ "/Thingworx/Things/"
                                + get_Thing + "/Properties/" + get_Property + "?appKey=" + appKey;
                               
        Serial.println(fullRequestURL);
        http.begin(client, fullRequestURL);
        http.addHeader("Accept",ACCEPT_TYPE,false,false);        
        httpCode = http.GET();
        Serial.println(httpCode);
       
        String responses;
        if(httpCode > 0)        
        {            
           responses= http.getString();
             //Serial.println(responses);      
             //Serial.print(responses[9]);        
        }
        else        
        {
            Serial.printf("[httpGetPropertry] failed, error: %s\n\n", http.errorToString(httpCode).c_str());
        }
        http.end();
        return responses;        
}