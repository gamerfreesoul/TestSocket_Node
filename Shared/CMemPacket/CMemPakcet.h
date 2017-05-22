#ifndef _MEM_PACKET_H_
#define _MEM_PACKET_H_

#include "../CTypeDef.h"

const int DEFAULT_PACKET_MAX_SIZE = 64 * 1024;

class CMemPacket 
{
	enum EPacketOpType
	{
		OP_NONE,
		OP_READ,
		OP_WRITE,
	};
public:
	CMemPacket();
	CMemPacket(int _size);
	//CMemPacket(char* _data);

	void BeginRead();
	void BeginWrite();

	bool ReadData(void *_data, int len);
	bool WriteData(const void *_data, int len);

	bool Read(bool* v);
	bool Read(char** v);
	bool Read(BYTE* v)				{ return ReadData(v, sizeof(BYTE)); }
	bool Read(char* v)				{ return ReadData(v, sizeof(char)); }
	bool Read(short* v)				{ return ReadData(v, sizeof(short)); }
	bool Read(WORD* v)				{ return ReadData(v, sizeof(WORD)); }
	bool Read(int* v)				{ return ReadData(v, sizeof(int)); }
	bool Read(DWORD* v)				{ return ReadData(v, sizeof(DWORD)); }
	bool Read(INT64* v)				{ return ReadData(v, sizeof(INT64)); }
	bool Read(UINT64* v)			{ return ReadData(v, sizeof(UINT64)); }

	bool Write(bool v);
	bool Write(const char* v);
	bool Write(BYTE v)				{ return WriteData(&v, sizeof(BYTE)); }
	bool Write(char v)				{ return WriteData(&v, sizeof(char)); }
	bool Write(short v)				{ return WriteData(&v, sizeof(short)); }
	bool Write(WORD v)				{ return WriteData(&v, sizeof(WORD)); }
	bool Write(int v)				{ return WriteData(&v, sizeof(int)); }
	bool Write(DWORD v)				{ return WriteData(&v, sizeof(DWORD)); }
	bool Write(INT64 v)				{ return WriteData(&v, sizeof(INT64)); }
	bool Write(UINT64 v)			{ return WriteData(&v, sizeof(UINT64)); }

	bool SetData(const char* _data);
	const char* GetData() const		{ return data; }
	const int GetCurByte() const    { return curByte; }

private:
	bool _setErr();

	int opType;
	int curByte;
	int maxByte;
	bool bIsErr;

	char* data;
};

#endif //_MEM_PACKET_H_
