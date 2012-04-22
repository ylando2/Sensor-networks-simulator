//***********************************************************//
//This network simulation wraparound omnet was created by:
//                    Yuval Lando
//***********************************************************//


#include "SensorPrototype.h"

#define EPSILON numeric_limits<float>::min()

using namespace std;


void SensorPrototype::dispatch(cMessage *obj)
{
  if (msgReceived!=NULL) delete msgReceived;		
  msgReceived=obj;
}

Protocol *SensorPrototype::getProtocol(string protocolName)
{
  return protocolMap[protocolName];
}

void SensorPrototype::addProtocol(string protocolName,Protocol *protocol)
{
  protocolMap.insert(pair<string,Protocol *>(protocolName,protocol));
}

void SensorPrototype::receive(cMessage *m)
{
  string temp(m->name());
  Protocol *p=getProtocol(temp);
  p->stackReceive(m);
}


void SensorPrototype::send(string protocolName,cMessage *data)
{
  Protocol *p=getProtocol(protocolName);
  p->stackSend(data);
}

void SensorPrototype::remove(list<SensorThread *>::iterator it)
{
  SensorThread *temp=*it;
  threads.erase(it);
  delete temp;
}

void SensorPrototype::initBlock(float &block)
{
  block=-1;
}

SensorPrototype::SensorPrototype(AbstractEmu *e,string name)
{
  this->emu=e;
  this->name=name;
  initBlock(blockT);
  msgReceived=NULL;
}

bool SensorPrototype::receive_block(string msgName)
{
  if (msgReceived==NULL) return true;
  if (string(msgReceived->name())!=msgName) return true;
  return false;
}

bool SensorPrototype::receive_block(string msgName,float &block,float time)
{
  float now;
  now=emu->getTime();
  //return true if msg do not exist
  bool msgNotExist;

  if (msgReceived==NULL) 
  {
    msgNotExist=true;
  }
  else
  {
    if (string(msgReceived->name())!=msgName) 
    {
      msgNotExist=true;
    }
    else
    {
      msgNotExist=false;
    }
  }

  if (block==-1)
  {
    block=now;
  }

  if ((now-(block+time))+EPSILON<0 && msgNotExist)
  {
    emu->wake(block+time);
    return true;
  }
  else
  {
    block=-1;
    return false;//exit block becouse 
    //it has run tic times on the block
  }
}

bool SensorPrototype::sleep(float &block,float time)
{
  float now;
  now=emu->getTime();
  if (block==-1)
  {
    block=now;
  }
  if ((now-(block+time))+EPSILON<0)
  {
    emu->wake(block+time);
    return true;
  }
  else
  {
    block=-1;
    return false;//exit block becouse 
    //it has run tic times on the block
  }
}

cMessage *SensorPrototype::getMsg(string msgName)
{
  if (msgReceived==NULL) return NULL;
  if (string(msgReceived->name())!=msgName) return NULL;
  cMessage *temp=msgReceived;
  msgReceived=NULL;
  return temp;
}


SensorPrototype::~SensorPrototype()
{
  //clean threads
  while(!threads.empty())
  {
    SensorThread *s=threads.front();
    threads.pop_front();
    delete s;
  }

  //removing protocols
  for(ProtocolMapType::const_iterator it = protocolMap.begin(); it != protocolMap.end(); ++it)
  {
    string temp=it->first;
    //delete prtocol if it has * in the end of it's name
    //handle a bug of sensor delete itself becouse it is also a protocol
    //so every protocol that is not sensor must be call
    //name*
    if (temp[temp.length()-1]=='*')
    {
      delete it->second;
    }
  }

  if (msgReceived!=NULL) delete msgReceived;
}


void SensorPrototype::startThread(SensorThread *s)
{
  if ((s->emuRun())) threads.push_back(s);
}

void SensorPrototype::runThreads()
{
  list<SensorThread *>::iterator it;
  list<SensorThread *>::iterator temp;
  it=threads.begin();
  while (it!=threads.end())
  {
    if (!((*it)->emuRun())) 
    {	
      temp=it;
      temp++;
      remove(it);
      it=temp;
    }
    else
    {
      it++;
    }
  }
}

SensorPrototype::SensorThread::SensorThread(SensorPrototype *s)
{
  sensor=s;
  sensor->initBlock(blockT);
  waitMode=false;
  hasNotify=false;
}

bool SensorPrototype::SensorThread::wait()
{
  //start waiting
  if (waitMode==false)
  {
    //notify before wait
    hasNotify=false;
    waitMode=true;
    //keep waiting
    return true;
  }
  //notify while waiting
  if (hasNotify) {
    hasNotify=false;
    waitMode=false;
    //stop waiting
    return false;
  }
  //waiting without notify
  //continue waiting
  return true;
}

bool SensorPrototype::SensorThread::wait(float time)
{
  //dealing with time
  float now;
  now=sensor->emu->getTime();
  if (blockT==-1)
  {
    blockT=now;
  }
  //If wait time passed
  if (!((now-(blockT+time))+EPSILON<0))
  {
    blockT=-1;
    hasNotify=false;
    waitMode=false;
    //stop waiting
    return false;
  }

  //dealing with has notify
  //start waiting
  if (waitMode==false)
  {
    //notify before wait
    hasNotify=false;
    waitMode=true;
    //keep waiting
    sensor->emu->wake(blockT+time);
    return true;
  }

  //notify while waiting
  if (hasNotify) {
    hasNotify=false;
    waitMode=false;
    blockT=-1;
    //stop waiting
    return false;
  }

  //waiting without notify
  //and time has not passed
  //continue waiting
  sensor->emu->wake(blockT+time);
  return true;
}

void SensorPrototype::SensorThread::notify()
{
  //Prevent recursive calls
  if (!waitMode) return;

  hasNotify=true;
  if (!emuRun()) 
  {
    sensor->threads.remove(this);
    delete this;
  }
}

SensorPrototype::SensorThread::~SensorThread()
{
}
