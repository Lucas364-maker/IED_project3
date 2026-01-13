// ---------- Ultrasonic Pins ----------
#define TrigPin1 11
#define EchoPin1 10

// ---------- Motor Pins ----------
#define motor_board_input_pin_IN1 9
#define motor_board_input_pin_IN2 6
#define motor_board_input_pin_IN3 5
#define motor_board_input_pin_IN4 3

#define OBSTACLE_DISTANCE 20   // cm

long pulseDuration;
int Distance;

void setup() {
  Serial.begin(9600);

  // Ultrasonic
  pinMode(TrigPin1, OUTPUT);
  pinMode(EchoPin1, INPUT);

  // Motors
  pinMode(motor_board_input_pin_IN1, OUTPUT);
  pinMode(motor_board_input_pin_IN2, OUTPUT);
  pinMode(motor_board_input_pin_IN3, OUTPUT);
  pinMode(motor_board_input_pin_IN4, OUTPUT);
}

void loop() {
  Distance = readUltrasonic();

  Serial.print("Distance = ");
  Serial.print(Distance);
  Serial.println(" cm");

  if (Distance <= OBSTACLE_DISTANCE && Distance > 0) {
    brake();
  } else {
    Full_speed_forward();
  }

  delay(100);
}

// ---------- Ultrasonic Function ----------
int readUltrasonic() {
  digitalWrite(TrigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin1, LOW);

  pulseDuration = pulseIn(EchoPin1, HIGH, 30000); // timeout 30ms
  return pulseDuration / 58;
}

// ---------- Motor Functions ----------
void Full_speed_forward() {
  digitalWrite(motor_board_input_pin_IN1, HIGH);
  digitalWrite(motor_board_input_pin_IN2, LOW);
  digitalWrite(motor_board_input_pin_IN3, HIGH);
  digitalWrite(motor_board_input_pin_IN4, LOW);
}

void brake() {
  digitalWrite(motor_board_input_pin_IN1, HIGH);
  digitalWrite(motor_board_input_pin_IN2, HIGH);
  digitalWrite(motor_board_input_pin_IN3, HIGH);
  digitalWrite(motor_board_input_pin_IN4, HIGH);
}

