/*
 * MyClientRunnerI.h
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#ifndef MYCLIENTRUNNERI_H_
#define MYCLIENTRUNNERI_H_

#include "MyFreenectUDPStreamer.h"
#include "MyMutex.h"
#include <queue>

using namespace cv;
using namespace std;

using namespace NUdpSocket;

static const uint PIPE_LENGTH(10);

class MyClientRunner_I {
public:
	MyClientRunner_I():m_runner_is_initialized(false){;}
	virtual ~MyClientRunner_I(){;}
	virtual void AllocateAndSendFrame() = 0;
	virtual void showAndDeallocateFrame();
protected:
	virtual void pushToPipe(Mat * pMat);
	virtual bool popFromPipeSucessfully(Mat ** ppMat);
	MyFreenectUDPStreamer m_udp_streamer;
	string m_window_name;
	queue<Mat*> m_pipe;
	MyMutex m_pipe_mutex;
	bool m_runner_is_initialized;



};

#endif /* MYCLIENTRUNNERI_H_ */
