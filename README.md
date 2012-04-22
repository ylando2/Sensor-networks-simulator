sensor-networks-simulator
=========================

Programmer: Yuval Lando

About
----------------------
A few years ago, I try to write programs for real sensors.
Sensor are very small (the size of a matchbox) 
computers with a usb connection and radio device.
The problem was that I did not see the outcome of the program 
(there was one nice simulator called tossim but it does not have graphic capability).
So I build my own using omnet++. This simulator is trying to emulate  
sensors with sentilla. Sentilla is a java platform and omnet++ a c++;
therefore there are some strange design choices.
I emulate the multithread using coroutine build on a duff device.
I also used the same technique on tiny-os device; so we can use
the program written in this simulator on real sensors with tiny-os install on them.
Those programs are released in a different repository.

This simulator works on omnetpp-3.3, if you want to update it to 4,
You can hire me as a programmer (or consultant) my email is:
ylando2@gmail.com

How to use the simulator
--------------------------
First we start with "local_protocol", you compile it in the same way that you compile the
tic tac toe example of the omnet tutorial.

We build a message by creating a [name].msg text file.
The text file looks like this:

```
message [name]
{
  fields:
  [type_in_c1] [name_of_var1]
  [type_in_c2] [name_of_var2]
  ...
} 
```

Now we can make it generate automatically:
`[name]_m.cpp` and `[name]_m.h` files.
They will have the class [name] with the getter and setter:
`getVariableName()` and `setVariableName(variable_type)`

Working with the ned file:
Make sure that for every new node that we draw,
We press attributes->predefine channel choose channel name
"slow".

Programming the simulator:
------------------------------
We will use `Sensor.cpp` and `Sensor.h` .
The main program will look something like this:
```c++
void run() 
{
switch (mode)
  {
    mode=0;
    case 0:
    
    //Start of the function
    while(true) 
    {
    //In c++ switch can go into while scope
      mode=1;
      case 1:    
      // if stopping exit the function other wise continue
      if (stop_func()) retrun;
    }
  }
}
```

Make sure that all the variables of run are class members of the sensor class.

Sending message is done by:
`send("protocol name",msg);`

Receiving message is done by:
```c++
mode=[number];
case [number]:
if (receive_block("msg name")) return;
myMsg *msg=static_cast<myMsg>(getMsg("msg name"));
```

If you want to have a timeout you can do:
```c++
mode=[number];
case [number]:
if (receive_block("msg name",[time])) return;
if (msg!=null)
{
...
}
```

It also support sleep by:
```c++
mode=[number];
case [number]:
if (sleep(time)) return;
```

`string getName()` - return the name of the node.
`float getTime()` - return the time in seconds.

Output can be done by:
`ev` - It is like `cout` print the streams to the console; for example: `ev<<getName<<" get the message"<<"\n";`
show - It show a text bubble on the node; for example: `show("send msg")`

Using the protocol:
--------------------------
`void stackSend(cMessage *data)` - get a message that had to be send.
Warp it out with something like:
```c++
protocolMsgType protocolMsg=new protocolMsgType("protocolName");
protocolMsg->encapsulate(static_cast<cMessage *>(data->dup));
```
Send it away by: `emu->sendBytes(protocolMsg)`
In the end we must delete the message by: `delete protocolMsg;`

`void stackReceive(cMessage *msg)` - get a message.
Unwarp it by: `myMsg *data=static_cast<myMsg>(msg->encapsulatedMsg());
Free and delete the warper by:
```c++
msg->decapsulate();
delete msg;
```
The threads need to receive the message by `getMsg("message name")` so
we use `dispatch(data);` transfer the message to the threads.
If we do that make sure that we do not delete the message.

Adding a protocol to the program is done by the function `addProtocol("protocolName",Protocol obj);`
We can make the program automatic delete the protocol by adding star to the end of it's name; 
for example: `addProtocol("myProtocol*",new myProtocol());`

Adding threads to the simulator:
-------------------------------------
Declaring a thread inside the sensor class by:
```c++
class MyThread;
friend class MyThread;
class MyThread:public SensorPrototype::SensorThread
{
  friend class Sensor;
  public:
  int mode;
  Sensor *sensor;
  explicit MyThread(Sensor *);
  virtual bool emuRun();
  virtual ~MyThread();
};
```

Initialize by:
```c++
Sensor::MyThread::MyThread(Sensor *s):SensorThread(s)
{
  mode=0;
  sensor=s;
}
```

We have to implement emuRun for the thread the same way that we implement run for the sensor.
Adding a thread is done by: 
```c++
MyThread *tr=new MyThread(this);
startThread(tr);
```
Do not delete this thread it will be done automatically by the program.
Thread supports all the blocking function that the sensor supports; but in additional it support:
wait and notify.
Inside the Thread we can put:
```c++
mode=[number];
case [number]:
if (wait()) return;
```
Some where else we put:
```c++
tr.notify();
```

Miscellaneous technical issues:
---------------------------------
The message will contain string with the type char *.
So if we want to compare it to the node name we need to write:
```c++
if (getName==string(msg->getId())) ...
```
And if we want to put the node name on the message we do:
```c++
msg->setId(getName.c_str());
```
`setId` copy the c string data to the message.

The message support double type but does not support float type.

License
-------
sensor-networks-simulator is released under the MIT license.
