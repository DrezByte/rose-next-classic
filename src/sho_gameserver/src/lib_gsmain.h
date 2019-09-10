#ifndef	__LIB_GAMEMAIN_H
#define	__LIB_GAMEMAIN_H

#define	__SHO_GS

#define ENABLE_CHATROOM		// Enables the chatroom functions


#include "LIB_Util.h"

#include "md5.h"
#include "CVector.h"
#include "DataTYPE.h"
#include "NET_Prototype.h"
#include "DLLIST.h"
#include "SLLIST.h"
#include "classSTR.h"
#include "classUTIL.h"
#include "classHASH.h"
#include "classINDEX.h"
#include "classIOCP.h"
#include "classTIME.h"
#include "classSYNCOBJ.h"
#include "CClientSOCKET.h"

#include "ETC_Math.h"

#include "IO_STB.h"
#include "OBJECT.h"

#include "rose/common/log.h"

class CLIB_GameSRV {
private  :
	static CLIB_GameSRV	*m_pInstance;

	HWND		m_hMainWND;				///< ������ �ڵ�
	CTimer	   *m_pWorldTIMER;			///< ���� Ÿ�̸�
	bool	   *m_pCheckedLocalZONE;

	CStrVAR	m_BaseDataDIR;				///< ���� ����Ÿ ����
	CStrVAR	m_ServerNAME;				///< ������ ���� �̸�
	CStrVAR m_ServerIP;					///< ���� IP
	int		m_iListenPortNO;			///< ����� ���� ��Ʈ��ȣ

	CStrVAR	m_DBServerIP;				///< ��� ���� IP
	CStrVAR	m_DBName;					///< ��� ����
	CStrVAR	m_DBUser;					///< ��� �����
	CStrVAR	m_DBPassword;				///< ��� ����� ��й�ȣ
	CStrVAR	m_LogUser;
	CStrVAR	m_LogPW;

	CStrVAR m_AccountServerIP;			///< sho_as�� ������ IP
	int		m_iAccountServerPORT;		///< sho_as�� ������ port no

	CStrVAR m_LoginServerIP;			///< sho_ws�� ������ IP
	int		m_iLoginServerPORT;			///< sho_ws�� ������ port no

	DWORD	m_dwRandomSEED;

	BYTE	m_btChannelNO;				///< �������� ä�� ��ȣ
	BYTE	m_btLowAGE;					///< ���� ���� ���� ���� ����
	BYTE	m_btHighAGE;				///< ���� ���� ���� �ְ� ����
	DWORD	m_dwUserLIMIT;				///< �ִ� ���� ������ �ο���

	DWORD	m_dwMaxLevel;
	DWORD	m_dwMaxStats;

	int		m_iLangTYPE;				///< ���� �ڵ�
	bool	m_bTestServer;				///< �׽�Ʈ �����ΰ� ?

	CLIB_GameSRV();
	virtual ~CLIB_GameSRV ();
	void SystemINIT( HINSTANCE hInstance, char *szBaseDataDIR, int iLangType );


	bool CheckSTB_UseITEM ();
	bool CheckSTB_NPC ();
	bool CheckSTB_DropITEM ();
	bool CheckSTB_ItemRateTYPE ();
	bool CheckSTB_Motion ();
	bool CheckSTB_GemITEM ();
	bool CheckSTB_ListPRODUCT ();

	bool Load_BasicDATA ();
	void Free_BasicDATA ();

	bool ConnectToLSV ();
	void DisconnectFromLSV ();

	bool ConnectToASV ();
	void DisconnectFromASV ();

	bool ConnectToLOG ();
	void DisconnectFromLOG ();

	void TranslateNameWithDescKey	( STBDATA *pOri, char *szStbFile, int iNameCol );
	void TranslateNameWithColoumKey ( STBDATA *pOri, char *szStbFile, int iLangCol, int iNameCol, int iDescCol );

	friend VOID CALLBACK GS_TimerProc ( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

public   :
	bool ConnectSERVER( char *szDBServerIP,		char *szDBName,
						char *szDBUser,			char *szDBPW,
						char *szLogUser,		char *szLogPW,
						char *szMallUser,		char *szMallPW,
						char *szLoginServerIP,	int iLoginServerPort,
						char *szLogServerIP,	int iLogServerPortNO );

	bool Start( HWND hMainWND, char *szServerName, char *szClientListenIP, int iClientListenPort, BYTE btChannelNO, BYTE btLowAge, BYTE btHighAge );
	void Shutdown ();

	short InitLocalZone( bool bAllActive );
	bool  CheckZoneToLocal(short nZoneNO, bool bChecked);
	char* GetZoneName( short nZoneNO );
	bool  IsLocalCheckedZONE( short nZoneNO )	{	return m_pCheckedLocalZONE[nZoneNO];	}	

	char* GetServerName()						{	return m_ServerNAME.Get();	}
	char* GetServerIP()							{	return m_ServerIP.Get();	}
	int	  GetListenPort()						{	return m_iListenPortNO;		}
	DWORD GetRandomSeed()						{	return m_dwRandomSEED;		}
	DWORD GetMaxStats()							{	return m_dwMaxStats;		}
	DWORD GetMaxLevel()							{	return m_dwMaxLevel;		}

	BYTE  GetChannelNO ()						{	return m_btChannelNO;		}
	BYTE  GetLowAGE ()							{	return m_btLowAGE;			}
	BYTE  GetHighAGE ()							{	return m_btHighAGE;			}
	bool  IsTestServer()						{	return m_bTestServer;		}

	int	  GetLangTYPE()							{	return m_iLangTYPE;			}

	void  Send_ANNOUNCE( short nZoneNO, char *szMsg );
	void  Set_UserLIMIT( DWORD dwUserLimit );
	DWORD Get_UserLIMIT()						{	return m_dwUserLIMIT;		}
	
	static CLIB_GameSRV *GetInstance ()			{	return m_pInstance;	}
	static CLIB_GameSRV *InitInstance( HINSTANCE hInstance, char *szBaseDataDIR, int iLangType )
	{
		if ( NULL == m_pInstance ) {
			m_pInstance = new CLIB_GameSRV();
			_ASSERT( m_pInstance );
			m_pInstance->SystemINIT( hInstance, szBaseDataDIR, iLangType );
		}
		return m_pInstance;
	}
	void Destroy ()								{	SAFE_DELETE( m_pInstance );	}
} ;

#define	GS_TIMER_LSV			1
#define	GS_TIMER_LOG			2
#define	GS_TIMER_WORLD_TIME		3
#define	GS_TIMER_ASV			4

#define	RECONNECT_TIME_TICK		10000	// 10 sec

extern VOID CALLBACK GS_TimerProc ( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

extern void Save_AllUSER ();

extern DWORD GetServerBuildNO ();
extern DWORD GetServerStartTIME ();

inline short Get_ServerChannelNO()
{
	return CLIB_GameSRV::GetInstance()->GetChannelNO();
}
inline int	 Get_ServerLangTYPE()
{
	return CLIB_GameSRV::GetInstance()->GetLangTYPE();
}

inline DWORD Get_ServerMaxStats()
{
	return CLIB_GameSRV::GetInstance()->GetMaxStats();
}
inline DWORD Get_ServerMaxLevel()
{
	return CLIB_GameSRV::GetInstance()->GetMaxLevel();
}

//-------------------------------------------------------------------------------------------------
#endif