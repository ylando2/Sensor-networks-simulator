#ifndef _SENSOR_676867
#define _SENSOR_676867

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

public:

private:

	//variable for fuction run courtine
	int mode;
	
	//variable in the function run
	dMsg *msg;
	stringstream out;

//protocol functions
public:
	//set of neighbors
	set<string> nSet;

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
