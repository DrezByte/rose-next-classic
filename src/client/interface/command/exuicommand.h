#ifndef _EXUICOMMAND_
#define _EXUICOMMAND_
#include "tgamectrl/tcommand.h"
#include <string>

/// ���Ӽ���â���� ĳ���� ������û
class CTCmdDeleteCharacter: public CTCommand {
public:
    CTCmdDeleteCharacter(int iSelectListID, const char* pszName);
    virtual ~CTCmdDeleteCharacter(void) {}
    virtual bool Exec(CTObject* pObj);

protected:
    int m_iSelectListID;
    std::string m_strName;
};

/// ���Ӽ���â���� ���� ������� ĳ���� �ǻ츮�� ��û
class CTCmdReviveCharacter: public CTCommand {
public:
    CTCmdReviveCharacter(int iSelectListID, const char* pszName);
    virtual ~CTCmdReviveCharacter(void) {}
    virtual bool Exec(CTObject* pObj);

protected:
    int m_iSelectListID;
    std::string m_strName;
};

class CTCmdCreateAvatar : public CTCommand {
public:
    CTCmdCreateAvatar(const std::string& name, int gender_id, int hair_id, int face_id, int job_id);
    virtual ~CTCmdCreateAvatar(void) {}
    virtual bool Exec(CTObject* object);

protected:
    std::string name;
    int gender_id;
    int hair_id;
    int face_id;
    int job_id;
};
#endif