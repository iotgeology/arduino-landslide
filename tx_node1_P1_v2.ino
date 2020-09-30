#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#include <Wire.h>

RF24 radio(7,8);                    // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);          // Network uses that radio
//THE LIST OF NODES ON THE TREE
// Address of our node in Octal format, And Node ID in number
  const uint16_t base_node = 00;       // Address of the other node in Octal format
  
  const uint16_t this_node = 01;        
  uint8_t nodeIDVALUE = 1;
  //const uint16_t this_node = 02;        
  //uint8_t nodeIDVALUE = 2; 
  //const uint16_t this_node = 012;        
  //uint8_t nodeIDVALUE = 3; 
  //const uint16_t this_node = 0112;        
  //uint8_t nodeIDVALUE = 4; 

const unsigned long interval = 1100; //ms  // How often to send 'hello world to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

uint8_t packetTypeVALUE = 1;
//defined up top: uint8_t nodeIDVALUE = 2; 
uint16_t accelXVALUE = 1;
uint16_t accelYVALUE = 2; 
uint16_t accelZVALUE= 3;
uint16_t  gyroXVALUE = 4;
uint16_t  gyroYVALUE = 5; 
uint16_t  gyroZVALUE = 6;
float tempVALUE = 1.1;

float distanceVALUE = 2.2; 

//FOR MPU 6050
  float accelX, accelY, accelZ;
  float gForceX, gForceY, gForceZ;
  float gyroX, gyroY, gyroZ;
  float rotX, rotY, rotZ;
  
  unsigned int gForceXint, gForceYint, gForceZint;
  unsigned int rotXint, rotYint, rotZint;

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

void setup(void)
{
  Serial.begin(115200);
  Serial.println("RF24Network/examples/helloworld_tx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);

  setupMPU();
}

void loop() {

  mpuINTOlongnumber();

  tempVALUE = 1.1;
  distanceVALUE = 2.2; 
    
  network.update();                          // Check the network regularly

  
  unsigned long now = millis();              // If it's time to send a message, send it!
  if ( now - last_sent >= interval  )
  {
    last_sent = now;

    Serial.print("Sending...");
    //payload_t payload = { millis(), packets_sent++, 1 };
    
    uint16_t msVALUE = millis();
    uint16_t counterVALUE = packets_sent++;
    
    payload_t payload = {packetTypeVALUE, nodeIDVALUE, msVALUE, counterVALUE,
                          accelXVALUE, accelYVALUE, accelZVALUE,  
                          gyroXVALUE, gyroYVALUE, gyroZVALUE,
                          tempVALUE,distanceVALUE};
    
    RF24NetworkHeader header(/*to node*/ base_node);
    bool ok = network.write(header,&payload,sizeof(payload));
    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");
  }
}


/*
 * FOR MPU-6050
 */
void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

//Reads in the Acc Data from the Registers on the Chip
void recordAccelRegisters() {
  //READ Acceleration VALUES
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  gForceX = accelX / 16384.0; //Converts Acceleration data into units of G
  gForceY = accelY / 16384.0;  //Converts Acceleration data into units of G
  gForceZ = accelZ / 16384.0; //Converts Acceleration data into units of G

  return gForceX, gForceY, gForceZ;
}

void recordGyroRegisters() {
  //READ GYRO VALUES
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  rotX = gyroX / 131.0; //This converts the Angles data into Actual Angles
  rotY = gyroY / 131.0;  //This converts the Angles data into Actual Angles
  rotZ = gyroZ / 131.0; //This converts the Angles data into Actual Angles

  return rotX, rotY, rotZ;
}

void mpuValues(){
  recordGyroRegisters();
  recordAccelRegisters();
}

void mpuINTOlongnumber(){
  mpuValues();
  
  if(gForceX>0){
    gForceXint = gForceX*100 +20000;
  }else{
    gForceXint = gForceX*100*(-1)+10000;
  }

  if(gForceY>0){
    gForceYint = gForceY*100 +20000;
  }else{
    gForceYint = gForceY*100*(-1)+10000;
  }

  if(gForceZ>0){
    gForceZint = gForceZ*100 +20000;
  }else{
    gForceZint = gForceZ*100*(-1)+10000;
  }


  if(rotX>0){
    rotXint = rotX*10 +20000;
  }else{
    rotXint = rotX*10*(-1)+10000;
  }

  if(rotY>0){
    rotYint = rotY*10 +20000;
  }else{
    rotYint = rotY*10*(-1)+10000;
  }

  if(rotZ>0){
    rotZint = rotZ*10 +20000;
  }else{
    rotZint = rotZ*10*(-1)+10000;
  }

//Set Up for the Packet
  accelXVALUE = gForceXint;
  accelYVALUE = gForceYint; 
  accelZVALUE= gForceZint;
  gyroXVALUE = rotXint;
  gyroYVALUE = rotYint; 
  gyroZVALUE = rotZint;
  
  return accelXVALUE, accelYVALUE, accelZVALUE;
  return gyroXVALUE, gyroYVALUE, gyroZVALUE; 
}
