-- functions

-- eDialogType
-- {
-- 	DLG_TYPE_MENU
-- 	DLG_TYPE_CHAR
-- 	DLG_TYPE_ITEM
-- 	DLG_TYPE_SKILL
-- 	DLG_TYPE_QUEST
-- 	DLG_TYPE_COMMUNITY
-- 	DLG_TYPE_CLAN
-- 	DLG_TYPE_HELP
-- 	DLG_TYPE_OPTION
-- }
-- eButton
-- {
--	MENU_BTN_CHAR
--	MENU_BTN_ITEM
--	MENU_BTN_SKILL
--	MENU_BTN_QUEST
--	MENU_BTN_COMMUNITY
--	MENU_BTN_CLAN
--	MENU_BTN_HELP
--	MENU_BTN_INFO
--	MENU_BTN_OPTION
--	MENU_BTN_EXIT
-- }
--
-- SC_OpenDialog ( eDialogType )
-- SC_SetButtonBlink( eDialogType, eButton )
-- SC_AddNpcIndicator ( npcno, auto_remove ) --npcno(�ߺ� ����), auto_remove: 1(��ȭ�õ��� �ڵ�����:�ߺ��ȸ��), 0(��ũ��Ʈ ȣ��� ��������)
-- SC_RemoveNpcIndicator ( npcno ) -- �ߺ��� �ߺ� ������ŭ ȣ�� �ʿ�
-- SC_AddCoordinatesIndicator ( index, zoneno, x, y ) -- index �ߺ��Ұ�
-- SC_RemoveCoordinatesIndicator ( index ) -- Add�� ������ index



-- Tutorial 01
-- ó�� ���ӽ�
function Tutorial01( iOwnerIndex )	
	SC_ShowNotifyMessage( 1 );
end

-- Tutorial 02
-- 2 ������ �±�
function Tutorial02( iOwnerIndex )	
	SC_ShowNotifyMessage( 2 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_CHAR )
end

-- Tutorial 03
-- 2���� ����ġ 150 �̻��� �Ǿ�����
function Tutorial03( iOwnerIndex )	
	SC_ShowNotifyMessage( 3 );
end

-- Tutorial 04
-- 2���� ����ġ 220 �̻��� �Ǿ�����
function Tutorial04( iOwnerIndex )	
	SC_ShowNotifyMessage( 4 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_ITEM )
end

-- Tutorial 05
-- 3������ �±�
function Tutorial05( iOwnerIndex )	
	SC_ShowNotifyMessage( 5 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_SKILL )
end

-- Tutorial 06
-- 3���� ����ġ 100 �̻��� �Ǿ�����
function Tutorial06( iOwnerIndex )	
	SC_ShowNotifyMessage( 6 );
end

-- 5051 ����Ʈ�� �޾����� ��ư ����
function Tutorial07button()	
	SC_CreateEventButton( 7 )
end

-- Tutorial 07
-- 5051 ����Ʈ�� �޾�����
function Tutorial07( iOwnerIndex )	
	SC_ShowNotifyMessage( 7 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_QUEST )
end

-- Tutorial 08
-- 4������ �±�
function Tutorial08( iOwnerIndex )	
	SC_ShowNotifyMessage( 8 );
end

-- T-warp Ʈ���� ������ ��ư ����
function Tutorial09button()	
	SC_CreateEventButton( 9 )
end

-- Tutorial 09
-- T-warp Ʈ���� ������ �
function Tutorial09( iOwnerIndex )	
	SC_ShowNotifyMessage( 9 );
end

-- Tutorial 10
-- 10������ �±��
function Tutorial10( iOwnerIndex )	
	SC_ShowNotifyMessage( 10 );
end



-- Notify 01
function Notify01( iOwnerIndex )	
	SC_ShowNotifyMessage( 3 );
end








function InitTutorialEvent()
	
end


iPrevEXP = 0;
iInitPrevEXP = 0;
iStartLevel1Event = 0;	-- 1�� ����ġ0 �϶� �̺�Ʈ�� ������ �߻���Ű�� �ʱ� ����.

-- CheckTutorialEvent
function CheckTutorialEvent()

	if iInitPrevEXP == 0 then
		iPrevEXP = SC_GetAvatarEXP();
		iInitPrevEXP = 1;
	end

	iAvatarExp 	= SC_GetAvatarEXP();
	iAvatarLEVEL 	= SC_GetAvatarLEVEL();
	
	-- ó�� ���ӽ�( 1���� ����ġ 0 )�ϰ��
	if ( iStartLevel1Event == 0 ) and ( iAvatarLEVEL == 1 ) and ( iAvatarExp == 0 ) then	
		SC_CreateEventButton( 1 )
		iStartLevel1Event = 1
	end
	
	--2 ���� ����ġ 150 �̻� �Ǿ�����
	if ( iAvatarLEVEL == 2 ) and ( iAvatarExp >= 150 ) and ( iPrevEXP < 150 ) then
		SC_CreateEventButton( 3 )
	end
	
	--2 ���� ����ġ 220 �̻� �Ǿ�����
	if ( iAvatarLEVEL == 2 ) and ( iAvatarExp >= 220 ) and ( iPrevEXP < 220 ) then
		SC_CreateEventButton( 4 )
	end
	
	--3 ���� ����ġ 220 �̻� �Ǿ�����
	if ( iAvatarLEVEL == 3 ) and ( iAvatarExp >= 100 ) and ( iPrevEXP < 100 ) then
		SC_CreateEventButton( 6 )
	end
	
	
		
	-- ��� ó���� iPrevEXP�� ���� ����ġ�� �����ؼ� ���� �����ӿ��� ����ġ�� ��ȭ�� �����Ѵ�.
	iPrevEXP = SC_GetAvatarEXP();
end




--SC_RunEvent : NPC ��ȭâ
--SC_RunEventFieldWarp : Warp gate
--SC_RunEventObjectEvent


-- ���ο��Ǽҵ� ����Ʈ �̺�Ʈ ������Ʈ

-- ���� �� �̺�Ʈ
function mushroom( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object002.con", -1 );
end

-- �𷡽ð� �̺�Ʈ
function sandglass( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object003.con", -1 );
end

-- ������ å �̺�Ʈ
function horriblebook( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object004.con", -1 );
end

-- �Ƕ�̵� �̺�Ʈ01
function piramid01( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object005.con", -1 );	
end

-- �Ƕ�̵� �̺�Ʈ02
function piramid02( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object006.con", -1 );
end

-- �û����� ��
function owl( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object007.con", -1 );
end

-- �Ƕ�̵� �̺�Ʈ03
function piramid03( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object005.con", -1 );
end

-- ����� ���� ����
function mana( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object008.con", -1 );
end

-- ��������
function genzistone( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object009.con", -1 );
end

