#include "Adafruit_VL53L1X.h"
#include <Servo.h>
#include <arduino-timer.h>

#define IRQ_PIN 1
#define XSHUT_PIN 2
#define TURN_SPEED 90 // degrees per sec
#define VERT_SPEED 90 
#define DETECTION_MIN 30
#define DETECTION_MAX 150
#define ARM_MIN_ANGLE 40
#define ARM_MAX_ANGLE 140
#define HEAD_MAX_Y 190
#define HEAD_MIN_Y 110
#define HEAD_MAX_TURN 180
#define HEAD_MIN_TURN 20

auto timer = timer_create_default();
Adafruit_VL53L1X sensor = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);
Servo arms[8];
Servo headUpServo;
Servo headTurnServo;
Servo sensorServo;

int headY = 80;
bool headUp = true;

int goalHeadAngle = 0;
int currHeadAngle = 0;
int prevGoal = 0;

bool headTurnIncreasing = true;

int armsAngle = 90;
int secondArmsAngle = 120;
bool secondArmsIncreasing;
bool armsIncreasing = true;

int sensorAngle = DETECTION_MIN;
bool sensorIncreasing = true;

int numDetections = 0;
int detections[120];

void headMotion()  {
  int yAmount =  5;
  if (!headUp){
    Serial.println("head down");
    if (headY > HEAD_MIN_Y){
      headY -= yAmount;
      Serial.println("going up");
      headUpServo.write(headY);
      Serial.println(headUpServo.read());
    }
  }
  else {
    Serial.println("head up");
    if (headY < HEAD_MAX_Y){
      headY += yAmount;
      headUpServo.write(headY);
      Serial.println("going dow");
      Serial.println(headUpServo.read());
    }
  }

  if (headUp){
    int turnAmount = 8;
    if (headTurnIncreasing){
      if (currHeadAngle < HEAD_MAX_TURN){
        currHeadAngle += turnAmount;
      }
      else {
        headTurnIncreasing = false;
        currHeadAngle -= turnAmount;
      }
    }
    else {
      if (currHeadAngle > HEAD_MIN_TURN) {
        currHeadAngle -= turnAmount;
      }
      else {
        headTurnIncreasing = true;
        currHeadAngle += turnAmount;
      }
    }
    headTurnServo.write(currHeadAngle);
    Serial.print(".");
  }
  else {
    int turnAmount = 4;
    if (abs(currHeadAngle - goalHeadAngle) / turnAmount > 1){
      if (currHeadAngle > goalHeadAngle){
        Serial.println("going left");
        currHeadAngle -=  turnAmount;
        headTurnServo.write(180 - currHeadAngle);
        Serial.println(headTurnServo.read());
      }
      else {
        currHeadAngle +=  turnAmount;
        Serial.println("going right");
        headTurnServo.write(180 - currHeadAngle);
        Serial.println(headTurnServo.read());
      }
    }
    else {
      Serial.println("ehhh");
    }
  }
}

void armMotion() {
  Serial.println("HEADUP");
  Serial.println(headUp);
  if (headUp){
    for (int i = 0; i < 8; i += 1){
      //arms[i].write(90);
      if (i == 4){
        arms[i].write(ARM_MAX_ANGLE - armsAngle + ARM_MIN_ANGLE); 
      }
      else if (i > 4){
        arms[i].write(secondArmsAngle);
        Serial.println(arms[i].read());
      }
      else{
        arms[i].write(armsAngle);
        Serial.println(arms[i].read());
      }
      
    }
    int x = 5;

    if (armsIncreasing){
      if (armsAngle < ARM_MAX_ANGLE){
        armsAngle += x;
      }
      else {
        armsIncreasing = false;
        armsAngle -= x;
      }
    }
    else {
      if (armsAngle > ARM_MIN_ANGLE) {
        armsAngle -= x;
      }
      else {
        armsIncreasing = true;
        armsAngle += x;
      }
    }
    if (secondArmsIncreasing){
      if (secondArmsAngle < ARM_MAX_ANGLE){
        secondArmsAngle += x;
      }
      else {
        secondArmsIncreasing = false;
        secondArmsAngle -= x;
      }
    }
    else {
      if (secondArmsAngle > ARM_MIN_ANGLE) {
        secondArmsAngle -= x;
      }
      else {
        secondArmsIncreasing = true;
        secondArmsAngle += x;
      }
    }
  }
  else {
    Serial.println("not  running");
  }
}

void sensing() {
  if (sensorIncreasing){
    sensorAngle += 10;
  }
  else {
    sensorAngle -= 10;
  }
  //Serial.println(sensorIncreasing);

  sensorServo.write(sensorAngle);
 // Serial.println(sensorServo.read());
  
  if (sensor.dataReady()) {
    // new measurement for the taking!
    int distance = sensor.distance();
    if (distance == -1) {
      // something went wrong!
      Serial.print("FAR");
      // Serial.println(vl53.vl_status);
      //return;
    }
    else {

      //Serial.println(numDetections);
      if (distance < 1000) {
        Serial.print(F("Distance: "));
        Serial.print(distance);
        Serial.println(" mm");
        // personPos = (pos * 2 + personPos) / 3;
        detections[numDetections] = sensorAngle;
        numDetections += 1;

      }
    }

    // data is read out, time for another reading!
  }
  else {
    Serial.println("FAILURE");
    Serial.println(sensorAngle);
    Serial.println("----");
  }
  sensor.clearInterrupt();

  if (sensorAngle == DETECTION_MIN  || sensorAngle == DETECTION_MAX){
    Serial.println(numDetections);
    if (numDetections > 1){
      headUp = false;
      int sum = 0;
      for (int i = 0; i < (DETECTION_MAX - DETECTION_MIN); i += 1){
        sum += detections[i];
      }
      if (abs(sum / numDetections - prevGoal)  > 15){
        prevGoal = goalHeadAngle;
        goalHeadAngle = sum / numDetections;
      
      }
    }
    else {
      headUp = true;
    }
    if (sensorIncreasing){
      sensorIncreasing = false;
    }
    else {
      sensorIncreasing = true;
    }
    numDetections = 0;
    for (int i = 0; i < (DETECTION_MAX - DETECTION_MIN);  i += 1){
      detections[i] = 0;
    }
  }
}


void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);


  headUpServo.attach(11);
  headTurnServo.attach(12);
  sensorServo.attach(13);
  Serial.println(sensorServo.read());
  sensorServo.write(40);
  Serial.println(sensorServo.read());
  for (int i = 0; i < 8; i += 1){
    arms[i].attach(i+2);
  }

  Wire.begin();
  Serial.println("beginning");
  if (! sensor.begin(0x29, &Wire)) {
    Serial.print("Error on init of VL sensor: ");
    Serial.println(sensor.vl_status);
    while (1)       delay(10);
  }
  Serial.println(("VL53L1X sensor OK!"));

  if (! sensor.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(sensor.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("Ranging started"));
  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  sensor.setTimingBudget(33);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(sensor.getTimingBudget());

  Serial.println("HI");

  timer.every(90, sensing);
  timer.every(130, headMotion);
  timer.every(70, armMotion);
  Serial.println("running");
}

void loop() {
  timer.tick();
}
