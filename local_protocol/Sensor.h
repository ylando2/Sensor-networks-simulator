#ifndef _SENSOR_676867
#define _SENSOR_676867

#include "SensorPrototype.h" 
#include "myMsg_m.h"
#include <stdlib.h>
#include <time.h>
#include "Protocol.h"
#include <sstream>

class Sensor :
  public SensorPrototype,public Protocol
{

  public:

  private:

    //variable for fuction run
    int mode;
    myMsg *msg;

    //protocol functions
  public:

    //resive local protocol message
    virtual void stackReceive(cMessage *);

    //send local protocol message
    virtual void stackSend(cMessage *);

    //sensor functions
  public:

    //the main function that run on the sensor
    //moteMain in the real sensor
    virtual void run();

    //sensor constructor
    Sensor(AbstractEmu *,string);

    //sensor destructor
    virtual ~Sensor();

};

#endif
