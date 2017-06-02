#include "stdafx.h"
#include "MatShowAndJPG2Mat.h"

int ShowJPG(unsigned char *pData,int iDataLen,CWnd *pwnd)
{
//char strTmp[MAX_PARA_LEN];
CString strPath;	

	IStream *pStm;  
	IPicture *pPic;
	OLE_XSIZE_HIMETRIC hmWidth;    
	OLE_YSIZE_HIMETRIC hmHeight;    
	CDC* pDC;
//	int s32PicWidth;
//	int s32PicHeight;
	int x = 0;
	int y = 0;
//CFileStatus fstatus; 
//CFile file; 
	LPVOID pvData;
	ULONGLONG cb = iDataLen;  
    HGLOBAL hGlobal;
	

	/************************************************
	step2: 打开 jpeg 文件 m_apFileName[nId]
	*************************************************/
/*	strPath.Format("C:/Users/teng/Pictures/4.jpg");
	if (!file.Open(strPath,CFile::modeRead))  
	{  
		AfxMessageBox("file has not found");
		return -1;  
	}  
	if (!file.GetStatus(strPath, fstatus))  
	{  
		AfxMessageBox("file status is not right");
		return -1;  
	}  
	if ((cb =fstatus.m_size)<=0)  
	{  
		AfxMessageBox("file size is error");
		return -1;  
	} 
*/
	/************************************************
	step3: 根据文件大小分配内存空间,记得释放内存  
	*************************************************/
	hGlobal = GlobalAlloc(GMEM_MOVEABLE, (unsigned int)cb);    
	if (hGlobal == NULL)   
	{  
		return -1;  
	}  

	/************************************************
	step4:  锁定刚才分配的内存空间  
	*************************************************/	
	pvData = GlobalLock(hGlobal);  
	if (pvData == NULL)    
	{    
		GlobalFree(hGlobal);  // 记得释放内存  
		return -1;  
	} 

	/************************************************
	step5:  将文件放到流中  
	*************************************************/		  
	//file.Read(pvData,(unsigned int)cb);    
memcpy(pvData,pData,iDataLen);
	GlobalUnlock(hGlobal);    
	CreateStreamOnHGlobal(hGlobal, TRUE, &pStm); 

	/************************************************
	step6:  从流中加载图片 (能显示jpeg图片和gif图片的一帧)
	*************************************************/		   
	if(OleLoadPicture(pStm,(LONG)iDataLen,TRUE,IID_IPicture,(LPVOID*)&pPic)!=S_OK)   
	{   
		GlobalFree(hGlobal);  // 记得释放内存  
		return -1;  
	}  

	/************************************************
	step7: 获取图像宽和高,注意这里的宽和高不是图像的分辨率  
	*************************************************/	
	pPic->get_Width(&hmWidth);    
	pPic->get_Height(&hmHeight);   

	/************************************************
	step8: use render function display image  
	*************************************************/		
	//x = m_aCRectThumbNail[nId].left;
	//y = m_aCRectThumbNail[nId].top;
	//s32PicWidth = m_aCRectThumbNail[nId].right - m_aCRectThumbNail[nId].left;
	//s32PicHeight = m_aCRectThumbNail[nId].bottom - m_aCRectThumbNail[nId].top;


	CRect rect;
	pwnd->GetClientRect( &rect ); // 获取控件尺寸位置
	pDC = pwnd->GetDC();

	//pDC = g_dlg->GetDC();
	//if(FAILED(pPic->Render(*pDC, x, y, s32PicWidth, s32PicHeight, 0, hmHeight, hmWidth, -hmHeight, NULL))) 
	if(FAILED(pPic->Render(*pDC, 0, 0, rect.Width(),rect.Height(), 0, hmHeight, hmWidth, -hmHeight, NULL))) 
	{  
		pwnd->ReleaseDC(pDC); 
		pPic->Release();  
		GlobalFree(hGlobal);  // 记得释放内存  
		return -1;  
	} 
	CPen *penXY = new CPen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen *pOldPen = pDC->SelectObject(penXY);
	pDC->MoveTo(int(rect.Width()/2.0-50),int(rect.Height()/2.0));
	pDC->LineTo(int(rect.Width()/2.0+50),int(rect.Height()/2.0));
	pDC->MoveTo(int(rect.Width()/2.0),int(rect.Height()/2.0 - 50));
	pDC->LineTo(int(rect.Width()/2.0),int(rect.Height()/2.0 + 50));
	pDC->SelectObject(pOldPen);
	delete penXY;

	if (NULL != hGlobal)
    {
		GlobalFree(hGlobal);  // 记得释放内存 	
    }
    pwnd->ReleaseDC(pDC); 
	pPic->Release(); 

	return 0;
}
IplImage* Jpeg2Ipl(const uchar *jpegData, int jpegSize)
{
IplImage *_pImg = 0;

struct jpeg_error_mgr jerr;
struct jpeg_decompress_struct cinfo;

cinfo.err = jpeg_std_error(&jerr);
jpeg_create_decompress(&cinfo);

//jpeg_stdio_mem_src(&cinfo, (char *)jpegData, jpegSize);
jpeg_mem_src(&cinfo, jpegData, jpegSize);  

jpeg_read_header(&cinfo, TRUE);

jpeg_start_decompress(&cinfo);

int nRowSize = cinfo.output_width * cinfo.output_components;
int w =cinfo.output_width;
int h =cinfo.output_height;

char *bmpBuffer = (char*)malloc(h*w*3);
JSAMPARRAY pBuffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, nRowSize, 1);

while(cinfo.output_scanline < cinfo.output_height)
{
	jpeg_read_scanlines(&cinfo, pBuffer, 1);
	int start=nRowSize*(cinfo.output_scanline-1);
	memcpy(bmpBuffer+start, pBuffer[0],nRowSize);
	/*for(int i=0;i<nRowSize;i++)
	{
		bmpBuffer[start+i]=pBuffer[0][i];
	}*/
}
jpeg_finish_decompress(&cinfo);
jpeg_destroy_decompress(&cinfo);

_pImg = cvCreateImage(cvSize(w,h),8,3);

for (h=0; h<_pImg->height; h++)
{
for (w=0; w<_pImg->widthStep; w+=3)
{
*(_pImg->imageData+h*_pImg->widthStep+w+0)=*(bmpBuffer+h*_pImg->widthStep+w+2);
*(_pImg->imageData+h*_pImg->widthStep+w+1)=*(bmpBuffer+h*_pImg->widthStep+w+1);
*(_pImg->imageData+h*_pImg->widthStep+w+2)=*(bmpBuffer+h*_pImg->widthStep+w+0);
}
}

//memcpy(_pImg->imageData,bmpBuffer,h*w*3);
//Mat m(_pImg);
//cvCvtColor(bmpBuffer,_pImg,CV_RGB2BGR);
free(bmpBuffer);

bmpBuffer=NULL;
return _pImg;
}

Mat Jpeg2Mat(const uchar *jpegData, int jpegSize)
{
	struct jpeg_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, jpegData, jpegSize);  
	
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	int nRowSize = cinfo.output_width * cinfo.output_components;
	int w =cinfo.output_width;
	int h =cinfo.output_height;

	char *bmpBuffer = (char*)malloc(h*w*3);
	JSAMPARRAY pBuffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, nRowSize, 1);
	while(cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, pBuffer, 1);
		int start=nRowSize*(cinfo.output_scanline-1);
		memcpy(bmpBuffer+start, pBuffer[0],nRowSize);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	Mat mMat(h, w, CV_8UC3,bmpBuffer);
	Mat m(h, w, CV_8UC3, cv::Scalar(0,0,255));
	cvtColor(mMat, m, COLOR_RGB2BGR);
	free(bmpBuffer);
	bmpBuffer=NULL;
	return m;
}

void showMatImgToWnd(CWnd* pWnd, const cv::Mat& img)
{ 
	if(img.empty()) return; 
	static BITMAPINFO *bitMapinfo = NULL;
	static bool First=TRUE;
	if(First)
	{
		BYTE *bitBuffer = new BYTE[40+4*256];//开辟一个内存区域
		if(bitBuffer == NULL)
		{ 
			return;
		}
		First=FALSE;
		memset(bitBuffer, 0, 40+4*256);
		bitMapinfo = (BITMAPINFO *)bitBuffer;
		bitMapinfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
		bitMapinfo->bmiHeader.biPlanes   = 1;
		for(int i=0; i<256; i++)
		{ //颜色的取值范围 (0-255)
			bitMapinfo->bmiColors[i].rgbBlue  =bitMapinfo->bmiColors[i].rgbGreen = bitMapinfo->bmiColors[i].rgbRed   =(BYTE) i;
		}
	}
	bitMapinfo->bmiHeader.biHeight = -img.rows;
    bitMapinfo->bmiHeader.biWidth = img.cols;
	bitMapinfo->bmiHeader.biBitCount= img.channels() *8;

	CRect drect;       
	pWnd->GetClientRect(drect);    //pWnd指向CWnd类的一个指针 
	CClientDC dc(pWnd);
	HDC hDC =dc.GetSafeHdc();                  //HDC是Windows的一种数据类型，是设备描述句柄；
	SetStretchBltMode(hDC, COLORONCOLOR);    
	StretchDIBits(hDC,0,0,
		drect.right,  //显示窗口宽度
		drect.bottom,  //显示窗口高度
		0,
		0,
		img.cols,     //图像宽度
		img.rows,     //图像高度
		img.data,   
		bitMapinfo,   
		DIB_RGB_COLORS,
		SRCCOPY
      );
}

unsigned char *g_buff = new unsigned char[102400];
int g_iBuffLen = 0;
bool g_bNetConnected = false;
CCriticalSection g_cs;
typedef struct _Data_Head
{
	uint32_t iCommand;
	uint32_t iDataLen;
	uint32_t iRoomId;
} DataHead;
int CanReceive(int fd,int p_iUsec)
{
	fd_set rf;
	struct timeval tox;
	
	FD_ZERO(&rf);
	FD_SET(fd, &rf);
	int i = p_iUsec / 1000 / 1000;
	tox.tv_sec = i;
	tox.tv_usec = p_iUsec % (1000 * 1000);
	//return select(fd + 1, &rf, NULL, NULL, &tox);
	return select(FD_SETSIZE, &rf, NULL, NULL, &tox);
}
int FromServerGetData()
{
	struct sockaddr_in ser;
	ser.sin_family = AF_INET;                                                       
	ser.sin_port = htons(9999);                                                          
	ser.sin_addr.s_addr = inet_addr("10.240.1.105");                                        
	SOCKET iFd = socket(AF_INET,SOCK_STREAM,0);                                        
	if(iFd == INVALID_SOCKET)                                                   
	{     
		char sMsg[128] = {0};
		sprintf_s(sMsg,"socket() Failed: %d/n",WSAGetLastError());
		closesocket(iFd);
		AfxMessageBox(sMsg);
		return -1;                                                                  
	}                                                                                                 
	if(connect(iFd,(struct sockaddr *)&ser,sizeof(ser)) == INVALID_SOCKET)      
	{  
		char sMsg[128] = {0};
		sprintf_s(sMsg,"connect() Failed: %d/n",WSAGetLastError());
		closesocket(iFd);
		AfxMessageBox(sMsg);
		return -1;                                                                  
	}                                                                               
	else                                                                            
	{     
		g_bNetConnected = TRUE;
		DataHead head;
		head.iCommand = htonl(0x00000002);
		head.iDataLen = htonl(0);
		head.iRoomId = htonl(0);
		send(iFd,(char *)&head,12,0);
		while(g_bNetConnected)
		{
			char buf[16] = {0};
			if(CanReceive(iFd,1000*1000))
			{                                                   
				int iLen = recv(iFd,buf,12,0); 
				if(iLen < 12)
				{
					g_bNetConnected = FALSE;
					break;
				}
				DataHead *h = (DataHead *)buf;
				int iCommand = ntohl(h->iCommand);
				int iDataLen = ntohl(h->iDataLen);
				int iTotal = 0;
				unsigned char *pData = new unsigned char[iDataLen+1];
				while(iTotal < iDataLen)
				{
					iLen = recv(iFd,(char *)pData+iTotal,iDataLen-iTotal,0);
					if(iLen <= 0)
					{
						g_bNetConnected = FALSE;
						break;
					}
					iTotal += iLen;
				}
				//g_dlg->DrawImage(pData,iDataLen);
				g_cs.Lock();
				g_iBuffLen = iDataLen;
				memcpy(g_buff,pData,iDataLen);
				g_cs.Unlock();
//				ShowJPG(pData,iDataLen);
/*	
IplImage* p =  Jpeg2Ipl((char *)g_buff,g_iBuffLen);	
cvNamedWindow("sobel", 1);  
cvShowImage("sobel", p);
waitKey(0);	
cvReleaseImage(&p);
break;
*/				
				delete [] pData;
			}
		}
	}                                                                               
	closesocket(iFd);
	return 0;
}
