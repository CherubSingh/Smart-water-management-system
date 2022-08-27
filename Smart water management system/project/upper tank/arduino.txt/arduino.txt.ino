#include <Wire.h>
#include <FlowMeter.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#define trigPin 12
#define echoPin 13
#define ValveCW 7
#define ValveCCW 8
#define ONE_WIRE_BUS 5

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celcius=0;

char valve ;
int flag = 0;

unsigned int avgValueUS; 
int bufUS[10],tempUS;
unsigned long duration;
 
FlowMeter Meter1 = FlowMeter(2);
FlowMeter Meter2 = FlowMeter(3);
uint8_t period = 1000;

String str;

void setup(){
  Wire.begin(8);  
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendEvent);
  
  Serial.begin(115200);
  
  sensors.begin();

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  
  pinMode(ValveCW,OUTPUT);
  pinMode(ValveCCW,OUTPUT);
  digitalWrite(ValveCW,1);
  digitalWrite(ValveCCW,1);
 
  attachInterrupt(digitalPinToInterrupt(2), Meter1ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(3), Meter2ISR, RISING);
  Meter1.reset();
  Meter2.reset();

}


void loop(){

/////////////////////////////////////ultrasonic///////////////////////////////////////////////////////

  for(int i=0;i<10;i++)      
  { 
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    bufUS[i]= float(duration*0.034/2);
    delay(200);
  }
  for(int i=0;i<9;i++)        
  {
    for(int j=i+1;j<10;j++)
    {
      if(bufUS[i]>bufUS[j])
      {
        tempUS=bufUS[i];
        bufUS[i]=bufUS[j];
        bufUS[j]=tempUS;
      }
    }
  }
  avgValueUS=0;
  for(int i=0;i<9;i++)
  {                     
    avgValueUS+=bufUS[i];
  }
  float distance =(float)avgValueUS/9; 
  Serial.println("Distance = "+ String(distance));
  
/////////////////////////////////////flow///////////////////////////////////////////////////////////////
  delay(period);
  Meter1.tick(period); 
  Meter2.tick(period);
  float m1=Meter1.getCurrentFlowrate();
  float m2=Meter2.getCurrentFlowrate();
  float r1=Meter1.getTotalVolume();
  float r2=Meter2.getTotalVolume();

  if (r1>1000)
  {
    r1 = 0;
  }
  if(r2>1000)
  {
    r2 = 0;
  }

  Serial.println("flow rate1 = "+ String(m1));
  Serial.println("flow rate2 = "+ String(m2));
  Serial.println("flow volume1 = "+ String(r1));
  Serial.println("flow volume2 = "+ String(r2));

///////////////////////////////////////////////temeprature/////////////////////////////////////////////////////////

  sensors.requestTemperatures();
  Celcius=sensors.getTempCByIndex(0);
  Serial.println("Temperature "+ String(Celcius));


  //////////////////////////////////final string/////////////////////////////////////////////////////

  str = String(distance)+ ","+ String(Celcius)+ ","+ String(m1)+ ","+ String(m2)+ ","+ String(r1)+ ","+ String(r2)+",";
  Serial.println(str);

 ////////////////////////////////////////////////////////////Valve////////////////////////////////////////////
 
 if(valve=='1' && flag == 1)
 { 
  digitalWrite(ValveCW,0);
  digitalWrite(ValveCCW,0);
  delay(6000);
  digitalWrite(ValveCW,1);
  digitalWrite(ValveCCW,1);
  flag=0;
  Serial.println("open");
 }
 else if(valve=='0' && flag == 0)
 {
  digitalWrite(ValveCW,0);
  delay(6000);
  digitalWrite(ValveCW,1);
  flag = 1;
  Serial.println("close");
 }
   
delay(1000);


}
void Meter1ISR() {
Meter1.count();
}

void Meter2ISR() {

Meter2.count();
}
void receiveEvent(int howmany){
  howmany=1;
  while (Wire.available()>0) {
     valve = Wire.read();
    Serial.print(valve);
  }
}
void sendEvent(int howmany){
  for(int i=0;i<str.length();i++)
   {
     Wire.write(str[i]);
   } 
}
