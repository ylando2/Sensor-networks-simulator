#ifndef _abstructEmu_456546
#define _abstructEmu_456546

#include <cmessage.h>

using namespace std;

class AbstractEmu 
{
  public:
    virtual float getTime()=0;
    virtual void sendBytes(cMessage *)=0;
    virtual void show(string)=0;
    //gets the number of nano seconds to wake
    virtual void wake(float)=0; 
    virtual ~AbstractEmu();
};

#endif
