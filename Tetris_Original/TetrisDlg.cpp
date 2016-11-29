#include "stdafx.h"
#include "Tetris.h"
#include "TetrisDlg.h"

#ifdef _DEBUG

#define new DEBUG_NEW
#endif

#define VK_A			  0x41



/* 게임 윈도우 구현을 위한 실질적인 코드 */

//////////////////////////
/* 0x01 = NEW */
/* 0x02 = PAUSE */
/* 0x04 = STOP */
/* 0x08 = SOUND */
/* 0x10 = EXIT */
//////////////////////////

// 게임 초기상태 구현(이미지 불러오기, 레벨, 게임상태)
CTetrisDlg::CTetrisDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTetrisDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bk.Load(_T("res\\003-StarlitSky01.jpg"));
    m_block.Load(_T("res\\178-Switch01.png"));
    m_window.Load(_T("res\\window.png"));
	m_gameover.Load(_T("res\\gameover.png"));

	// 블럭이 채워질 배열(= 게임화면)
    m_board = new BYTE*[ROW];
    for(BYTE i = 0; i < ROW; ++i)
    {
        m_board[i] = new BYTE[COL];
    }

	//변수들 초기화
	m_befortime = GetTickCount();
    m_level = 4;//속도 4 고정
    m_gameParam = 0x08;
    m_pBlock = NULL;
	//이동방해, 회전방해 초기화.
	m_ninterruptCount =0;  
	m_ninterruptTarget=0;
	m_ninterrupt =0;

	//섞기 초기화
	m_bShake =false;
	m_bStart=false;
	oneShot = 1;//아이템 1개 
}

// Window 소멸자
CTetrisDlg::~CTetrisDlg()
{
    m_memBmp.DeleteObject();
    m_memDC.DeleteDC();
    delete m_pDC;
    delete m_pBlock;
    for(BYTE i = 0; i < ROW; ++i)
    {
        delete[] m_board[i];

    }
    delete[] m_board;
	
	
}

// Event 메시지 맵핑
BEGIN_MESSAGE_MAP(CTetrisDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_GETDLGCODE()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_COMMAND(ID_GAME_NEW, &CTetrisDlg::OnGameNew)
    ON_COMMAND(ID_GAME_PAUSE, &CTetrisDlg::OnGamePause)
    ON_COMMAND(ID_GAME_STOP, &CTetrisDlg::OnGameStop)
    ON_COMMAND(ID_LEVEL_BEGINNER, &CTetrisDlg::OnLevelBeginner)
    ON_COMMAND(ID_LEVEL_INTERMEDIATE, &CTetrisDlg::OnLevelIntermediate)
    ON_COMMAND(ID_LEVEL_ADVANCED, &CTetrisDlg::OnLevelAdvanced)
    ON_COMMAND(ID_LEVEL_EXPERT, &CTetrisDlg::OnLevelExpert)
    ON_COMMAND(ID_LEVEL_DEVIL, &CTetrisDlg::OnLevelDevil)
    ON_COMMAND(ID_LEVEL_HELL, &CTetrisDlg::OnLevelHell)
    ON_COMMAND(ID_GAME_SOUND, &CTetrisDlg::OnGameSound)
    ON_COMMAND(ID_GAME_EXIT, &CTetrisDlg::OnGameExit)
	ON_COMMAND(ID_HELP_ABOUT, &CTetrisDlg::OnHelpAbout)
END_MESSAGE_MAP()

// 화면 업데이트
void CTetrisDlg::Update()
{
    RedrawBkgnd(CRect(0, 0, WIDTH + 320, HEIGHT));
    UpdateBlock();
    UpdateWindow();

	// gameover일 때
    if(m_gameParam & 0x04)
    {
		// gameover 이미지 적용
        CDC gameoverMemDC;
        gameoverMemDC.CreateCompatibleDC(&m_memDC);
        gameoverMemDC.SelectObject(m_gameover);

		// 비트맵 이미지 사용시 blend option
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = AC_SRC_ALPHA;
        ::AlphaBlend(m_memDC, 0, 0, WIDTH + 320, HEIGHT, gameoverMemDC, 0, 0, WIDTH + 320, HEIGHT, bf);
    }
    m_pDC->BitBlt(0, 0, WIDTH + 320, HEIGHT, &m_memDC, 0, 0, SRCCOPY);
}

// 창 조절
void CTetrisDlg::AdjustFrame()
{
    CRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = WIDTH + 320;
    rect.bottom = HEIGHT;
    ::AdjustWindowRectEx(rect, ::GetWindowLong(m_hWnd, GWL_STYLE), TRUE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE));
    SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
}

// 초기화
void CTetrisDlg::Initialize()
{
	// 타이머 초기화
    KillTimer(555);
    m_lines = 0;
    m_score = 0; // 원래는 점수였으나 시간으로 쓰임;
	oneShot = 1; //아이템 다시 1개
	m_befortime = GetTickCount(); //시스템 시간 가져오기
    for(BYTE i = 0; i < ROW; ++i)
    {
        memset(m_board[i], 0, COL);
    } // 배열 초기화
    delete m_pBlock;
    m_pBlock = NULL;

	// STOP, PAUSE 비활성화
    m_menu.EnableMenuItem(ID_GAME_STOP, MF_DISABLED | MF_GRAYED);
    m_menu.EnableMenuItem(ID_GAME_PAUSE, MF_DISABLED | MF_GRAYED);
    m_menu.CheckMenuItem(ID_GAME_PAUSE, MF_UNCHECKED);
}

// 배경 그리기
void CTetrisDlg::RedrawBkgnd(RECT rect)
{
    CBrush bkBr(CBitmap::FromHandle(m_bk));
    m_memDC.FillRect(&rect, &bkBr);
}


// TEXT
void CTetrisDlg::DrawText(SHORT x1, SHORT y1, SHORT x2, SHORT y2, CString &text, UINT format, COLORREF clr = 0xFFFFFF)
{
    m_memDC.SetTextColor(0x000000);
    m_memDC.DrawText(text, CRect(x1 + 2, y1 + 2, x2 + 2, y2 + 2), format | DT_NOCLIP);
    m_memDC.SetTextColor(clr);
    m_memDC.DrawText(text, CRect(x1, y1, x2, y2), format | DT_NOCLIP);
}

// Font Size
void CTetrisDlg::SetFontSize(BYTE size)
{
    CFont font;
    font.CreateFont(
        size,                       // height of font
        0,                          // average character width
        0,                          // angle of escapement
        0,                          // base-line orientation angle
        FW_NORMAL,                  // font weight
        FALSE,                      // italic attribute option
        FALSE,                      // underline attribute option
        0,                          // strikeout attribute option
        ANSI_CHARSET,               // character set identifier
        OUT_DEFAULT_PRECIS,         // output precision
        CLIP_DEFAULT_PRECIS,        // clipping precision
        DEFAULT_QUALITY,            // output quality
        DEFAULT_PITCH | FF_SWISS,   // pitch and family
        _T("Impact")                // typeface name
    );
    m_memDC.SelectObject(&font);
}

// SOUND

//효과음,배경음악 정지
void CTetrisDlg::Stop(MCIDEVICEID id)
{
    if(m_gameParam & 0x08)
    {
        MCI_PLAY_PARMS playParms;
        playParms.dwFrom = 0;
        ::mciSendCommand(id, MCI_STOP, MCI_FROM, (DWORD)(LPMCI_PLAY_PARMS)&playParms);
    }
}

//효과음,배경음악 재생
void CTetrisDlg::Play(MCIDEVICEID id)
{
    if(m_gameParam & 0x08)
    {
        MCI_PLAY_PARMS playParms;
        playParms.dwFrom = 0;
        ::mciSendCommand(id, MCI_PLAY, MCI_FROM, (DWORD)(LPMCI_PLAY_PARMS)&playParms);
    }
}

// block update
void CTetrisDlg::UpdateBlock()
{
    CDC blockMemDC;
    blockMemDC.CreateCompatibleDC(&m_memDC);
    blockMemDC.SelectObject(m_block);

	// 비트맵 이미지 사용시 blend option
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 220;
    bf.AlphaFormat = AC_SRC_ALPHA;

    for(BYTE i = 0; i < ROW; ++i)
    {
        BYTE *nextRow = m_board[i];
        for(BYTE j = 0; j < COL; ++j)
        {
            BYTE clr = nextRow[j];
            if(clr)
            {
                ::AlphaBlend(m_memDC, j << 5, i << 5, 32, 32, blockMemDC, (clr - 1) << 5, 0, 32, 32, bf);
                //m_memDC.StretchBlt(j << 5, i << 5, 32, 32, &blockMemDC, (clr - 1) << 5, 0, 32, 32, SRCCOPY);
            }
        }
    }
    blockMemDC.DeleteDC();
}

// 화면 update
void CTetrisDlg::UpdateWindow()
{
	// window 이미지 적용
    CDC wndMemDC;
    wndMemDC.CreateCompatibleDC(&m_memDC);
    wndMemDC.SelectObject(m_window);

	// 비트맵 이미지 blend option
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;
    ::AlphaBlend(m_memDC, 328, 10, 308, 629, wndMemDC, 0, 0, 308, 629, bf);
    wndMemDC.DeleteDC();

	// text 설정
    SetFontSize(30);
    CString str;
    DrawText(360, 32, 600, 64, str = "ITEM : A / DROP : SPACE", DT_LEFT);

	// block 이미지 적용
    CDC blockMemDC;
    blockMemDC.CreateCompatibleDC(&m_memDC);
    blockMemDC.SelectObject(m_block);

    BYTE x = (m_nextColor - 1) << 5;
    switch((m_gameParam & 0xE0) >> 5)
    {
    case 1: // 일직선
        m_memDC.StretchBlt(414, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(446, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(478, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(510, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        break;
    case 2: //기억자
        m_memDC.StretchBlt(430, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(462, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(494, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(494, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        break;
    case 3://L자
        m_memDC.StretchBlt(430, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(462, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(494, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(430, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        break;
    case 4: //정사각형
        m_memDC.StretchBlt(446, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(478, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(446, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(478, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        break;
    case 5://ㄹ자
        m_memDC.StretchBlt(462, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(494, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(430, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(462, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        break;
    case 6://ㅗ자
        m_memDC.StretchBlt(430, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(462, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(494, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(462, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        break;
    case 7:// ㄹ 반대편
        m_memDC.StretchBlt(430, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(462, 96, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(462, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
        m_memDC.StretchBlt(494, 128, 32, 32, &blockMemDC, x, 0, 32, 32, SRCCOPY);
    }
    blockMemDC.DeleteDC();

	if(m_bStart==false)
		m_befortime = GetTickCount();
	// 각각의 버튼 글자 설정
    DrawText(360, 192, 600, 224, str = "LEVEL:", DT_LEFT);
    str.Format(_T("%d"), m_level + 1);
    DrawText(360, 192, 600, 224, str, DT_RIGHT);
    DrawText(360, 256, 600, 288, str = "TIME:", DT_LEFT);

	//저장된 시간과 현재 시스템 시간의 차가 1마이크로 초 이상 난다면 TIME 1증가
	DWORD ple = GetTickCount()-m_befortime;
	if(ple>=1000.0f)
	{
		m_score++;
		m_befortime =GetTickCount();
	}

    str.Format(_T("%d"), m_score);
   DrawText(360, 256, 600, 288, str, DT_RIGHT);
  
	str.Format(_T("ITEM:"));
	DrawText(360, 320, 600, 352, str, DT_LEFT);

	//소지중인 아이템 갯수 그리기
	str.Format(_T("%d"), oneShot);
    DrawText(360, 320, 600, 352, str, DT_RIGHT);
    SetFontSize(m_mouseOver == 0x01 ? 36 : 30);
    DrawText(360, 384, 600, 416, str = "NEW", DT_CENTER);

	// 게임 시작을 안했거나 STOP 상태일 때
    if(!(m_gameParam & 0x01) || (m_gameParam & 0x04))
    {
        SetFontSize(30);
        DrawText(400, 448, 560, 480, str = "PAUSE", DT_LEFT, 0xA0A0A0);
        DrawText(400, 448, 560, 480, str = "STOP", DT_RIGHT, 0xA0A0A0);
		
    }
    else
    {
		
		// PAUSE에 마우스오버 했을때 글자 커짐
        SetFontSize(m_mouseOver == 0x02 ? 36 : 30);
        DrawText(400, 448, 560, 480, str = "PAUSE", DT_LEFT, m_gameParam & 0x02 ? 0x0000FF : 0xFFFFFF);
        
		// STOP에 마우스오버 했을때 글자 커짐
		SetFontSize(m_mouseOver == 0x04 ? 36 : 30);
        DrawText(400, 448, 560, 480, str = "STOP", DT_RIGHT, 0xFFFFFF);
    }

	 


	// SOUND에 마우스오버 했을때 글자 커짐
    SetFontSize(m_mouseOver == 0x08 ? 36 : 30);
    SetTextColor(m_memDC, 0xFFFFFF);
    DrawText(360, 512, 600, 544, str = "SOUND", DT_CENTER, m_gameParam & 0x08 ? 0x0000FF : 0xFFFFFF);
    
	// EXIT에 마우스오버 했을때 글자 커짐
	SetFontSize(m_mouseOver == 0x10 ? 36 : 30);
    DrawText(360, 576, 600, 608, str = "EXIT", DT_CENTER);
}

// 다음 block random
void CTetrisDlg::NextRandomBlock()
{
    UINT r;
    rand_s(&r);
    m_gameParam &= 0x1F;
    m_gameParam |= ((BYTE)((DOUBLE)r / ((__int64)UINT_MAX + 1) * 7) + 1) << 5;
    m_nextColor = NextRandomColor();
}

// 다음 block color random
BYTE CTetrisDlg::NextRandomColor()
{
    UINT r;
    rand_s(&r);
    return (BYTE)((DOUBLE)r / ((__int64)UINT_MAX + 1) * 4 + 1);
}



// 다음 block random
void CTetrisDlg::NextRandomBlock(int t)
{
    UINT r;
    rand_s(&r);
    m_gameParam &= 0x1F;
    m_gameParam |= ((BYTE)((DOUBLE)r / ((__int64)UINT_MAX + 1) * 7) + 1) << 5;
    m_nextColor = NextRandomColor(t);
}

// 다음 block color random
BYTE CTetrisDlg::NextRandomColor(int t)
{
    return t;
}

Block *CTetrisDlg::BlockFromIndex(BYTE i, int ty)
{
	
    Play(theApp.m_se_apprID);
    switch(i >> 5)
    {
    case 1:
        return new BlockI(ty, m_board);
    case 2:
        return new BlockJ(ty, m_board);
    case 3:
        return new BlockL(ty, m_board);
    case 4:
        return new BlockO(ty, m_board);
    case 5:
        return new  BlockS(ty, m_board);
    case 6:
        return new BlockT(ty, m_board);
    default:
        return new BlockZ(ty, m_board);
    }
}
// Block Index 부여
Block *CTetrisDlg::BlockFromIndex(BYTE i)
{
    Play(theApp.m_se_apprID);
    switch(i >> 5)
    {
    case 1:
        return new BlockI(m_nextColor, m_board);
    case 2:
        return new BlockJ(m_nextColor, m_board);
    case 3:
        return new BlockL(m_nextColor, m_board);
    case 4:
        return new BlockO(m_nextColor, m_board);
    case 5:
        return new BlockS(m_nextColor, m_board);
    case 6:
        return new BlockT(m_nextColor, m_board);
    default:
        return new BlockZ(m_nextColor, m_board);
    }
}

// 현재 줄 체크
BOOL CTetrisDlg::CheckLine(BYTE row)
{
    BYTE *thisRow = m_board[row];
    
	// 한 열, 한 열 체크해서 하나라도 비어있으면 return false
	for(BYTE i = 0; i < COL; ++i)
    {
        if(!thisRow[i])
            return false;
    }
    return true;
}

// 줄 제거
void CTetrisDlg::RemoveLine(BYTE row)
{
    BYTE *prevRow;
    BYTE *thisRow = m_board[row];

	// block 이미지 적용
    CDC blockMemDC;
    blockMemDC.CreateCompatibleDC(&m_memDC);
    blockMemDC.SelectObject(m_block);

	// 비트맵 이미지 적용 옵션
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.AlphaFormat = AC_SRC_ALPHA;

    Play(theApp.m_se_dsprID);

	// 지워질 줄의 블럭 Opacity를 0으로 낮춤
    for(BYTE opacity = 220; opacity != 0; --opacity)
    {
        RedrawBkgnd(CRect(0, row << 5, WIDTH, (row + 1) << 5));
        bf.SourceConstantAlpha = opacity;
        for(BYTE i = 0; i < COL; ++i)
        {
            ::AlphaBlend(m_memDC, i << 5, row << 5, 32, 32, blockMemDC, (thisRow[i] - 1) << 5, 0, 32, 32, bf);
        }
        m_pDC->BitBlt(0, 0, WIDTH, HEIGHT, &m_memDC, 0, 0, SRCCOPY);
    }
    for(CHAR i = row; i > 0; --i)
    {
		// 윗줄이 한 칸 아래로 내려옴
        prevRow = m_board[i - 1];
        thisRow = m_board[i];

		// 내려온 줄을 thisRow로 저장
        for(BYTE j = 0; j < COL; ++j)
        {
            thisRow[j] = prevRow[j];
        }
    }
    for(BYTE i = 0; i < COL; ++i)
    {
        prevRow[i] = 0;
    }
    blockMemDC.DeleteDC();
    ++m_lines;
    Update();
}

// 게임 끝났는지 확인
BOOL CTetrisDlg::IsGameOver(BYTE blockType)
{
    switch(blockType)
    {
		/* **** */
    case 0:
        return m_board[0][3] || m_board[0][4] || m_board[0][5] || m_board[0][6];
		/*   * */
		/* *** */
	case 1:
        return m_board[0][3] || m_board[0][4] || m_board[0][5]|| m_board[1][5];
		/* *   */
		/* *** */
	case 2:
        return m_board[0][3] || m_board[0][4] || m_board[0][5] || m_board[1][3];
		/* ** */
		/* ** */
	case 3:
        return m_board[0][4] || m_board[0][5] || m_board[1][4] || m_board[1][5];
		/* **  */
		/*  ** */
	case 4:
        return m_board[0][4] || m_board[0][5] || m_board[1][3] || m_board[1][4];
		/*  *  */
		/* *** */
	case 5:
        return m_board[0][3] || m_board[0][4] || m_board[0][5] || m_board[1][4];
		/*  ** */
		/* **  */
	default:
        return m_board[0][3] || m_board[0][4] || m_board[1][4] || m_board[1][5];
    }
}

// 게임 종료
void CTetrisDlg::GameOver()
{
	// 게임 오버 실행
    Play(theApp.m_me_gmvrID);

	//게임이 꺼졌을때 timer를 꺼주는 함수
    KillTimer(555);

	// 게임 STOP
    m_gameParam |= 0x04;

	// 게임메뉴에서 stop과 pause 비활성화
    m_menu.EnableMenuItem(ID_GAME_STOP, MF_DISABLED | MF_GRAYED);
    m_menu.EnableMenuItem(ID_GAME_PAUSE, MF_DISABLED | MF_GRAYED);    
}

BOOL CTetrisDlg::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
        switch(pMsg->wParam)
		{
        // Disable OK & Cancel function
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
		}
	}
    return CDialog::PreTranslateMessage(pMsg);
}

// 버튼 및 메뉴 초기화
BOOL CTetrisDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 메뉴 로드
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
    m_menu.LoadMenu(IDR_MENU);

	// 메뉴설정
    SetMenu(&m_menu);
    m_menu.EnableMenuItem(ID_GAME_PAUSE, MF_DISABLED | MF_GRAYED);
    m_menu.EnableMenuItem(ID_GAME_STOP, MF_DISABLED | MF_GRAYED);

	// client 영역 활성화(테트리스 화면)
    m_pDC = new CClientDC(this);

	// 화면에 비트맵 출력
    m_memDC.CreateCompatibleDC(m_pDC);
    m_memBmp.CreateCompatibleBitmap(m_pDC, WIDTH + 320, HEIGHT);
    m_memDC.SelectObject(m_memBmp);

	// 글자의 back을 투명하게 설정
    m_memDC.SetBkMode(TRANSPARENT);
    SetFontSize(30);
    AdjustFrame();
    Initialize();

	// sound, level - beginner 메뉴 활성화
	m_menu.CheckMenuItem(ID_GAME_SOUND, MF_CHECKED);
	m_menu.CheckMenuItem(ID_LEVEL_BEGINNER, MF_CHECKED);


    return TRUE;
}

// WM_PAINT가 발생하면 해당 함수가 호출
void CTetrisDlg::OnPaint()
{
    Update();
	if (IsIconic())
	{
		// 그리기를 위한 컨텍스트
	    CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춤
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그림
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTetrisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 키보드 합수
void CTetrisDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 단축키 활용(F1, F2, F3, F4)
    switch(nChar)
    {
		// F1, NEW
    case VK_F1:
        PostMessage(WM_COMMAND, ID_GAME_NEW, 0L);
        break;
		// F2, PAUSE
    case VK_F2:
        if((m_gameParam & 0x01) && !(m_gameParam & 0x04))
            PostMessage(WM_COMMAND, ID_GAME_PAUSE, 0L);
        break;
		// F3, STOP
	case VK_F3:
        if((m_gameParam & 0x01) && !(m_gameParam & 0x04))
            PostMessage(WM_COMMAND, ID_GAME_STOP, 0L);
        break;
		// F4, SOUND
    case VK_F4:
        PostMessage(WM_COMMAND, ID_GAME_SOUND, 0L);
		break;
    }

	// 방향키 조작
    if((m_gameParam & 0x07) == 0x01)
    {
        switch(nChar)
        {
			// 위 방향키
        case VK_UP:
			if((m_ninterrupt ==2||m_ninterrupt==3)&&(m_ninterruptCount ==m_ninterruptTarget))
			{
				//인터럽트 카운트 초기화, 경고음 재생, 인터럽트 확률 재 설정.
				Play(theApp.m_w_ID);
				m_ninterruptTarget =GetTickCount()%5+1;
				m_ninterruptCount=0;
			
			}
			else
			{
				if(!(nFlags & 0x4000))
				{
					if(m_pBlock->canTurn())
					{
					
						Play(theApp.m_se_turnID);
						m_pBlock->turn();
					}
					else if(m_gameParam & 0x08)
					{
						Play(theApp.m_se_dsblID);
					}
				}
			}
			m_ninterruptCount++;
            break;
			// 아래 방향키
        case VK_DOWN:
			if((m_ninterrupt ==1||m_ninterrupt==3)&&(m_ninterruptCount ==m_ninterruptTarget))
			{
				//인터럽트 카운트 초기화, 경고음 재생, 인터럽트 확률 재 설정.
				int k = GetTickCount()%3;
				if(k==0)
				{
					if(m_pBlock->canMoveRight())
					{
						Play(theApp.m_w_ID);
						m_pBlock->moveRight();
					
					}
				}

				if(k==1)
				{
					if(m_pBlock->canMoveLeft())
					{
						Play(theApp.m_w_ID);
						m_pBlock->moveLeft();
					
					}
				}

				if(k==2)
				{
					Play(theApp.m_w_ID);
				}

				m_ninterruptTarget =GetTickCount()%5+1;
				m_ninterruptCount=0;
				
			}
			else
			{
				if(m_pBlock->canMoveDown())
				{
					m_pBlock->moveDown();
				}
			}
			m_ninterruptCount++;
            break;

			// 왼쪽 방향키
        case VK_LEFT:
			if((m_ninterrupt ==1||m_ninterrupt==3)&&(m_ninterruptCount ==m_ninterruptTarget))
			{
				//인터럽트 카운트 초기화, 경고음 재생, 인터럽트 확률 재 설정.
				if(m_pBlock->canMoveRight())
				{
					Play(theApp.m_w_ID);
					m_pBlock->moveRight();
					
				}
				m_ninterruptTarget =GetTickCount()%5+1;
				m_ninterruptCount=0;
				
			}
			else
			{
				if(m_pBlock->canMoveLeft())
				{
					m_pBlock->moveLeft();
				}
			}
			m_ninterruptCount++;
            break;

			// 오른쪽 방향키
        case VK_RIGHT:
			if((m_ninterrupt ==1||m_ninterrupt==3)&&(m_ninterruptCount ==m_ninterruptTarget))
			{
				//인터럽트 카운트 초기화, 경고음 재생, 인터럽트 확률 재 설정.
				if(m_pBlock->canMoveLeft())
				{
					Play(theApp.m_w_ID);
					m_pBlock->moveLeft();
				
				}
				m_ninterruptTarget =GetTickCount()%5+1;
				m_ninterruptCount=0;
			}
			else
			{
			    if(m_pBlock->canMoveRight())
				{
					m_pBlock->moveRight();
				}
			}
			m_ninterruptCount++;
            break;
		
		/* a 키를 누르면 게임당 한 번!
		   화면 안에 있는 모든 블럭을 지울 수 있다.*/
		case VK_A:
			if(oneShot == 1){
				for(BYTE i = 0; i < ROW; ++i)
				{
					BYTE *nextRow = m_board[i];
					
					for(BYTE j = 0; j < COL; ++j)
					{
						if( nextRow[j] !=0)
						nextRow[j] =0;
					}
				}
				
			oneShot = 0;
			}

			break;

			// space bar를 누르면 블럭이 한번에 내려옴,못내려올때까지 계속내림
		case VK_SPACE:
			while(m_pBlock->canMoveDown())
		   {
			    m_pBlock->moveDown();
		   }
			break;
        }
        Invalidate(FALSE);
    }
    CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

// 마우스를 클릭했을 때(클릭했다가 다른곳에서 마우스를 띄었을때를 위함)
void CTetrisDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if(m_mouseOver != 0x00)
    {
        m_gameParam |= 0x10;
		if(m_gameParam==24)
			m_bStart =true;
    }
}

// 마우스를 띄었을 때 동작
void CTetrisDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if(m_gameParam & 0x10)
    {
		// NEW
        if(m_mouseOver == 0x01)
        {
			//m_bStart=true;
            PostMessage(WM_COMMAND, ID_GAME_NEW);
        }

		// PAUSE
        else if((m_gameParam & 0x01) && !(m_gameParam & 0x04) && m_mouseOver == 0x02)
        {
            PostMessage(WM_COMMAND, ID_GAME_PAUSE);
        }

		// STOP
        else if((m_gameParam & 0x01) && !(m_gameParam & 0x04) && m_mouseOver == 0x04)
        {
            PostMessage(WM_COMMAND, ID_GAME_STOP);
        }

		// SOUND
        else if(m_mouseOver == 0x08)
        {
            PostMessage(WM_COMMAND, ID_GAME_SOUND);
        }

		// EXIT
        else if(m_mouseOver == 0x10)
        {
            PostMessage(WM_COMMAND, ID_GAME_EXIT);
        }
    }
}

// 버튼위에 마우스오버 했을 때
void CTetrisDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// NEW에 마우스 오버
    CRect rect;
	// NEW 영역
    rect.SetRect(459, 384, 502, 413);
    if(m_mouseOver != 0x01 && ::PtInRect(&rect, point))
    {
        Play(theApp.m_se_crsrID);
        m_mouseOver = 0x01;
        Invalidate(FALSE);
    }
    else if(m_mouseOver == 0x01 && !::PtInRect(&rect, point))
    {
        m_mouseOver = 0x00;
        Invalidate(FALSE);
    }

	// (NEW에 마우스 오버X) PAUSE에 마우스 오버
    else
    {
		// PAUSE 영역
        rect.SetRect(400, 448, 459, 477);
        if((m_gameParam & 0x01) && !(m_gameParam & 0x04) && m_mouseOver != 0x02 && ::PtInRect(&rect, point))
        {
            Play(theApp.m_se_crsrID);
            m_mouseOver = 0x02;
            Invalidate(FALSE);
        }
        else if(m_mouseOver == 0x02 && !::PtInRect(&rect, point))
        {
            m_mouseOver = 0x00;
            Invalidate(FALSE);
        }

		// (NEW, PAUSE에 마우스오버 X) STOP에 마우스 오버
        else
        {
			// STOP 영역
            rect.SetRect(512, 448, 560, 477);
            if((m_gameParam & 0x01) && !(m_gameParam & 0x04) && m_mouseOver != 0x04 && ::PtInRect(&rect, point))
            {
                Play(theApp.m_se_crsrID);
                m_mouseOver = 0x04;
                Invalidate(FALSE);
            }
            else if(m_mouseOver == 0x04 && !::PtInRect(&rect, point))
            {
                m_mouseOver = 0x00;
                Invalidate(FALSE);
            }

			// (NEW, PAUSE, STOP에 마우스오버 X) SOUND에 마우스 오버
            else
            {
				// SOUND 영역
                rect.SetRect(448, 512, 512, 541);
                if(m_mouseOver != 0x08 && ::PtInRect(&rect, point))
                {
                    Play(theApp.m_se_crsrID);
                    m_mouseOver = 0x08;
                    Invalidate(FALSE);
                }
                else if(m_mouseOver == 0x08 && !::PtInRect(&rect, point))
                {
                    m_mouseOver = 0x00;
                    Invalidate(FALSE);
                }

				// (NEW, PAUSE, STOP, SOUND에 마우스오버 X) EXIT에 마우스 오버
                else
                {
					// EXIT 영역
                    rect.SetRect(460, 576, 500, 608);
                    if(m_mouseOver != 0x10 && ::PtInRect(&rect, point))
                    {
                        Play(theApp.m_se_crsrID);
                        m_mouseOver = 0x10;
                        Invalidate(FALSE);
                    }
                    else if(m_mouseOver == 0x10 && !::PtInRect(&rect, point))
                    {
                        m_mouseOver = 0x00;
                        Invalidate(FALSE);
                    }
                }
            }
        }
    }
}

UINT CTetrisDlg::OnGetDlgCode()
{
    return DLGC_WANTARROWS | CDialog::OnGetDlgCode();
}

// 배경 지움
BOOL CTetrisDlg::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

// 타이머
void CTetrisDlg::OnTimer(UINT_PTR nIDEvent)
{
	// 일정 시간동안 block 아래로 내려옴
    if(m_pBlock->canMoveDown())
    {
        m_pBlock->moveDown();
    }

	// 줄을 체크하여 전부 채워진 줄은 제거
    else
    {
        for(CHAR i = ROW - 1; i >= 0; --i)
        {
            if(CheckLine(i))
            {
                RemoveLine(i);
                ++i;
            }
        }

        MSG msg;
        while(::PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE));
		
		// 게임이 종료되었는지 확인
        if(IsGameOver(m_gameParam & 0xE0))
        {
            GameOver();
        }
        
		// 게임이 종료되지 않았다면 계속 진행
		else
        {

			for(BYTE i = 0; i < ROW; ++i)
			{
				BYTE *nextRow = m_board[i];
				for(BYTE j = 0; j < COL; ++j)
				{
					BYTE clr = nextRow[j];
					if(clr ==100)
					{
						nextRow[j] =1;
					}
				}
			}

			if(m_bShake)
			{
				 Play(theApp.m_w_ID);
				int count =0;
				for(BYTE i = 0; i < ROW; ++i)
				{
					BYTE *nextRow = m_board[i];
					
					for(BYTE j = 0; j < COL; ++j)
					{
						if( nextRow[j] !=0)
							count++;
						nextRow[j] =0;
					}
				}

				int u=0;
				while(u<count)
				{
					for(char i = (ROW-1); i >=0 ; i--)
					{
						BYTE *nextRow = m_board[i];
						for(BYTE j = 0; j < COL; ++j)
						{
							if(u<count)
							{
								int k = rand()%3;
								if( k==0)
								{
									nextRow[j] =1;
									u++;
								}
							}
							else
							{
							
								break;
							}

							
						}
					}

					if(u>count)
					{
						break;
					}
				}

				m_bShake =false;
			}

            delete m_pBlock;
			m_pBlock = BlockFromIndex(m_gameParam & 0xE0);

			
			

			int r= GetTickCount()%7;
			if( r==0)
			{
				Play(theApp.m_w_ID);
				NextRandomBlock(100);
				m_nextColor = NextRandomColor(100); 
			}
			else
			{
				NextRandomBlock();
				m_nextColor = NextRandomColor(); 
			}

			//시스템시간을 10으로 나눈 나머지가 0일때 셰이크 발동(10의배수)
			r= GetTickCount()%10;
			if(r==0)
			{
				m_bShake=true;
			}

			//키 입력과는 상관없이 인터럽트 발동
			m_ninterrupt = rand()%5+1;
			m_ninterruptCount =0;
			m_ninterruptTarget =rand()%5+1;
			
			
	
        }
    }
    Invalidate(FALSE);
    CDialog::OnTimer(nIDEvent);
}

// NEW button
void CTetrisDlg::OnGameNew()
{
	// 초기화
    if(m_gameParam & 0x01)
    {
        Initialize();
		Stop(theApp.m_me_gmvrID);
    }
    m_gameParam |= 0x01;
    m_gameParam &= ~0x06;
    
	// STOP, PAUSE 활성화
	m_menu.EnableMenuItem(ID_GAME_STOP, MF_ENABLED);
    m_menu.EnableMenuItem(ID_GAME_PAUSE, MF_ENABLED);

	// level에 따른 타이머 설정
    SetTimer(555, 500 - m_level * 90, NULL);

	// 다음 block random
    NextRandomBlock();
    m_pBlock = BlockFromIndex(m_gameParam & 0xE0);
    NextRandomBlock();
    Invalidate(FALSE);
}

// PAUSE button
void CTetrisDlg::OnGamePause()
{
    Play(theApp.m_se_slctID);
    if(m_gameParam & 0x02)
    {
		// PAUSE 선택표시
        m_menu.CheckMenuItem(ID_GAME_PAUSE, MF_UNCHECKED);
        m_gameParam &= ~0x02;
        SetTimer(555, 500 - m_level * 90, NULL);
		m_bStart =true;
    }
    else
    {
        m_menu.CheckMenuItem(ID_GAME_PAUSE, MF_CHECKED);
        m_gameParam |= 0x02;
        KillTimer(555);
		m_bStart=false;
    }
    Invalidate(FALSE);
}

// STOP button
void CTetrisDlg::OnGameStop()
{
	m_befortime=GetTickCount();
	m_bStart=false;
    Play(theApp.m_se_slctID);
    m_gameParam &= ~0xE7;
    Initialize();
    Invalidate(FALSE);
}

// Beginner level
void CTetrisDlg::OnLevelBeginner()
{
    m_menu.GetSubMenu(0)->GetSubMenu(3)->CheckMenuItem(m_level, MF_BYPOSITION | MF_UNCHECKED);
    m_menu.CheckMenuItem(ID_LEVEL_BEGINNER, MF_CHECKED);
    m_level = 0;
    if(m_gameParam & 0x01)
        PostMessage(WM_COMMAND, ID_GAME_STOP, 0L);
}

// Intermediate level
void CTetrisDlg::OnLevelIntermediate()
{
    m_menu.GetSubMenu(0)->GetSubMenu(3)->CheckMenuItem(m_level, MF_BYPOSITION | MF_UNCHECKED);
    m_menu.CheckMenuItem(ID_LEVEL_INTERMEDIATE, MF_CHECKED);
    m_level = 1;
    if(m_gameParam & 0x01)
        PostMessage(WM_COMMAND, ID_GAME_STOP, 0L);
}

// Advanced level
void CTetrisDlg::OnLevelAdvanced()
{
    m_menu.GetSubMenu(0)->GetSubMenu(3)->CheckMenuItem(m_level, MF_BYPOSITION | MF_UNCHECKED);
    m_menu.CheckMenuItem(ID_LEVEL_ADVANCED, MF_CHECKED);
    m_level = 2;
    if(m_gameParam & 0x01)
        PostMessage(WM_COMMAND, ID_GAME_STOP, 0L);
}

// Expert level
void CTetrisDlg::OnLevelExpert()
{
    m_menu.GetSubMenu(0)->GetSubMenu(3)->CheckMenuItem(m_level, MF_BYPOSITION | MF_UNCHECKED);
    m_menu.CheckMenuItem(ID_LEVEL_EXPERT, MF_CHECKED);
    m_level = 3;
    if(m_gameParam & 0x01)
        PostMessage(WM_COMMAND, ID_GAME_STOP, 0L);
}

// Devil level
void CTetrisDlg::OnLevelDevil()
{
    m_menu.GetSubMenu(0)->GetSubMenu(3)->CheckMenuItem(m_level, MF_BYPOSITION | MF_UNCHECKED);
    m_menu.CheckMenuItem(ID_LEVEL_DEVIL, MF_CHECKED);
    m_level = 4;
    if(m_gameParam & 0x01)
        PostMessage(WM_COMMAND, ID_GAME_STOP, 0L);
}

// Hell level
void CTetrisDlg::OnLevelHell()
{
    m_menu.GetSubMenu(0)->GetSubMenu(3)->CheckMenuItem(m_level, MF_BYPOSITION | MF_UNCHECKED);
    m_menu.CheckMenuItem(ID_LEVEL_HELL, MF_CHECKED);
    m_level = 5;
    if(m_gameParam & 0x01)
        PostMessage(WM_COMMAND, ID_GAME_STOP, 0L);
}

// SOUND button
void CTetrisDlg::OnGameSound()
{
    Play(theApp.m_se_slctID);
    if(m_gameParam & 0x08)
    {
        m_menu.CheckMenuItem(ID_GAME_SOUND, MF_UNCHECKED);
        m_gameParam &= ~0x08;
    }
    else
    {
        m_menu.CheckMenuItem(ID_GAME_SOUND, MF_CHECKED);
        m_gameParam |= 0x08;
    }
    Invalidate(FALSE);
}

// EXIT button
void CTetrisDlg::OnGameExit()
{
    PostMessage(WM_COMMAND, IDOK, 0L);
}

// help - about
void CTetrisDlg::OnHelpAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}
