/*READ BEFORE USAGE!
if OS wont recognise canX (X-0,1,2,3,4,5...) install drivers from 
http://www.peak-system.com/fileadmin/media/linux/index.htm

Please make sure you set baud rate of canX!
sudo ip link set can0 up txqueuelen 1000 type can bitrate 500000

compile with
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
 


#include "Class_SRR_track.h"

using namespace std;

int main(){

	SRRTrack track1(0x200);
	
	cout << "radar status id " << hex << track1.GetRadarStatus() << endl;
	cout << "track status id " << hex <<  track1.GetTrackStatus() << endl;
	cout << "track can1 id " << hex <<  track1.GetTrack1() << endl;
	cout << "track can2 id " << hex <<  track1.GetTrack2() << endl;
	
	return 0; 

}