
#include "stdAFX.h"

#include "IO_PAT.h"

CPatITEM g_PatITEM;

//-------------------------------------------------------------------------------------------------
CPatITEM::CPatITEM() {}
CPatITEM::~CPatITEM() {}

//-------------------------------------------------------------------------------------------------
// 2004/9/10일 nAvy(수정) - client는 Desc를 false로 Load하고 있었다.
bool
CPatITEM::LoadPatITEM(const char* szFileName) {

    if (!m_ItemDATA.Load((char*)szFileName, true, true))
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------------
void
CPatITEM::Free() {
    m_ItemDATA.Free();
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
