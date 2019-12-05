#ifndef __TRIGGERINFO_H__
#define __TRIGGERINFO_H__

#ifdef TRIGGERINFO_DLL
#define TRIGGERINFO_API __declspec(dllexport)
#elif TRIGGERINFO_STATIC
#define TRIGGERINFO_API
#else
#define TRIGGERINFO_API __declspec(dllimport)
#endif


#include <string>

// adapter information�� �����ϴ� ����
#define ADPTINFO_FILE "sysadpt.dat"

struct SystemResolution{
      
	int pixel_width;
	int pixel_height;
	int pixel_color;
    int frequency; 
	int	adaptor_number;
};

struct SystemAdapter{
    
	char Driver[ 100 ];
    char Description[ 100 ];
    char DeviceName[ 32 ];
    int  AdapterNumber;
	int	 nResolution;
    
	SystemResolution *pResolution;
};


struct AdapterInformation
{
  	  SystemAdapter *	pAdapter;
	  int				iAdapterCNT;
};


// Resolution ������ Release�Ѵ�
TRIGGERINFO_API void TI_ReleaseAdptInfo (void);

// ���Ͽ� ����Ǿ� �ִ� ������ ������ �´�.
TRIGGERINFO_API AdapterInformation * TI_ReadAdptInfoFile (void);

#endif
