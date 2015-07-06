#include "mythCameraDecoder.hh"
#include <iostream>
using namespace std;

mythCameraDecoder::mythCameraDecoder()
:mythVirtualDecoder()
{
	flag = 0;
	startthread = NULL;
	encoder = NULL;
	camera = NULL;
	startmutex = SDL_CreateMutex();
}


mythCameraDecoder::~mythCameraDecoder()
{
	if (encoder != NULL){
		encoder->Cleanup();
		delete encoder;
		encoder = NULL;
	}
}

void mythCameraDecoder::start()
{
	startthread = SDL_CreateThread(decodethreadstatic, "decode", this);
}

int mythCameraDecoder::decodethread()
{
	encoder = NULL;
	SDL_LockMutex(startmutex);
	if (camera == NULL){
		camera = mythCamera::CreateNew();
	}
	if (!camera){
		cout << "set up camera failed!" << endl;
	}
	SDL_UnlockMutex(startmutex);
	encoder = mythFFmpegEncoder::CreateNew(this, 640, 480);
	char* yy = new char[640 * 480];
	char* uu = new char[640 * 480 / 4];
	char* vv = new char[640 * 480 / 4];
	char* tmpsrc [] = { yy, uu, vv };
	char* tmpsrc2[] = { yy, vv, uu };
	int width, height;
	char* tmp;
	while (flag == 0){
		int time0 = SDL_GetTicks();
		int ret = camera->Capture(&width, &height, (void**)&tmp);
		//int time4 = SDL_GetTicks();
		if (ret == 0){
			int tmplinesize[] = { width, width / 2, width / 2 };
			//int time3 = SDL_GetTicks();
			mythFFmpegEncoder::RGB2yuv(width, height, tmp, (void**) tmpsrc);
			//cout << "Start process Frame" << endl;
			//int time2 = SDL_GetTicks();
			encoder->ProcessFrame((unsigned char**)tmpsrc2, tmplinesize, mythCameraDecoder::staticresponse);
			int realdelay = (SDL_GetTicks() - time0);
			if (realdelay > 40){
				SDL_Delay(1);
			}
			else{
				SDL_Delay(40 - realdelay);
			}
		}
	}
	delete [] yy;
	delete [] uu;
	delete [] vv;
	camera->CloseCamera();
	encoder->Cleanup();
	delete encoder;
	delete camera;
	encoder = NULL;
	return 0;
}

int mythCameraDecoder::decodethreadstatic(void* data)
{
	mythCameraDecoder* decoder = (mythCameraDecoder*) data;
	decoder->decodethread();
	return 0;
}

void mythCameraDecoder::stop()
{
	flag = 1;
	if (startthread)
		SDL_WaitThread(startthread, NULL);
}

void mythCameraDecoder::staticresponse(void *myth, char* pdata, int plength)
{
	//cout << "process frame callback" << endl;
	auto decoder = (mythCameraDecoder*) myth;
	decoder->response(pdata, plength);
}

void mythCameraDecoder::response(char* pdata, int plength)
{
	put((unsigned char*)pdata, (unsigned int)plength);
}
