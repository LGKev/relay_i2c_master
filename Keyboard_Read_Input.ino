
/*
  Take user input from keyboard. When q is sent Turn on RELAY, when p is sent Turn off relay.


*/

#include <Wire.h>

#define DEBUG_OUTPUT
#define REGISTER_MAP_SIZE    4
#define BIT0              0b00000001

int incomingByte = 0;   // for incoming serial data
volatile int LATEST_ADDRESS = 0x18;     //global so address can be changed by user.
byte x = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("master awake");
  pinMode(13, OUTPUT);
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop() {

  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);

  }

  switch (incomingByte) {
    case 113:
	incomingByte = 0; // reset, or else infinite loop.
      relayON(); //"q"
      digitalWrite(13, HIGH);
      Serial.println("ON");
      break;

    case 112:
	incomingByte = 0;
      relayOFF(); //"p"
      digitalWrite(13, LOW);
      Serial.println("OFF");
      break;

    case 115:
	incomingByte = 0;
      getStatus(); //"s"
      Serial.println("Status");
      break;

    case 97:
	incomingByte = 0;
      changeAddress(0x20); //"a"
      break;
    case 106:
	incomingByte = 0;
      changeAddress(0x19); //"j"
      break;
    case 111:
	incomingByte = 0;
   changeAddress(0x19); //"o"
      break;
	  
    default:
	incomingByte = 0;
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
  Wire.write(0x00);              // sends one byte, set the bit in the ON register on (1) or off (0)
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
  Wire.write(0x03);        // sends five bytes to the 0x00 address
  Wire.write(_address);
  Wire.endTransmission();
  LATEST_ADDRESS = _address;
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
      if (!(c & 0b00000000)) {
        Serial.println("relay is off");
      }
      else if (c & BIT0) {
        Serial.println("relay is on");
      }
      else {
        Serial.print("???  ");
        Serial.println(c);
      }
    }
  Serial.println();
}
