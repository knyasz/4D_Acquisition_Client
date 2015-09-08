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
	MyClientRGBRunner RGB(device);
	MyClientDepthRunner Depth(device);


	if(!Depth.AllocateAndSendFrameRun()){
		return 1;
	}
	if(!Depth.showAndDeallocateFrameRun()){
		return 1;
	}
//	if(!RGB.AllocateAndSendFrameRun()){
//		return 1;
//	}
//	if(!RGB.showAndDeallocateFrameRun()){
//		return 1;
//	}


//	while(true){
//		Depth.AllocateAndSendFrame();
//		RGB.AllocateAndSendFrame();
//	}





	Depth.JoinAllocateAndSend();
	Depth.JoinShowAndeallocate();
//	RGB.JoinAllocateAndSend();
//	RGB.JoinShowAndeallocate();

	return 0;
}

