#ifndef _CREATEAVATA_
#define _CREATEAVATA_

#include "CExternalUI.h"

#include "Net_Prototype.h"
#include "tgamectrl/tdialog.h"
#include <string>
#include <deque>

struct CreateCharacterInfo {
    int gender_id;
    int birthstone_id;
    int hair_id;
    int face_id;
    int job_id;
};

///
/// create specific avata
///
struct AvatarSelectItem {
    std::string m_strValue;
    std::string m_strIdentify;
};

/**
 * ĳ���� ������ ���Ǵ� �������̽�
 *
 *
 * @Warning		��ũ��Ʈ�� ���� �����Ѵ�.( SystemFunc.lua )
 * @Author		������
 * @Date			2005/9/6
 *
 */
class CCreateAvata: public CTDialog {
public:
    CExternalUIManager* m_pEUIManager;

    /// ��ũ��Ʈ�󿡼� �߰��Ǿ� ����.
    std::deque<AvatarSelectItem> m_mapSex; /// ĳ���ͻ����� �����Ҽ� �ִ� ����
    std::deque<AvatarSelectItem> m_mapFace; /// ĳ���ͻ����� �����Ҽ� �ִ� ����
    std::deque<AvatarSelectItem> m_mapHair; /// ĳ���ͻ����� �����Ҽ� �ִ� �Ӹ����
    std::deque<AvatarSelectItem> m_mapJob; /// ������ġ

    int m_iSelectedSex; /// ������ ����
    int m_iSelectedFace; /// ������ ��
    int m_iSelectedHair; /// ������ �Ӹ����
    int m_iSelectedJob; /// ������ġ
public:
    CCreateAvata();
    ~CCreateAvata();

    ///
    /// set observer class
    ///
    void SetEUIObserver(CExternalUIManager* pObserver);
    ///
    /// overrided function
    ///
    virtual UINT Process(UINT uiMsg, WPARAM wParam, LPARAM lParam);
    ///
    /// overrided function
    ///

    virtual void Show();
    virtual void Hide();
    virtual void Draw();

    void FailCreateAvata();
    ///
    /// Packet receive proc
    ///
    bool RecvCreateAvata(t_PACKET* recvPacket);

    void MsgBoxProc(unsigned int iMsgType, short int iButton);

    void ClearSelectItem(int iType);
    void AddSelectItem(int iType, const char* pszIdentify, const char* pszValue);
    void SelectItem(int iType, unsigned iSubscript);

    enum {
        IID_EDITBOX = 7,
        IID_BTN_OK = 10,
        IID_BTN_CANCEL = 11,
        IID_BTN_LEFT_SEX = 20,
        IID_BTN_LEFT_FACE = 21,
        IID_BTN_LEFT_HAIR = 22,
        IID_BTN_LEFT_JOB = 23,

        IID_BTN_RIGHT_SEX = 30,
        IID_BTN_RIGHT_FACE = 31,
        IID_BTN_RIGHT_HAIR = 32,
        IID_BTN_RIGHT_JOB = 33,
    };

    enum { MSG_TYPE_NORMAL };

protected:
    void OnLButtonUp(unsigned iProcID);
    void CreateAvata(const std::string& name,
        int iSex,
        int iHair,
        int iFace,
        int iJob);
};

#endif //_CREATEAVATA_