#include "stdAFX.h"
#include "CEconomy.h"
#include "IO_STB.h"

short g_nWorldRate=100;
short g_nWorldProc=100;

short Get_WorldRATE ()						{	return g_nWorldRate;		}
short Get_WorldPROD ()						{	return g_nWorldProc;		}

void  Set_WorldRATE (short nWorldRate)		{	g_nWorldRate=nWorldRate;	}
void  Set_WorldPROD (short nWorldProd)		{	g_nWorldProc=nWorldProd;	}

CEconomy::CEconomy ()
{
	Init ();
}
CEconomy::~CEconomy ()
{
	// save ...
}

void CEconomy::Init (void)
{
	m_btTOWN_RATE  = 100;

	for (short nP=MIN_PRICE_TYPE; nP<MAX_PRICE_TYPE; nP++) {
		m_btItemRATE[ nP ] = 50;
	}
}

#define	MIN_TOWN_ITEM	100
#define	MAX_TOWN_ITEM	32000

void CEconomy::SetTownRATE (int iValue)
{
	m_btTOWN_RATE = iValue;
}
void CEconomy::SetItemRATE(int iPriceType, int iValue)
{
	assert ( iPriceType >= MIN_PRICE_TYPE && iPriceType <  MAX_PRICE_TYPE );

	m_btItemRATE[ iPriceType ] = iValue;
}

bool CEconomy::IsEssentialGoods (int iItemTYPE)
{
	// ITEM_TYPE( X,Y )
	if ( ( iItemTYPE >= 421 && iItemTYPE <= 428 ) ||
		 ( iItemTYPE >= 311 && iItemTYPE <= 312 ) )
		return true;

	return false;
}

int CEconomy::Get_ItemBuyPRICE (short nItemTYPE, short nItemNO, short nBuySkillVALUE)
{
	int iPrice = 0;
	int iItemRate, iPriceRate;

	switch( nItemTYPE ) {
		case ITEM_TYPE_FACE_ITEM:  
		case ITEM_TYPE_HELMET	:  
		case ITEM_TYPE_ARMOR	:  
		case ITEM_TYPE_GAUNTLET :  
		case ITEM_TYPE_BOOTS	:  
		case ITEM_TYPE_KNAPSACK :  
		case ITEM_TYPE_WEAPON	:
		case ITEM_TYPE_SUBWPN	:
		case ITEM_TYPE_RIDE_PART:
			// 2004. 1. 2 구입 가격 = 
			iPrice = (int)(	ITEM_BASE_PRICE( nItemTYPE, nItemNO ) * ( ITEM_QUALITY( nItemTYPE, nItemNO )+50.f ) * ( 1.f - nBuySkillVALUE * 0.01 ) / 100.f + 0.5f );
			break;

		case ITEM_TYPE_USE		:
		case ITEM_TYPE_NATURAL	:
			if ( IsEssentialGoods( ITEM_TYPE( nItemTYPE, nItemNO ) ) ) {
				// 생필품.
				// 구입 가격 = ITEM_BASE * { 1 + (ITEM_RATE - 50) * ITEM_FLUC / 1000 } * (1 - 구입스킬레벨 * 0.02)
				iItemRate  = this->Get_ItemRATE( ITEM_RATE_TYPE( nItemTYPE, nItemNO ) );
				iPriceRate = ITEM_PRICE_RATE( nItemTYPE, nItemNO );

				iPrice = (int) (
							ITEM_BASE_PRICE( nItemTYPE, nItemNO ) * 
							( 1.f + ( iItemRate - 50.f ) * iPriceRate / 1000.f ) * ( 1.f - nBuySkillVALUE * 0.01f ) + 0.5f );
				break;
			}

		case ITEM_TYPE_JEWEL	:
		case ITEM_TYPE_GEM		:
		case ITEM_TYPE_QUEST	:
			// 2003. 08. 18 구입 가격 = ITEM_BASE * { 1 + (TOWN_RATE - 50) * ITEM_FLUC / 1000 } * (1 - 구입스킬레벨 * 0.02)
			iPriceRate = ITEM_PRICE_RATE( nItemTYPE, nItemNO );
			iPrice = (int) ( 
							ITEM_BASE_PRICE( nItemTYPE, nItemNO ) * 
							( 1.f + ( this->Get_TownRATE() - 50.f ) * iPriceRate / 1000.f ) * ( 1 - nBuySkillVALUE * 0.01f ) + 0.5f
							);
			break;
	}

	return iPrice;
}

int CEconomy::Get_ItemSellPRICE (tagITEM &sITEM, short nSellSkillVALUE)
{
	int iPrice = 0;
	int iItemRate, iPriceRate;

	switch( sITEM.m_cType ) {
		case ITEM_TYPE_FACE_ITEM:  
		case ITEM_TYPE_HELMET	:  
		case ITEM_TYPE_ARMOR	:  
		case ITEM_TYPE_GAUNTLET :  
		case ITEM_TYPE_BOOTS	:  
		case ITEM_TYPE_KNAPSACK :  
		case ITEM_TYPE_WEAPON	:
		case ITEM_TYPE_SUBWPN	:
		case ITEM_TYPE_RIDE_PART:
			// 추가옵션은 보석과 같이
			iPrice = (int) ( 
						(float)ITEM_BASE_PRICE( sITEM.GetTYPE(), sITEM.GetItemNO() ) * 
						(float)( 40 + sITEM.GetGrade() ) * 
						(float)( 200 + sITEM.GetDurability() ) *
						(float)( 200 - Get_WorldRATE() ) * 
						(float)( 1 + nSellSkillVALUE * 0.01f ) / 1000000.f *
						(float)( (4000+sITEM.GetLife()) / 14000.f ) +
						(float)( sITEM.IsAppraisal() * GEMITEM_BASE_PRICE( sITEM.GetOption() ) * 0.2f )
					) ;
			break;

		case ITEM_TYPE_USE		:
		case ITEM_TYPE_NATURAL	:
			if ( IsEssentialGoods( ITEM_TYPE( sITEM.m_cType, sITEM.m_nItemNo ) ) ) {
				iItemRate  = this->Get_ItemRATE( ITEM_RATE_TYPE( sITEM.m_cType, sITEM.m_nItemNo ) );
				iPriceRate = ITEM_PRICE_RATE( sITEM.m_cType, sITEM.m_nItemNo );
				// 생필품.
				// 판매 가격 = ITEM_BASE * { 1000 + (ITEM_RATE - 50) * ITEM_FLUC } * (1 + 판매스킬레벨 * 0.02) * (200- WORLD_RATE) / 200000
				iPrice = (int) (
					ITEM_BASE_PRICE( sITEM.m_cType, sITEM.m_nItemNo) * 
					( 1000 + ( iItemRate-50 ) * iPriceRate ) *
					( 1 + nSellSkillVALUE * 0.01 ) * ( 200 - Get_WorldRATE() ) / 180000.f
					) ;
				break;
			}

		case ITEM_TYPE_JEWEL	:
		case ITEM_TYPE_GEM		:
		case ITEM_TYPE_QUEST	:
			// 2003.08.018 그 밖의 아이템.
			// 판매 가격 = ITEM_BASE * { 1000 + (TOWN_RATE - 50) * ITEM_FLUC } * (1 + 판매스킬레벨 * 0.02) * (200- WORLD_RATE) / 200000
			iPriceRate = ITEM_PRICE_RATE( sITEM.m_cType, sITEM.m_nItemNo );
			iPrice = (int) ( 
					ITEM_BASE_PRICE( sITEM.m_cType, sITEM.m_nItemNo) * 
					( 1000 + ( this->Get_TownRATE() - 50 ) * iPriceRate ) *
					( 1 + nSellSkillVALUE * 0.01 ) * ( 200 - Get_WorldRATE() ) / 180000.f
					) ;
			break;
	}

	return iPrice;
}
