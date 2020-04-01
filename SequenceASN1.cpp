#include"SequenceASN1.h"

SequenceASN1::SequenceASN1()
{
}

SequenceASN1::~SequenceASN1()
{
}

int SequenceASN1::writeHeadNode(int iValue)
{
	ITCAST_INT ret = -1;
	ret = DER_ItAsn1_WriteInteger((ITCAST_UINT32)iValue, &m_header);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_WriteInteger() err");
		return -1;
	}

	m_next = m_header;
	return 0;
}

int SequenceASN1::writeHeadNode(char *sValue, int len)
{
	ITCAST_INT ret = -1;
	ret = DER_ITCAST_String_To_AnyBuf(&m_tmp, (unsigned char*)sValue, len);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ITCAST_String_To_AnyBuf() err");
		return -1;
	}

	ret = DER_ItAsn1_WritePrintableString(m_tmp, &m_header);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_WritePrintableString() err");
		return -1;
	}

	DER_ITCAST_FreeQueue(m_tmp);

	m_next = m_header;
	return 0;
}

// 添加后继结点
int SequenceASN1::writeNextNode(int iValue)
{
	ITCAST_INT ret = -1;
	ret = DER_ItAsn1_WriteInteger(iValue, &(m_next->next));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_WriteInteger() err");
		return -1;
	}

	m_next = m_next->next;
	return 0;
}

int SequenceASN1::writeNextNode(char *sValue, int len)
{
	ITCAST_INT ret = -1;
	ret = DER_ITCAST_String_To_AnyBuf(&m_tmp, (unsigned char*)sValue, len);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ITCAST_String_To_AnyBuf() err");
		return -1;
	}

	ret = DER_ItAsn1_WritePrintableString(m_tmp, &(m_next->next));
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_WritePrintableString() err");
		return -1;
	}

	DER_ITCAST_FreeQueue(m_tmp);
	m_next = m_next->next;
	return 0;
}


// 读头结点数据
int SequenceASN1::readHeadNode(int &iValue)
{
	ITCAST_INT ret = -1;
	ret = DER_ItAsn1_ReadInteger(m_header, (ITCAST_UINT32*)&iValue);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_ReadInteger() err");
		return -1;
	}

	m_next = m_header->next;
	return 0;
}

int SequenceASN1::readHeadNode(char *sValue)
{
	ITCAST_INT ret = -1;
	ret = DER_ItAsn1_ReadPrintableString(m_header, &m_tmp);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_ReadPrintableString() err");
		return -1;
	}

	memcpy(sValue, m_tmp->pData, m_tmp->dataLen);

	DER_ITCAST_FreeQueue(m_tmp);

	m_next = m_header->next;
	return 0;
}

// 读后继结点数据
int SequenceASN1::readNextNode(int &iValue)
{
	ITCAST_INT ret = -1;
	ret = DER_ItAsn1_ReadInteger(m_next, (ITCAST_UINT32*)&iValue);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_ReadInteger() err");
		return -1;
	}

	m_next = m_next->next;
	return 0;
}

int SequenceASN1::readNextNode(char *sValue)
{
	ITCAST_INT ret = -1;
	ret = DER_ItAsn1_ReadPrintableString(m_next, &m_tmp);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_ReadPrintableString() err");
		return -1;
	}

	memcpy(sValue, m_tmp->pData, m_tmp->dataLen);

	DER_ITCAST_FreeQueue(m_tmp);
	m_next = m_next->next;
	return 0;
}

// 打包链表
int SequenceASN1::packSequence(char **outData, int &outLen)
{
	ITCAST_INT ret = -1;
	char *out = NULL;
	ret = DER_ItAsn1_WriteSequence(m_header, &m_tmp);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_WriteSequence() err");
		return -1;
	}

	//传出
	out = new char[m_tmp->dataLen + 1];

	memset(out, 0, m_tmp->dataLen + 1);
	memcpy(out, m_tmp->pData, m_tmp->dataLen);

	*outData = out;
	outLen = m_tmp->dataLen;

	DER_ITCAST_FreeQueue(m_tmp);
	return 0;
}


// 解包链表
int SequenceASN1::unpackSequence(char *inData, int inLen)
{
	ITCAST_INT ret = -1;
	ret = DER_ITCAST_String_To_AnyBuf(&m_tmp, (unsigned char*)inData, inLen);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ITCAST_String_To_AnyBuf() err");
		return -1;
	}

	ret = DER_ItAsn1_ReadSequence(m_tmp, &m_header);
	if (0 != ret)
	{
		mLog.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func DER_ItAsn1_ReadSequence() err");
		return -1;
	}


	DER_ITCAST_FreeQueue(m_tmp);
	return 0;
}

// 释放链表
void SequenceASN1::freeSequence(ITCAST_ANYBUF *node)
{
	if (NULL == node)
	{
		DER_ITCAST_FreeQueue(m_header);
	}
	else
	{
		DER_ITCAST_FreeQueue(node);
	}
}