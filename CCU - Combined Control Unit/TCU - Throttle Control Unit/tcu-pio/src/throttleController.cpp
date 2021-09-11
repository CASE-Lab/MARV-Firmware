#include "throttleController.h"
#include "config.h"

ThrottleController::ThrottleController(Heartbeat* heartbeat, AnalogIn* senseAuto) :
    _aps1In(Pin::APS1_IN),
    _apsOcIn(Pin::APS_OC_IN),
    _rpsIn(Pin::RPS_IN),
    _apsOut(Pin::APS_OUT),
    _ocOut(Pin::OC_OUT),
    _rpsOut(Pin::RPS_OUT),
    _heartbeat (heartbeat),
    _senseAuto (senseAuto){
    _speedUpdated = false;
    _state = State::STANDBY;
    _refSource = RefSource::NONE;
    _senseAutoFiltered = 0;
    _filterTimer.start();

    //init to zero throttle:
    _aps1Var = Throttle::apsMin;
    _fcOutVar = Throttle::apsClosed;
    _rpsVar = Throttle::rpsMin;
}

// set speed 0-100% according to input
void ThrottleController::_setSpeed(){
    //int multiplier1 = 0.01; // if input speed 1-100 (serial port)
    constexpr float multiplier2 = 0.0001; // if input speed 1-10000 (CAN Bus)
    //inputSpeed = (float) multiplier1*serialInVar; 
    
    float CANInVar = _canCommand[1]+(_canCommand[0]<<8);
    //printf("Set speed CANInVArAPS: %.6f\r", CANInVar);
    float inputSpeed =  multiplier2 * CANInVar;
    //printf("Set speed inputSpeedAPS: %.6f\r", inputSpeed);
    float playground = Throttle::apsMax - Throttle::apsMin; 
    //printf("set speed playground: %.6f\r", playground);
    _aps1Var = Throttle::apsMin + inputSpeed*playground;
    //printf("set speed out: %.6f\r", aps1Var);
    if(_aps1Var < Throttle::apsMin){
        _aps1Var = Throttle::apsMin; 
        //printf("input to small, APS set to min\r");
        }
    if(_aps1Var > Throttle::apsMax){
            _aps1Var=Throttle::apsMax; 
            //printf("input to big, APS set to max\r");
            }
    }

// Open/Closed control of aps
// Uses hysteresis control
void ThrottleController::_ocCtrl(){
    char state='C';
    float threshold = Throttle::apsThreshold;
    if(state=='C'){
        threshold=(Throttle::apsThreshold + Throttle::hyster);
        }
    else if(state=='O'){
        threshold=(Throttle::apsThreshold - Throttle::hyster);
        }
        else{
            threshold=Throttle::apsThreshold;
            }
    if(_aps1Var >= threshold) {
        _fcOutVar = Throttle::apsOpen;
        //printf("apsOpen\r");
        state='O';
        }
    else if(_aps1Var < threshold) {
        _fcOutVar = Throttle::apsClosed;
        //printf("apsClosed\r");
        state='C';
    }
    else{
        _fcOutVar = Throttle::apsClosed;
        //printf("ocCtrl ERROR -> apsClosed\r");
        state='C';
        }
}

//MUST have speedUpdated (flag) set to update main
//note: rps read in irq/CAN
void ThrottleController::_setRps(){
    //inputRps=rpsTest; 
    
    constexpr float multiplier2 = 0.0001; // if input speed 1-10000 (CAN Bus)
    //inputSpeed = (float) multiplier1*serialInVar; 
    
    float CANInVar = _canCommand[3] + (_canCommand[2]<<8);
    //printf("Set speed CANInVArRPS: %.6f\r", CANInVar);
    float inputRps =  multiplier2*CANInVar;
    //printf("Set speed inputSpeedRPS: %.6f\r", inputRps);
    
    float playground = Throttle::rpsMax - Throttle::rpsMin;
    _rpsVar = Throttle::rpsMin + inputRps * playground;

    if(_rpsVar > Throttle::rpsMax) {
        _rpsVar = Throttle::rpsMax;
        //printf("input to big, RPS set to max\r");
    }
    if(_rpsVar < Throttle::rpsMin) {
        _rpsVar = Throttle::rpsMin;
        //printf("input to small, RPS set to min\r");
    }
}

void ThrottleController::pollSpeedUpdate(){
    if(_speedUpdated==true){
        _setSpeed();
        _ocCtrl();
        _setRps();
        //printf("speedUpdated -> %d\n", serialInVar);
        _speedUpdated=false;
    }
}

bool ThrottleController::setThrottle(uint8_t* msgData, RefSource source){

    if(_state == State::ERROR){
        return false;
    }

    //if no change in source set target angle and reset timer
    if(_refSource == source){
        memcpy(_canCommand, msgData, sizeof(_canCommand));  //sets new can command value
        _speedUpdated = true;
        _refSourceTimeout.reset();
        return true;
    }
    //if test mode disable timeout and set target angle
    else if(source == RefSource::TESTING){
        _refSourceTimeout.stop();
        _refSourceTimeout.reset();
        memcpy(_canCommand, msgData, sizeof(_canCommand));  //sets new can command value
        _speedUpdated = true;
        _refSource = source;
        return true;
    }
    //if there is no current source select source and restart timer
    else if (_refSource == RefSource::NONE)
    {
        _refSource = source;
        memcpy(_canCommand, msgData, sizeof(_canCommand));  //sets new can command value
        _speedUpdated = true;
        _refSourceTimeout.stop();
        _refSourceTimeout.reset();
        _refSourceTimeout.start();

        switch (source)
        {
        case RefSource::ACU:
            printf("Control signal set to ACU\n\r");
            _heartbeat->setSource(Heartbeat::Source::ACU);
            break;
        
        case RefSource::RCU:
            printf("Control signal set to RCU\n\r");
            _heartbeat->setSource(Heartbeat::Source::RCU);
            break;

        case RefSource::TESTING:
            printf("Control signal set to TESTING\n\r");
            _heartbeat->setSource(Heartbeat::Source::TESTING);
            break;
        default:
            break;
        }

        return true;
    }
    //other source allready selected, set error state
    else{
        _setState(State::ERROR);
        printf("Other control signal already selected, enetering error state\n\r");
        return false;
    }
}

void ThrottleController::_setState(State state){
    switch (state)
    {
    case State::STANDBY:
        _state = state;
        break;
    case State::RUNNING:
        _state = state;
        break;
    case State::STARTING:
        _state = state;
        break;
    case State::ERROR:
        _state = state;
        _heartbeat->setStatus(Heartbeat::Status::ERROR);
        _heartbeat->setSource(Heartbeat::Source::NONE);
        break;
    
    default:
        break;
    }
}

void ThrottleController::pollState(){
    float dT = 0.001*(uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(_filterTimer.elapsed_time()).count();
    _senseAutoFiltered += _filterTimeConstant*dT * (_senseAuto->read_voltage() - _senseAutoFiltered);
    _filterTimer.stop();
    _filterTimer.reset();
    _filterTimer.start();

    bool _senseAutoRising = _senseAutoFiltered > (_senseAutoThreshold + _senseAutoHyst);
    if(_state == State::STANDBY && _senseAutoRising){
        _setState(State::STARTING);
        debug("12V AUTO activated\n\r");

        //check if there is no control source
        if(_refSource == RefSource::NONE){
            _setState(State::ERROR);
            printf("No control source selected. Entering error state\n\r");
        }
        else{
            _setState(State::RUNNING);
            printf("Startup succesful. Entering running state\n\r");
        }
    }

    if(_state == State::RUNNING && _senseAutoFiltered < (_senseAutoThreshold - _senseAutoHyst)){
        //go back to standby control with no control source selected
        _setState(State::STANDBY);
        _refSource = RefSource::NONE;
        _heartbeat->setSource(Heartbeat::Source::NONE);
        debug("12V AUTO deactivated\n\r");
    }

    bool timeout = _refSourceTimeout.elapsed_time() > Constant::RECIVE_TIMEOUT;
    //check for control signal reccive timeout while running (disabled when source is TESTING)
    if(timeout && _state == State::RUNNING){
        _setState(State::ERROR);
        printf("RX Timeout while running. Entering error state\n\r");
    }

    //check for control signal reccive timeout while running (disabled when source is TESTING)
    if(timeout && _state == State::STANDBY && _refSource != RefSource::NONE){
        _refSource = RefSource::NONE;
        _heartbeat->setSource(Heartbeat::Source::NONE);
        printf("Control signal set to none\n\r");
    }
}

bool ThrottleController::writeThrottle(){
    //guard if state is not running
    if(_state != State::RUNNING){
        //write zero throttle
        _aps1Var = Throttle::apsMin;
        _fcOutVar = Throttle::apsClosed;
        _rpsVar = Throttle::rpsMin;

        _apsOut.write(_aps1Var);
        _ocOut.write(_fcOutVar);
        _rpsOut.write(_rpsVar);
        return false;   //returns not set
    }

    _apsOut.write(_aps1Var * Throttle::OUT_CALIB);
    _ocOut.write(_fcOutVar * Throttle::OUT_CALIB);
    _rpsOut.write(_rpsVar * Throttle::OUT_CALIB);
    return true;
}