#pragma once

// #include "classTIME.h"

class CTimer;

namespace Rose {
namespace Common {
struct DatabaseConfig;
}
} // namespace Rose

class SHO_LS {
public:
    int m_iClientListenPortNO;
    int m_iServerListenPortNO;
    bool m_bShowOnlyWS;

    SHO_LS();
    ~SHO_LS();
    void SystemINIT(HINSTANCE hInstance);

    static SHO_LS* m_pInstance;

    CTimer* m_pTIMER;

public:
    union {
        BYTE m_btMD5[33];
        DWORD m_dwMD5[8];
    };

    bool StartClientSOCKET(int iClientListenPort, int iLimitUserCNT, BYTE btMD5[32]);
    void CloseClientSOCKET();
    bool StartServerSOCKET(HWND hMainWND,
        char* szDBServerIP,
        int iServerListenPort,
        DWORD dwLoginRight,
        bool bShowOnlyWS);
    void Shutdown();

    void Send_ANNOUNCE(void* pServer, char* szAnnounceMsg);

    void SetLoginRIGHT(DWORD dwLoginRight);
    void SetLimitUserCNT(int iLimitUserCNT);

    //---------------------------------------------------------------------------------------------
    static bool IsShowOnlyWS() { return m_pInstance->m_bShowOnlyWS; }
    static SHO_LS* GetInstance() { return m_pInstance; }
    static SHO_LS* InitInstance(HINSTANCE hInstance) {
        if (NULL == m_pInstance) {
            // m_pInstance = (class SHO_WS *)1;
            m_pInstance = new SHO_LS();
            _ASSERT(m_pInstance);
            m_pInstance->SystemINIT(hInstance);
        }
        return m_pInstance;
    }
    void Destroy() { SAFE_DELETE(m_pInstance); }

    bool connect_database(const Rose::Common::DatabaseConfig& config);
};

extern DWORD GetServerBuildNO();
extern DWORD GetServerStartTIME();