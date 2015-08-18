/*
 * MyClientDepthRunner.h
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#ifndef MYCLIENTDEPTHRUNNER_H_
#define MYCLIENTDEPTHRUNNER_H_

#include "MyClientRunnerI.h"

class MyClientDepthRunner: public virtual MyClientRunner_I {
public:
	MyClientDepthRunner(MyFreenectDevice& device);
	virtual ~MyClientDepthRunner();
	virtual void AllocateAndSendFrame();

};

#endif /* MYCLIENTDEPTHRUNNER_H_ */
