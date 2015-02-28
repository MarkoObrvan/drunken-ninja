#include "Class_SRR_track.h"

#include <stdio.h>
#include <iostream>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

using namespace std;


/*
Use mainly for debug, use as you wish
*/
void SRRTrack::Display()
{
	cout << "gather long:" << Track_LongDispl[Track_Index] << "    lat: " << Track_LatDispl[Track_Index] << "    index: " << Track_Index << "    index: " << Track_Index2 << endl; 
	//cout << "NumOfTracks:" << NumOfTracks<< "    TrackSt_RollCount: " << TrackSt_RollCount << endl; 
}


/*
Constructor, sets track message ID-s based on set up id using function call.
*/
SRRTrack::SRRTrack(int SRR_track_id)
{
    SetSRRMsgID(SRR_track_id);
}


/*
SRRMsgCheckout is class function that checks if given CAN frame belongs to given track class. 
Return 0 if does and saves all information from frame, 1 if it's last message of current cycle,
and -1 if message does not belong to given track.
*/
int SRRTrack::SRRMsgCheckout (struct can_frame frame)
{
	if (frame.can_id == CAN1_TrackStatus)
	{

			SetTrackStatus(frame);
			return 0; 
	}
	
	else if(frame.can_id == CAN1_Track_1)
	{

		SetTrack1(frame);
		return 0;
	}
	
	else if (frame.can_id == CAN1_Track_2)
	{

		SetTrack2(frame);
		
		//DISPLAY IS HEREEEE!!!!!
		Display();
		
		if (Track_Index2 == NumOfTracks-1) return 1;
		
		return 0;
	}
	
	return 0;
}



void SRRTrack::SetSRRMsgID(int SRR_track_id)
{
    int id_no=(SRR_track_id % 0x100)/0x10;
	Radar_Status = 0x60A + id_no*0x10;
	CAN1_TrackStatus = 0x60B + id_no*0x10;
	CAN1_Track_1 = 0x60C + id_no*0x10;
	CAN1_Track_2 = 0x60D + id_no*0x10;
	Track_Index = Track_Index2 = 0;
}


/*
Here we can see some data parsing from CAN SRR 2XX.
For more information check documentation.
Class saves only last message cycle.
*/

void SRRTrack::SetTrackStatus(struct can_frame frame)
{

	NumOfTracks = frame.data[0];
	TrackSt_RollCount = frame.data[1] & 0x03;

}

void SRRTrack::SetTrack1(struct can_frame &frame)
{

	Track_Index = frame.data[3] & 0x1F; //OK
	Track_ID[Track_Index] = frame.data[0]*256 + frame.data[1]; //OK
	Track_LongDispl[Track_Index] = (frame.data[2] & 0x3F) * 8 + (frame.data[3] >> 5); //OK
	Track_LatDispl[Track_Index] = frame.data[4] * 4 + (frame.data[5] >> 6); //OK
	Track_VrelLong[Track_Index] = (frame.data[5] & 0x3F) * 64 + (frame.data[6] >> 2); //OK
	Track_VrelLat[Track_Index] = frame.data[7]; //OK
	Track1_RollCount[Track_Index] = frame.data[6] & 0x03;
	//track_1.refresh_var = 0;

	Track_LongDispl[Track_Index] = Track_LongDispl[Track_Index] * 0.1;
	Track_LatDispl[Track_Index] = Track_LatDispl[Track_Index] * 0.1 - 51.1;
	Track_VrelLong[Track_Index] = Track_VrelLong[Track_Index] * 0.02 - 35;
	Track_VrelLat[Track_Index] = Track_VrelLat[Track_Index] * 0.25 - 32;

}


void SRRTrack::SetTrack2(struct can_frame &frame)
{

	Track_Index2 = frame.data[3] & 0x1F;
	Track_RCSValue[Track_Index2] = frame.data[0];
	Track_Lifetime[Track_Index2] = frame.data[1]*256 + frame.data[2]; 
	Track2_RollCount[Track_Index2] = frame.data[4] & 0x03;

	Track_RCSValue[Track_Index2] = Track_RCSValue[Track_Index2] / 2;
	Track_Lifetime[Track_Index2] = Track_Lifetime[Track_Index2] / 10;

}

