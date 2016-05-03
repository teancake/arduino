/**
   srf01_changeaddr.ino
   @author Xiaoke Yang <das.xiaoke@hotmail.com>

   Change the address of an SRF01 rangefinder.

*/

#include <SoftwareSerial.h>


/* RX and TX for SRF01, i.e. one-wire comm, Digital pin number on Arduino */
#define SRF_TXRX		10


/* List of Commands for SRF01 *************************************************/
/* Real Ranging Mode - Result in inches */
#define RANGE_INCH		0x50
/* Real Ranging Mode - Result in centimeters */
#define RANGE_CM		0x51
/* Real Ranging Mode - Result in inches, automatically Tx range back
   to controller as soon as ranging is complete.
*/
#define QRANGE_INCH		0x53
/* Real Ranging Mode - Result in centimeters, automatically Tx range back
   to controller as soon as ranging is complete.
*/
#define QFASTRANGE_CM           0x54
/* Fake Ranging Mode - Result in inches */
#define FAKERANGE_INCH          0x56
/* Fake Ranging Mode - Result in centimeters */
#define FAKERANGE_CM            0x57
/* Fake Ranging Mode - Result in inches, automatically Tx range back to
   controller as soon as ranging is complete.
*/
#define QFAKERANGE_INCH		0x59
/* Fake Ranging Mode - Result in centimeters, automatically Tx range back to
   controller as soon as ranging is complete.
*/
#define QFAKERANGE_INCM         0x5A
/* Transmit an 8 cycle 40khz burst - no ranging takes place */
#define SENDBURST		0x5C
/* Get software version - sends a single byte back to the controller */
#define GETVERSION		0x5D
/* Get Range, returns two bytes (high byte first) from the most recent ranging.
*/
#define GETRANGE		0x5E
/* Get Status, returns a single byte. Bit 0 indicates "Transducer locked",
   Bit 1 indicates "Advanced Mode"
*/
#define GETSTATUS		0x5F
/* Sleep, shuts down the SRF01 so that it uses very low power (55uA). */
#define SLEEP			0x60
/* Unlock. Causes the SRF01 to release and re-acquire its "Transducer lock".
   Used by our factory test routines.
*/
#define UNLOCK			0x61
/* Set Advanced Mode (Factory default) - When locked, SRF01 will range down
   to zero.
*/
#define ADVANCEDMODE		0x62
/* Clear Advanced Mode - SRF01 will range down to approx. 12cm/5in, */
#define CLEARADVANCED		0x63
/* Changes the baud rate to 19200 */
#define BAUD19200		0x64
/* Changes the baud rate to 38400 */
#define BAUD38400		0x65

/* 1st in sequence to change address */
#define CHANGEADDR_FIRST	0xA0
/* 3rd in sequence to change address */
#define CHANGEADDR_THIRD    0xA5
/* 2nd in sequence to change address */
#define CHANGEADDR_SECOND   0xAA


/* Set up software serial port for the SRF01 */
SoftwareSerial srf01 = SoftwareSerial(SRF_TXRX, SRF_TXRX);

/* old and new addresses */
byte addr_old, addr_new;
/* range finding test value */
int range;

void setup() {
  /* start the software serial */
  srf01.begin(9600);
  /* listening for coming data on the software serial port */
  srf01.listen();
  /* Initialize the default serial and wait for port to open */
  Serial.begin(115200);
  /* Wait some time to make sure everything is powered up */
  delay(200);
  
  /* the new address to be set */
  addr_new = 2;
  /* get the old address */
  addr_old = srf01_get_address();
  Serial.print("The old address is ");
  Serial.println(addr_old);
  /* a range-finding test */
  range = srf01_get_distance_cm(addr_old);
  Serial.print("We do a rangefinding test on this old address. Range = ");
  Serial.println(range);
  /* set the new address */
  srf01_set_address(addr_old, addr_new);
  /* test the new address */
  addr_old = srf01_get_address();
  Serial.print("The address has been changed. The new address is ");
  Serial.println(addr_old);
  /* a range-finding test on the new address */
  range = srf01_get_distance_cm(addr_old);
  Serial.print("We do a rangefinding test on the new address. Range = ");
  Serial.println(range);
}

void loop(void) {

}

/****************************************************************************
   Name: srf01_get_address

   Description:
     This function gets the address of the current srf01 range finder.
     Only ONE srf01 is allowed to connect, since this function pools from
     address 0x01 t0 0x10 and see which address responds to the pool.
   Input Parameters:
     None
   Returned Value:
     the address of the srf01 module connected.

 ****************************************************************************/
byte srf01_get_address() {
  byte addr = 1;
  /* we use the software version command to check responses from the srf01 */
  byte swv;
  while (addr < 16) {
    /* the software version variable is reset to 0 at each loop */
    swv = 0;
    /* send the GET SOFTWARE VERSION command */
    srf01_send_cmd(addr, GETVERSION);
    /* wait 500 ms to see whether there are reponses received */
    unsigned long tic = millis();
    while (millis() - tic < 500) {
      /* if there are responses */
      if (srf01.available() > 0) {
        /* update the software version variable, otherwise it is kept as 0 */
        swv = srf01.read();
      }
    }
    /* if the software version variable is not 0, the address is found */
    if (swv != 0) {
      return addr;
    }
    addr++;
  }
  /* if error occurs, 0 is returned, and 0 is not a valid address for srf01 */
  return 0;
}


/****************************************************************************
   Name: srf01_set_address

   Description:
     This function sets the address of the current srf01 range finder.
     Only ONE srf01 is allowed to connect.
   Input Parameters:
     addr_old - The old address of the srf01 module
     addr_new - The address to be set
   Returned Value:
     None

 ****************************************************************************/
void srf01_set_address(byte addr_old, byte addr_new) {
  /* send the 4 address-changing commands in a sequence */
  srf01_send_cmd(addr_old, CHANGEADDR_FIRST);
  srf01_send_cmd(addr_old, CHANGEADDR_SECOND);
  srf01_send_cmd(addr_old, CHANGEADDR_THIRD);
  /* the 4th command contains the target address */
  srf01_send_cmd(addr_old, addr_new);
}

/****************************************************************************
   Name: get_distance_cm_from_srf01

   Description:
     This function gets the distance returned by the srf01 rangefinder
     
   Input Parameters:
     addr - The address of the srf01 module

   Returned Value:
     The distance measurement in centimeters

 ****************************************************************************/
int srf01_get_distance_cm(byte addr) {
  byte hByte = 0;
  byte lByte = 0;
  /* initiate a ranging action */
  srf01_send_cmd(addr, RANGE_CM); 
  /* wait 70ms according to the manual before the data can be queried */
  _delay_ms(70);
  /* obtain the ranging data */
  srf01_send_cmd(addr, GETRANGE);
  /* wait for two bytes of data within 500ms */
    unsigned long tic = millis();
  while (millis() - tic < 500){
    if (srf01.available() >= 2){
        /* the higher byte is transmitted back first */
        hByte = srf01.read(); 
        /* then the lower byte */
        lByte = srf01.read();     
    }
  }
  /* return the computed range as a 16-bit integer */
  return ((hByte << 8) + lByte); 
}


/****************************************************************************
   Name: srf01_send_cmd

   Description:
     This function sends a command to the srf01 rangefinder.

   Input Parameters:
     addr - The address of the srf01 rangefinder
     cmd - The command to be sent

   Returned Value:
     None

 ****************************************************************************/
void srf01_send_cmd(byte addr, byte cmd) {
  /* set the digital pin on arduino as output */
  pinMode(SRF_TXRX, OUTPUT);
  /* Send a 2ms break to begin communications with the srf01.
     This is required by the manual of the srf01 rangefinder. A break is a low
     level for 12-bit time or longer.
  */
  digitalWrite(SRF_TXRX, LOW);
  delay(2);
  /* Restore the level of the TXRX wire */
  digitalWrite(SRF_TXRX, HIGH);
  delay(1);
  /* Write the first byte, i.e. the address */
  srf01.write(addr);
  /* Write the second byte, i.e. the command itself */
  srf01.write(cmd);
  /* set the digital pin as input */
  pinMode(SRF_TXRX, INPUT);
  /* As RX and TX are the same pin, the data sent out will be received
     on the same wire and have to be ignored.
  */
  byte datasent = srf01.available();
  for (byte x = 0;  x < datasent ; x++) {
    byte junk = srf01.read();
  }
}


