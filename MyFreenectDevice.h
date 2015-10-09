/*
 * MyFreenectDevice.h
 *
 *  Created on: Jun 7, 2015
 *      Author: alexandalex
 */

#ifndef MYFREENECTDEVICE_H_
#define MYFREENECTDEVICE_H_

#include <libfreenect.hpp>


#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "unistd.h"
#include "time.h"
#include "math.h"



#include "udpSocket.h"
#include "dtm.h"
#include "MyMutex.h"

using namespace cv;
using namespace std;

using namespace NUdpSocket;





static const TUDWord DELAY_SEND(6000); //U SEC
static const TUDWord KINECT_ROWS(480);
static const TUDWord KINECT_COLS(640);

static uint depthCounter = 0;
static time_t startTime = 0.;
static time_t currTime = 0.;
static void showFPS(){
	++depthCounter;
	if (abs(difftime(startTime, time(&currTime))) >= 1){ //if time passed one sec
		printf("I successfully sent (%d) frame at this second \n",depthCounter);
		depthCounter = 0;
		startTime = time(NULL);
	}
}

static TUDWord countGlobal;

class MyFreenectDevice: public Freenect::FreenectDevice {KINECT_FRAME_GRAY_SIZE
public:


	MyFreenectDevice(freenect_context *_ctx, int _index);
	// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp);
	bool getVideo(Mat& output);
	bool getColorDist(Mat& output);
	bool getWorldDataColor(Mat& output) ;

	// Do not call directly even in child
	void DepthCallback(void* _depth, uint32_t timestamp);
	bool getDepth(Mat& output);
	bool getDepthWithDist(Mat& output);
	bool IsDepthFrameReadyDrop();

	float dtmCpu(uint16* inMat, uint16* outMat, uchar minRange,uchar maxRange);











private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;



	Mat depthMat;
	Mat rgbMat;
	Mat ownMat;
	MyMutex m_rgb_mutex;
	MyMutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;


//	MyFreenectDevice();
	virtual ~MyFreenectDevice(){

	}
};

#endif /* MYFREENECTDEVICE_H_ */
