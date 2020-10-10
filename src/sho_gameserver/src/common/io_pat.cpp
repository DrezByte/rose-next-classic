
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
#ifdef CLIENT
    return CVFSManager::GetSingleton().load_stb(m_ItemDATA, szFileName);
#else
    return m_ItemDATA.load(szFileName);
#endif
}

//-------------------------------------------------------------------------------------------------
void
CPatITEM::Free() {
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
