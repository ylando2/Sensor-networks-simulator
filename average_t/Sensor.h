#ifndef _SENSOR_6456745
#define _SENSOR_6456745

#include "SensorPrototype.h" 
#include "nMsg_m.h"
#include "dMsg_m.h"
#include <stdlib.h>
#include <time.h>
#include "Protocol.h"
#include <sstream>
#include <set>

class Sensor :
  public SensorPrototype,public Protocol
{
  private:

    //variable for fuction run courtine
    int mode;

    //variable in the function run
    dMsg *msg;
    stringstream out;
    nMsg *Pmsg;
    dMsg *heatMsg;
    double heat;

    //protocol functions
  public:
    //set of neighbors
    set<string> nSet;
    int countAck;
    string parent;
    int sonNum;
    double heatSum;

    //resive local protocol message
    virtual void stackReceive(cMessage *);

    //send local protocol message
    virtual void stackSend(cMessage *);

    //sensor functions

    //the main function that run on the sensor
    //moteMain in the real sensor
    virtual void run();

    //sensor constructor
    Sensor(AbstractEmu *,string);

    //sensor destructor
    virtual ~Sensor();

    //thread
    class countThread;
    friend class countThread;
    class countThread:public SensorPrototype::SensorThread
    {
      friend class Sensor;
      public:
      int mode;
      Sensor *sensor;
      explicit countThread(Sensor *);
      virtual bool emuRun();
      virtual ~countThread();
    };

    countThread *tr;

    //Get the temperature around the sensor
    double getHeat();
};

#endif
