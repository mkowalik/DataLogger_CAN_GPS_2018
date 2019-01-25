#ifndef CONFIGFRAME_H
#define CONFIGFRAME_H
#include "ReadingClass.h"
#include "WritingClass.h"
#include "ConfigChannel.h"
#include <vector>
#include <fstream>

using namespace std;

class ConfigFrame : public WritableToBin, public ReadableFromBin {
private:
    int                     ID;
    string                  moduleName;
    vector <ConfigChannel>  channels;
public:

    class iterator : public std::iterator<std::bidirectional_iterator_tag, ConfigChannel> {
        friend class ConfigFrame;
    private:
       vector <ConfigChannel>::iterator innerIterator;
       ConfigFrame& frameReference;
    protected:
       iterator(vector <ConfigChannel>::iterator it, ConfigFrame& frameRef);
    public:
       bool operator==(const ConfigFrame::iterator& second);
       bool operator!=(const ConfigFrame::iterator& second);
       ConfigChannel& operator*();
       ConfigChannel* operator->();
       iterator& operator++();
       iterator operator++(int);
       iterator& operator--();
       iterator operator--(int);
    };

    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const ConfigChannel> {
        friend class ConfigFrame;
    private:
       vector <ConfigChannel>::const_iterator innerIterator;
       const ConfigFrame& frameReference;
    protected:
       const_iterator(vector <ConfigChannel>::const_iterator it, const ConfigFrame& frameRef);
    public:
       bool operator==(const ConfigFrame::const_iterator& second);
       bool operator!=(const ConfigFrame::const_iterator& second);
       const ConfigChannel& operator*() const;
       const ConfigChannel* operator->() const;
       const_iterator& operator++();
       const_iterator operator++(int);
       const_iterator& operator--();
       const_iterator operator--(int);
    };

    int                     get_ID() const;
    int                     get_DLC() const;
    string                  get_moduleName() const;

    void                    set_ID(int aID);
    void                    set_moduleName(string aModuleName);

    iterator                begin();
    iterator                end();
    const_iterator          cbegin() const;
    const_iterator          cend() const;
    ConfigChannel&          get_channel_by_position(int position);

    void                    remove_channel_by_position(int position);

    void                    add_channel(ConfigChannel aChannel);

    void                    write_to_bin(WritingClass& writer) override;
    void                    read_from_bin(ReadingClass& reader) override;
};

#endif // CONFIGFRAME_H
