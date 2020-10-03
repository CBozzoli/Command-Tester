// SacAstromech Command Tester v1.1
// By Chris Bozzoli (DBoz)
// With code contributions from Neil Hutchison (theJugg1er)
// v1.1 Added Debug Switch and I2C Receive Switch. Rewrote Read Function and added do_Command function. Simplified Main Loop. 9/26/20

//User modifiable variables are located on the config.h tab.
//DO NOT MODIFY ANYTHING BELOW THIS LINE

#include <Wire.h>
#include "config.h"

int I2CCommand = 0;

int SHIFT_SW;

//Variables for Read Function
byte buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16}; // pin numbers for all Command Buttons
#define STATUS_LED  13
#define SHIFT_PIN   17
#define NUMBUTTONS sizeof(buttonPins) // number of buttons (automatically calculated)
byte buttonState[NUMBUTTONS];  // array holds the actual HIGH/LOW states
byte buttonChange[NUMBUTTONS]; // array holds the state changes when button is pressed or released
enum {UNCHANGED, BUTTONUP, BUTTONDOWN};

// ------------------------------------------
void setup() {

  uint16_t baudrate;

#ifdef _57600BAUDStealth_
  baudrate = 57600;
#elif  _9600BAUDSJEDI_
  baudrate = 9600;
#else
  baudrate = 2400;
#endif

  Serial.begin(baudrate); //Begin Serial Bus

  DEBUG_PRINT_LN("SacAstromech Command Tester v1.0 (9-25-20)");
  DEBUG_PRINT_LN(COMMAND_SET);

  DEBUG_PRINT("My i2C Address: ");
  DEBUG_PRINT_LN(MYI2CADDR);

  Wire.begin();  // Start I2C communication Bus as the Master

  Wire.onReceive(receiveEvent); // routine to call/run when we receive a simple I2C command

  for (int i = 0; i < NUMBUTTONS; i++) pinMode(buttonPins[i], INPUTMODE); // Defines mode for Command Button Pins

  pinMode(SHIFT_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);

  digitalWrite(STATUS_LED, LOW);

}

//----------------------------------------------------------------------------
//  Delay function
//----------------------------------------------------------------------------
void waitTime(unsigned long waitTime)
{
  unsigned long endTime = millis() + waitTime;
  while (millis() < endTime)
  {}// do nothing
}

//----------------------------------------------------------------------------
//  Blink function
//----------------------------------------------------------------------------

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i = 0; i < loops; i++)  {
    digitalWrite(PIN, HIGH);
    waitTime(DELAY_MS);
    digitalWrite(PIN, LOW);
    waitTime(DELAY_MS);
  }
}

//----------------------------------------------------------------------------
//  I2C Command Functions
//----------------------------------------------------------------------------

//Send Event

void sendI2C(uint8_t addr, const char *I2Ccmd, bool CheckSum) {
  byte sum = 0; //stores Checksum for Stealth Receiver
  const char *i;
  i = I2Ccmd;
  Wire.beginTransmission(addr);
  while (*i) {
    Wire.write(*i);
    sum += byte(*i); //Checksum for Stealth Controller
    i++;
  }
  if (CheckSum) {
    Wire.write(sum); //Checksum for Stealth Controller
    DEBUG_PRINT("Checksum: ");
    DEBUG_PRINT_LN(sum);
    Wire.endTransmission();
  }
  else {
    Wire.endTransmission();
  }

  DEBUG_PRINT("Sent I2C Command: ");
  DEBUG_PRINT(I2Ccmd);
  DEBUG_PRINT(" to: ");
  DEBUG_PRINT_LN(addr);
}

//----------------------------------------------------------------------------
/* The default behaviour of the Command Tester does not need to receive
   I2C commands; however, the receive code is included in case a scenario
   arises where it is needed.
   The receive command function is different depending on what device
   is sending the I2C commands. For example, the Stealth Controller sends
   integer I2C commands only, but the Marcduino sends commands as character
   strings.  You can define which receive event to use on the config.h tab.
   The complex example will only handle commands that are numbers.
   It will not parse JAWALITE commands.
*/

void receiveEvent(int eventCode) {

#ifdef SIMPLE_I2C
  //Receive event for Integer commands only as come from the Stealth Controller)

#define I2C_REC_MODE "Simple"

  I2CCommand = Wire.read();

  DEBUG_PRINT("Received I2C Command Code: ");
  DEBUG_PRINT_LN(I2CCommand);

#else
  // Receive event for complex commands made up of strings.
  // The command line builder is needed along with this function.
  // Will not parse JAWALITE commands.

#define I2C_REC_MODE "Complex"

  while (Wire.available()) {

    bool command_available;
    char ch = (char)Wire.read();

    DEBUG_PRINT("I2C Character received ");
    DEBUG_PRINT_LN(ch);

    command_available = buildCommand(ch, cmdString); // build command line

    if (command_available)
    {
      I2CCommand = atoi(cmdString); // Converts the command line from ASCII to an Integer

      DEBUG_PRINT("Received Command String: ");
      DEBUG_PRINT_LN(cmdString);

      DEBUG_PRINT("Received I2C Command Code: ");
      DEBUG_PRINT_LN(I2CCommand);
    }
  }

#endif

  DEBUG_PRINT("I2C Receive Mode: ");
  DEBUG_PRINT_LN(I2C_REC_MODE);

}

// Receive event command line builder, makes a valid command line from the input

byte buildCommand(char ch, char* output_str)
{
  static uint8_t pos = 0;
  switch (ch)
  {
    case '\r':                          // end character recognized
      output_str[pos] = '\0'; // append the end of string character
      pos = 0;      // reset buffer pointer
      return true;      // return and signal command ready
      break;
    default:        // regular character
      output_str[pos] = ch; // append the  character to the command string
      if (pos <= CMD_MAX_LENGTH - 1)pos++; // too many characters, discard them.
      break;
  }
  return false;
}

//----------------------------------------------------------------------------
//  Button Read/Debounce function
//----------------------------------------------------------------------------

void Read() { // read the input state and state changes of all Command Buttons

  static unsigned long lastButtonTime; // time stamp for remembering the time when the button states were last read
  memset(buttonChange, 0, sizeof(buttonChange)); // reset all old state changes

  if (millis() - lastButtonTime < BOUNCETIME)
    return; // within bounce time: leave the function

  lastButtonTime = millis(); // remember the current time

  for (int i = 0; i < NUMBUTTONS; i++) {
    byte curState = digitalRead(buttonPins[i]);      // current button state


    if (INPUTMODE == INPUT_PULLUP) curState = !curState; // logic is inverted with INPUT_PULLUP

    if (curState != buttonState[i]) {                 // state change detected

      if (curState == HIGH) buttonChange[i] = BUTTONDOWN;

      else buttonChange[i] = BUTTONUP;
    }

    buttonState[i] = curState; // save the current button state
  }
}

//----------------------------------------------------------------------------
//  Execute Command Function
//----------------------------------------------------------------------------
void do_Command() {

  for (int i = 0; i < NUMBUTTONS; i++) {

    if ((buttonChange[i]) == BUTTONDOWN) {

      SHIFT_SW = digitalRead(SHIFT_PIN);

      if (SHIFT_SW == HIGH) { //For Commands 1-14
        sendI2C(PSI_FRNT, CMD[i], false);
        Serial.print(CMD[i]);
        DEBUG_PRINT_LN(); //Starts new line
        Blink(STATUS_LED, 500, 2);
      }
      else { //For Commands 15-28
        sendI2C(PSI_FRNT, SHIFT_CMD[i], false);
        Serial.print(SHIFT_CMD[i]);
        DEBUG_PRINT_LN(); //Starts new line
        Blink(STATUS_LED, 500, 3);
      }
    }
  }
}

//-----------------------------------------------------
// Main Loop
//-----------------------------------------------------

void loop() {

  Read(); // Read the Command Pins

  do_Command(); // Do commands for any buttons that have been pressed
}
