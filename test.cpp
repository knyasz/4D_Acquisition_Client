#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "unistd.h"
#include "time.h"
#include "math.h"

#include "libfreenect.hpp"


#include "MyFreenectDevice.h"



using namespace cv;
using namespace std;

using namespace NUdpSocket;

//#define __CPU_VERSION__



int main(int argc, char **argv) {
	bool die(false);
	string filename("snapshot");
	string suffix(".png");
	int i_snap(0), iter(0);
	int depthCounter(0);
	time_t startTime(0.), currTime(0.);
	//FILE* sentData;
	//TUByte* buffer = new TUByte[KINECT_FRAME_SIZE];

	Mat depthFromKinect(Size(640, 480), CV_16UC1);
	Mat depthConvertedToShow(Size(640, 480), CV_8UC1);
	Mat rgbMat(Size(640, 480), CV_8UC3, Scalar(0));

	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
		// Freenect::Freenect<MyFreenectDevTSDWordice> freenect;
		// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

//	printf("the size of the color matrix "
//			"is %d just as rows*cols*sizeofchar*3channels %d",
//			rgbMat.rows*rgbMat.cols*rgbMat.elemSize(),640*480*sizeof(uchar)*3);
	device.InitDepthUdpSocket();

	namedWindow("rgb", CV_WINDOW_AUTOSIZE);
	namedWindow("depth", CV_WINDOW_AUTOSIZE);
	//namedWindow("regularDepth", CV_WINDOW_AUTOSIZE);

//	device.startVideo();
	device.startDepth();
	time(&startTime);
	while (!die) {


		{// stream depth
			while(!device.getDepth(depthConvertedToShow)){;}
//			cv::imshow("depth", depthConvertedToShow);
//			/*
//			 * A common mistake for opencv newcomers
//			 * is to call cv::imshow() in a loop through video frames,
//			 * without following up each draw with cv::waitKey(30).
//			 * In this case, nothing appears on screen,
//			 * because highgui is never given time
//			 * to process the draw requests from cv::imshow()
//			 */
//			cvWaitKey(1);
			while(!device.sendKinectFrameUDP(
					static_cast<TUByte*>(depthConvertedToShow.data),
					CHUNK_SIZE,
					KINECT_FRAME_SIZE,
					DEPTH)){;}
		}


//		{// Get frames only test = 30 FPS :)
//			while (!device.IsDepthFrameReadyDrop()){}
//		}

		{//stream RGB
			while(!device.getVideo(rgbMat)){;}
//			cv::imshow("rgb", rgbMat);
//			/*
//			 * A common mistake for opencv newcomers
//			 * is to call cv::imshow() in a loop through video frames,
//			 * without following up each draw with cv::waitKey(30).
//			 * In this case, nothing appears on screen,
//			 * because highgui is never given time
//			 * to process the draw requests from cv::imshow()
//			 */
//			cvWaitKey(1);
			device.sendKinectFrameUDP(
					static_cast<TUByte*>(rgbMat.data),
					CHUNK_SIZE,
					KINECT_FRAME_SIZE*3,
					RGB);
		}


//		device.getDepth(depthFromKinect);

//		device.getColorDist(rgbMat);

//		device.getDepthWithDist(depthConvertedToShow);
//		device.getDepth(depthConvertedToShow);


//		depthFromKinect.convertTo(depthConvertedToShow, CV_8UC1, 255.0/2048.0);
//		cv::imshow("depth", depthConvertedToShow);
//		cv::imshow("regularDepth", depthConvertedToShow);

//		device.sendData(reinterpret_cast<TUByte*>(depthConvertedToShow.data),KINECT_FRAME_SIZE);

//		device.sendData(c,15000);
//		cv::imwrite("GrayImg.jpg",depthConvertedToShow);

//		device.sendKinectFrameUDP(
//				static_cast<TUByte*>(depthConvertedToShow.data),
//				CHUNK_SIZE,
//				KINECT_FRAME_SIZE);


//		++depthCounter;
//		if (abs(difftime(startTime, time(&currTime))) >= 1){ //if time passed one sec
//			printf("I successfully sent (%d) frame at this second \n",
//					depthCounter);
//			depthCounter = 0;
//			startTime = time(NULL);
//		}


//		getchar();
//		sentData = fopen("sentData.bin","wb");
//		fwrite(depthf.data,sizeof(char),KINECT_FRAME_SIZE,sentData);
//		fclose(sentData);


//		char k = cvWaitKey(1);
//		if (k == 27) {
//			cvDestroyWindow("rgb");
//			cvDestroyWindow("depth");
//			break;
//		}
//		if (k == 8) {
//			std::ostringstream file;
//			file << filename << i_snap << suffix;
//			cv::imwrite(file.str(), rgbMat);
//			i_snap++;
//		}
//		if (iter >= 1000)
//			break;
//		iter++;
	}

	device.stopVideo();
	device.stopDepth();
	return 0;
}

