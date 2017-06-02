// Comm.h: interface for the CComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(COMM_H__)
#define COMM_H__
class SerialPort
{
protected:
	// ´®¿ÚÉè±¸¾ä±ú
	HANDLE hComm;
	DCB dcb;
	COMMTIMEOUTS to;
	bool bIsOpen,bIsInit;
	int m_BaudRate;
public:
	SerialPort();
	virtual ~SerialPort();
	BOOL OpenComm(CString strPort, int nBaudRate=57600, int nParity=NOPARITY, int nByteSize=8, int nStopBits=ONESTOPBIT);
//	BOOL OpenComm();
	BOOL CloseComm();
	int ReadComm(void* pData, int nLength);
	int WriteComm(void* pData, int nLength);

//int byteSendData(const byte* byteBuff,int nSize){  return  WriteComm((void *)byteBuff, nSize);}	//
//int	byteReadData(byte* byteBuff,int nSize) { return ReadComm((void *)byteBuff, nSize); }//
};
#endif // !defined(COMM_H__)
