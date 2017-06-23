// Camera_Select_CardDlg.cpp : ʵ���ļ�
#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <vector>
#include <list>
#include <iostream>
#include <io.h>
#include <Setupapi.h>
#include <devguid.h>
#include "Camera_Select_Card.h"
#include "Camera_Select_CardDlg.h"
#include "afxdialogex.h"
#include "OperDobot2.h"
#include "MatShowAndJPG2Mat.h"

#pragma  comment(lib,   "setupapi")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
// CCamera_Select_CardDlg �Ի���
CCamera_Select_CardDlg::CCamera_Select_CardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCamera_Select_CardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCamera_Select_CardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCamera_Select_CardDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCamera_Select_CardDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_DOBOT, &CCamera_Select_CardDlg::OnBnClickedButtonConnectDobot)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_DOBOT, &CCamera_Select_CardDlg::OnBnClickedButtonCloseDobot)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_CAMER, &CCamera_Select_CardDlg::OnBnClickedButtonConnectCamer)
	ON_BN_CLICKED(IDC_BUTTON_START_SELECTION, &CCamera_Select_CardDlg::OnBnClickedButtonStartSelection)
	ON_BN_CLICKED(IDC_BUTTON_CENTER_JUST, &CCamera_Select_CardDlg::OnBnClickedButtonCenterJust)
	ON_BN_CLICKED(IDC_BUTTON_LEFT_JUST, &CCamera_Select_CardDlg::OnBnClickedButtonLeftJust)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_JUST, &CCamera_Select_CardDlg::OnBnClickedButtonRightJust)
	ON_BN_CLICKED(IDC_BUTTON_DOBOT_GO_HOME, &CCamera_Select_CardDlg::OnBnClickedButtonDobotGoHome)
//	ON_WM_CLOSE()
ON_WM_DESTROY()
END_MESSAGE_MAP()


// CCamera_Select_CardDlg ��Ϣ�������

//unsigned char *g_buff = new unsigned char[102400];
//int g_iBuffLen = 0;
//CCriticalSection g_cs;
CDobotPoint g_center_point,g_left_point,g_right_point;
int g_width = 0,g_height = 0;
int g_cut_width = 588,g_cut_height = 432;
typedef struct _imageE
{
	int iseq;
	Point2f cPiont;
	float radius;
	float angle;
	float color_average;
	int type;
} IMAGE_ELEMEMT;
VideoCapture g_cap;
OperDobot *g_dobot;
CCamera_Select_CardDlg *g_dlg;
BOOL g_bConnected = false;
Mat Operater(const Mat &gray);
void LogInf(const char *str);
unsigned int __stdcall Thread_GetData(void * param);
unsigned int __stdcall Thread_Selection_Running(void * param);
vector<IMAGE_ELEMEMT>  GetElememtFromImg(const Mat &car_img);
Mat rotateImage(const Mat &img, double degree,int iWidth,int iHeight);
int checkImgType(Mat &img);
float checkHSVColor(Mat &oriImg);
string  getSubtract(Mat &src);
void getPXSum(Mat &src, int &a);

void getPXSum(Mat &src, int &a)//��ȡ�������ص��
{ 
	threshold(src, src, 100, 255, CV_THRESH_BINARY);
	  a = 0;
	for (int i = 0; i < src.rows;i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			a += src.at <uchar>(i, j);
		}
	}
}
string  getSubtract1(Mat &src) //����ͼƬ���
{
	//double same = matchShapes(parMat,image_roi,CV_CONTOURS_MATCH_I3,1.0);
	Mat img_result;
	int min = 1000000;
	//int serieNum = 0;
	string strVal;
	_finddata_t   FileInfo;
	string FilePathStr;
	char * FilePath = "data\\";
	long   Handle =_findfirst(FilePathStr.assign(FilePath).append("*.jpg").c_str(),&FileInfo);
	while(Handle >= 0)
	{
		char name[60];
		sprintf_s(name,"%s", FilePathStr.assign(FilePath).append( FileInfo.name).c_str());
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		threshold(Template, Template, 100, 255, CV_THRESH_BINARY);
		threshold(src, src, 100, 255, CV_THRESH_BINARY);
	    resize(Template, Template, Size(50, 20), 0, 0, CV_INTER_LINEAR);//�����ߴ�
		try{
			resize(src, src, Size(50, 20), 0, 0, CV_INTER_LINEAR);
			absdiff(Template, src, img_result);//����ͼƬ��Ӧ���ص�ֵ���
		}
		catch(Exception e)
		{
			TRACE("error code:%d,%s\n",e.code,e.what());
		}
		int diff = 0;
		getPXSum(img_result, diff);
		if (diff < min)
		{
			min = diff;
			//serieNum = i;
			strVal = FileInfo.name;
		}

		if( _findnext( Handle,  &FileInfo) < 0) break;
	}
	_findclose(Handle); 
	//printf("��С������%d ", min);
	//printf("%s\n", strVal);
	return strVal.substr(0,strVal.length()-4);
}
string  getSubtract(Mat &src) //����ͼƬ���
{
	//double same = matchShapes(parMat,image_roi,CV_CONTOURS_MATCH_I3,1.0);
	Mat img_result;
	double min = 1000000;
	//int serieNum = 0;
	string strVal;
	_finddata_t   FileInfo;
	string FilePathStr;
	char * FilePath = "data\\";
	long   Handle =_findfirst(FilePathStr.assign(FilePath).append("*.jpg").c_str(),&FileInfo);
	while(Handle >= 0)
	{
		double diff = 0;
		char name[60];
		sprintf_s(name,"%s", FilePathStr.assign(FilePath).append( FileInfo.name).c_str());
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		
		float fTemplate = float(Template.rows)/float(Template.cols);
		float fSrc =  float(src.rows)/float(src.cols);

		diff = fabs(fTemplate-fSrc);
		
		if (diff < min)
		{
			min = diff;
			//serieNum = i;
			strVal = FileInfo.name;
		}

		if( _findnext( Handle,  &FileInfo) < 0) break;
	}
	_findclose(Handle); 
	//printf("��С������%d ", min);
	//printf("%s\n", strVal);
	return strVal.substr(0,strVal.length()-4);
}
float checkHSVColor(Mat &oriImg)
{
	Mat imgHSV(oriImg.size(),CV_8U,Scalar(0));  
	vector<Mat> hsvSplit;
	blur(oriImg,oriImg,Size(3,3));
	cvtColor(oriImg, imgHSV, COLOR_BGR2HSV);//CV_BGR2YCrCb
	//cvtColor(oriImg, imgHSV, CV_BGR2YCrCb);
	split(imgHSV,hsvSplit);

	int i=0;
	int fTotalColor = 0;
	MatIterator_<uchar> it_H = hsvSplit[0].begin<uchar>();
	for( ; it_H != hsvSplit[0].end<uchar>(); ++it_H)
	{
		uchar value = *it_H;
		fTotalColor += value;
		i++;
	}
	return float(fTotalColor)/i;
}
int checkImgType(Mat &img)
{
	string str = getSubtract(img);
	int iRet = atoi(str.c_str());
	return iRet;
}
Mat Operater(const Mat &gray)
{
	//��˹�˲����˲�ȥ�루��ѡ��
	int ksize = 3;
	Mat g_gray;
	Mat G_kernel = getGaussianKernel(ksize,0.3*((ksize-1)*0.5-1)+0.8);
	filter2D(gray,g_gray,-1,G_kernel);

/*	//Sobel���ӣ�x�����y����
	Mat sobel_x,sobel_y;
	Sobel(g_gray,sobel_x,CV_16S,1,0,3);
	Sobel(g_gray,sobel_y,CV_16S,0,1,3); 
	Mat abs_x,abs_y;
	convertScaleAbs(sobel_x,abs_x);
	convertScaleAbs(sobel_y,abs_y);
	Mat grad;
	addWeighted(abs_x,0.5,abs_y,0.5,0,grad);
	*/
	Mat detected_edges;
	blur( g_gray, detected_edges, Size(3,3) );//��׼�����˲�
	/// ����Canny����
	Canny( detected_edges, detected_edges, 80, 80*2.5, 3 );
	/// ʹ�� Canny���������Ե��Ϊ������ʾԭͼ��
	/*
	dst = Scalar::all(0);
	src.copyTo( dst, detected_edges);
	imshow( window_name, dst );
	*/
	//return detected_edges;
	Mat img_bin;
	threshold(detected_edges,img_bin,0,255,CV_THRESH_BINARY |CV_THRESH_OTSU);
return img_bin;
/*
    Mat elementX = getStructuringElement(MORPH_RECT, Size(3, 3),Point(-1,-1));
    Mat m_ResImg;
    dilate(img_bin, m_ResImg,elementX,Point(-1,-1),1);
	//erode(m_ResImg, m_ResImg,elementX,Point(-1,-1),1);
	//dilate(m_ResImg, m_ResImg,elementX,Point(-1,-1),1);	
	return m_ResImg;
*/
}
Mat rotateImage(const Mat &img, double degree,int iWidth,int iHeight)
{
    //degree = -degree;//warpAffineĬ�ϵ���ת��������ʱ�룬���ԼӸ��ű�ʾת��Ϊ˳ʱ��
    double angle = degree  * CV_PI / 180.; // ����  
    double a = sin(angle), b = cos(angle);
    int width = img.cols;
    int height = img.rows;
    int width_rotate = iWidth;//int(height * fabs(a) + width * fabs(b));
    int height_rotate = iHeight;//int(width * fabs(a) + height * fabs(b));
	Vec3b v3b;
	if(img.rows > 5 && img.cols > 5)
		v3b = img.at<Vec3b>(5,5);
	else
		v3b = img.at<Vec3b>(0,0);
    //��ת����map
    // [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
    // [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
    float map[6];
    Mat map_matrix = Mat(2, 3, CV_32F, map);
    // ��ת����
    CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);
    CvMat map_matrix2 = map_matrix;
    cv2DRotationMatrix(center, degree, 1.0, &map_matrix2);//�����ά��ת�ķ���任����
    map[2] += (width_rotate - width) / 2;
    map[5] += (height_rotate - height) / 2;
    Mat img_rotate;
    //��ͼ��������任
    //CV_WARP_FILL_OUTLIERS - ����������ͼ������ء�
    //�������������������ͼ��ı߽��⣬��ô���ǵ�ֵ�趨Ϊ fillval.
    //CV_WARP_INVERSE_MAP - ָ�� map_matrix �����ͼ������ͼ��ķ��任��
    warpAffine(img, img_rotate, map_matrix, Size(width_rotate,height_rotate), CV_INTER_CUBIC | CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT, Scalar(v3b[0],v3b[1],v3b[2])/*1, 0, 0*/);
	return img_rotate;
	//Rect r( (width_rotate-iWidth)/2,(height_rotate-iHeight)/2,iWidth,iHeight);
    //return img_rotate(r);
}
vector<IMAGE_ELEMEMT>  GetElememtFromImg(const Mat &car_img)
{
	vector<IMAGE_ELEMEMT> imglist;
	imglist.clear();		
	//ת��ɻҶ�ͼ��
	Mat gray;
	cvtColor(car_img,gray,CV_BGR2GRAY);
	Mat gray_bi = Operater(gray);
	CWnd* pWnd1 = g_dlg->GetDlgItem(IDC_STATIC_GRAY);
	showMatImgToWnd(pWnd1,gray_bi);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( gray_bi, contours, hierarchy, 
	  CV_RETR_EXTERNAL,//CV_RETR_TREE,CV_RETR_LIST,CV_RETR_CCOMP,CV_RETR_EXTERNAL
	  CV_CHAIN_APPROX_SIMPLE, 
	  Point(0, 0) );
//  findContours(gray_bi, 
//		contours, // a vector of contours 
//		CV_RETR_EXTERNAL, // retrieve the external contours
//		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours

	//char gStrLogInfo[128] = {0};
	//sprintf(gStrLogInfo,"width:%d width:%d",gray_bi.size().width,gray_bi.size().height);
	//LogInf(gStrLogInfo);
	/// �������
	Mat drawing = Mat::zeros( gray_bi.size(), CV_8UC3 );
	//Vec3b temp = drawing.at<Vec3b>(0,1);

	RNG rng(12345);
	vector<vector<Point>>::const_iterator itc= contours.begin();
	while (itc!=contours.end()) 
	{	 
		double tmparea = fabs(contourArea(*itc));
		RotatedRect minRect = minAreaRect(*itc); 
		Point2f vertices[4];
		minRect.points(vertices); //�����С��Ӿ���4����
		double L1 = sqrt((vertices[0].x-vertices[1].x) * (vertices[0].x-vertices[1].x) + (vertices[0].y-vertices[1].y) * (vertices[0].y-vertices[1].y));
		double L2 = sqrt((vertices[2].x-vertices[1].x) * (vertices[2].x-vertices[1].x) + (vertices[2].y-vertices[1].y) * (vertices[2].y-vertices[1].y));
		double angle;
		if(L1 > L2) 
		{
			double T = L2;
			L2 = L1;
			L1 = T;
			angle = atan2((vertices[0].y-vertices[1].y),(vertices[0].x-vertices[1].x)) * 180.0/CV_PI;
		}
		else
			angle = atan2((vertices[2].y-vertices[1].y),(vertices[2].x-vertices[1].x)) * 180.0/CV_PI;

		int iType = 0;

		//-------��С���Բ-------------------  
		Point2f center;//Բ��  
		float radius;//�뾶  
		minEnclosingCircle(*itc, center, radius);  
		/*
		if( contourArea(*itc) < radius*radius*3.14*0.61  
			|| contourArea(*itc) > radius*radius*3.14*0.75  
			|| contourArea(*itc) < 2000 || contourArea(*itc) > 10000  
					|| arcLength(*itc, true) < 230 || arcLength(*itc, true) > 250 )  
			itc =  contours.erase(itc); 
		*/
		double contLenth =  arcLength(*itc,true);
		double Afa = (4 * CV_PI *  tmparea)/(contLenth * contLenth);//��Բ�Ľ��ƶ�

		Rect rt = boundingRect(*itc);
		Moments m = moments( *itc, false );
		double fXC = m.m10/m.m00;
		double fYC = m.m01/m.m00;//����������
			
		if(L2 > 8*L1)
			itc = contours.erase(itc);
		else if( L1< g_width/25.0)
			itc = contours.erase(itc);
		else if(L2 > g_height/2.0)
			itc = contours.erase(itc);
		else if(fYC > car_img.size().height*5/6.0 || fYC < car_img.size().height/6.0)
			itc = contours.erase(itc);
		else
		{

			Mat image_roi = car_img(rt).clone();
			Mat parMat = rotateImage(image_roi, angle,(int)L2,(int)L1);
			/*
			char sFile[24] = {0};
			sprintf_s(sFile,"%d.jpg",itc-contours.begin());
			imwrite(sFile,parMat);
			*/
			//convexHull(
			float fHaverage = checkHSVColor(parMat.clone());

			cvtColor(parMat,parMat,CV_BGR2GRAY);
			iType = checkImgType(parMat);
			//iType = getCheckTemplate(*itc);//
			IMAGE_ELEMEMT ce3f;
			ce3f.cPiont = center;
			ce3f.cPiont.x = (float)fXC;
			ce3f.cPiont.y = (float)fYC;
			ce3f.radius = radius;
			ce3f.angle = (float)angle;
			ce3f.color_average = fHaverage;
			ce3f.type = iType;
			ce3f.iseq = itc-contours.begin();
			imglist.push_back(ce3f);
			//sprintf(gStrLogInfo,"rt.width:%d  rt.height:%d center:(%f,%f) radius:%f,r1/r2=%f", rt.width, rt.height,center.x,center.y,radius,52.0/radius);
			//LogInf(gStrLogInfo);

			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( drawing, contours,itc-contours.begin(), color, 2, 8, hierarchy, 0, Point() );
			char tam[32] = {0};
			sprintf_s(tam,"%d",ce3f.iseq);
			putText(drawing, tam, Point(int(fXC), int(fYC)), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(255,0,255),1);
			itc++;
		}
    }
	CWnd* pWnd = g_dlg->GetDlgItem(IDC_STATIC_VIEW_RESULT);
	showMatImgToWnd(pWnd,drawing);
	return imglist;
}
unsigned int __stdcall Thread_Selection_Running(void * param)
{
	CDobotPoint pn = {107.39f,-236.47f,10.0f};
	CWnd *pwnd = g_dlg->GetDlgItem(IDC_BUTTON_START_SELECTION);
	static bool b = false;
	Mat map1, map2;
	b=!b;
	if(b)
	{
		pwnd->SetWindowTextA("ֹͣʶ��");
		Mat img;
		g_cap>>img;
		FileStorage fs2("config/camera.yml", FileStorage::READ);
		int frameCount = (int)fs2["nframes"];
		std::string date;
		fs2["calibration_time"] >> date;
		Mat cameraMatrix, distCoeffs;
		fs2["camera_matrix"] >> cameraMatrix;
		fs2["distortion_coefficients"] >> distCoeffs;
		initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, img.size(), 1, img.size(), 0),img.size(), CV_16SC2, map1, map2);
	}

while(b)
{
	Mat view;
	g_cap>>view;
	if(!view.data)
	{
		return 0;
	}
	Mat frame;
	remap(view, frame, map1, map2, INTER_LINEAR);
	Rect rt((frame.size().width-g_cut_width)/2 ,(frame.size().height-g_cut_height)/2,g_cut_width,g_cut_height);
	Mat car_img = frame(rt);
	g_width = car_img.size().width;
	g_height =  car_img.size().height;
	double fRe = (g_right_point.y - g_left_point.y)/g_width;//52.0/117.0;
	vector<IMAGE_ELEMEMT> imglist = GetElememtFromImg(car_img);
	if(imglist.empty())
	{
		Sleep(1000);
		continue;
	}
	vector<IMAGE_ELEMEMT>::iterator it = imglist.begin();
	//for(it = imglist.begin(); it != imglist.end();++it)
	if(it != imglist.end())
	{
		IMAGE_ELEMEMT c3f = *it;

		CDobotPoint p1;
		p1.x = g_center_point.x - (g_height/2.0  - c3f.cPiont.y) * fRe;
		p1.y = g_center_point.y - (g_width/2.0   - c3f.cPiont.x) * fRe;
		p1.z = g_center_point.z + 50;
		char gStrLogInfo[128] = {0};
		sprintf_s(gStrLogInfo,"get object%d:centerX:%0.0f,\tcenterY:%0.0f,\tradius:%0.0f,\tangle:%0.0f,\ttype:%d,\tcolor:%0.0f",c3f.iseq,c3f.cPiont.x,c3f.cPiont.y,c3f.radius,c3f.angle,c3f.type,c3f.color_average);
		//sprintf_s(gStrLogInfo,"get object:%d:IMG(%0.02f,%0.02f),DOBOT(%0.02f,%0.02f)",c3f.iseq,c3f.cPiont.x,c3f.cPiont.y,p1.x,p1.y);
		LogInf(gStrLogInfo);
		
		g_dobot->Move2AbsolutePosition(p1);
		CDobotPoint p2 = p1;
		p2.z = g_center_point.z;
		g_dobot->Move2AbsolutePosition(p2);
		g_dobot->SetGrab(true);
		Sleep(500);
		g_dobot->Move2AbsolutePosition(p1); 
		g_dobot->Move2AbsolutePosition(pn);
		CDobotPoint pr = pn;
		pr.z -= 50;
		g_dobot->Move2AbsolutePosition(pr);
		g_dobot->SetGrab(false);
		Sleep(1000);
		g_dobot->Move2AbsolutePosition(pn);
	}
}
	pwnd->SetWindowTextA("��ʼʶ��");	
	return 0;
}

int FromCameraGetData()
{
	if(!g_cap.open(0))
	{
		AfxMessageBox("������ͷʧ��!");
		return -1;
	}
	Mat img;
	g_cap>>img;
	FileStorage fs2("config/camera.yml", FileStorage::READ);
	int frameCount = (int)fs2["nframes"];
	string date;
	fs2["calibration_time"] >> date;
	Mat cameraMatrix, distCoeffs;
	fs2["camera_matrix"] >> cameraMatrix;
	fs2["distortion_coefficients"] >> distCoeffs;
	Mat map1, map2;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, img.size(), 1, img.size(), 0),img.size(), CV_16SC2, map1, map2);
	g_width = img.size().width;
	g_height = img.size().height;

	Mat frame;
	g_bConnected = TRUE;
	while(g_bConnected)
	{
		g_cap>>frame;
		Mat rview;
		Mat m;
		remap(frame, m, map1, map2, INTER_LINEAR);
		Rect rt((frame.size().width-g_cut_width)/2 ,(frame.size().height-g_cut_height)/2,g_cut_width,g_cut_height);
		rview = m(rt).clone();

		imshow("Video",rview); 
		CWnd* pWnd1 = g_dlg->GetDlgItem(IDC_STATIC_VIEW);
		showMatImgToWnd(pWnd1,rview);

		CRect rect;
		pWnd1->GetClientRect( &rect );
		CDC *pDC = pWnd1->GetDC();
		CPen *penXY = new CPen(PS_SOLID, 0, RGB(255, 0, 0));
		CPen *pOldPen = pDC->SelectObject(penXY);
		pDC->MoveTo(0,int(rect.Height()/2.0));
		pDC->LineTo(rect.Width(),int(rect.Height()/2.0));
		pDC->MoveTo(int(rect.Width()/2.0),0);
		pDC->LineTo(int(rect.Width()/2.0),rect.Height());
		pDC->MoveTo(10,int(rect.Height()/6.0));
		pDC->LineTo(rect.Width()-10,int(rect.Height()/6.0));
		int R = (rect.Height()/2.0-rect.Height()/6.0)/2;
		int CXL = 10 + R;
		int CYL = rect.Height() - rect.Height()/6.0 - R;
		pDC->MoveTo(10,int(rect.Height()/6.0));
		pDC->LineTo(CXL-R,CYL);
		pDC->AngleArc(CXL,CYL,R,180,90);
		int CXR = rect.Width() - 10 - R;
		int CYR = CYL;
		pDC->AngleArc(CXR,CYR,R,270,90);
		pDC->LineTo(rect.Width()-10,int(rect.Height()/6.0));
		pDC->SelectObject(pOldPen);
		delete penXY;
		pWnd1->ReleaseDC(pDC);
		//Sleep(30);
		if(waitKey(30) >=0)
			g_bConnected = false;
		//cvReleaseImage(&Show1);
	}
	g_cap.release();
	return 0;
}
unsigned int __stdcall Thread_GetData(void * param)
{
	//FromServerGetData();
	FromCameraGetData();
	return 0;
}
unsigned int __stdcall Thread_MoveCenterPoint(void * param)
{
	g_dobot->Move2AbsolutePosition(g_center_point); 
	return 0;
}
unsigned int __stdcall Thread_ScandPoint(void * param)
{
	CDobotPoint p = g_center_point;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.y -= 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.y -= 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.y -= 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.y -= 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p = g_center_point;
	p.y += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.y += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.y += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.y += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);

	p = g_center_point;
	p.x += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.x += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.x += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.x += 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);

	p = g_center_point;
	p.x -= 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.x -= 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);
	p.x -= 21.3;
	g_dobot->Move2AbsolutePosition(p);Sleep(1000);

	return 0;
}
void LogInf(const char *str)
{
	CString strTxt = "";
	g_dlg->GetDlgItem(IDC_EDIT_LOG_SHOW)->GetWindowText(strTxt);
	if(strTxt.GetLength() > 2048) strTxt = "";
	g_dlg->GetDlgItem(IDC_EDIT_LOG_SHOW)->SetWindowText(strTxt + str + "\r\n");
}
void ShowDobotStatus(float f1,float f2,float f3,float f4,float f5,float f6,float f7,float f8,float f9,float f10)
{
	char sData[512] = {0};
	sprintf_s(sData,"������\r\nX�����꣺\t%0.5f\tY�����꣺%0.5f\tZ�����꣺\t%0.5f\tR�����λ�ã�\t%0.5f\r\n�����Ƕȣ�%0.5f\t��۽Ƕȣ�%0.5f\tС�۽Ƕȣ�%0.5f\t����Ƕȣ�\t%0.5f\r\nצ�ӽǶȣ�%0.5f",f1,f2,f3,f4,f5,f6,f7,f8,f9);
	g_dlg->GetDlgItem(IDC_EDIT_DOBOT_STATUS)->SetWindowText(sData);
}
BOOL CCamera_Select_CardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	g_dlg = this;
	g_dobot = new OperDobot2();
	g_dobot->SetLogInfoFun(&LogInf);

	g_dobot->ShowDobotStatus=ShowDobotStatus;

	WSADATA wsaData;

	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{  
		MessageBox("�����������");
		return TRUE;
	} 
	Load3Point();

	char sComName[8] = {0}; 
	//SearchCOM(sComName,"Arduino Mega 2560");//"USB-SERIAL CH340" 
	SearchCOM(sComName,"USB-SERIAL CH340"); 
	CString strCom1; 
	strCom1.Format( "//./%s",sComName); 
	string strCom = (LPCTSTR)strCom1;
	g_dobot->SetCOM(strCom);
namedWindow("Video");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
bool CCamera_Select_CardDlg::SearchCOM(char *pName,const char *pFindName)  
{ 
	bool bArduino = false; 
	//   �õ��豸��Ϣ��     
	HDEVINFO   hDevInfo   =   SetupDiGetClassDevs(     
		(LPGUID)&GUID_DEVCLASS_PORTS,NULL,0,DIGCF_PRESENT/*   |   DIGCF_ALLCLASSES*/);     
    if(hDevInfo==INVALID_HANDLE_VALUE) 
    { 
        printf("Error!   SetupDiGetClassDevs()   return   %d\n",   GetLastError());     
        SetupDiDestroyDeviceInfoList(hDevInfo);     
        return     0;         
    }     
    TCHAR   szBuf[MAX_PATH];     
    SP_DEVINFO_DATA   spDevInfoData   =   {sizeof(SP_DEVINFO_DATA)};     
    //   ��ʼ�о��豸     
    DWORD   i = 0;  
    for(i = 0;SetupDiEnumDeviceInfo(hDevInfo, i,&spDevInfoData);   i++)     
    {     
		//   �õ��豸����     
		if(SetupDiGetDeviceRegistryProperty(hDevInfo,&spDevInfoData,SPDRP_FRIENDLYNAME,NULL,(PBYTE)szBuf, MAX_PATH,NULL)) 
        {         
			if(memcmp(szBuf,pFindName,strlen(pFindName)) == 0)//�������⴮�� 
			{ 
				string strPort = szBuf; 
				int nPos1,nPos2; 
				nPos1 = strPort.find("(",0); 
				nPos2 = strPort.find(")",nPos1); 
				strPort = strPort.substr(nPos1+1,nPos2-nPos1-1); 
				OperDobot::strTrim(strPort); 
				strcpy(pName,strPort.c_str()); 
				bArduino = true; 
				break; 
			} 
        } 
	} 
	return bArduino; 


} 
void CCamera_Select_CardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CCamera_Select_CardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCamera_Select_CardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CCamera_Select_CardDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	/*
	static int i = 0;
	char sName[64] = {0};
	sprintf(sName,"img/left%02d.jpg",++i);
	Mat img;
	g_cap >> img;
	imwrite(sName,img);	
	*/
	//CDialogEx::OnOK();
	//unsigned int nThreadAddr;
	//HANDLE	hSession = (HANDLE)_beginthreadex(0, 0, Thread_ScandPoint, this, 0, &nThreadAddr);
	//if(hSession) CloseHandle( hSession );

	char gInfo[512] = {0};
	sprintf_s(gInfo,"g_left_point(%0.02f,%0.02f),g_center_point(%0.02f,%0.02f),g_right_point(%0.02f,%0.02f)",g_left_point.x,g_left_point.y,g_center_point.x,g_center_point.y,g_right_point.x,g_right_point.y);
	LogInf(gInfo);
	memset(gInfo,0,512);
	sprintf_s(gInfo,"(imgW:%d,imgH:%d),(dobotW:%f)",g_width,g_height,g_right_point.y-g_left_point.y);
	LogInf(gInfo);
}
BOOL CCamera_Select_CardDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_LBUTTONDOWN)
	{
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_LEFT)->m_hWnd)
		{
                     // ��ť����
			//if(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck() || ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())
			//	g_dobot->PointMoving(2,TRUE);
			//else
				g_dobot->PointMoving(4,FALSE);
        }
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_RIGHT)->m_hWnd)
		{
			//if(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck() || ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())
			//	g_dobot->PointMoving(1,TRUE);
			//else
				g_dobot->PointMoving(3,FALSE);
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_UP)->m_hWnd)
		{
			/*if(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck())
				g_dobot->PointMoving(4,TRUE);
			else if(((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())
				g_dobot->PointMoving(6,TRUE);
			else*/
				g_dobot->PointMoving(5,FALSE);
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_DOWN)->m_hWnd)
		{
			/*if(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck())
				g_dobot->PointMoving(3,TRUE);
			else if(((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())
				g_dobot->PointMoving(5,TRUE);
			else*/
				g_dobot->PointMoving(6,FALSE);
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_BACK)->m_hWnd)
		{
			g_dobot->PointMoving(2,FALSE);
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_FORWARD)->m_hWnd)
		{
			g_dobot->PointMoving(1,FALSE);
		}
	}
	else if(pMsg->message == WM_LBUTTONUP)
	{
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_LEFT)->m_hWnd)
		{
			g_dobot->PointMoving(0,false);// ��ť����
			//m_PressureCardNew.m_PressureCardDlg->OnBnClickedButtonRefresh();
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_RIGHT)->m_hWnd)
		{
			g_dobot->PointMoving(0,false);
			//m_PressureCardNew.m_PressureCardDlg->OnBnClickedButtonRefresh();
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_UP)->m_hWnd)
		{
			g_dobot->PointMoving(0,false);
			//m_PressureCardNew.m_PressureCardDlg->OnBnClickedButtonRefresh();
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_DOWN)->m_hWnd)
		{
			g_dobot->PointMoving(0,false);
			//m_PressureCardNew.m_PressureCardDlg->OnBnClickedButtonRefresh();
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_BACK)->m_hWnd)
		{
			g_dobot->PointMoving(0,FALSE);
			//m_PressureCardNew.m_PressureCardDlg->OnBnClickedButtonRefresh();
		}
		if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_FORWARD)->m_hWnd)
		{
			g_dobot->PointMoving(0,FALSE);
			//m_PressureCardNew.m_PressureCardDlg->OnBnClickedButtonRefresh();
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
void CCamera_Select_CardDlg::OnBnClickedButtonConnectDobot()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWnd *pwnd = g_dlg->GetDlgItem(IDC_BUTTON_CONNECT_DOBOT);
	pwnd->EnableWindow(false);
	g_dobot->DobotStart();
}
void CCamera_Select_CardDlg::OnBnClickedButtonCloseDobot()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_dobot->DobotStop();
	CWnd *pwnd = g_dlg->GetDlgItem(IDC_BUTTON_CONNECT_DOBOT);
	pwnd->EnableWindow(true);
}
void CCamera_Select_CardDlg::OnBnClickedButtonConnectCamer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWnd *pwnd = g_dlg->GetDlgItem(IDC_BUTTON_CONNECT_CAMER);
	if(g_bConnected )
	{
		g_bConnected = FALSE;
		pwnd->SetWindowTextA("��������ͷ");
	}
	else
	{
		unsigned int nThreadAddr;
		HANDLE	hSession = (HANDLE)_beginthreadex(0, 0, Thread_GetData, this, 0, &nThreadAddr);
		if(hSession) CloseHandle( hSession );
		pwnd->SetWindowTextA("�ر�����ͷ");
	}
}
void CCamera_Select_CardDlg::OnBnClickedButtonStartSelection()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	unsigned int nThreadAddr;
	HANDLE	hSession = (HANDLE)_beginthreadex(0, 0, Thread_Selection_Running, this, 0, &nThreadAddr);
	if(hSession) CloseHandle( hSession );
}
void CCamera_Select_CardDlg::OnBnClickedButtonCenterJust()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	/*
	g_center_point.x = (g_left_point.x + g_right_point.x)/2;
	g_center_point.y = (g_left_point.y + g_right_point.y)/2;
	g_center_point.z = g_left_point.z;
	*/
	g_center_point = g_dobot->GetCurrentAbsolutePosition();
	Save3Point();
	/*
	unsigned int nThreadAddr;
	HANDLE	hSession = (HANDLE)_beginthreadex(0, 0, Thread_MoveCenterPoint, this, 0, &nThreadAddr);
	if(hSession) CloseHandle( hSession );
	*/
}
void CCamera_Select_CardDlg::OnBnClickedButtonLeftJust()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_left_point = g_dobot->GetCurrentAbsolutePosition();
	Save3Point();
}
void CCamera_Select_CardDlg::OnBnClickedButtonRightJust()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_right_point = g_dobot->GetCurrentAbsolutePosition();
	Save3Point();
	char sLog[32] = {0};
	sprintf(sLog,"length:%f,%f",g_right_point.y-g_left_point.y,g_right_point.x-g_left_point.x);
	LogInf(sLog);
}
void CCamera_Select_CardDlg::Save3Point(void)
{
	ofstream m_ofs;
	m_ofs.open ("config/point3.conf",ios::out);
	if(!m_ofs) 
	{
		MessageBox("���ļ�<config/point3.conf>ʧ��");
		return ;
	}
	char sBuffer[128] = {0};
	memset(sBuffer,0,128);
	sprintf(sBuffer,"%0.4f,%0.4f,%0.4f,",g_left_point.x,g_left_point.y,g_left_point.z);
	m_ofs <<sBuffer<< endl;
	memset(sBuffer,0,128);
	sprintf(sBuffer,"%0.4f,%0.4f,%0.4f,",g_center_point.x,g_center_point.y,g_center_point.z);
	m_ofs <<sBuffer<< endl;
	memset(sBuffer,0,128);
	sprintf(sBuffer,"%0.4f,%0.4f,%0.4f,",g_right_point.x,g_right_point.y,g_right_point.z);
	m_ofs <<sBuffer<< endl;
	m_ofs.flush();
	m_ofs.close();
}
void CCamera_Select_CardDlg::Load3Point(void)
{
	ifstream m_ifs;
	m_ifs.open ("config/point3.conf",ios::in);
	if(!m_ifs) 
	{
		MessageBox("���ļ�<config/point3.conf>ʧ��");
		return ;
	}
	char cLine[128] = {0};
	char sBuffer[16] = {0};
	string strLine;
	int nPos = 0;

		memset(cLine,0,128);
		m_ifs.getline(cLine,128);
		strLine = cLine;OperDobot::strTrim(strLine);
		nPos = strLine.find (",");memset(sBuffer,0,16);
		memcpy(sBuffer,strLine.c_str(),nPos);
		g_left_point.x = atof(sBuffer);
		strLine.erase(0,nPos+1);

		nPos = strLine.find (",");memset(sBuffer,0,16);
		memcpy(sBuffer,strLine.c_str (),nPos);
		g_left_point.y = atof(sBuffer);
		strLine.erase(0,nPos+1);

		nPos = strLine.find (",");memset(sBuffer,0,16);
		memcpy(sBuffer,strLine.c_str (),nPos);
		g_left_point.z = atof(sBuffer);
		strLine.erase(0,nPos+1);


	nPos = 0;
		memset(cLine,0,128);
		m_ifs.getline(cLine,128);
		strLine = cLine;OperDobot::strTrim(strLine);

			nPos = strLine.find (",");memset(sBuffer,0,16);
			memcpy(sBuffer,strLine.c_str(),nPos);
			g_center_point.x = atof(sBuffer);
			strLine.erase(0,nPos+1);

			nPos = strLine.find (",");memset(sBuffer,0,16);
			memcpy(sBuffer,strLine.c_str (),nPos);
			g_center_point.y = atof(sBuffer);
			strLine.erase(0,nPos+1);

			nPos = strLine.find (",");memset(sBuffer,0,16);
			memcpy(sBuffer,strLine.c_str (),nPos);
			g_center_point.z = atof(sBuffer);
			strLine.erase(0,nPos+1);
		
		memset(cLine,0,128);
		m_ifs.getline(cLine,128);
		strLine = cLine;OperDobot::strTrim(strLine);

			 nPos = strLine.find (",");memset(sBuffer,0,16);
			memcpy(sBuffer,strLine.c_str(),nPos);
			g_right_point.x = atof(sBuffer);
			strLine.erase(0,nPos+1);
			nPos = strLine.find (",");memset(sBuffer,0,16);
			memcpy(sBuffer,strLine.c_str (),nPos);
			g_right_point.y = atof(sBuffer);
			strLine.erase(0,nPos+1);

			nPos = strLine.find (",");memset(sBuffer,0,16);
			memcpy(sBuffer,strLine.c_str (),nPos);
			g_right_point.z = atof(sBuffer);
			strLine.erase(0,nPos+1);
	m_ifs.close();
}
void CCamera_Select_CardDlg::OnBnClickedButtonDobotGoHome()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((OperDobot2 *)g_dobot)->GoHome();
}
void CCamera_Select_CardDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: �ڴ˴������Ϣ����������
	g_bConnected = false;
	g_dobot->DobotStop();
}
