/*
 * MyFreenectStreamer.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#include "MyUDPStreamer.h"

bool MyUDPStreamer::sendKinectFrameUDP(	TUByte* buffer,
											TUDWord chunkSize,
											const TUDWord totSize) {
	bool rv(false), status(true);
	TSDWord bytesLeft(totSize);
	TUDWord buffIndex(0);
	TUDWord packet(0);

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


