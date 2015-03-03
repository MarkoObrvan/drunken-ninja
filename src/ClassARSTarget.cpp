#include "ClassARSTarget.h"

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

//#DEFINE PI 3.14
#define dek_to_rad 0.01745329251 

using namespace std;


/*
Use mainly for debug, use as you wish
*/
void ARSTarget::Display()
{
	cout << "gather long:" << longDispl[NoOfTarget2-1] << "    lat: " << latDispl[NoOfTarget2-1] << "    index: " << NoOfTarget2-1 << endl; 
	//cout << "NumOfTargets:" << Sum_Of_Targets<< endl; 
}


/*
Constructor, sets track message ID-s based on set up id using function call.
*/
ARSTarget::ARSTarget(int ARS_target_id)
{
    SetARSMsgID(ARS_target_id);
}


/*
SRRMsgCheckout is class function that checks if given CAN frame belongs to given track class. 
Return 0 if does and saves all information from frame, 1 if it's last message of current cycle,
and -1 if message does not belong to given track.
*/
int ARSTarget::ARSMsgCheckout (struct can_frame frame)
{
	if (frame.can_id == Target_Status)
	{
			SetTargetStatus(frame);
			return 0; 
	}
	
	
	else if (frame.can_id == CAN1_Target_1)
	{
		SetTarget1(frame);
		
		return 0;
	}
	
	else if (frame.can_id == CAN1_Target_2)
	{
		SetTarget2(frame);
		
		//DISPLAY IS HEREEEE!!!!!
		Display();   //if (NoOfTarget2 < Sum_Of_Targets+2) 
		
		if (NoOfTarget2 == Sum_Of_Targets)
		{ 
		//cout << "   index of target: " << NoOfTarget2 << "   sum of tar: " << Sum_Of_Targets <<endl;;
		return 1;
		}
		
		return 0;
	}
	
	return 0;
}



void ARSTarget::SetARSMsgID(int ARS_Target_id)
{
    int id_no=(ARS_Target_id % 0x100)/0x10;
	ID = ARS_Target_id;
	Target_Status = 0x600 + id_no*0x10;
	CAN1_Target_1 = 0x701 + id_no*0x10;
	CAN1_Target_2 = 0x702 + id_no*0x10;
	
	cout << "target stat: " << Target_Status << "   target1: " << CAN1_Target_1  <<  "   target2: " << CAN1_Target_2  <<endl;
	
}


/*
Here we can see some data parsing from CAN SRR 2XX.
For more information check documentation.
Class saves only last message cycle.
*/

void ARSTarget::SetTargetStatus(struct can_frame frame)
{

	NoOfTargetsNear=frame.data[0];
	NoOfTargetsFar=frame.data[1];
//cout << "status msg near:  " << NoOfTargetsNear << "   far: " << NoOfTargetsFar  <<endl;
	Sum_Of_Targets = NoOfTargetsNear + NoOfTargetsFar;
	
cout << "NumOfTargets:" << Sum_Of_Targets<< endl; 
}

void ARSTarget::SetTarget1(struct can_frame frame)
{

	NoOfTarget1= frame.data[0];
	Tar_Dist_Rms[NoOfTarget1-1] = frame.data[1] & 0xff;
	Tar_Ang_Rms[NoOfTarget1-1] = (frame.data[2]>>1) & 0xff;
	Tar_Vrel_Rms[NoOfTarget1-1] = (frame.data[2] & 0x01)*256 + frame.data[3];
	Tar_Vrel[NoOfTarget1-1] = frame.data[4] * 16 + frame.data[5] >> 4;
	Tar_Dist[NoOfTarget1-1] = (((frame.data[5] & 0x07) * 256) + (frame.data[6] & 0xff));

	Tar_Dist_Rms[NoOfTarget1-1] = Tar_Dist_Rms[NoOfTarget1-1] * 0.1 - 10;
	Tar_Ang_Rms[NoOfTarget1-1] = Tar_Ang_Rms[NoOfTarget1-1] * 0.1;
	Tar_Vrel_Rms[NoOfTarget1-1] = Tar_Vrel_Rms[NoOfTarget1-1] * 0.03 - 5;
	Tar_Vrel[NoOfTarget1-1] = Tar_Vrel[NoOfTarget1-1] * 0.03 - 25;
	//cout << Tar_Dist[NoOfTarget1-1] << "  ";
	Tar_Dist[NoOfTarget1-1] = Tar_Dist[NoOfTarget1-1] * 0.1;
	//cout << "  " << Tar_Dist[NoOfTarget1-1] << "  ";


}

void ARSTarget::SetTarget2(struct can_frame frame)
{

	NoOfTarget2 = frame.data[0];
	Tar_PdH[NoOfTarget2-1] = frame.data[1]>>1;
	Tar_Length[NoOfTarget2-1] = (frame.data[1] & 0x01) * 256 + frame.data[2];
	Tar_Widt[NoOfTarget2-1] = (frame.data[3] & 0x01) * 256 + frame.data[4];
	Tar_Ang_st[NoOfTarget2-1] = (frame.data[5] & 0x30) >> 4;
	Tar_Typ[NoOfTarget2-1] = frame.data[5] >> 6;
	Tar_Ang[NoOfTarget2-1] = (frame.data[5] & 0x0F)*64 + (frame.data[6] >> 2);
	Tar_RCSValu[NoOfTarget2-1] = (frame.data[6] & 0x3)*256 + frame.data[7];

	Tar_Length[NoOfTarget2-1] = Tar_Length[NoOfTarget2-1] * 0.01;
	Tar_Widt[NoOfTarget2-1] = Tar_Widt[NoOfTarget2-1] * 0.01;

	Tar_Ang[NoOfTarget2-1] = Tar_Ang[NoOfTarget2-1] * 0.1 - 30;
	Tar_RCSValu[NoOfTarget2-1] = Tar_RCSValu[NoOfTarget2-1] * 0.1 - 50;

	//TODO NEKA BANANA OVDJE MOZDA?! ne dobijem float nazad!
	//idiote, da bi dobio float varijable sigurno moraju biti long -.-
	longDispl[NoOfTarget2-1] = Tar_Dist[NoOfTarget2-1] * cos (abs(Tar_Ang[NoOfTarget2-1]) * dek_to_rad) ;
	//if (NoOfTarget2 < Sum_Of_Targets+2)  cout << "longdispl: " << longDispl[NoOfTarget2-1] << " = " << Tar_Dist[NoOfTarget2-1] << " * " << cos (abs(Tar_Ang[NoOfTarget2-1]) * dek_to_rad) << endl;
	
	latDispl[NoOfTarget2-1] = Tar_Dist[NoOfTarget2-1] * sin (Tar_Ang[NoOfTarget2-1] * dek_to_rad) ;
	
	//Cluster_Long[Cluster_Index] = Cluster_Range[Cluster_Index] * cos (Cluster_Azimuth[Cluster_Index] * dek_to_rad) ; //dek_to_rad PI/180
	//Cluster_Lat[Cluster_Index] = Cluster_Range[Cluster_Index] * sin (Cluster_Azimuth[Cluster_Index] * dek_to_rad) ;

}

