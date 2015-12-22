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

MyClientRunner_I::MyClientRunner_I(	MyFreenectDevice& device,
									MyUDPStreamer& udp_streamer):
	m_runner_is_initialized(false),
	m_device(device),
	m_udp_streamer(udp_streamer){;}



bool MyClientRunner_I::showAndDeallocateFrameSucessfully(){
	if(!m_runner_is_initialized){
		throw Exception(
				CV_StsBackTrace,
				"Runner is not initialized",
				"MyClientRunner_I::showAndDeallocateFrame",
				"MyClientRunner_I",
				17);
	}
	bool result = true;
	Mat * frameToShow = NULL;
	uint retries=0;
	while(!popFromPipeSucessfully(&frameToShow)){
//		cout<<endl<<"cant pop no "<<retries++<<endl;
		if(retries>RETRIES_LIMIT){
//			cout<<endl<<"Popper going to sleep"<<endl;
			usleep(33);
			return false;
		}
		pthread_yield();
	}
	if (!frameToShow){
		return false;
	}
	if (frameToShow->empty()){
		return false;
	}
	try {
		cv::imshow(m_window_name, *frameToShow);
	} catch (Exception& e) {
		const char* err_msg = e.what();
		std::cout << "exception caught: imshow:\n" << err_msg << std::endl;
		result=false;
	}
	/*
	 * A common mistake for opencv newcomers
	 * is to call cv::imshow() in a loop through video frames,
	 * without following up each draw with cv::waitKey(33).
	 * In this case, nothing appears on screen,
	 * because highgui is never given time
	 * to process the draw requests from cv::imshow()
	 * 1/30 = 33msec
	 */
//	cvWaitKey(33);
	cvWaitKey(1);
	delete frameToShow;
	mShowCounter.PrintoutEventsCounted();
	return result;
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
//	cout<<endl<<"popping, pipeLength is "<<m_pipe.size()<<endl;
	bool result = false;
		if (!m_pipe.empty()){
			*ppMat= m_pipe.back();
			m_pipe.pop();
			result = true;
		}
	m_pipe_mutex.unlock();
	return result;
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

