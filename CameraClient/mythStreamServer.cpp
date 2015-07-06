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
#include "mythStreamServer.hh"
#include <string>
//#include <omp.h>
mythStreamServer* mythStreamServer::CreateNew(int cameraid){
	return new mythStreamServer(cameraid);
}

mythStreamServer::mythStreamServer(int cameraid)
	//:mythVirtualSqlite()
{
	streamserverthread = NULL;
	decoder = NULL;
	ClientNumber = 0;
	m_cameraid = cameraid;
	PeopleAdd = 0;
	numbermutex = SDL_CreateMutex();
	streamservermutex = SDL_CreateMutex();
	decodemutex = SDL_CreateMutex();
	baselist.reserve(100);
	//open("myth.db");
}


mythStreamServer::~mythStreamServer(void)
{
	//delete decoder;
	SDL_DestroyMutex(numbermutex);
	SDL_DestroyMutex(streamservermutex);
	//delete decoder;
}

void mythStreamServer::connect()
{
	if(!decoder)
		decoder = mythCameraDecoder::CreateNew();
	if (decoder)
		decoder->start();
	
}

int mythStreamServer::mainthreadstatic( void* data )
{
	if(data){
		mythStreamServer* server = (mythStreamServer*)data;
		server->mainthread();
	}
	return 0;
}
bool mythStreamServer::FindClient(vector <mythBaseClient*>::iterator beg,
	vector <mythBaseClient*>::iterator end, mythBaseClient* ival)
{
	while (beg != end)
	{
		if (*beg == ival)
			break;
		else
			++beg;
	}

	if (beg != end)
		return true;
	else
		return false;
}
int mythStreamServer::AppendClient(mythBaseClient* client){
	SDL_LockMutex(streamservermutex);
	if (!FindClient(baselist.begin(), baselist.end(), client)){
		baselist.push_back(client);
	}
	SDL_UnlockMutex(streamservermutex);
	return 0;
}
int mythStreamServer::mainthread()
{
	PacketQueue* tmp = NULL;
	connect();
	while(isrunning == 0){
		SDL_PollEvent(NULL);
		if (decoder){
			if (baselist.size() <= 0){
				//printf("size <= 0\n");
				SDL_Delay(100);
			}
			else{
				tmp = decoder->get();
				if (tmp != NULL){
					//add omp version
					for (int i = 0; i < baselist.size(); i++){
						mythBaseClient* tmpclient = baselist.at(i);
						if (tmpclient != NULL){
							tmpclient->DataCallBack(tmp->h264Packet, tmp->h264PacketLength);
						}
					}
					SDL_Delay(1);
					//baselist.push_back(this);
				}
				//else{
				SDL_PollEvent(NULL);
				SDL_Delay(1);
				//}
			}
		}
	}
	if (decoder)
		decoder->stop();
	delete decoder;
	return 0;
}

int mythStreamServer::start(bool canthread)
{
	if (!canthread){
		mainthreadstatic(this);
	}
	else{
		isrunning = 0;
		if (!streamserverthread)
			streamserverthread = SDL_CreateThread(mainthreadstatic, "static", this);
	}
	return 0;
}

int mythStreamServer::stop()
{
	isrunning = 1;
	if (streamserverthread)
		SDL_WaitThread(this->streamserverthread,NULL);
	streamserverthread = NULL;
	return 0;
}

int mythStreamServer::getClientNumber()
{
	return baselist.size();
}

int mythStreamServer::DropClient(mythBaseClient* client)
{
	SDL_LockMutex(streamservermutex);
	for (vector<mythBaseClient*>::iterator iter = baselist.begin(); iter != baselist.end();iter++)
	{
		if (*iter == client){
			iter = baselist.erase(iter);
			break;
		}
	}
	SDL_UnlockMutex(streamservermutex);
	return 0;
}