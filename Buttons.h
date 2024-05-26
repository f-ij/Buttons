#ifndef Buttons
#define Buttons

#include <stdio.h>
#include <Arduino.h>
#include <circle_buf.h>

enum SteadyState{
    PRESSED,
    RELEASED
};

enum PressType{
    INDETERM,
    SINGLE,
    DOUBLE,
    TRIPLE,
    SINGLE_HOLD,
    DOUBLE_HOLD,
    TRIPLE_HOLD
};

class Button{
    private:
        int pin;                    // Pin number of the button
        int steadyState;            // Steady State of the button
        int lastRawState;           // Last state of the button
        unsigned long lastRawTime;  // Time of last change
        unsigned long steadyTime;   // Time when the state became steady
        unsigned long jitterDelay;  // Jitter delay
        bool stateRead; // To check if some state has already been read
                        // To prevent multiple reads of the same state
        int activeState;            // Active state of the button, HIGH or LOW
        unsigned long pressWindow;  // Time window to check for another press
        unsigned long holdWindow;   // Time window for which the button
                                    // is considered held
        unsigned long holdTime;     // Time for which the button has been held
        
        int pressEvents; // How many press events have been recorded before
        

    public:
        Button(int pin, unsigned long jitterDelay = 20, int activeState = LOW, unsigned long pressWindow = 500, unsigned long holdWindow = 1000);
        // TODO: Move these to private
        int rawRead() const;
        void steadyRead();
        //
        int getState() const;
        void setDelay(const unsigned long newdelay);
        bool isInactive() const;
        bool isActive() const;
        bool isWaiting() const;
        bool previousWaitingState() const;
        bool wasHeld() const;

        PressType monitorPress();
        
        
};

#endif
