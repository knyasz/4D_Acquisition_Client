/*
 * MyFreenectStreamer.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#include "MyUDPStreamer.h"
#include "Messages.h"
#include <chrono>

bool MyUDPStreamer::sendKinectFrameUDP(	TUByte* buffer,
											TUDWord chunkSize,
											const TUDWord totSize,
											EOpCodesSend opCode) {
	bool rv(false), status(true);
	TSDWord bytesLeft(totSize);
	TUDWord buffIndex(0);
	TUDWord packet(0);
        
        //send header before you send anything else
        NUdpMessages::SHeader header;
//        header.opCode = NUdpMessages::EOpCodesSend::OP_FRAME_DEP_SND;
        header.opCode = opCode;
        header.size = totSize;
        header.checksum = 0xabcdef00;
        sendData(reinterpret_cast<TUByte*>(&header), sizeof(NUdpMessages::SHeader));

	while ((status) && (bytesLeft > 0)) {
		TUDWord currChunk=
				(bytesLeft - (TSDWord) chunkSize) > 0 ?	chunkSize : bytesLeft;
//		usleep(DELAY_SEND);
		if (sendData(buffer + buffIndex, currChunk)) {
			++packet;
			bytesLeft -= currChunk;
			buffIndex += currChunk;
			//printf("Sent pack (%d) a chunck of (%d) bytes \n theres (%d) data left \n",packet,currChunk,bytesLeft);
			//getchar();
		} else {
			printf("Couldn't Send the data\n");
			status = false;
		}

	}

	if (status) {
		rv = true;
		//printf("Successfuly sent data of size (%d)\n",totSize);
	}

	return rv;

}
bool MyUDPStreamer::getFirstSyncFromHost(){
	SStart receiveBuffer;
	TSDWord uSecdelayTime = 10*1000;//10sec
	if (	! receiveData(	reinterpret_cast<TUByte*>(&receiveBuffer),
							sizeof(SStart),
							uSecdelayTime)				 ) {
		cout<<"MyUDPStreamer::getFirstSyncFromHost - receive timed out "<<endl;
		return false;
	}
	if (receiveBuffer.sync != SYNC  || receiveBuffer.opCode != OP_START_SND){
		printf("Message validation failed : \n");
		printf("Sync - Expected:Recived[%x:%x] \n",SYNC,receiveBuffer.sync);
		printf("Opcode - Expected:Recived[%d:%d] \n",OP_START_SND,receiveBuffer.opCode);
		return false;
	}

	TReal64 currTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

	if (currTime < receiveBuffer.timeStamp)
	{
		throw Exception(
				CV_StsBackTrace,
				"Client time is less than host time - not synchronized",
				"MyUDPStreamer::getFirstSyncFromHost",
				"getFirstSyncFromHost",
				__LINE__);
	}

	TUDWord delay(currTime - receiveBuffer.timeStamp);

	if (delay >= 1) //greater then one second
	{
		printf("Big delay at reciving start Delay[%d] . \n", delay);
		return false;
	}

	SAck ack;
	ack.opCode = OP_ACK_SND;
	ack.size = sizeof(SAck);

	currTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	ack.timeStamp = currTime;

	sendData(reinterpret_cast<TUByte*>(&ack), sizeof(ack));

	TReal64 delta = 1 - (currTime - static_cast<TUDWord>(currTime));

	// after the start was received we want to wait to a whole second before
	//start sending the frames to the
	//host so this delta is the delta before the current time and a whole second ,
	//we need to mult it by 1000
	//to get the number of milli seconds that we need to wait for that
	usleep(static_cast<TUDWord>(delta*1000));
	return true;
}



