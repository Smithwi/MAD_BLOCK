#include "stdafx.h"
#include "Tetris.h"
#include "TetrisDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")
// CTetrisApp 생성자 호출
CTetrisApp::CTetrisApp()
{
    MCI_OPEN_PARMS openParms;
    memset(&openParms, 0, sizeof(openParms));

	// m_se_apprID 일때 001-System01.wav 재생
    openParms.lpstrDeviceType = _T("waveaudio");
    openParms.lpstrElementName = _T("res\\001-System01.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_apprID = openParms.wDeviceID;

	// m_se_turnID 일때 003-System03.wav 재생
    openParms.lpstrElementName = _T("res\\003-System03.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_turnID = openParms.wDeviceID;

	// m_se_dsblID 일때 004-System04.wav 재생
    openParms.lpstrElementName = _T("res\\004-System04.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_dsblID = openParms.wDeviceID;

	// m_se_dsprID 일때 012-System12.wav 재생
    openParms.lpstrElementName = _T("res\\012-System12.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_dsprID = openParms.wDeviceID;

	// m_se_crsrID 일때 032-Switch01.wav 재생
    openParms.lpstrElementName = _T("res\\032-Switch01.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_crsrID = openParms.wDeviceID;

	// m_se_slctID 일때 033-Switch02.wav 재생
    openParms.lpstrElementName = _T("res\\033-Switch02.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_slctID = openParms.wDeviceID;

	//경고음 재생
	openParms.lpstrElementName = _T("res\\034-Warnning.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_w_ID = openParms.wDeviceID;

	// m_se_gmvrID 일때 006-Defeat02.mid 재생
    openParms.lpstrElementName = _T("res\\006-Defeat02.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_me_gmvrID = openParms.wDeviceID;


	
}

// CTetrisApp 소멸자 호출
CTetrisApp::~CTetrisApp()
{
    ::mciSendString(_T("close all"), NULL, 0, 0);
}

// the App이라는 객체 생성
CTetrisApp theApp;

// 인스턴스 초기화
/* 응용프로그램의 설정(윈도우 크기, 옵션, 스타일 등)이나, 실행허용여부 */
/* 혹은 사용자 인증, 중복실행방지 등과 관련된 코드가 들어간다.         */
BOOL CTetrisApp::InitInstance()
{
	CWinApp::InitInstance();

	// 대화상자 객체 생성
	CTetrisDlg dlg;
	m_pMainWnd = &dlg;
	
	// 대화상자 사용
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}