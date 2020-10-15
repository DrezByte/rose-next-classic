#ifndef _CCLIENTSTORAGE_
#define _CCLIENTSTORAGE_

#include "Interface/InterfaceType.h"
#include <string>
//////////////////////////////////////////////////////////////////////
/// Ŭ���̾�Ʈ ���̵忡 ����Ǵ� Data�� Load/Save�ϴ� Class
/// 1. �ɼ�, �κ��丮���� ��������ġ, ���̾�α׵��� ��ġ���� �����Ѵ�.
/// 2. �κ��丮�� ���̾�α���ġ���� WritePrivateProfileStruct�� �̿�����.

struct t_OptionResolution {
    int iWidth;
    int iHeight;
    int iDepth;
    int iFrequency;
};

struct t_OptionVideo {
    t_OptionResolution tResolution;
    UINT iCamera;
    UINT iPerformance;
    UINT iFullScreen;
    UINT iUseRoughMap;
    UINT iAntiAlising;
    // Render the game even if window is not focused
    bool background_render;
};

struct t_OptionSound {
    UINT iBgmVolume;
    UINT iEffectVolume;
};

struct t_OptionPlay {
    UINT uiControlType;
    int iShowNpcName;
    int iShowMobName;
    int iShowPcName;
    // Enable/Disable drawing healthbar + name above current player
    bool iShowMyName;
};

struct t_OptionCommunity {
    int iWhisper;
    int iAddFriend;
    int iExchange;
    int iParty;
    int iMessanger;
};

struct t_OptionKeyboard {
    int iChattingMode;
};

struct t_DialogPos {
    short nPosX;
    short nPosY;
};

struct S_InventoryData {
    long lRealIndex;
    long lVirtualIndex;
};

const int MAX_BGM_VOLUME = 9;
const int MAX_EFFECT_VOLUME = 9;
const int DEFAULT_BGM_VOLUME = 1; /// -1000 : OptionDlg::Create()����
const int DEFAULT_EFFECT_VOLUME = 3;
const int MAX_GAMMA_COUNT = 5;
const int MAX_PERFORMANCE_COUNT = 5;
const int g_iDefaultCamera = 1;
const int g_iDefaultPerfromance = 3;
const int g_iDefaultFullScreen = 0;
const int g_iDefaultAntiAlising = 1; // <AntiAlising />
const int c_iDefaultControlType = 1;

const int c_iDefaultScreenWidth = 1024;
const int c_iDefaultScreenHeight = 768;
const int c_iDefaultColorDepth = 32;
const int c_iDefaultFrequency = 60;

const short g_nDefaultInventoryIdx = 0;
const float c_fDefaultGammaValue = 1.0f;

const int c_iDefaultCommunityOption = 1; ///���
const int c_iDefaultShowName = 1;
extern const long g_ListBgmVolume[MAX_BGM_VOLUME];
extern const long g_ListEffectVolume[MAX_EFFECT_VOLUME];
extern const float c_GammaValues[MAX_GAMMA_COUNT];
extern const int c_iPeformances[MAX_PERFORMANCE_COUNT];

class CClientStorage {
public:
    CClientStorage(void);
    ~CClientStorage(void);

    void Load();
    void Save();

    void LoadAvatarData();

    bool ReadBool(const char* section, const char* key, bool default = false);
    void WriteBool(const char* section, const char* key, bool val);

    ///*********************************************************************/
    /// VIDEO
    void SetVideoOption(t_OptionVideo& option);
    void GetVideoOption(t_OptionVideo& option);

    UINT GetVideoPerformance() { return m_VideoOption.iPerformance; }
    UINT GetVideoFullScreen() { return m_VideoOption.iFullScreen; }
    t_OptionResolution GetResolution() { return m_VideoOption.tResolution; }
    void SetVideoFullScreen(UINT iMode) { m_VideoOption.iFullScreen = iMode; }

    void SetVideoPerformance(int iValue) { m_VideoOption.iPerformance = iValue; }
    void SetUseRoughMap(UINT iUseRoughMap) { m_VideoOption.iUseRoughMap = iUseRoughMap; }
    UINT GetUseRoughMap() { return m_VideoOption.iUseRoughMap; }
    ///*********************************************************************/
    /// Sound
    void SetSoundOption(t_OptionSound& option);
    void GetSoundOption(t_OptionSound& option);
    UINT GetBgmVolumeIndex() { return m_SoundOption.iBgmVolume; }
    UINT GetEffectVolumeIndex() { return m_SoundOption.iEffectVolume; }

    long GetBgmVolumeByIndex(int i);
    long GetEffectVolumeByIndex(int i);
    ///*********************************************************************/
    /// KeyBoard
    void SetKeyboardOption(t_OptionKeyboard& option);
    void GetKeyboardOption(t_OptionKeyboard& option);
    ///*********************************************************************/
    /// Play
    void SetPlayOption(t_OptionPlay& option);
    void GetPlayOption(t_OptionPlay& option);
    UINT GetControlType();
    bool IsShowNpcName();
    bool IsShowPcName();
    bool IsShowMobName();

    ///*********************************************************************/
    /// Party
    bool IsShowPartyMemberHpGuage();
    void SetShowPartyMemberHpGuage(bool b);
    ///*********************************************************************/
    /// Community
    void SetCommunityOption(t_OptionCommunity& option);
    void GetCommunityOption(t_OptionCommunity& option);

    bool IsApproveAddFriend(); //
    bool IsApproveWhisper(); //
    bool IsApproveMessanger(); //
    bool IsApproveParty(); //
    bool IsApproveExchange(); //
    ///---------------------------------------------------------------------/
    /// Dialog Position
    bool HasSavedDialogPos() { return m_bHasSavedDialogPos; }
    short GetSavedDialogPosX(int iDialogID);
    short GetSavedDialogPosY(int iDialogID);
    void SetSavedDialogPos(int iDialogID, POINT pt);

    /// Dialog Type
    int GetQuickBarDlgType() { return m_iQuickBarDlgType; }
    int GetQuickBarExtDlgType() { return m_iQuickBarExtDlgType; }
    int GetChatDlgType() { return m_iChatDlgType; }
    void SetQuickBarDlgType(int iType) { m_iQuickBarDlgType = iType; }
    void SetQuickBarExtDlgType(int iType) { m_iQuickBarExtDlgType = iType; } // 2nd Skillbar
    void SetChatDlgType(int iType) { m_iChatDlgType = iType; }
    /// QuickBar�� Chat Dialog�� Type

    ///*********************************************************************/
    /// Virtual Inventory Table
    // short GetInvenIdxFromSavedVirtualInven( int iType, int iIdx ){ return m_VirtualInventory[
    // iType ] [ iIdx ] ; } void  ClearVirtualInvenTable(){ ZeroMemory( m_VirtualInventory, sizeof(
    // m_VirtualInventory )); } void  SetVirutalInven( int iType, int iIdx, short nRealInvenIdx ){
    // m_VirtualInventory[ iType ][ iIdx ] = nRealInvenIdx; }
    ///*********************************************************************/
    ///������ �ɼ��� �����Ű�� Method : COptionDlg�� WinMain.cpp��� ���ȴ�.
    void ApplyCameraOption(short i);

    void SetSelectAvatarName(const char* pszName) { m_strAvatarName = pszName; }

    void GetInventoryData(const char* pszName, std::list<S_InventoryData>& list);
    void SetInventoryData(const char* pszName, std::list<S_InventoryData>& list);

    bool IsSaveLastConnectID();
    void SaveLastConnectID(const char* pszID);
    const char* GetLastConnectedID();
    void SetSaveLastConnectID(bool b);
    void SaveOptionLastConnectID();

    /// �Ϻ� ��Ʈ�ʻ� ���� ����
    int GetJapanRoute();
    void SetJapanRoute(int route);
    void SaveJapanRoute();

public:
    bool m_bHasSavedDialogPos; ///������ ����� Dialog Position�� �ִ°�?
    t_OptionVideo m_VideoOption;
    t_OptionSound m_SoundOption;
    t_OptionPlay m_PlayOption;
    t_OptionCommunity m_CommunityOption;
    t_DialogPos m_DialogPos[DLG_TYPE_MAX + 1];

    t_OptionKeyboard m_KeyboardOption;

    bool m_bShowPartyMemberHpGuage;
    int m_iQuickBarDlgType; /// Vertical, Horizontal
    int m_iQuickBarExtDlgType; /// 2nd skillbar Vertical, Horizontal
    int m_iChatDlgType; /// Small, Big
    int m_iJapanRoute;
    // short			m_VirtualInventory[ MAX_INV_TYPE ][ INVENTORY_PAGE_SIZE ];
    std::string m_strAvatarName;

    bool m_bSaveLastConnectID;
    std::string m_strLastConnectID;
};
extern CClientStorage g_ClientStorage;
#endif