#pragma once

#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include <iostream>
using namespace std;
class mythCamera
{
public:
	static mythCamera* CreateNew(){
		return new mythCamera();
	}
	~mythCamera();
	void CloseCamera();
	int Capture(int *width, int *height,void** src);
protected:
	mythCamera();
private:
	IplImage* pFrame;
	CvCapture* pCapture;
};

