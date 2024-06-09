#ifndef Buttons
#define Buttons

#include <stdio.h>
#include <Arduino.h>
#include <type_traits>

enum ButtonType{
    ONLYPRESS,
    ONLYHOLD,
    BOTH
};

enum PressType{
    INDETERM,
    PRESS,
    HOLD
};

// Class to track the events of the button
class PressEvent{
    PressType type;
    int amount;
    unsigned long time;
    unsigned long waittime;
    int triggd;
    

    public:
        PressEvent(PressType t = INDETERM, int a = 0, unsigned long time = 0 , bool triggd = false);
        unsigned long eventtime() const;           // Get the time of the event
        int triggered() const;                 // Get the triggered 
                                                // status of the event
        

        void settype(PressType t);              // Set the type of the event
        void setamount(int a);                  // Set the amount of the event
        void settime(unsigned long t);          // Set the time of the event
        void settriggd(int t);                 // Set the event as triggered
        void sethold(int amount, unsigned long time); // Set the event as hold

        bool istriggered() const;               // Check if the hold event 
                                                // has been triggered
        

        bool isindeterm() const;                // Check if the event is 
                                                // indeterminate
        int ispress(PressType t) const; // Check if the event is of type
                                                // t and amount a
};

#include <Buttons.h>

PressEvent::PressEvent(PressType t, 
                        int a , 
                        unsigned long time, 
                        bool triggd):
            type(t),
            amount(a),
            time(time),
            triggd(triggd)
        {};

unsigned long PressEvent::eventtime() const{
    return time;
};

int PressEvent::triggered() const{
    return triggd;
};

int PressEvent::ispress(PressType t) const{
    if (type == t){
        return amount;
    }
    return 0;
};

void PressEvent::settype(PressType t){
    if ( t != type){
        triggd = false;
        type = t;
        time = millis();
    };
};

void PressEvent::setamount(int a){
    amount = a;
};

void PressEvent::settime(unsigned long t){
    time = t;
};

void PressEvent::settriggd(int t){
    triggd = t;
};

void PressEvent::sethold(int amount, unsigned long time){
    if (type != HOLD){
        settype(HOLD);
        setamount(amount);
        settime(time);
    }
};

bool PressEvent::istriggered() const{
    return triggd == 2;
};

bool PressEvent::isindeterm() const{
    return type == INDETERM;
};


template <ButtonType T = BOTH>
class Button{
    private:
        int pin;                    // Pin number of the button
        int steadyState;            // Steady State of the button
        int lastRawState;           // Last state of the button
        unsigned long lastRawTime;  // Time of last change
        unsigned long steadyTime;   // Time when the state became steady
        unsigned long steadyDelay;  // Jitter delay
        bool stateRead; // To check if some state has already been read
                        // To prevent multiple reads of the same state
        int activeState;            // Active state of the button, HIGH or LOW
        unsigned long pressWindow;  // Time window to check for another press
        unsigned long holdWindow;   // Time window for which the button
                                    // is considered held
        unsigned long holdTime;     // Time for which the button has been held
        int maxPresses;             // Maximum number of consecutive presses
                                    // that can be registered

        unsigned long retrigTime;   // Time to retrigger the button
        unsigned long manualRetrigTime; // Time to manually retrigger the button
        
        int stableEvents; // How many press events have been recorded before
        PressEvent pevent; // Last event recorded
        

    public:
        Button(int pin, int maxPresses = 3, unsigned long retrigTime = 500, 
                unsigned long steadyDelay = 7, int activeState = LOW, 
                unsigned long pressWindow = 225, unsigned long holdWindow = 300);
        
        void pullUpMode() const { pinMode(pin, INPUT_PULLUP);};
   
        int rawRead() const;
        void steadyRead();

        int getState() const;

        // Setters
        void setSteadyDelay(const unsigned long newdelay);
        void setPressWindow(const unsigned long newwindow);
        void setHoldWindow(const unsigned long newwindow);
        void setMaxPresses(const int newmax);
        void setRetrigTime(const unsigned long newtime);
        void setActiveState(const int newstate);


        bool isInactive() const;
        bool isActive() const;
        bool isWaiting() const;
        bool previousWaitingState() const;

        template <ButtonType U = T>
        typename std::enable_if<U == ONLYHOLD || U == BOTH, bool>::type
        wasHeld() const;

        bool nopress() const;        // Check if the button is not pressed

        template <ButtonType U = T> // Don't compile 
        typename std::enable_if<U == ONLYPRESS || U == BOTH, int>::type
        press() const;          // Check if button is pressed num times

        template <ButtonType U = T>
        typename std::enable_if<U == ONLYHOLD || U == BOTH, int>::type
        holdSingle();        // Check if button is held, but only once

        template <ButtonType U = T>
        typename std::enable_if<U == ONLYHOLD || U == BOTH, int>::type
        holdRepeat();        // Check if button is held, but repeated

        template <ButtonType U = T>
        typename std::enable_if<U == ONLYHOLD || U == BOTH, int>::type
        held() const;               // Check if the button is held
   
        void monitorPress();

        void showEvent() const;
        
        
};
template <ButtonType T>
Button<T>::Button(int pin, int maxPresses, 
                    unsigned long retrigTime, unsigned long steadyDelay, 
                    int activeState, unsigned long pressWindow, 
                    unsigned long holdWindow):
    pin(pin),
    steadyState(HIGH),
    lastRawState(HIGH),
    lastRawTime(0),
    steadyTime(0),
    steadyDelay(steadyDelay),
    stateRead(false),
    activeState(LOW),
    pressWindow(pressWindow),
    holdWindow(holdWindow),
    holdTime(0),
    maxPresses(maxPresses),
    retrigTime(retrigTime),
    manualRetrigTime(0),
    stableEvents(0),
    pevent(PressEvent())
{
    pinMode(pin, INPUT); // Set the pin as input
};

template <ButtonType T>
int Button<T>::rawRead() const{
    return digitalRead(pin);
};

template <ButtonType T>
void Button<T>::steadyRead(){
    int readstate = rawRead();
    if(readstate != lastRawState){     // if state changes
        lastRawState = readstate;      // save the new state
        lastRawTime = millis();   // note the time
    }
    else if (millis() - lastRawTime > steadyDelay){
        if (readstate != steadyState){ // If the state has been steady for long enough
            steadyState = readstate;
            stateRead = false;
            steadyTime = millis();
        }
    }
};

template <ButtonType T>
int Button<T>::getState() const{
    return steadyState;
};

template <ButtonType T>
void Button<T>::setSteadyDelay(const unsigned long newdelay){
    steadyDelay = newdelay;
};

template <ButtonType T>
void Button<T>::setPressWindow(const unsigned long newwindow){
    pressWindow = newwindow;
};

template <ButtonType T>
void Button<T>::setHoldWindow(const unsigned long newwindow){
    holdWindow = newwindow;
};

template <ButtonType T> 
void Button<T>::setMaxPresses(const int newmax){
    maxPresses = newmax;
};

template <ButtonType T> 
void Button<T>::setRetrigTime(const unsigned long newtime){
    retrigTime = newtime;
};

template <ButtonType T>
void Button<T>::setActiveState(const int newstate){
    activeState = newstate;
};

template <ButtonType T>
bool Button<T>::isInactive() const{
    return getState() == !activeState && stableEvents == 0;
}

template <ButtonType T>
bool Button<T>::isActive() const{
    return getState() == activeState;
};

template <ButtonType T>
bool Button<T>::isWaiting() const{
    return stableEvents != 0;
};

template <ButtonType T>
bool Button<T>::previousWaitingState() const{
    return stableEvents % 2 != 0;
};

template <ButtonType T>
template <ButtonType U>
typename std::enable_if<U == ONLYHOLD || U == BOTH, bool>::type
Button<T>::wasHeld() const{
    // return holdTime != 0;
    return pevent.ispress(HOLD);
};

template <ButtonType T>
bool Button<T>::nopress() const{
    return pevent.isindeterm();
};

template <ButtonType T>
template <ButtonType U>
typename std::enable_if<U == ONLYPRESS || U == BOTH, int>::type
Button<T>::press() const{
    return pevent.ispress(PRESS);
};

// This works as follows:
// If the button is held,
// Monitorbutton will only have one cycle where triggered is 1 before setting 
// it to 2. When it is one it will register a hold. 2 is the waiting state
// which will be set to 1 after the retrigger time has passed. 
template <ButtonType T>
template <ButtonType U>
typename std::enable_if<U == ONLYHOLD || U == BOTH, int>::type
Button<T>::holdSingle(){
    if (pevent.triggered() == 1)
    {
        manualRetrigTime = 4294967295;
        return pevent.ispress(HOLD);
    };

    return 0;
};

template <ButtonType T>
template <ButtonType U>
typename std::enable_if<U == ONLYHOLD || U == BOTH, int>::type
Button<T>::holdRepeat(){
    if (pevent.triggered() == 1)
    {
        return pevent.ispress(HOLD);
    };

    return 0;
};

template <ButtonType T>
template <ButtonType U>
typename std::enable_if<U == ONLYHOLD || U == BOTH, int>::type
Button<T>::held() const{
    return pevent.ispress(HOLD);
};


template <ButtonType T>
void Button<T>::monitorPress(){
    steadyRead();

    if (isInactive()) { // If button is inactive
        goto indeterm_event;
    }

    if (!isWaiting() && isActive()){ // First press after inactivity
        stableEvents = 1;
        goto indeterm_event;
    }

    if (isWaiting()){
        if (isActive()){ // Is waiting and now active
            if(previousWaitingState()){ // Case: Is active was active
                
                if (T == ONLYHOLD || T == BOTH){
                    // If it was still active, check if it is a hold
                    unsigned long this_time = millis();
                    if (this_time - steadyTime > holdWindow ){ 
                                                        // If long enough, it's held
                                                        // and it was not already
                                                        // triggered    
                        if (pevent.triggered() == 0){       // If not already triggered
                            int pamount = stableEvents/2 + 1 ; //
                            pevent.sethold(pamount, this_time);
                            pevent.settriggd(1);
                            // holdTime = this_time; // is this still neccesary?
                        }
                        else if (pevent.triggered() == 1){ // If already triggered
                            pevent.settriggd(2);
                            pevent.settime(millis());  
                        }
                        else{ // pevent.triggered() == 2
                            if (manualRetrigTime == 0){ // If not manually retriggered
                                                        // just wait for the retrigger time
                                if (millis() - pevent.eventtime() > retrigTime)
                                pevent.settriggd(1);
                            }
                            else{   // If manually retriggered use that time once   
                                if (millis() - pevent.eventtime() > manualRetrigTime){
                                    manualRetrigTime = 0;
                                    pevent.settriggd(1);
                                }
                            }
                        }

                        return;
                    }
                    else{
                        goto indeterm_event;
                    }
                }
                else{ // If hold doesn't need to be checked, just wait when held
                    goto indeterm_event;
                }
                
            }
            else{ // Case: Is active was inactive
                    stableEvents++;
                    goto indeterm_event;
            }
        }
        else{ // Is waiting, and currently inactive
            if (previousWaitingState()){ // Case: Is inactive was active
                
                if constexpr (T == ONLYHOLD || T == BOTH){
                    if (wasHeld()){ // If it was held, we're done
                        manualRetrigTime = 0;
                        stableEvents = 0;
                        goto indeterm_event;
                    }
                }
                
                // holdTime = 0;
                if (T == ONLYPRESS || T == BOTH){
                    if (stableEvents == maxPresses*2-1){ // If max presses reached
                                                        // Immediately terminate
                        pevent.settype(PRESS);
                        pevent.setamount(maxPresses);
                        stableEvents = 0;
                        return;
                    }
                }
                else{
                    if (stableEvents == maxPresses*2-1){ // If max presses reached
                                                        // Immediately terminate
                        stableEvents = 0;
                        goto indeterm_event;
                    }
                }

                stableEvents++;
                goto indeterm_event;
            }
            else{ // Case: Is inactive was inactive
                if  (T == ONLYPRESS || T == BOTH){
                    unsigned long this_time = millis();
                    if (this_time - steadyTime > pressWindow){ // Press window passed
                        int pamount = stableEvents/2;
                        pevent.settype(PRESS);
                        pevent.setamount(pamount);
                        stableEvents = 0;
                        return;
                    }
                }
                else{
                    unsigned long this_time = millis();
                    if (this_time - steadyTime > pressWindow){ // Press window passed
                        stableEvents = 0;
                    }
                }
                
                // ELSE
                goto indeterm_event;
                
            }
        }
    }

    return;
    indeterm_event:{
        if (!pevent.isindeterm()){
            pevent.settype(INDETERM);
            pevent.settime(millis());   
            pevent.settriggd(0);
        }
        return;
    }
};


template <ButtonType T>
void Button<T>::showEvent() const{
    if (pevent.isindeterm()){
        Serial.println("Indeterminate");
    }
    else if (pevent.ispress(PRESS)){
        Serial.print("Press: ");
        Serial.println(pevent.ispress(PRESS));
    }
    else if (pevent.ispress(HOLD)){
        Serial.print("Hold: ");
        Serial.println(pevent.ispress(HOLD));
    }
};

#endif
