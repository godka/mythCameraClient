#include "mythBaseServer.hh"
#include <iostream>
mythBaseServer::mythBaseServer(int port)
	:mythVirtualServer(port)//,mythVirtualSqlite()
{
	//"update server set internalip = '" + ip + "',externalip = '" + ip + "' where serverid = 1"
	//this->open("myth.db");
	server = mythStreamServer::CreateNew(0);
	server->start();
}

mythBaseServer* mythBaseServer::CreateNew(int port){
	mythBaseServer* baseserver = new mythBaseServer(port);
	return baseserver;
}

int mythBaseServer::UserResult(int argc, char **argv, char **azColName){

	return 0;
}
void mythBaseServer::ServerDecodeCallBack(PEOPLE *people,char* data,int length)
{
	if (server){
		mythBaseClient* baseclient = mythBaseClient::CreateNew(people);
		people->addtionaldata = baseclient;
		server->AppendClient(baseclient);
	}
	return;
}

mythBaseServer::~mythBaseServer(void)
{
}

void mythBaseServer::ServerCloseCallBack(PEOPLE* people){
	if (server){
		if (people->addtionaldata)
			server->DropClient((mythBaseClient*)people->addtionaldata);
	}
}