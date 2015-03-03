#include "ClassARSObject.h"

#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include <math.h>  

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define dek_to_rad 0.01745329251 

using namespace std;


/*
Use mainly for debug, use as you wish
*/
void ARSObject::Display()
{
	//cout << "gather long:" << longDispl[NoOfObject2-1] << "    lat: " << latDispl[NoOfObject2-1] << "    index: " << NoOfObject2-1 << endl; 
	//cout << "NumOfObjects:" << Sum_Of_Objects<< endl; 
}


/*
Constructor, sets track message ID-s based on set up id using function call.
*/
ARSObject::ARSObject(int ARS_Object_id)
{
    SetARSMsgID(ARS_Object_id);
}


/*
SRRMsgCheckout is class function that checks if given CAN frame belongs to given track class. 
Return 0 if does and saves all information from frame, 1 if it's last message of current cycle,
and 0 if message does not belong to given track.
*/
int ARSObject::ARSMsgCheckout (struct can_frame frame)
{
	if (frame.can_id == CAN1_Object_Status)
	{
			SetObjectStatus(frame);
			return 0; 
	}
	
	
	else if (frame.can_id == CAN1_Object_1)
	{
		SetObject1(frame);
		
		return 0;
	}
	
	else if (frame.can_id == CAN1_Object_2)
	{
		SetObject2(frame);
		
		//DISPLAY IS HEREEEE!!!!!
		Display();   //if (NoOfObject2 < Sum_Of_Objects+2) 
		
		if (Obj_ID == NoOfObjects)
		{ 
		//cout << "   index of Object: " << NoOfObject2 << "   sum of tar: " << Sum_Of_Objects <<endl;;
		return 1;
		}
		
		return 0;
	}
	
	return 0;
}



void ARSObject::SetARSMsgID(int ARS_Object_id)
{
    int id_no=(ARS_Object_id % 0x100)/0x10;
	ID = ARS_Object_id;
	CAN1_Object_Status = 0x60A + id_no*0x10;
	CAN1_Object_1 = 0x60B + id_no*0x10;
	CAN1_Object_2 = 0x60C + id_no*0x10;
	
	cout << "Object stat: " << CAN1_Object_Status << "   Object1: " << CAN1_Object_1  <<  "   Object2: " << CAN1_Object_2  <<endl;
	
}


/*
Here we can see some data parsing from CAN SRR 2XX.
For more information check documentation.
Class saves only last message cycle.
*/

void ARSObject::SetObjectStatus(struct can_frame frame)
{

	NoOfObjects;
	MeasCounter;
	
	
	/*
	Object STATUS used  exclusive ONLY for the 
	automotive applications and NOT for 
	industrial functions. 
	*/
	SensorOutputReduced; 
	SensorExternalDisturbed; 
	SensorSwitchedOff; 
	SensorDefective;
	SensorSupplyVoltageLow;
	SensorRxInvalid;
	Reserved;
	InterfaceVersionNumber;
	NumOfLaneRight;
	NumOfLanesLeft;
	LatDistToBorderLeft;
	LatDistToBorderRight;
	
}

void ARSObject::SetObject1(struct can_frame frame)
{


	Obj_ID;
	Obj_RolCount; 
	Obj_LongDispl[ID];
	Obj_VrelLong[ID];
	Obj_AccelLong[ID]; 
	Obj_ProbOfExist[ID];
	Obj_DynProp[ID];
	Obj_LatDispl[ID];
	Obj_Length[ID];
	Obj_Width[ID];
	Obj_MeasStat[ID];


}

void ARSObject::SetObject2(struct can_frame frame)
{

	Obj_RCSVal[ID];
	Obj_LatSpeed[ID];
	Obj_ObstacleProbability[ID];

}

