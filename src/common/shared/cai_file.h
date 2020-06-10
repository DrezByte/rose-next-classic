#pragma once

#include "cai_lib.h"

#pragma pack() // default

//-------------------------------------------------------------------------------------------------

#define AIG_NAME_MAX 32
typedef DWORD AITYPE;

#define AICOND_GENERAL 0x04000000 // �Ϲ����� ���ǵ�

#define AICOND_00 (0x000000001 | AICOND_GENERAL) // ����-00 , �ڽ��� ��������/����� �ϰ��
#define AICOND_01 (0x000000002 | AICOND_GENERAL)
#define AICOND_02 (0x000000003 | AICOND_GENERAL)
#define AICOND_03 (0x000000004 | AICOND_GENERAL)
#define AICOND_04 (0x000000005 | AICOND_GENERAL)
#define AICOND_05 (0x000000006 | AICOND_GENERAL)
#define AICOND_06 (0x000000007 | AICOND_GENERAL)
#define AICOND_07 (0x000000008 | AICOND_GENERAL)
#define AICOND_08 (0x000000009 | AICOND_GENERAL)
#define AICOND_09 (0x00000000A | AICOND_GENERAL)
#define AICOND_10 (0x00000000B | AICOND_GENERAL)
#define AICOND_11 (0x00000000C | AICOND_GENERAL)

/// ������ ���ΰ� / ���ΰ� ?
#define AICOND_12 (0x00000000D | AICOND_GENERAL)
/// �ڽ�/Ÿ�ٿ��� �ɷ��ִ� ���� üũ.
#define AICOND_13 (0x00000000E | AICOND_GENERAL)
// NPC ���� üũ
#define AICOND_14 (0x00000000F | AICOND_GENERAL)
#define AICOND_15 (0x000000010 | AICOND_GENERAL)
#define AICOND_16 (0x000000011 | AICOND_GENERAL)

/// NPC ����
#define AICOND_17 (0x000000012 | AICOND_GENERAL)
/// �ڽŰ� ����ĳ����(�ڽ��� ��ȯ�� �ƹ�Ÿ)���� �Ÿ� üũ
#define AICOND_18 (0x000000013 | AICOND_GENERAL)

/// ���� ���ð� üũ
#define AICOND_19 (0x000000014 | AICOND_GENERAL)

/// �ڽ��� �ɷ�ġ üũ
#define AICOND_20 (0x000000015 | AICOND_GENERAL)
// ���� ĳ���� ���� ���� Ȯ��.
#define AICOND_21 (0x000000016 | AICOND_GENERAL)
// ���� ĳ���Ͱ� ���� ����Ÿ���� ������ �ִ°�
#define AICOND_22 (0x000000017 | AICOND_GENERAL)
// ���� �ð� üũ
#define AICOND_23 (0x000000018 | AICOND_GENERAL)

/// ��¥ + �ð� üũ
#define AICOND_24 (0x000000019 | AICOND_GENERAL)
/// ���� + �ð� üũ
#define AICOND_25 (0x00000001A | AICOND_GENERAL)
/// ä�� ���� üũ
#define AICOND_26 (0x00000001B | AICOND_GENERAL)
// �ֺ�ĳ���� ��Ȳüũ �� ��� "�̻�/����" �ΰ� �߰�
#define AICOND_27 (0x00000001C | AICOND_GENERAL)
// Monster ���� üũ ( 0 ~ 4 �� )
#define AICOND_28 (0x00000001D | AICOND_GENERAL)

//
// Ŭ��
//
// Ŭ�������� üũ
#define AICOND_29 (0x00000001E | AICOND_GENERAL)

///////////////////////////////////////////////////////////////////////////////////////////////////
// ���� ���
// ���� ��� �ٷ� ������ ���� ������ ��
struct AI_FILE_HEADER {
    int iNumOfPattern;
    int iSecond; // ���������϶� üũ �ʴ���
    int iSecondOfAttackMove; // Ÿ�ݽ� üũȮ��
    int iNumOfTitle;
};

//======================================================================================
// Structure Name : stEvent
//======================================================================================
struct stEvent {
    char szEventName[AIG_NAME_MAX];
    int iNumOfCond;
    //	std::vector<stCond *> vecpCond ;
    int iNumOfAct;
    //	std::vector<stAct *> vecpAct ;
};

//============================================================================
// struct stPattern
//============================================================================
struct stPattern {
    char szName[AIG_NAME_MAX];
    int iNumOfEvent;
    //	std::vector<stEvent *>		vecEvent ;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_00
// Explain :
//
struct AICOND00 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    BYTE cNotfightOrDelay; // 0= ������ , 1=���
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_01
// Explain :
//
struct AICOND01 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDamage; // ��������
    BYTE cRecvOrGive; // 0=�޴� , 1=�ִ�
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_02
// Explain :
//
/****************************************************************************************************
 * Struct Name : AICOND_02 - <<2004�� 1�� 2�� ����>>
 * �ֺ�ĳ���� ��Ȳ üũ (���� Ŭ���� CCondDlg_02)
 * Explain : üũ���� iDistance�̳��� ĳ����Ÿ�� cChrType�̰� �������� cLevelDiff�̻��̰� ĳ���ͼ���
 * wChrNum�̻��� ���
 */
struct AICOND02 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // üũ�Ÿ�
    BYTE btIsAllied; // ĳ���� Ÿ�� (����=0,�Ʊ�=1)
    short nLevelDiff; // ������ (-128~127 )  �̰� �̻�
    short nLevelDiff2; // ������ (-128~127 )  �̰� �̻�
    WORD wChrNum;
    /*
    union {
        struct {
            char	cLevelDiff2 ;			// ������ (-128~127 )  �̰� ����. ��: cLevelDiff�̻��̰�
    cLevelDiff2������ .. <<2004�� 1�� 2�� ����>> WORD	wChrNum ;				// ĳ���� ���� } ;
        struct {
            WORD	wChrNum_Before;
        } ;
    } ;
    */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_03
// Explain :
//
struct AICOND03 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_04
// Explain :
//
struct AICOND04 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
    BYTE cMoreLess; // �̻� ����
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_05
// Explain :
//
struct AICOND05 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    //	int				cAbType ;					// �ɷ�ġ Ÿ��
    BYTE cAbType; // �ɷ�ġ Ÿ��
    int iDiff;
    BYTE cMoreLess; // ū�� , ������
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_06
// Explain :
//
struct AICOND06 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    DWORD wHP; // HP ��ġ
    BYTE cMoreLess; // ū�� , ������
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_07
// Explain :
//
struct AICOND07 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    BYTE cPercent; // Ȯ��
};

struct AICOND08 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
    short nLevelDiff; // ������ (-128~127 ) �ڽ��� �������� cLevelDiff�̻���
    short nLevelDiff2; // ������ (-128~127 ) �ڽ��� �������� cLevelDiff�̻���
    // BYTE			cChrType ;					// ĳ���� Ÿ�� (�ƹ�Ÿ=0,����=1)
    BYTE btIsAllied; // ĳ���� Ÿ�� (����=0,�Ʊ�=1)
};

struct AICOND09 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
};

struct AICOND10 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    BYTE cAbType; // �ɷ�ġ Ÿ��
    BYTE cMoreLess; // ū=0 , ����=1
};

/*******************************************************************************************
 * Struct Name : AICOND_11
 * Explain : �ڽ��� ������ ĳ������ A�ɷ�ġ�� B������ ũ�ų�/���� ���
 */
struct AICOND11 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ�� = AICOND_11
    BYTE cAbType; // �ɷ�ġ (����=0, ���ݷ�=1, ����=2, �׸���=3, HP=4, �ŷ�=5)
    int iValue; // �񱳰�. �̰����� ū��/������. �̷�������
    BYTE cMoreLess; // ū=0 , ����=1
};

// ���⼭���� �����ҽ��� �´��� ��
// ������ ���ΰ� / ���ΰ� ?
struct AICOND12 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ�� = AICOND_12
    BYTE cWhen; // 0 = ������ üũ, 1 = ������ üũ
};

// �ڽ�/Ÿ�ٿ��� �ɷ��ִ� ���� üũ.
struct AICOND13 {
    DWORD dwSize;
    AITYPE Type;

    BYTE btCheckTarget; // 0 = �ڱ��ڽ�, 1 = ����Ÿ��
    BYTE btStatusType; // 0 = �طο�, 1 = ������, 2 = �طο�/������ �ƹ��ų�
    BYTE btHave; // 0 = ���°� ? 1 = �ִ°� ?
};

// NPC ���� üũ	- AICOND14 AIACT25
// ���庯�� üũ	- AICOND15 AIACT26
// ���� ������ üũ - AICOND16 AIACT27
// ���� ���� üũ - AICOND28
typedef struct tagValueAI {
    DWORD dwSize;
    AITYPE Type;

    short nVarNo; // ���� ��ȣ : 0 ~ , ���� ���������� ����
    int iValue; // ���� �����Ͱ� (iType�� �ɷ�ġ Op iValue)
    BYTE btOp; // iValue���� ���� �񱳹��. 0 = ����, 1 = ũ��, 2 = ũ�ų� ����. 3=�۴�, 4=�۰ų�
               // ����. (�׼���) 5 = ���ٲ�, 6 = ����, 7 = ����
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27, AICOND28;

/// NPC ����
#define AICOND_17 (0x000000012 | AICOND_GENERAL)

// NPC ����
struct AICOND17 {
    DWORD dwSize;
    AITYPE Type;

    //	short			nZoneNo;
    int iNpcNo; /// ������ NPC ��ȣ
};

// �ڽŰ� ����ĳ����(�ڽ��� ��ȯ�� �ƹ�Ÿ)���� �Ÿ� üũ - ��ȯ�� ���� ���
struct AICOND18 {
    DWORD dwSize;
    AITYPE Type;

    int iDistance; // ������ NPC ��ȣ. (���ΰ��� �Ÿ� Op iDistance)
    BYTE btOp; // 0 = ����, 1 = ũ��, 2 = ũ�ų� ����. 3=�۴�, 4=�۰ų� ����.

    // ��ȯ�� �� �Ÿ� ���� ������ �Ʒ� Ȱ��. 2004-4-9 ����� ���� ���
    // BYTE			btOrg;		// 0 = ��ȯ�� ĳ����, ...
};

// ���� ���ð� üũ
struct AICOND19 {
    DWORD dwSize;
    AITYPE Type;

    unsigned long ulTime; //
    unsigned long ulEndTime; // ulTime <= ���� ���ð� <= ulEndTime �ΰ� ?
};

// �ڽ�(����)�� �ɷ�ġ/��Ÿ/����ĳ���� ���� ������ üũ
struct AICOND20 {
    DWORD dwSize;
    AITYPE Type;

    BYTE btAbType; // �ɷ�ġ (����=0, ���ݷ�=1, ����=2, �׸���=3, HP=4, �ŷ�=5)

    int iValue; // ���� �����Ͱ� (iType�� �ɷ�ġ Op iValue)
    BYTE btOp; // iData���� ���� �񱳹��. 0 = ����, 1 = ũ��, 2 = ũ�ų� ����. 3=�۴�, 4=�۰ų�
               // ����.
};

// ����ĳ���Ͱ� ���� ���� Ȯ��
struct AICOND21 {
    DWORD dwSize;
    AITYPE Type;
};

// ����ĳ���Ͱ� ���� ����Ÿ���� ������ �ִ°� ?
struct AICOND22 {
    DWORD dwSize;
    AITYPE Type;
};

// ����ð� üũ
struct AICOND23 {
    DWORD dwSize;
    AITYPE Type;

    unsigned long ulTime; //
    unsigned long ulEndTime; // ulTime <= ���� ���ð� <= ulEndTime �ΰ� ?
};

// ��¥ + �ð� üũ
struct AICOND24 {
    DWORD dwSize;
    AITYPE Type;

    BYTE btDate; // ��¥. (1 ~ 31)

    BYTE btHour1; // �� (1 ~ 24)
    BYTE btMin1; // �� (1 ~ 60)
    BYTE btHour2; // ��
    BYTE btMin2; // ��
    //  ������ ��¥�� btDate �̰�, btHour1�� btMin1��  <= ���� �ð� <= btHour1�� btMin1�� �̸� true
};

// ���� + �ð� üũ
struct AICOND25 {
    DWORD dwSize;
    AITYPE Type;

    BYTE btWeekDay; // ���� (0 ~ 6)

    BYTE btHour1; // ��
    BYTE btMin1; // ��
    BYTE btHour2;
    BYTE btMin2;
    //  ������ ������ btWeekDay �̰�, btHour1�� btMin1��  <= ���� �ð� <= btHour1�� btMin1�� �̸�
    //  true
};

// ä�� ���� üũ
struct AICOND26 {
    DWORD dwSize;
    AITYPE Type;

    unsigned short nX;
    unsigned short nY; // (nX <= ä�μ��� <= nY) �̸� true
};

/****************************************************************************************************
 * Struct Name : AICOND_27
 * �ֺ�ĳ���� ��Ȳ üũ
 * Explain : üũ���� iDistance�̳��� ĳ����Ÿ�� cChrType�̰� �������� cLevelDiff�̻��̰� ĳ���ͼ���
 * wChrNum�̻�/������ ���
 */
struct AICOND27 // 2�� �����ϰ� ���...
{
    DWORD dwSize;
    AITYPE Type;
    int iDistance; // üũ�Ÿ�
    BYTE btIsAllied; // ĳ���� Ÿ�� (�ƹ�Ÿ=0,����=1)
    short nLevelDiff; //
    short nLevelDiff2; // nLevelDiff <= XXXX <= nLevelDiff2

    WORD wChrNum; // ĳ���� ����
    BYTE
        btOp; // ĳ���� ���ڿ� ���� OP (0 = ����, 1 = ũ��, 2 = ũ�ų� ����. 3=�۴�, 4=�۰ų� ����.)
};

// ������ / ����Ÿ���� Ŭ���������ΰ� ?
struct AICOND29 {
    DWORD dwSize;
    AITYPE Type;

    BYTE btTargetType; // 0 : ������, 1 : ����Ÿ��
};

struct stCondHead {
    DWORD dwSize;
    AITYPE Type;
};

struct AI_CONDITION // stCond
{
    union {
        BYTE m_pDATA[1];
        stCondHead m_Header;
        /*
        struct
        {
            DWORD	dwSize ;
            AITYPE	Type ;
        } ;
        */
        //------------------------------------------------------------------------------
        // ���� ���� ��Ʈ���� ��� ����Ʈ �Ҳ�
        AICOND00 st00;
        AICOND01 st01;
        AICOND02 st02;
        AICOND03 st03;
        AICOND04 st04;
        AICOND05 st05;
        AICOND06 st06;
        AICOND07 st07;
        AICOND08 st08;
        AICOND09 st09;
        AICOND10 st10;

        AICOND11 st11;
        AICOND12 st12;
        AICOND13 st13;
        AICOND14 st14;
        AICOND15 st15;
        AICOND16 st16;
        AICOND17 st17;

        AICOND18 st18;
    };
};

#define AIACT_GENERAL 0x0B000000

#define AIACT_00 (0x000000001 | AIACT_GENERAL)
#define AIACT_01 (0x000000002 | AIACT_GENERAL)
#define AIACT_02 (0x000000003 | AIACT_GENERAL)
#define AIACT_03 (0x000000004 | AIACT_GENERAL)
#define AIACT_04 (0x000000005 | AIACT_GENERAL)
#define AIACT_05 (0x000000006 | AIACT_GENERAL)
#define AIACT_06 (0x000000007 | AIACT_GENERAL)
#define AIACT_07 (0x000000008 | AIACT_GENERAL)
#define AIACT_08 (0x000000009 | AIACT_GENERAL)
#define AIACT_09 (0x00000000A | AIACT_GENERAL)
#define AIACT_10 (0x00000000B | AIACT_GENERAL)
#define AIACT_11 (0x00000000C | AIACT_GENERAL)
#define AIACT_12 (0x00000000D | AIACT_GENERAL)
#define AIACT_13 (0x00000000E | AIACT_GENERAL)
#define AIACT_14 (0x00000000F | AIACT_GENERAL)
#define AIACT_15 (0x000000010 | AIACT_GENERAL)
#define AIACT_16 (0x000000011 | AIACT_GENERAL)
#define AIACT_17 (0x000000012 | AIACT_GENERAL)

#define AIACT_18 (0x000000013 | AIACT_GENERAL)
#define AIACT_19 (0x000000014 | AIACT_GENERAL)
#define AIACT_20 (0x000000015 | AIACT_GENERAL)

// #define		AIACT_21					(0x000000016 | AIACT_GENERAL)	// ��� ����. ** ���
// ����
#define AIACT_22 (0x000000017 | AIACT_GENERAL) // ������ ���͸� ������ ��ġ�� ��ȯ��Ų��
#define AIACT_23 (0x000000018 | AIACT_GENERAL) // �ڻ� ���� ������
#define AIACT_24 (0x000000019 | AIACT_GENERAL) // ����� ���� ��ų/���� ����

#define AIACT_25 (0x00000001A | AIACT_GENERAL)
#define AIACT_26 (0x00000001B | AIACT_GENERAL)
#define AIACT_27 (0x00000001C | AIACT_GENERAL)
#define AIACT_28 (0x00000001D | AIACT_GENERAL)

// ����ĳ����(�ڱ⸦ ��ȯ�� ĳ����)�� �ִ� ������ �̵�
#define AIACT_29 (0x00000001E | AIACT_GENERAL)
// Ʈ���Ž���
#define AIACT_30 (0x00000001F | AIACT_GENERAL)

// ������ ������ �ִ� ����Ÿ�� ����
#define AIACT_31 (0x000000020 | AIACT_GENERAL)
// Pk Mode On/Off
#define AIACT_32 (0x000000021 | AIACT_GENERAL)

// ����/�� ������ ������ On/Off �� ����
#define AIACT_33 (0x000000022 | AIACT_GENERAL)
// ����ĳ���Ϳ��� ������ ����
#define AIACT_34 (0x000000023 | AIACT_GENERAL)
// ���� ���� �� ����
#define AIACT_35 (0x000000024 | AIACT_GENERAL)

//
// Ŭ��
//
// �ڽ� �ֺ��� ���� ��ȯ
#define AIACT_36 (0x000000025 | AIACT_GENERAL)
// ���� �ֺ��� ���� ��ȯ
#define AIACT_37 (0x000000026 | AIACT_GENERAL)

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT00
// Explain :
//

struct AIACT00 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT01
// Explain :
//

struct AIACT01 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    BYTE cAction; // ���۹�ȣ
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT02
// Explain :
//

struct AIACT02 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    // char			szCon[1] ;					// ��� ������
    int iStrID;
};
struct AIACT02_STR {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    char szCon[1]; // ��� ������
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT03
// Explain :
//

struct AIACT03 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
    BYTE cSpeed; // 0: �ȱ�, 1: �ٱ�
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT04
// Explain :
//

struct AIACT04 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
    BYTE cSpeed; // 0: �ȱ�, 1: �ٱ�
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT05
// Explain :
//
struct AIACT05 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    BYTE cSpeed; // 0: �ȱ�, 1: �ٱ�
    // BYTE			cAttackOrMove ;				// ���� , �̵�
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT06
// Explain :
//

struct AIACT06 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
    BYTE cAbType; // �ɷ�ġ Ÿ��
    BYTE cMoreLess; // ū�� , ������
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT07
// Explain :
//

struct AIACT07 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
                 // ������ �ʿ� ����
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT08
// Explain :
//

struct AIACT08 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
    BYTE cSpeed; // 0: �ȱ�, 1: �ٱ�
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT09
// Explain :
//

typedef struct tagAIACT09 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    //	BYTE			cMonster ;					// ���͹�ȣ
    WORD wMonster; // ���͹�ȣ
} AIACT09, AIACT10;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT10
// Explain :
//

// struct		AIACT10
//{
//	DWORD			dwSize ;					// �� ��Ʈ������ ũ��
//	AITYPE			Type ;						// ���� Ÿ��
//} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT11
// Explain :
//

struct AIACT11 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
    //	BYTE			cCallType ;					// ����ȣ�� ���� , (��� , ����������)
    int iNumOfMonster;
};

struct AIACT12 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
};

struct AIACT13 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
};

struct AIACT14 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    int iDistance; // �Ÿ�
};

struct AIACT15 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
};

/**********************************************************************************
 * Struct Name : AIACT16
 * ���� : �̵��� ���е�. �������߿��� �¾Ƶ� ��� ������
 */
struct AIACT16 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type = AIACT_16 . ����
    int iDistance; /// ������ �߽����� �� ���� �̳��� ����
};

/**********************************************************************************
 * Struct Name : AIACT17
 * ���� : �����۹�ȣ�� 5�� ������ �ִٰ� �����ϰ� ����߸�. ������ ��ȣ�� 5�ڸ�
 */
struct AIACT17 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type = AIACT_16 . ����
    union {
        struct {
            short item0; /// ������ ��ȣ 0  . ���� �����۹�ȣ�� ������ -1�� ���� �ִ´�
            short item1; /// ������ ��ȣ 1
            short item2; /// ������ ��ȣ 2
            short item3; /// ������ ��ȣ 3
            short item4; /// ������ ��ȣ 4
        };
        short m_ITEMS[5];
    };
    int iToOwner;
};

/**********************************************************************************
 * Struct Name : AIACT18 - <<2004.1.29 �߰�>>
 * ������ ��ȣ�� ���������� ������ ����ȣ�� ����. (���͹�ȣ, �Ÿ�, ������)
 * �߰� ���� : ������ �Ÿ��ȿ� ������ ��������ŭ�� ������ ��ȣ�� ���Ϳ��� ����ȣ�� ����
 */
struct AIACT18 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type���� AIACT_18

    WORD cMonster; // ���͹�ȣ
    WORD wHowMany; // ���
    int iDistance; // �Ÿ�
};

/**********************************************************************************
 * Struct Name : AIACT19 - <<2004.1.29 �߰�>>
 * ���� ������ �ִ� �ƹ�Ÿ�� ����Ÿ������ ����
 */
struct AIACT19 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type���� AIACT_19

    /// �߰� ���� �ʿ�
};

/**********************************************************************************
 * Struct Name : AIACT20 - <<2004.1.29 �߰�>>
 * ������ ���͸� ��ȯ��Ų��. �ڽ�/������(�ڽ��� ������ ĳ����)/����Ÿ��(�ڽ��� �����ϴ� ĳ����).
 * ��ó ?���� �̳��� ��ȯ
 * �߰����� : �������� 1�����̴�.
 */
struct AIACT20 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type���� AIACT_20

    WORD cMonster; /// ���͹�ȣ
    BYTE btPos; /// ��ȯ��ġ : 0 = �ڽ� ����,  1 = ������ ����(�ڽ��� ������), 2 = ����Ÿ��
                /// ����(�ڽ��� �����ϰ� �ִ�)
    int iDistance; // �Ÿ� . �� �Ÿ� �̳��� ��ȯ
};

/**********************************************************************************
 * Struct Name : AIACT22 - <<2004.2.21 �߰�>>
 * ������ ���͸� ������ ��ġ�� ��ȯ
 */
/*
struct AIACT22
{
    DWORD		dwSize;			/// �� ��Ʈ������ ũ��
    AITYPE		Type;			/// Type���� AIACT_22

};
*/

// �ڻ��Ų��
struct AIACT23 {
    DWORD dwSize;
    AITYPE Type;
};

// ��ų�� ������ ����� ���� ���
struct AIACT24 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type

    BYTE btTarget; // 0=���ǿ� üũ�� ĳ����, 1=���� ����Ÿ��, 2=�ڱ��ڽ�(������ų)
    short nSkill; // ��ų��ȣ
    short nMotion; // ���۹�ȣ -1 = ���۾���
};

struct AIACT28 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type

    BYTE btMsgType; /// 0 = �����ʵ�, 1 = ������, 2=��ü����
    // short	nMsgLength;		/// ����� ���� NULL����
    // char		szMsg[ 1 ];		/// ���
    int iStrID;
};
struct AIACT28_STR {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type

    BYTE btMsgType; /// 0 = �����ʵ�, 1 = ������, 2=��ü����
    char szMsg[1]; /// ���
};

// ����ĳ����(�ڽ��� ��ȯ�� ĳ����)�� �ִ� ������ �̵�
struct AIACT29 {
    DWORD dwSize;
    AITYPE Type;

    // 2004-4-9 ��������� ������ ����
    // short	nCheckUnit; // �� ��ŭ ������ üũ . �̰͵� �� �� ����. ����=�ð�, ������
    // BYTE		btSpeed; // 0 = õõ�� , 1 = ���� (��ȯ�ڿ��� ���µ� �̵��ӵ� ������ ����)
    // BYTE		btAddOp; // ��ȯ�ڰ� �׾��� ��� (�Ǵ� ���� ���) ���ư� ��ġ ����
};

// Ʈ���� ����
struct AIACT30 {
    DWORD dwSize;
    AITYPE Type;

    short shNameLen;

    //  union �����ϸ� struct ���������� �����Ϳ� Ʋ���� ��縮��
    //	union {
    //		t_HASHKEY	m_HashNextTrigger[0];	/// �ε��� Ʈ���Ÿ��� �콬������ ���� ��Ŵ...
    char szTrigger[1]; // Ʈ���Ÿ�. NULL����
    //	} ;
};

// ������ ������ �ִ� ����Ÿ�� ����
struct AIACT31 {
    DWORD dwSize;
    AITYPE Type;
};

// PK ��� On/Off. NPC�� �����ϴ� ����.
struct AIACT32 {
    DWORD dwSize;
    AITYPE Type;

    short nZoneNo; // ����ȣ, ����� ������ 0
    BYTE btOnOff; // 0 = Off , 1 = On
};

// ����/��� ���� ������ On/Off ��
struct AIACT33 {
    DWORD dwSize;
    AITYPE Type;

    short nZonNo; // 0 : ������, �̿�: ����ȣ
    BYTE btOp; // 0: Off, 1: On, 2: Toggle
};

// ����ĳ���Ϳ��� ������ ����
struct AIACT34 {
    DWORD dwSize;
    AITYPE Type;

    short nItemNo;
    short nCnt; // nItemNo �� nCnt ���� ��ŭ ����
};

// ���� ���� �� ����
struct AIACT35 {
    DWORD dwSize;
    AITYPE Type;

    short nVarNo; // ���� ��ȣ : 0 ~ 4
    int iValue; // ���� �����Ͱ�
    BYTE btOp; // (�׼���) 5 = ���ٲ�, 6 = ����, 7 = ����
};

//
// Ŭ��
//
// �ڽ��� ��ó�� ?�� ���� ��ȯ ( ���� 10������ ���� ���� �߰� )
struct AIACT36 {
    DWORD dwSize; // �� ��Ʈ������ ũ��
    AITYPE Type; // ���� Ÿ��
    WORD cMonster; // ���͹�ȣ

    BYTE btMaster; // 0: ������, 1: �ڽ�(ȣ����)�� �������� ����
};

// ?�ֺ��� ?���� �̳��� ?�� ���� ��ȯ ( ���� 20������ ���� ���� �߰� )
struct AIACT37 {
    DWORD dwSize; /// �� ��Ʈ������ ũ��
    AITYPE Type; /// Type���� AIACT_20

    WORD cMonster; /// ���͹�ȣ
    BYTE btPos; /// ��ȯ��ġ : 0 = �ڽ� ����,  1 = ������ ����(�ڽ��� ������), 2 = ����Ÿ��
                /// ����(�ڽ��� �����ϰ� �ִ�)
    int iDistance; // �Ÿ� . �� �Ÿ� �̳��� ��ȯ

    BYTE btMaster; // 0: ������, 1: �ڽ�(ȣ����)�� �������� ����
};

struct stActHead {
    DWORD dwSize;
    AITYPE Type;
};
struct AI_ACTION // stAct
{
    union {
        BYTE m_pDATA[1];
        stActHead m_Header;
        /*
        struct
        {
            DWORD	dwSize ;
            AITYPE	Type ;
        } ;
        */
        AIACT00 st00;
        AIACT01 st01;
        AIACT02 st02;
        AIACT03 st03;
        AIACT04 st04;
        AIACT05 st05;
        AIACT06 st06;
        AIACT07 st07;
        AIACT08 st08;
        AIACT09 st09;
        AIACT10 st10;
        AIACT11 st11;
        AIACT12 st12;
        AIACT13 st13;
        AIACT14 st14;
        AIACT15 st15;
        AIACT16 st16;
        AIACT17 st17;
        AIACT18 st18;
        AIACT19 st19;
        AIACT20 st20;

        // AIACT22		st22 ;
        AIACT23 st23;
        AIACT24 st24;
        AIACT25 st25;
        AIACT26 st26;
        AIACT27 st27;
        AIACT28 st28;
        AIACT29 st29;
        AIACT30 st30;
        AIACT31 st31;

        AIACT32 st32;
    };
};

//-------------------------------------------------------------------------------------------------
#define AI_PATTERN_CREATED 0
#define AI_PATTERN_STOP 1
#define AI_PATTERN_ATTACKMOVE 2
#define AI_PATTERN_DAMAGED 3
#define AI_PATTERN_KILL 4
#define AI_PATTERN_DEAD 5

struct t_AIPARAM {
    CAI_OBJ* m_pSourCHAR;
    CAI_OBJ* m_pDestCHAR;
    CAI_OBJ* m_pFindCHAR;
    CAI_OBJ* m_pNearCHAR;
#ifdef __SERVER
    CObjNPC* m_pLocalNPC;
#endif

    int m_iDamage;
    WORD m_wPattern;
};

class CFileSystem;
class CAI_EVENT {
private:
    AI_CONDITION** m_ppConditionLIST;
    int m_iConditionCNT;

    AI_ACTION** m_ppActionLIST;
    int m_iActionCNT;

public:
    CAI_EVENT();
    ~CAI_EVENT();

#ifdef __SERVER
    bool Load(FILE* fp, STBDATA* pSTB, int iLangCol);
#else
    bool Load(CFileSystem* pFileSystem, STBDATA* pSTB, int iLangCol);
#endif
    bool Check(t_AIPARAM* pAIParam);
};

//-------------------------------------------------------------------------------------------------

class CAI_PATTERN {
private:
    CAI_EVENT* m_pEvent;
    int m_iEventCNT;

public:
    CAI_PATTERN();
    ~CAI_PATTERN();

#ifdef __SERVER
    bool Load(FILE* fp, STBDATA* pSTB, int iLangCol);
#else
    bool Load(CFileSystem* pFileSystem, STBDATA* pSTB, int iLangCol);
#endif
    void Check(t_AIPARAM* pAIParam);
};

//-------------------------------------------------------------------------------------------------
struct tagConditionFUNC {
    bool (*m_fpCondition)(stCondHead* pConDATA, t_AIPARAM* pParam);
};
extern tagConditionFUNC g_FuncCOND[];

struct tagActionFUNC {
    void (*m_fpAction)(stActHead* pActDATA, t_AIPARAM* pParam);
};
extern tagActionFUNC g_FuncACTION[];

//-------------------------------------------------------------------------------------------------

int AI_Get_Ability(CAI_OBJ* pCHAR, int iAbTYPE);

//-------------------------------------------------------------------------------------------------

template<class dType1, class dType2>
inline bool
Check_AiOP(BYTE btOP, dType1 iLeft, dType2 iRight) {
    bool bResult;
    switch (btOP) {
        case 0: // ����
            bResult = (iLeft == iRight);
            break;
        case 1: // ũ��
            bResult = (iLeft > iRight);
            break;
        case 2: // ũ�ų� ����
            bResult = (iLeft >= iRight);
            break;
        case 3: // �۴�
            bResult = (iLeft < iRight);
            break;
        case 4: // �۰ų� ����.
            bResult = (iLeft <= iRight);
            break;
        case 10: // ���� �ʴ�.
            bResult = (iLeft != iRight);
            break;
        default:
            return false;
    }

    return bResult;
}

template<class dType>
inline dType
Result_AiOP(BYTE btOP, dType iLeft, dType iRight) {
    switch (btOP) {
        case 5: // ���ٲ�
            return iRight;
        case 6: // ����(�־��� ��ŭ)
            return (iLeft + iRight);
        case 7: // ����
            return (iLeft - iRight);
    }
    return 0;
}