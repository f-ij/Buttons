#ifndef Buttons
#define Buttons

#include <stdio.h>
#include <Arduino.h>

enum SteadyState{
    PRESSED,
    RELEASED
};

enum PressType{
    INDETERM,
    PRESS,
    HOLD
};

class PressEvent{
    PressType type;
    int amount;
    unsigned long time;
    bool triggd;
    

    public:
        PressEvent(PressType t = INDETERM, int a = 0, unsigned long time = 0 , bool triggd = false);
        void settype(PressType t);              // Set the type of the event
        void setamount(int a);                  // Set the amount of the event
        void settime(unsigned long t);          // Set the time of the event
        void settriggd(bool t);                 // Set the event as triggered

        bool istriggered() const;               // Check if the hold event 
                                                // has been triggered
        

        bool isindeterm() const;                // Check if the event is 
                                                // indeterminate
        int ispress(PressType t) const; // Check if the event is of type
                                                // t and amount a
        void retrig(unsigned long time);        // Retrigger the after a delay

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
        int maxPresses;             // Maximum number of consecutive presses
                                    // that can be registered
        
        int stableEvents; // How many press events have been recorded before
        PressEvent pevent; // Last event recorded
        

    public:
        Button(int pin, unsigned long jitterDelay = 15, int activeState = LOW, unsigned long pressWindow = 200, unsigned long holdWindow = 300, int maxPresses = 3);
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

        int press() const;    // Check if button is pressed num times
        int holdTriggered() const;      // Check if button is held, but only once
        int held() const;               // Check if the button is held
        void retrig(unsigned long delay); // Set the time of the press event
                                        // to a future time so that it will trigger
                                        // again later

        void monitorPress();
        
        
};

#endif
