#ifndef CONFIGFRAME_H
#define CONFIGFRAME_H

#include <vector>
#include <fstream>

#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"

#include "AGHConfig/ConfigSignal.h"

using namespace std;

class Config;

class ConfigFrame : public WritableToBin, public ReadableFromBin {
    friend class Config;
    friend class ConfigSignal;
public:
    static constexpr unsigned int MAX_DLC_VALUE = 8U;
    static constexpr unsigned int MAX_ID_BITS   = 11U;
    static constexpr unsigned int MAX_ID_VALUE  = (1<<MAX_ID_BITS)-1;

    using SignalsIterator       = std::vector<ConfigSignal*>::iterator;
    using ConstSignalsIterator  = std::vector<ConfigSignal*>::const_iterator;
private:
    ConfigFrame (Config* pConfig, unsigned int frameID, string frameName);
    ConfigFrame (Config* pConfig, ReadingClass& reader);

    void                                        addSignal(ConfigSignal *pSignal);
    std::vector<ConfigSignal*>::const_iterator  lowerBoundSignalConstIterator(unsigned int signalID) const;
    std::vector<ConfigSignal*>::iterator        lowerBoundSignalIterator(unsigned int signalID);
    bool                                        signalsEmpty() const;
    void                                        sortSignalsCallback();
    void                                        _setFrameID(unsigned int frameID);

    Config*                     pParentConfig;
    unsigned int                frameID;
    string                      frameName;
    std::vector<ConfigSignal*>  signalsVector;

public:
    Config*                 getParentConfig() const;
    unsigned int            getFrameID() const;
    string                  getFrameName() const;

    void                    setFrameID(unsigned int frameID);
    void                    setFrameName(string frameName);

    //<----- Access to signals definitions ----->/
    bool                    hasSignalWithID(unsigned int signalID) const;
    ConfigSignal*           getSignalWithID(unsigned int signalID) const;
    ConfigSignal*           addSignal(unsigned int   signalID,
                                      unsigned int   startBit,
                                      unsigned int   lengthBits,
                                      ValueType      valueType,
                                      int            multiplier,
                                      unsigned int   divider,
                                      int            offset,
                                      string         signalName,
                                      string         unitName,
                                      string         comment
                            );
    ConfigSignal*           addSignal(unsigned int   startBit,
                                      unsigned int   lengthBits,
                                      ValueType      valueType,
                                      int            multiplier,
                                      unsigned int   divider,
                                      int            offset,
                                      string         signalName,
                                      string         unitName,
                                      string         comment
                            );
    void                    removeSignal(const SignalsIterator signalIt);
    void                    removeSignal(unsigned int signalId);

    SignalsIterator         beginSignals();
    SignalsIterator         endSignals();
    ConstSignalsIterator    cbeginSignals() const;
    ConstSignalsIterator    cendSignals() const;

    //<----- General purpose methods definitions ----->/
    virtual void            writeToBin(WritingClass& writer) override;
    virtual void            readFromBin(ReadingClass& reader) override;

    virtual                 ~ConfigFrame() override;
};

#endif // CONFIGFRAME_H
