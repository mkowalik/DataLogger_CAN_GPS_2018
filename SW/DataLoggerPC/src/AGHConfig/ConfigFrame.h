#ifndef CONFIGFRAME_H
#define CONFIGFRAME_H
#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"
#include <vector>
#include <fstream>

using namespace std;

class ConfigSignal;

class ConfigFrame : public WritableToBin, public ReadableFromBin {
private:
    unsigned int             id;
    unsigned int             dlc;
    string                   moduleName;
    vector <ConfigSignal*>  signalsVector;
public:
    static constexpr int MAX_FRAME_BYTES_LENGTH = 8;

    class iterator : public std::iterator<std::bidirectional_iterator_tag, ConfigSignal> {
        friend class ConfigFrame;
    private:
       vector <ConfigSignal*>::iterator innerIterator;
       ConfigFrame& frameReference;
    protected:
       iterator(vector <ConfigSignal*>::iterator it, ConfigFrame& frameRef);
    public:
       bool operator==(const ConfigFrame::iterator& second);
       bool operator!=(const ConfigFrame::iterator& second);
       ConfigSignal* operator*();
       ConfigSignal* operator->();
       iterator& operator++();
       iterator operator++(int);
       iterator& operator--();
       iterator operator--(int);
    };

    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const ConfigSignal> {
        friend class ConfigFrame;
    private:
       vector <ConfigSignal*>::const_iterator innerIterator;
       const ConfigFrame& frameReference;
    protected:
       const_iterator(vector <ConfigSignal*>::const_iterator it, const ConfigFrame& frameRef);
    public:
       bool operator==(const ConfigFrame::const_iterator& second);
       bool operator!=(const ConfigFrame::const_iterator& second);
       const ConfigSignal* operator*() const;
       const ConfigSignal* operator->() const;
       const_iterator& operator++();
       const_iterator operator++(int);
       const_iterator& operator--();
       const_iterator operator--(int);
    };

    ConfigFrame (unsigned int id, unsigned int dlc, string moduleName);
    ConfigFrame (ReadingClass& reader);

    unsigned int    getID() const;
    unsigned int    getDLC() const;
    string          getModuleName() const;

    void            setID(unsigned int id);
    void            setDLC(unsigned int dlc);
    void            setModuleName(string moduleName);

    iterator        begin();
    iterator        end();
    const_iterator  cbegin() const;
    const_iterator  cend() const;
    ConfigSignal&   getSignalByPosition(unsigned int position); //< TODO Deprecated

    void            removeAndDeleteSignalByPosition(unsigned int position); //< TODO deprecated

    void            addSignal(ConfigSignal* pSignal);

    void            writeToBin(WritingClass& writer) override;
    void            readFromBin(ReadingClass& reader) override;

    virtual         ~ConfigFrame() override;
};

#endif // CONFIGFRAME_H
