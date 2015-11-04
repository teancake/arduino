This program tests the writing speed of a microSD card module. 
The microSD module used in this test can be found here
http://www.ebay.com/itm/Micro-SD-Card-Module-TF-Reader-Storage-Card-Shield-SPI-for-Arduino-/272016000451?hash=item3f556ac5c3:g:xl4AAOSwbqpTt8jP

Arduino MEGA2560 and Arduino Pro Mini boards are tested.

The interface between the microSD module and the arduino is SPI which has 4 wires plus the VCC and the GND wires.

Results from this test are as follows:

There are possile conflicts between Timer 1 interrupts and the delay function in arduino. When delay function is used in the Timer 1 interrupt routine, the timing can be very poor, e.g. 10-20 milliseconds difference.

There are also possible conflicts between mills()/micros() functions and the Timer 1 interrrupts. When micros/mills are called repeatedly in the interupt service routine, micros called at a later time may have slightly smaller values, very weird. 

mills() function could be used to simulate a timer interrupt, as shown in the code. The resolution of micros() is not 1 microsecond, but a few microseconds, which means that when micros() function is used to simulate a timer interrupt, the interval between subsequent interrupts cannot be less than its resolution.

The writing speed varies considerably on microSD cards with different sizes/speed class. For my testing, it varies from around 10ms to 40ms for a flush operation.


