# MicroSemaphore
Dirty semaphore to play with.

It's written for a ATMEGA168p but can be easily ported to any microcontroller of the same family.


```
 5v--- BTN-----> PIN4 (INT0)
        |
        150R
        |
        GND
        
 GND---150R---|<RED     ----> PIN11 (PD6)
 GND---150R---|<YELLOW  ----> PIN12 (PD7)
 GND---150R---|<GREEN   ----> PIN17 (PB0)
```

The microcontroller sleeps after a few sequences.
The button allows the user to change the sequence.
