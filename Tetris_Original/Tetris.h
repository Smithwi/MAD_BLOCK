#pragma once

#include "resource.h"

// CWinApp�� ��ӹ޴� CTetrisApp Ŭ����
class CTetrisApp : public CWinApp
{
public:
    CTetrisApp();
    ~CTetrisApp();

public:
	// �ν��Ͻ��� �ʱ�ȭ�ϴ� �����Լ�
    virtual BOOL InitInstance();

	/* ���� ����� ���� ��Ȳ�� ��ü ���� */
	
    MCIDEVICEID m_se_apprID;    // ���� ����
    MCIDEVICEID m_se_turnID;	// block ȸ��
	MCIDEVICEID m_se_dsprID;    // �� ����
    MCIDEVICEID m_se_dsblID;	// ȸ���Ұ�
    MCIDEVICEID m_se_crsrID;	// ��ư ���콺 ����
    MCIDEVICEID m_se_slctID;	// ��ư Ŭ��
	MCIDEVICEID m_me_gmvrID;    // ���� ����
};

extern CTetrisApp theApp;