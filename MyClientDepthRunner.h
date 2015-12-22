/*
 * MyClientDepthRunner.h
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#ifndef MYCLIENTDEPTHRUNNER_H_
#define MYCLIENTDEPTHRUNNER_H_

#include "MyClientRunnerI.h"
#include "Messages.h"

using namespace NUdpMessages;

class MyClientDepthRunner: public virtual MyClientRunner_I {
public:
	MyClientDepthRunner(	MyFreenectDevice& device,
							MyUDPStreamer& udp_streamer);
	virtual ~MyClientDepthRunner();
	virtual void AllocateAndSendFrame();
protected:

};

#endif /* MYCLIENTDEPTHRUNNER_H_ */
