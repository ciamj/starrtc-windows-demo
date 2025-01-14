// CAudioChatroomDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "CAudioChatroomDlg.h"
#include "afxdialogex.h"
#include "CCreateLiveDialog.h"
#include "CInterfaceUrls.h"
// CAudioChatroomDlg 对话框
#define WM_RECV_AUDIO_CHATROOM_MSG WM_USER + 2

DWORD WINAPI setPicControlShow(LPVOID p)
{
	CAudioChatroomDlg* pAudioChatroomDlg = (CAudioChatroomDlg*)p;

	while (WaitForSingleObject(pAudioChatroomDlg->m_hSetShowPicEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (pAudioChatroomDlg->m_bExit)
		{
			break;
		}
		pAudioChatroomDlg->setPictureControlShow();
	}
	return 0;
}

IMPLEMENT_DYNAMIC(CAudioChatroomDlg, CDialogEx)

CAudioChatroomDlg::CAudioChatroomDlg(CUserManager* pUserManager, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AUDIO_CHATROOM, pParent)
{
	m_pUserManager = pUserManager;
	XHLiveManager::addChatroomGetListListener(this);
	m_pLiveManager = new XHLiveManager(this);
	m_pSoundManager = new CSoundManager(this);
	m_pLiveManager->setRtcMediaType(LIVE_MEDIA_TYPE_AUDIO_ONLY);
	m_pCurrentLive = NULL;
	m_hSetShowPicThread = NULL;
	m_hSetShowPicEvent = NULL;
	m_bExit = false;
	m_pConfig = NULL;
}

CAudioChatroomDlg::~CAudioChatroomDlg()
{
	m_bExit = true;
	m_pConfig = NULL;
	KillTimer(0);
	if (m_pSoundManager != NULL)
	{
		delete m_pSoundManager;
		m_pSoundManager = NULL;
	}

	if (m_pLiveManager != NULL)
	{
		delete m_pLiveManager;
		m_pLiveManager = NULL;
	}
	if (m_pCurrentLive != NULL)
	{
		delete m_pCurrentLive;
		m_pCurrentLive = NULL;
	}
	if (m_hSetShowPicEvent != NULL)
	{
		SetEvent(m_hSetShowPicEvent);
		m_hSetShowPicEvent = NULL;
	}

	mVLivePrograms.clear();
}

void CAudioChatroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTCONTROL_AUDIO_CHATROOM_LIST, m_List);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_PIC1, m_Picture1);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_PIC2, m_Picture2);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_PIC3, m_Picture3);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_PIC4, m_Picture4);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_PIC5, m_Picture5);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_PIC6, m_Picture6);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_PIC7, m_Picture7);
	DDX_Control(pDX, IDC_BUTTON_SEND_AUDIO, m_AudioButton);
	DDX_Control(pDX, IDC_LIST_AUDIO_CHATROOM_HISTORY_MSG, m_MsgList);
	DDX_Control(pDX, IDC_EDIT_AUDIO_CHATROOM_SEND_MSG, m_SendMsg);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_USERID1, m_UserId1);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_USERID2, m_UserId2);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_USERID3, m_UserId3);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_USERID4, m_UserId4);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_USERID5, m_UserId5);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_USERID6, m_UserId6);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_USERID7, m_UserId7);
	DDX_Control(pDX, IDC_BUTTON_AUDIO_CHATROOM_APPLY_UPLOAD, m_ApplyUploadButton);
	DDX_Control(pDX, IDC_BUTTON_AUDIO_CHATROOM_EXIT_UPLOAD, m_ExitUpload);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHATROOM_NAME, m_AudioCharroomName);
}


BEGIN_MESSAGE_MAP(CAudioChatroomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_CHATROOM_LISTBRUSH, &CAudioChatroomDlg::OnBnClickedButtonAudioChatroomListbrush)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_AUDIO_CHATROOM, &CAudioChatroomDlg::OnBnClickedButtonCreateAudioChatroom)
	ON_NOTIFY(NM_CLICK, IDC_LISTCONTROL_AUDIO_CHATROOM_LIST, &CAudioChatroomDlg::OnNMClickListcontrolAudioChatroomList)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_CHATROOM_SEND_MSG, &CAudioChatroomDlg::OnBnClickedButtonAudioChatroomSendMsg)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_MESSAGE(WM_RECV_AUDIO_CHATROOM_MSG, OnRecvAudioChatroomMsg)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_CHATROOM_APPLY_UPLOAD, &CAudioChatroomDlg::OnBnClickedButtonAudioChatroomApplyUpload)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_CHATROOM_EXIT_UPLOAD, &CAudioChatroomDlg::OnBnClickedButtonAudioChatroomExitUpload)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAudioChatroomDlg 消息处理程序


void CAudioChatroomDlg::OnBnClickedButtonAudioChatroomListbrush()
{
	getLiveList();
}


void CAudioChatroomDlg::OnBnClickedButtonCreateAudioChatroom()
{
	m_MsgList.ResetContent();
	m_SendMsg.SetSel(0, -1); // 选中所有字符
	m_SendMsg.ReplaceSel(_T(""));
	m_AudioCharroomName.SetWindowText("");
	m_ApplyUploadButton.EnableWindow(FALSE);
	m_ExitUpload.EnableWindow(FALSE);
	KillTimer(0);
	clearUpUser();

	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}
	if (m_pCurrentLive != NULL)
	{
		delete m_pCurrentLive;
		m_pCurrentLive = NULL;
	}

	CString strName = "";
	bool bPublic = false;

	XH_CHATROOM_TYPE chatRoomType = XH_CHATROOM_TYPE::XH_CHATROOM_TYPE_GLOBAL_PUBLIC;
	XH_LIVE_TYPE channelType = XH_LIVE_TYPE::XH_LIVE_TYPE_GLOBAL_PUBLIC;

	CCreateLiveDialog dlg(m_pUserManager);
	if (dlg.DoModal() == IDOK)
	{
		strName = dlg.m_strLiveName;
		bPublic = dlg.m_bPublic;
	}
	else
	{
		return;
	}
	if (m_pLiveManager != NULL)
	{
		string strLiveId = m_pLiveManager->createLive(strName.GetBuffer(0), chatRoomType, channelType);
		if (strLiveId != "")
		{
			string strInfo = "{\"id\":\"";
			strInfo += strLiveId;
			strInfo += "\",\"creator\":\"";
			strInfo += m_pUserManager->m_ServiceParam.m_strUserId;
			strInfo += "\",\"name\":\"";
			strInfo += strName;
			strInfo += "\"}";
			if (true)
			{
				CInterfaceUrls::demoSaveToList(m_pUserManager->m_ServiceParam.m_strUserId, CHATROOM_LIST_TYPE_AUDIO_LIVE, strLiveId, strInfo);
			}
			else
			{
				m_pLiveManager->saveToList(m_pUserManager->m_ServiceParam.m_strUserId, CHATROOM_LIST_TYPE_AUDIO_LIVE, strLiveId, strInfo);
			}

			bool bRet = m_pLiveManager->startLive(strLiveId);
			if (bRet)
			{
				if (m_pCurrentLive == NULL)
				{
					m_pCurrentLive = new CLiveProgram();
				}
				m_pCurrentLive->m_strId = strLiveId.c_str();
				m_pCurrentLive->m_strName = strName;
				m_pCurrentLive->m_strCreator = m_pUserManager->m_ServiceParam.m_strUserId.c_str();
				if (m_pSoundManager != NULL)
				{
					m_pSoundManager->startSoundData(false);
				}
				m_AudioCharroomName.SetWindowText(m_pCurrentLive->m_strName);
				onActorJoined(m_pCurrentLive->m_strId.GetBuffer(0), m_pUserManager->m_ServiceParam.m_strUserId);
				SetTimer(0, 3000, 0);
			}
			getLiveList();
		}
		else
		{
			AfxMessageBox("创建直播失败!");
		}
	}
}


void CAudioChatroomDlg::OnNMClickListcontrolAudioChatroomList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	KillTimer(0);
	m_MsgList.ResetContent();
	m_SendMsg.SetSel(0, -1); // 选中所有字符
	m_SendMsg.ReplaceSel(_T(""));
	m_AudioCharroomName.SetWindowText("");

	m_ApplyUploadButton.EnableWindow(FALSE);
	m_ExitUpload.EnableWindow(FALSE);
	
	clearUpUser();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}
	if (m_pCurrentLive != NULL)
	{
		delete m_pCurrentLive;
		m_pCurrentLive = NULL;
	}

	int nItem = -1;
	if (pNMItemActivate != NULL)
	{
		nItem = pNMItemActivate->iItem;
		if (nItem >= 0)
		{
			CString strId = m_List.GetItemText(nItem, LIVE_VIDEO_ID);
			CString strName = m_List.GetItemText(nItem, LIVE_VIDEO_NAME);
			CString strCreater = m_List.GetItemText(nItem, LIVE_VIDEO_CREATER);

			CLiveProgram* pLiveProgram = getLiveProgram(strId, strName, strCreater);
			CString strUserId = m_pUserManager->m_ServiceParam.m_strUserId.c_str();
			if (pLiveProgram != NULL)
			{
				//if (/*pLiveProgram->m_liveState == false && */pLiveProgram->m_strCreator != strUserId)
				//{
				//	AfxMessageBox("直播尚未开始");
				//	return;
				//}
				string strId = pLiveProgram->m_strId.GetBuffer(0);
				if (strId.length() == 32)
				{
					if (pLiveProgram->m_strCreator == strUserId)
					{
						bool bRet = m_pLiveManager->startLive(strId);
						if (bRet)
						{
							if (m_pCurrentLive == NULL)
							{
								m_pCurrentLive = new CLiveProgram();
							}
							m_pCurrentLive->m_strId = pLiveProgram->m_strId;
							m_pCurrentLive->m_strName = pLiveProgram->m_strName;
							m_pCurrentLive->m_strCreator = pLiveProgram->m_strCreator;
							m_AudioCharroomName.SetWindowText(m_pCurrentLive->m_strName);
							if (m_pSoundManager != NULL)
							{
								m_pSoundManager->startSoundData(false);
							}
							onActorJoined(m_pCurrentLive->m_strId.GetBuffer(0), m_pUserManager->m_ServiceParam.m_strUserId);
							SetTimer(0, 3000, 0);
						}
						else
						{
							AfxMessageBox("开启直播失败");
						}
					}
					else
					{
						bool bRet = m_pLiveManager->watchLive(strId);
						if (bRet)
						{
							m_ApplyUploadButton.EnableWindow(TRUE);
							if (m_pCurrentLive == NULL)
							{
								m_pCurrentLive = new CLiveProgram();
							}
							m_pCurrentLive->m_strId = pLiveProgram->m_strId;
							m_pCurrentLive->m_strName = pLiveProgram->m_strName;
							m_pCurrentLive->m_strCreator = pLiveProgram->m_strCreator;
							m_AudioCharroomName.SetWindowText(m_pCurrentLive->m_strName);
							if (m_pSoundManager != NULL)
							{
								m_pSoundManager->startSoundData(false);
							}
							SetTimer(0, 3000, 0);
						}
						else
						{
							AfxMessageBox("观看直播失败");
						}
					}
				}
				else
				{
					CString strMessage = "";
					strMessage.Format("err id %s", strId.c_str());
					AfxMessageBox(strMessage);
				}
				delete pLiveProgram;
				pLiveProgram = NULL;
			}

		}
	}

}

/**
 * 查询聊天室列表回调
 */
int CAudioChatroomDlg::chatroomQueryAllListOK(list<ChatroomInfo>& listData)
{
	mVLivePrograms.clear();
	m_List.DeleteAllItems();

	list<ChatroomInfo>::iterator iter = listData.begin();
	int i = 0;
	for (; iter != listData.end(); iter++)
	{
		CLiveProgram liveProgram;
		liveProgram.m_strName = (char*)(*iter).m_strName.c_str();
		liveProgram.m_strId = (char*)(*iter).m_strRoomId.c_str();
		liveProgram.m_strCreator = (char*)(*iter).m_strCreaterId.c_str();
		mVLivePrograms.push_back(liveProgram);
	}
	int nRowIndex = 0;
	CString strStatus = "";
	for (int i = 0; i < (int)mVLivePrograms.size(); i++)
	{
		m_List.InsertItem(i, mVLivePrograms[i].m_strName);
		//m_liveList.AddString(mVLivePrograms[i].m_strName);
		m_List.SetItemText(i, LIVE_VIDEO_ID, mVLivePrograms[i].m_strId);

		m_List.SetItemText(i, LIVE_VIDEO_CREATER, mVLivePrograms[i].m_strCreator);
		if (mVLivePrograms[i].m_liveState)
		{
			strStatus = "正在直播";
		}
		else
		{
			strStatus = "直播未开始";
		}
		m_List.SetItemText(i, LIVE_VIDEO_STATUS, strStatus);
	}
	return 0;
}
/*
 * 获取音频聊天室列表
 */
void CAudioChatroomDlg::getLiveList()
{
	char strListType[10] = { 0 };
	sprintf_s(strListType, "%d,%d", CHATROOM_LIST_TYPE_AUDIO_LIVE, CHATROOM_LIST_TYPE_AUDIO_LIVE_PUSH);
	if (m_pConfig != NULL && m_pConfig->m_bAEventCenterEnable)
	{
		list<ChatroomInfo> listData;
		CInterfaceUrls::demoQueryList(strListType, listData);
		chatroomQueryAllListOK(listData);
	}
	else
	{
		XHLiveManager::getLiveList("", strListType);
	}
	
	//if (m_pUserManager->m_bUserDispatch)
	{
		//list<ChatroomInfo> listData;
		//CInterfaceUrls::demoRequestAudioLiveList(listData, m_pUserManager);
		//channelQueryAllListOK(listData);
	}
	//else
	//{
	//	XHLiveManager::getLiveList("", CHATROOM_LIST_TYPE_LIVE);
	//}
	
}
void CAudioChatroomDlg::setConfig(CConfigManager* pConfig)
{
	m_pConfig = pConfig;
}

BOOL CAudioChatroomDlg::OnInitDialog()
{
	__super::OnInitDialog();
	m_AudioButton.setCallback(this);
	LONG lStyle;
	lStyle = GetWindowLong(m_List.m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= LVS_REPORT;
	SetWindowLong(m_List.m_hWnd, GWL_STYLE, lStyle);

	DWORD dwStyleLiveList = m_List.GetExtendedStyle();
	dwStyleLiveList |= LVS_EX_FULLROWSELECT;                                        //选中某行使整行高亮(LVS_REPORT)
	dwStyleLiveList |= LVS_EX_GRIDLINES;                                            //网格线(LVS_REPORT)
	//dwStyle |= LVS_EX_CHECKBOXES;                                            //CheckBox
	m_List.SetExtendedStyle(dwStyleLiveList);

	m_List.InsertColumn(LIVE_VIDEO_ID, _T("ID"), LVCFMT_LEFT, 110);
	m_List.InsertColumn(LIVE_VIDEO_NAME, _T("Name"), LVCFMT_LEFT, 120);
	m_List.InsertColumn(LIVE_VIDEO_CREATER, _T("Creator"), LVCFMT_LEFT, 80);
	m_List.InsertColumn(LIVE_VIDEO_STATUS, _T("liveState"), LVCFMT_LEFT, 100);

	m_PictureControlArr[0] = &m_Picture1;
	m_PictureControlArr[1] = &m_Picture2;
	m_PictureControlArr[2] = &m_Picture3;
	m_PictureControlArr[3] = &m_Picture4;
	m_PictureControlArr[4] = &m_Picture5;
	m_PictureControlArr[5] = &m_Picture6;
	m_PictureControlArr[6] = &m_Picture7;

	m_UserIdArr[0] = &m_UserId1;
	m_UserIdArr[1] = &m_UserId2;
	m_UserIdArr[2] = &m_UserId3;
	m_UserIdArr[3] = &m_UserId4;
	m_UserIdArr[4] = &m_UserId5;
	m_UserIdArr[5] = &m_UserId6;
	m_UserIdArr[6] = &m_UserId7;
	getLiveList();

	m_ApplyUploadButton.EnableWindow(FALSE);
	m_ExitUpload.EnableWindow(FALSE);
	m_hSetShowPicEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSetShowPicThread = CreateThread(NULL, 0, setPicControlShow, (void*)this, 0, 0); // 创建线程


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CAudioChatroomDlg::clearUpUser()
{
	map<string, CAudioUpUserInfo*>::iterator iter = m_UpUserInfos.begin();
	for (;iter != m_UpUserInfos.end(); ++iter)
	{
		CAudioUpUserInfo* pUserInfo = iter->second;
		delete pUserInfo;
		pUserInfo = NULL;
	}
	m_UpUserInfos.clear();
	setPictureControlShow();
}
void CAudioChatroomDlg::setPictureControlShow()
{
	int nSize = (int)m_UpUserInfos.size();
	int nIndex = 0;
	string strCreator = "";
	if (m_pCurrentLive != NULL)
	{
		strCreator = m_pCurrentLive->m_strCreator.GetBuffer(0);
	}
	bool bFindCreator;
	map<string, CAudioUpUserInfo*>::iterator iter = m_UpUserInfos.begin();
	for (; iter != m_UpUserInfos.end(); ++iter)
	{
		string strUserId = iter->first;	
		if (strUserId == strCreator)
		{
			bFindCreator = true;
			m_UserIdArr[0]->SetWindowText(strUserId.c_str());
			m_PictureControlArr[0]->SetBitmap((HBITMAP)iter->second->m_bitmap.GetSafeHandle());
		}
		else
		{
			if (bFindCreator)
			{
				m_UserIdArr[nIndex]->SetWindowText(strUserId.c_str());
				m_PictureControlArr[nIndex]->SetBitmap((HBITMAP)iter->second->m_bitmap.GetSafeHandle());

			}
			else
			{
				m_UserIdArr[nIndex + 1]->SetWindowText(strUserId.c_str());
				m_PictureControlArr[nIndex + 1]->SetBitmap((HBITMAP)iter->second->m_bitmap.GetSafeHandle());

			}
					
		}
		nIndex++;
	}
	//CBitmap bitmap;
	//bitmap.LoadBitmap(IDB_BITMAP2);  // 将位图IDB_BITMAP1加载到bitmap  
	//for (; nIndex < 7; nIndex++)
	//{
	//	m_UserIdArr[nIndex]->SetWindowText("");
	//	m_PictureControlArr[nIndex]->SetBitmap((HBITMAP)bitmap.GetSafeHandle());
	//}
}
/**
 * 有主播加入
 * @param liveID 直播ID
 * @param actorID 新加入的主播ID
 */
void CAudioChatroomDlg::onActorJoined(string liveID, string actorID)
{
	CAudioUpUserInfo* pUserInfo = new CAudioUpUserInfo();
	pUserInfo->m_strUserId = actorID;
	//pUserInfo->m_bitmap.LoadBitmap(IDB_BITMAP1);
	m_UpUserInfos[actorID] = pUserInfo;
	SetEvent(m_hSetShowPicEvent);
}
/**
 * 有主播离开
 * @param liveID 直播ID
 * @param actorID 离开的主播ID
 */
void CAudioChatroomDlg::onActorLeft(string liveID, string actorID)
{
	map<string, CAudioUpUserInfo*>::iterator iter = m_UpUserInfos.find(actorID);
	if (iter != m_UpUserInfos.end())
	{
		CAudioUpUserInfo* pUserInfo = iter->second;
		delete pUserInfo;
		pUserInfo = NULL;
		m_UpUserInfos.erase(iter);
	}
	SetEvent(m_hSetShowPicEvent);
}

/**
 * 房主收到其他用户的连麦申请
 * @param liveID
 * @param applyUserID
 */
void CAudioChatroomDlg::onApplyBroadcast(string liveID, string applyUserID)
{
	CString strMsg;
	strMsg.Format("是否同意用户:%s申请连麦", applyUserID.c_str());
	if (m_pLiveManager != NULL)
	{
		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
		{
			m_pLiveManager->agreeApplyToBroadcaster(applyUserID);
			onActorJoined(liveID, applyUserID);
			CString strMsg = "欢迎新的小伙伴上麦！！！";
			if (m_pLiveManager != NULL && m_pCurrentLive != NULL)
			{
				CIMMessage* pIMMessage = m_pLiveManager->sendMessage(strMsg.GetBuffer(0));
				CString strMsg = "";
				strMsg.Format("%s:%s", pIMMessage->m_strFromId.c_str(), pIMMessage->m_strContentData.c_str());
				m_MsgList.InsertString(m_MsgList.GetCount(), strMsg);
				delete pIMMessage;
				pIMMessage = NULL;
				m_SendMsg.SetSel(0, -1); // 选中所有字符
				m_SendMsg.ReplaceSel(_T(""));
			}
		}
		else
		{
			m_pLiveManager->refuseApplyToBroadcaster(applyUserID);
		}
	}
}

LRESULT CAudioChatroomDlg::OnRecvAudioChatroomMsg(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0:
		break;
	case 1:
	{
		if (IDYES == AfxMessageBox("是否上传数据", MB_YESNO))
		{
			KillTimer(0);
			m_ApplyUploadButton.EnableWindow(FALSE);
			m_ExitUpload.EnableWindow(FALSE);

			SetEvent(m_hSetShowPicEvent);

			if (m_pSoundManager != NULL)
			{
				m_pSoundManager->stopSoundData();
			}

			bool bRet = m_pLiveManager->startLive(m_pCurrentLive->m_strId.GetBuffer(0));
			if (bRet)
			{
				if (m_pSoundManager != NULL)
				{
					m_pSoundManager->startSoundData(false);
				}
				onActorJoined(m_pCurrentLive->m_strId.GetBuffer(0), m_pUserManager->m_ServiceParam.m_strUserId);
				SetTimer(0, 3000, 0);

				m_ApplyUploadButton.EnableWindow(FALSE);
				m_ExitUpload.EnableWindow(TRUE);
			}
			else
			{
				AfxMessageBox("join audio chatroom failed");
			}
		}
	}
	break;
	case 2:
		break;
	}
	return 0;
}

/**
 * 申请连麦用户收到的回复
 * @param liveID
 * @param result
 */
void CAudioChatroomDlg::onApplyResponsed(string liveID, bool bAgree)
{
	if (bAgree)
	{
		if (bAgree)
		{
			PostMessage(WM_RECV_AUDIO_CHATROOM_MSG, 1, 0);
		}		
	}
	else
	{
		AfxMessageBox("对方拒绝连麦请求");
	}
}

/**
* 普通用户收到连麦邀请
* @param liveID 直播ID
* @param applyUserID 发出邀请的人的ID（主播ID）
*/
void CAudioChatroomDlg::onInviteBroadcast(string liveID, string applyUserID)
{
}

/**
* 主播收到的邀请连麦结果
* @param liveID 直播ID
* @param result 邀请结果
*/
void CAudioChatroomDlg::onInviteResponsed(string liveID)
{
}

/**
 * 一些异常情况引起的出错，请在收到该回调后主动断开直播
 * @param liveID 直播ID
 * @param error 错误信息
 */
void CAudioChatroomDlg::onLiveError(string liveID, string error)
{
	KillTimer(0);
	m_MsgList.ResetContent();
	m_SendMsg.SetSel(0, -1); // 选中所有字符
	m_SendMsg.ReplaceSel(_T(""));
	m_AudioCharroomName.SetWindowText("");

	m_ApplyUploadButton.EnableWindow(FALSE);
	m_ExitUpload.EnableWindow(FALSE);

	SetEvent(m_hSetShowPicEvent);
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}
	if (m_pCurrentLive != NULL)
	{
		delete m_pCurrentLive;
		m_pCurrentLive = NULL;
	}
}

/**
 * 聊天室成员数变化
 * @param number
 */
void CAudioChatroomDlg::onMembersUpdated(int number)
{
	CString strVal = "";
	if (m_pCurrentLive != NULL)
	{
		strVal.Format("%s(%d)", m_pCurrentLive->m_strName, number);
	}
	m_AudioCharroomName.SetWindowText(strVal);
}

/**
 * 自己被踢出聊天室
 */
void CAudioChatroomDlg::onSelfKicked()
{
	KillTimer(0);
}

/**
 * 自己被禁言
 */
void CAudioChatroomDlg::onSelfMuted(int seconds)
{
	CString strMsg = "";
	strMsg.Format("被禁言 %d 秒", seconds);
	AfxMessageBox(strMsg);
}

/**
* 连麦者的连麦被强制停止
* @param liveID 直播ID
*/
void CAudioChatroomDlg::onCommandToStopPlay(string  liveID)
{
	KillTimer(0);
	m_ApplyUploadButton.EnableWindow(FALSE);
	m_ExitUpload.EnableWindow(FALSE);

	clearUpUser();

	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}
	if (m_pLiveManager != NULL)
	{
		m_pLiveManager->leaveLive();
	}
	if (m_pCurrentLive != NULL)
	{
		delete m_pCurrentLive;
		m_pCurrentLive = NULL;
	}
}
/**
 * 收到消息
 * @param message
 */
void CAudioChatroomDlg::onReceivedMessage(CIMMessage* pMessage)
{
	CString strMsg = "";
	strMsg.Format("%s:%s", pMessage->m_strFromId.c_str(), pMessage->m_strContentData.c_str());

	m_MsgList.InsertString(m_MsgList.GetCount(), strMsg);
}

/**
 * 收到私信消息
 * @param message
 */
void CAudioChatroomDlg::onReceivePrivateMessage(CIMMessage* pMessage)
{
	CString strMsg = "";
	strMsg.Format("%s:%s", pMessage->m_strFromId.c_str(), pMessage->m_strContentData.c_str());
	m_MsgList.AddString(strMsg);
}


int CAudioChatroomDlg::getRealtimeData(string strUserId, uint8_t* data, int len)
{
	return 0;
}
int CAudioChatroomDlg::getVideoRaw(string strUserId, int w, int h, uint8_t* videoData, int videoDataLen)
{
	return 0;
}
void CAudioChatroomDlg::getLocalSoundData(char* pData, int nLength)
{
	if (m_pLiveManager != NULL)
	{
		m_pLiveManager->insertAudioRaw((uint8_t*)pData, nLength);
	}
}

void CAudioChatroomDlg::querySoundData(char** pData, int* nLength)
{
	if (m_pLiveManager != NULL)
	{
		m_pLiveManager->querySoundData((uint8_t**)pData, nLength);
	}
}

CLiveProgram* CAudioChatroomDlg::getLiveProgram(CString strId, CString strName, CString strCreator)
{
	CLiveProgram* pRet = NULL;
	for (int i = 0; i < (int)mVLivePrograms.size(); i++)
	{
		if (//mVLivePrograms[i].m_strId == strId &&
			mVLivePrograms[i].m_strName == strName)// &&
			//mVLivePrograms[i].m_strCreator == strCreator)
		{
			pRet = new CLiveProgram();
			pRet->m_strId = mVLivePrograms[i].m_strId;
			pRet->m_strName = mVLivePrograms[i].m_strName;
			pRet->m_strCreator = mVLivePrograms[i].m_strCreator;
			pRet->m_liveState = mVLivePrograms[i].m_liveState;
			break;
		}
	}
	return pRet;
}

void CAudioChatroomDlg::OnLButtonDownCallback()
{
	if (m_pSoundManager != NULL && m_pCurrentLive != NULL)
	{
		m_pSoundManager->startGetSoundData();
	}
}

void CAudioChatroomDlg::OnLButtonUpCallback()
{
	if (m_pSoundManager != NULL && m_pCurrentLive != NULL)
	{
		m_pSoundManager->stopGetSoundData();
	}
}
void CAudioChatroomDlg::OnBnClickedButtonAudioChatroomSendMsg()
{
	CString strMsg = "";
	m_SendMsg.GetWindowText(strMsg);

	if (strMsg == "")
	{
		return;
	}
	if (m_pLiveManager != NULL && m_pCurrentLive != NULL)
	{
		CIMMessage* pIMMessage = m_pLiveManager->sendMessage(strMsg.GetBuffer(0));
		CString strMsg = "";
		strMsg.Format("%s:%s", pIMMessage->m_strFromId.c_str(), pIMMessage->m_strContentData.c_str());
		m_MsgList.InsertString(m_MsgList.GetCount(), strMsg);
		delete pIMMessage;
		pIMMessage = NULL;
		m_SendMsg.SetSel(0, -1); // 选中所有字符
		m_SendMsg.ReplaceSel(_T(""));
	}
}


void CAudioChatroomDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 __super::OnPaint()
	//CBitmap bitmap;
	// CBitmap对象，用于加载位图   
	//HBITMAP hBmp;    // 保存CBitmap加载的位图的句柄   

	//bitmap.LoadBitmap(IDB_BITMAP3);  // 将位图IDB_BITMAP1加载到bitmap  
	//hBmp = (HBITMAP)bitmap.GetSafeHandle();  // 获取bitmap加载位图的句柄  

	//CBitmap bitmap2;  // CBitmap对象，用于加载位图   
	//HBITMAP hBmp2;    // 保存CBitmap加载的位图的句柄   

	//bitmap2.LoadBitmap(IDB_BITMAP2);  // 将位图IDB_BITMAP1加载到bitmap  
	//hBmp2 = (HBITMAP)bitmap2.GetSafeHandle();  // 获取bitmap加载位图的句柄 

	//m_Picture1.SetBitmap(hBmp);
	//m_Picture2.SetBitmap(hBmp2);
	//m_Picture3.SetBitmap(hBmp2);
	//m_Picture4.SetBitmap(hBmp2);
	//m_Picture5.SetBitmap(hBmp2);
	//m_Picture6.SetBitmap(hBmp2);
	//m_Picture7.SetBitmap(hBmp2);
}


void CAudioChatroomDlg::OnBnClickedButtonAudioChatroomApplyUpload()
{
	m_pLiveManager->applyToBroadcaster(m_pCurrentLive->m_strCreator.GetBuffer(0));
}


void CAudioChatroomDlg::OnBnClickedButtonAudioChatroomExitUpload()
{
	m_ApplyUploadButton.EnableWindow(FALSE);
	m_ExitUpload.EnableWindow(FALSE);
	clearUpUser();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopSoundData();
	}

	bool bRet = m_pLiveManager->watchLive(m_pCurrentLive->m_strId.GetBuffer(0));
	if (bRet)
	{
		m_ApplyUploadButton.EnableWindow(TRUE);
		if (m_pSoundManager != NULL)
		{
			m_pSoundManager->startSoundData(false);
		}
	}
	else
	{
		AfxMessageBox("join audio chatroom failed");
	}
}


void CAudioChatroomDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_pLiveManager != NULL && m_pCurrentLive != NULL)
	{
		m_pLiveManager->getOnlineNumber(m_pCurrentLive->m_strId.GetBuffer(0));
	}
	__super::OnTimer(nIDEvent);
}
