#include "stdAFX.h"
#include "IO_Skill.h"
#ifndef	__SERVER
	#include "..\util\CGameSTB.h"
#else
	#include "..\util\classSTB.h"
#include "..\GameProc\UseItemDelay.h"
#endif


CSkillLIST g_SkillList;

//-------------------------------------------------------------------------------------------------
CSkillLIST::CSkillLIST ()
{
	m_iSkillCount = 0;

	m_pCastingAniSPEED = NULL;
	m_pActionAniSPEED = NULL;

	m_pReloadTIME = NULL;
}

CSkillLIST::~CSkillLIST ()
{
	this->Free ();
}

void CSkillLIST::Free ()
{
	m_SkillDATA.Free ();
	SAFE_DELETE_ARRAY( m_pCastingAniSPEED );
	SAFE_DELETE_ARRAY( m_pActionAniSPEED );

	SAFE_DELETE_ARRAY( m_pReloadTIME );
}

bool CSkillLIST::LoadSkillTable(const char* pFileName)
{
	if ( !m_SkillDATA.Load( (char*)pFileName, true, true) )
		return false;

	m_pCastingAniSPEED = new float[ m_SkillDATA.m_nDataCnt ];
	m_pActionAniSPEED = new float[ m_SkillDATA.m_nDataCnt ];

	m_pReloadTIME = new float[ m_SkillDATA.m_nDataCnt ];
	::ZeroMemory( m_pReloadTIME, sizeof(float) * m_SkillDATA.m_nDataCnt );

	for (short nI=1; nI<m_SkillDATA.m_nDataCnt; nI++) {
		if ( SKILL_ANI_CASTING_REPEAT_CNT(nI) ) {
			if ( SKILL_ANI_CASTING_REPEAT(nI ) < 1 ) {
				SKILL_ANI_CASTING_REPEAT_CNT(nI) = 0;
			}
		}
		
		m_pCastingAniSPEED[ nI ] = SKILL_ANI_CASTING_SPEED(nI) / 100.f;

		m_pReloadTIME[ nI ] = ((float)SKILL_RELOAD_TIME(nI) / 5.0f) * 1000.0f;

		 if ( m_pReloadTIME[ nI ] <= 0.f )
				m_pReloadTIME[ nI ] = 1.0f;	

		if ( m_pCastingAniSPEED[ nI ] <= 0.f ) {
			m_pCastingAniSPEED[ nI ] = 1.0f;		// 디폴드로 만들어 버림...
		}

		m_pActionAniSPEED[ nI ] = SKILL_ANI_ACTION_SPEED(nI) / 100.f;
		if ( m_pActionAniSPEED[ nI ] <= 0.f ) {
			m_pActionAniSPEED[ nI ] = 1.0f;
		}
	}

	m_iSkillCount = m_SkillDATA.m_nDataCnt;

	return true;
}

float CSkillLIST::GetDelayTickCount(DWORD i)
{

	if((i < 0) || (i >= m_SkillDATA.m_nDataCnt))
		return 1.0f;

	return m_pReloadTIME[ i ];

}

