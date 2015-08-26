/*
 * MyClientRGBRunner.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#include "MyClientRGBRunner.h"



using namespace cv;

MyClientRGBRunner::MyClientRGBRunner(MyFreenectDevice& device):
		MyClientRunner_I(device){
	m_window_name = "RGB";
	SSocketConfig conf(	"10.0.0.2",
						"10.0.0.1",
						70777,
						70777,
						KINECT_FRAME_SIZE,
						"IR Video Img");
	if(!m_udp_streamer.InitSocket(conf)){
		throw Exception(
				CV_StsBackTrace,
				"Can't initialize streamer socket",
				"MyClientRGBRunner::MyClientRGBRunner",
				"MyClientRGBRunner",
				20);
	}
	namedWindow(m_window_name, CV_WINDOW_AUTOSIZE);
	m_device.startVideo();
	mAllocateCounter.Init("RGB send");
	mShowCounter.Init("RGB show");
	m_runner_is_initialized = true;

}

MyClientRGBRunner::~MyClientRGBRunner(){
	m_device.stopVideo();
	destroyWindow(m_window_name);
}
void MyClientRGBRunner::AllocateAndSendFrame (){
	Mat * pMat;
	if(!m_runner_is_initialized){
		throw Exception(
				CV_StsBackTrace,
				"Runner is not initialized",
				"MyClientRGBRunner::AllocateAndSendFrame",
				"MyClientRGBRunner",
				17);
	}
	pMat = new Mat(Size(640, 480), CV_8UC3,	Scalar(0));
	uint retries=0;
	while (!m_device.getVideo(*pMat)){
//		cout<<endl<<"can't get RGB "<<retries++<<endl;
		if(retries>RETRIES_LIMIT){
			usleep(33);
			delete pMat;
			return;
		}
		pthread_yield();;
	}

	//NEVER push to pipe before sending !!!
	while(	!m_udp_streamer.sendKinectFrameUDP(
								static_cast<TUByte*>(pMat->data),
								CHUNK_SIZE,
								KINECT_FRAME_SIZE)				){;}
	pushToPipe(pMat);
	mAllocateCounter.PrintoutEventsCounted();

}
