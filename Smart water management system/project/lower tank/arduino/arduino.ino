#include <Wire.h>
#include <FlowMeter.h> 
#define PHpin A0
#define TurbidityPin A1
#define trigPin 12
#define echoPin 13
#define CW 6

unsigned int avgValueUS; 
int bufUS[10],tempUS;
unsigned long duration;
 
FlowMeter Meter1 = FlowMeter(2);
FlowMeter Meter2 = FlowMeter(3);
uint8_t period = 1000;
char c;

unsigned int avgValuePH; 
int bufPH[10],tempPH;

unsigned int avgValue; 
int buf[10],temp;

String str;

void setup(){
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendEvent);
  
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  
  pinMode(CW, OUTPUT);
  digitalWrite(CW,HIGH);
 
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

///////////////////////////////////////////////PH/////////////////////////////////////////////////////////

   for(int i=0;i<10;i++)      
  { 
    bufPH[i]=analogRead(PHpin);
    delay(100);
  }
  for(int i=0;i<9;i++)        
  {
    for(int j=i+1;j<10;j++)
    {
      if(bufPH[i]>bufPH[j])
      {
        tempPH=bufPH[i];
        bufPH[i]=bufPH[j];
        bufPH[j]=tempPH;
      }
    }
  }
  avgValuePH=0;
  for(int i=2;i<8;i++)
  {
     avgValuePH+=bufPH[i];                     
  }
 
  float PHVolt=(float)avgValuePH*5.0/1024/6; 
  float PHValue = 14 - (-5.75 * PHVolt + 18.34);
  Serial.println("ph volt "+ String(PHVolt));
  Serial.println("ph value"+ String(PHValue));

//////////////////////////////////////////////////////turbidity////////////////////////////////////////

  for(int i=0;i<10;i++)      
  { 
    buf[i]=analogRead(TurbidityPin);
    delay(100);
  }
  for(int i=0;i<9;i++)        
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)
  {
     avgValue+=buf[i];                     
  }
 
  float TurbidityVolt=(float)avgValue*5.0/1024/6;
 float TurbidityValue = map(TurbidityVolt,3.76,0.03,0,3000);
  Serial.println("turbidity volt "+ String(TurbidityVolt));
  Serial.println("turbidity value"+ String(TurbidityValue));

  //////////////////////////////////final string/////////////////////////////////////////////////////

  str = String(distance)+ ","+ String(TurbidityValue)+ ","+ String(PHValue)+ ","+ String(m1)+ ","+ String(m2)+ ","+ String(r1)+ ","+ String(r2)+",";
  Serial.println(str);


  ////////////////////////////////////////////////////////////motor////////////////////////////////////////////
 
  if(c=='0')
  {
    digitalWrite(CW,HIGH);
    Serial.println(c); 
  }
  else if(c=='1')
  {
    digitalWrite(CW, LOW);
    Serial.println(c); 
  }
   
delay(1000);


}
void Meter1ISR() {
Meter1.count();
}

void Meter2ISR() {
Meter2.count();
}
void receiveEvent(int howMany){
  while (Wire.available()>0) {
     c = Wire.read();
    // Serial.print(c);
  }
}
void sendEvent(int howmany){
  for(int i=0;i<str.length();i++)
   {
     Wire.write(str[i]);

   }  
}
