/*
 * MyMutex.h
 *
 *  Created on: Aug 5, 2015
 *      Author: alexandalex
 */

#ifndef MYMUTEX_H_
#define MYMUTEX_H_

#include <pthread.h>

class MyMutex {
public:
	MyMutex() {
		pthread_mutex_init(&m_mutex, NULL);
	}
	void lock() {
		pthread_mutex_lock(&m_mutex);
	}
	void unlock() {
		pthread_mutex_unlock(&m_mutex);
	}
private:
	pthread_mutex_t m_mutex;
};

#endif /* MYMUTEX_H_ */
