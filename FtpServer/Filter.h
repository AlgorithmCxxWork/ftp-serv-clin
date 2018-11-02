/***************************************************
*
*	@�汾:	1.0
*	@����:	Mengxl
*	@ʱ��:	2018-11-02 10:55:17
*	@�ļ�:	Filter.h
*	@������
*
****************************************************/

#ifndef FILTER_INCLUDE_H
#define FILTER_INCLUDE_H
#include "ftp.h"

class CBaseFilter
{
public:
	CBaseFilter(CBaseFilter *filter = NULL);
	virtual	~CBaseFilter(){}
	virtual BOOL	filter(FTP_Connection_s *conn);
	void			setNextFilter(CBaseFilter *next){m_next = next;}
protected:
	CBaseFilter		*m_next;
};

class CLogStatusFilter:public CBaseFilter
{
public:
	CLogStatusFilter(CBaseFilter *next=NULL);
	virtual	BOOL	filter(FTP_Connection_s *conn);
};

class CPasvFilter:public CBaseFilter
{
public:
	CPasvFilter(CBaseFilter *next=NULL);
	virtual BOOL  filter(FTP_Connection_s *conn);
};

#endif