#pragma once

#include "resource.h"

// CWinApp을 상속받는 CTetrisApp 클래스
class CTetrisApp : public CWinApp
{
public:
    CTetrisApp();
    ~CTetrisApp();

public:
	// 인스턴스를 초기화하는 가상함수
    virtual BOOL InitInstance();

	/* 사운드 재생을 위한 상황별 객체 생성 */
	
    MCIDEVICEID m_se_apprID;    // 게임 실행
    MCIDEVICEID m_se_turnID;	// block 회전
	MCIDEVICEID m_se_dsprID;    // 줄 제거
    MCIDEVICEID m_se_dsblID;	// 회전불가
    MCIDEVICEID m_se_crsrID;	// 버튼 마우스 오버
    MCIDEVICEID m_se_slctID;	// 버튼 클릭
	MCIDEVICEID m_me_gmvrID;    // 게임 오버
};

extern CTetrisApp theApp;