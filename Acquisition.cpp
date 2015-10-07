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

int main(int argc, char **argv) {
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

//	namedWindow("rgb", CV_WINDOW_AUTOSIZE);
	namedWindow("depth", CV_WINDOW_AUTOSIZE);

//	device.startVideo();
	device.startDepth();

//	//Mat depthFromKinect(Size(640, 480), CV_16UC1);
	Mat depthConvertedToShow(Size(640, 480), CV_8UC1);
//	Mat rgbMat(Size(640, 480), CV_8UC3, Scalar(0));


	while(true){// stream depth
		while(!device.getDepth(depthConvertedToShow)){;}
		cv::imshow("depth", depthConvertedToShow);
		/*
		 * A common mistake for opencv newcomers
		 * is to call cv::imshow() in a loop through video frames,
		 * without following up each draw with cv::waitKey(30).
		 * In this case, nothing appears on screen,
		 * because highgui is never given time
		 * to process the draw requests from cv::imshow()
		 */
		cvWaitKey(1);
	}

//	{//stream RGB
//		while(!device.getVideo(rgbMat)){;}
//		cv::imshow("rgb", rgbMat);
//		/*
//		 * A common mistake for opencv newcomers
//		 * is to call cv::imshow() in a loop through video frames,
//		 * without following up each draw with cv::waitKey(30).
//		 * In this case, nothing appears on screen,
//		 * because highgui is never given time
//		 * to process the draw requests from cv::imshow()
//		 */
//		cvWaitKey(1);
//	}

//	cvDestroyWindow("rgb");
	cvDestroyWindow("depth");
}
