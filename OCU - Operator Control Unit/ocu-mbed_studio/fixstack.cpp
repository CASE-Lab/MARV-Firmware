#include "fixstack.h"

fixstack::fixstack() :
_stackArray {0}{_currentPos = _stackArray;}


Menu* fixstack::top(){
    return *_currentPos;
}

void fixstack::push(Menu* dataIn){
    if(_currentPos == _stackArray+(sizeof(_stackArray)/sizeof(*_stackArray)) )
        return;
    _currentPos++;
    *_currentPos = dataIn;
}

void fixstack::pop(){
    if(_currentPos == _stackArray)
        return;
    *_currentPos = 0;
    _currentPos--;
}

bool fixstack::empty(){
    return _currentPos == _stackArray;
}