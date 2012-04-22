//***********************************************************//
//This network simulation wraparound omnet was created by:
//                    Yuval Lando
//***********************************************************//

#ifndef _txc1_34345
#define _txc1_34343

#include <string.h>
#include <omnetpp.h>
#include "AbstractEmu.h"
#include "Sensor.h"

class Txc1 : public cSimpleModule,public AbstractEmu
{
  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

  private:
    //function for the implements of emulator
    Sensor *sensor;
    bool startemu;

    //timer that wake the sensor in the next event
    cMessage *timer;
    //Minimum time to the next event 
    float minTime;

  public:
    virtual void sendBytes(cMessage *);
    virtual void show(string);
    virtual void wake(float);
    virtual float getTime();

};

// The module class needs to be registered with OMNeT++
Define_Module(Txc1);

#endif
