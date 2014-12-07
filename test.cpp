#include "libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "udpSocket.h"
#include "unistd.h"
#include "time.h"
#include "math.h"
#include "dtm.h"


using namespace cv;
using namespace std;
using namespace NUdpSocket;

const TUDWord CHUNK_SIZE(60000); //60k byte
const TUDWord DELAY_SEND(6000); //U SEC
const TUDWord KINECT_ROWS(480);
const TUDWord KINECT_COLS(640);

TUDWord countGlobal;

class myMutex {
	public:
		myMutex() {
			pthread_mutex_init( &m_mutex, NULL );
		}
		void lock() {
			pthread_mutex_lock( &m_mutex );
		}
		void unlock() {
			pthread_mutex_unlock( &m_mutex );
		}
	private:
		pthread_mutex_t m_mutex;
};


class MyFreenectDevice : public Freenect::FreenectDevice {
	public:
		MyFreenectDevice(freenect_context *_ctx, int _index)
	 		: Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
			m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false),
			m_new_depth_frame(false), depthMat(Size(640,480),CV_16UC1),
			rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
			ownMat(Size(640,480),CV_8UC3,Scalar(0)) {
			
			for( unsigned int i = 0 ; i < 2048 ; i++) {
				float v = i/2048.0;
				v = std::pow(v, 3)* 6;
				m_gamma[i] = v*6*256;
			}
		}
		
		// Do not call directly even in child
		void VideoCallback(void* _rgb, uint32_t timestamp) {
			//std::cout << "RGB callback" << std::endl;
			m_rgb_mutex.lock();
			uint8_t* rgb = static_cast<uint8_t*>(_rgb);
			rgbMat.data = rgb;
			m_new_rgb_frame = true;
			m_rgb_mutex.unlock();
		};
		
		// Do not call directly even in child
		void DepthCallback(void* _depth, uint32_t timestamp) {
			//std::cout << "Depth callback" << std::endl;
			static time_t startTime(time(NULL));
			m_depth_mutex.lock();
			uint16_t* depth = static_cast<uint16_t*>(_depth);
			depthMat.data = (uchar*) depth;
			
			m_new_depth_frame = true;
			++countGlobal;
			if (abs(difftime(startTime,time(NULL))) >= 1) //if time passed one sec
			{
			//printf("I successfully sent (%d) frame at this second \n",countGlobal);
			countGlobal = 0;
			//getchar();
			startTime = time(NULL);
			}
			m_depth_mutex.unlock();
		}
		
		bool getVideo(Mat& output) {
			m_rgb_mutex.lock();
			if(m_new_rgb_frame) {
				cv::cvtColor(rgbMat, output, CV_RGB2BGR);
				m_new_rgb_frame = false;
				m_rgb_mutex.unlock();
				return true;
			} else {
				m_rgb_mutex.unlock();
				return false;
			}
		}
		
		bool getDepth(Mat& output) {
				Mat dtmMat(Size(640,480),CV_16UC1);
				m_depth_mutex.lock();
				if(m_new_depth_frame) {
			
					dtmGpu(depthMat.data,dtmMat.data,KINECT_ROWS,KINECT_COLS,1,2);
					dtmMat.convertTo(output,CV_8UC1, 255.0/2048.0);
					m_new_depth_frame = false;
					m_depth_mutex.unlock();
					return true;
				} else {
					m_depth_mutex.unlock();
					return false;
				}
			}

		bool getWorldDataColor(Mat& output) {
						Mat dtmMat(Size(640,480),CV_16UC1);
						static Mat rotMat(Size(4,4),CV_64F);
						static Mat transMat(Size(4,4),CV_64F);
						static Mat final(Size(4,4),CV_64F);
						bool first(true);

						if (first)
						{
							double rot[] = {9.9984628826577793e-01f, -1.4779096108364480e-03f, 1.7470421412464927e-02f, 0 ,
								1.2635359098409581e-03f, 9.9992385683542895e-01f, -1.2251380107679535e-02f , 0 ,
								1.7470421412464927e-02f, 1.2275341476520762e-02f, 9.9977202419716948e-01f, 0,
								0,						 0,  					 0, 						1};
							double trans[] = {0,0,0,1.9985242312092553e-02f,
										  0,0,0,-7.4423738761617583e-04f,
										  0,0,0,-1.0916736334336222e-02f,
										  0,0,0,1};
							rotMat.data = (uchar*)rot;
							transMat.data = (uchar*)trans;
							cv::multiply(rotMat,transMat,final);
							first = false;
						}


						m_depth_mutex.lock();
						if(m_new_depth_frame) {

							depthToWorldColorKernel(depthMat.data,final , dtmMat.data,KINECT_ROWS,KINECT_COLS,1,2);
							dtmMat.convertTo(output,CV_8UC1, 255.0/2048.0);
							m_new_depth_frame = false;
							m_depth_mutex.unlock();
							return true;
						} else {
							m_depth_mutex.unlock();
							return false;
						}
					}

		bool getColorDist(Mat& output) {
				Mat dtmMat(Size(640,480),CV_8UC3);
				m_depth_mutex.lock();
				m_rgb_mutex.lock();
				if(m_new_depth_frame) {
					
				dtmGpuColor(depthMat.data,
		rgbMat.data,dtmMat.data,KINECT_ROWS,KINECT_COLS,1,2);
					cv::cvtColor(dtmMat, output, CV_RGB2BGR);
					m_new_depth_frame = false;
					m_depth_mutex.unlock();
					m_rgb_mutex.unlock();
					return true;
				} else {
					m_depth_mutex.unlock();
					m_rgb_mutex.unlock();
					return false;
				}
			}
		
		bool InitSocket(){
			bool rv(true);
			SSocketConfig conf("10.0.0.2","10.0.0.1",50555,50555,KINECT_FRAME_SIZE,"IR Video Img");
			rv = rv && m_udpSocket.configureSocket(conf);
			rv = rv && m_udpSocket.openSocket();
			
			return rv;
		}
		
		inline bool sendData(TUByte* buffer, TUDWord size){
			return m_udpSocket.sendData(buffer,size);
		}

                bool sendKinectFrameUDP(TUByte* buffer, TUDWord chunkSize, const TUDWord totSize){
			bool rv(false),status(true);
			TSDWord bytesLeft(totSize);
			TUDWord buffIndex(0);
			TUDWord packet(0);			

		        while ((status) && (bytesLeft > 0))
			{
			      TUDWord currChunk((bytesLeft - (TSDWord)chunkSize) > 0 ? chunkSize : bytesLeft);
			      usleep(DELAY_SEND);
			      if(sendData(buffer+buffIndex,currChunk))
			      {
				++packet;
				bytesLeft -= currChunk;
				buffIndex += currChunk;
				//printf("Sent pack (%d) a chunck of (%d) bytes \n theres (%d) data left \n",packet,currChunk,bytesLeft);
//getchar();
			      }
			      else
				{
				  printf("Couldnt Send the data\n");
				  status = false;
				}
			   		
			}
			
			if (status)
			{
				rv = true;
				//printf("Successfuly sent data of size (%d)\n",totSize);
			}
			
			return rv;
			
		}

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
};


int main(int argc, char **argv) {
	bool die(false);
	string filename("snapshot");
	string suffix(".png");
	int i_snap(0),iter(0);
	int depthCounter(0);
	time_t startTime(0.),currTime(0.);
//FILE* sentData;
	//TUByte* buffer = new TUByte[KINECT_FRAME_SIZE];

	Mat depthMat(Size(640,480),CV_16UC1);
	Mat depthf (Size(640,480),CV_8UC1);
	Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));
	
	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
	// Freenect::Freenect<MyFreenectDevice> freenect;
	// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:
	
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
	device.InitSocket();
	namedWindow("rgb",CV_WINDOW_AUTOSIZE);
	namedWindow("depth",CV_WINDOW_AUTOSIZE);
	device.startVideo();
	device.startDepth();
time(&startTime);
	while (!die) {
		//device.getVideo(rgbMat);
		//device.getDepth(depthMat);
		//device.getColorDist(rgbMat);
		device.getDepth(depthf);
		//cv::imshow("rgb", rgbMat);
		//depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
		cv::imshow("depth",depthf);
		//device.sendData(reinterpret_cast<TUByte*>(depthf.data),KINECT_FRAME_SIZE);
		//device.sendData(c,15000);
		//cv::imwrite("GrayImg.jpg",depthf);
		device.sendKinectFrameUDP(static_cast<TUByte*>(depthf.data),CHUNK_SIZE,KINECT_FRAME_SIZE);
++depthCounter;
if (abs(difftime(startTime,time(&currTime))) >= 1) //if time passed one sec
{
	
	//printf("I successfully sent (%d) frame at this second \n",depthCounter);
	depthCounter = 0;
//getchar();
startTime = time(NULL);
}
		//getchar();
		//sentData = fopen("sentData.bin","wb");
		//fwrite(depthf.data,sizeof(char),KINECT_FRAME_SIZE,sentData);
		//fclose(sentData);
		char k = cvWaitKey(5);
		if( k == 27 ){
			cvDestroyWindow("rgb");
			cvDestroyWindow("depth");
			break;
		}
		if( k == 8 ) {
			std::ostringstream file;
			file << filename << i_snap << suffix;
			cv::imwrite(file.str(),rgbMat);
			i_snap++;
		}
		if(iter >= 1000) break;
		iter++;
	}
	
	device.stopVideo();
	device.stopDepth();
	return 0;
}

