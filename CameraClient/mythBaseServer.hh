#pragma once
#include "mythVirtualServer.hh"
#include "mythStreamServer.hh"
class mythBaseServer :
	public mythVirtualServer//,public mythVirtualSqlite
{
public:
	int UserResult(int argc, char **argv, char **azColName);
	void ServerDecodeCallBack(PEOPLE* people,char* data,int datalength);
	void ServerCloseCallBack(PEOPLE* people);
	static mythBaseServer* CreateNew(int port);
	~mythBaseServer();
protected:
	mythStreamServer* server;
	mythBaseServer(int port);
};

