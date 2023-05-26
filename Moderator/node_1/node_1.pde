import processing.serial.*;
Serial arduino;  // Create object from Serial class, used to communicate with Arduino board
import processing.net.*; 
Server node1;
Client node2;

String colorsAndTime = "";
String motorStopped = "";

void setup(){
  arduino = new Serial(this, "/dev/tty.usbmodem14101", 9600);
  arduino.bufferUntil('\n'); // Reads data until next line, instead of taking every character
  arduino.clear();
  node1 = new Server(this, 5204);
   
}

// This is called every time a client connects to our server
void serverEvent(Server someServer, Client someClient) {
  println("We have a new client: " + someClient.ip());
  node2 = someClient; // We can use this to send message to the client later.
}

// This is called every time a new message is received from the client node
void clientEvent(Client client) { 
  //String received = client.readString();
  //arduino.write("1");
}

void draw() {
  if (colorsAndTime != "") {
    node2.write(colorsAndTime);
    if (node2.available() > 0) {
      String darkValue = node2.readString();
      arduino.write(darkValue);
   if (motorStopped == "1"); {
     node2.write(motorStopped);
   }
     }
  }
  

}

// Callback function for reading and updating arduino values
void serialEvent(Serial arduino) {
  String receivedData = arduino.readStringUntil('\n');
  colorsAndTime = receivedData;
  println(colorsAndTime);
}
