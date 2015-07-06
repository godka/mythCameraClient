#include "PEOPLE.hh"


PEOPLE::PEOPLE()
{
}

int PEOPLE::peopleSendMessage(char* data, int length){
	return SDLNet_TCP_Send(this->sock, data, length);
//	return 0;
}

PEOPLE::~PEOPLE()
{
}
