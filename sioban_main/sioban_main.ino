#include <Servo.h>

int pos = 0;
int dis = 100;
Servo myservo;

int trigL = 11;
int echoL = 12;  
int trigM = 9;  
int echoM = 10;  
int trigR = 7;  
int echoR = 8;  
long durationM, durationL, durationR;

void setup() {
  myservo.attach(6);
  //Serial Port begin
  Serial.begin(9600);
  //Define inputs and outputs
  pinMode(trigM, OUTPUT);
  pinMode(echoM, INPUT);
  pinMode(trigL, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(trigR, OUTPUT);
  pinMode(echoR, INPUT);
  myservo.write(0);
}


void loop() {
  digitalWrite(trigM, LOW);
  delayMicroseconds(5);
  digitalWrite(trigM, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigM, LOW);

  pinMode(echoM, INPUT);
  durationM = pulseIn(echoM, HIGH);
  delayMicroseconds(20);

  digitalWrite(trigL, LOW);
  delayMicroseconds(5);
  digitalWrite(trigL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigL, LOW);

  pinMode(echoL, INPUT);
  durationL = pulseIn(echoL, HIGH);

  delayMicroseconds(20);

  digitalWrite(trigR, LOW);
  delayMicroseconds(5);
  digitalWrite(trigR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigR, LOW);

  pinMode(echoR, INPUT);
  durationR = pulseIn(echoR, HIGH);

  delayMicroseconds(20);
  
  bool l = (durationL / 2) / 29.1  < dis;
  bool m = (durationM / 2) / 29.1 < dis;
  bool r = (durationR / 2) / 29.1  < dis;

  if (l && m){
    pos = 60;
    Serial.println("L M");
  }
  else if (m && r){
    pos = 120;
    Serial.println("M R");
  }
  else if (l){
    pos = 30;
    Serial.println("L");
  }
  else if (m){
    pos =  90;
    Serial.println("M");
  }
  else if (r){
    pos = 150;
    Serial.println("R");
    //Serial.println((durationR / 2) / 29.1);
  }
  myservo.write(pos);

  delay(250);
}