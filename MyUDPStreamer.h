
#ifndef MYUDPSTREAMER_H_
#define MYUDPSTREAMER_H_

#include "MyFreenectDevice.h"
#include "udpSocket.h"
#include "Messages.h"

using namespace NUdpSocket;
using namespace NUdpMessages;

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
										const TUDWord totSize,
										EOpCodesSend opCode);
	virtual bool syncClientWithHost();

private:
	bool sendData(TUByte* buffer,TUDWord size) {
		if (!m_socket.isOpened() || !m_socket.isConfiguref()){
			return false;
		}
		return m_socket.sendData(buffer,size);
	}
	bool receiveData(TUByte* buffer,TUDWord size,TSDWord timeOut){
		if (!m_socket.isOpened() || !m_socket.isConfiguref()){
			return false;
		}
		return m_socket.reciveData(buffer, size, timeOut);
	}
	virtual bool getFirstSyncFromHost();
private:
	CUdpSocket m_socket;
};

#endif /* MYUDPSTREAMER_H_ */
