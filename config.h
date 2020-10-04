// SacAstromech Command Tester v1.1
// Configuration Header

// Place the name of the command set here.  This is used for Debug lines.
// (e.g. "PSI Pro Commands")

#define COMMAND_SET "Test Command Set"

// Enter Command strings here.
// Place the command between the quote marks.
// Format is dependent on the devices you are using
// (e.g. 0T1|10\r for a JAWALITE formatted PSI Pro command with a time modifier)

//Array for Commands 1-14
char *CMD[] {
"" ,     // CMD 1 
"" ,     // CMD 2 
"" ,     // CMD 3 
"" ,     // CMD 4 
"" ,     // CMD 5 
"" ,     // CMD 6 
"" ,     // CMD 7 
"" ,     // CMD 8 
"" ,     // CMD 9 
"" ,     // CMD 10 
"" ,     // CMD 11 
"" ,     // CMD 12 
"" ,     // CMD 13 
"" ,     // CMD 14 
};

//Array for Commands 15-28
char *SHIFT_CMD[] {
"" ,     // SHIFT/CMD 1 
"" ,     // SHIFT/CMD 2 
"" ,     // SHIFT/CMD 3 
"" ,     // SHIFT/CMD 4 
"" ,     // SHIFT/CMD 5 
"" ,     // SHIFT/CMD 6 
"" ,     // SHIFT/CMD 7 
"" ,     // SHIFT/CMD 8 
"" ,     // SHIFT/CMD 9 
"" ,     // SHIFT/CMD 10 
"" ,     // SHIFT/CMD 11 
"" ,     // SHIFT/CMD 12
"" ,     // SHIFT/CMD 13
"" ,     // SHIFT/CMD 14
};

// Baud Rate default is 2400.  Uncomment to switch to 9600 or 57600
//#define _57600BAUDStealth_  //Default BAUD Rate for Stealth Controller systems
//#define _9600BAUDSJEDI_     //Default BAUD Rate for Jedi Controller systems

// incoming I2C command
#define MYI2CADDR 0      //I2C Address for Command Tester

// Destination I2C address (change to match the device you are testing)
#define DEST_I2C 22 //PSI Pro

/* The default behaviour of the Command Tester does not need to receive
*  I2C commands; however, the receive code is included in case a scenario
*  arises where it is needed.
*  The receive command function is different depending on what device
*  is sending the I2C commands. For example, the Stealth Controller sends
*  integer I2C commands only, but the Marcduino sends commands as character
*  strings.
*  The complex example will only handle commands that are numbers.
*  It will not parse JAWALITE commands.
*/

// Uncomment to use basic integer I2C receive handling
//#define SIMPLE_I2C

// Uncomment this if you want Debug output.
// By default debug output is not enabled in
// the release version
#define DEBUG

//Setup Debug Switch
#ifdef DEBUG
  #define DEBUG_PRINT_LN(msg)  Serial.println(msg)
  #define DEBUG_PRINT(msg)  Serial.print(msg)
#else
  #define DEBUG_PRINT_LN(msg)
  #define DEBUG_PRINT(msg)
#endif

//-------------- Nothing Below This Line Should Need to be Changed --------------

//Variables for Read Function
#define INPUTMODE INPUT_PULLUP    // Pin Mode for Command pins: INPUT or INPUT_PULLUP
#define BOUNCETIME 50             // bouncing time in milliseconds

// maximum number of characters in a command (63 chars since we need the null termination)
#define CMD_MAX_LENGTH 64

// memory for command string processing
char cmdString[CMD_MAX_LENGTH];
