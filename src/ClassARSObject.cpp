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
Here we can see some data parsing from ARS.
For more information check documentation.
Class saves only last message cycle.
*/

void ARSObject::SetObjectStatus(struct can_frame frame)
{

	NoOfObjects = frame.data[0];
	MeasCounter = frame.data[1]*256 + frame.data[2];
	InterfaceVersionNumber = frame.data[4] & 0x0F;
	
	/*
	Object STATUS used  exclusive ONLY for the 
	automotive applications and NOT for 
	industrial functions. 
	*/
	
	/*
	Needed? working?
	SensorOutputReduced = frame.data[3] & 0x01;
	SensorExternalDisturbed = frame.data[3] & 0x02;
	SensorSwitchedOff = frame.data[3] & 0x04;
	SensorDefective = frame.data[3] & 0x08;
	SensorDefective = frame.data[3] & 0x10;
	SensorSupplyVoltageLow = frame.data[3] & 0x20;
	SensorRxInvalid  = frame.data[3] & 0x40;
	Reserved = frame.data[3] & 0x80;
	NumOfLaneRight = (frame.data[4] >> 4) & 0x03;
	NumOfLanesLeft = (frame.data[4] >> 6) & 0x03;
	LatDistToBorderLeft = (frame.data[5] * 0.1;
	LatDistToBorderRight = (frame.data[6] * 0.1;
	*/
}

void ARSObject::SetObject1(struct can_frame frame)
{


	Obj_ID = frame.data[0] >> 2;
	Obj_RolCount = frame.data[0] & 0x03; 
	Obj_LongDispl[Obj_ID] = frame.data[1] * 4 + (frame.data[2] >> 5);
	Obj_VrelLong[Obj_ID] = ( frame.data[2] & 0x1F ) * 128 + ( frame.data[3] >> 1 ) ;
	Obj_AccelLong[Obj_ID] = ( frame.data[3] & 0x01 ) * 256 + frame.data[4]; 
	Obj_ProbOfExist[Obj_ID] = frame.data[6] & 0x07;
	Obj_DynProp[Obj_ID] = ( frame.data[6] >> 3 ) & 0x07;
	Obj_LatDispl[Obj_ID] = frame.data[5] * 4 + ( frame.data[6] >> 6 ) ;
	Obj_Length[Obj_ID] = frame.data[7] & 0x07;
	Obj_Width[Obj_ID] = (frame.data[7] >> 3) & 0x07;
	Obj_MeasStat[Obj_ID] = ( frame.data[7] >> 6 ) & 0x3;
	
	
	Obj_LongDispl[Obj_ID] = Obj_LongDispl[Obj_ID] * 0.1;
	Obj_VrelLong[Obj_ID] = Obj_VrelLong[Obj_ID] * 0.0625 * - 128;
	Obj_AccelLong[Obj_ID] = Obj_AccelLong[Obj_ID] * 0.0625 - 16; 
	Obj_LatDispl[Obj_ID] = Obj_LatDispl[Obj_ID] * 0.1 - 52;
	
	/*have to deal how we are going to display those*/
	
	/*
	Obj_ProbOfExist[Obj_ID] = ;
	Obj_DynProp[Obj_ID] = ;
	Obj_Length[Obj_ID] = ;
	Obj_Width[Obj_ID] = ;
	Obj_MeasStat[Obj_ID] = ;
	*/

}

void ARSObject::SetObject2(struct can_frame frame)
{

	Obj_RCSVal[Obj_ID] = frame.data[0];
	Obj_LatSpeed[Obj_ID] = frame.data[1];
	Obj_ObstacleProbability[Obj_ID] = frame.data[2] & 0x7F;
	
	Obj_RCSVal[Obj_ID] = Obj_RCSVal[Obj_ID] * 0.5 - 64;
	Obj_LatSpeed[Obj_ID] = Obj_LatSpeed[Obj_ID] * 0.25 - 32;
	
}

