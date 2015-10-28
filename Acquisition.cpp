#include <stdint.h>//for uint8_t type
#include <vector>
#include <cmath>
#include <pthread.h>
#include "unistd.h"


//CUDA includes
//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>

// System includes
#include <iostream>
#include <cstring>
#include <string>
#include "stdlib.h"//for system()
#include "time.h"
#include "math.h"

#include "libfreenect.hpp"


//#include "MyFreenectDevice.h"
using namespace std;
//using namespace cv;

// MPI include
#include <mpi.h>
#define BUFFSIZE 10000

// Shut down MPI cleanly if something goes wrong
void my_abort(int err)
{
    cout << "Test FAILED\n";
    MPI_Abort(MPI_COMM_WORLD, err);
}
string GetIP(const char * command){
	string str("empty");
	FILE * fp = popen(command, "r");
	if (!fp){
		return str;
	}
	char *p=NULL, *e; size_t n;
	while ((getline(&p, &n, fp) > 0) && p){}
	fclose(fp);
	return str = p;

}
// Error handling macros
#define MPI_CHECK(call) \
    if((call) != MPI_SUCCESS) { \
        cerr << "MPI error calling \""#call"\"\n"; \
        my_abort(-1); }



int main(int argc, char **argv) {
	/*
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
	namedWindow("depth", CV_WINDOW_AUTOSIZE);
	device.startDepth();
	Mat depthConvertedToShow(Size(640, 480), CV_8UC1);
	while(trstatic_cast<TUByte*>(pMat->data)ue){// stream depth
		while(!device.getDepth(depthConvertedToShow)){;}
		cv::imshow("depth", depthConvertedToShow);
		 //A common mistake for opencv newcomers
		 //is to call cv::imshow() in a loop through video frames,
		 //without following up each draw with cv::waitKey(30).
		 //In this case, nothing appears on screen,
		 //because highgui is never given time
		 //to process the draw requests from cv::imshow()
		cvWaitKey(1);
	}
	cvDestroyWindow("depth");
	*/

     // Initialize MPI state
    MPI_CHECK(MPI_Init(&argc, &argv));

    // Get our MPI node number and node count
    int commSize, commRank;
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &commSize));
    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &commRank));

    int size = BUFFSIZE;
    char *buff = new char[BUFFSIZE];
    MPI_Buffer_attach( buff, size);
    /* a buffer of 10000 bytes can now be used by MPI_Bsend */

		//    MPI_Buffer_attach( malloc(BUFFSIZE), BUFFSIZE);
			/* a buffer of 10000 bytes can now be used by MPI_Bsend */
		//    MPI_Buffer_detach( &buff, &size);
		//    /* Buffer size reduced to zero */
		//    MPI_Buffer_attach( buff, size);
		//    /* Buffer of 10000 bytes available again */





    const int ROOT_ID = 0;
    const int DepthFrameTag = 100;
    MPI_Status status;
//	Mat depthConvertedToShow(Size(640, 480), CV_8UC1);

	const uint KINECT_FRAME_SIZE = 640*480;
	uint8_t * pFrame = NULL;

	char msgToSend[200];
	char* msgToReceive;
	const int msgTag=200;




	try{
		if(commRank != ROOT_ID){

//			Freenect::Freenect freenect;
//			MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);


		//	device.startVideo();
//			device.startDepth();



//			while(true)
//			{// stream depth
//				while(!device.getDepth(depthConvertedToShow)){;}
				cout<<"MPI commSize = "<< commSize << "  MPI commRank = "<<
						commRank << GetIP("ifconfig | grep addr:10")<<endl;
				printf("commRanc[%d] got frame from kinect. \n",commRank);


				strcpy(msgToSend,"Hello");
				MPI_Bsend(	msgToSend,
							8,
							MPI_CHAR,
							ROOT_ID,
							msgTag,
							MPI_COMM_WORLD);


//				MPI_Send(	(const void*)depthConvertedToShow.data,
//							KINECT_FRAME_SIZE,
//							MPI_BYTE,
//							ROOT_ID,
//							DepthFrameTag,
//							MPI_COMM_WORLD);
				printf("commRanc[%d] sent frame from kinect. \n",commRank);
//			}



		}else{//ROOT_ID
			cout<<"MPI commSize = "<< commSize << "  MPI commRank From root = "
					<< commRank << GetIP("ifconfig | grep addr:10")<<endl;
			//	namedWindow("rgb", CV_WINDOW_AUTOSIZE);
//			namedWindow("depth", CV_WINDOW_AUTOSIZE);
//			while (true){
			int senderID=1;
//				for(int senderID=1;senderID<commSize;++senderID){
					msgToReceive = new char[200];
					sleep(2);
					printf("commRanc[%d] start receiving frame from [%d]. \n",
							commRank,senderID);
					MPI_Recv(	msgToReceive,
								8,
								MPI_CHAR,
								senderID,
								msgTag,
								MPI_COMM_WORLD,
								& status);
//					MPI_Recv(	(void*)depthConvertedToShow.data,
//								KINECT_FRAME_SIZE,
//								MPI_BYTE,
//								senderID,
//								DepthFrameTag,
//								MPI_COMM_WORLD,
//								& status);
				printf("commRanc[%d] received frame from [%d]. \n",
						commRank,senderID);
				printf("%s. \n",msgToReceive);
				delete[]  msgToReceive;
//					{//Stream Depth
//						cv::imshow("depth", depthConvertedToShow);
//						 //A common mistake for opencv newcomers
//						 //is to call cv::imshow() in a loop through video frames,
//						 //without following up each draw with cv::waitKey(30).
//						 //In this case, nothing appears on screen,
//						 //because highgui is never given time
//						 //to process the draw requests from cv::imshow()
//						cvWaitKey(1);
//					}

					//	{//stream RGB
					//		while(!device.getVideo(rgbMat)){;}
					//		cv::imshow("rgb", rgbMat);
					//		  //A common mistake for opencv newcomers
					//		  //is to call cv::imshow() in a loop through video frames,
					//		  //without following up each draw with cv::waitKey(30).
					//		  //In this case, nothing appears on screen,
					//		  //because highgui is never given time
					//		  //to process the draw requests from cv::imshow()
					//		cvWaitKey(1);
					//	}
//				}
//			}
//			cvDestroyWindow("depth");
	//		cvDestroyWindow("rgb");

		}
	}catch(std::runtime_error &e){
		cout<<"Exception: "<<commRank<<"  :  "<<e.what()<<endl;
	}


    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    MPI_CHECK(MPI_Finalize());

    if (commRank == 0)
    {
        cout << "PASSED\n";
    }
    return 0;
}
