#ifndef __LIB_GAMEMAIN_H
#define __LIB_GAMEMAIN_H

#define __SHO_GS

#define ENABLE_CHATROOM // Enables the chatroom functions

#include "LIB_Util.h"

#include "CClientSOCKET.h"
#include "CVector.h"
#include "DLLIST.h"
#include "DataTYPE.h"
#include "NET_Prototype.h"
#include "SLLIST.h"
#include "classHASH.h"
#include "classINDEX.h"
#include "classIOCP.h"
#include "classSTR.h"
#include "classSYNCOBJ.h"
#include "classTIME.h"
#include "classUTIL.h"
#include "md5.h"

#include "ETC_Math.h"

#include "IO_STB.h"
#include "OBJECT.h"

#include "rose/common/log.h"
#include "rose/common/game_config.h"
#include "rose/common/server_config.h"

namespace Rose::Common {
class ServerConfig;
}

class CLIB_GameSRV {
public:
    Rose::Common::ServerConfig config;

private:
    static CLIB_GameSRV* m_pInstance;

    HWND m_hMainWND; ///< 윈도우 핸들
    CTimer* m_pWorldTIMER; ///< 월드 타이머
    bool* m_pCheckedLocalZONE;

    DWORD m_dwRandomSEED;

    BYTE m_btChannelNO; ///< 동작중인 채널 번호
    BYTE m_btLowAGE; ///< 연령 접속 제한 최저 나이
    BYTE m_btHighAGE; ///< 연령 접속 제한 최고 나이
    DWORD m_dwUserLIMIT; ///< 최대 접속 가능한 인원수

    DWORD m_dwMaxLevel;
    DWORD m_dwMaxStats;

    int m_iLangTYPE; ///< 국가 코드
    bool m_bTestServer; ///< 테스트 서버인가 ?

    CLIB_GameSRV();
    virtual ~CLIB_GameSRV();
    void init(HINSTANCE hInstance, const Rose::Common::ServerConfig& config);

    bool CheckSTB_UseITEM();
    bool CheckSTB_NPC();
    bool CheckSTB_DropITEM();
    bool CheckSTB_ItemRateTYPE();
    bool CheckSTB_Motion();
    bool CheckSTB_GemITEM();
    bool CheckSTB_ListPRODUCT();

    bool Load_BasicDATA();
    void Free_BasicDATA();

    bool ConnectToLSV();
    void DisconnectFromLSV();

    bool ConnectToLOG();
    void DisconnectFromLOG();

    void TranslateNameWithDescKey(STBDATA* pOri, char* szStbFile, int iNameCol);
    void TranslateNameWithColoumKey(STBDATA* pOri,
        char* szStbFile,
        int iLangCol,
        int iNameCol,
        int iDescCol);

    friend VOID CALLBACK GS_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

public:
    bool connect_database();

    bool Start(HWND hMainWND,
        BYTE btChannelNO,
        BYTE btLowAge,
        BYTE btHighAge);
    void Shutdown();

    short InitLocalZone(bool bAllActive);
    bool CheckZoneToLocal(short nZoneNO, bool bChecked);
    char* GetZoneName(short nZoneNO);
    bool IsLocalCheckedZONE(short nZoneNO) { return m_pCheckedLocalZONE[nZoneNO]; }

    DWORD GetRandomSeed() { return m_dwRandomSEED; }
    DWORD GetMaxStats() { return m_dwMaxStats; }
    DWORD GetMaxLevel() { return m_dwMaxLevel; }

    BYTE GetChannelNO() { return m_btChannelNO; }
    BYTE GetLowAGE() { return m_btLowAGE; }
    BYTE GetHighAGE() { return m_btHighAGE; }
    bool IsTestServer() { return m_bTestServer; }

    int GetLangTYPE() { return m_iLangTYPE; }

    void Send_ANNOUNCE(short nZoneNO, char* szMsg);
    void Set_UserLIMIT(DWORD dwUserLimit);
    DWORD Get_UserLIMIT() { return m_dwUserLIMIT; }

    static CLIB_GameSRV* GetInstance() { return m_pInstance; }
    static CLIB_GameSRV* InitInstance(HINSTANCE hInstance, const Rose::Common::ServerConfig& config) {
        if (NULL == m_pInstance) {
            m_pInstance = new CLIB_GameSRV();
            _ASSERT(m_pInstance);
            m_pInstance->init(hInstance, config);
        }
        return m_pInstance;
    }

    static Rose::Common::GameConfig& game_config() {
        return CLIB_GameSRV::GetInstance()->config.game;
    }

    void Destroy() { SAFE_DELETE(m_pInstance); }
};

#define GS_TIMER_LSV 1
#define GS_TIMER_LOG 2
#define GS_TIMER_WORLD_TIME 3

#define RECONNECT_TIME_TICK 10000 // 10 sec

extern VOID CALLBACK GS_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

extern void Save_AllUSER();

extern DWORD GetServerBuildNO();
extern DWORD GetServerStartTIME();

inline short
Get_ServerChannelNO() {
    return CLIB_GameSRV::GetInstance()->GetChannelNO();
}
inline int
Get_ServerLangTYPE() {
    return CLIB_GameSRV::GetInstance()->GetLangTYPE();
}

inline DWORD
Get_ServerMaxStats() {
    return CLIB_GameSRV::GetInstance()->GetMaxStats();
}
inline DWORD
Get_ServerMaxLevel() {
    return CLIB_GameSRV::GetInstance()->GetMaxLevel();
}

//-------------------------------------------------------------------------------------------------
#endif
