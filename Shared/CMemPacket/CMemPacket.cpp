#include "CMemPakcet.h"
#include <string.h>

CMemPacket::CMemPacket()
{
	bIsErr = false;
	opType = OP_NONE;
	curByte = 0;
	maxByte = DEFAULT_PACKET_MAX_SIZE;

	data = new char[DEFAULT_PACKET_MAX_SIZE];
}

CMemPacket::CMemPacket(int _size)
{
	bIsErr = false;
	opType = OP_NONE;
	curByte = 0;
	maxByte = _size;

	data = new char[_size];
}

CMemPacket::CMemPacket(char* _data)
{
	bIsErr = false;
	opType = OP_READ;
	curByte = 0;
	maxByte = strlen(_data);

	data = _data;
}

void CMemPacket::BeginRead()
{
	opType = OP_READ;
	curByte = 0;
	bIsErr = false;
}

void CMemPacket::BeginWrite()
{
	opType = OP_WRITE;
	curByte = 0;
	bIsErr = false;
}

bool CMemPacket::ReadData(void *_data, int len)
{
	if (!_data)
		return _setErr();

	if (opType != OP_READ)
		return _setErr();

	if (curByte + len > maxByte)
		return _setErr();

	if (len > 0)
	{
		memcpy(_data, data+len, len);
		curByte += len;
	}
	return true;
}

bool CMemPacket::WriteData(const void *_data, int len)
{
	if (!_data)
		return _setErr();

	if (opType != OP_WRITE)
		return _setErr();

	if (curByte + len > maxByte)
		return _setErr();

	if (len > 0)
	{
		memcpy(data+len, _data, len);
		curByte += len;
	}
	return true;
}

bool CMemPacket::Read(bool* v)
{
	if (!v)
		return _setErr();

	BYTE _v = (BYTE)*v;
	return Read(&_v);
}

bool CMemPacket::Write(bool v)
{
	BYTE _v = (BYTE)v;
	return Write(_v);
}

//bool CMemPacket::Write(char* v)
//{
//	if (!v)
//		return _setErr();
//
//	int len = strlen(v);
//	return WriteData(v, len);
//}

bool CMemPacket::_setErr()
{
	bIsErr = true;
	return false;
}
