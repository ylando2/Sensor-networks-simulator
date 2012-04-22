#include "txc1.h"

void Txc1::initialize()
{
  //initialize is called from the builder so
  //it must create the sensor in lazy way
  //becouse it needs the emu to construct itself
  startemu=false;
  timer = new cMessage("timer");
  scheduleAt(1, timer);
}

void Txc1::handleMessage(cMessage *msg)
{
  // The handleMessage() method is called whenever a message arrives
  // at the module. Here, we just send it to the other module, through
  // gate `out'. Because both `tic' and `toc' does the same, the message
  // will bounce between the two.

  //The next even is not schedule yet
  minTime=-1;

  //construct sensor in a lazy way
  if (!startemu)
  {
    string tempname(parentModule()->name());
    sensor=new Sensor(this,tempname);//need to fix
    startemu=true;
  }

  if (strcmp("timer",msg->name())==0)
  {	
    sensor->runThreads();
    sensor->run();
  }
  else
  {
    //resive message
    sensor->receive(msg);
    sensor->runThreads();
    sensor->run();
  }

  //minTime is bigger then -1
  if (minTime>0)
  {
    if (timer->isScheduled()) cancelEvent(timer);
    scheduleAt(minTime, timer);
  }

}


//send msg broadcast to all the neightboards
void Txc1::sendBytes(cMessage *msg)
{
  int i;
  for (i=0;i<gate("out")->size();i++)
    send(static_cast<cMessage *>(msg->dup()), "out",i);
}

void Txc1::show(string str)
{
  parentModule()->bubble(str.c_str());
}

void Txc1::wake(float time)
{
  if (minTime<0 || minTime>time)
    minTime=time;
}

float Txc1::getTime()
{
  return simTime();
}

void Txc1::finish()
{
  delete sensor;
  if (timer->isScheduled()) cancelEvent(timer);
  delete timer;
}

