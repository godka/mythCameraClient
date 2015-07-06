#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythVirtualServer.hh"
#include "mythBaseClient.hh"
#include "mythCameraDecoder.hh"
#include <vector>
using namespace std;
class mythStreamServer 
{
public:
	int getClientNumber();
	int AppendClient(mythBaseClient* client);
	int DropClient(mythBaseClient* client);
	static int mainthreadstatic(void* data);
	int mainthread();
	static mythStreamServer* CreateNew(int cameraid);
	~mythStreamServer(void);
	int start(bool canthread = true);
	int stop();
	int m_cameraid;
private:
	vector<mythBaseClient*> baselist;
	int ClientNumber;
	void connect();
protected:
	mythVirtualDecoder* decoder;
	string username;
	string password;
	string httpport;
	string FullSize;
	string vstypeid;
	string ip;
	string realcameraid;
	//vector<mythBaseClient*> mpeople;
	//PeopleMap mpeople;
	int PeopleAdd;
	mythStreamServer(int cameraid);
	bool FindClient(vector <mythBaseClient*>::iterator beg, vector <mythBaseClient*>::iterator end, mythBaseClient* ival);
	SDL_mutex* numbermutex;
	SDL_mutex* streamservermutex;
	SDL_Thread* streamserverthread;
	SDL_mutex* decodemutex;
	int isrunning;
	char* topchar;
	int toplength;
};

