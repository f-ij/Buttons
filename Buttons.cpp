#include <Buttons.h>

Button::Button(int pin, unsigned long jitterDelay, int activeState, unsigned long pressWindow, unsigned long holdWindow):
    pin(pin),
    steadyState(HIGH),
    lastRawState(HIGH),
    lastRawTime(0),
    steadyTime(0),
    jitterDelay(jitterDelay),
    stateRead(false),
    activeState(LOW),
    pressWindow(500),
    holdWindow(1000),
    holdTime(0),
    pressEvents(0)
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
        steadyState = readstate;
        stateRead = false;
        steadyTime = millis();
    }
    // Serial.println(millis() - lastRawTime);
    // If the state has been steady for long enough
    // Then we update the new state and note down the time
    
    // return steadyState;
};

int Button::getState() const{
    return steadyState;
};

void Button::setDelay(const unsigned long newdelay){
    jitterDelay = newdelay;
};

bool Button::isInactive() const{
    return getState() == !activeState && pressEvents == 0;
}

bool Button::isActive() const{
    return getState() == activeState;
}

bool Button::isWaiting() const{
    return pressEvents != 0;
}

bool Button::previousWaitingState() const{
    return pressEvents % 2 != 0;
}

bool Button::wasHeld() const{
    return holdTime != 0;
}

PressType Button::monitorPress(){
    steadyRead();

    if (isInactive()) { // If button is inactive
        return INDETERM;
    }

    if (!isWaiting() && isActive()){ // First press after inactivity
        pressEvents = 1;
        return INDETERM;
    }

    if (isWaiting()){
        if (isActive()){ // Is waiting and now active
            if(previousWaitingState()){ // Case: Is active was active
                this_time = millis();
                if (this_time - steadyTime > holdWindow){
                    PressType ptype = SINGLE_HOLD;
                    ptype += pressEvents/2 + (pressEvents % 2 != 0) - 1; // 
                    holdTime = this_time;
                    return ptype;
                }
                else{
                    return INDETERM;
                }
            }
            else{ // Case: Is active was inactive
                    pressEvents++;
                    return INDETERM;
            }
        }
        else{ // Is waiting, and currently inactive
            if (previousWaitingState()){ // Case: Is inactive was active
                holdTime = 0;

                if (pressEvents == 5){ // If already triple press, imediately
                    pressEvents = 0;
                    return TRIPLE;      // determine triple press
                }

                if (wasHeld()){ // If it was held, we're done
                    pressEvents = 0;
                    return INDETERM;
                }

                pressEvents++;
                return INDETERM;
            }
            else{ // Case: Is inactive was inactive
                this_time = millis();
                if (this_time - steadyTime > pressWindow){ // Press window passed
                    PressType ptype = SINGLE;
                    ptype += pressEvents/2 - 1; // 
                    pressEvents = 0;
                    return ptype;
                }
                else{
                    return INDETERM;
                }
            }
        }
    }
    return INDETERM;
};
