#include "controller.h"
#include "config.h"

Controller::Controller(Heartbeat* heartbeat, AnalogIn* senseAuto, Indicator* led):
    _state (State::STANDBY),
    _senseAuto (senseAuto),
    _senseAutoFiltered (0),
    _refSource (RefSource::NONE),
    _heartbeat (heartbeat),
    _autoEn(Pin::RLY_AUTO_EN),
    _systemKill(Pin::SYS_KILL),
    _led (led){
        _autoEn = 0;    //start with 12V auto disabled
        _systemKill = 0;
        _filterTimer.start();
        led->setColor(&IndicatorColor::WHITE);
    }


void Controller::pollState(){
    float dT = 0.001*(uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(_filterTimer.elapsed_time()).count();
    _senseAutoFiltered += _filterTimeConstant*dT * (_senseAuto->read_voltage() - _senseAutoFiltered);
    _filterTimer.stop();
    _filterTimer.reset();
    _filterTimer.start();

    //poll all units:
    unitNCU.poll();
    unitRCU.poll();
    unitTCU.poll();
    unitACU.poll();

    //check for status OK and common ref source to enter enabled state:
    if(_state == State::STANDBY && _refSource == RefSource::ACU){
            bool ncuOk = unitNCU.getState() == UnitMonitor::State::OK;
            bool tcuOk = unitTCU.getState() == UnitMonitor::State::OK;
            bool acuOk = unitACU.getState() == UnitMonitor::State::OK;

            bool refAcuCommon = (unitNCU.getSource() == UnitMonitor::Source::ACU) && (unitTCU.getSource() == UnitMonitor::Source::ACU);
            
            if(ncuOk && tcuOk && acuOk && refAcuCommon){
                _setState(State::ENABLED);
                printf("12V Auto enabled with ACU as common controller\n\r");
            }
    }
    if(_state == State::STANDBY && _refSource == RefSource::RCU){
            bool ncuOk = unitNCU.getState() == UnitMonitor::State::OK;
            bool tcuOk = unitTCU.getState() == UnitMonitor::State::OK;
            bool rcuOk = unitRCU.getState() == UnitMonitor::State::OK;
            
            bool refRCUCommon = (unitNCU.getSource() == UnitMonitor::Source::RCU) && (unitTCU.getSource() == UnitMonitor::Source::RCU);
            
            if(ncuOk && tcuOk && rcuOk && refRCUCommon){
                _setState(State::ENABLED);
                printf("12V Auto enabled with RCU as common controller\n\r");
            }
    }

    //TODO: add case that ignores check if in testing mode

    bool _senseAutoRising = _senseAutoFiltered > (_senseAutoThreshold + _senseAutoHyst);
    if(_state == State::ENABLED && _senseAutoRising){
        _state = State::STARTING;
        debug("12V AUTO activated\n\r");

        //TODO: Add check that control source of TCU, NCU, PDU is the same

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

    //check for control signal reccive timeout while in standy (disabled when source is TESTING)
    if(timeout && _state == State::STANDBY && _refSource != RefSource::NONE){
        _refSource = RefSource::NONE;
        _heartbeat->setSource(Heartbeat::Source::NONE);
        printf("Control signal set to none\n\r");
    }

    //check for control signal reccive timeout while in enabled (disabled when source is TESTING)
    if(timeout && _state == State::ENABLED && _refSource != RefSource::NONE){
        _setState(State::STANDBY);
        _refSource = RefSource::NONE;
        _heartbeat->setSource(Heartbeat::Source::NONE);
        printf("Control signal set to none. State set to standby\n\r");
    }

    //check if relevant units are reporting okay state while running

    if(_state == State::RUNNING && _refSource == RefSource::ACU){
            bool ncuOk = unitNCU.getState() == UnitMonitor::State::OK;
            bool tcuOk = unitTCU.getState() == UnitMonitor::State::OK;
            bool acuOk = unitACU.getState() == UnitMonitor::State::OK;
            if(!(ncuOk && tcuOk && acuOk)){
                _setState(State::ERROR);
                printf("Controlling unit disconnected or in error. Entering error state\n\r");
            }
    }
    if(_state == State::RUNNING && _refSource == RefSource::RCU){
            bool ncuOk = unitNCU.getState() == UnitMonitor::State::OK;
            bool tcuOk = unitTCU.getState() == UnitMonitor::State::OK;
            bool rcuOk = unitRCU.getState() == UnitMonitor::State::OK;
            if(!(ncuOk && tcuOk && rcuOk)){
                _setState(State::ERROR);
                printf("Controlling unit disconnected or in error. Entering error state\n\r");
            }
    }

    //check if 12V auto rises without being enabled
    if(_state == State::STANDBY && _senseAutoRising){
        _setState(State::ERROR);
        printf("Critical error! 12V Auto on detected while not enabled. Entering error state\n\r");
    }
}


bool Controller::setRefSource(RefSource source){
    //consider adding bounds checking for targetAngle (+- 25deg?)

    if(_state == State::ERROR){
        return false;
    }

    //if no change in source reset timer
    if(_refSource == source){
        _refSourceTimeout.reset();
        return true;
    }
    //if test mode disable timeout
    else if(source == RefSource::TESTING){
        _refSourceTimeout.stop();
        _refSourceTimeout.reset();
        _refSource = source;
        return true;
    }
    //if there is no current source select source and restart timer
    else if (_refSource == RefSource::NONE)
    {
        _refSource = source;
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

void Controller::_setState(State state){
    switch (state)
    {
    case State::STANDBY:
        _state = state;
        _led->setColor(&IndicatorColor::WHITE);
        _autoEn = 0;
        break;
    case State::ENABLED:
        _state = state;
        _led->setColor(&IndicatorColor::GREEN);
        _autoEn = 1;
        break;
    case State::STARTING:
        _state = state;
        break;
    case State::RUNNING:
        _state = state;
        _led->setColor(&IndicatorColor::BLUE);
        break;
    case State::ERROR:
        _state = state;
        _autoEn = 0;    //turns off 12V auto if in errror state
        _heartbeat->setStatus(Heartbeat::Status::ERROR);
        _heartbeat->setSource(Heartbeat::Source::NONE);
        _led->setColor(&IndicatorColor::RED);
        break;
    
    default:
        break;
    }
}

bool Controller::getAuto12V(){
    if(_state == State::RUNNING){
        return true;
    }
    else{
        return false;
    }
}

void Controller::sysKill(){
    _autoEn = 0;
    _systemKill = 1;
}