#include "Class_SRR_track.h"


SRRTrack::SRRTrack(int SRR_track_id)
{
    SetSRRMsgID(SRR_track_id);
}



void SRRTrack::SetSRRMsgID(int SRR_track_id)
{
    int id_no=(SRR_track_id % 0x100)/0x10;
	Radar_Status = 0x60A + id_no*0x10;
	CAN1_ClusterStat = 0x70B + id_no*0x10;
	CAN1_Cluster_1 = 0x70C + id_no*0x10;
	CAN1_TrackStatus = 0x60B + id_no*0x10;
	CAN1_Track_1 = 0x60C + id_no*0x10;
	CAN1_Track_2 = 0x60D + id_no*0x10;
}