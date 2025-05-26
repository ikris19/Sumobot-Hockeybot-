#define IN1 7
#define IN2 8
#define ENA 9

#define IN3 11
#define IN4 12
#define ENB 10

#define MODE_SWITCH 4  // D4 switch between sumo and Bluetooth
#define TRIG_PIN 2
#define ECHO_PIN 3
#define IR_SENSOR A0

char command = 'B';  // Default to Stop
bool isMovingForward = false; // Track if forward is pressed
bool wasControlledMode = false; // Track the previous mode

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(MODE_SWITCH, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_SENSOR, INPUT);

  Serial.begin(9600);
  delay(5000); // Autonomous mode start delay
}

void loop() {
  int mode = digitalRead(MODE_SWITCH);

  if (mode == HIGH) {
    wasControlledMode = true; // Track that we were in controlled mode
    controlledMode();
  } else {
    if (wasControlledMode) {
      delay(5000); // Delay when transitioning to autonomous mode
      wasControlledMode = false; // Reset the flag
    }
    autonomousMode();
  }
}

// Controlled Mode Functions
void controlledMode() {
  if (Serial.available()) {
    command = Serial.read();  // Read single character
  }

  switch (command) {
    case 'W': moveForward(); break;      // Start moving forward
    case 'G': if (isMovingForward) boostForward(); break; // Boost only if moving forward
    case 'S': moveBackward(); break;
    case 'A': turnLeftGradual(); break;
    case 'D': turnRightGradual(); break;
    case 'L': spinLeft(); break;
    case 'R': spinRight(); break;
    case 'B': stopMotors(); break;
    default: stopMotors(); break;
  }
  delay(30);
}

// Autonomous Mode Functions
void autonomousMode() {
  int distance = readUltrasonic();
  int irValue = digitalRead(IR_SENSOR);

  if (irValue == LOW) {
    reverse();
    delay(400);
  } else if (distance > 0 && distance <= 10) {
    closeAttack();
  } else if (distance > 6 && distance <= 20) {
    attack();
  } else {
    search(); // Nothing detected = start searching
  }
  delay(10); // small delay for fast loop cycle
}

// Shared Motor Control Functions
void moveForward() {
  isMovingForward = true; 
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 150);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 150);
}

void boostForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 255);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 255);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 150);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 150);
}

void turnLeftGradual() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 150);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 80);
}

void turnRightGradual() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 80);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 150);
}

void spinLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 120);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 120);
}

void spinRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 120);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 120);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// Autonomous Specific Functions
void attack() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 255);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 255);

  monitorIRDuring(1000);
}

void closeAttack() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 100);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 100);

  monitorIRDuring(1000);
}

void reverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 180);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 120);
}

void search() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 90);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 30);
}

int readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  return duration * 0.034 / 2;
}

void monitorIRDuring(unsigned long duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    if (digitalRead(IR_SENSOR) == LOW) {
      reverse();
      delay(400);
      break;
    }
    delay(1);
  }
}
