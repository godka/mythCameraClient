#pragma once
#include "MythConfig.hh"
extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#ifdef WIN32
//	#include <Windows.h>
//#endif
#include <time.h>
};
#include "PEOPLE.hh"
class mythBaseClient
{
public:
	//static mythBaseClient* CreateNew(mythStreamServer* server , PEOPLE* people);

	static mythBaseClient* CreateNew(PEOPLE* people);
	//static int mainthreadstatic(void* data);
	//mythStreamServer* getServer();	//removed
	//int changeStreamServer(mythStreamServer* server);
	//int mainthread();
	//int start();
	//int stop();
	//int SetTop(char* data,int length);
	PEOPLE* mpeople;
	bool isfirst;
	int mythSendMessage(void* data, int length = -1);
	int DataCallBack(void* data, int len);
	~mythBaseClient(void);
	//int active;
private:
	int isrunning;
	SDL_Thread* mainthreadhandle;
	//int mypacketlength;
	//char* mypacketdata;
	//int tmppacketlength;
	//char* tmppacketdata;
	SDL_mutex* mymutex;
	int generate(char* data,int length);
	int iFrameCount;
	//int GetTop();
protected:
	//mythStreamServer* mserver;
	//mythBaseClient(mythStreamServer* server , PEOPLE* people);
	mythBaseClient(PEOPLE* people);
};

