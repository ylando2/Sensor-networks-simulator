#ifndef _Protocol4365476
#define _Protocol4365476

#include <cmessage.h>
class Protocol
{

  public:

    //Overide this function
    //for resiving message in protcol
    virtual void stackReceive(cMessage *)=0;

    //Overide this function
    //for sending message in protocol
    virtual void stackSend(cMessage *)=0;

    virtual ~Protocol();

};

#endif
