
// FtpServerUIDlg.h : ͷ�ļ�
//

#pragma once
#include "FtpServer.h"

// CFtpServerUIDlg �Ի���
class CFtpServerUIDlg : public CDialogEx
{
// ����
public:
	CFtpServerUIDlg(CWnd* pParent = NULL);	// ��׼���캯��

	~CFtpServerUIDlg();

// �Ի�������
	enum { IDD = IDD_FTPSERVERUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CFtpServer	*m_server;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
