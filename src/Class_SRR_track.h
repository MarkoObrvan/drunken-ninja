#ifndef CLASS_SRR_TRACK_H
#define CLASS_SRR_TRACK_H
 
class SRRTrack
{
private:
	
	unsigned short int ID;
	unsigned short int Radar_Status;
	unsigned short int CAN1_TrackStatus;
	unsigned short int CAN1_Track_1;
	unsigned short int CAN1_Track_2;

	
	
	//Track Status
	unsigned short int NumOfTracks;
	unsigned short int TrackSt_RollCount;
	
	//Track1
	unsigned short int Track_ID;
	float Track_LongDispl; 
	float Track_LatDispl;
	unsigned short int Track_Index;
	float Track_VrelLong; 
	float Track_VrelLat;
	unsigned short int Track1_RollCount;
	
	//Track2
	float Track_RCSValue;
	float Track_Lifetime; 
	unsigned short int Track_Index2;
	unsigned short int Track2_RollCount;
	
	
    SRRTrack() { 
	} // private default constructor
	
	
public:
    SRRTrack(int SRR_track_id);
 
    void SetSRRMsgID(int SRR_track_id);
	
	void SetTrackStatus(struct can_frame frame);
	
	void SetTrack1(struct can_frame &frame);
	
	void SetTrack2(struct can_frame &frame);
 
	void Display();
 
 
    int GetID() { return ID; }
	unsigned short int GetRadarStatus(){return Radar_Status;}
	unsigned short int GetTrackStatus(){return CAN1_TrackStatus;}
	unsigned short int GetTrack1(){return CAN1_Track_1;}
	unsigned short int GetTrack2(){return CAN1_Track_2;}
	unsigned short int GetIndex(){ return Track_Index;}
	float GetLongDispl(){ return Track_LongDispl;}
	float GetLatDispl(){ return Track_LatDispl;}

};
 
#endif