#ifndef CLASS_ARS_TARGET_H
#define CLASS_ARS_TARGET_H
 
class ARSTarget
{
private:
	
	unsigned short int ID;
	unsigned short int Radar_Status;
	unsigned short int Target_Status;
	unsigned short int CAN1_Target_1;
	unsigned short int CAN1_Target_2;
	
	//Target Status
	int NoOfTargetsNear;
	int NoOfTargetsFar;
	
	//
	int Sum_Of_Targets; 

	//Target 1
	int NoOfTarget1;
	float Tar_Dist_Rms[96];
	float Tar_Ang_Rms[96];
	float Tar_Vrel_Rms[96];
	float Tar_Vrel[96];
	float Tar_Dist[96];

	//Target 2
	int NoOfTarget2;
	float Tar_PdH[96];
	float Tar_Length[96];
	float Tar_Widt[96];
	unsigned short int Tar_Ang_st[96];
	unsigned short int Tar_Typ[96];
	float Tar_Ang[96];
	float Tar_RCSValu[96];
	
	
	//Long and lat dist
	long longDispl[96];
	long latDispl[96];
	
    ARSTarget() { 
	} // private default constructor
	
	
public:
    ARSTarget(int ARS_target_id);
 
	int ARSMsgCheckout ( struct can_frame frame);
 
    void SetARSMsgID(int ARS_target_id);
	
	void SetTargetStatus(struct can_frame frame);
	
	void SetTarget1(struct can_frame frame);
	
	void SetTarget2(struct can_frame frame);

 
	void Display();
 
 
    int GetID() { return ID; }

	unsigned short int GetIndex(){ return NoOfTarget1;}
	unsigned short int GetSumOfTargets(){ return Sum_Of_Targets;}
	float GetLongDispl(int i){ return longDispl[i];}
	float GetLatDispl(int i){ return latDispl[i];}

};
 
#endif