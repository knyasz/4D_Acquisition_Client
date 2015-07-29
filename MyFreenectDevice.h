/*
 * MyFreenectDevice.h
 *
 *  Created on: Jun 7, 2015
 *      Author: alexandalex
 */

#ifndef MYFREENECTDEVICE_H_
#define MYFREENECTDEVICE_H_

#include <libfreenect.hpp>

#include <pthread.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "unistd.h"
#include "time.h"
#include "math.h"

#include "udpSocket.h"
//#include "udpSocket64bit.a"
#include "dtm.h"

using namespace cv;
using namespace std;

using namespace NUdpSocket;

#define __CPU_VERSION__

class myMutex {
public:
	myMutex() {
		pthread_mutex_init(&m_mutex, NULL);
	}
	void lock() {
		pthread_mutex_lock(&m_mutex);
	}
	void unlock() {
		pthread_mutex_unlock(&m_mutex);
	}
private:
	pthread_mutex_t m_mutex;
};

static const TUDWord CHUNK_SIZE(60000); //60k byte
static const TUDWord DELAY_SEND(6000); //U SEC
static const TUDWord KINECT_ROWS(480);
static const TUDWord KINECT_COLS(640);

static TUDWord countGlobal;

class MyFreenectDevice: public Freenect::FreenectDevice {
public:
	MyFreenectDevice(freenect_context *_ctx, int _index);
	// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp);
	// Do not call directly even in child
	void DepthCallback(void* _depth, uint32_t timestamp);
	bool getVideo(Mat& output);
	bool getDepthWithDist(Mat& output);
	bool getWorldDataColor(Mat& output) ;
	float dtmCpu(uint16* inMat, uint16* outMat, uchar minRange,uchar maxRange);
	bool getDepth(Mat& output);
	bool getColorDist(Mat& output);

	bool InitSocket();
	inline bool sendData(TUByte* buffer, TUDWord size) {
		return m_udpSocket.sendData(buffer, size);
	}

	bool sendKinectFrameUDP(	TUByte* buffer,
								TUDWord chunkSize,
								const TUDWord totSize);


private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	Mat depthMat;
	Mat rgbMat;
	Mat ownMat;
	myMutex m_rgb_mutex;
	myMutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
	CUdpSocket m_udpSocket;

//	MyFreenectDevice();
//	virtual ~MyFreenectDevice();
};

#endif /* MYFREENECTDEVICE_H_ */
