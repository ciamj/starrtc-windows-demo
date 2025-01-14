
// startrtcdemoDlg.h: 头文件
//

#pragma once
#include "CLiveProgram.h"
#include<vector>

#include "CUserManager.h"
#include "CProcessInfo.h"
#include "CPicControlCallback.h"
#include "IChatroomGetListListener.h"
#include "CConfigManager.h"
using namespace std;

// CstartrtcdemoDlg 对话框
class CstartrtcdemoDlg : public CDialogEx, public CPicControlMonitorCallback, public IChatroomGetListListener
{
	// 构造
public:
	CstartrtcdemoDlg(CUserManager* pUserManager, CWnd* pParent = nullptr);	// 标准构造函数
	~CstartrtcdemoDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STARTRTCDEMO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	void clear();

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonBrush();
	void getLiveList();
	
	CLiveProgram* getLiveProgram(CString strName);
	/**
	 * 查询聊天室列表回调
	 */
	virtual int chatroomQueryAllListOK(list<ChatroomInfo>& listData);
public:
	HMODULE m_hRecHinstance;
	vector<CLiveProgram> mVLivePrograms;
	CStatic* mShowPicControlVector[4];
	vector<CProcessInfo*> mProcessVector;
	CUserManager* m_pUserManager;
	CConfigManager* m_pConfig;
	PROCESS_INFORMATION* openProcess(CString strWindowName);
	bool sendMessage(CString winName, CString strData);
	CProcessInfo* findProcessInfo(string strWinName);
	CListBox m_liveListBox;
	CButton m_ShowArea;
	int m_nShowLiveSize;
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonUserRegister();
	void liveExit(void* pParam);
	virtual void closeCurrentLive(void* pParam);
	virtual void startFaceFeature(void* pParam);
	virtual void stopFaceFeature(void* pParam);
	void setConfig(CConfigManager* pConfig);
};
