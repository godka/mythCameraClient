/********************************************************************
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
#include "mythBaseClient.hh"
#define firstrequest "HTTP/1.1 200 OK\r\nServer: WWeb/2.0\r\nConnection: Close\r\nContent-Type: multipart/x-mixed-replace;boundary=--myboundary\r\n\r\n \n\n--myboundary\n"
//#define requestend " \n\n\-\-myboundary\"
/*
mythBaseClient::mythBaseClient(mythStreamServer* server , PEOPLE* people)
{
	mserver = server;
	mypacketdata = NULL;
	tmppacketdata = NULL;
	mypacketlength = 0;
	tmppacketlength = 0;
	mpeople = people;
	mainthreadhandle = NULL;
	isrunning = 0;
	isfirst = true;
	iFrameCount = 4096;
	mymutex = SDL_CreateMutex();
}
*/
mythBaseClient::mythBaseClient(PEOPLE* people)
{
	//mserver = server;
	//mypacketdata = NULL;
	//tmppacketdata = NULL;
	//mypacketlength = 0;
	//tmppacketlength = 0;
	mpeople = people;
	mainthreadhandle = NULL;
	isrunning = 0;
	isfirst = true;
	iFrameCount = 4096;
	mymutex = SDL_CreateMutex();
}
mythBaseClient::~mythBaseClient(void)
{
}

//int mythBaseClient::mainthreadstatic( void* data )
//{
//	if(data != nullptr){
//		mythBaseClient* client = static_cast<mythBaseClient*>(data);
//		client->mainthread();
//	}
//	return 0;
//}


int mythBaseClient::DataCallBack(void* data, int len)
{
	char tempbuf[256] = { 0 };

	if (isfirst == true){
		mythSendMessage(firstrequest);
		isfirst = false;
	}
#if 1
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	SDL_snprintf(tempbuf, 256,
		"Content-Type: image/h264\r\nContent_Length: %06d Stamp:%04x%02x%02x %04x%02x%02x %02d %08x\n\n", len,
		1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
		0, iFrameCount);
#else
	//GetSystemTime()
	//tm sys_time;
	//_getsystime(&sys_time);
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);
	SDL_snprintf(tempbuf, 256,
		"Content-Type: image/h264\r\nContent_Length: %06d Stamp:%04x%02x%02x %04x%02x%02x %02d %08x\n\n",
		len,
		sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond,
		sys_time.wMilliseconds / 10, iFrameCount);
#endif
	mythSendMessage(tempbuf);
	iFrameCount++;
	mythSendMessage(data, len);
	mythSendMessage(" \n\n--myboundary\n");
	//mypacketlength = tmppacketlength;
	//SDL_UnlockMutex(mymutex);
	return 0;
}
/*
int mythBaseClient::mainthread()
{
	char tempbuf[256] = {0};
	while(isrunning == 0){
		if(1){
			//tmppacket = mserver->getTop(&tmppacketlength);
			if(GetTop() == 0){
				//send data begin
				if(isfirst == true){
					mythSendMessage(firstrequest,strlen(firstrequest));
					isfirst = false;
				}
#ifdef linux
				time_t rawtime; 
				struct tm * timeinfo; 
				time( &rawtime ); 
				timeinfo = localtime(&rawtime ); 
				SDL_snprintf(tempbuf,256,
					"Content-Type: image/h264\r\nContent_Length: %06d Stamp:%04x%02x%02x %04x%02x%02x %02d %08x\n\n",mypacketlength,
					1900+timeinfo->tm_year,1+timeinfo->tm_mon,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,
					0,iFrameCount);
#else
				SYSTEMTIME sys_time;
				GetLocalTime( &sys_time );
				SDL_snprintf(tempbuf,256,
					"Content-Type: image/h264\r\nContent_Length: %06d Stamp:%04x%02x%02x %04x%02x%02x %02d %08x\n\n",
					mypacketlength,
					sys_time.wYear,sys_time.wMonth,sys_time.wDay,sys_time.wHour,sys_time.wMinute,sys_time.wSecond,
					sys_time.wMilliseconds/10,iFrameCount);
#endif
				mythSendMessage(tempbuf);
				iFrameCount++;
				mythSendMessage(mypacketdata,tmppacketlength);
				mythSendMessage(" \n\n--myboundary\n");
				//mypacketlength = tmppacketlength;
				//SDL_UnlockMutex(mymutex);
			}else
			{
				SDL_PollEvent(NULL);
				SDL_Delay(1);
			}
		}
	}
	return 0;
}
*/
/*
int mythBaseClient::start()
{
	if(!mainthreadhandle){
		isrunning = 0;
		mserver->addClientNumber();
		//mserver->setClientNumber(tmpnum+1);
		this->mainthreadhandle = SDL_CreateThread(mainthreadstatic,"main",this);
	}
	//cout << "start server\n" << endl;
	return 0;
}
*/
//mythStreamServer* mythBaseClient::getServer(){
//	return mserver;
//}
//int mythBaseClient::stop()
//{
//	if(mainthreadhandle){
//		isrunning = 1;
//		SDL_WaitThread(mainthreadhandle,NULL);
//		mserver->minClientNumber();
//	}
//	mainthreadhandle = NULL;
//	//printf("stop client\n");
//	return 0;
//}
/*
mythBaseClient* mythBaseClient::CreateNew(mythStreamServer* server , PEOPLE* people )
{
	return new mythBaseClient(server , people);
}
*/

mythBaseClient* mythBaseClient::CreateNew(PEOPLE* people)
{
	return new mythBaseClient(people);
}
int mythBaseClient::generate( char* data,int length )
{

	return 0;
}

int mythBaseClient::mythSendMessage( void* data,int length )
{
	int tmplength = 0;
	if(length == -1)
		length = strlen((char*)data);
		//if(SDLNet_SocketReady(mpeople->sock))
	if (mpeople){
		tmplength = mpeople->peopleSendMessage((char*) data, length);
		if (tmplength < length){
			//printf("error occoured! %d:%d\n",tmplength,length);
			//stop();				//may be occoured some errors.
		}

	}
	return tmplength;
}
/*
int mythBaseClient::SetTop( char* data,int length )
{
	SDL_LockMutex(this->mymutex);
	tmppacketdata = data;
	tmppacketlength = length;
	SDL_UnlockMutex(this->mymutex);
	return 0;
}

int mythBaseClient::GetTop()
{
	int ret = 0;
	SDL_LockMutex(this->mymutex);
	tmppacketdata = mserver->getTop(&tmppacketlength);
	if(mypacketdata != tmppacketdata){
		mypacketdata = tmppacketdata;
		mypacketlength = tmppacketlength;
	}else{
		ret = 1;
	}
	SDL_UnlockMutex(this->mymutex);
	return ret;
}
*/
//int mythBaseClient::changeStreamServer( mythStreamServer* server )
//{
//	if(mserver != server)
//		mserver = server;
//	return 0;
//}
