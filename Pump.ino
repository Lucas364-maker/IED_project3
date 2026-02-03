
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
int pos=0;// ---------- IR Sensor Pins ----------
#define IR_LEFT_Pin   A1
#define IR_RIGHT_Pin  A0

// ---------- Ultrasonic Pins ----------
#define TrigPin1 11
#define EchoPin1 10

// ---------- Motor Pins ----------
#define motor_board_input_pin_IN1 9   // Motor A PWM
#define motor_board_input_pin_IN2 6   // Motor A DIR
#define motor_board_input_pin_IN4 5   // Motor B PWM
#define motor_board_input_pin_IN3 3   // Motor B DIR

// ---------- EXTRA OUTPUTS (leave pins blank) ----------
#define BUZZER_PIN  2   // intentionally left blank
#define SERVO_PIN    4  // intentionally left blank

// ---------- Constants ----------
#define OBSTACLE_DISTANCE 40   // cm

long pulseDuration;
int Distance;
int ultraState;   // 1 = obstacle, 0 = no obstacle
bool missionStarted = false;

void setup() {
  Serial.begin(9600);

  pinMode(IR_LEFT_Pin, INPUT);
  pinMode(IR_RIGHT_Pin, INPUT);
pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TrigPin1, OUTPUT);
  pinMode(EchoPin1, INPUT);

  pinMode(motor_board_input_pin_IN1, OUTPUT);
  pinMode(motor_board_input_pin_IN2, OUTPUT);
  pinMode(motor_board_input_pin_IN3, OUTPUT);
  pinMode(motor_board_input_pin_IN4, OUTPUT);

  lcd.init();
lcd.backlight();
lcd.clear();
lcd.print("READY FOR YOUR");
lcd.setCursor(0,1);
lcd.print("MEAL");

  Brake();
  Serial.println("Line Follower Started");
}

void loop() {
if (!missionStarted) {
     lcdReady();

    int ultraState = readUltrasonicDigital();
    if (ultraState == 1) {
      missionStarted = true;   // LATCH
      Serial.println("TARGET HIT → DELIVERY STARTED");
    }

    Brake();
    return;   // do nothing else until hit
  }
lcdDelivering();
lineFollowerLogic();
decision();


}


void lcdReady() {
  static bool shown = false;
  if (!shown) {
    lcd.clear();
    lcd.print("READY FOR YOUR");
    lcd.setCursor(0,1);
    lcd.print("MEAL");
    shown = true;
  }
}

void lcdDelivering() {
  static bool shown = false;
  if (!shown) {
    lcd.clear();
    lcd.print("DELIVERING");
    shown = true;
  }
}


void Forward() {
  analogWrite(motor_board_input_pin_IN3, 90);
  analogWrite(motor_board_input_pin_IN1, 90);
  analogWrite(motor_board_input_pin_IN2, 0);
  analogWrite(motor_board_input_pin_IN4, 0);
}

// Turn Right
void Turn_right() {
  analogWrite(motor_board_input_pin_IN3, 0);
  analogWrite(motor_board_input_pin_IN1, 90);
  analogWrite(motor_board_input_pin_IN2, 0);
  analogWrite(motor_board_input_pin_IN4, 0);
}

// Turn Left
void Turn_left() {
  analogWrite(motor_board_input_pin_IN3, 90);
  analogWrite(motor_board_input_pin_IN1, 0);
  analogWrite(motor_board_input_pin_IN2, 0);
  analogWrite(motor_board_input_pin_IN4, 0);
}
void Brake() {
  analogWrite(motor_board_input_pin_IN1, 0);
  analogWrite(motor_board_input_pin_IN4, 0);
  analogWrite(motor_board_input_pin_IN2, 0);
  analogWrite(motor_board_input_pin_IN3, 0);
}
// Stop
void Brake1() {
  analogWrite(motor_board_input_pin_IN1, 0);
  analogWrite(motor_board_input_pin_IN4, 0);
  analogWrite(motor_board_input_pin_IN2, 0);
  analogWrite(motor_board_input_pin_IN3, 0);
       for (int i = 0; i < 2; i++) {
    myservo.write(0);   // forward rotation
    delay(1000); 

    
    
    myservo.write(90);   // forward rotation
    delay(1000); 
         // small pause between rotations
  }

  delay(5000);

for (int i = 0; i < 2; i++) {
    myservo.write(270);   // forward rotation
    delay(1000); 

    
    
    myservo.write(90);   // forward rotation
    delay(1000); 
         // small pause between rotations
  }
  delay(5000);
}



// ---------- ULTRASONIC DIGITAL FUNCTION ----------
void lineFollowerLogic() {
  int irLeft  = digitalRead(IR_LEFT_Pin);
  int irRight = digitalRead(IR_RIGHT_Pin);

  Serial.print("L:");
  Serial.print(irLeft == LOW ? "BLACK" : "WHITE");
  Serial.print(" R:");
  Serial.print(irRight == LOW ? "BLACK" : "WHITE");
  Serial.print(" -> ");

  

  // ---- LINE FOLLOWING ----
  if (irLeft == LOW && irRight == LOW) {
    Serial.println("Forward");
    Forward();
  }
  else if (irLeft == LOW && irRight == HIGH) {
    Serial.println("Turn Right");
    Turn_right();
  }
  else if (irLeft == HIGH && irRight == LOW) {
    Serial.println("Turn Left");
    Turn_left();
  }
  else {
    Serial.println("servo");
    Brake1();


}
  delay(1);
}
int readUltrasonicDigital() {
  digitalWrite(TrigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin1, LOW);

  pulseDuration = pulseIn(EchoPin1, HIGH, 30000);

  if (pulseDuration == 0) {
    Serial.println("Ultrasonic: No Echo");
    return 0;
  }

  Distance = pulseDuration / 58;

  Serial.print("Ultrasonic Distance: ");
  Serial.print(Distance);
  Serial.println(" cm");

  if (Distance > 0 && Distance <= 50) {
    Serial.println("Ultrasonic State: OBSTACLE");
    return 1;
  } else {
    Serial.println("Ultrasonic State: CLEAR");
    return 0;
  }
}

// ---------- LINE FOLLOWER + ACTION LOGIC ----------
void decision() {
  int irLeft  = digitalRead(IR_LEFT_Pin);
  int irRight = digitalRead(IR_RIGHT_Pin);
  int ultraState = readUltrasonicDigital();

  Serial.print("IR Left: ");
  Serial.print(irLeft == LOW ? "BLACK" : "WHITE");
  Serial.print(" | IR Right: ");
  Serial.println(irRight == LOW ? "BLACK" : "WHITE");

  // -------- BUZZER CONDITION --------
  if (ultraState == 1 && (irLeft == LOW || irRight == LOW)) {
    Serial.println("ACTION: BUZZER ON (Obstacle + Line detected)");
    Brake();
    tone(BUZZER_PIN, 200);
    delay(1000);
    noTone(BUZZER_PIN);
  }

  // -------- SERVO CONDITION --------
}
