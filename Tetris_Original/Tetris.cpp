#include "stdafx.h"
#include "Tetris.h"
#include "TetrisDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTetrisApp ������ ȣ��
CTetrisApp::CTetrisApp()
{
    MCI_OPEN_PARMS openParms;
    memset(&openParms, 0, sizeof(openParms));

	// m_se_apprID �϶� 001-System01.wav ���
    openParms.lpstrDeviceType = _T("waveaudio");
    openParms.lpstrElementName = _T("res\\001-System01.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_apprID = openParms.wDeviceID;

	// m_se_turnID �϶� 003-System03.wav ���
    openParms.lpstrElementName = _T("res\\003-System03.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_turnID = openParms.wDeviceID;

	// m_se_dsblID �϶� 004-System04.wav ���
    openParms.lpstrElementName = _T("res\\004-System04.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_dsblID = openParms.wDeviceID;

	// m_se_dsprID �϶� 012-System12.wav ���
    openParms.lpstrElementName = _T("res\\012-System12.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_dsprID = openParms.wDeviceID;

	// m_se_crsrID �϶� 032-Switch01.wav ���
    openParms.lpstrElementName = _T("res\\032-Switch01.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_crsrID = openParms.wDeviceID;

	// m_se_slctID �϶� 033-Switch02.wav ���
    openParms.lpstrElementName = _T("res\\033-Switch02.wav");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_se_slctID = openParms.wDeviceID;

	// m_se_gmvrID �϶� 006-Defeat02.mid ���
    openParms.lpstrElementName = _T("res\\006-Defeat02.mid");
    openParms.lpstrDeviceType = _T("sequencer");
    ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPMCI_OPEN_PARMS)&openParms);
    m_me_gmvrID = openParms.wDeviceID;
}

// CTetrisApp �Ҹ��� ȣ��
CTetrisApp::~CTetrisApp()
{
    ::mciSendString(_T("close all"), NULL, 0, 0);
}

// the App�̶�� ��ü ����
CTetrisApp theApp;

// �ν��Ͻ� �ʱ�ȭ
/* �������α׷��� ����(������ ũ��, �ɼ�, ��Ÿ�� ��)�̳�, ������뿩�� */
/* Ȥ�� ����� ����, �ߺ�������� ��� ���õ� �ڵ尡 ����.         */
BOOL CTetrisApp::InitInstance()
{
	CWinApp::InitInstance();

	// ��ȭ���� ��ü ����
	CTetrisDlg dlg;
	m_pMainWnd = &dlg;
	
	// ��ȭ���� ���
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}