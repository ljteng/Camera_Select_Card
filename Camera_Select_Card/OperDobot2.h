#pragma once
#include "operdobot.h"
#include "stdint.h"
#include <vector>

typedef struct tagPTPCmd {
uint8_t ptpMode;
float x;
float y;
float z;
float r;
} PTPCmd;
typedef struct tagPose {
float x; //机械臂坐标系x
float y; //机械臂坐标系y
float z; //机械臂坐标系z
float r; //机械臂坐标系r
float jointAngle[4]; //机械臂4 轴角度
} Pose;

typedef struct tagBufferE
{
	uint8_t* pData;
	int iLen;
} BufferE;

class OperDobot2 :
	public OperDobot
{
private:
	//bool m_bHavingRecvData;
	Pose m_CurrentPos;
	//vector<BufferE> _cmd_list;
protected:

public:
	OperDobot2(void);
	virtual ~OperDobot2(void);
	void AddOrderList(DobotOrder &order);
	void Move2AbsolutePosition(const CDobotPoint &pt,float angle = 0.0);
	void SetGrab(bool b);
	void SetSpeed(float f1,float f2,float f3,float f4,float f5,float f6,float f7,float f8=0.0);
	int ReadFromDobot();
	void SendGetPoseOrder();
	int GetVersion(){ return 2; }
	virtual void PointMoving(int nAxis,bool bAngle);
	virtual void DobotStart();
	Pose GetCurrentPoint2();
	void GoHome();
	void ClearCmd();
	void SetCmdDownLoadStar(uint32_t totalLoop, uint32_t linePerLoop);
	void SetCmdDownLoadStop();
	BufferE CreateMove2AbsoluteCmd(const CDobotPoint &pt);
	BufferE CreateGohomeCmd();
	BufferE CreateTimeWaitCmd(int timeout);
	BufferE CreateGrabCmd(bool b);
};
