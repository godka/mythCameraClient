#include "mythVirtualServer.hh"
mythVirtualServer::mythVirtualServer(int port)
{
	m_stop = false;
	m_port = port;
	//this->StartServer();
}

void mythVirtualServer::HandleServer(void)
{
    TCPsocket newsock;
    int which;

    newsock = SDLNet_TCP_Accept(servsock);
    if ( newsock == NULL ) {
        return;
    }
	
    /* Look for unconnected person slot */
    for ( which=0; which<CHAT_MAXPEOPLE; ++which ) {
		if (!people[which]->sock) {
            break;
        }
    }
    if ( which == CHAT_MAXPEOPLE ) {
        /* Look for inactive person slot */
        for ( which=0; which<CHAT_MAXPEOPLE; ++which ) {
			if (people[which]->sock && !people[which]->active) {
                /* Kick them out.. */
                //data = CHAT_BYE;
                //SDLNet_TCP_Send(people[which].sock, &data, 1);
                SDLNet_TCP_DelSocket(socketset,
					people[which]->sock);
				SDLNet_TCP_Close(people[which]->sock);
#ifdef DEBUG
    fprintf(stderr, "Killed inactive socket %d\n", which);
#endif
                break;
            }
        }
    }
    if ( which == CHAT_MAXPEOPLE ) {
        /* No more room... */
        //data = CHAT_BYE;
        //SDLNet_TCP_Send(newsock, &data, 1);
        SDLNet_TCP_Close(newsock);
#ifdef DEBUG
    fprintf(stderr, "Connection refused -- chat room full\n");
#endif
    } else {
        /* Add socket as an inactive person */
		people[which]->sock = newsock;
		people[which]->peer = *SDLNet_TCP_GetPeerAddress(newsock);
		SDLNet_TCP_AddSocket(socketset, people[which]->sock);
#ifdef DEBUG
    fprintf(stderr, "New inactive socket %d\n", which);
#endif
    }
}
int mythVirtualServer::StartServer(){
	this->initalsocket(m_port);
	acceptthreadstatic(this);
	//acceptthreadHandle = SDL_CreateThread(acceptthreadstatic,"accept",this);
	return 0;
}

int mythVirtualServer::StopServer(){
	this->m_stop = true;
	SDL_WaitThread(this->acceptthreadHandle,0);
	return 0;
}
int mythVirtualServer::acceptthreadstatic(void* data){
	if(data != NULL){
		mythVirtualServer* tmp = (mythVirtualServer*)data;
		tmp->acceptthread();
	}
	return 0;
}
void mythVirtualServer::acceptthread(){
	    /* Loop, waiting for network events */
    while (!m_stop) {
        /* Wait for events */
        SDLNet_CheckSockets(socketset, 10);

        /* Check for new connections */
        if ( SDLNet_SocketReady(servsock) ) {
            HandleServer();
        }

        /* Check for events on existing clients */
//#pragma omp parallel for
        for (int i=0; i< CHAT_MAXPEOPLE; ++i ) {
			if (SDLNet_SocketReady(people[i]->sock)) {
				//ServerDecodeCallBack(&people[i]);
                HandleClient(i);
            }
        }
    }
    cleanup(0);
}

void mythVirtualServer::HandleClient(int which)
{
	int closesocket;
	char data[4096] = {0};
	//char tmpdata[512] = {0};
	int datalength;
	int length;
	datalength = 0;
	for(;;){
		length = SDLNet_TCP_Recv(people[which]->sock, data + datalength, 512);
		datalength += length;
		if(length < 512)
			break;
	}
    /* Has the connection been closed? */
    if ( datalength <= 0 ) {
#ifdef DEBUG
    fprintf(stderr, "Closing socket %d (was%s active)\n",
            which, people[which].active ? "" : " not");
#endif
        /* Notify all active clients */
		closePeople(people[which]);
    } else {
		this->ServerDecodeCallBack(people[which],data,datalength);
    }
}
int mythVirtualServer::closePeople(PEOPLE* people){
	if ( people->active ) {
		people->active = 0;
	}
	ServerCloseCallBack(people);
	SDLNet_TCP_DelSocket(socketset, people->sock);
	SDLNet_TCP_Close(people->sock);
	people->sock = NULL;
	people->addtionaldata = NULL;
	return 0;
}
//string mythVirtualServer::GetLocalAddress(){
//	unsigned char iphost[4] = { 0 };
//	IPaddress serverip;
//	SDLNet_GetLocalAddress(&serverip);
//	SDL_memcpy(iphost, &serverip.host, 4);
//	char tmpstr[100] = { 0 };
//	sprintf(tmpstr, "%d.%d.%d.%d", iphost[0], iphost[1], iphost[2], iphost[3]);
//	return tmpstr;
//}
int mythVirtualServer::initalsocket(int port){
	
    IPaddress serverIP;
	int i;
	unsigned char iphost[4] = {0};

        /* Initialize SDL */
    if ( SDL_Init(0) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
        exit(1);
    }

    /* Initialize the network */
    if ( SDLNet_Init() < 0 ) {
        fprintf(stderr, "Couldn't initialize net: %s\n",
                        SDLNet_GetError());
        SDL_Quit();
        exit(1);
    }
#pragma omp parallel for
    /* Initialize the channels */
    for ( i=0; i<CHAT_MAXPEOPLE; ++i ) {
		people[i] = PEOPLE::CreateNew();
		people[i]->addtionaldata = NULL;
		people[i]->active = 0;
		people[i]->sock = NULL;
    }

    /* Allocate the socket set */
    socketset = SDLNet_AllocSocketSet(CHAT_MAXPEOPLE+1);
    if ( socketset == NULL ) {
        fprintf(stderr, "Couldn't create socket set: %s\n",
                        SDLNet_GetError());
        cleanup(2);
    }

    /* Create the server socket */
	SDLNet_ResolveHost(&serverIP, NULL, port);
	//iphost[0] = &serverIP.host;
	SDL_memcpy(iphost, &serverIP.host, 4);
	printf("Server IP: %d.%d.%d.%d ---  %d\n", iphost[0], iphost[1], iphost[2], iphost[3], port);
    servsock = SDLNet_TCP_Open(&serverIP);
    if ( servsock == NULL ) {
        fprintf(stderr, "Couldn't create server socket: %s\n",
                        SDLNet_GetError());
        cleanup(2);
    }
    SDLNet_TCP_AddSocket(socketset, servsock);
	return 0;
}

mythVirtualServer* mythVirtualServer::CreateNew(int port){
	return new mythVirtualServer(port);
}
void mythVirtualServer::cleanup(int exitcode)
{
	int i;
    if ( servsock != NULL ) {
        SDLNet_TCP_Close(servsock);
        servsock = NULL;
    }
    if ( socketset != NULL ) {
        SDLNet_FreeSocketSet(socketset);
        socketset = NULL;
    }
	for (i = 0; i < CHAT_MAXPEOPLE; ++i){
		delete this->people[i];
	}
    SDLNet_Quit();
    SDL_Quit();
    exit(exitcode);
}
void mythVirtualServer::ServerDecodeCallBack(PEOPLE *people,char* data,int datalength){
	
}

mythVirtualServer::~mythVirtualServer(void)
{
	//this->cleanup(0);
}

int mythVirtualServer::ContainPeople( PEOPLE* mpeople )
{
//#pragma omp parallel for
	for (int i=0; i< CHAT_MAXPEOPLE; ++i ) {
		if (people[i]->sock == mpeople->sock){
			return 1;
		}
	}
	return 0;
}

void mythVirtualServer::ServerCloseCallBack( PEOPLE* people )
{

}
