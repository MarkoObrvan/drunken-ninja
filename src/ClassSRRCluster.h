#ifndef CLASS_SRR_CLUSTER_H
#define CLASS_SRR_CLUSTER_H
 
class SRRCluster
{
private:
	
	unsigned short int ID;
	unsigned short int Radar_Status;
	unsigned short int CAN1_Cluster_Status;
	unsigned short int CAN1_Cluster_1;
	
	//CLuster Status
	unsigned short int NoOfClusters;
	int ClusterSt_RollCount;
	
	//Cluster1
	int Cluster_Index;
	int Cluster_RollCount[128];
	float Cluster_RCSValue[128];
	float Cluster_Range[128];
	float Cluster_Azimuth[128]; 
	float Cluster_Vrel[128];	
	
	//Cluster coordinates
	float Cluster_Long[128];
	float Cluster_Lat[128];
	
	
    SRRCluster() { 
	} // private default constructor
	
	
public:
    SRRCluster(int SRR_track_id);
 
	int SRRMsgCheckout ( struct can_frame frame);
 
    void SetSRRMsgID(int SRR_track_id);
	
	void SetClusterStatus(struct can_frame frame);
	
	void SetCluster1(struct can_frame frame);
	
 
	void Display();
 
 
    int GetID() { return ID; }

	unsigned short int GetClusterStatus(){return CAN1_Cluster_Status;}
	unsigned short int GetCluster1(){return CAN1_Cluster_1;}
	unsigned short int GetIndex(){ return Cluster_Index;}
	unsigned short int GetNumOfClusters(){ return NoOfClusters;}
	float GetLongDispl(int i){ return Cluster_Long[i];}
	float GetLatDispl(int i){ return Cluster_Lat[i];}

};
 
#endif