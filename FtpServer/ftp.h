#ifndef FTP_DATA_INCLUDE_H
#define FTP_DATA_INCLUDE_H

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <map>
#include <string>

enum FTP_Status
{
	FTP_CONNECTED,
	FTP_NEED_PASSWORD,
	FTP_LOGIN,
	FTP_OPEN_DATA_CHANNEL
};

typedef struct
{
	int				m_fd;
	bufferevent		*m_bev;
	FTP_Status		m_status;//ftp���ӵ�ǰ״̬
	std::string			m_userName;//��ǰ��¼���û���
	std::string			m_rootDir;//��ǰ�û���Ŀ¼��
	char			*m_recvBuf;//�ӿͻ������յ�������
	UINT32			m_recvLen;//��ǰrecvBuf���յ��ֽ�����
	UINT32			m_recvCapacity;//m_recvBuf���ɴ��ֽ���
	char			*m_command;//��ǰ���ڴ����command����
	UINT32			m_commandLen;//�����ַ�����
	char			*m_param;//��ǰ����Ĳ���ֵ
	UINT32			m_paramLen;//�����ַ�����
	std::string		m_workDir;//��ǰ����Ŀ¼
}FTP_Connection_s;

class CFtpCommand
{
public:
	virtual void processCommand(FTP_Connection_s *conn) = 0;
};

#endif