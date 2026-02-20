#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ---------- LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- IR Sensors ----------
#define IR_LEFT  A0
#define IR_RIGHT A1

// ---------- Ultrasonic ----------
#define TRIG_PIN 11
#define ECHO_PIN 10

// ---------- Servo + Buzzer ----------
#define SERVO_PIN 4
#define BUZZER_PIN 2
#define SERVO_STOP     90
#define SERVO_FORWARD  105
#define SERVO_BACKWARD 75

Servo scanServo;

// ---------- Motors ----------
#define LM1 6
#define LM2 9
#define RM1 5
#define RM2 3

// ---------- Target Board ----------
#define TARGET_PIN 7

volatile bool hitDetected = false;

// ---------- Bed Logic ----------
int bedNumber = 0;
unsigned long hitTime = 0;
const unsigned long HIT_TIMEOUT = 5000;
bool deliverydone = false;

// ---------- States ----------
enum State { WAITING, COUNTDOWN, DELIVERING };
State state = WAITING;

int obstacleDistance = 10;

// ---------- SETUP ----------
void setup() {

  Serial.begin(9600);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TARGET_PIN, INPUT_PULLUP);

  scanServo.attach(SERVO_PIN);
  scanServo.write(SERVO_STOP);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Waiting for hit");

  // Enable interrupt
  PCICR |= B00000100;
  PCMSK2 |= B00010000;

  stopMotors();
}

// ---------- LOOP ----------
void loop() {

if (hitDetected) {

  hitDetected = false;

  bedNumber++;
  if (bedNumber > 2) bedNumber = 1;   // cycle 1 → 2 → 1 → 2

  lcd.clear();
  lcd.print("Bed ");
  lcd.print(bedNumber);

  hitTime = millis();
  state = COUNTDOWN;
}

  if (state == COUNTDOWN && millis() - hitTime >= HIT_TIMEOUT) {

    state = DELIVERING;

    lcd.clear();
    lcd.print("Food delivering");
    lcd.setCursor(0,1);
    lcd.print("to Bed ");
    lcd.print(bedNumber);
  }

  if (state == DELIVERING) {
    lineFollowerWithObstacle();
  }
}

// ---------- INTERRUPT ----------
ISR(PCINT2_vect) {
  if (!digitalRead(TARGET_PIN)) {
    hitDetected = true;
  }
}

// ---------- LINE FOLLOWER ----------
void lineFollowerWithObstacle() {

  int leftIR  = digitalRead(IR_LEFT);
  int rightIR = digitalRead(IR_RIGHT);
  int distance = getDistance();

  Serial.println(distance);

  if (distance > 0 && distance <= obstacleDistance) {

    stopMotors();
  tone(BUZZER_PIN,200);
      delay(200);
      noTone(BUZZER_PIN);
    unsigned long start = millis();
    while (millis() - start < 5000) {
      if (getDistance() > obstacleDistance) {
        return;
      }
    }

    // Deliver
    servoBackFront();
    rotate360();

    if (bedNumber == 1) {
      moveForward();
      delay(1000);
      turnRight();
      delay(1000);
    }
    else {
      moveForward();
      delay(1000);
      turnLeft();
      delay(1000);
    }

    deliverydone = true;
  }

  if (!deliverydone) {

    if (leftIR == LOW && rightIR == LOW) {
      moveForward();
    }
    else if (leftIR == HIGH && rightIR == HIGH) {
      if (bedNumber == 1)
        turnLeft();
      else
        turnRight();
    }
    else if (leftIR == LOW && rightIR == HIGH) {
      turnLeft();
    }
    else if (leftIR == HIGH && rightIR == LOW) {
      turnRight();
    }
  }
  else {

    if (leftIR == LOW && rightIR == LOW) {
      moveForward();
    }
    else if (leftIR == HIGH && rightIR == HIGH) {
      stopMotors();
    }
    else if (leftIR == LOW && rightIR == HIGH) {
      turnLeft();
    }
    else if (leftIR == HIGH && rightIR == LOW) {
      turnRight();
    }
  }
}

// ---------- ULTRASONIC ----------
int getDistance() {

  long pulseDuration;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  pulseDuration = pulseIn(ECHO_PIN, HIGH, 25000);

  if (pulseDuration == 0) return -1;

  return pulseDuration / 58;
}

// ---------- SERVO ----------
void servoBackFront() {

  tone(BUZZER_PIN, 200);

  scanServo.write(SERVO_BACKWARD);
  delay(3000);

  scanServo.write(SERVO_STOP);
  delay(5000);

  scanServo.write(SERVO_FORWARD);
  delay(3000);

  scanServo.write(SERVO_STOP);

  noTone(BUZZER_PIN);
}
// ---------- MOTOR CONTROL ----------
void moveForward() {
  analogWrite(LM1,115); analogWrite(LM2,0);
  analogWrite(RM1,130); analogWrite(RM2,0);
}

void turnLeft() {
  analogWrite(LM1,0);   analogWrite(LM2,0);
  analogWrite(RM1,130); analogWrite(RM2,0);
}

void turnRight() {
  analogWrite(LM1,130); analogWrite(LM2,0);
  analogWrite(RM1,0);   analogWrite(RM2,0);
}

void stopMotors() {
  analogWrite(LM1,0); analogWrite(LM2,0);
  analogWrite(RM1,0); analogWrite(RM2,0);
}

void rotate360() {
   analogWrite(LM1,250); analogWrite(LM2,0);
  analogWrite(RM2,250); analogWrite(RM1,0);
  delay(600);
  stopMotors();
  delay(1000);

} 
