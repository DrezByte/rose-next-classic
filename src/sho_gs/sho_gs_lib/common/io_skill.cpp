#include "stdAFX.h"
#include "IO_Skill.h"
#ifndef	__SERVER
	#include "..\util\CGameSTB.h"
#else
	#include "..\util\classSTB.h"
#endif


CSkillLIST g_SkillList;

//-------------------------------------------------------------------------------------------------
CSkillLIST::CSkillLIST ()
{
	m_iSkillCount = 0;

	m_pCastingAniSPEED = NULL;
	m_pActionAniSPEED = NULL;

#ifdef	__SERVER
	m_pReloadTIME = NULL;
#endif
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

#ifdef	__SERVER
	SAFE_DELETE_ARRAY( m_pReloadTIME );
#endif
}

bool CSkillLIST::LoadSkillTable(const char* pFileName)
{
	if ( !m_SkillDATA.Load( (char*)pFileName, true, true) )
		return false;
//	assert( m_SkillDATA.m_nColCnt > 88 );

	m_pCastingAniSPEED = new float[ m_SkillDATA.m_nDataCnt ];
	m_pActionAniSPEED = new float[ m_SkillDATA.m_nDataCnt ];

#ifdef	__SERVER
	m_pReloadTIME = new DWORD[ m_SkillDATA.m_nDataCnt ];
	::ZeroMemory( m_pReloadTIME, sizeof(DWORD) * m_SkillDATA.m_nDataCnt );
#endif

	// 0번 스킬 사용안함...
	for (short nI=1; nI<m_SkillDATA.m_nDataCnt; nI++) {
		if ( SKILL_ANI_CASTING_REPEAT_CNT(nI) ) {
			if ( SKILL_ANI_CASTING_REPEAT(nI ) < 1 ) {
				SKILL_ANI_CASTING_REPEAT_CNT(nI) = 0;
			}
		}

		if ( SKILL_ANI_CASTING_REPEAT_CNT(nI) ) {
			tagMOTION *pMOTION = NULL;
			float fSpeed;
			DWORD dwTotTime;
			WORD wTotFrame;
			short nMotionTYPE=0;
			// 반복 모션
			for (short nM=0; nM<g_TblAniTYPE.m_nColCnt; nM++) {
				if ( FILE_MOTION( nM, SKILL_ANI_CASTING_REPEAT(nI) ) ) {
					nMotionTYPE = nM;
					break;
				}
			}

			pMOTION = g_MotionFILE.IDX_GetMOTION( FILE_MOTION( nMotionTYPE, SKILL_ANI_CASTING_REPEAT(nI) ), 0 );
			wTotFrame = pMOTION->Get_TotalFRAME () * SKILL_ANI_CASTING_REPEAT_CNT(nI);

			// 모션 속도
			fSpeed = SKILL_ANI_CASTING_SPEED(nI) / 100.f;

			// 총 반복시 소요될 시간...
			dwTotTime = pMOTION->Get_NeedTIME( wTotFrame, fSpeed );

			// 케스팅 모션
			pMOTION = g_MotionFILE.IDX_GetMOTION( FILE_MOTION( nMotionTYPE, SKILL_ANI_CASTING(nI) ) , 0 );
			wTotFrame = pMOTION->Get_TotalFRAME ();

			// 전체 소요될 시간..
			dwTotTime += pMOTION->Get_NeedTIME( wTotFrame, fSpeed );

			m_pCastingAniSPEED[ nI ] = ( 1000.f * wTotFrame ) / ( dwTotTime * pMOTION->m_wFPS );
		} else {
			m_pCastingAniSPEED[ nI ] = SKILL_ANI_CASTING_SPEED(nI) / 100.f;
		}

		// 스킬 반복가능 딜레이 시간( 단위 1당 0.2초 )
		m_pReloadTIME[ nI ] = SKILL_RELOAD_TIME(nI) * 200 - 100;	// 0.2f * 1000 :: 0.1초 유예~;

		if ( m_pCastingAniSPEED[ nI ] <= 0.f ) {
			m_pCastingAniSPEED[ nI ] = 1.0f;		// 디폴드로 만들어 버림...
		}


		if ( SKILL_RELOAD_TYPE(nI) < 0 ) 
			SKILL_RELOAD_TYPE(nI) = 0;
		else
		if ( SKILL_RELOAD_TYPE(nI) >= MAX_SKILL_RELOAD_TYPE )
			SKILL_RELOAD_TYPE(nI) = 0;

		m_pActionAniSPEED[ nI ] = SKILL_ANI_ACTION_SPEED(nI) / 100.f;
		if ( m_pActionAniSPEED[ nI ] <= 0.f ) {
			m_pActionAniSPEED[ nI ] = 1.0f;
		}
	}

	m_iSkillCount = m_SkillDATA.m_nDataCnt;

	return true;
}

//-------------------------------------------------------------------------------------------------
