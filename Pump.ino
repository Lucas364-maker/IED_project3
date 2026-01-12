const int trigPin = 2;
const int echoPin = 3;
const int ledPin = 4;

long duration;
int distance;
int limitDistance = 5; // cm, change if needed

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  double averageD = 0;
  int num = 5;  // number of samples

  for (int i = 0; i < num; i++) {
    // Send ultrasonic pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read echo time
    duration = pulseIn(echoPin, HIGH);

    // Convert to cm and add to total
    distance = duration * 0.034 / 2;
    averageD += distance;

    delay(10); // small delay between samples
  }

  // Calculate average
  averageD = averageD / num;

  Serial.print("Distance: ");
  Serial.print(averageD);
  Serial.println(" cm");

  // Control LED
  if (averageD <= limitDistance) {
    digitalWrite(ledPin, HIGH);  // LED ON
  } else {
    digitalWrite(ledPin, LOW);   // LED OFF
  }

  delay(100);
}
