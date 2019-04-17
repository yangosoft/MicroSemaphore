/*ORIGINAL E:F8, H:DC, L:E2
 * 1MHZ E:F8, H:DD, L:62
 128kHz -U lfuse:w:0xe3:m -U hfuse:w:0xdd:m -U efuse:w:0xf8:m
 */

#define F_CPU   1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 


#define LED _BV(PD6)     // = (1 << PB0)


#define LED_DDR DDRD
#define LED_PORT PORTD

volatile int disable = 0;


enum class STATE : int { SLEEP, NORMAL, BLINK, GROW, FIXED_RED, FIXED_GREEN, FIXED_YELLOW };

STATE state;
int counter = 0;
static constexpr int REPEAT_SEQUENCE_BEFORE_SLEEP = 4;

/*
 5v--- BTN-----> PIN4 (INT0)
        |
        150R
        |
        GND
        
 GND---150R---|<RED     ----> PIN11 (PD6)
 GND---150R---|<YELLOW  ----> PIN12 (PD7)
 GND---150R---|<GREEN   ----> PIN17 (PB0)

*/


void setupGPIO()
{
    DDRB = 0xFF; //1 output port
    PORTB = 0xFF; // 1 high
    
    DDRD &= ~(1<<PD2);
    PORTD |= (1<<PD2);
    
    
    DDRB  = 0b00001000;   // PB3 output
    DDRD  = 0b01100000;   // PD5 and PD6 outputs
    
    
    
    DDRD |= (1<<PD7);
    
    DDRB |= (1<<PB0);
    
    
    
}

void setRed(bool on)
{
    if(on)
    {
        PORTD |= _BV(PD6);
    }else
    {
        PORTD &= ~_BV(PD6);
    }
}

void setYellow(bool on)
{
    if(on)
    {
        PORTD |= _BV(PD7);
    }else
    {
        PORTD &= ~_BV(PD7);
    }
}

void setGreen(bool on)
{
    if(on)
    {
        PORTB |= _BV(PB0);
    }else
    {
        PORTB &= ~_BV(PB0);
    }
}



ISR (INT0_vect)
{
      EIMSK = 0;
      setRed(false);
      setYellow(false);
      setGreen(false);
    
      if(state == STATE::BLINK)
      {
          state = STATE::GROW;
      }else if(state == STATE::NORMAL)
      {
          state = STATE::FIXED_GREEN;
      }
      else if(state == STATE::FIXED_GREEN)
      {
          state = STATE::FIXED_YELLOW;
      }
      else if(state == STATE::FIXED_YELLOW)
      {
          state = STATE::FIXED_RED;
      }
      else if(state == STATE::FIXED_RED)
      {
          state = STATE::BLINK;
      }
      else if(state == STATE::GROW)
      {
          state = STATE::NORMAL;
      }
      _delay_ms(500);
}



void disableInt0()
{
     cli();
     PCMSK0 &= ~(1 << PCINT0); 
     EIMSK &= ~(1 << INT0);
}

void enableSleep()
{
    setRed(true);
    setYellow(true);
    setGreen(true);
    _delay_ms(2000);
    setRed(false);
    setYellow(false);
    setGreen(false);
    
    
    cli();
    PCMSK0 |= (1 << PCINT0); 
    EIMSK |= (1 << INT0);
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    
    sleep_enable();
    cli();
    sei();
    // actually sleep
    sleep_cpu();
    sleep_disable();
        
}

void normalSequence()
{
    
    setYellow(false);
    setGreen(false);
    setRed(true);
    _delay_ms(2500);
    if(state != STATE::NORMAL)
    {
        counter = 0;
        return;
    }
    
    setRed(false);
    setGreen(true);
    for(int i = 0; i < 7; i++)
    {
        _delay_ms(1000);
        if(state != STATE::NORMAL)
        {
            counter = 0;
            return;
        }
    }
    
    setYellow(true);
    setGreen(false);
    for(int i = 0; i < 3; i++)
    {
        _delay_ms(1000);
        if(state != STATE::NORMAL)
        {
            counter = 0;
            return;
        }
    }
    
    setYellow(false);
    setRed(true);
    _delay_ms(2500);
    if(state != STATE::NORMAL)
    {
        counter = 0;
        return;
    }
}

void blinkSequence()
{
    for(int i = 0; i < 10; i++)
    {
        setRed(false);
        setYellow(true);
        setGreen(false);
        _delay_ms(500);
         if(state != STATE::BLINK)
        {
            counter = 0;
            return;
        }
        
        setYellow(false);
        _delay_ms(500);
        
        if(state != STATE::BLINK)
        {
            counter = 0;
            return;
        }
            
    }
}

void growSequence()
{
    setRed(false);
    setYellow(false);
    setGreen(false);
        
        
    
    setRed(false);
    setYellow(false);
    setGreen(true);
    _delay_ms(500);
    if(state != STATE::GROW)
    {
        counter = 0;
        return;
    }
    
    setGreen(false);
    _delay_ms(500);
    
    if(state != STATE::GROW)
    {
        counter = 0;
        return;
    }
    
    setRed(false);
    setYellow(false);
    setGreen(true);
    
        
    for(int i = 0; i < 2; i++)
    {      
        setYellow(false);

        _delay_ms(500);
        if(state != STATE::GROW)
        {
            counter = 0;
            return;
        }
        
        setYellow(true);
        _delay_ms(500);
        
        if(state != STATE::GROW)
        {
            counter = 0;
            return;
        }
    }
    
    setRed(false);
    setYellow(true);
    setGreen(true);
    
    for(int i = 0; i < 3; i++)
    {      
        setRed(false);
        
        _delay_ms(500);
        if(state != STATE::GROW)
        {
            counter = 0;
            return;
        }
        
        setRed(true);
        _delay_ms(500);
        
        if(state != STATE::GROW)
        {
            counter = 0;
            return;
        }
    }
    setRed(true);
    setYellow(true);
    setGreen(true);
     _delay_ms(500);
        
    if(state != STATE::GROW)
    {
        counter = 0;
        return;
    }
        
    setRed(false);
    setYellow(false);
    setGreen(false);
    _delay_ms(500);
        
    if(state != STATE::GROW)
    {
         counter = 0;
         return;
    }
}

void wakeUpSequence()
{
    for(int i = 0;i < 2; ++i)
    {
        setYellow(false);
        setGreen(false);
        setRed(true);
        _delay_ms(100);
    
        setRed(false);
        setGreen(true);
        _delay_ms(100);
    
        setYellow(true);
        setGreen(false);
        _delay_ms(100);
        setYellow(false);
        setRed(true);
        _delay_ms(100);
    }
    
    setRed(false);
    setYellow(false);
    setGreen(false);
}

void fixedRedSequence()
{
    for(int i = 0;i < 50; ++i)
    {
        setYellow(false);
        setGreen(false);
        setRed(true);
        _delay_ms(100);
        if(state != STATE::FIXED_RED)
        {
            counter = 0;
            return;
        }
    }
}

void fixedGreenSequence()
{
    for(int i = 0;i < 50; ++i)
    {
        setYellow(false);
        setGreen(true);
        setRed(false);
        _delay_ms(100);
        if(state != STATE::FIXED_GREEN)
        {
            counter = 0;
            return;
        }
    }
}

void fixedYellowSequence()
{
    for(int i = 0;i < 50; ++i)
    {
        setYellow(true);
        setGreen(false);
        setRed(false);
        _delay_ms(100);
        if(state != STATE::FIXED_YELLOW)
        {
            counter = 0;
            return;
        }
    }
}


int main (void) 
{   
    setupGPIO();
   
    
    wakeUpSequence();
    state = STATE::SLEEP;
    
    
    while(1)
    {
        switch(state)
        {
            case STATE::NORMAL:
                normalSequence();
                counter++;
                break;
                
            case STATE::BLINK:
                blinkSequence();        
                counter++;
                break;
            
            case STATE::GROW:
                growSequence();        
                counter++;
                break;
                
            case STATE::FIXED_RED:
                fixedRedSequence();
                counter++;
                break;
                
            case STATE::FIXED_GREEN:
                fixedGreenSequence();
                counter++;
                break;
                
            case STATE::FIXED_YELLOW:
                fixedYellowSequence();
                counter++;
                break;
                
                
            case STATE::SLEEP:
                enableSleep();
                wakeUpSequence();
                counter = 0;
                state = STATE::NORMAL;
                break;
        }
        cli();
        PCMSK0 |= (1 << PCINT0); 
        EIMSK |= (1 << INT0);
        sei();
        
        if(counter == REPEAT_SEQUENCE_BEFORE_SLEEP)
        {
            state = STATE::SLEEP;
        }
    }
}
