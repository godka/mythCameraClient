#pragma once
#include "MythConfig.hh"
#include <stdio.h>
/* The maximum number of people who can talk at once */
#define CHAT_MAXPEOPLE  200
#include "PEOPLE.hh"
#include <string>
using namespace std;
/*
typedef struct {
	int active;
	TCPsocket sock;
	IPaddress peer;
	void* addtionaldata;
} PEOPLE;
*/
//people[CHAT_MAXPEOPLE];


class mythVirtualServer
{
public:
//	string mythVirtualServer::GetLocalAddress();
	static int acceptthreadstatic(void* data);
	void acceptthread();
	static mythVirtualServer* CreateNew(int port);
	int StartServer();
	int StopServer();
	virtual void ServerCloseCallBack(PEOPLE* people);
	virtual void ServerDecodeCallBack(PEOPLE* people,char* data,int datalength);
	int closePeople(PEOPLE* people);
	int ContainPeople(PEOPLE* mpeople);
	~mythVirtualServer(void);
protected:
	mythVirtualServer(int port);
private:
	 TCPsocket servsock;
	 SDLNet_SocketSet socketset;
	void cleanup(int exitcode);
	int initalsocket(int port);
	int m_port;
	bool m_stop;
	void HandleServer(void);
	void HandleClient(int which);
	PEOPLE *people[CHAT_MAXPEOPLE];
	SDL_Thread* acceptthreadHandle;
};

