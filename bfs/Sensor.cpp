#include "Sensor.h"

using namespace std;

Sensor::Sensor(AbstractEmu *emu,string name):SensorPrototype(emu,name)
{
  //variable for function blocks
  mode=0;
  msg=NULL;
  //still unknown
  parent=string("");
}


void Sensor::stackReceive(cMessage *m)
{
  bfsMsg *msg=static_cast<bfsMsg *>(m);
  if (parent==string(""))
  {
    parent=string(msg->getId());
    msg->setId((getName()).c_str());
    emu->sendBytes(msg);
    pMsg *pm=new pMsg("pMsg");
    pm->setParent(parent.c_str());
    dispatch(pm);
    pm=NULL;
  }
  delete msg;
}


void Sensor::stackSend(cMessage *data)
{
  //A message from this protocol
  bfsMsg *msg;
  msg=new bfsMsg("bfs");
  parent=string("None");
  msg->setId(getName().c_str());
  emu->sendBytes(msg);
  delete msg;
  pMsg *pm=new pMsg("pMsg");
  pm->setParent(parent.c_str());
  dispatch(pm);
}



Sensor::~Sensor(void)
{
  if (msg!=NULL) delete msg;
}

void Sensor::run(void)
{
  //all function variables are in the class
  //start coroutine

  switch (mode)
  {
    //start of the function
    case 0:

    addProtocol("bfs",this);
    msg=NULL;
    if (getName()==string("Node1"))
      send("bfs",NULL);
    
    mode=1;
    case 1:
     
    if (sleep(1)) return;

    while (true) { 
      mode=2;
      case 2:
 
      if (receive_block("pMsg")) return;
      msg=static_cast<pMsg *>(getMsg("pMsg"));
      ev<<"the parent of "<<getName()<<" is "<<msg->getParent()<<"\n";
      out<<"the parent is "<<msg->getParent();
      show(out.str());
      delete msg;
      msg=NULL;
      
      mode=3;
      case 3:
      
      if (sleep(1)) return;
      }//end of while(true)
  }//end coroutine
}//end sensor run function
