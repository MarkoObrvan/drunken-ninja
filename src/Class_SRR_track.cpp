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

void SRRTrack::Display()
{
	cout << "long:" << Track_LongDispl << "    lat: " << Track_LatDispl << "    index: " << Track_Index << endl; 
	//cout << "NumOfTracks:" << NumOfTracks<< "    TrackSt_RollCount: " << TrackSt_RollCount << endl; 
}


SRRTrack::SRRTrack(int SRR_track_id)
{
    SetSRRMsgID(SRR_track_id);
}

void SRRTrack::SetSRRMsgID(int SRR_track_id)
{
    int id_no=(SRR_track_id % 0x100)/0x10;
	Radar_Status = 0x60A + id_no*0x10;
	CAN1_TrackStatus = 0x60B + id_no*0x10;
	CAN1_Track_1 = 0x60C + id_no*0x10;
	CAN1_Track_2 = 0x60D + id_no*0x10;
}


void SRRTrack::SetTrackStatus(struct can_frame frame)
{

	NumOfTracks = frame.data[0];
	TrackSt_RollCount = frame.data[1] & 0x03;

}

void SRRTrack::SetTrack1(struct can_frame &frame)
{

	Track_ID = frame.data[0]*256 + frame.data[1]; //OK
	Track_LongDispl = (frame.data[2] & 0x3F) * 8 + (frame.data[3] >> 5); //OK
	Track_LatDispl = frame.data[4] * 4 + (frame.data[5] >> 6); //OK
	Track_Index = frame.data[3] & 0x1F; //OK
	Track_VrelLong = (frame.data[5] & 0x3F) * 64 + (frame.data[6] >> 2); //OK
	Track_VrelLat = frame.data[7]; //OK
	Track1_RollCount = frame.data[6] & 0x03;
	//track_1.refresh_var = 0;

	Track_LongDispl = Track_LongDispl * 0.1;
	Track_LatDispl = Track_LatDispl * 0.1 - 51.1;
	Track_VrelLong = Track_VrelLong * 0.02 - 35;
	Track_VrelLat = Track_VrelLat * 0.25 - 32;

}


void SRRTrack::SetTrack2(struct can_frame &frame)
{

	Track_RCSValue = frame.data[0];
	Track_Lifetime = frame.data[1]*256 + frame.data[2]; 
	Track_Index2 = frame.data[3] & 0x1F;
	Track2_RollCount = frame.data[4] & 0x03;

	Track_RCSValue = Track_RCSValue / 2;
	Track_Lifetime = Track_Lifetime / 10;

}

