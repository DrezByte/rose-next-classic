// TriggerInfo.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "triggerinfo.h"
#include "SysInfo.h"
#include "Http.h"
#include "__d3d9.h"
#include "AdapterInfo.h"


/*************************************************************
 * Adapter 정보를 얻기 위한 클래스, ReleaseAdapterInfo 로 call해야
 * delete 됨
 */
static CAdapterInfo * g_pAdInfo = NULL;

/************************************************************
 * 시스템 정보를 저장하는 문자열
 */
static string g_Info;
static string g_InfoForWeb;

#ifdef TRIGGERINFO_DLL
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif

bool SaveSysResolution (const char * FileName, AdapterInformation * pInfo)
{
	/********************************************************************
	* 파일에 저장
	*/
	if(pInfo)
	{
		FILE * fp = fopen (FileName, "wb");
		if(fp)
		{
			fwrite (&pInfo->iAdapterCNT, sizeof (int), 1, fp);

			for(int i = 0; i < pInfo->iAdapterCNT; i++)
			{
				SystemAdapter * pSysAdpt = &pInfo->pAdapter[ i ];

				fwrite (pSysAdpt->Driver, sizeof (char), 100, fp);
				fwrite (pSysAdpt->Description, sizeof (char), 100, fp);
				fwrite (pSysAdpt->DeviceName, sizeof (char), 32, fp);
				fwrite (&pSysAdpt->AdapterNumber, sizeof (int), 1, fp);
				fwrite (&pSysAdpt->nResolution, sizeof (int), 1, fp);

				int iResCNT = pSysAdpt->nResolution;
				SystemResolution *pResolution = pSysAdpt->pResolution;

				if(iResCNT > 0)
				{
					for(int i = 0; i < iResCNT; i++)
					{				
						fwrite (&pResolution[ i ].pixel_width, sizeof (int), 1, fp);
						fwrite (&pResolution[ i ].pixel_height, sizeof (int), 1, fp);
						fwrite (&pResolution[ i ].pixel_color, sizeof (int), 1, fp);
						fwrite (&pResolution[ i ].frequency, sizeof (int), 1, fp);
						fwrite (&pResolution[ i ].adaptor_number, sizeof (int), 1, fp);
					}
				}
			}

			fclose(fp);
			return true;
		}
	}

	return false;
}

AdapterInformation * ReadSysResolution (const char * FileName)
{
	/********************************************************************
	* 파일에 저장
	*/

	FILE * fp = fopen (FileName, "rb");
	// __fmode = _O_BINARY;
	if(fp)
	{
		AdapterInformation * pInfo = new AdapterInformation;
		fread (&pInfo->iAdapterCNT, sizeof (int), 1, fp);
		pInfo->pAdapter = NULL;
		if(pInfo->iAdapterCNT > 0)
		{
			pInfo->pAdapter = new SystemAdapter[ pInfo->iAdapterCNT ];

			for(int i = 0; i < pInfo->iAdapterCNT; i++)
			{
				SystemAdapter * pSysAdpt = &pInfo->pAdapter[ i ];

				fread (pSysAdpt->Driver, sizeof (char), 100, fp);
				fread (pSysAdpt->Description, sizeof (char), 100, fp);
				fread (pSysAdpt->DeviceName, sizeof (char), 32, fp);
				fread (&pSysAdpt->AdapterNumber, sizeof (int), 1, fp);
				fread (&pSysAdpt->nResolution, sizeof (int), 1, fp);

				if(pSysAdpt->nResolution > 0)
				{
					pSysAdpt->pResolution = new SystemResolution[ pSysAdpt->nResolution ];

					for(int i = 0; i < pSysAdpt->nResolution; i++)
					{				
						fread (&pSysAdpt->pResolution[ i ].pixel_width	, sizeof (int), 1, fp);
						fread (&pSysAdpt->pResolution[ i ].pixel_height	, sizeof (int), 1, fp);
						fread (&pSysAdpt->pResolution[ i ].pixel_color	, sizeof (int), 1, fp);
						fread (&pSysAdpt->pResolution[ i ].frequency		, sizeof (int), 1, fp);
						fread (&pSysAdpt->pResolution[ i ].adaptor_number	, sizeof (int), 1, fp);
					}
				}
			}
		}
		fclose(fp);

		if(!g_pAdInfo)
			g_pAdInfo = new CAdapterInfo (pInfo);

		return pInfo;
	}

	return NULL;
}

TRIGGERINFO_API void TI_ReleaseAdptInfo (void)
{
	delete g_pAdInfo;
	g_pAdInfo = NULL;
}

TRIGGERINFO_API AdapterInformation * TI_ReadAdptInfoFile (void)
{
	return ReadSysResolution (ADPTINFO_FILE);
}
