#ifndef _SENSOR_PROTOTYPE_495687
#define _SENSOR_PROTOTYPE_495687

#include <list>
#include "AbstractEmu.h"
#include <string>
#include <map>
#include "protocol.h"
#include <limits>

using namespace std;

class SensorPrototype
{
  public:
  class SensorThread;//abstract subclass
  
  private:
    //variable for dispatch
    cMessage *msgReceived;

  protected:
    AbstractEmu *emu;//omnet prog


  private:
    typedef map<string, Protocol *> ProtocolMapType;
    ProtocolMapType protocolMap;

    string name;
    list<SensorThread *> threads;

    void remove(list<SensorThread *>::iterator it);

    float blockT;

    void initBlock(float &);
    bool sleep(float &,float ); 
    bool receive_block(string ,float &,float);
  protected:
    bool sleep(float t){return sleep(blockT,t);}
    bool receive_block(string msgName,float t)
    {return receive_block(msgName,blockT,t);}

    class cMessage *getMsg(string); 

    bool receive_block(string);

  public:

    //Move the message so it can be received in getMsg("msgName")
    void dispatch(cMessage *);

    //send resive function
    void receive(cMessage *);
    void send(string protocolName,cMessage *);
    //protocol function
    Protocol *getProtocol(string);
    void addProtocol(string,Protocol *);

    void show(string s) {emu->show(s);}

    string getName(){return name;}
    float getTime(){return emu->getTime();}
    SensorPrototype(AbstractEmu *,string);
    virtual void run()=0;
    void runThreads();
    virtual ~SensorPrototype();

  protected:
    void startThread(SensorThread *);
  
  public: 
    friend class SensorThread;
    class SensorThread
    {
      private:
        float blockT;
        bool waitMode;
        bool hasNotify;
      protected:
        bool wait();
        bool wait(float);
        SensorPrototype *sensor;
        cMessage *getMsg(string msgName) 
        {return sensor->getMsg(msgName);}
        bool receive_block(string msgName)
        {return sensor->receive_block(msgName);}
        string getName(){return sensor->getName();}
        bool sleep(float t){return sensor->sleep(blockT,t);}
        bool receive_block(string msgName,float t)
        {return sensor->receive_block(msgName,blockT,t);}
        void send(string protocolName,cMessage *m)
        {sensor->send(protocolName,m);}
        void dispatch(cMessage *m){sensor->dispatch(m);}
        float getTime(){return sensor->getTime();};
        void show(string s){sensor->show(s);}	
      public:
        void notify();

        explicit SensorThread(SensorPrototype *s);

        //Return true if it is still running 
        //and false otherwise
        virtual bool emuRun()=0;
        virtual ~SensorThread();
    };

};

#endif
