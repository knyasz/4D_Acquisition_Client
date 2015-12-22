/*
 * Client.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: alexandalex
 */


#include "MyClientDepthRunner.h"
#include "MyClientRGBRunner.h"

int main(int argc, char **argv) {
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
		SSocketConfig conf(	"10.0.0.2",
							"10.0.0.1",
							50555,
							50555,
							"Video socket");
	MyUDPStreamer udp_streamer;

	if(!udp_streamer.InitSocket(conf)){
		throw Exception(
				CV_StsBackTrace,
				"Can't initialize streamer socket",
				"main", __FILE__, __LINE__);
	}
	if(!udp_streamer.syncClientWithHost()){
		throw Exception(
						CV_StsBackTrace,
						"Can't sync with host",
						"main", __FILE__, __LINE__);
	}
	MyClientRGBRunner RGB(device, udp_streamer);
	MyClientDepthRunner Depth(device, udp_streamer);




	if(!Depth.AllocateAndSendFrameRun()){
		return 1;
	}
	if(!Depth.showAndDeallocateFrameRun()){
		return 1;
	}
	if(!RGB.AllocateAndSendFrameRun()){
		return 1;
	}
	if(!RGB.showAndDeallocateFrameRun()){
		return 1;
	}


//	while(true){
//		Depth.AllocateAndSendFrame();
//		RGB.AllocateAndSendFrame();
//	}





	Depth.JoinAllocateAndSend();
	Depth.JoinShowAndeallocate();
	RGB.JoinAllocateAndSend();
	RGB.JoinShowAndeallocate();

	return 0;
}

