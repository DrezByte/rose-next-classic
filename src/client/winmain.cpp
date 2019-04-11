#include "stdafx.h"
#define AROSE

#include "CApplication.h"
#include "Game.h"
#include "Network/CNetwork.h"
#include "Util/VFSManager.h"
#include "Util/SystemInfo.h"
#include "CClientStorage.h"
#include "System/CGame.h"
#include "Interface/ExternalUI/CLogin.h"
#include "triggerinfo/triggerinfo.h"

#include "Util/CheckHack.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


bool Init_DEVICE (void)
{
	bool bRet = false;

	//--------------------------[ engine related ]-----------------------//
	::initZnzin();	
	::openFileSystem("data.idx");
	::doScript("scripts/init.lua");

	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	::setDisplayQualityLevel( c_iPeformances[g_ClientStorage.GetVideoPerformance()] );
	t_OptionVideo Video;
	g_ClientStorage.GetVideoOption(Video);
	setFullSceneAntiAliasing( Video.iAntiAlising );


	if(!g_pCApp->IsFullScreenMode()) 
	{
		RECT ClientRt;
		GetClientRect(g_pCApp->GetHWND(),&ClientRt);
		::setScreen(ClientRt.right, ClientRt.bottom, Resolution.iDepth, g_pCApp->IsFullScreenMode() );		
	}
	else
		::setScreen(g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT(), Resolution.iDepth, g_pCApp->IsFullScreenMode() );


	bRet = ::attachWindow((const void*)g_pCApp->GetHWND());


	CD3DUtil::Init( );

	g_pSoundLIST = new CSoundLIST( g_pCApp->GetHWND() );
	g_pSoundLIST->Load ( "3DDATA\\STB\\FILE_SOUND.stb" );

	return bRet;
}

//-------------------------------------------------------------------------------------------------
void Free_DEVICE (void)
{	
	delete g_pSoundLIST;

	CD3DUtil::Free ();

	//--------------------------[ engine related ]-----------------------//
	::detachWindow();

	::closeFileSystem();
	::destZnzin();  
}
//------------------------------------------------------------------------------------------------
// 2005. 5. 6. 조호동
// 중복 실행 체크용 소켓 해제
SOCKET listener;

void CloseDuplicateAppSocket (void)
{
	closesocket(listener);
	::WSACleanup();
}

// 중복 실행 체크 : 특정 포트가 중복 생성 안되는 점을 이용.
bool IsDuplicateApp (void)
{
	WSADATA wsadata;
	::WSAStartup( MAKEWORD( 2, 2 ), &wsadata );

	// socket 생성
	listener = ::socket(AF_INET, SOCK_STREAM, 0);
	// listening
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(7777);

	int result = ::bind(listener, (sockaddr*) &addr, sizeof(sockaddr_in));
	result = ::listen(listener, 5);

	if( result == SOCKET_ERROR )
	{
		CloseDuplicateAppSocket();
		MessageBox(NULL, "이미 게임이 실행 중입니다 !", "에러", MB_OK );
		return TRUE;
	}

	return FALSE;
}

//-------------------------------------------------------------------------------------------------
int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#if 0
	if( IsDuplicateApp() )
		return FALSE;
#endif

	g_SystemInfo.CollectingSystemInfo();
	int iWindowVersion = g_SystemInfo.GetWindowsVersion();

	//-------------------------------------------------------------------------------
	/// Init Trigger VFS
	//-------------------------------------------------------------------------------
	VHANDLE hVFS = OpenVFS( "data.idx", (iWindowVersion == WINDOWS_98)?"r":"mr" );	
	(CVFSManager::GetSingleton()).SetVFS( hVFS );
	(CVFSManager::GetSingleton()).InitVFS( VFS_TRIGGER_VFS );	


	//-------------------------------------------------------------------------------
	/// Get Time
	//-------------------------------------------------------------------------------
	GetLocalTime(	&g_GameDATA.m_SystemTime );	

	//-------------------------------------------------------------------------------
	/// Init System object
	//-------------------------------------------------------------------------------
	g_pCApp		= CApplication::Instance ();
	g_pNet		= CNetwork::Instance (hInstance);
	g_pCRange	= CRangeTBL::Instance ();

	//-------------------------------------------------------------------------------
	/// Load Range table
	//-------------------------------------------------------------------------------
	if ( !g_pCRange->Load_TABLE ("3DDATA\\TERRAIN\\O_Range.TBL") ) {
		g_pCApp->ErrorBOX ( "3DDATA\\TERRAIN\\O_Range.TBL file open error", CUtil::GetCurrentDir (), MB_OK);
		return 0;
	}	

	//-------------------------------------------------------------------------------
	/// IP/Port Setting
	//-------------------------------------------------------------------------------
	g_GameDATA.m_wServerPORT = TCP_LSV_PORT;
	g_GameDATA.m_ServerIP.Set( TCP_LSV_IP );

	if ( !g_pCApp->ParseArgument( lpCmdLine ) )
		return 0;

	//-------------------------------------------------------------------------------
	/// 윈도우 생성시 해상도에 관한 Data가 필요하여 이곳에서 로드한다.
	//-------------------------------------------------------------------------------	
	g_TblResolution.Load2( "3DDATA\\STB\\RESOLUTION.STB",	false, false );
	g_TblCamera.Load2( "3DDATA\\STB\\LIST_CAMERA.STB" ,false, false );

	//-------------------------------------------------------------------------------
	///클라이언트에 저장된 Data를 로드한다.
	//-------------------------------------------------------------------------------
	g_ClientStorage.Load();

	//-------------------------------------------------------------------------------
	///이전 옵션에서 조정된 해상도의 인덱스를 가져와서 g_TblResolution을 참조하여
	///해상도를 조정한다.
	//-------------------------------------------------------------------------------	
#pragma message("저장된 해상도를 테스트해서 현재 실행가능한지 판단하고 안되면 메세지 박스 띄우고 TriggerDetect를 실행시킨다")
	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	/// 범위값 체크 
	UINT iFullScreen = g_ClientStorage.GetVideoFullScreen();

	g_pCApp->SetFullscreenMode( iFullScreen );
	g_pCApp->CreateWND ("classCLIENT", "Tsuki Online [loading]", Resolution.iWidth, Resolution.iHeight,Resolution.iDepth, hInstance);

#ifndef _DEBUG
	// 시스템 정보를 모음
	TI_ReadSysInfoFile ();
#endif


	g_pObjMGR = CObjectMANAGER::Instance ();
	g_pCApp->ResetExitGame();

	bool bDeviceInitialized = Init_DEVICE();

	if ( bDeviceInitialized ) {

#ifndef singleclient
		CGame::GetInstance().GameLoop();
#else
		///중복실행막은 버젼
		HANDLE  hMUTEX = NULL;
		const char szMUTEX[] = "ROSEonline";

		/// 중복실행 체크...		2004. 9. 30 by icarus
#ifdef	_DEBUG
		if ( !g_GameDATA.m_bCheckDupRUN ) 
		{
			CGame::GetInstance().GameLoop();
		} 
		else
#endif
		{
			hMUTEX = ::CreateMutex( NULL, true, szMUTEX );
			switch( GetLastError() ) {
				case ERROR_SUCCESS	:
					CGame::GetInstance().GameLoop();

					::ReleaseMutex( hMUTEX );
					::CloseHandle( hMUTEX );
					break;
				case ERROR_ALREADY_EXISTS :
					// 이미 다른 클라이언트가 실행중이다..
				default :
					// 뭐냐 ???
					break;
			}				
			/*	
			hMUTEX = ::OpenMutex( MUTEX_ALL_ACCESS, false, szMUTEX );
			if ( NULL == hMUTEX ) {
			hMUTEX = ::CreateMutex( NULL, true, szMUTEX );

			CGame::GetInstance().GameLoop();

			::ReleaseMutex( hMUTEX );
			::CloseHandle( hMUTEX );
			} /// else 이미 다른 클라이언트가 실행중이다..
			*/
		}
#endif
	}

#if !defined(_DE) && !defined(_TAIWAN)
	CloseDuplicateAppSocket();
#endif

	Free_DEVICE ();


	g_TblCamera.Free();
	g_TblResolution.Free();

	g_pCApp->Destroy ();
	g_pNet->Destroy ();

	g_pCRange->Destroy ();	

	return 0;
}
