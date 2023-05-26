#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SparkFun_CAP1203.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
CAP1203 touchSensor;

const int buzzerPin = 6;
const int fotoPin = A0;
const int buttonPin = 12;

const int sequenceLength = 10;
int colorSequence[sequenceLength];
int trueSequence[10];
int colorIndex = 0;
int lightValue = 0;
int playerPoints = 0;
int gameRound = 0;
bool thisRound = false;
unsigned long previousTime = 0;
unsigned long interval = 0;

// Received values from Processing
String receivedColorString = "";
int readIndex = 0;
int thinkTime;
int motorStopped = 0;

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); // Address 0x3C for the I2C interface
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20, (SCREEN_HEIGHT/2)-8);      // Set the cursor position to centered X and Y
  display.println(playerPoints);
  display.display();

  touchSensor.begin();
  touchSensor.setSensitivity(SENSITIVITY_2X);

  pinMode(buzzerPin, OUTPUT);
  pinMode(fotoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop()
{

  lightValue = analogRead(fotoPin);


  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    if (receivedData.indexOf(',') == -1) {
      if (gameRound == 5) {
        delay(500);      
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(30, 20);
        display.println("Total");
        display.setCursor(30, 40);
        display.println(playerPoints);
        display.display();
        delay(5000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, (SCREEN_HEIGHT/2)-8);
        playerPoints = 0;
        display.println(playerPoints);
        gameRound = 0;
        display.display();
      }
      
      else {
        display.clearDisplay();
        display.setCursor(20, (SCREEN_HEIGHT/2)-8);
        display.println(playerPoints);
        display.display();
        if (thisRound == true) {
          motorStopped = receivedData.toInt();
        }
      }
      
    }
    else {
      int separatorIndex = receivedData.indexOf(',');

      String colorSequenceString = receivedData.substring(0, separatorIndex);
      String thinkTimeString = receivedData.substring(separatorIndex + 1);
      thinkTime = thinkTimeString.toInt();
      interval = thinkTime*1000;

      for (int i = 0; i < colorSequenceString.length(); i++) {
        trueSequence[readIndex] = colorSequenceString.charAt(i) - '0';
        readIndex++;
      }


      if (lightValue <= 250) {
        Serial.println("1");
      } else {
        Serial.println("0");
      }
      readIndex = 0;
      thisRound = true;
      motorStopped = 0;
    }
    


  }


  if (touchSensor.isLeftTouched() == true)
  {
    while(touchSensor.isLeftTouched() == true) {
      ;
    }
    colorSequence[colorIndex] = 0; // Green
    colorIndex++;
  }

  if (touchSensor.isMiddleTouched() == true)
  {
    while(touchSensor.isMiddleTouched() == true) {
      ;
    }
    colorSequence[colorIndex] = 1; // Yellow
    colorIndex++;
  }

  if (touchSensor.isRightTouched() == true)
  {
    while(touchSensor.isRightTouched() == true) {
      ;
    }
    colorSequence[colorIndex] = 2; // Red
    colorIndex++;
  }

  if (digitalRead(buttonPin) == LOW && colorIndex > 0) {
    bool correctSequence = true;
    for (int i = 0; i < colorIndex; i++) {
      if(colorSequence[i] != trueSequence[i]) {
        correctSequence = false;
        break;
      }
    }
    if (correctSequence == true && motorStopped != 1) {
      playerPoints += round(1000 * colorIndex/thinkTime);
    }

    if (motorStopped == 1) {
      playerPoints -= 100;
    }

    colorIndex = 0;
    motorStopped = 0;
    gameRound += 1;
    thisRound = false;
  }


}