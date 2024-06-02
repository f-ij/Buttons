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

void settype(PressType t){
    type = t;
};

int PressEvent::ispress(PressType t) const{
    if (type == t && amount == a){
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

void PressEvent::settriggd(bool t){
    triggd = t;
};

bool PressEvent::istriggered() const{
    return triggd;
};

bool PressEvent::isindeterm() const{
    return type == INDETERM;
};

void PressEvent::retrig(unsigned long delay){
    triggd = false;
    time = millis() + delay;
};

Button::Button(int pin, unsigned long jitterDelay, int activeState, unsigned long pressWindow, unsigned long holdWindow, int maxPresses):
    pin(pin),
    steadyState(HIGH),
    lastRawState(HIGH),
    lastRawTime(0),
    steadyTime(0),
    jitterDelay(jitterDelay),
    stateRead(false),
    activeState(LOW),
    pressWindow(pressWindow),
    holdWindow(holdWindow),
    holdTime(0),
    maxPresses(maxPresses),
    stableEvents(0),
    pevent(PressEvent())
{
    pinMode(pin, INPUT); // Set the pin as input
};

int Button::rawRead() const{
    return digitalRead(pin);
};

void Button::steadyRead(){
    int readstate = rawRead();
    if(readstate != lastRawState){     // if state changes
        lastRawState = readstate;      // save the new state
        lastRawTime = millis();   // note the time
    }
    else if (millis() - lastRawTime > jitterDelay){
        if (readstate != steadyState){ // If the state has been steady for long enough
            steadyState = readstate;
            stateRead = false;
            steadyTime = millis();
        }
    }
};

int Button::getState() const{
    return steadyState;
};

void Button::setDelay(const unsigned long newdelay){
    jitterDelay = newdelay;
};

bool Button::isInactive() const{
    return getState() == !activeState && stableEvents == 0;
}

bool Button::isActive() const{
    return getState() == activeState;
};

bool Button::isWaiting() const{
    return stableEvents != 0;
};

bool Button::previousWaitingState() const{
    return stableEvents % 2 != 0;
};

bool Button::wasHeld() const{
    return holdTime != 0;
};

int Button::press() const{
    return pevent.ispress(PRESS);
};

int Button::holdTriggered() const{
    auto holdnum = pevent.ispress(HOLD)
    if (holdnum != 0 && !pevent.triggd && millis() >= pevent.time)
    {
        pevent.settriggd(true);
        return holdnum;
    }

    return 0
};

int Button:held() const{
    return pevent.ispress(HOLD);
};


void Button::retrig(unsigned long delay){
    pevent.retrig(delay);
};

void Button::monitorPress(){
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
                unsigned long this_time = millis();
                if (this_time - steadyTime > holdWindow 
                        && !(pevent.istriggered())){ // If long enough, it's held
                                                    // and it was not already
                                                    // triggered    
                    int pamount = stableEvents/2 + 1 ; //
                    pevent.settype(HOLD);
                    pevent.setamount(pamount);
                    pevent.settime(this_time);
    
                    holdTime = this_time; // is this still neccesary?
                }
                else{
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
                

                if (wasHeld()){ // If it was held, we're done
                    stableEvents = 0;
                    holdTime = 0;
                    goto indeterm_event;
                }

                holdTime = 0;

                if (stableEvents == maxPresses*2-1){ // If max presses reached
                                                    // Immediately terminate
                    pevent.settype(PRESS);
                    pevent.setamount(maxPresses);
                    stableEvents = 0;
                }

                stableEvents++;
                goto indeterm_event;
            }
            else{ // Case: Is inactive was inactive
                unsigned long this_time = millis();
                if (this_time - steadyTime > pressWindow){ // Press window passed
                    int pamount = stableEvents/2;
                    pevent.settype(PRESS);
                    pevent.setamount(pamount);
                    stableEvents = 0;
                    // return pevent;
                }
                else{
                    goto indeterm_event;
                }
            }
        }
    }
    indeterm_event:{
        if (!pevent.isindeterm()){
            pevent.settype(INDETERM);
            pevent.settime(millis());   
            pevent.settriggd(false);
        }
    }
};s
