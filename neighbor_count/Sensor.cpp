#include "Sensor.h"

using namespace std;

Sensor::Sensor(AbstractEmu *emu,string name):SensorPrototype(emu,name)
{
  //variable for function blocks
  mode=0;
  msg=NULL;
}

void Sensor::stackReceive(cMessage *m)
{
  nMsg *msg=static_cast<nMsg *>(m);
  if (string(msg->getType())==string("who"))
  {
    nMsg *whoAck=new nMsg("neighbor");
    whoAck->setId(getName().c_str());
    whoAck->setTargetId(msg->getId());
    whoAck->setType("whoAck");
    emu->sendBytes(whoAck);
    delete whoAck;
  }
  if (string(msg->getType())==string("whoAck"))
  {
    if (string(msg->getTargetId())==getName())
    {
      nSet.insert(string(msg->getId()));
      dMsg *dm=new dMsg("dMsg");
      dm->setDegree(nSet.size());
      dispatch(dm);
      dm=NULL;
    }
  }
  delete msg;
}

void Sensor::stackSend(cMessage *data)
{
  //A message from this protocol
  nMsg *Pmsg=new nMsg("neighbor");
  Pmsg->setId(getName().c_str());
  Pmsg->setTargetId("");
  Pmsg->setType("who");
  emu->sendBytes(Pmsg);
  delete Pmsg;
}

Sensor::~Sensor()
{
  if (msg!=NULL) delete msg;
}

void Sensor::run()
{
  //all function variables are in the class
  //start coroutine
  switch (mode)
  { 
    case 0:
    
    addProtocol("neighbor",this);
    msg=NULL;
    send("neighbor",NULL);

    mode=1; 
    case 1:
   
    if (sleep(1)) return;

    while (true) 
    { 
      mode=2;
      case 2:
     
      if (receive_block("dMsg")) return;
      msg=static_cast<dMsg *>(getMsg("dMsg"));
      ev<<getName()<<" has a degree of "<<msg->getDegree()<<"\n";

      out<<"the degree is "<<msg->getDegree();
      show(out.str());
      delete msg;
      msg=NULL;
      
      mode=3; 
      case 3:
          
      if (sleep(1)) return;
    }//end of while(true)
  }//end coroutine
}//end sensor run function
