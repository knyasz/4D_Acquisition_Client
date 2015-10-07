#include <vector>
#include <cmath>
#include <pthread.h>
#include "unistd.h"


//CUDA includes
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

// System includes
#include <iostream>
#include "time.h"
#include "math.h"

#include "libfreenect.hpp"


#include "MyFreenectDevice.h"
using namespace std;
using namespace cv;

// MPI include
#include <mpi.h>

// Shut down MPI cleanly if something goes wrong
void my_abort(int err)
{
    cout << "Test FAILED\n";
    MPI_Abort(MPI_COMM_WORLD, err);
}
// Error handling macros
#define MPI_CHECK(call) \
    if((call) != MPI_SUCCESS) { \
        cerr << "MPI error calling \""#call"\"\n"; \
        my_abort(-1); }



int main(int argc, char **argv) {


    // Initialize MPI state
    MPI_CHECK(MPI_Init(&argc, &argv));

    // Get our MPI node number and node count
    int commSize, commRank;
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &commSize));
    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &commRank));

    cout<<"MPI commSize = "<< commSize << endl;
    cout<<"MPI commRank = "<< commRank << endl;

    if(commRank != 0){

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

    MPI_CHECK(MPI_Finalize());

    if (commRank == 0)
    {
        cout << "PASSED\n";
    }

    return 0;
}
