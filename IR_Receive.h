#ifndef IR_RECEIVE_H
#define IR_RECEIVE_H

#define INPUT PORTBbits.RB0

// Set OSC
#define _XTAL_FREQ 8000000
#define __delay_50() _delay(50*(_XTAL_FREQ/4000000.0))

void IR_init();
unsigned int IR_recv();

#endif /* IR_RECEIVE_H */

/*  Command Table
 *  0x62    UP
 *  0x22    LEFT
 *  0xC2    RIGHT
 *  0xA8    DOWN
 *  0x02    OK
 *  0x4A    0
 *  0x68    1
 *  0x98    2
 *  0xB0    3
 *  0x30    4
 *  0x18    5
 *  0x7A    6
 *  0x10    7
 *  0x38    8
 *  0x52    9
 *  0x42    *
 *  0x52    #
 *  0xFF    repeat
 */