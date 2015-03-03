#ifndef CLASS_ARS_OBJECT_H
#define CLASS_ARS_OBJECT_H
 
class ARSObject
{
private:
	
	unsigned short int ID;
	unsigned short int Radar_Status;
	unsigned short int Object_Status;
	unsigned short int CAN1_Object_1;
	unsigned short int CAN1_Object_2;
	
	
	/*Object STATUS... a bit of too much info...
	begins here...
	*/
	int NoOfObjects;
	int MeasCounter;
	/*
	Object STATUS used  exclusive ONLY for the 
	automotive applications and NOT for 
	industrial functions. 
	*/
	unsigned short int SensorOutputReduced; 
	unsigned short int SensorExternalDisturbed; 
	unsigned short int SensorSwitchedOff; 
	unsigned short int SensorDefective;
	unsigned short int SensorSupplyVoltageLow;
	unsigned short int SensorRxInvalid;
	unsigned short int Reserved;
	unsigned short int InterfaceVersionNumber;
	unsigned short int NumOfLaneRight;
	unsigned short int NumOfLanesLeft;
	float LatDistToBorderLeft;
	float LatDistToBorderRight;
	/*Ends here*/
	
	

	/*Object 1 strcture*/
	unsigned short int Obj_RolCount[64]; 
	int Obj_ID;
	float Obj_LongDispl[64];
	float Obj_VrelLong[64];
	float Obj_AccelLong[64]; 
	unsigned short int Obj_ProbOfExist[64];
	unsigned short int Obj_DynProp[64];
	float Obj_LatDispl[64];
	short int Obj_Length[64];
	short int Obj_Width[64];
	short int Obj_MeasStat[64];

	
	/*Object 2 structure */
	float Obj_RCSVal[64];
	float Obj_LatSpeed[64];
	float Obj_ObstacleProbability[64]; 
	
	
    ARSObject() { 
	} // private default constructor
	
	
public:
    ARSObject(int ARS_object_id);
 
	int ARSMsgCheckout ( struct can_frame frame);
 
    void SetARSMsgID(int ARS_object_id);
	
	void SetObjectStatus(struct can_frame frame);
	
	void SetObject1(struct can_frame frame);
	
	void SetObject2(struct can_frame frame);

 
	void Display();
 
 
    int GetID() { return ID; }

	unsigned short int GetIndex(){ return Obj_ID;}
	unsigned short int GetSumOfObjects(){ return Sum_Of_Objects;}
	float GetLongDispl(int i){ return Obj_LongDispl[i];}
	float GetLatDispl(int i){ return Obj_LatDispl[i];}

};
 
#endif