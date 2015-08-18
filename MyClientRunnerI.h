/*
 * MyClientRunnerI.h
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#ifndef MYCLIENTRUNNERI_H_
#define MYCLIENTRUNNERI_H_

#include "MyUDPStreamer.h"
#include "MyFreenectDevice.h"
#include <libfreenect.hpp>
#include "MyMutex.h"
#include <queue>

using namespace cv;
using namespace std;

using namespace NUdpSocket;

static const uint PIPE_LENGTH(10);
static const Freenect::Freenect freenect;

class MyClientRunner_I {
public:
//	static MyFreenectDevice& m_device;
//	static void SetDevice();
public:
	MyFreenectDevice& m_device = freenect.createDevice<MyFreenectDevice>(0);
//	static bool m_device_is_set;
//	void static SetDevice();
public:
	MyClientRunner_I();
	virtual ~MyClientRunner_I(){;}
	virtual void AllocateAndSendFrame() = 0;
	virtual void showAndDeallocateFrame();
	virtual void Run();
protected:
	virtual void pushToPipe(Mat * pMat);
	virtual bool popFromPipeSucessfully(Mat ** ppMat);
protected:
	string m_window_name;

	bool m_runner_is_initialized;
	MyUDPStreamer m_udp_streamer;
	queue<Mat*> m_pipe;
	MyMutex m_pipe_mutex;





};


#endif /* MYCLIENTRUNNERI_H_ */
