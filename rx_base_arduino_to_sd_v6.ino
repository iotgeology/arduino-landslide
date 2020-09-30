
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
const int chipSelect = 4;

String wholePacket= "";

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  Serial.begin(9600);           // start serial for output

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}


void loop() {
  delay(100);
}

void receiveEvent() {
  String wholePacketA = "";
  
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    //Serial.println(c);         // print the character
    wholePacketA = wholePacketA+c;
  }
  Serial.println(wholePacketA);
  
  File dataFile = SD.open("tiltTest.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(wholePacketA);       
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening tiltTest.txt");
  }
}
