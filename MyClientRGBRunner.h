/*
 * MyClientRGBRunner.h
 *
 *  Created on: Aug 10, 2015
 *      Author: alexandalex
 */

#ifndef MYCLIENTRGBRUNNER_H_
#define MYCLIENTRGBRUNNER_H_

#include "MyClientRunnerI.h"

using namespace NUdpSocket;

class MyClientRGBRunner: public virtual MyClientRunner_I {
public:
	MyClientRGBRunner(MyFreenectDevice& device) ;
	virtual ~MyClientRGBRunner() ;
	virtual void AllocateAndSendFrame();
protected:
};

#endif /* MYCLIENTRGBRUNNER_H_ */
