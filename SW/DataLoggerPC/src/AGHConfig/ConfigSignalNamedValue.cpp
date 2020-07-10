#include "ConfigSignalNamedValue.h"

using namespace std;

ConfigSignalNamedValue::ConfigSignalNamedValue(string _name)
{
    name = _name;
}

void ConfigSignalNamedValue::setName(string _name, int _length){
    if (_length >= 0){
        name = string(_name, 0, static_cast<size_t>(_length));
    } else {
        name = _name;
    }
}

void ConfigSignalNamedValue::setName(char* _name, int _length){
    if (_length >= 0){
        name = string(_name, 0, static_cast<size_t>(_length));
    } else {
        name = string(_name);
    }
}

string ConfigSignalNamedValue::getName(){
    return name;
}
