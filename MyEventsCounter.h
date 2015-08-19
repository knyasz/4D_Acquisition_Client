/*
 * MyEventsCounter.h
 *
 *  Created on: Aug 19, 2015
 *      Author: alexandalex
 */

#ifndef MYEVENTSCOUNTER_H_
#define MYEVENTSCOUNTER_H_



#include <iostream>
#include <cmath>
#include "unistd.h"

#include "time.h"

using namespace std;

class MyEventsCounter {
public:
	MyEventsCounter();
	void Init(string eventName);
	MyEventsCounter(string eventName);
	virtual ~MyEventsCounter(){;}
	virtual void PrintoutEventsCounted();
protected:
	string m_eventName;
	unsigned int m_event_counter;
	time_t m_startTime;
	time_t m_currTime;
};

#endif /* MYEVENTSCOUNTER_H_ */
