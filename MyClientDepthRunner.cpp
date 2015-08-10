/*
 * MyClientDepthRunner.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#include "MyClientDepthRunner.h"

using namespace cv;

MyClientDepthRunner::MyClientDepthRunner(){
	m_window_name="Depth";
	SSocketConfig conf(	"10.0.0.2",
						"10.0.0.1",
						50555,
						50555,
						KINECT_FRAME_SIZE,
						"IR Video Img");
	if(!m_udp_streamer.InitSocket(conf)){
		throw Exception(
				CV_StsBackTrace,
				"Can't initialize streamer socket",
				"MyClientDepthRunner::MyClientDepthRunner",
				"MyClientDepthRunner",
				20);
	}
	m_runner_is_initialized = true;
}

void MyClientDepthRunner::AllocateAndSendFrame (){
	Mat * pMat;
	CUdpSocket *pSocket;
	if(!m_runner_is_initialized){
		throw Exception(
				CV_StsBackTrace,
				"Runner is not initialized",
				"MyClientDepthRunner::AllocateAndSendFrame",
				"MyClientDepthRunner",
				34);
	}
	pMat = new Mat(Size(640, 480), CV_16UC1);
	while (!m_udp_streamer.getDepth(*pMat)){;}

	//NEVER push to pipe before sending !!!
	while(	!m_udp_streamer.sendKinectFrameUDP(
								static_cast<TUByte*>(pMat->data),
								CHUNK_SIZE,
								KINECT_FRAME_SIZE)				){;}
	pushToPipe(pMat);

}
