
#ifndef MYUDPSTREAMER_H_
#define MYUDPSTREAMER_H_

#include "MyFreenectDevice.h"
#include "udpSocket.h"

using namespace NUdpSocket;

class MyUDPStreamer {
public:
	MyUDPStreamer(){;}
	virtual ~MyUDPStreamer(){
		m_socket.closeSocket();
	}
	virtual bool InitSocket( SSocketConfig& conf){
		if(!m_socket.configureSocket(conf)){
			return false;
		}
		return m_socket.openSocket();
	}
	virtual bool sendKinectFrameUDP(	TUByte* buffer,
										TUDWord chunkSize,
										const TUDWord totSize);
private:
	bool sendData(TUByte* buffer,TUDWord size) {
		if (!m_socket.isOpened() || !m_socket.isConfiguref()){
			return false;
		}
		return m_socket.sendData(buffer,size);
	}
private:
	CUdpSocket m_socket;
};

#endif /* MYUDPSTREAMER_H_ */
