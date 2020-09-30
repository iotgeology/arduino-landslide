#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#include <Wire.h>

RF24 radio(7,8);                // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 00;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 01;   // Address of the other node in Octal format

struct payload_t {                 
// Structure of our payload
//Packet uses 28 of up to 32 Bytes for rf24L01+ radio
  
  //Packet Header (8 Bytes)
    uint8_t packetType;
    uint8_t nodeID;
    uint32_t ms; //timer for packet
    uint16_t counter;//packet counter
  
  //From The Accelerometer (12 Bytes)     
    //Acceleration Data (6 Bytes)
      uint16_t accelX;
      uint16_t accelY; 
      uint16_t accelZ;
    //Gyro Data (6 Bytes)
      uint16_t  gyroX;
      uint16_t  gyroY; 
      uint16_t  gyroZ;
    //Temp Data (4 Bytes)
      float temp;
  //Ultra Sonic Distance  (4 Bytes)
    float distance; 
};

//String accelX = "121212";

void setup() {
  Serial.begin(9600);
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  
  Wire.begin(8);                // join i2c bus with address #8
  //Wire.onRequest(requestEvent); // register event
}

void loop() {
  
  network.update();                  // Check the network regularly
 
  while ( network.available() ) {     // Is there anything ready for us?
    
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.print(payload.ms);
    Serial.print(" from node ");
    Serial.println(payload.nodeID);
    
    Serial.print("Acceleration Data: ");
    Serial.print(payload.accelX);
    Serial.print(", ");
    Serial.print(payload.accelY);
    Serial.print(", ");
    Serial.println(payload.accelZ);  
      
    Serial.print("Gyroscope Data: "); 
    Serial.print(payload.gyroX);
    Serial.print(", ");
    Serial.print(payload.gyroY);
    Serial.print(", ");
    Serial.println(payload.gyroZ);

    Wire.beginTransmission(8); // transmit to device #8
      //Wire.write("H");
      String dataString = String(payload.counter) + "," + String(payload.ms) + "," + String(payload.nodeID) ;
      const char* dataString_array  = {dataString.c_str()};
      Wire.write( dataString_array ); 
    Wire.endTransmission();
  
    Wire.beginTransmission(8);
      Wire.write(","); 
      float aX = payload.accelX/100.0-100.0;
        if(aX>100){aX = aX-100;} else{aX=aX*(-1);}
      
      float aY = payload.accelY/100.0-100.0;
        if(aY>100){aY = aY-100;} else{aY=aY*(-1);}
        
      float aZ = payload.accelY/100.0-100.0;
        if(aZ>100){aZ = aZ-100;} else{aZ=aZ*(-1);}
      
      dataString = String(aX) + "," + String(aY) + "," + String(aZ) ;
      dataString_array  = {dataString.c_str()};
      Wire.write( dataString_array );
    Wire.endTransmission();    // stop transmitting
  
    Wire.beginTransmission(8);   
      Wire.write(",");
      float gX = payload.gyroX/10.0-1000.0;
        if(gX>1000){gX = gX-1000.0;} else{gX=gX*(-1);}
        
      float gY = payload.gyroY/10.0-1000.0;
        if(gY>1000){gY = gY-1000.0;} else{gY = gY*(-1);}
        
      float gZ = payload.gyroZ/10.0-1000.0;
        if(gZ>1000){gZ = gZ-1000.0;} else{gZ = gZ*(-1);}
      
      dataString = String(gX) + "," + String(gY) + "," + String(gZ) + "\n" ;
      dataString_array  = {dataString.c_str()};         
      Wire.write( dataString_array );   
    Wire.endTransmission();    // stop transmitting
  }
}
