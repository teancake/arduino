#include <SPI.h>
#include <SD.h>
#include <TimerOne.h>



// The CS pin on the MicroSD card module is connected to pin CS_PIN
// of the Arduino board
#define CS_PIN 2
// logging interval in microseconds
#define LOG_INTERVAL_MS  50
// log file name
#define LOG_FILE_NAME   "datalog.txt"


// debug flag
#define DEBUG
// software serial might be useful for boards like arduino pro mini 
//#define USE_SOFTWARE_SERIAL
// baudrate for debugging serial connection
#define debugBaudRate 115200


#ifdef DEBUG
#define DBG(message)    DebugSerial.print(message)
#define DBGLN(message)    DebugSerial.println(message)
#define DBGW(message)    DebugSerial.write(message)
#else
#define DBG(message)
#define DBGLN(message)
#define DBGW(message)
#endif


// The following code defines a software serial for debugging
#ifdef USE_SOFTWARE_SERIAL
#include <SoftwareSerial.h>
#define _DBG_RXPIN_ 2
#define _DBG_TXPIN_ 3
SoftwareSerial DebugSerial(_DBG_RXPIN_, _DBG_TXPIN_);
#else
#define DebugSerial Serial
#endif



// file pointer
File fp;
// a buffer of 200 chars
char *buf="123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,123456789,";  

unsigned long time;
unsigned long time_ms;


void setup() {
  // Open serial communications and wait for port to open:
  DebugSerial.begin(debugBaudRate); 
  
  DBGLN("Initializing SD card...");
  // set the chip select pin to be output
   pinMode(CS_PIN, OUTPUT);
   
  // see if the card is present and can be initialized:
  while (!SD.begin(CS_PIN)) {
    DBGLN("Card failed, or not present");
    delay(100);
  }
  DBGLN("Card initialised.");
  // If timer1 is used, the following are the settings.
  //  Timer 1 settings
  //  Timer1.initialize(LOG_INTERVAL_MS*1000);
  //  Timer1.attachInterrupt(datalog_callback);
  
  // check whether the file exists
  if (SD.exists(LOG_FILE_NAME)) {
    DBGLN("File exists, deleting the file.");
    SD.remove(LOG_FILE_NAME);
  }
  DBGLN("System initialisation finished.");

}


void loop(void) {
  // make a string for assembling the data to log:
  time = micros();
  time_ms = time / 1000;
  if (time_ms % LOG_INTERVAL_MS) {
  } else {
    DBGLN(time_ms);
    DBG("OPENING FILES...");
    DBGLN(micros()/1000);
    fp = SD.open(LOG_FILE_NAME, FILE_WRITE);
    DBG("OPENING FILES DONE. ");
    DBGLN(micros()/1000);
    if (fp) {
      DBG("WRITING...");
      DBGLN(micros()/1000 );
      fp.print(time_ms);
      fp.print(",");
      fp.println(buf);
      DBG("WRITING DONE. ");
      DBGLN(micros()/1000 );
      DBG("CLOSING FILES...");
      DBGLN(micros()/1000 );
      fp.close();
      DBG("CLOSING FILES DONE. ");
      DBGLN(micros()/1000);
    } else {
      DBGLN("error opening datalog.txt");
    }

  }
}

// if timer 1 is used then uncomment the following callback code
/*
void datalog_callback(void) {

}
*/

