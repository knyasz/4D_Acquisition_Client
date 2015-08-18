/*
 * MyClientRunnerI.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#include "MyClientRunnerI.h"


MyClientRunner_I::MyClientRunner_I():m_runner_is_initialized(false){;}

//void MyClientRunner_I::SetDevice(){
//	if(m_device_is_set){
//		return;
//	}
//	Freenect::Freenect freenect;
//	m_device ( freenect.createDevice<MyFreenectDevice>(0) );
//}


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
void MyClientRunner_I::Run(){
	//TODO insert threads
	while(true){
		AllocateAndSendFrame();
		showAndDeallocateFrame();
	}
}

