#include "MythConfig.hh"
#include "mythBaseServer.hh"
int main(int argc, char* argv[])
{
	//mythCameraDecoder* cameradecoder = mythCameraDecoder::CreateNew();
	mythCamera* camera = mythCamera::CreateNew();
	mythVirtualServer* server = mythBaseServer::CreateNew(5834);
	server->StartServer();
	return 0;
}

