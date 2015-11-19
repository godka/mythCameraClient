#include "mythCamera.hh"
#include "MythConfig.hh"
mythCamera::mythCamera()
{
	pCapture = cvCreateCameraCapture(MYTHCAMERAPOS);
	pFrame = NULL;
	cout << "start OK!" << endl;
}
void mythCamera::CloseCamera(){
	cvReleaseImage(&pFrame);
}
int mythCamera::Capture(int* width, int *height,void** imageSource){
	//int t = SDL_GetTicks();
	pFrame = cvQueryFrame(pCapture);
	//printf("capture time: %dms\n", SDL_GetTicks() - t);
	if (pFrame){
		if(width)*width = pFrame->width;
		if(height)*height = pFrame->height;
		if (imageSource)*imageSource = pFrame->imageData;
		return 0;
	}
	else{
		return 1;
	}
}

mythCamera::~mythCamera()
{
}
