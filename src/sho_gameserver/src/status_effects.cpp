
#include "stdAFX.h"

#include "status_effects.h"

#include "rose/common/status_effect/status_effect_data.h"
#include "rose/io/stb.h"

using namespace Rose::Common;

bool
StatusEffects::IsTauntSTATUS(int iAttackObjIDX, CZoneTHREAD* pCurZone) {
    if (this->IsSET(c_dwIngFLAG[ING_TAUNT])) {
        CObjCHAR* pDestCHAR = g_pObjMGR->Get_CharOBJ(this->m_iTargetOBJ[ING_TAUNT], true);
        if (pDestCHAR) {
            // Ÿ��Ʈ�� �ǳ��̸� Ÿ��Ʈ ���°� �ƴѰ�����...
            if (this->m_iTargetOBJ[ING_TAUNT] == iAttackObjIDX)
                return false;

            if (pCurZone == pDestCHAR->GetZONE())
                return true;
        }

        // Ÿ���� ���ų� ���� �̵��ߴ�...Ÿ��Ʈ Ǯ������..
        m_nTICKs[ING_TAUNT] = 0;
        return false;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
bool
StatusEffects::IsEnableApplayITEM(short nIngStbIDX) {
    short nTypeIDX = STATE_TYPE(nIngStbIDX);

    // �̹� ����Ǿ� �ִ°� ?
    if (this->IsSET(c_dwIngFLAG[nTypeIDX])) {
        if (0 == STATE_CAN_DUPLICATED(nIngStbIDX)) {
            // ���� ���� �Ұ��� ������ ���...
            return false;
        }

        // ���� �Ǿ� �ִ°� ���� �켱 ������ �ִ°� ?
        if (m_nIngSTBIdx[nTypeIDX] > nIngStbIDX) {
            return false;
        }
    }

    return true;
}
bool
StatusEffects::IsEnableApplay(short nIngStbIDX, short nAdjValue) {
    short nTypeIDX = STATE_TYPE(nIngStbIDX);

    // �̹� ����Ǿ� �ִ°� ?
    if (this->IsSET(c_dwIngFLAG[nTypeIDX])) {
        if (0 == STATE_CAN_DUPLICATED(nIngStbIDX)) {
            // ���� ���� �Ұ��� ������ ���...
            return false;
        }

        if (m_nAdjVALUE[nTypeIDX] > nAdjValue) {
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
DWORD
StatusEffects::UpdateIngSTATUS(CObjCHAR* pCharOBJ,
    short nIngStbIDX,
    short nTick,
    short nAdjVALUE,
    short nSkillIDX,
    int iTargetObjIDX) {
    // �ݵ�� ���� ���ɿ��� �Ǵ����� ȣ��Ǿ�� �Ѵ� !!!
    short nTypeIDX = STATE_TYPE(nIngStbIDX);

    // if ( nTypeIDX <= ING_INC_MP ) {
    //	// ��ų�δ� �������� ����Ǵ� ���¸� �����Ҽ� ������...
    //	return 0;
    //}

    if (nTypeIDX > ING_CHECK_END) {
        switch (nTypeIDX) {
            case ING_CLEAR_INVISIBLE: // ������� ����..
            {
                if (this->IsSET(c_dwIngFLAG[ING_DISGUISE])) {
                    m_nTICKs[ING_DISGUISE] =
                        0; // �ٷ� ���� ���� ���� ���� ó���� ������ ���۵ɼ� �ֵ���..
                }
                if (this->IsSET(c_dwIngFLAG[ING_TRANSPARENT])) {
                    m_nTICKs[ING_TRANSPARENT] =
                        0; // �ٷ� ���� ���� ���� ���� ó���� ������ ���۵ɼ� �ֵ���..
                }
                return 0;
            }
            case ING_CLEAR_GOOD: // ����
            {
                for (short nI = ING_POISONED; nI <= ING_CHECK_END; nI++) {
                    if (this->IsSET(c_dwIngFLAG[nI])
                        && STATE_PRIFITS_LOSSES(m_nIngSTBIdx[nI]) == 0) {
                        m_nTICKs[nI] =
                            0; // �ٷ� ���� ���� ���� ���� ó���� ������ ���۵ɼ� �ֵ���..
                    }
                }
                return 0;
            }
            case ING_CLEAR_BAD: // �Ҹ�
            {
                for (short nI = ING_POISONED; nI <= ING_CHECK_END; nI++) {
                    if (this->IsSET(c_dwIngFLAG[nI])
                        && STATE_PRIFITS_LOSSES(m_nIngSTBIdx[nI]) == 1) {
                        m_nTICKs[nI] =
                            0; // �ٷ� ���� ���� ���� ���� ó���� ������ ���۵ɼ� �ֵ���..
                    }
                }
                if (this->IsSET(FLAG_ING_TAUNT)) {
                    m_nTICKs[ING_TAUNT] = 0;
                }
                return 0;
            }
            case ING_CLEAR_ALL: {
                // ��ü ������ ���൵ �����Ǳ� ������...
                // this->ClearALL( FLAG_ING_CLEAR );		// ���� ���´� ����.
                for (short nI = ING_POISONED; nI <= ING_CHECK_END; nI++) {
                    if (this->IsSET(c_dwIngFLAG[nI])
                        && STATE_PRIFITS_LOSSES(m_nIngSTBIdx[nI]) < 2) {
                        this->ClearSTATUS((eING_TYPE)nI);
                    }
                }
                return 0;
            }
            case ING_DEC_LIFE_TIME:
                this->SetFLAG(c_dwIngFLAG[nTypeIDX]);
                this->m_nIngSTBIdx[nTypeIDX] = nIngStbIDX;
                return 0;
        }

        // Ÿ��Ʈ��...
    }

    this->SetFLAG(c_dwIngFLAG[nTypeIDX]);

    m_nAdjVALUE[nTypeIDX] = nAdjVALUE;
    m_nTICKs[nTypeIDX] = nTick;
    m_nIngSTBIdx[nTypeIDX] = nIngStbIDX;
    m_nIngSKILL[nTypeIDX] = nSkillIDX;
    m_iTargetOBJ[nTypeIDX] = iTargetObjIDX;

    /*
        ����~~~
        if ( c_dwIngFLAG[ nTypeIDX ] & ( FLAG_ING_MAX_HP | FLAG_ING_MAX_MP ) ) {
            // �ִ� HP,MP�� �ö󰡴� ��� ���� ��ġ���� ���� �ø���...	2004. 4. 12
            // ���� MAX_HP�� �÷��� �Ʒ��� ���� �ǰ� ������.
            switch( nTypeIDX ) {
                case ING_INC_MAX_HP :	pCharOBJ->Add_HP( nAdjVALUE );	break;
                case ING_INC_MAX_MP :	pCharOBJ->Add_MP( nAdjVALUE );	break;
            }
        }
    */

    return (c_dwIngFLAG[nTypeIDX] & (FLAG_ING_FAINTING | FLAG_ING_SLEEP));
}

void
StatusEffects::UpdateIngPOTION(short nIngStbIDX, short nTotValue, short nAdjPerSEC) {
    // �ݵ�� ���� ���ɿ��� �Ǵ����� ȣ��Ǿ�� �Ѵ� !!!
    short nTypeIDX = STATE_TYPE(nIngStbIDX);

    if (nTypeIDX > FLAG_ING_MP)
        return;

    this->SetFLAG(c_dwIngFLAG[nTypeIDX]);

    m_nIngSTBIdx[nTypeIDX] = nIngStbIDX;

    m_POTION[nTypeIDX].m_dwTotTIME = (1000 * nTotValue / nAdjPerSEC); // ��ü �ʿ�ð�

    assert(m_POTION[nTypeIDX].m_dwTotTIME);

    m_POTION[nTypeIDX].m_dwAccTIME = 0; // �����ð�.
    m_POTION[nTypeIDX].m_nTotVALUE = nTotValue;
    m_POTION[nTypeIDX].m_nAccVALUE = 0;
}

short
StatusEffects::Proc_IngPOTION(PotionEffect* pPOTION, DWORD dwPassTIME) {
    pPOTION->m_dwAccTIME += dwPassTIME;

    assert(pPOTION->m_dwTotTIME);

    int iCurAccValue =
        floor_int(pPOTION->m_dwAccTIME * pPOTION->m_nTotVALUE / pPOTION->m_dwTotTIME);

    short nDiff;
    if (iCurAccValue >= pPOTION->m_nTotVALUE) {
        // ���볡...
        nDiff = pPOTION->m_nTotVALUE - pPOTION->m_nAccVALUE;
    } else {
        nDiff = iCurAccValue - pPOTION->m_nAccVALUE;
    }
    pPOTION->m_nAccVALUE += nDiff;

    return nDiff;
}

//-------------------------------------------------------------------------------------------------
#define ONE_SECOND 1000
DWORD
StatusEffects::Proc(CObjCHAR* pCharOBJ, DWORD dwPassTIME) {
    DWORD dwChangedFLAG = 0;

    // ���� ó��... �� �����Ӻ�...
    if (this->IsSET(FLAG_ING_HP | FLAG_ING_MP)) {
        short nAdj;
        PotionEffect* pPOTION;
        BYTE btIngEND = 0;
        if (this->IsSET(FLAG_ING_HP)) {
            pPOTION = &m_POTION[ING_INC_HP];
            nAdj = this->Proc_IngPOTION(pPOTION, dwPassTIME);

            // LogString( 0xfff, "HP: %d,  %d/%d,   %d  PassTIME:%d \n", nAdj, pPOTION->m_nAccVALUE,
            // pPOTION->m_nTotVALUE, pCharOBJ->Get_HP(), dwPassTIME );

            if (pCharOBJ->Add_HP(nAdj) >= pCharOBJ->Get_MaxHP()
                || pPOTION->m_nAccVALUE >= pPOTION->m_nTotVALUE) {
                // ó�� �Ϸ� or ����.
                this->ClearStatusFLAG(FLAG_ING_HP);
                dwChangedFLAG |= FLAG_ING_HP;
                btIngEND = 0x01;
            }
        }

        if (this->IsSET(FLAG_ING_MP)) {
            pPOTION = &m_POTION[ING_INC_MP];
            nAdj = this->Proc_IngPOTION(pPOTION, dwPassTIME);

            // LogString( 0xfff, "MP: %d,  %d/%d,   %d  PassTIME:%d \n", nAdj, pPOTION->m_nAccVALUE,
            // pPOTION->m_nTotVALUE, pCharOBJ->Get_MP(), dwPassTIME );

            if (pCharOBJ->Add_MP(nAdj) >= pCharOBJ->Get_MaxMP()
                || pPOTION->m_nAccVALUE >= pPOTION->m_nTotVALUE) {
                // ó�� �Ϸ� or ����.
                this->ClearStatusFLAG(FLAG_ING_MP);
                dwChangedFLAG |= FLAG_ING_MP;
                btIngEND |= 0x02;
            }
        }

        if (btIngEND) {
            pCharOBJ->Send_gsv_SET_HPnMP(btIngEND);
        }
    }

    // 1�ʴ� ó�� �Ұ͵�...
    m_dwAccTIME += dwPassTIME;
    if (m_dwAccTIME < ONE_SECOND)
        return dwChangedFLAG;
    m_dwAccTIME -= ONE_SECOND;

    if (this->IsSET(FLAG_ING_PROC_PER_TICK)) {
        // �ߵ�...
        if (this->IsSET(FLAG_ING_POISONED)) {
            if (m_nTICKs[ING_POISONED]-- > 0) {
                // dec hp
                if (pCharOBJ->Get_HP() > 0) {
                    short nValue = STATE_APPLY_ABILITY_VALUE(m_nIngSTBIdx[ING_POISONED], 0);

                    if (pCharOBJ->Get_HP() > nValue)
                        pCharOBJ->Sub_HP(nValue);
                    else
                        pCharOBJ->Set_HP(1);
                }
            } else {
                this->ClearStatusFLAG(FLAG_ING_POISONED);
                dwChangedFLAG |= FLAG_ING_POISONED;
            }
        }

        // ��ȯ�� ���� ���� �ð�....
        if (this->IsSET(FLAG_ING_DEC_LIFE_TIME)) {
            if (pCharOBJ->Get_HP() > 0) {
                short nValue = STATE_APPLY_ABILITY_VALUE(m_nIngSTBIdx[ING_DEC_LIFE_TIME], 0);

                if (pCharOBJ->Get_HP() > nValue) {
                    pCharOBJ->Sub_HP(nValue);
                } else {
                    // �ڻ� ��Ų��...
                    pCharOBJ->Add_DAMAGE(nValue);
                    return 0;
                }
            }
        }
    }

    // ��Ʈ �÷��� ����
    if (this->IsSET(FLAG_ING_CHECK_PER_TICK)) {
        // �ִ� HP
        if (this->IsSET(FLAG_ING_MAX_HP)) {
            if (m_nTICKs[ING_INC_MAX_HP]-- <= 0) {
                this->ClearSTATUS(ING_INC_MAX_HP);
                pCharOBJ->Add_HP(0);
                dwChangedFLAG |= FLAG_ING_MAX_HP;
            }
        }

        // �ִ� MP
        if (this->IsSET(FLAG_ING_MAX_MP)) {
            if (m_nTICKs[ING_INC_MAX_MP]-- <= 0) {
                this->ClearSTATUS(ING_INC_MAX_MP);
                pCharOBJ->Add_MP(0);
                dwChangedFLAG |= FLAG_ING_MAX_MP;
            }
        }

        // ���� �ð� üũ..
        for (short nI = ING_CHECK_START; nI <= ING_CHECK_END; nI++) {
            if (!this->IsSET(c_dwIngFLAG[nI]))
                continue;

            if (m_nTICKs[nI]-- <= 0) {
                // �ð��� �Ϸ� �Ǿ�����...
                this->ClearSTATUS((eING_TYPE)nI);
                dwChangedFLAG |= c_dwIngFLAG[nI];
            }
        }

        // Ÿ��Ʈ..
        if (this->IsSET(FLAG_ING_TAUNT)) {
            if (m_nTICKs[ING_TAUNT]-- <= 0) {
                this->m_iTargetOBJ[ING_TAUNT] = 0;
                this->ClearSTATUS(ING_TAUNT);
                dwChangedFLAG |= FLAG_ING_TAUNT;
            }
        }

        // ��Ȱ
        if (this->IsSET(FLAG_ING_REVIVE)) {
            if (m_nTICKs[ING_REVIVE]-- <= 0) {
                this->ClearSTATUS(ING_REVIVE);
                dwChangedFLAG |= FLAG_ING_REVIVE;
            }
        }
    }

    // TODO: RAM: This doesn't need to be updated per tick. Update conditions:
    //	- Update on level up
    //	- Update on create
    if (this->is_enabled(StatusEffectType::Goddess)) {
        this->goddess_effect.update(pCharOBJ->Get_LEVEL());
    }

    return dwChangedFLAG;
}

short
StatusEffects::Adj_RUN_SPEED() {
    short val = m_nAdjVALUE[ING_INC_MOV_SPD] - m_nAdjVALUE[ING_DEC_MOV_SPD] + m_nAruaRunSPD;

    // Goddess buff doesn't stack with other buffs
    if (is_enabled(StatusEffectType::Goddess)) {
        val += max(0, goddess_effect.move_speed - m_nAdjVALUE[ING_INC_MOV_SPD]);
    }

    return val;
}

short
StatusEffects::Adj_ATK_SPEED() {
    short val = m_nAdjVALUE[ING_INC_ATK_SPD] - m_nAdjVALUE[ING_DEC_ATK_SPD];

    // Goddess buff doesn't stack with other buffs
    if (is_enabled(StatusEffectType::Goddess)) {
        val += max(0, goddess_effect.attack_speed - m_nAdjVALUE[ING_INC_ATK_SPD]);
    }

    return val;
}

short
StatusEffects::Adj_APOWER() {
    short val = m_nAdjVALUE[ING_INC_APOWER] - m_nAdjVALUE[ING_DEC_APOWER] + m_nAruaATK;

    // Goddess buff doesn't stack with other buffs
    if (is_enabled(StatusEffectType::Goddess)) {
        val += max(0, goddess_effect.attack_power - m_nAdjVALUE[ING_INC_APOWER]);
    }

    return val;
}

short
StatusEffects::Adj_DPOWER() {
    return m_nAdjVALUE[ING_INC_DPOWER] - m_nAdjVALUE[ING_DEC_DPOWER] + m_nAruaRES;
}

short
StatusEffects::Adj_RES() {
    return m_nAdjVALUE[ING_INC_RES] - m_nAdjVALUE[ING_DEC_RES];
}

short
StatusEffects::Adj_HIT() {
    short val = m_nAdjVALUE[ING_INC_HIT] - m_nAdjVALUE[ING_DEC_HIT];

    // Goddess buff doesn't stack with other buffs
    if (is_enabled(StatusEffectType::Goddess)) {
        val += max(0, goddess_effect.hit - m_nAdjVALUE[ING_INC_HIT]);
    }

    return val;
}

short
StatusEffects::Adj_CRITICAL() {
    short val = m_nAdjVALUE[ING_INC_CRITICAL] - m_nAdjVALUE[ING_DEC_CRITICAL] + m_nAruaCRITICAL;

    // Goddess buff doesn't stack with other buffs
    if (is_enabled(StatusEffectType::Goddess)) {
        val += max(0, goddess_effect.crit - m_nAdjVALUE[ING_INC_CRITICAL]);
    }

    return val;
}

short
StatusEffects::Adj_AVOID() {
    return m_nAdjVALUE[ING_INC_AVOID] - m_nAdjVALUE[ING_DEC_AVOID];
}

void
StatusEffects::enable_status(StatusEffectType type) {
    StatusEffectFlag flag = set2sef(type);
    is_sub_type(type) ? set_sub_flag(flag) : set_flag(flag);
}

void
StatusEffects::disable_status(StatusEffectType type) {
    StatusEffectFlag flag = set2sef(type);
    is_sub_type(type) ? unset_sub_flag(flag) : unset_flag(flag);
}

bool
StatusEffects::is_enabled(StatusEffectType type) {
    StatusEffectFlag flag = set2sef(type);
    if (is_sub_type(type)) {
        return is_sub_flag_set(flag);
    }

    return is_flag_set(flag);
}

void
StatusEffects::set_flag(StatusEffectFlag flag) {
    m_dwIngStatusFLAG |= static_cast<uint32_t>(flag);
}

void
StatusEffects::unset_flag(StatusEffectFlag flag) {
    m_dwIngStatusFLAG &= ~static_cast<uint32_t>(flag);
}

bool
StatusEffects::is_flag_set(StatusEffectFlag flag) {
    return (m_dwIngStatusFLAG & static_cast<uint32_t>(flag));
}

void
StatusEffects::set_sub_flag(StatusEffectFlag flag) {
    m_dwSubStatusFLAG |= static_cast<uint32_t>(flag);
}

void
StatusEffects::unset_sub_flag(StatusEffectFlag flag) {
    m_dwSubStatusFLAG &= ~static_cast<uint32_t>(flag);
}

bool
StatusEffects::is_sub_flag_set(StatusEffectFlag flag) {
    return (m_dwSubStatusFLAG & static_cast<uint32_t>(flag));
}

bool
StatusEffects::is_sub_flag(StatusEffectFlag flag) {
    switch (flag) {
        case StatusEffectFlag::Hide:
        case StatusEffectFlag::StoreMode:
        case StatusEffectFlag::IntroChat:
        case StatusEffectFlag::Goddess:
        case StatusEffectFlag::AruaFairy:
        case StatusEffectFlag::Invincible:
            return true;
        default:
            return false;
    }
    return false;
}

bool
StatusEffects::is_sub_type(Rose::Common::StatusEffectType type) {
    switch (type) {
        case StatusEffectType::Hide:
        case StatusEffectType::StoreMode:
        case StatusEffectType::IntroChat:
        case StatusEffectType::Goddess:
        case StatusEffectType::AruaFairy:
        case StatusEffectType::Invincible:
            return true;
        default:
            return false;
    }
    return false;
}