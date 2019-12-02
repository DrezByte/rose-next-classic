#ifndef	STATUS_EFFECTS_H
#define	STATUS_EFFECTS_H
#pragma once

#define	FLAG_CHEAT_INVINCIBLE 0x80000000
#define	IDX_ING_STB_DEC_LIFE_TIME 43

class CObjCHAR;

// TODO: Complete and use this enum
enum StatusEffectFlag {};

// TODO: Use this enum
enum class StatusEffectType: unsigned int {
	Null,

	// HP/MP status effects
	HPMPStart,
	IncreaseHP = HPMPStart,
	IncreaseMP,
	Poisoned,
	IncreaseMaxHP,
	IncreaseMaxMP,
	HPMPEnd = IncreaseMaxMP,

	// General status effects
	GeneralStart,
	IncreaseMoveSpeed = GeneralStart,
	DecreaseMoveSpeed,
	IncreaseAttackSpeed,
	DecreaseAttackSpeed,
	IncreaseAttackPower,
	DecreaseAttackPower,
	IncreaseDefense,
	DecreaseDefense,
	IncreaseMagicRes,
	DecreaseMagicRes,
	IncreaseHit,
	DecreaseHit,
	IncreaseCrit,
	DecreaseCrit,
	IncreaseDodge,
	DecreaseDodge,
	Dumb,
	Sleep,
	Faint,
	Disguise,
	Invisible,
	ShieldDamage,
	AdditionalDamage,
	GeneralEnd = AdditionalDamage,

	DecreaseLifetime, // TODO: What does this do?

	ClearGood,
	ClearBad,
	ClearAll,
	ClearInvisible,

	Taunt,
	Revive,

	Max
};

// TODO: Use this class
struct StatusEffect {
	short id;
	short value;
	short skill_id;
	short ticks;
	int target_id;
};

struct PotionEffect {
	DWORD m_dwTotTIME;
	DWORD m_dwAccTIME;
	short m_nAccVALUE;
	short m_nTotVALUE;
};

/// Avatar Status Effects
///
/// This object contains information about buffs/debuffs/effects/etc. on the avatar.
#pragma pack (push, 1)
class StatusEffects {
public:
	union {
		short		m_nAruaVALUE[9];
		struct {
			short	m_nAruaAtkSPD;
			short	m_nAruaRunSPD;
			short	m_nAruaCRITICAL;
			short	m_nAruaMaxHP;
			short	m_nAruaMaxMP;
			short	m_nAruaRcvHP;
			short	m_nAruaRcvMP;
			short	m_nAruaRES;
			short	m_nAruaATK;
		};
	};

	DWORD m_dwSubStatusFLAG;

public :
	short Inc_MAX_HP() {
		return m_nAdjVALUE[ ING_INC_MAX_HP ] + m_nAruaMaxHP;
	}

	short Inc_MAX_MP() { 
		return m_nAdjVALUE[ ING_INC_MAX_MP ] + m_nAruaMaxMP;
	}

	short Inc_RUN_SPEED() {
		return m_nAdjVALUE[ ING_INC_MOV_SPD ];
	}

	short Dec_RUN_SPEED() {
		return m_nAdjVALUE[ ING_DEC_MOV_SPD ];
	}

	short Inc_ATK_SPEED() {
		return m_nAdjVALUE[ ING_INC_ATK_SPD ];
	}

	short Dec_ATK_SPEED() {
		return m_nAdjVALUE[ ING_DEC_ATK_SPD ];
	}

	short Adj_RUN_SPEED() {	
		return m_nAdjVALUE[ ING_INC_MOV_SPD ] - m_nAdjVALUE[ ING_DEC_MOV_SPD ] + m_nAruaRunSPD;
	}

	short Adj_ATK_SPEED() {
		return m_nAdjVALUE[ ING_INC_ATK_SPD ] - m_nAdjVALUE[ ING_DEC_ATK_SPD ];
	}

	short Adj_APOWER() {
		return m_nAdjVALUE[ ING_INC_APOWER ] - m_nAdjVALUE[ ING_DEC_APOWER ] + m_nAruaATK;
	}

	short Adj_DPOWER() {
		return m_nAdjVALUE[ ING_INC_DPOWER ] - m_nAdjVALUE[ ING_DEC_DPOWER ] + m_nAruaRES;
	}

	short Adj_RES() {
		return m_nAdjVALUE[ ING_INC_RES ] - m_nAdjVALUE[ ING_DEC_RES ];
	}

	short Adj_HIT() {
		return m_nAdjVALUE[ ING_INC_HIT ] - m_nAdjVALUE[ ING_DEC_HIT ];
	}

	short Adj_CRITICAL() {
		return m_nAdjVALUE[ ING_INC_CRITICAL] - m_nAdjVALUE[ ING_DEC_CRITICAL	] + m_nAruaCRITICAL;
	}

	short Adj_AVOID() {
		return m_nAdjVALUE[ ING_INC_AVOID	] - m_nAdjVALUE[ ING_DEC_AVOID ];
	}

	void SetSubFLAG( DWORD dwFLAG ) {
		m_dwSubStatusFLAG |= dwFLAG;
	}

	void ClearSubFLAG( DWORD dwFLAG ){
		m_dwSubStatusFLAG &= ~dwFLAG;
	}

	DWORD IsSubSET( DWORD dwCheckFLAG ){
		return ( m_dwSubStatusFLAG & dwCheckFLAG );
	}

	void ToggleSubFLAG( DWORD dwFLAG )
	{
		if ( this->IsSubSET( dwFLAG ) )
			this->ClearSubFLAG( dwFLAG );
		else
			this->SetSubFLAG( dwFLAG );
	}

	void DelArua()
	{
		::ZeroMemory( m_nAruaVALUE, sizeof(m_nAruaVALUE) );
		this->ClearSubFLAG( FLAG_SUB_ARUA_FAIRY );
	}

	void Reset( bool bFirst )
	{
		if ( bFirst ) {
			m_dwSubStatusFLAG = 0;
			::ZeroMemory( m_nAruaVALUE, sizeof(m_nAruaVALUE) );
		} else
			m_dwSubStatusFLAG &= FLAG_SUB_ARUA_FAIRY;

		m_dwAccTIME = 0;
		m_dwIngStatusFLAG = 0;
		::ZeroMemory( m_nAdjVALUE, sizeof(short) * ING_MAX_STATUS );
	}

	void ClearALL( DWORD dwAndMask=0 )
	{
		m_dwAccTIME = 0;
		m_dwIngStatusFLAG &= dwAndMask;
		::ZeroMemory( m_nAdjVALUE, sizeof(short) * ING_MAX_STATUS );
	}

	void ClearAllGOOD()
	{
		for (short nI=ING_POISONED; nI<=ING_CHECK_END; nI++) {
			if ( ( FLAG_ING_GOOD & c_dwIngFLAG[ nI ] ) && this->IsSET( c_dwIngFLAG[ nI ] ) ) {
				this->m_nAdjVALUE[ nI ] = 0;
			}
		}

		this->ClearStatusFLAG( FLAG_ING_GOOD );
	}

	DWORD IsSET( DWORD dwCheckFLAG ) {
		return ( m_dwIngStatusFLAG & dwCheckFLAG );
	}

	DWORD GetFLAGs() {
		return m_dwIngStatusFLAG;
	}

	short GetSkillIDX( eING_TYPE eTYPE ) { 
		return m_nIngSKILL[ eTYPE ]; 
	}

	void SetFLAG( DWORD dwIngFLAG ) {
		m_dwIngStatusFLAG |= dwIngFLAG;
	}

	void ClearStatusFLAG( DWORD dwIngFLAG ) {
		m_dwIngStatusFLAG &= ~dwIngFLAG;
	}

	void ClearSTATUS( eING_TYPE IngType )
	{
		this->ClearStatusFLAG( c_dwIngFLAG[ IngType ] );
		m_nAdjVALUE[ IngType ] = 0;
	}

	void ClearStatusIfENABLED( eING_TYPE IngType )
	{
		if ( this->IsSET( c_dwIngFLAG[ IngType ] ) && STATE_PRIFITS_LOSSES( m_nIngSTBIdx[IngType] ) < 2 ) {
			this->ClearSTATUS( IngType );
		}
	}

	void ExpireSTATUS( eING_TYPE IngType )	{	m_nTICKs[ IngType ] = 0;	}

	bool IsTauntSTATUS( int iAttackObjIDX, CZoneTHREAD *pCurZone );
	bool IsIgnoreSTATUS ()
	{
		if ( this->IsSET( FLAG_ING_SLEEP | FLAG_ING_FAINTING ) )
			return true;
		if ( this->IsSubSET( FLAG_SUB_STORE_MODE ) )
			return true;
		return false;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool IsEnableApplay( short nIngStbIDX, short nAdjValue );
	bool IsEnableApplayITEM( short nIngStbIDX );

	DWORD UpdateIngSTATUS( CObjCHAR *pCharOBJ, short nIngStbIDX, short nTick, short nAdjVALUE, short nSkillIDX, int iTargetObjIDX=0 );
	void UpdateIngPOTION( short nIngStbIDX, short nTotValue, short nAdjPerSEC );

	DWORD Proc (CObjCHAR *pCharOBJ, DWORD dwPassTIME);

private:
	short Proc_IngPOTION(PotionEffect* pPOTION, DWORD dwPassTIME);

private:
	DWORD m_dwIngStatusFLAG;
	DWORD m_dwAccTIME;

	PotionEffect m_POTION[FLAG_ING_MP + 1];

	int m_iTargetOBJ[ING_MAX_STATUS];
	short m_nTICKs[ING_MAX_STATUS];
	short m_nIngSKILL[ING_MAX_STATUS];
	short m_nAdjVALUE[ING_MAX_STATUS];
	short m_nIngSTBIdx[ING_MAX_STATUS];

};
#pragma pack (pop)


#endif // STATUS_EFFECTS_H