/*
 * MyEventsCounter.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: alexandalex
 */

#include "MyEventsCounter.h"

MyEventsCounter::MyEventsCounter(string eventName):
	m_eventName(eventName),
	m_event_counter(0),
	m_startTime(0.),
	m_currTime(0.)
	{;}
MyEventsCounter::MyEventsCounter():
	m_event_counter(0),
	m_startTime(0.),
	m_currTime(0.)
	{;}
void MyEventsCounter::Init(string eventName){
	m_eventName=eventName;
}
void MyEventsCounter::PrintoutEventsCounted(){
	++m_event_counter;
	if (abs(difftime(m_startTime, time(&m_currTime))) >= 1){ //if time passed one sec
		cout<<endl<<"Performed (" << m_event_counter << ") "<< m_eventName
				<< " events at this second \n" << endl;
		m_event_counter = 0;
		m_startTime = time(NULL);
	}
}

