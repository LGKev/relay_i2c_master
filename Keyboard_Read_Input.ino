
/*
 Take user input from keyboard. When q is sent Turn on RELAY, when p is sent Turn off relay.

 
*/

#include <Wire.h>

int incomingByte = 0;   // for incoming serial data
int LATEST_ADDRESS = 1;     //global so address can be changed by user.
byte x = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("master awake");
  pinMode(13, OUTPUT);
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop() {

 // getStatus();

  
        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();

                // say what you got:
                Serial.print("I received: ");
                Serial.println(incomingByte, DEC);
        }

//if(incomingByte == 113){
//    relayON();
//  digitalWrite(13, HIGH);
//}
//
//if(incomingByte == 112){
//    relayOFF();
//  digitalWrite(13, LOW);
//}

switch(incomingByte){
 case 113: 
    relayON();
    digitalWrite(13, HIGH);
    break;
  
 case 112:
     relayOFF();
    digitalWrite(13, LOW);
    break;

 case 115:
      getStatus();
      break;

  case 97:
    //changeAddress(0x90);
    break;

  default:
      break;   
}


        
}


/*   ==================     ==================     ==================  */
/*   ==================     ==================     ==================  */

void relayON(){
  Wire.beginTransmission(1); // transmit to device #1
  Wire.write(0x01);        // sends five bytes
  Wire.write(1);              // sends one byte
  Wire.endTransmission();    // stop transmitting
}

void relayOFF(){
  Wire.beginTransmission(1); // transmit to device #1
  Wire.write(0x01);        // sends five bytes to the ON register 
  Wire.write(0);              // sends one byte, set the bit in the ON register on (1) or off (0)
  Wire.endTransmission();    // stop transmitting
}


void changeAddress(int _address){
  Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
  Wire.write(0x00);        // sends five bytes to the 0x00 addredss
 // LATEST_ADDRESS = _address;
  Wire.write(LATEST_ADDRESS);              // sends new address
  Wire.endTransmission();    // stop transmitting

  Wire.begin(LATEST_ADDRESS);//
}

void getStatus(){
  Wire.requestFrom(1, 4);    // request 6 bytes from slave device #8
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character.

    Serial.print(c);         // print the character
  }
  Serial.println();
}
