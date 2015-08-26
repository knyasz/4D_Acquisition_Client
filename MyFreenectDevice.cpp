/*
 * MyFreenectDevice.cpp
 *
 *  Created on: Jun 7, 2015
 *      Author: alexandalex
 */

#include "MyFreenectDevice.h"



//MyFreenectDevice::MyFreenectDevice() {
//	// TODO Auto-generated constructor stub
//
//}

//MyFreenectDevice::~MyFreenectDevice() {
//	// TODO Auto-generated destructor stub
//}

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index) :
		Freenect::FreenectDevice(_ctx, _index),
		m_buffer_depth(FREENECT_DEPTH_11BIT),
		m_buffer_rgb(FREENECT_VIDEO_RGB),
		m_gamma(2048),
		m_new_rgb_frame(false),
		m_new_depth_frame(false),
		depthMat(Size(640, 480), CV_16UC1),
		rgbMat(Size(640, 480), CV_8UC3,	Scalar(0)),

		ownMat(Size(640, 480), CV_8UC3, Scalar(0)){

//	startVideo();
//	startDepth();

	for (unsigned int i = 0; i < 2048; i++) {
		float v = i / 2048.0;
		v = std::pow(v, 3) * 6;
		m_gamma[i] = v * 6 * 256;
	}
}

void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp) {
	//std::cout << "RGB callback" << std::endl;
	m_rgb_mutex.lock();
	if(_rgb){
		uint8_t* rgb = static_cast<uint8_t*>(_rgb);
		rgbMat.data = rgb;
		m_new_rgb_frame = true;
	}
	m_rgb_mutex.unlock();
}
void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
	//std::cout << "Depth callback" << std::endl;
//	static time_t startTime(time(NULL));
	m_depth_mutex.lock();
	if(_depth){
		uint16_t* depth = static_cast<uint16_t*>(_depth);
		depthMat.data = (uchar*) depth;

		m_new_depth_frame = true;
	}
//	++countGlobal;
//	if (abs(difftime(startTime, time(NULL))) >= 1) //if time passed one sec
//			{
//		//printf("I successfully sent (%d) frame at this second \n",countGlobal);
//		countGlobal = 0;
//		//getchar();
//		startTime = time(NULL);
//	}
	m_depth_mutex.unlock();
}

bool MyFreenectDevice::getVideo(Mat& output) {
	bool result = false;
	m_rgb_mutex.lock();
	if (m_new_rgb_frame) {
		cv::cvtColor(rgbMat, output, CV_RGB2BGR);
		m_new_rgb_frame = false;
		result =  true;;
	}
	m_rgb_mutex.unlock();
	return result;
}

bool MyFreenectDevice::getDepth(Mat& output) {
	bool result = false;
	m_depth_mutex.lock();
	if(m_new_depth_frame) {
		depthMat.convertTo(output, CV_8UC1, 255.0/2048.0);
		//depthMat.copyTo(output);
		m_new_depth_frame = false;
		result =  true;
	}
	m_depth_mutex.unlock();
	return result;
}
bool MyFreenectDevice::IsDepthFrameReadyDrop(){
	bool result = false;
	m_depth_mutex.lock();
	if(m_new_depth_frame) {
		m_new_depth_frame = false;
		result = true;
	}
	m_depth_mutex.unlock();
	return result;
}

bool MyFreenectDevice::getDepthWithDist(Mat& output) {
		Mat dtmMat(Size(640, 480), CV_16UC1);
		float ms1, ms2;
		static float totalmsg(0.f),totalmsc(0.f);
		static uint32 count(0);
		m_depth_mutex.lock();
		if (m_new_depth_frame) {
#ifdef __CPU_VERSION__
			ms1 = dtmCpu(reinterpret_cast<uint16*>(depthMat.data),
					reinterpret_cast<uint16*>(dtmMat.data), 2, 2.5);
			//printf("The Cpu version took me %f millisecond\n", ms1);
			ms2 = dtmGpu(depthMat.data, dtmMat.data, KINECT_ROWS, KINECT_COLS,
					2, 2.5);
			totalmsg += ms2;
			totalmsc += ms1;
			++count;
			if (count == 100){
		   		printf("average time to perform from gpu is: %lf and cpu is: %lf\n the gpu is x%lf faster the cpu\n",
		   				totalmsg/(float)count, totalmsc/(float)count,totalmsc/totalmsg);
				totalmsg = 0.f;
				totalmsc = 0.f;
				count = 0;
				//getchar();

			}
			SSocketConfigifeq ($(OS_ARCH),armv7l)

			//printf("the Gpu version took me millisecond %fl\n", ms2);
#else
			dtmGpu(depthMat.data,dtmMat.data,KINECT_ROWS,KINECT_COLS,1,2);
#endif
			dtmMat.convertTo(output, CV_8UC1, 255.0 / 2048.0);
			m_new_depth_frame = false;
			m_depth_mutex.unlock();
			return true;
		}
		m_depth_mutex.unlock();
	return false;
}

bool MyFreenectDevice::getWorldDataColor(Mat& output) {
		Mat dtmMat(Size(640, 480), CV_16UC1);
		static Mat rotMat(Size(4, 4), CV_64F);
		static Mat transMat(Size(4, 4), CV_64F);
		static Mat final(Size(4, 4), CV_64F);
		bool first(true);

		if (first) {
			double rot[] = { 9.9984628826577793e-01f, -1.4779096108364480e-03f,
					1.7470421412464927e-02f, 0, 1.2635359098409581e-03f,
					9.9992385683542895e-01f, -1.2251380107679535e-02f, 0,
					1.7470421412464927e-02f, 1.2275341476520762e-02f,
					9.9977202419716948e-01f, 0, 0, 0, 0, 1 };
			double trans[] = { 0, 0, 0, 1.9985242312092553e-02f, 0, 0, 0,
					-7.4423738761617583e-04f, 0, 0, 0, -1.0916736334336222e-02f,
					0, 0, 0, 1 };
			rotMat.data = (uchar*) rot;
			transMat.data = (uchar*) trans;
			cv::multiply(rotMat, transMat, final);
			first = false;
		}

		m_depth_mutex.lock();
		if (m_new_depth_frame) {

			depthToRgbWorldPoint(
					depthMat.data,
					reinterpret_cast<float*>(final.data),
					dtmMat.data,
					KINECT_ROWS,
					KINECT_COLS,
					2,
					2.5);
			dtmMat.convertTo(output, CV_8UC1, 255.0 / 2048.0);
			m_new_depth_frame = false;
			m_depth_mutex.unlock();
			return true;
		}
		m_depth_mutex.unlock();
	return false;

}

float MyFreenectDevice::dtmCpu(uint16* inMat, uint16* outMat, uchar minRange,uchar maxRange) {
	clock_t start, end;
	double miliseconds(0);
	start = clock();

	//uint16** inMat = reinterpret_cast<uint16**>(inImg);
	//uint16** outMat = reinterpret_cast<uint16**>(outImg);

	for (uint16 rows(0); rows < KINECT_ROWS; ++rows) {
		for (uint16 cols(0); cols < KINECT_COLS; ++cols) {
			uint32 index(rows * KINECT_COLS + cols);
			if (inMat[index] < 2047) {
				float tmp = static_cast<float>(1.0
						/ static_cast<double>(inMat[index] * -0.0030711016
								+ 3.330949516));

				if ((tmp >= minRange) && (tmp <= maxRange)) {
					outMat[index] = static_cast<uint16>(tmp);
				} else {
					outMat[index] = 0;
				}
			}
		}
	}

	end = clock();
	miliseconds = (float) ((((float) end - (float) start) / 1000000.f)
			* 1000.f);
	return miliseconds;
}

bool MyFreenectDevice::getColorDist(Mat& output) {
		Mat dtmMat(Size(640, 480), CV_8UC3);
		m_depth_mutex.lock();
		m_rgb_mutex.lock();
		if (m_new_depth_frame) {
			cv::cvtColor(rgbMat, output, CV_RGB2BGR);
			dtmGpuColor(reinterpret_cast<uint16*>(depthMat.data), output.data,
					output.data, KINECT_ROWS, KINECT_COLS, 2, 2.5, 3);
			m_new_depth_frame = false;
			m_depth_mutex.unlock();
			m_rgb_mutex.unlock();
			return true;
		}
		m_depth_mutex.unlock();
		m_rgb_mutex.unlock();
	return false;
}

























