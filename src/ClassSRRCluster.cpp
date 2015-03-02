#include "ClassSRRCluster.h"

#include <stdio.h>
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
void SRRCluster::Display()
{
	cout << "gather long:" << Cluster_Long[Cluster_Index] << "    lat: " << Cluster_Lat[Cluster_Index] << "    index: " << Cluster_Index << endl; 
	//cout << "NumOfTracks:" << NumOfTracks<< "    TrackSt_RollCount: " << TrackSt_RollCount << endl; 
}


/*
Constructor, sets track message ID-s based on set up id using function call.
*/
SRRCluster::SRRCluster(int SRR_track_id)
{
    SetSRRMsgID(SRR_track_id);
}


/*
SRRMsgCheckout is class function that checks if given CAN frame belongs to given track class. 
Return 0 if does and saves all information from frame, 1 if it's last message of current cycle,
and -1 if message does not belong to given track.
*/
int SRRCluster::SRRMsgCheckout (struct can_frame frame)
{
	if (frame.can_id == CAN1_Cluster_Status)
	{
			SetClusterStatus(frame);
			return 0; 
	}
	
	
	else if (frame.can_id == CAN1_Cluster_1)
	{
		SetCluster1(frame);
		
		//DISPLAY IS HEREEEE!!!!!
		//Display();
		
		if (Cluster_Index == NoOfClusters-1) return 1;
		
		return 0;
	}
	
	return 0;
}



void SRRCluster::SetSRRMsgID(int SRR_track_id)
{
    int id_no=(SRR_track_id % 0x100)/0x10;
	ID = SRR_track_id;
	Radar_Status = 0x60A + id_no*0x10;
	CAN1_Cluster_Status = 0x70B + id_no*0x10;
	CAN1_Cluster_1 = 0x70C + id_no*0x10;
	
	cout << "cluster stat: " << CAN1_Cluster_Status << "   cluster1 " << CAN1_Cluster_1  <<endl;
	Cluster_Index = 0;
}


/*
Here we can see some data parsing from CAN SRR 2XX.
For more information check documentation.
Class saves only last message cycle.
*/

void SRRCluster::SetClusterStatus(struct can_frame frame)
{

	NoOfClusters = frame.data[0];
	ClusterSt_RollCount = frame.data[1] & 0x03;
}

void SRRCluster::SetCluster1(struct can_frame frame)
{

	Cluster_Index = frame.data[0];
	Cluster_RollCount[Cluster_Index] = frame.data[4] >> 6;
	Cluster_RCSValue[Cluster_Index] = frame.data[1];
	Cluster_Range[Cluster_Index] = frame.data[2];
	Cluster_Azimuth[Cluster_Index] = frame.data[3] & 0x7F; 
	Cluster_Vrel[Cluster_Index] = (frame.data[4] & 0x07) * 256 + frame.data[5];


	Cluster_RCSValue[Cluster_Index] = (Cluster_RCSValue[Cluster_Index] * 0.5) - 50;
	Cluster_Range[Cluster_Index] = Cluster_Range[Cluster_Index] * 0.2;
	Cluster_Azimuth[Cluster_Index] = Cluster_Azimuth[Cluster_Index] * 2 - 90;
	Cluster_Vrel[Cluster_Index] = Cluster_Vrel[Cluster_Index] * 0.05 - 35;
	
	Cluster_Long[Cluster_Index] = Cluster_Range[Cluster_Index] * cos (Cluster_Azimuth[Cluster_Index] * dek_to_rad) ; //dek_to_rad PI/180
	Cluster_Lat[Cluster_Index] = Cluster_Range[Cluster_Index] * sin (Cluster_Azimuth[Cluster_Index] * dek_to_rad) ;

}


