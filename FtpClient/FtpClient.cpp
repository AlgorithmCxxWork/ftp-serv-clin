#include "FtpClient.h"

#pragma comment(lib,"ws2_32.lib")

CFtpClient::CFtpClient()
	:m_socket(INVALID_SOCKET)
{
	this->initSocket();
}

CFtpClient::~CFtpClient()
{
	if(m_socket!=INVALID_SOCKET)
	{
		closesocket(m_socket);
	}
	WSACleanup();
}

bool CFtpClient::connectToHost(const string &serverIP,int serverPort/* =21 */)
{
	char		respCode[4] = {0};
	string		resStr;
	ULONG	ip = inet_addr(serverIP.c_str());
	m_socket = this->createSocket(ntohl(ip),serverPort);
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}
	bool result = this->readResponse(respCode,resStr);
	if(!result)
	{
		return false;
	}
	if(respCode[0]!='2')
	{
		return false;
	}
	return true;
}

bool	CFtpClient::login(const string &user,const string &password)
{
	string		respStr;
	bool		result = this->sendCommand("user",user.c_str());
	if(!result)
	{
		goto ERR;
	}
	result = this->getResponseResult('3',respStr);
	if(!result)
	{
		goto ERR;
	}
	result = this->sendCommand("pass",password.c_str());
	if(!result)
	{
		goto ERR;
	}
	return this->getResponseResult('2',respStr);
ERR:
	return false;
}

bool CFtpClient::cd(const string &path)
{
	string		respStr;
	bool	result = this->sendCommand("cwd",path.c_str());
	if(!result)
	{
		return false;
	}
	return this->getResponseResult('2',respStr);
}

bool CFtpClient::deleteFile(const string &path)
{
	string		respStr;
	bool	result = this->sendCommand("dele",path.c_str());
	if(!result)
	{
		return false;
	}
	return this->getResponseResult('2',respStr);
}

bool CFtpClient::rmdir(const string &path)
{
	string		respStr;
	bool	result = this->sendCommand("rmd",path.c_str());
	if(!result)
	{
		return false;
	}
	return this->getResponseResult('2',respStr);
}

bool CFtpClient::mkdir(const string &path)
{
	string		respStr;
	bool	result = this->sendCommand("mkd",path.c_str());
	if(!result)
	{
		return false;
	}
	return this->getResponseResult('2',respStr);
}

string	CFtpClient::pwd()
{
	string		respStr;
	bool	result = this->sendCommand("pwd","");
	if(!result)
	{
		return false;
	}
	this->getResponseResult('2',respStr);

	int		left = respStr.find("\"");
	int		right = respStr.rfind("\"");
	return respStr.substr(left+1,right-left-1);
}

void CFtpClient::list(const string &path,vector<Ftp_File_s*> &fileList)
{
	USHORT	port = 0;
	ULONG	serverIP = 0;
	if (!this->parsePasvData(&serverIP, &port))
	{
		return;
	}
	if(!this->openDataChannel(serverIP,port))
	{
		return;
	}
	this->sendCommand("List", path.c_str());
	char recvBuf[4096] = {0};
	int	 unParseBytes = 0;//�洢recvBuf�����ж����ֽڻ�δ����
	int	 readBytes = 0;//�洢����recvʵ�ʶ�ȡ�˶����ֽ�
	int	 totalBytes = 0;//�����ܵĿɴ����ִ���=unParseBytes+readBytes��
	while((readBytes = recv(m_dataSocket, recvBuf + unParseBytes, 4095-unParseBytes, 0)) >0)
	{
		totalBytes = unParseBytes + readBytes;
		recvBuf[totalBytes] = 0;
		if(strstr(recvBuf,"\r\n")!=NULL)
		{
			//parseBytes���ڴ洢һ��parseLines�����˶����ֽڡ�
			int parseBytes = parseLines(recvBuf,totalBytes,fileList);
			if(totalBytes - parseBytes > 0)
			{
				memcpy(recvBuf,recvBuf+parseBytes,totalBytes-parseBytes);
			}
			unParseBytes = totalBytes - parseBytes;
			//printf("�����ֽ�:%d,�յ��ֽ�:%d\n",parseBytes,bytes);
		}
	}
}

void CFtpClient::put(const string &remoteFile, const string &localFile)
{
	string respStr;
	ULONG	serverIP = 0;
	USHORT  port = 0;
	if (!this->parsePasvData(&serverIP, &port))
	{
		return;
	}
	if(!this->openDataChannel(serverIP,port))
	{
		return;
	}
	HANDLE	tmpFileHandle = ::CreateFile(localFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (tmpFileHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	char	*msg = (char*)malloc(1024 * 1024);
	if (msg == NULL)
	{
		return;
	}
	this->sendCommand("STOR", remoteFile.c_str());
	this->getResponseResult('1', respStr);

	DWORD		tmpFileSize = ::GetFileSize(tmpFileHandle, NULL);
	DWORD		bytesSend = 0;
	while (bytesSend <= tmpFileSize)
	{
		DWORD bytesRead = 0;
		ReadFile(tmpFileHandle, msg, 1024 * 1024, &bytesRead, NULL);
		if (bytesRead == 0)
		{
			break;
		}
		bytesSend += bytesRead;
		send(m_dataSocket, msg, bytesRead, 0);
	}
	::CloseHandle(tmpFileHandle);
	free(msg);
	closesocket(m_dataSocket);
	this->getResponseResult('2', respStr);
}

void CFtpClient::get(const string &remoteFile, const string &localFile)
{
	string respStr;
	ULONG	serverIP = 0;
	USHORT  port = 0;
	if (!this->parsePasvData(&serverIP, &port))
	{
		return;
	}
	if(!this->openDataChannel(serverIP,port))
	{
		return;
	}
	this->sendCommand("RETR", remoteFile.c_str());
	this->getResponseResult('1', respStr);

	char msg[2332] = { 0 };
	int	bytes = 0;
	FILE	*fp = NULL;
	fopen_s(&fp, localFile.c_str(), "wb");
	while ((bytes = recv(m_dataSocket, msg, 2332,0)) > 0)
	{
		fwrite(msg, bytes, 1, fp);
	}
	fclose(fp);
	closesocket(m_dataSocket);
	this->getResponseResult('2', respStr);
}

bool CFtpClient::quit()
{
	string		respStr;
	bool	result = this->sendCommand("quit","");
	if (!result)
	{
		return false;
	}
	return this->getResponseResult('2', respStr);
}

bool CFtpClient::sendRawCommand(const char *cmd,const char *data,char expectCode)
{
	string		respStr;
	if(!this->sendCommand(cmd,data))
	{
		return false;
	}
	return this->getResponseResult(expectCode,respStr);
}

SOCKET CFtpClient::createSocket(ULONG serverIP,USHORT port)
{
	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}
	sockaddr_in		sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htonl(serverIP);
	if (connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr)) != 0)
	{
		closesocket(sockfd);
		return INVALID_SOCKET;
	}
	return sockfd;
}

bool CFtpClient::readResponse(char	*resCode,string &str)
{
	char		msgBuf[4096+2] = {0};
	char		codeStr[4] = {0};
	str.clear();
	int		bytes = recv(m_socket,msgBuf,4096,0);
	if(bytes <= 0)
	{
		return false;
	}
	msgBuf[bytes] = 0;
	if(msgBuf[bytes-2]!='\r' || msgBuf[bytes-1]!='\n')
	{
		return false;
	}
	char		*context = NULL;
	char		*tmpTok = strtok_s(msgBuf,"\r\n",&context);
	memcpy(resCode,msgBuf,3);
	m_statusCode = (resCode[0]-'0')*100 + (resCode[1]-'0')*10 + resCode[2]-'0';
	//����һ����Ӧ��������״̬�뼰����������Ϣ���
	while(tmpTok)
	{
		if(tmpTok[3]!='-')
		{
			str.append(tmpTok+3);
			break;
		}
		else
		{
			str.append(tmpTok+4);
		}
		tmpTok = strtok_s(NULL,"\r\n",&context);
	}
	m_desc = str;
	return true;
}

void CFtpClient::initSocket()
{
	WSADATA		wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	m_socket = socket(AF_INET,SOCK_STREAM,0);
}

bool	CFtpClient::sendCommand(const char *cmd,const char *data)
{
	string		sendData = cmd;
	sendData.append(" ").append(data).append("\r\n");
	int bytes = send(m_socket,sendData.c_str(),sendData.length(),0);
	return bytes == sendData.length();
}

bool CFtpClient::getResponseResult(char code,string &respStr)
{
	char		respCode[4] = {0};
	bool result = this->readResponse(respCode,respStr);
	if(!result)
	{
		return false;
	}
	if(respCode[0]!=code)
	{
		return false;
	}
	return true;
}

bool CFtpClient::parsePasvData(ULONG  *prmServerIP, USHORT *prmPort)
{
	char	msg[128] = { 0 };
	string respStr;
	bool result = this->sendCommand("Type", "I");
	if (!result)
	{
		return false;
	}
	this->getResponseResult('2', respStr);
	result = this->sendCommand("PASV", "");
	this->getResponseResult('2', respStr);
	if (!result)
	{
		return false;
	}
	int		ip[4];
	int		port[2];
	//�ֽ���˿ں�
	int		left = respStr.find("(");
	int		right = respStr.find(")");
	string  portStr = respStr.substr(left + 1, right - left - 1);
	sscanf_s(portStr.c_str(), "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1],
		&ip[2], &ip[3], &port[0], &port[1]);
	*prmPort = port[0] * 256 + port[1];
	*prmServerIP = ip[0] * 256 * 256 * 256 + ip[1] * 256 * 256 + ip[2] * 256 + ip[3];
	return true;
}

bool CFtpClient::openDataChannel(ULONG serverIP,USHORT port)
{
	m_dataSocket = this->createSocket(serverIP,port);
	if(m_dataSocket==INVALID_SOCKET)
	{
		return false;
	}
	return true;
}

int CFtpClient::parseLines(char *msg, int bytes, vector<Ftp_File_s*> &fileList)
{
	int		curPos = 0;
	char	*tokEndPos = NULL;
	char	*tokStartPos = msg;
	while( (tokEndPos=strstr(tokStartPos,"\r\n")) != NULL)
	{
		*tokEndPos = 0;
		this->parseFtpFileInfo(tokStartPos,fileList);
		tokStartPos = tokEndPos + 2;
	}
	return tokStartPos - msg;
}

void CFtpClient::parseFtpFileInfo(char *msg, vector<Ftp_File_s*> &fileList)
{
	Ftp_File_s	*fileInfo = new Ftp_File_s;
	if(msg[0]=='-')
	{
		fileInfo->m_isDir = FALSE;
	}
	else
	{
		fileInfo->m_isDir = TRUE;
	}
	//tok+1Ϊ�ڶ����ֵĿ�ʼ
	char *tok = strchr(msg,' ');
	//tok+1Ϊ�������ֵĿ�ʼλ��
	tok = strchr(tok+1,' ');
	//tok+1Ϊ���Ĳ��ֵĿ�ʼλ��
	tok = strchr(tok+1,' ');
	//tokΪ���岿�ֵĿ�ʼλ�ã����岿�ֹ�20�ֽڣ���ʾ�ļ���С
	//����ļ���С����20�ֽڣ���ǰ��ʹ�ÿո���
	tok = strchr(tok+1,' ');
	while(*tok==' ')
	{
		tok++;
	}
	char *byteEnd = strchr(tok,' ');
	//��ǰbyteEnd+1Ϊ��6�������ݡ�
	*byteEnd = 0;
	fileInfo->m_fileSize = _atoi64(tok);
	
	//ִ����strchr��tok+1Ϊ��7��������
	tok = strchr (byteEnd+1, ' ');
	
	//ִ����strchr��tok+1Ϊ��8��������
	tok = strchr(tok+1,' ');
	
	//ִ����strchr��tok+1Ϊ��9��������
	tok = strchr(tok+1,' ');
	memset(fileInfo->m_fileName, 0, sizeof(fileInfo->m_fileName));
	int	bytes = strlen(tok + 1) > MAX_PATH ? MAX_PATH : strlen(tok + 1);
	memcpy(fileInfo->m_fileName, tok + 1, bytes);
	fileList.push_back(fileInfo);
}