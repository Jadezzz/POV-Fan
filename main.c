

#include <xc.h>
#include <stdlib.h>
#include <pic18f4520.h>
#include "IR_Receive.h"
#include "constant_variables.h"
#include "config.h"

void CCP1_init();
void INTERRUPT_init();

void time_mode(void);
void speed_mode(void);
void game_mode(void);
void game_pre_animation(void);

// Utility functions
void printChar(char letter);
void printNumber(int num);
void delay_between_words();
void game_win();
void game_over();
void print_word();
void update_time();

// Global Variables
volatile int code=0; // IR code 
volatile int LIGHT = 0; // Control a light period
volatile int state=0;
/*0:menu 1:timeMode 2:RPM mode 3: Pure Game Mode 4: Loading to Game*/

// Use for Clock
int seconds;
int minutes;
int hours;

// Use to control player position
volatile int player_row = 0;
volatile int player_col = 0;

// Use to control player blink frequency
char is_print = 0;

// Use to control animation
int pic_cycle = 0;
int pic = 0;
int pic_count = 0;
int pic_flag = 0;

// Use for RPM mode
volatile int is_rpm_count = 0;
volatile unsigned long int round_time = 0;


// Use for clock function
int time_count=0;


void OSC_init(){
    OSCCONbits.IRCF2 = 1;       // OSC = 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
}

void CCP1_init(){
    
    TRISCbits.TRISC2 = 1; // Set CCP1 pin as input
    T3CONbits.T3CCP2 = 0; // Timer1 is the capture/compare clock source for the CCP modules.
    T3CONbits.T3CCP1 = 0;
    CCP1CONbits.CCP1M = 0b0100; //Capture mode, for every falling edge.
    
    // TImer1 Settings
    T1CONbits.RD16 = 1;
    T1CONbits.T1RUN = 0;
    // Prescalar 1:8
    T1CONbits.T1CKPS = 0b11;
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;
    
    T3CONbits.T3CKPS=3;   
}

void INTERRUPT_init(){
    RCONbits.IPEN = 1; // Interrupt priority enable.
    INTCONbits.GIE = 1;  // Global interrupt enable.
    PIE1bits.CCP1IE = 1; // CCP1 interrupt enable.
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    IPR1bits.CCP1IP = 1; // CCP1 interrupt is low priority.
    PIR2bits.TMR3IF=0;
    PIE2bits.TMR3IE=1;
    IPR2bits.TMR3IP=1;

}


void main(void) {
    IR_init();
    OSC_init();
    INTERRUPT_init();
    CCP1_init();
    
    PORTA = 0;
    TRISD = 0;
    TRISAbits.RA6 = 0;
    TRISAbits.RA7 = 0;
    PORTD = 0;
    
    seconds=22;
    minutes=58;
    hours=19;
    
    state=0;
    
    T3CONbits.TMR3ON=1;

    while(1){

        if(LIGHT){
            if(state==0){
                print_word();       
            }else if(state==1){
                time_mode();     
            }else if(state==2){
                speed_mode();
            }else if(state == 3){
                game_mode();
                LIGHT = 0;
            }else if(state == 4){
                game_pre_animation();
            }else if(state == 5){
                game_over();
            }else if(state == 6){
                game_win();
            }
           
        }       
    }
    return;
}

void printNumber(int num){
    int delay;  // used to control print Number
    for(int i = 0;i < 5; i++){
        if(num == 0) PORTD = digit[num][i];
        else if(num == 1)PORTD = digit[num][i]; 
        else if(num == 2) PORTD = digit[num][i];
        else if(num == 3) PORTD = digit[num][i];
        else if(num == 4) PORTD = digit[num][i];
        else if(num == 5) PORTD = digit[num][i];
        else if(num == 6) PORTD = digit[num][i];
        else if(num == 7) PORTD = digit[num][i];
        else if(num == 8) PORTD = digit[num][i];
        else if(num == 9) PORTD = digit[num][i];
        else if(num == 10) PORTD = digit[num][i];
        delay = 50;
        while(delay--);
        PORTD = 0;
        delay = 50;
        while(delay--);
    } 
    delay = 100;
    while(delay--);
}

void delay_between_words(){
    int delay = 200;
    while(delay--);
}


void game_mode(){
    int delay = 0;
    int mask = 0b00000001;
    char player = 0b00000011;
    char player_now = player << player_row;
    // make some delay before 
    for(int i=0;i<84;i++){ 
        if(player_col + 2 > 81){
            state = 6;
            player_row = 0;
            player_col = 0;
        } 
        char print = game_map[i];
      
        if(i >= player_col && i <= player_col + 1){
            if(print & player_now){
                state = 5;
                player_row = 0;
                player_col = 0;
            }       
            if(is_print > 4){
                print = print | player_now;
            }       
        }
        PORTD = print >> 1;
        PORTAbits.RA7 = print & mask;
     
        PORTDbits.RD7 = 1;
        PORTAbits.RA6 = 1;
        
        delay = 45;
        while(delay--);
        
        PORTA = 0;
        PORTD = 0;
        
        PORTDbits.RD7 = 1;
        PORTAbits.RA6 = 1;
        
        delay = 45;
        while(delay--);   
    }
    if(is_print > 4){
        is_print = 0;
    }
    is_print++;
}
void game_win(){
    int delay = 0;
    // start delay - to put the string in the middle of the fan
    delay = 1700;
    while(delay--);

    printChar('Y');
    delay_between_words();
    printChar('O');
    delay_between_words();
    printChar('U');
    delay_between_words();
    delay_between_words();
    printChar('W');
    delay_between_words();
    printChar('I');
    delay_between_words();
    printChar('N');
    LIGHT = 0; 
}

void game_over(){
    int delay = 0;
    // start delay - to put the string in the middle of the fan
    delay = 1300;
    while(delay--);

    printChar('G');
    delay_between_words();
    printChar('A');
    delay_between_words();
    printChar('M');
    delay_between_words();
    printChar('E');
    delay_between_words();
    delay_between_words();
    printChar('O');
    delay_between_words();
    printChar('V');
    delay_between_words();
    printChar('E');
    delay_between_words();
    printChar('R');
    LIGHT = 0; 
}

void game_pre_animation(){
    int delay = 0;
    LIGHT = 0;
    pic_flag = 1;
    //  delay used to put the string at the middle of the fan
    delay = 1300;
    while(delay--);
    printChar('L');
    printChar('O');
    printChar('A');
    printChar('D');
    printChar('I');
    printChar('N');
    printChar('G');
    printChar('.');
    if(pic_cycle <= 25){
        
        for(int i = 0;i < 9;i++){
            PORTD = walker[pic][i] >> 2;
            PORTA = (walker[pic][i] << 8) >> 2;
            delay = 45;
            while(delay--);
            PORTD = 0;
            PORTA = 0;
            delay = 45;
            while(delay--);
        }

        if(pic_count== 1){
            if(pic < 6){
                pic++;
            }
            else {
                pic = 0;
            }
            pic_count = 0;
        }
        else {
            pic_count++;
        }
    }
    else{
        state = 3;
        pic_cycle = 0;
        pic_flag = 0;
    }
      
}

void print_word(){
    int delay = 0;
     // start delay - to put the string in the middle of the fan
    delay = 1300;
    while(delay--);

    printChar('C');
    delay_between_words();
    printChar('S');
    delay_between_words();
    printChar('I');
    delay_between_words();
    printChar('E');
    delay_between_words();
    delay_between_words();
    printChar('X');
    delay_between_words();
    printChar('D');
    delay_between_words();
    printChar('D');
    delay_between_words();
    printChar('D');
    LIGHT = 0; 
}




void printChar(char letter){
    //digital_counter++;
    for(int n = 0;n<8;n++){
        if(letter == 'A') PORTD = letter_A[n];
        else if(letter == 'B') PORTD = letter_B[n];
        else if(letter == 'C') PORTD = letter_C[n];
        else if(letter == 'D') PORTD = letter_D[n];
        else if(letter == 'E') PORTD = letter_E[n];
        else if(letter == 'F') PORTD = letter_F[n];
        else if(letter == 'G') PORTD = letter_G[n];
        else if(letter == 'H') PORTD = letter_H[n];
        else if(letter == 'I') PORTD = letter_I[n];
        else if(letter == 'J') PORTD = letter_J[n];
        else if(letter == 'K') PORTD = letter_K[n];
        else if(letter == 'L') PORTD = letter_L[n];
        else if(letter == 'M') PORTD = letter_M[n];
        else if(letter == 'N') PORTD = letter_N[n];
        else if(letter == 'O') PORTD = letter_O[n];
        else if(letter == 'P') PORTD = letter_P[n];
        else if(letter == 'Q') PORTD = letter_Q[n];
        else if(letter == 'R') PORTD = letter_R[n];
        else if(letter == 'S') PORTD = letter_S[n];
        else if(letter == 'T') PORTD = letter_T[n];
        else if(letter == 'U') PORTD = letter_U[n];
        else if(letter == 'V') PORTD = letter_V[n];
        else if(letter == 'W') PORTD = letter_W[n];
        else if(letter == 'X') PORTD = letter_X[n];
        else if(letter == 'Y') PORTD = letter_Y[n];
        else if(letter == 'Z') PORTD = letter_Z[n];
        else if(letter == '.') PORTD = letter_dot[n];
        int delay = 35;
        while(delay--);
        PORTD = 0;
        delay = 35;
        while(delay--);
    }
    
}

void __interrupt(high_priority) Hi_ISR(void)
{
    
    if(CCP1IF){
        if(is_rpm_count == 1){
            round_time = CCPR1;
            TMR1 = 0;
        }
        LIGHT = 1;
        CCP1IF = 0;
    }
    
    if(INT0IF){
        PORTD = 0;
        PORTA = 0;
        int temp = IR_recv();
        
        if(state==0){//0 menu
            if(temp == 0x68){
                state=1;
            }else if(temp == 0x98){ // RPM mode
                state=2;
                is_rpm_count = 1;
            }else if(temp == 0xB0){
                state = 3;
            }else if(temp == 0x30){
                state = 4; //game_pre_animation
            }
            
        
        }else if(state==1){//1 timeMode
            if(temp==0x62){
                hours++;
                if(hours==24) hours=0;
            }else if(temp==0xC2){
                minutes++;
                if(minutes==60) minutes=0;
            }else if(temp==0xA8){
                seconds++;
                if(seconds==60) seconds=0;
            }else if(temp==0x52){
                state=0;
            }
        }else if(state==2){//RPM Mode
            if(temp==0x52){
                state=0;
                is_rpm_count = 0;
            }
        }else if(state == 3){ // game mode
            if(temp == 0x52){
                state = 0;
            }
            else if(temp == 0x62){
                if(player_row < 6){
                    player_row ++;
                }
            }
            else if(temp == 0xA8){
                if(player_row > 0){
                    player_row --;
                }
            }
            else if(temp == 0x22){
                if(player_col > 0){
                    player_col--;
                }
            }
            else if(temp == 0xC2){
                if(player_col < 86){
                    player_col++;
                }
            }
            else if(temp == 0x38){
                player_col = 79;
            }
        }else if(state ==4){            // loading state
            if(temp == 0x52){
                state = 0;
            }
        }else if(state ==5){            // Game over state
            if(temp == 0x52){
                state = 0;
            }
        }else if(state ==6){            // You win state
            if(temp == 0x52){
                state = 0;
            }
        }
        INT0IF = 0;
        
        
    }
    
    if(TMR3IE && TMR3IF) {  //timer3 for time
        TMR3= 15535;
        TMR3IF=0;
        time_count++;
        if(pic_flag){
            pic_cycle++;
        }
        if(time_count == 5) { // (value/5) seconds
            time_count = 0;
            update_time();
        }
  
    }  
    
    return;
}

void update_time()
{
    if(++seconds == 60) {
        seconds = 0;
        if(++minutes==60) {
            minutes = 0;
            if(++hours == 24)
                hours = 0;
        }
    }
}


void time_mode()
{
    int delay = 0;
    //T3CONbits.TMR3ON=1;
    state=1;//1 for time mode
    LIGHT = 0;
    //Fan
    //  delay used to put the string at the middle of the fan
    delay = 1800;
    while(delay--);
    
    printNumber(hours/10);
    printNumber(hours%10);
    printNumber(10);
    printNumber(minutes/10);
    printNumber(minutes%10);
    printNumber(10);
    printNumber(seconds/10);
    printNumber(seconds%10);
}



void speed_mode(){
    
    float temp = (60000 / (float)(round_time << 2)) * 1000;
    unsigned int rpm = (unsigned int)temp;
    
    int delay = 1500;
    while(delay--);
    printChar('R');
    delay_between_words();
    printChar('P');
    delay_between_words();
    printChar('M');
    delay_between_words();
   
    printNumber(rpm / 1000);
    rpm = rpm % 1000;
    printNumber(rpm / 100);
    rpm = rpm % 100;
    printNumber(rpm / 10);
    rpm = rpm % 10;
    printNumber(rpm);

    
    LIGHT = 0;
    
}