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

#include "MyEventsCounter.h"

#include "pthread.h"

using namespace cv;
using namespace std;

using namespace NUdpSocket;

static const uint PIPE_LENGTH(20);
static const uint RETRIES_LIMIT(10);


class MyClientRunner_I {
public:


	static void* AllocateAndSendFrameThread(void * This);
	static void* showAndDeallocateFrameThread(void * This);

public:
	MyClientRunner_I(MyFreenectDevice& device);
	virtual ~MyClientRunner_I(){
//		pthread_join(t_allocate_and_send,NULL);
//		pthread_join(t_show_and_deallocate,NULL);

	}
	virtual void JoinAllocateAndSend(){
		pthread_join(t_allocate_and_send,NULL);
	}
	virtual void JoinShowAndeallocate(){
		pthread_join(t_show_and_deallocate,NULL);
	}
	virtual bool AllocateAndSendFrameRun();
	virtual bool showAndDeallocateFrameRun();
	virtual void AllocateAndSendFrame() = 0;
protected:
	virtual bool showAndDeallocateFrameSucessfully();
	virtual void pushToPipe(Mat * pMat);
	virtual bool popFromPipeSucessfully(Mat ** ppMat);
	virtual void AllocateAndSendFrameLoop(){
		while(true){
			AllocateAndSendFrame();
		}
	}
	virtual void showAndDeallocateFrameLoop(){
		while(true){
			showAndDeallocateFrameSucessfully();
		}
	}
protected:
	string m_window_name;
	MyFreenectDevice& m_device;

	bool m_runner_is_initialized;
	MyUDPStreamer m_udp_streamer;
	queue<Mat*> m_pipe;
	MyMutex m_pipe_mutex;

	pthread_t t_allocate_and_send;
	pthread_t t_show_and_deallocate;

	MyEventsCounter mAllocateCounter;
	MyEventsCounter mShowCounter;

};





#endif /* MYCLIENTRUNNERI_H_ */
