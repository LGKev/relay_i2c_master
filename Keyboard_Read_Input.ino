
/*
  Take user input from keyboard. When q is sent Turn on RELAY, when p is sent Turn off relay.


*/

#include <Wire.h>

#define DEBUG_OUTPUT

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
      changeAddress(0x21); //"a"
      incomingByte = 0;
      break;
    case 106:
      changeAddress(0x19); //"j"
      incomingByte = 0;
      break;
    case 111:
      changeAddress(0x18); //"o"
      incomingByte = 0;
      break;

	case 102:  //f
	Serial.print("version Number: ");
	Serial.println(getFirmwareVersion(), 1);
	incomingByte = 0;
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
      write a 1 to 0x01 turns on relay
      write a 0 to 0x01 turns off relay
    @input:  none
    @returns: none
    @flags:  none
*/
void relayON() {
	Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
	//check here for an ACK from the slave, if no ack don't allow change?
	//if 2 no slave attached. 
	if(Wire.endTransmission() == 2){
		Serial.println("Check Connections. No slave attached.");
	}
	else{
	Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
	Wire.write(0x01);        // sends five bytes
	Wire.endTransmission();    // stop transmitting
	}
}
/*
    @brief: Starts I2C transmission with a LATEST_ADDRESS, writing to 0x01 register
      write a 1 to 0x01 turns on relay
      write a 0 to 0x01 turns off relay
    @input:  none
    @returns: none
    @flags:  none
*/
void relayOFF() {
	
	Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
	//check here for an ACK from the slave, if no ack don't allow change?
	//if 2 no slave attached. 
	if(Wire.endTransmission() == 2){
		Serial.println("Check Connections. No slave attached.");
	}
	else{
  Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
  Wire.write(0x00);              // sends one byte, set the bit in the ON register on (1) or off (0)
  Wire.endTransmission();    // stop transmitting
	}
	
	
  
}

float getFirmwareVersion(){
	
	Wire.beginTransmission(LATEST_ADDRESS);
	Wire.write(0x04); //command for firmware
	Wire.endTransmission();
	//request data
	Wire.requestFrom(LATEST_ADDRESS, 4); //4 BYTES for the [float] Version Number
	
	unsigned long versionNumber = 0;
	byte counter = 0; // keep track of how many bytes we have actually gotten.
	while (Wire.available() > 4) { // slave must sent the 4 bytes, 3 or 4? i think 3 or 4>=
	if(counter < 4){
    versionNumber |= Wire.read(); // receive a byte, and logical or into the version number
	versionNumber = versionNumber << 8; //shift over 1 byte (8 bits)
	counter++;
		}
	} //the while loop should run 4 times
	return(versionNumber);
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
	//check if valid address. 
	if(_address > 0x07 && _address < 0x78){
	Serial.print("the current address is: ");
	Serial.println(LATEST_ADDRESS, HEX);
	
		//valid address
	Wire.beginTransmission(LATEST_ADDRESS); // transmit to device #1
	
	//check here for an ACK from the slave, if no ack don't allow change?
	//if 2 no slave attached. 
	if(Wire.endTransmission() == 2){
		Serial.println("Check Connections. No slave attached.");
	}
	else{
		Wire.beginTransmission(LATEST_ADDRESS);
	}
	
	Wire.write(0x03);        // sends five bytes to the 0x00 address
	LATEST_ADDRESS = _address;
	Wire.write(LATEST_ADDRESS);              // sends new address
	Wire.endTransmission();    // stop transmitting //this looks like it was essential. 
	Wire.begin(LATEST_ADDRESS);// start with the new address..
	Serial.print("The new address is: 0x");
	Serial.println(_address, HEX);
	}
	else{
		Serial.println("Not a valid I2C Address. Needs to be between 0x77 and 0x78.");
		Serial.print("The address is still:     ");
		Serial.println(LATEST_ADDRESS, HEX);
	}
 }
/*
    @brief: Requests data from the slave by writing to the
    @input:  none
    @returns: none
    @flags:  none
*/
void getStatus() {
  Wire.requestFrom(LATEST_ADDRESS, 1);    // request 1 bytes from slave device LATEST_ADDRESS
  
  Serial.print("you are talking to slave at the address: 0x");
  Serial.println(LATEST_ADDRESS, HEX);
  //TODO: maybe report the address? well we would technically know because thats how we init the i2c write. 
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character. 
	if(c ==0x01)Serial.println("relay on");
	else{
		Serial.println("relay off!");
	}
  }
 }

