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

//CMemPacket::CMemPacket(char* _data)
//{
//	bIsErr = false;
//	opType = OP_READ;
//	curByte = 0;
//	maxByte = strlen(_data);
//	data = new char[maxByte];
//
//	memcpy(data, _data, maxByte);
//}

bool CMemPacket::SetData(const char* _data)
{
	if (!_data)
		return _setErr();

	memcpy(data, _data, maxByte);
	return true;
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
		memcpy(_data, data, len);
		curByte += len;
		data += len;
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
		memcpy(data+curByte, _data, len);
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

bool CMemPacket::Read(char** v)
{
	*v = data;
	curByte += strlen(data) + 1;
	data += strlen(data) + 1;

	return true;
}

bool CMemPacket::Write(const char* v)
{
	if (!v)
		return _setErr();
	return WriteData((const void*)v, strlen(v)+1);
}

bool CMemPacket::_setErr()
{
	bIsErr = true;
	return false;
}
