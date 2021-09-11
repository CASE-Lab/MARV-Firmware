#include "unitMonitor.h"
#include "config.h"

UnitMonitor::UnitMonitor() :
_state (State::NOT_CONNECTED),
_source (Source::NONE){
    _timeout.start();
}


void UnitMonitor::poll(){
    //check for reccive timeout
    if(_timeout.elapsed_time() > Constant::HEARTBEAT_TIMEOUT){
        _state = State::NOT_CONNECTED;
        _timeout.reset();
    }
}

void UnitMonitor::setState(State state){
    _timeout.reset();
    _state = state;
}