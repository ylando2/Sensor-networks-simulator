#include "Sensor.h"

using namespace std;

Sensor::Sensor(AbstractEmu *emu,string name):SensorPrototype(emu,name)
{
  //variable for function blocks
  mode=0;
  //pointers in the function
  msg=NULL;
}


void Sensor::stackReceive(cMessage *m)
{
  cMessage *data=m->encapsulatedMsg();
  m->decapsulate();
  delete m;
  dispatch(data);
  data=NULL;
}


void Sensor::stackSend(cMessage *data)
{
  //A message from this protocol
  cMessage *Pmsg=new cMessage("local");
  Pmsg->encapsulate(static_cast<cMessage *>(data->dup()));
  emu->sendBytes(Pmsg);
  delete Pmsg;
}



Sensor::~Sensor()
{
  //variable to delete from the function
  //pointer in functions
  if (msg!=NULL) delete msg;
}

void Sensor::run()
{
  //all function variables are in the class
  //start coroutine
  switch (mode)
  {
    case 0:

    addProtocol("local",this);
    msg=NULL;

    if (getName()==string("Node1"))
    {
      msg=new myMsg("myMsg");
      msg->setIndex(1);
      send("local",msg);
      delete msg;
      msg=NULL;
    }

    while (true) { 
      //Coroutine block 1
      mode=1;
      case 1:
      //Wait untill msg resived
      if (receive_block("myMsg")) return;
      msg=static_cast<myMsg *>(getMsg("myMsg"));
      if (msg!=NULL)
      {
        //Write in bubble that he get the msg
        show("get the message");
        //Write in consule that he get the msg
        ev<<"get the message \n";
        delete msg;
        //msg must be equal to null every time you delete it
        msg=NULL;
      }//end if (msg!=NULL)
    }//end while(true)

  }//end coroutine
}//end sensor run function
