


-- ��ų �����ϴ°�( 1~5 )...
function DO_SKILL ( iObject, iSkillIDX )	
end


-- �ƹ�Ÿ ó�� ������ �� ����� ȿ��
function SE_CharCreate  ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_warp_join_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- �ƹ�Ÿ�� ó�� ���忡 �����Ҷ�
function SE_CharJoinZone( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_warp_join_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- �ƹ�Ÿ �����
function SE_CharDie		( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_gost_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- �ƹ�Ÿ ����
function SE_CharWarp	( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_warp_join_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- �ƹ�Ÿ ������
function SE_CharLevelUp ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/levelup_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- ��Ƽ ������
function SE_PartyLevelUp ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/party_up_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- ���� ������
function SE_MobDie ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_gost_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end


-- ���� ����
function SE_StartMake ( iObject )
end
-- ���� ����
function SE_StartUpgrade ( iObject )
end
-- ���� ����
function SE_SuccessMake ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_success_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end


-- ���� ����
function SE_SuccessUpgrade ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_success_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

	
-- �и�/���� ����
function SE_SuccessSeparate ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_success_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );

end 

-- ���� ����
function SE_FailMake ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_failed_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );

end

-- ���� ����
function SE_FailUpgrade ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_failed_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );

end


-- ��ų������
function SE_SkillLevelUp ( iObject )
end




-- ���� ȿ��
function SE_WeatherEffect( iObject, iWeatherType )
	GF_DeleteEffectFromObject( iObject );
	-- ��
	if iWeatherType == 1 then
		iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_snow_01.eft" )	
		GF_WeatherEffectOnObject( iObject, iEffect );
	end
	
	-- ����
	if iWeatherType == 2 then
		iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_flower_01.eft" )	
		GF_WeatherEffectOnObject( iObject, iEffect );
	end

           -- ���� ����
	if iWeatherType == 3 then
		iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_ed_leaf01.eft" )	
		GF_WeatherEffectOnObject( iObject, iEffect );
	end

end