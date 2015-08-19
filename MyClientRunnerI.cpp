/*
 * MyClientRunnerI.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#include "MyClientRunnerI.h"


void* MyClientRunner_I::AllocateAndSendFrameThread(void * This){
	((MyClientRunner_I *)This)->AllocateAndSendFrameLoop();
	return NULL;
}
void* MyClientRunner_I::showAndDeallocateFrameThread(void * This){
	((MyClientRunner_I *)This)->showAndDeallocateFrameLoop();
	return NULL;
}

MyClientRunner_I::MyClientRunner_I(MyFreenectDevice& device):
	m_runner_is_initialized(false),
	m_device(device){;}



void MyClientRunner_I::showAndDeallocateFrame(){
	if(!m_runner_is_initialized){
		throw Exception(
				CV_StsBackTrace,
				"Runner is not initialized",
				"MyClientRunner_I::showAndDeallocateFrame",
				"MyClientRunner_I",
				17);
	}
	Mat * frameToShow;
	while(!popFromPipeSucessfully(&frameToShow)){;}
	cv::imshow(m_window_name, *frameToShow);
	/*
	 * A common mistake for opencv newcomers
	 * is to call cv::imshow() in a loop through video frames,
	 * without following up each draw with cv::waitKey(30).
	 * In this case, nothing appears on screen,
	 * because highgui is never given time
	 * to process the draw requests from cv::imshow()
	 */
	cvWaitKey(1);
	delete frameToShow;
}
void MyClientRunner_I::pushToPipe(Mat * pMat){
	m_pipe_mutex.lock();
		if (m_pipe.size()<PIPE_LENGTH){
			m_pipe.push(pMat);
		}else{
			delete pMat;
		}
	m_pipe_mutex.unlock();
}
bool MyClientRunner_I::popFromPipeSucessfully(Mat ** ppMat){
	m_pipe_mutex.lock();
		if (!m_pipe.empty()){
			*ppMat= m_pipe.back();
			m_pipe.pop();
		}
	m_pipe_mutex.unlock();
	return true;
}
/** Returns true if the thread was successfully started,
 *  false if there was an error starting the thread */
bool MyClientRunner_I::AllocateAndSendFrameRun(){
	return (	pthread_create(
								&t_allocate_and_send,
								NULL,
								AllocateAndSendFrameThread,
								this)
															) == 0;
}
/** Returns true if the thread was successfully started,
 *  false if there was an error starting the thread */
bool MyClientRunner_I::showAndDeallocateFrameRun(){
	return (	pthread_create(
								&t_show_and_deallocate,
								NULL,
								showAndDeallocateFrameThread,
								this)
															) == 0;
}

