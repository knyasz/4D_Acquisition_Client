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

#include <queue>

#include "udpSocket.h"
#include "dtm.h"
#include "MyMutex.h"

using namespace cv;
using namespace std;

using namespace NUdpSocket;

typedef enum {
	DEPTH,
	RGB
}TFrame;

static const uint PIPE_LENGTH(10);
static const TUDWord CHUNK_SIZE(60000); //60k byte
static const TUDWord DELAY_SEND(6000); //U SEC
static const TUDWord KINECT_ROWS(480);
static const TUDWord KINECT_COLS(640);

static uint depthCounter = 0;
static time_t startTime = 0.;
static time_t currTime = 0.;
static void showFPS(){
	++depthCounter;
	if (abs(difftime(startTime, time(&currTime))) >= 1){ //if time passed one sec
		printf("I successfully sent (%d) frame at this second \n",
				depthCounter);
		depthCounter = 0;
		startTime = time(NULL);
	}
}

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
	bool IsDepthFrameReadyDrop();
	bool getColorDist(Mat& output);

	bool InitDepthUdpSocket();
	bool InitRgbUdpSocket();
	inline bool InitSocket(TFrame frameType) {
		if (frameType==DEPTH){
			return InitDepthUdpSocket();
		}
		return InitRgbUdpSocket();
	}
	inline bool sendData(TUByte* buffer, TUDWord size,TFrame frameType) {
		if (frameType==DEPTH){
			return m_udp_depth_socket.sendData(buffer, size);
		}
		return m_udp_rgb_socket.sendData(buffer, size);
	}

	bool sendKinectFrameUDP(	TUByte* buffer,
								TUDWord chunkSize,
								const TUDWord totSize,
								TFrame frameType );

	void allocateAndSendFrame(TFrame frameType);
	void showAndDeallocateFrame(TFrame frameType);
private:
	void pushDepthPipe(Mat * pMat);
	void pushRgbPipe(Mat * pMat);
	bool popDepthPipeSucessfully(Mat ** ppMat);
	bool popRgbPipeSucessfully(Mat ** ppMat);




private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;

	string depth_window_name;
	queue<Mat*> m_depth_pipe;
	MyMutex m_depth_pipe_mutex;

	string rgb_window_name;
	queue<Mat*> m_RGB_pipe;
	MyMutex m_RGB_pipe_mutex;
	Mat depthMat;
	Mat rgbMat;
	Mat ownMat;
	MyMutex m_rgb_mutex;
	MyMutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
	CUdpSocket m_udp_depth_socket;
	CUdpSocket m_udp_rgb_socket;

//	MyFreenectDevice();
	virtual ~MyFreenectDevice(){
		stopVideo();
		stopDepth();
	}
};

#endif /* MYFREENECTDEVICE_H_ */
