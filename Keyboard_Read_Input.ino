
/*
  Take user input from keyboard. When q is sent Turn on RELAY, when p is sent Turn off relay.


*/

#include <Wire.h>

#define DEBUG_OUTPUT
#define REGISTER_MAP_SIZE    4
#define BIT0              0b00000001

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
#ifdef DEBUG_OUTPUT
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
#endif
  }

  switch (incomingByte) {
    case 113:
      relayON(); //"q"
      digitalWrite(13, HIGH);
#ifdef DEBUG_OUTPUT
      Serial.println("ON");
      incomingByte = 0;
#endif
      break;

    case 112:
      relayOFF(); //"p"
      digitalWrite(13, LOW);
#ifdef DEBUG_OUTPUT
      Serial.println("OFF");
      incomingByte = 0;
#endif
      break;

    case 115:
      getStatus(); //"s"
#ifdef DEBUG_OUTPUT
      Serial.println("Status");
      incomingByte = 0;
#endif
      break;

    case 97:
      changeAddress(2); //"a"
#ifdef DEBUG_OUTPUT
      Serial.print("new address: 2");
      incomingByte = 0;
#endif
      break;
    case 106:
      changeAddress(200); //"j"
#ifdef DEBUG_OUTPUT
      Serial.println("new addres: 200");
      incomingByte = 0;
#endif
      break;
    case 111:
      changeAddress(1); //"o"
#ifdef DEBUG_OUTPUT
      Serial.println("new address: 1");
      incomingByte = 0;
#endif
      break;
    default:
      break;
  }

}


/*   ==================     ==================     ==================  */
/*   ==================     ==================     ==================  */

/*========================================================*/
//         Helper Functions
/*========================================================*/
/*
    @brief: Starts I2C transmission with a LATEST_ADDRESS, writing to 0x01 register
      wirte a 1 to 0x01 turns on relay
      write a 0 to 0x01 turns off relay
    @input:  none
    @returns: none
    @flags:  none
*/
void relayON() {
  Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
  Wire.write(0x01);        // sends five bytes
  Wire.write(1);              // sends one byte
  Wire.endTransmission();    // stop transmitting
}
/*
    @brief: Starts I2C transmission with a LATEST_ADDRESS, writing to 0x01 register
      wirte a 1 to 0x01 turns on relay
      write a 0 to 0x01 turns off relay
    @input:  none
    @returns: none
    @flags:  none
*/
void relayOFF() {
  Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
  Wire.write(0x01);        // sends five bytes to the ON register
  Wire.write(0);              // sends one byte, set the bit in the ON register on (1) or off (0)
  Wire.endTransmission();    // stop transmitting
}

/*
    @brief: Starts I2C transmission with a LATEST_ADDRESS, writing to 0x03 register
        write to the 0x03 register,
        send _address to set slave a new slave address.
        restarts i2c communication with the latest address.
    @input:  _address, the new address for the slave.
    @returns: none
    @flags:  none
*/
void changeAddress(int _address) {
  Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
  Wire.write(0x03);        // sends five bytes to the 0x00 addredss
  LATEST_ADDRESS = _address;
#ifdef DEBUG_OUTPUT
  Serial.print("the current address is: ");
  Serial.println(LATEST_ADDRESS);
#endif
  Wire.write(LATEST_ADDRESS);              // sends new address
  Wire.endTransmission();    // stop transmitting
  Wire.begin(LATEST_ADDRESS);// start wtih the new address.
}
/*
    @brief: Requests data from the slave by writing to the
    @input:  none
    @returns: none
    @flags:  none
*/
void getStatus() {
  Wire.requestFrom(LATEST_ADDRESS, REGISTER_MAP_SIZE);    // request 4 bytes from slave device #1
  byte byteCount = 0; //way to know what bytes to throw away.
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character.
    byteCount++; 
    if (byteCount == 3) { //we know that the byteCount needs to be 3 because 
      //we know the status Register is the 3rd element in the array
      if (!(c & 0b00000000)) {
        Serial.println("relay is off");
      }
    else if (c & BIT0) {
        Serial.println("relay is on");
      }
      else{
        Serial.print("???  ");
        Serial.println(c);
      }
    }
  }
#ifdef DEBUG_OUTPUT
  Serial.println();
#endif
}
