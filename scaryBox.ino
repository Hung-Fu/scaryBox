#include<Servo.h>
#include <Ultrasonic.h>

//===== servo variable =======
Servo myServo1;
const int SERVO1_PIN = 9;

int angleCurrent = 5;
const int ANGLE_MIN = 5;
const int ANGLE_MAX = 100;
int angleMoveSpeed = 0;
int repeatCountDown = 0;
const int DEFAULT_REPEAT_PER_TRIGGER = 1;
enum MoveSpeed{
  Speed_1,
  Speed_5,
};

long sleepDelay = 0;
//const long SLEEP_SEED = 100000;  // around 1 second
long sleepDelayCounter = 0;
boolean servoIsSleeping = false;
enum SleepTime{
  Second_0,
  Second_1,
  Second_5,
};
// ==========================

// ======== ULTRASOUND (HC-SR04) variable ===
const int ULTRASOUND_TRIG_PIN = 12;
const int ULTRASOUND_ECHO_PIN = 13;
long duration;
long distance;
Ultrasonic ultrasonic(ULTRASOUND_TRIG_PIN, ULTRASOUND_ECHO_PIN);
//===========================================

// ===== LED ========
const int LED_PIN = 8;
int ledDelayCounter = 0;
int ledState = 0;
//===================

void setup() {
  Serial.begin(9600);

  myServo1.attach(SERVO1_PIN);
  myServo1.write(angleCurrent);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  motionDetect();
  moveServo();
  lightLED();
}

void motionDetect() {
  if (repeatCountDown != 0) {
    return;
  }

  if (hasMotion()) {
    initAction();
    trigger(DEFAULT_REPEAT_PER_TRIGGER);
  }
}

void initAction(){
  angleMoveSpeed = getAngleMoveSpeed(getRandom(2));
  sleepDelay = getSleepDelay(getRandom(3));
}

boolean hasMotion() {
  long microsec = ultrasonic.timing();
  distance = ultrasonic.convert(microsec, Ultrasonic::CM);
  Serial.println("Distance: " + String(distance));

  if (distance < 20) {
    return true;
  }
  else {
    return false;
  }
}

void trigger(int triggerTime) {
  //  Serial.println("Servo is triggered " + String(triggerTime) + " times");
  repeatCountDown = triggerTime;
}

void moveServo() {
  //  Serial.println("get repeatCountDown " + String(repeatCountDown));

  if (repeatCountDown == 0) {
    return;
  }

  if (!servoIsSleeping) {
    changeAngle();
    sleepAtEnd();
    verifyRepeatCountDown();
  } else {
    sleeping();
  }
}

// servo reach the end then return back
void changeAngle() {

  angleCurrent = angleCurrent + angleMoveSpeed;

  if (angleCurrent == ANGLE_MIN || angleCurrent == ANGLE_MAX) {
    angleMoveSpeed = -angleMoveSpeed;
  }

  myServo1.write(angleCurrent);

  delay(5);
}

void sleepAtEnd() {

  if (angleCurrent == ANGLE_MAX) {
//    sleepDelay = random(SLEEP_SEED);
//    sleepDelay = SLEEP_SEED;

    Serial.println("sleep " + String(sleepDelay));

    servoIsSleeping = true;
  }
}

void sleeping() {
  //  Serial.println("servo is sleeping...");

  if (sleepDelayCounter < sleepDelay) {
    sleepDelayCounter++;
  } else {
    sleepDelayCounter = 0;
    servoIsSleeping = false;
  }
}

void verifyRepeatCountDown() {
  if (angleCurrent == ANGLE_MIN) {
    repeatCountDown = repeatCountDown - 1;
  }
}

void lightLED() {
  if (repeatCountDown != 0) {
    blinkGreen();
  } else {
    solidGreen();
  }
}

void solidGreen() {
//  Serial.println("LED: solid");
  
  digitalWrite(LED_PIN, HIGH);
}

void blinkGreen() {
//  Serial.println("LED: blink");
  
  if (ledDelayCounter < 10) {
    ledDelayCounter++;
    return;
  } else {
    ledDelayCounter = 0;
  }

  ledState++;
  if (ledState % 2 == 0) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

int getRandom(int seedNum){
  return random(seedNum);
}

int getAngleMoveSpeed(int randomNum){
  switch(randomNum){
    case Speed_1:
      return 1;
    case Speed_5:
      return 5;
    default:
      Serial.println("wrong angle speed");
      return 0;
  }
}

long getSleepDelay(int randomNum){
  switch(randomNum){
    case Second_0:
      return 0;
    case Second_1:
      return 100000;
    case Second_5:
      return 500000;
    default:
      Serial.println("wrong sleep time");
      return 0;
  }
}


