/*
 * MyFreenectStreamer.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#include "MyUDPStreamer.h"
#include "Messages.h"

bool MyUDPStreamer::sendKinectFrameUDP(	TUByte* buffer,
											TUDWord chunkSize,
											const TUDWord totSize) {
	bool rv(false), status(true);
	TSDWord bytesLeft(totSize);
	TUDWord buffIndex(0);
	TUDWord packet(0);
        
        //send header before you send anything else
        NUdpMessages::SHeader header;
        header.opCode = NUdpMessages::EOpCodesSend::OP_FRAME_DEP_SND;
        header.size = KINECT_FRAME_SIZE;
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



