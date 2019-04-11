--=================--
-- init script
--=================--
useVertexShader             ( 1 )
usePixelShader              ( 0 )
useWireMode                 ( 0 )
useCull                     ( 1 )
useObjectSorting            ( 1 )
usePolygonSorting           ( 0 )
useHardwareVertexProcessing ( 1 )
useMultiPass                ( 0 )
useShadowmap                ( 1 )
useLightmap                 ( 1 )
useFileTimeStamp            ( 0 )
useGlow                     ( 1 )
useFullSceneGlow            ( 0 )
useTerrainLOD               ( 0 )

setScreen ( 640, 480, 32, 0 ) -- width, depth, bpp, fullscreen
setBuffer ( 640, 480, 32 ) -- affected only in windowed mode
setTimeDelay ( 0 )

setClearColor ( 0, 0, 0 ) -- r, g, b as [0...1]
setShadowmapSize ( 256 )
setShadowmapBlurType( 1 ) -- none = 0; blur once = 1; blur twice = 2
setShadowmapColor( 0.2, 0.2, 0.2 ) -- shadow color
setGlowmapSize ( 256 )
setGlowColor ( 0.06, 0.06, 0.06 )
glowscale = 0.015
setFullSceneGlowColor ( glowscale, glowscale, glowscale )
setFullSceneGlowType( 0 )
setMipmapFilter ( 2 ) -- none = 0, point = 1, linear = 2, anisotropic = 3 (only minfilter)
setMipmapLevel ( 3 ) -- -1(�ؽ��ĳ�������), 0 (Ǯ����), 1(�Ӹʾ���), 2(�ΰ�), 3(����)...
setMinFilter ( 3 ) -- point = 1, linear = 2 
setMagFilter ( 2 ) -- point = 1, linear = 2
setFullSceneAntiAliasing ( 0 )
setUseTimeWeight ( 1 )
setUseFixedFramerate ( 0 )
setFramerateRange( 15, 1000 ) -- �ּ�, �ִ� �����ӷ� ����
setLightmapBlendStyle ( 5 ) -- 4 : modulate, 5 : modulate2x, 6 : modulate4x
setCameraTransparency( .3 )
setDrawShadowmapViewport( 0 )
setTextureLoadingScale( 0 ) -- �ؽ��� �ε��� ������ �ؽ��� ������ ���� : ����� �׷���ī�忡���� 1�̳� 2�� ���� ���
setTextureLoadingFormat( 0 ) -- �ؽ��� �ν̽� �ؽ��� ���� ��ü�� ���� : 0(�����ؽ�������, default), 1(16��Ʈ), 2(����)
setDisplayQualityLevel( 0 ) -- ���� ����Ƽ ���� : 0(�ڵ�), 1(�ֻ��-�������ϴ��), 2, 3(�߰�ǰ��), 4, 5(���ϱ�ǰ��)
useVSync( 1 ) -- Ǯ��ũ���� ���� ���� ����. 0���� �ϸ� Ǯ��ũ����忡�� ����� ���ļ������� �� ���� fps�� ����.
---setMonitorRefreshRate( 60 )
---setAdapter( 0 )
---useDebugDisplay( 1 ) -- ����� �޼��� ȭ�鿡 ��� ����

useMotionInterpolation( 1 ) -- ��� ���� ����
setMotionInterpolationRange( 20 ) -- ��� ������ ����Ǵ� ����. ���� ����. 
---setDataPath( "d:/online2003/data/" )

setLazyBufferSize( 1000, 1000, 1000, 1000 ) -- ������ �ε� ���� ������. ����Ʈ�� ���� 300. ����:(�ؽ���, �Ϲݸ޽�, �����޽�, �ٴٸ޽�)
