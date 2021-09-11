#include "controller.h"
#include "config.h"

Controller::Controller(Heartbeat* heartbeat, AngleSensor* encoder, Tmcm* driver, AnalogIn* senseAuto, TmcmCommand* initCmdList, uint8_t nrOfCmds):
    _encoder (encoder),
    _driver (driver),
    _state (State::STANDBY),
    _senseAuto (senseAuto),
    _initCmdList (initCmdList),
    _nrOfCmds (nrOfCmds),
    _senseAutoFiltered (0),
    _refSource (RefSource::NONE),
    _heartbeat (heartbeat){
        _filterTimer.start();
        _turnTest = false; //default value
        _driverVelocity = 0;
        _currentAngle = 0;
    }

bool checkFail(int result){
    return !(result == TmcmStatus::SUCCESS || result == TmcmStatus::EEPROM_LOAD_SUCCESS);
}

int Controller::configMotor(TmcmCommand* cmdArray, uint8_t nrOfCmds){
    debug("Runnig motor config\n\r");
    int result;
    for(uint8_t i = 0; i < nrOfCmds; i++){
        if (checkFail(result = _driver->sendRawCommandRetry(cmdArray+i))){
            printf("ERROR: Command number %u failed with code %u\n\r", i, result);        
            return result;
        }
    }
    return 0;
}

void Controller::pollState(){
    float dT = 0.001*(uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(_filterTimer.elapsed_time()).count();
    _senseAutoFiltered += _filterTimeConstant*dT * (_senseAuto->read_voltage() - _senseAutoFiltered);
    _filterTimer.stop();
    _filterTimer.reset();
    _filterTimer.start();

    bool _senseAutoRising = _senseAutoFiltered > (_senseAutoThreshold + _senseAutoHyst);
    if(_state == State::STANDBY && _senseAutoRising){
        _state = State::STARTING;
        debug("12V AUTO activated\n\r");

        //check if there is no control source
        if(_refSource == RefSource::NONE){
            setState(State::ERROR);
            printf("No control source selected. Entering error state\n\r");
        }
        //run config and check if it fails (return zero upon sucess, otherwise fail code)
        else if(configMotor(_initCmdList, _nrOfCmds)){
            setState(State::ERROR);
            printf("Motor configuration failed. Entering error state\n\r");
        }
        else{
            _state = State::RUNNING;
            printf("Motor configuration succesful. Entering running state\n\r");
            setTargetAngle(0, _refSource);  //sets initial target angle to zero
            if(_turnTest){
                printf("Performing right and then left turn\n\r");
                _driver->sendRawCommand(TmcmInstruction::ROR, TmcmType::NO_TYPE, _testVel);
                ThisThread::sleep_for(500ms);
                _driver->sendRawCommand(TmcmInstruction::ROL, TmcmType::NO_TYPE, _testVel);
                ThisThread::sleep_for(500ms);
                _driver->sendRawCommand(TmcmInstruction::MST, TmcmType::NO_TYPE, TmcmConfig::NO_VALUE);
            }
        }
    }

    if(_state == State::RUNNING && _senseAutoFiltered < (_senseAutoThreshold - _senseAutoHyst)){
        //go back to standby control with no control source selected
        setState(State::STANDBY);
        _refSource = RefSource::NONE;
        _heartbeat->setSource(Heartbeat::Source::NONE);
        debug("12V AUTO deactivated\n\r");
    }

    bool timeout = _refSourceTimeout.elapsed_time() > Constant::RECIVE_TIMEOUT;
    //check for control signal reccive timeout while running (disabled when source is TESTING)
    if(timeout && _state == State::RUNNING){
        setState(State::ERROR);
        printf("RX Timeout while running. Entering error state\n\r");
    }

    //check for control signal reccive timeout while running (disabled when source is TESTING)
    if(timeout && _state == State::STANDBY && _refSource != RefSource::NONE){
        _refSource = RefSource::NONE;
        _heartbeat->setSource(Heartbeat::Source::NONE);
        printf("Control signal set to none\n\r");
    }
}

void Controller::runController(){
    //start by polling state
    pollState();

    //if state is not running simply return
    if(_state != State::RUNNING){
        return;
    }

    _currentAngle = _encoder->getAngleMA();
    _angleError = _targetAngle - _currentAngle;

    _currentEncoderVelocity = _encoder->getHoloboro5();

    if( abs(_angleError) < Constant::ANGLE_DEADBAND){
        //_driver->sendRawCommand(TmcmInstruction::MST, TmcmType::NO_TYPE, TmcmConfig::NO_VALUE);
        cmdVelocity(0, Dir::RIGHT);
        return;
    }

    //find angle error direction
    Dir errorDirection;

    if(_angleError > 0){
        errorDirection = Dir::LEFT;
    }
    else{
        errorDirection = Dir::RIGHT;
    }

    //debug("Error signal: %d\n\r", (int)(_gainP * _angleError));

    //start with simple proportional velocity control
    uint32_t velCommand = (uint32_t) abs(_gainP * _angleError);

    cmdVelocity(velCommand, errorDirection);

}

void Controller::cmdVelocity(uint32_t velocity, Dir direction){
    if(direction == Dir::RIGHT){
        _driver->sendRawCommand(TmcmInstruction::ROR, TmcmType::NO_TYPE, velocity);
    }
    else if(direction == Dir::LEFT){
        _driver->sendRawCommand(TmcmInstruction::ROL, TmcmType::NO_TYPE, velocity);
    }
    else{
        debug("E: Incorrect velocity in Controller::cmdVelocity");
        return;
    }
}

int Controller::readDriverVelocity(){
    //guard check if motor is in running state
    if(_state != State::RUNNING){
        return -1;
    }

    int replyStatus = _driver->sendRawCommand(&TmcmCmd::GET_ACTUAL_SPEED);
    if(replyStatus == TmcmStatus::SUCCESS){
        //read was succesful
        _driverVelocity =  _driver->getReply();
        return 0;
    }
    else{
        debug("Vel read erroor\n\r");
        return -1;
    }
}

void Controller::updateAngle(){
    _currentAngle = _encoder->getAngleMA();
    _currentAngleRaw = _encoder->getAngle();
    _angleError = _targetAngle - _currentAngle;

    _currentEncoderVelocity = _encoder->getHoloboro5();
    
}

float Controller::readEncVolt(){
    return _encoder->readVoltage();
}

void Controller::pollAngle(){
    _encoder->pollSensor();
}

bool Controller::setTargetAngle(float targetAngle, RefSource source){
    //consider adding bounds checking for targetAngle (+- 25deg?)

    if(_state == State::ERROR){
        return false;
    }

    //if no change in source set target angle and reset timer
    if(_refSource == source){
        _targetAngle = targetAngle;
        _refSourceTimeout.reset();
        return true;
    }
    //if test mode disable timeout and set target angle
    else if(source == RefSource::TESTING){
        _refSourceTimeout.stop();
        _refSourceTimeout.reset();
        _targetAngle = targetAngle;
        _refSource = source;
        return true;
    }
    //if there is no current source select source and restart timer
    else if (_refSource == RefSource::NONE)
    {
        _refSource = source;
        _targetAngle = targetAngle;
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
        setState(State::ERROR);
        printf("Other control signal already selected, enetering error state\n\r");
        return false;
    }
}

void Controller::setState(State state){
    switch (state)
    {
    case State::STANDBY:
        _state = state;
        break;
    case State::STARTING:
        _state = state;
        break;
    case State::RUNNING:
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