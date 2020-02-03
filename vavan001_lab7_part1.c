
/* Author: Vik Avantsa
 * Partner(s) Name: Tzin Lo
 * Lab Section :23
 * Assignment: Lab #7 Exercise #1
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example code,is my own original work.
 */

#include <avr/io.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <avr/interrupt.h>


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. Cprogrammer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.

 unsigned long _avr_timer_M = 1; // Start count from here, down to 0.Default 1 ms.
 unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
  TCCR1B = 0x0B;
        OCR1A = 125;
        TIMSK1 = 0x02;
        TCNT1=0;
        _avr_timer_cntcurr = _avr_timer_M;

 SREG |= 0x80; // 0x80: 1000000
}


void TimerOff() {
        TCCR1B = 0x00; 
}

void TimerISR() {
        TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, butrather TimerISR()

ISR(TIMER1_COMPA_vect) {
  _avr_timer_cntcurr--;
 if (_avr_timer_cntcurr == 0) {
TimerISR();
  _avr_timer_cntcurr = _avr_timer_M;
  }
}
void TimerSet(unsigned long M)
{
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

enum states {start,s0,s1,s2,winChk,wait,reset} state;
unsigned char num = 0x00;
unsigned char button = 0x00;

void tick(){
    switch(state){
        case start:
          PORTB = 0x00;
          state = s0;
          num = 5;
          LCD_ClearScreen();
          LCD_Cursor(1);
          LCD_WriteData(num + '0');
        break;

        case s0: // B0 lights up
          if(button==0x01)
            state = winChk;
          else
            state = s1;
        break;

        case s1:
          if(button==0x01)
            state = winChk;
          else
            state = s2;
        break;

        case s2:
          if(button==0x01)
            state = winChk;
          else
            state = s0;
        break;

        case winChk:
          state = wait;
        break;
        case wait:
           if(button==0x01)
             state = wait;
           else{
             state = s0;
             if(num==9)
             state = reset;
           }
        break;

        case reset:
            state = start;
        break;

        default:
           state = start;
        break;
     }

switch(state){
        case start:
        break;

        case s0:
         PORTB = 0x01;
        break;

        case s1:
         PORTB = 0x02;
        break;

        case s2:
         PORTB = 0x04;
        break;

        case winChk:
         if(PORTB == 0x02){
           ++num;
           if(num==9){
             LCD_DisplayString(1, "WINNER!!");
           }else{
             LCD_Cursor(1);
             LCD_WriteData(num+'0');
           }
         }else{
           --num;
           if(num<=0){
             num = 0;
            }
            LCD_Cursor(1);
            LCD_WriteData(num+'0');
         }
         break;

       case wait:
        break;

        case reset:
        break;

        default:
        break;
     }
}

int main(void) {
DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00; 

 LCD_init();
    TimerSet(500);
    TimerOn();
    while (1) {
        button = ~PINA&0x01;
        tick();
        while (!TimerFlag); 
  TimerFlag = 0;
    }
}


