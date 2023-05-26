import processing.net.*; 
Client node1;
import processing.serial.*;
Serial arduino;

String darkValue;

void setup() { 
  node1 = new Client(this, "130.229.138.98", 5204);
  arduino = new Serial(this, Serial.list()[0], 9600);
  arduino.bufferUntil('\n'); // Reads data until next line, instead of taking every character
  arduino.clear();
}

void draw() { 
  if (node1.available() > 0) {
    String colorsAndTime = node1.readString();
    arduino.write(colorsAndTime);
  }
  
  if (darkValue != null) {
    node1.write(darkValue);
  }

} 




void serialEvent(Serial arduino) {
  String receivedData = arduino.readStringUntil('\n');
  darkValue = receivedData;
  //println(darkValue);
}
