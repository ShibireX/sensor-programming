#include <Servo.h>

Servo sg90;

const int bluePin = 11;
const int greenPin = 10;
const int redPin = 9;
const int trigPin = 6;
const int echoPin = 5;
const int buttonPin = 2;

const int redPressurePin = A2;
const int yellowPressurePin = A0;
const int greenPressurePin = A1;
const int timerMax = 30;

int redPressureValue;
int yellowPressureValue;
int greenPressureValue;
int timerSeconds;
float timerAngle;

unsigned long lastInteractionTime = 0;
long duration;
int distance;

struct SequenceStep {
  int color[3];
  unsigned long duration;
};

SequenceStep sequenceSteps[10];
int sequenceLength = 0;

bool buttonPressed = false;

// Data to be sent
String colorString = "";

// Data to be received
char receivedDarkValue;

void setup() {
  pinMode(bluePin, INPUT);
  pinMode(greenPin, INPUT);
  pinMode(redPin, INPUT);

  pinMode(redPressurePin, INPUT);
  pinMode(yellowPressurePin, INPUT);
  pinMode(greenPressurePin, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  sg90.attach(3);

  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {

  // Check for pressure plate interactions
  redPressureValue = analogRead(redPressurePin);
  yellowPressureValue = analogRead(yellowPressurePin);
  greenPressureValue = analogRead(greenPressurePin);

  unsigned long redDuration = 0;
  unsigned long yellowDuration = 0;
  unsigned long greenDuration = 0;

  if (redPressureValue > 50) {
    redDuration = getSensorDuration(redPressurePin);
    addSequenceStep(0, 255, 255, redDuration);
    colorString += "2";
  }

  if (yellowPressureValue > 50) {
    yellowDuration = getSensorDuration(yellowPressurePin);
    addSequenceStep(0, 255, 0, yellowDuration);
    colorString += "1";
  }

  if (greenPressureValue > 50) {
    greenDuration = getSensorDuration(greenPressurePin);
    addSequenceStep(255, 255, 0, greenDuration);
    colorString += "0";
  }

  // Distance calculation ULTRASOUND
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;

  if (digitalRead(buttonPin) == LOW && sequenceLength > 0) {
    Serial.print(colorString);
    Serial.print(",");
    Serial.println(distance);
    // Color Sequence    
    playSequence();

    colorString = "";
    buttonPressed = true;

  }

  if (Serial.available()) {
    receivedDarkValue = Serial.read(); // from photoresistive sensor
  }


  if (buttonPressed == true && receivedDarkValue == '1') {
    timerSeconds = distance;
    timerAngle = 180 / (timerMax / timerSeconds);

    sg90.write(timerAngle);  // rotates the servo depending on chosen seconds
    delay(1000);  // wait for 1 seconds
    
    for (int i = timerAngle-(timerAngle/timerSeconds); i >= 0; i -= (timerAngle/timerSeconds)) {
      sg90.write(i);  // rotates the servo back to 0 degrees in i-degree increments
      delay(1000);  // wait for 1 second between each increment
    }

    buttonPressed = false;
    int motorStopped = 1;
    Serial.println(motorStopped);

  }


}

void addSequenceStep(int redValue, int greenValue, int blueValue, unsigned long duration) {
  if (sequenceLength >= 10) {
    return;
  }

  int color[3] = {redValue, greenValue, blueValue};

  sequenceSteps[sequenceLength].color[0] = color[0];
  sequenceSteps[sequenceLength].color[1] = color[1];
  sequenceSteps[sequenceLength].color[2] = color[2];
  sequenceSteps[sequenceLength].duration = duration;

  sequenceLength++;

  /*Serial.print("Added sequence step: ");
  Serial.print(color[0]);
  Serial.print(",");
  Serial.print(color[1]);
  Serial.print(",");
  Serial.println(color[2]);*/
}

void playSequence() {
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  
  for (int i = 0; i < sequenceLength; i++) {
    analogWrite(redPin, sequenceSteps[i].color[0]);
    analogWrite(greenPin, sequenceSteps[i].color[1]);
    analogWrite(bluePin, sequenceSteps[i].color[2]);

    delay(sequenceSteps[i].duration);
  }

  pinMode(bluePin, INPUT);
  pinMode(greenPin, INPUT);
  pinMode(redPin, INPUT);

  sequenceLength = 0;
  lastInteractionTime = millis();
}

unsigned long getSensorDuration(int sensorPin) {
  unsigned long startTime = millis();

  while (analogRead(sensorPin) > 50) {
    // Wait for sensor to release
  }

  return millis() - startTime;
}