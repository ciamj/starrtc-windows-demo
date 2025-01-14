#pragma once
#include "XHLiveManager.h"
#include "ISoundCallback.h"
#include "CLiveProgram.h"
#include "CSoundManager.h"
#include "CUserInformation.h"
#include "CCustomButton.h"
#include "CConfigManager.h"
#include <vector>
using namespace std;
// CAudioChatroomDlg 对话框

class CAudioUpUserInfo
{
public:
	string m_strUserId;
	CBitmap m_bitmap;
};

class CAudioChatroomDlg : public CDialogEx, public ILiveManagerListener, public IChatroomGetListListener, public ISoundCallback, public IButtonCallback
{
	DECLARE_DYNAMIC(CAudioChatroomDlg)

public:
	CAudioChatroomDlg(CUserManager* pUserManager, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAudioChatroomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AUDIO_CHATROOM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAudioChatroomListbrush();
	afx_msg void OnBnClickedButtonCreateAudioChatroom();
	afx_msg void OnNMClickListcontrolAudioChatroomList(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnRecvAudioChatroomMsg(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();

	/**
	* 查询聊天室列表回调
	*/
	virtual int chatroomQueryAllListOK(list<ChatroomInfo>& listData);
	/*
	 * 获取音频聊天室列表
	 */
	void getLiveList();
	void setConfig(CConfigManager* pConfig);
	void clearUpUser();
	void setPictureControlShow();
public:
	// start回调函数
	/**
	 * 有主播加入
	 * @param liveID 直播ID
	 * @param actorID 新加入的主播ID
	 */
	virtual void onActorJoined(string liveID, string actorID);

	/**
	 * 有主播离开
	 * @param liveID 直播ID
	 * @param actorID 离开的主播ID
	 */
	virtual void onActorLeft(string liveID, string actorID);

	/**
	 * 房主收到其他用户的连麦申请
	 * @param liveID
	 * @param applyUserID
	 */
	virtual void onApplyBroadcast(string liveID, string applyUserID);

	/**
	 * 申请连麦用户收到的回复
	 * @param liveID
	 * @param result
	 */
	virtual void onApplyResponsed(string liveID, bool bAgree);

	/**
	* 普通用户收到连麦邀请
	* @param liveID 直播ID
	* @param applyUserID 发出邀请的人的ID（主播ID）
	*/
	virtual void onInviteBroadcast(string liveID, string applyUserID);

	/**
	 * 主播收到的邀请连麦结果
	 * @param liveID 直播ID
	 * @param result 邀请结果
	 */
	virtual void onInviteResponsed(string liveID);

	/**
	 * 一些异常情况引起的出错，请在收到该回调后主动断开直播
	 * @param liveID 直播ID
	 * @param error 错误信息
	 */
	virtual void onLiveError(string liveID, string error);

	/**
	 * 聊天室成员数变化
	 * @param number
	 */
	virtual void onMembersUpdated(int number);

	/**
	 * 自己被踢出聊天室
	 */
	virtual void onSelfKicked();

	/**
	 * 自己被禁言
	 */
	virtual void onSelfMuted(int seconds);

	/**
	 * 连麦者的连麦被强制停止
	 * @param liveID 直播ID
	 */
	virtual void onCommandToStopPlay(string  liveID);

	/**
	 * 收到消息
	 * @param message
	 */
	virtual void onReceivedMessage(CIMMessage* pMessage);

	/**
	 * 收到私信消息
	 * @param message
	 */
	virtual void onReceivePrivateMessage(CIMMessage* pMessage);

	virtual int getRealtimeData(string strUserId, uint8_t* data, int len);
	virtual int getVideoRaw(string strUserId, int w, int h, uint8_t* videoData, int videoDataLen);
public:
	virtual void getLocalSoundData(char* pData, int nLength);
	virtual void querySoundData(char** pData, int* nLength);

	CLiveProgram* getLiveProgram(CString strId, CString strName, CString strCreator);

	virtual void OnLButtonDownCallback();
	virtual void OnLButtonUpCallback();
public:
	CListCtrl m_List;
	CUserManager* m_pUserManager;
	CConfigManager* m_pConfig;
	XHLiveManager* m_pLiveManager;
	CSoundManager* m_pSoundManager;
	vector<CLiveProgram> mVLivePrograms;
	CLiveProgram* m_pCurrentLive;
	vector<HBITMAP> m_ResBitmaps;
	vector<CUserInformation> m_UserInformation;
	CStatic m_Picture1;
	CStatic m_Picture2;
	CStatic m_Picture3;
	CStatic m_Picture4;
	CStatic m_Picture5;
	CStatic m_Picture6;
	CStatic m_Picture7;

	CStatic m_UserId1;
	CStatic m_UserId2;
	CStatic m_UserId3;
	CStatic m_UserId4;
	CStatic m_UserId5;
	CStatic m_UserId6;
	CStatic m_UserId7;

	CStatic* m_PictureControlArr[7];
	CStatic* m_UserIdArr[7];
	map<string, CAudioUpUserInfo*> m_UpUserInfos;
	CCustomButton m_AudioButton;
	CListBox m_MsgList;
	CEdit m_SendMsg;
	
	afx_msg void OnBnClickedButtonAudioChatroomSendMsg();
	afx_msg void OnPaint();
	
	afx_msg void OnBnClickedButtonAudioChatroomApplyUpload();
	afx_msg void OnBnClickedButtonAudioChatroomExitUpload();
	CButton m_ApplyUploadButton;
	CButton m_ExitUpload;
	HANDLE m_hSetShowPicThread;
	HANDLE m_hSetShowPicEvent;
	bool m_bExit;
	CStatic m_AudioCharroomName;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
