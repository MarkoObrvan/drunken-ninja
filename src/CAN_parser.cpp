/*READ BEFORE USAGE!
if OS wont recognise canX (X-0,1,2,3,4,5...) install drivers from 
http://www.peak-system.com/fileadmin/media/linux/index.htm

Please make sure you set baud rate of canX!
sudo ip link set can0 up txqueuelen 1000 type can bitrate 500000

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


#include "Class_SRR_track.h"
#include "ClassSRRCluster.h"
#include "ClassARSTarget.h"

using namespace std;
typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;


int main(int argc, char** argv){


	/*
	Initializing CAN over socket, just take it for granted, it works
	there are few extra steps that are not needed at every computer
	*/
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	char *ifname = "can0";
	int s, nbytes;
	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Error while opening socket");
		return -1;
	}
 
	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);
 
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex; 
	addr.can_ifindex = 0; 
 
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
	
 	ros::init(argc, argv, "SRR_node_2");

	ros::NodeHandle nh;

	ros::Publisher pub = nh.advertise<sensor_msgs::PointCloud2> ("output1", 10);
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
	
	
	ros::start();
	
	
	
	SRRTrack track1(0x270);
	
	cout << "radar status id " << hex << track1.GetRadarStatus() << endl;
	cout << "track status id " << hex <<  track1.GetTrackStatus() << endl;
	cout << "track can1 id " << hex <<  track1.GetTrack1() << endl;
	cout << "track can2 id " << hex <<  track1.GetTrack2() << endl;
	
	
	SRRCluster cluster1(0x270);
	
	ARSTarget target1(0x200);
	
	
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
		*/	
		if (track1.SRRMsgCheckout(frame)){
cout << "Ne ulazi track" << endl;			
			for (int i=0; i<track1.GetNumOfTracks(); i++)
			{
				msg->points.push_back (pcl::PointXYZ(track1.GetLongDispl(i), track1.GetLatDispl(i), 0.0));
				msg->height = 0;
				msg->width = 0;
				msg->header.stamp =  g;
				pcl::toROSMsg(*msg, output);				
			}
			
			pub.publish (output);
			msg->points.clear();

		}
		
		
		/*SRR CLUSTER*/
		if (cluster1.SRRMsgCheckout(frame)){
cout << "Ne ulazi cluster" << endl;
			for (int i=0; i<cluster1.GetNumOfClusters(); i++)
			{
				msg->points.push_back (pcl::PointXYZ(cluster1.GetLongDispl(i), cluster1.GetLatDispl(i), 0.0));
				msg->height = 0;
				msg->width = 0;
				msg->header.stamp =  g;
				pcl::toROSMsg(*msg, output2);				
			}
			
			pub.publish (output2);
			msg->points.clear();

		}
		
		/*ARS TARGET*/
		if (target1.ARSMsgCheckout(frame)){
cout << "number of targets: "<< target1.GetSumOfTargets() <<"\n" << endl;
			for (int i=0; i<target1.GetSumOfTargets(); i++)
			{
cout << "long disp: " << target1.GetLongDispl(i) << "   lat displ: " <<  target1.GetLatDispl(i) << endl; 
				msg->points.push_back (pcl::PointXYZ(target1.GetLongDispl(i), target1.GetLatDispl(i), 0.0));
				msg->height = 0;
				msg->width = 0;
				msg->header.stamp =  g;
				pcl::toROSMsg(*msg, OutTarget1);				
			}
			
			pub.publish (OutTarget1);
			msg->points.clear();

		}
		
		
		/*
		deprecated code, will be removed in late versions
		if (frame.can_id == track1.GetTrackStatus())
		{
			track1.SetTrackStatus(frame);
		}
		else if (frame.can_id == track1.GetTrack1())
		{
			if(track1.GetIndex() == 0)
			{
					pub.publish (output);
					msg->points.clear();
					cout << "PUBLISHED2" << endl;
			}
			track1.SetTrack1(frame);
		}
		else if (frame.can_id == track1.GetTrack2())
		{
			track1.SetTrack2(frame);
			track1.Display();
			msg->points.push_back (pcl::PointXYZ(track1.GetLongDispl(), track1.GetLatDispl(), 0.0));
					//cout << "X axis long: " << track_1.Track_LongDispl << "Y axis lat: " << track_1.Track_LatDispl << endl;
			msg->height = 0;
			msg->width = 0;

			msg->header.stamp =  g;
			pcl::toROSMsg(*msg, output);
		}
		*/
		
		
	
	}
	
	
	
	
	ros::spin();
	ros::shutdown();
	return 0; 

}