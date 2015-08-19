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
	while(true){
		RGB.AllocateAndSendFrameRun();
		Depth.AllocateAndSendFrameRun();
		RGB.showAndDeallocateFrameRun();
		Depth.showAndDeallocateFrameRun();
	}


	return 0;
}

