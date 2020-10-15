#include "stdafx.h"

#include ".\tutorialeventutility.h"
#include "../System/SystemProcScript.h"
#include "../Object.h"

CTutorialEventUtility _TutorialEventUtility;

CTutorialEventUtility::CTutorialEventUtility(void) {}

CTutorialEventUtility::~CTutorialEventUtility(void) {}

bool
CTutorialEventUtility::Init() {
    /// Load notify button table
    bool loaded = CVFSManager::GetSingleton().load_stb(m_TblNotifyButtonEvent, EVENT_BUTTON_STB);
    if (!loaded)
        return false;

    return true;
}

void
CTutorialEventUtility::Release() {
}

/// �˸� ��ư �̺�Ʈ�� �����Ѵ�.
bool
CTutorialEventUtility::CreateNotifyEventButton(int iEventIdx) {
    if (iEventIdx <= 0)
        return false;

    if (iEventIdx >= m_TblNotifyButtonEvent.row_count)
        return false;

    g_itMGR.AddNotifybutton(iEventIdx);

    return true;
}

//-------------------------------------------------------------------------------------------
/// EventButton.STB�� ������ �˸� ��ư �̺�Ʈ�� �����Ѵ�.
//-------------------------------------------------------------------------------------------
bool
CTutorialEventUtility::ExecNotifyEvent(int iEventIdx) {
    if (iEventIdx <= 0)
        return false;

    if (iEventIdx >= m_TblNotifyButtonEvent.row_count)
        return false;
    

    CSystemProcScript::GetSingleton().CallLuaFunction(m_TblNotifyButtonEvent.get_cstr(iEventIdx, m_TblNotifyButtonEvent.col_count - 1),
        ZZ_PARAM_INT,
        g_pAVATAR->Get_INDEX(),
        ZZ_PARAM_END);

    return true;
}