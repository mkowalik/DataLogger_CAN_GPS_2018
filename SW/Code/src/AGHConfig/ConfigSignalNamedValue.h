#ifndef CONFIGSIGNALNAMEDVALUE_H
#define CONFIGSIGNALNAMEDVALUE_H

#include <string>

class ConfigSignalNamedValue
{
private:
    std::string name;
public:
    ConfigSignalNamedValue(std::string _name);         //< Constructor. Stores given _name in object. Value should be saved in ConfigChannel data structure.
    void setName(std::string _name, int _length = -1); //< Stores given _name in object
    void setName(char* _name, int _length = -1);       //< Stores given _name in object
    std::string getName();                             //< Returns name stored in object
};

#endif // CONFIGSIGNALNAMEDVALUE_H
