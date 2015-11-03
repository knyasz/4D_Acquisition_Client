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
    MPI_Status status;
    MPI_Request request;
	const int msgTag=200;
	int * msg = new int(0);
	int sendReceiveFlag=0;




	try{
		if(commRank != ROOT_ID){
			printf("commRanc[%d], MPI commSize = %d, my IP  = %s\n",
					commRank,commSize,GetIP("ifconfig | grep addr:10").c_str());
			printf("commRanc[%d] Start sending... \n",commRank);
			*msg=55;
//			MPI_Isend(	msg,1,MPI_INT,ROOT_ID,msgTag,MPI_COMM_WORLD,&request);
//			MPI_Send(	msg,1,MPI_INT,ROOT_ID,msgTag,MPI_COMM_WORLD);
			MPI_Bsend(	msg,1,MPI_INT,ROOT_ID,msgTag,MPI_COMM_WORLD);
			printf("commRanc[%d] sent frame . \n",commRank);
		}else{//ROOT_ID
			printf("commRanc[%d], MPI commSize = %d, my IP  = %s\n",
					commRank,commSize,GetIP("ifconfig | grep addr:10").c_str());
			int senderID=1;
			printf("commRanc[%d] start receiving frame from [%d]. \n",
					commRank,senderID);
//			MPI_Irecv(	msg,1,MPI_INT,senderID,msgTag,MPI_COMM_WORLD,& request);
			MPI_Recv(	msg,1,MPI_INT,senderID,msgTag,MPI_COMM_WORLD,&status);

			printf("commRanc[%d] received frame from [%d]. \n",
					commRank,senderID);
//			while(!sendReceiveFlag){
//				MPI_Iprobe(senderID,msgTag,MPI_COMM_WORLD,&sendReceiveFlag,&status);
//				printf("commRanc[%d] got flag[%d] \n",commRank,sendReceiveFlag);
//				sleep(1);
//			}
		}
//		MPI_Wait(&request,&status);
		if (commRank != ROOT_ID){
			printf("commRanc[%d]  Sent msg - %d. \n",commRank,*msg);
		}else{
			printf("commRanc[%d]  Received msg- %d. \n",commRank,*msg);
		}
		MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
		delete  msg;
		MPI_CHECK(MPI_Finalize());
	}catch(std::runtime_error &e){
		cout<<"Exception: "<<commRank<<"  :  "<<e.what()<<endl;
	}catch(...){
		cout<<"some exception"<<endl;
	}



    if (commRank == 0)
    {
        cout << "PASSED\n";
    }
    return 0;
}
