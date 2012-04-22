#include "Sensor.h"

using namespace std;

Sensor::Sensor(AbstractEmu *emu,string name):SensorPrototype(emu,name)
{
  //variable for function blocks
  mode=0;

  msg=NULL;
  Pmsg=NULL;
  heatMsg=NULL;
  parent=string("");
  countAck=0;
  sonNum=0;
  heatSum=0;
}


void Sensor::stackReceive(cMessage *m)
{
  nMsg *msg=static_cast<nMsg *>(m);
  if (string(msg->getType())==string("who"))
  {
    if (parent==string(""))
    {
      parent=string(msg->getId());
      tr->notify();
    }
  }
  if (string(msg->getType())==string("whoAck"))
  {
    if (string(msg->getTargetId())==getName())
    {
      nSet.insert(string(msg->getId()));
    }
  }
  if (string(msg->getType())==string("sonMsg"))
  {
    if (string(msg->getTargetId())==getName())
    {
      countAck++;
      sonNum+=1+msg->getSonNum();
      heatSum+=msg->getHeat();
      tr->notify();
    }
  }
  if (string(msg->getType())==string("heat"))
  {
    //Get the message only from the parent
    if (string(msg->getId())==parent)
    {
      //Dispatch the average heat
      heatMsg=new dMsg("dMsg");
      heatMsg->setHeat(msg->getHeat());
      dispatch(heatMsg);
      heatMsg=NULL;
      //If it has sons
      if (sonNum>0)
      {
        //Send the average heat
        Pmsg=new nMsg("avg_heat");
        Pmsg->setId(getName().c_str());
        Pmsg->setTargetId("");//don't matter
        Pmsg->setSonNum(0);//don't matter
        //Average heat
        Pmsg->setHeat(msg->getHeat());
        Pmsg->setType("heat");
        emu->sendBytes(Pmsg);
        delete Pmsg;
        Pmsg=NULL;
      }
    }
  }
  delete msg;
}


void Sensor::stackSend(cMessage *data)
{
  //A message from this protocol
  parent="none";
  tr->notify();
}



Sensor::~Sensor()
{
  if (msg!=NULL) delete msg;
  if (Pmsg!=NULL) delete Pmsg;
  if (heatMsg!=NULL) delete heatMsg;
}


void Sensor::run()
{
  //all function variables are in the class
  //start coroutine
  switch (mode)
  {
    case 0:

    addProtocol("avg_heat",this);
    msg=NULL;
    tr=new countThread(this);
    startThread(tr);
    heat=getHeat();
    out<<"The temperature of node "<<getName()<<" is "<<heat;
    show(out.str());
    ev<<out.str()<<"\n";
    //flash stream out
    out.str("");
    //give the thread a chance to sleep
    mode=1;
    case 1:

    if (sleep(1)) return;
    if (getName()==string("Node1"))
    {	
      send("avg_heat",NULL);
    }

    mode=2;
    case 2:
    
    if (sleep(1)) return;

    while (true) 
    { 
      mode=3;
      case 3:
        
      //Wait untill msg resived
      if (receive_block("dMsg")) return;
      msg=static_cast<dMsg *>(getMsg("dMsg"));
      ev<<"The graph average heat is "<<msg->getHeat()<<"\n";
      out<<"The graph average heat is "<<msg->getHeat()<<"\n";
      show(out.str());
      //flash stream out
      out.str("");
      delete msg;
      msg=NULL;
 
      mode=4;
      case 4:
       
      if (sleep(1)) return;
    }//end of while(true)
  }//end coroutine
}//end sensor run function


Sensor::countThread::countThread(Sensor *s):SensorThread(s)
{
  mode=0;
  sensor=s;
}

Sensor::countThread::~countThread()
{
}

bool Sensor::countThread::emuRun()
{	
  //Start corutine 
  switch (mode){
    case 0:
      
    if (wait()) return true;
    if (sensor->parent!=string("none"))
    {
      //Send who ack message
      sensor->Pmsg=new nMsg("avg_heat");
      sensor->Pmsg->setId(getName().c_str());
      sensor->Pmsg->setTargetId(sensor->parent.c_str());
      sensor->Pmsg->setType("whoAck");
      sensor->emu->sendBytes(sensor->Pmsg);
      delete sensor->Pmsg;

      mode=1;
      case 1:
      
      if (sleep(1)) return true;
    }

    //Send who message
    sensor->Pmsg=new nMsg("avg_heat");
    sensor->Pmsg->setId(getName().c_str());
    sensor->Pmsg->setTargetId("");
    sensor->Pmsg->setType("who");
    sensor->emu->sendBytes(sensor->Pmsg);
    delete sensor->Pmsg;
    sensor->Pmsg=NULL;
    
    mode=2;
    case 2:
    
    if (sleep(1)) return true;
    while(sensor->nSet.size()>sensor->countAck)
    {
      mode=3;
      case 3:
        
      if (wait()) return true;
    }
    
    mode=4;
    case 4:
    
    if (sleep(1)) return true;
    if (sensor->parent!=string("none"))
    {
      //Send son number and heat sum
      sensor->Pmsg=new nMsg("avg_heat");
      sensor->Pmsg->setId(getName().c_str());
      sensor->Pmsg->setTargetId(sensor->parent.c_str());
      sensor->Pmsg->setSonNum(sensor->sonNum);
      sensor->Pmsg->setHeat(sensor->heatSum+sensor->heat);
      sensor->Pmsg->setType("sonMsg");
      sensor->emu->sendBytes((cMessage *)sensor->Pmsg);
      delete sensor->Pmsg;
      sensor->Pmsg=NULL;
    }
    else
    {
      //Send the average heat
      sensor->Pmsg=new nMsg("avg_heat");
      sensor->Pmsg->setId(getName().c_str());
      sensor->Pmsg->setTargetId("");//don't matter
      sensor->Pmsg->setSonNum(0);//don't matter
      //Average heat
      sensor->Pmsg->setHeat((sensor->heat+sensor->heatSum)/(sensor->sonNum+1));
      sensor->Pmsg->setType("heat");
      sensor->emu->sendBytes(sensor->Pmsg);
      delete sensor->Pmsg;
      sensor->Pmsg=NULL;
      //Dispatch the average heat
      sensor->heatMsg=new dMsg("dMsg");
      sensor->heatMsg->setHeat((sensor->heat+sensor->heatSum)/(sensor->sonNum+1));
      dispatch(sensor->heatMsg);
      sensor->heatMsg=NULL;
    }
  }//end switch
  return false;
}

double Sensor::getHeat()
{
  //array of temperature
  float arr[]={1,2,3,4,5};
  //find the index of the sensor
  string indexStr=getName().substr(4);
  int index=atoi(indexStr.c_str())-1;
  //return the temperature of the sensor by the index
  return arr[index];
}
