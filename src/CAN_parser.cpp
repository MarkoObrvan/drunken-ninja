/*READ BEFORE USAGE!
if OS wont recognise canX (X-0,1,2,3,4,5...) install drivers from 
http://www.peak-system.com/fileadmin/media/linux/index.htm

Please make sure you set baud rate of canX!
sudo ip link set can0 up txqueuelen 1000 type can bitrate 500000
sudo ip link set can1 up txqueuelen 1000 type can bitrate 500000

compile with
this compile wont work on latest version. use cmake and make, then run exe. 
Left it here cause i dont need to search for some commands during development
g++ SOCKET.cpp -o socket_node -I/opt/ros/hydro/include -L/opt/ros/hydro/lib -Wl,-rpath,/opt/ros/hydro/lib -lroscpp -lrosconsole -lrostime
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <math.h>
 
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <ros/ros.h>


#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <tf/transform_broadcaster.h>
#include <pcl/io/pcd_io.h>

#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
 
#include "std_msgs/Float32MultiArray.h"


#include "Class_SRR_track.h"
#include "ClassSRRCluster.h"
#include "ClassARSTarget.h"
#include "ClassARSObject.h"

using namespace std;
typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;


int main(int argc, char *argv[]){


	string temps;
 	ros::init(argc, argv, temps);
	ros::NodeHandle nh("~");

	nh.getParam("can", temps);
	
	std::cout<< "Param: " <<temps<<endl;
	
	/*
	Initializing CAN over socket, just take it for granted, it works
	there are few extra steps that are not needed at every computer
	*/
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
//	char *ifname = argv[1];
	char *ifname = const_cast<char*> (temps.c_str());
	int s, nbytes;
	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Error while opening socket");
		return -1;
	}
 
	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);
	cout << "name: " << ifname << "  index: " <<  ifr.ifr_ifindex << endl;



	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex; 
	//addr.can_ifindex = 0; 
 
	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);
 
	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Error in socket bind");
		return -2;
	}
	


	
	/*
	Seting up ros node and sensor messages
	frame_id must be the same for each message type
	if we ant to print on samo rviz window
	for using other windows use other frame names 
	and separated windows. msg2 is left as example, there will
	be no usage of it in rest of code.
	*/


	string outSwitcher[4];
	string outSwitcherSpeed[4];
	
	if (temps.compare("can1") == 0){
		outSwitcher[0] = "can1_ClusterPC";
		outSwitcher[1] = "can1_output2";
		outSwitcher[2] = "can1_TrackPC";
		outSwitcher[3] = "can1_output4";
		outSwitcherSpeed[0] = "can1_Cluster";
		outSwitcherSpeed[1] = "can1_Track";
	}
		
	else
	{
		outSwitcher[0] = "can0_ClusterPC";
		outSwitcher[1] = "can0_output2";
		outSwitcher[2] = "can0_TrackPC";
		outSwitcher[3] = "can0_output4";
		outSwitcherSpeed[0] = "can0_Cluster";
		outSwitcherSpeed[1] = "can0_Track";
	}
	
	
	/*
	Separated publishers are MUST
	if we want to keep display in rviz at the same time and
	separated one scan from another!
	*/
	ros::Publisher pub1 = nh.advertise<sensor_msgs::PointCloud2> (outSwitcher[0], 10);
	ros::Publisher pub2 = nh.advertise<sensor_msgs::PointCloud2> (outSwitcher[1], 10);
	ros::Publisher pub3 = nh.advertise<sensor_msgs::PointCloud2> (outSwitcher[2], 10);
	ros::Publisher pub4 = nh.advertise<sensor_msgs::PointCloud2> (outSwitcher[3], 10);
	
	
	ros::Publisher pubVel = nh.advertise<std_msgs::Float32MultiArray>("Cluster_vrel", 128);
	ros::Publisher pubAng = nh.advertise<std_msgs::Float32MultiArray>("Cluster_angl_deg", 128);
	ros::Publisher pubRCS = nh.advertise<std_msgs::Float32MultiArray>("Cluster_rcs", 128);
	std_msgs::Float32MultiArray vel;
	std_msgs::Float32MultiArray ang;
	std_msgs::Float32MultiArray rcs;
	
	
	
	ros::Publisher pubVel_long = nh.advertise<std_msgs::Float32MultiArray>("Track_vrel_long", 32);
	ros::Publisher pubVel_lat = nh.advertise<std_msgs::Float32MultiArray>("Track_vrel_lat", 32);
	ros::Publisher pubRCS_track = nh.advertise<std_msgs::Float32MultiArray>("Track_rcs", 32);
	ros::Publisher pubLife = nh.advertise<std_msgs::Float32MultiArray>("Track_lifetime", 32);
	std_msgs::Float32MultiArray vel_long;
	std_msgs::Float32MultiArray vel_lat;
	std_msgs::Float32MultiArray life;
	
	
	
	PointCloud::Ptr msg (new PointCloud);
	msg->height = msg->width = 1;
	msg->header.frame_id = "socketcanframe2";

	
	/*
	Separated output sensor messages are MUST
	if we want to keep scans from separated sensors
	*/
	sensor_msgs::PointCloud2 output;
	sensor_msgs::PointCloud2 output2;
	sensor_msgs::PointCloud2 OutTarget1;
	sensor_msgs::PointCloud2 OutObject1;
	
	
	ros::start();
	
	
	
	SRRTrack track1(0x270);
	
	SRRCluster cluster1(0x270);
	
	ARSTarget target1(0x200);
	
	ARSObject object1(0x200);
	
	
	int testrun = 1000, g=0;
	while (testrun)
	{
		g++;
		//testrun--;
		
		nbytes = read(s, &frame, sizeof(struct can_frame));
		
		if (nbytes < 0) 
		{
			perror("can raw socket read");
			return 1;
		} 
		
		else if (nbytes < sizeof(struct can_frame)) 
		{
			fprintf(stderr, "read: incomplete CAN frame\n");
		}
		
		/*there is display in class_srr_track.cpp 
		for printing gathered informations
		used to compared published informatons in 
		this section. For additional info about class calls
		check class file.
		
		Sends whole frame and does parsing and saves data from frame. 
		If frame is last, then true is returned, in which case function can
		iterate trough colected data and push it on output.
		
		
	ros::Publisher pubVel_long = nh.advertise<std_msgs::Float32MultiArray>("Track_vrel_long", 32);
	ros::Publisher pubVel_lat = nh.advertise<std_msgs::Float32MultiArray>("Track_vrel_lat", 32);
	ros::Publisher pubRCS_track = nh.advertise<std_msgs::Float32MultiArray>("Track_rcs", 32);
	ros::Publisher pubLife = nh.advertise<std_msgs::Float32MultiArray>("Track_lifetime", 32);
	std_msgs::Float32MultiArray vel_long;
	std_msgs::Float32MultiArray vel_lat;
	std_msgs::Float32MultiArray life;
		
		
		*/	
		if (track1.SRRMsgCheckout(frame)){
cout << "Uazi track:  " + temps << endl;			
			for (int i=0; i<track1.GetNumOfTracks(); i++)
			{
				msg->points.push_back (pcl::PointXYZ(track1.GetLongDispl(i), track1.GetLatDispl(i), 0.0));
				msg->height = 0;
				msg->width = 0;
				msg->header.stamp =  g;
				pcl::toROSMsg(*msg, output);				
			}
			
			pub3.publish (output);
			msg->points.clear();

			vel_long.data.clear();
			life.data.clear();
			vel_lat.data.clear();
			rcs.data.clear();
			
			for (int i=0; i<track1.GetNumOfTracks(); i++)
			{

				vel_long.data.push_back(track1.GetVrelLong(i));
				vel_lat.data.push_back(track1.GetVrelLat(i));
				life.data.push_back(track1.GetLifetime(i));
				rcs.data.push_back(track1.GetRCS(i));
			}
			
			pubVel_long.publish(vel_long);
			pubVel_lat.publish(vel_lat);
			pubRCS_track.publish(rcs);
			pubLife.publish(life);
			
		}
		
		
		/*SRR CLUSTER*/
		if (cluster1.SRRMsgCheckout(frame)){
cout << "Ulazi cluster:  " + temps << endl;
			
			int iter = cluster1.GetNumOfClusters();
			
			for (int i=0; i<iter; i++)
			{
//cout << "long disp: " << cluster1.GetLongDispl(i) << "   lat displ: " <<  cluster1.GetLatDispl(i) << endl;
				msg->points.push_back (pcl::PointXYZ(cluster1.GetLongDispl(i), cluster1.GetLatDispl(i), 0.0));
				msg->height = 0;
				msg->width = 0;
				msg->header.stamp =  g;
				pcl::toROSMsg(*msg, output2);				
			}
//cout << endl;
			
			pub1.publish (output2);
			msg->points.clear();
			
			vel.data.clear();
			ang.data.clear();
			rcs.data.clear();
			
			for (int i=0; i<iter; i++)
			{

				vel.data.push_back(cluster1.GetVel(i));
				ang.data.push_back(cluster1.GetAngl(i));
				rcs.data.push_back(cluster1.GetRCS(i));
			}
			
			pubVel.publish(vel);
			pubAng.publish(ang);
			pubRCS.publish(rcs);

		}
		
		/*ARS TARGET*/
		if (target1.ARSMsgCheckout(frame)){
//cout << "number of targets: "<< target1.GetSumOfTargets() <<"\n" << endl;
			for (int i=0; i<target1.GetSumOfTargets(); i++)
			{
//cout << "long disp: " << target1.GetLongDispl(i) << "   lat displ: " <<  target1.GetLatDispl(i) << endl; 
				msg->points.push_back (pcl::PointXYZ(target1.GetLongDispl(i), target1.GetLatDispl(i), 0.0));
				msg->height = 0;
				msg->width = 0;
				msg->header.stamp =  g;
				pcl::toROSMsg(*msg, OutTarget1);				
			}
			
			pub4.publish (OutTarget1);
			msg->points.clear();

		}
		
		/*ARS OBJECT*/
		if (object1.ARSMsgCheckout(frame)){

			for (int i=0; i<object1.GetSumOfObjects(); i++)
			{
 
				msg->points.push_back (pcl::PointXYZ(object1.GetLongDispl(i), object1.GetLatDispl(i), 0.0));
				msg->height = 0;
				msg->width = 0;
				msg->header.stamp =  g;
				pcl::toROSMsg(*msg, OutObject1);				
			}
			pub2.publish (OutObject1);
			msg->points.clear();

		}
		
		

		
		
	
	}
	
	
	
	
	ros::spin();
	ros::shutdown();
	return 0; 

}
