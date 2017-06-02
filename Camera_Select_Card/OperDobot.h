/*------------------------------------------------------------------------
File name:             OperDobot.h
Author:                linhu
Version:               1.0
Date:                  2016 - 03 - 2
Description:           操作Dobot类
Revision history:      
   1:
     Date:				
     Author:			
     Modifiacation:		
   2：……
------------------------------------------------------------------------*/

#ifndef _OPERDOBOT_H_
#define _OPERDOBOT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Comm.h"
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>
using namespace std;

unsigned int __stdcall ReadCom(void * param);

//#include "common/CriticalLock.h"
#define DOBOTDATABUFFERSIZE	42
#define FOURZERO 0x00000000 ;

//客户端下发指令的格式
//每帧数据共（2 + 4 * 10） = 42个字节，其中包括包头（0xA5）、包尾（0x5A）和 10 个参数，
//所有参数均为单精度浮点型（32 位），每个参数大小为4个字节
typedef struct
{
	unsigned char cHead[1] ;
	unsigned char cState[4] ;
	unsigned char cAxis[4] ;
	unsigned char cX[4] ;		//底座绝对角度
	unsigned char cY[4] ;		//大臂绝对角度
	unsigned char cZ[4] ;		//小臂绝对角度
	unsigned char cRHead[4] ;	//舵机旋转绝对角度
	unsigned char cIsGrip[4] ;
	unsigned char cStartVe[4] ;
	unsigned char cEndVel[4] ;
	unsigned char cMaxVe[4] ;
	unsigned char cEnd[1] ;
	HWND hWins;
	bool bStart;
} DobotOrder;

//Dobot返回请求指令格式
//每帧数据共（2 + 4 * 10） = 42个字节，其中包括包头（0xA5）、包尾（0x5A）和10个参数，
//所有参数均为单精度浮点型（32 位），每个参量大小为4个字节
typedef struct
{
	unsigned char cHead[1] ;
	unsigned char cX[4] ;
	unsigned char cY[4] ;
	unsigned char cZ[4] ;
	unsigned char cR[4] ;
	unsigned char cBaseAngel[4] ;
	unsigned char cLongAngel[4] ;
	unsigned char cShortAngel[4] ;
	unsigned char cPawAngel[4] ;
	unsigned char cIsGrip[4] ;
	unsigned char cGripperAngel[4] ;
	unsigned char cEnd[1] ;
} DobotStatus;
typedef struct
{
	float x;
	float y;
	float z;
} CDobotPoint,CRelativeSpace,CAngle;
typedef struct
{
	int nLooped ;
	bool bExeStop;
	DobotStatus m_DS;
	char cErrInfo[128];
} DobotAndWinStatus;

typedef struct 
{
	CDobotPoint dp_point;
	CAngle an_point;
	float pawArmAngle;
	unsigned long lInitFee;
	unsigned long lBeginWFee;
	unsigned long lEndWFee;
	int iSucceedSum;
	int iFailureSum;
	char szCardNo[24];
} CardInf;

typedef list<DobotOrder> CMDLIST;

class OperDobot : public SerialPort
{
protected:
	int nStatSend;
	string m_strCom,strErrInfo,strRX;
	bool bStatus;
	byte byteDobot2PCData[DOBOTDATABUFFERSIZE+1];//
	long lRecv,bList;
	void * ListParam;
	HWND hWin;
	DobotAndWinStatus * pDWS;
	ofstream m_statofs;
	string strPos;
	CMDLIST m_CMDList;
public:
	OperDobot();
	virtual ~OperDobot();
	void SetLogInfoFun(void (*f)(const char *)){ logInfo =  f;}
	virtual CDobotPoint GetCurrentAbsolutePosition();
	virtual CAngle GetCurrentCAnglePosition();
	CDobotPoint GetStartAbsolutePosition();
	virtual void Move2AbsolutePosition(const CDobotPoint &pt,float angle = 0.0);
	CDobotPoint Move2RelativePosition(const CRelativeSpace &sp);
	CDobotPoint MarkADobotPiont();
	void Move2AnglePoint(const CAngle &pt,float angle,float startVe=1.0);
	void Go2StartPoint();
	static bool IsSameDobotPoint(const CDobotPoint& p1,const CDobotPoint& p2)
	{
		return fabs(p1.x-p2.x) < 0.1 && fabs(p1.y-p2.y) < 0.1  && fabs(p1.z-p2.z) < 0.1;
	}
	virtual int GetVersion(){ return 1; }

	//CCriticalSection  theLogCritical;
	CCriticalSection  theDobotStatusCritical;
	//CCriticalSection  theDobotOrderCritical;
	CCriticalSection  m_csList;
	DobotStatus m_DobotStartStatus;
	DobotStatus m_DobotCurStatus;
	DobotAndWinStatus m_DobotAndWinStatus;
	string GetErrInfo(){return strErrInfo;};
	CDobotPoint m_CurrentMarkedDobotPoint;
	void (*logInfo)(const char *);
	bool m_bRunning;
	bool m_bGrab;

	bool OpenCom();//打开串口
	void SetCOM(const string& strCom);
//	void CloseCom(){if(bIsOpen){Close();bIsOpen=false;}};//关闭串口
//	bool IsOpenCom(){ return bIsOpen;};//关闭串口
	virtual int ReadFromDobot();
	virtual void SendOrderFromList();
	int GetList();
	int SendOrder2Dobot(DobotOrder &order);
	virtual void AddOrderList(DobotOrder &order);
	virtual void DobotStart();
	virtual void DobotStop();
	virtual void SetSpeed(float f1,float f2,float f3,float f4,float f5,float f6,float f7,float f8=0.0);
	void (*ShowDobotStatus)(float f1,float f2,float f3,float f4,float f5,float f6,float f7,float f8,float f9,float f10);
	virtual void PointMoving(int nAxis,bool bAngle);
	virtual void SetGrab(bool b);
	
	CardInf m_CardInfo[4];
	CDobotPoint m_ReadMachePoint;
	CDobotPoint m_WorkCenterPoint;

	static void FloatToByte(float floatNum, unsigned char* byteArry);
	static float Hex_To_Decimal(unsigned char *Byte, int num);//十六进制到浮点数
	static void stringToHEX(const string& strTmp,unsigned char* ucHEX,int nstrLen);
	static void charToHEX(char c,int& ucTmp);
	static string& strTrim(string& strValue);
	static string& strRTrim(string& strValue);
	static string& strLTrim(string& strValue);
	static string IToString(const int&,int nLen=0);
};
#endif


