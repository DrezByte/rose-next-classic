

bFadeIn = 0;
bWhiteFadeIn = 0;
bEndFadeOut = 0;



-- �⺻ ������� ��� ���� �� ī�޶� ��� ����
function InitPlanetMoveWorld( iPlanet )
	ClearMovePlanetCutScene();
	
	-- �������� ������ �����
	
	-- ���
	if iPlanet == 0 then
	end
	
	-- �糪
	if iPlanet == 1 then	
	end
	
	-- ���������� ������ �����
	AttachMotionToEnvObject();
end

function AttachMotionToEnvObject()
	
	-- Camera
	hCameraMotion = SC_FindNode( "MovePlanet_Camera01" );	
	if hCameraMotion == 0 then
		hCameraMotion = SC_LoadMotion( "MovePlanet_Camera01", "3DData\\CUTSCENE\\WARP01\\Camera01.ZMO", 1, 4, 3, 1, 0 );
	end
	
	camera  = SC_FindNode( "motion_camera")
	if camera ~= 0 then
		SC_AttachMotion( camera, hCameraMotion );
		SC_ControlAnimation( camera, 1 );
		SC_SetCameraDefault( camera );
		SC_LogString( "SetCameraDefault [ motion_camera ]" );
	end

	
	
end

function ProcMovePlanetCutScene( iPlayTime )
	
	if iPlayTime > 560 then
		SC_ChangeState( 12 );
		bFadeIn = 0;
		bWhiteFadeIn = 0;
		bEndFadeOut = 0;	
	end
		
	if iPlayTime > 0 then
		if bFadeIn == 0 then
			SC_ScreenFadeInStart( 0, 0, 2, 0, 0, 0 ); 	
			bFadeIn = 1;
		end
	end	
	
	if bWhiteFadeIn == 0 then
		if iPlayTime > 122 then		
			SC_ScreenFadeInStart( 2.2, 1.6, 2.2, 255, 255, 255 ); 	
			bWhiteFadeIn = 1;
		end
	end
	
	if bEndFadeOut == 0 then
		if iPlayTime > 496 then		
			SC_ScreenFadeInStart( 5.4, 1, 0, 255, 255, 255 ); 	
			bEndFadeOut = 1;
		end
	end
	
	--====================================================================
	-- ��Ȧ ����Ʈ
	--====================================================================
	
	if bAttachBlackHall == 0 then
		if iPlayTime > 210 then
			bAttachBlackHall = 1;
			hBlackHallObj = SC_GetEventObject( 8, 1 );
			if hBlackHallObj ~= 0 then
				iEffect = SC_GetEffectUseFile( "3DDATA/Effect/_blackholl_01.eft" )	
				SC_EffectOnObject( hBlackHallObj, iEffect );

			end

		end
	end 
end


function ClearMovePlanetCutScene()
	bFadeIn = 0;
	bWhiteFadeIn = 0;
	bEndFadeOut = 0;
	bAttachBlackHall = 0;
	--SC_LogString( "FadeIn"..bFadeIn.."WhiteFadeIn"..bWhiteFadeIn.."EndFadeOut"..bEndFadeOut.."\n" );
end