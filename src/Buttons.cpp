#include <Buttons.h>

Button::Button(int pin, unsigned long delay)
    pin(pin),
    state(false),
    lastState(false),
    last_time(0),
    delay(delay)
{
    pinMode(pin, INPUT);
}

bool Button::rawread() const{
            return digitalRead(pin);
}

bool Button::steadyRead(){
    state = rawread();
    if(state != lastState){     // if state changes
        last_time = millis();   // note the time
    }

    if(millis() - last_time < delay){ // If not steady yet ret last state
        return lastState;
    }
    else{ // If steady, return the new state and save
        lastState = state;
        return state;
    }
}

void Button::setDelay(const unsigned long newdelay){
    delay = newdelay;
}
