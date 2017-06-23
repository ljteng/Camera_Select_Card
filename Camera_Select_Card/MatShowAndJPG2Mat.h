#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\flann\dist.h>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv\cv.h>
extern "C"
{
#include "jpeglib.h"
}
using namespace std;
using namespace cv;
void showMatImgToWnd(CWnd* pWnd, const cv::Mat& img);
Mat Jpeg2Mat(const uchar *jpegData, int jpegSize);
IplImage* Jpeg2Ipl(const uchar *jpegData, int jpegSize);
int ShowJPG(unsigned char *pData,int iDataLen,CWnd *pwnd);