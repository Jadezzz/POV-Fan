#include "IR_Receive.h"
#include <xc.h>
#include <pic18f4520.h>

//void __interrupt(high_priority) Hi_ISR(void)
//{
//    if(INT0IF){
//        code = IR_recv();
//        NOP();
//        INT0IF = 0;
//    }
//    return;
//}

void IR_init(){
    // Set input pin
    TRISBbits.RB0 = 1;
    // Set osc 8MHz
    OSCCONbits.IRCF = 0b111; //8MHz
    
    // Set interrupt
    RCONbits.IPEN = 1;
    INTCONbits.GIE = 1;
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 0;      
}

unsigned int IR_recv(){
    unsigned int ir_code = 0;
    int count = 0, i;
    ir_code = 0;
    //check 9ms pulse
    while((INPUT == 0) && (count < 200)){
        count++;
        __delay_50();
    }
    
    count = 0;
    
    // check 4.5ms space
    while((INPUT == 1) && (count < 100)){
        count++;
        __delay_50();
    }
    
    
    // Read message
    for(i = 0; i < 32; i++){
        count = 0;
        while((INPUT == 0) && (count < 14)){
            count++;
            __delay_50();
        }    
        count = 0;
        while((INPUT == 1) && (count < 40)){
            count++;
            __delay_50();
        }
        if(count > 20){
            ir_code |= (1 << (31 - i));
        }    
    }   
    return ir_code >> 8;
}
