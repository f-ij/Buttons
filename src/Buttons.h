#ifndef Buttons
#define Buttons

#include <stdio.h>
#include <Arduino.h>

struct Button{
    private:
        int pin;
        bool state;
        bool lastState;
        unsigned long last_time;
        unsigned long delay;

    public:
        Button(int pin, unsigned long delay);

        bool rawread() const;

        bool steadyRead();
        
        void setDelay(const unsigned long newdelay);
}

#endif
