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


//	cout<<"Depth.AllocateAndSendFrameRun=========== "
//			<<Depth.AllocateAndSendFrameRun();
//	cout<<"Depth.showAndDeallocateFrameRun=========== "
//			<<Depth.showAndDeallocateFrameRun();
//
//
//	cout<<"RGB.AllocateAndSendFrameRun=========== "
//			<<RGB.AllocateAndSendFrameRun()<<endl;
//	cout<<"RGB.showAndDeallocateFrameRun=========== "
//			<<RGB.showAndDeallocateFrameRun()<<endl;

	while(true){
		RGB.AllocateAndSendFrame();
//		RGB.AllocateAndSendFrame();
//		RGB.showAndDeallocateFrame();
		Depth.AllocateAndSendFrame();
//		Depth.AllocateAndSendFrame();
//		Depth.showAndDeallocateFrame();
	}

	return 0;
}

