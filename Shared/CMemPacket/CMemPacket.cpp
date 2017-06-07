#include "CMemPakcet.h"
#include <string.h>

CMemPacket::CMemPacket()
{
	bIsErr = false;
	opType = OP_NONE;
	curByte = 0;
	bitPos = 0;
	maxByte = DEFAULT_PACKET_MAX_SIZE;

	data = new char[DEFAULT_PACKET_MAX_SIZE];
}

CMemPacket::CMemPacket(int _size)
{
	bIsErr = false;
	opType = OP_NONE;
	curByte = 0;
	bitPos = 0;
	maxByte = _size;

	data = new char[_size];
}

bool CMemPacket::SetData(const char* _data, int _size)
{
	if (!_data)
		return _setErr();

	memcpy(data, _data, _size);
	maxByte = _size;
	curByte = 0;
	bitPos = 0;
	return true;
}

void CMemPacket::BeginRead()
{
	opType = OP_READ;
	curByte = 0;
	bitPos = 8;
	bIsErr = false;
}

void CMemPacket::BeginWrite()
{
	opType = OP_WRITE;
	curByte = 0;
	bitPos = 8;
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
		memcpy(_data, data+curByte, len);
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
		memcpy(data+curByte, _data, len);
		curByte += len;
	}
	return true;
}

bool CMemPacket::ReadBit(DWORD v, int len)
{
	if (opType != OP_READ)
		return _setErr();

	if (curByte > maxByte || (curByte == curByte && bitPos >= 8))
		return _setErr();

	static int bitMask[] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
	v = 0;
	while (len > 0)
	{
		if (bitPos == 8)
		{
			bitData = data + curByte;
			bitPos = 0;
			curByte++;
			if (curByte > maxByte || (curByte == curByte && bitPos >= 8))
				return _setErr();
		}
		int canUseBit = 8 - bitPos;
		int useBit = canUseBit;
		if (canUseBit > len)
			canUseBit = len;
		v |= ((*bitData >> (8 - bitPos - useBit)) & bitMask[useBit]) << (len - useBit);
		len -= useBit;
		bitPos += (BYTE)useBit;
	}
	return true;
}

bool CMemPacket::WriteBit(DWORD v, int len)
{
	if (opType != OP_WRITE)
		return _setErr();

	if (curByte >= maxByte)
		return _setErr();

	static int bitMask[] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
	while (len > 0)
	{
		if (bitPos == 8)
		{
			bitData = data + curByte;
			bitPos = 0;
			curByte++;
			if (curByte >= maxByte)
				return _setErr();
			
			*bitData = 0;
		}
		int canUseBit = 8 - bitPos;
		int useBit = canUseBit;
		if (canUseBit > len)
			useBit = len;
		*bitData |= ((v >> (len - useBit)) & bitMask[useBit]) << (8 - bitPos - useBit);
		len -= useBit;
		bitPos += (BYTE)useBit;
	}
	return true;
}

bool CMemPacket::Read(bool* v)
{
	if (!v)
		return _setErr();

	DWORD _v = (DWORD)*v;
	return ReadBit(_v, sizeof(bool));
}

bool CMemPacket::Write(bool v)
{
	DWORD _v = (DWORD)v;
	return WriteBit(_v, sizeof(bool));
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
