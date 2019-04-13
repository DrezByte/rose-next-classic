#ifndef __CLS_API_H
#define __CLS_API_H


class EXE_LS_API
{
public :
	virtual void  __stdcall WriteLOG(char *szString) = 0;
    virtual void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue) = 0;
	virtual void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr) = 0;

	virtual void* __stdcall AddConnectorITEM (void *pOwner, char *szIP) = 0;
	virtual void  __stdcall DelConnectorITEM (void *pListItem) = 0;

	virtual void* __stdcall AddWorldITEM (void *pOwner, char *szWorld, char *szIP, int iPort, unsigned int dwRight) = 0;
	virtual void  __stdcall DelWorldITEM (void *pListItem) = 0;

	virtual void* __stdcall AddBlockITEM (void *pOwner, char *szIP, unsigned int dwEndTime) = 0;
	virtual void  __stdcall DelBlockITEM (void *pListItem) = 0;

	virtual void  __stdcall SetStatusBarTEXT (unsigned int iItemIDX, char *szText) = 0;
} ;

/*
class CEXE_API : public EXE_LS_API
{
public :
	void  __stdcall WriteLOG(char *szString);
    void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue);
	void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr);

	void* __stdcall AddConnectorITEM (void *pOwner, char *szIP);
	void  __stdcall DelConnectorITEM (void *pListItem);

	void* __stdcall AddWorldITEM (void *pOwner, char *szWorld, char *szIP, int iPort, unsigned int dwRight);
	void  __stdcall DelWorldITEM (void *pListItem);

	void* __stdcall AddBlockITEM (void *pOwner, char *szIP, unsigned int dwEndTime);
	void  __stdcall DelBlockITEM (void *pListItem);

	void  __stdcall SetStatusBarTEXT (unsigned int iItemIDX, char *szText);
} ;


class ExeFFiApi : public CEXE_API {
public:
	void (*print)(const char* foo);

public:
	void  __stdcall WriteLOG(char *szString) {
		this->print(szString);
	}

	void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue) {
		this->print("Setting list item INT");
	}

	void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr) {
		this->print("Setting list item STR");
	}

	void* __stdcall AddConnectorITEM(void *pOwner, char *szIP) {
		this->print("Adding connector item");
		return nullptr;
	}

	void  __stdcall DelConnectorITEM(void *pListItem) {
		this->print("Deleting connector item");
	}

	void* __stdcall AddWorldITEM(void *pOwner, char *szWorld, char *szIP, int iPort, unsigned int dwRight) {
		this->print("Adding world item");
		return nullptr;
	}

	void  __stdcall DelWorldITEM(void *pListItem) {
		this->print("Deleting world item");
	}

	void* __stdcall AddBlockITEM(void *pOwner, char *szIP, unsigned int dwEndTime) {
		this->print("Adding block item");
		return nullptr;
	}

	void  __stdcall DelBlockITEM(void *pListItem) {
		this->print("Deleting block item");
	}

	void  __stdcall SetStatusBarTEXT(unsigned int iItemIDX, char *szText) {
		this->print("Setting status bar text");
		this->print(szText);
	}
};

/// Create the exe api passing in a function pointer for printing logs
void* create_exe_api(void(*printFn)(const char* foo)) {
	auto api = new ExeFFiApi();
	api->print = printFn;
	return (void*)api;
}

void free_exe_api(void* api) {
	delete api;
}
*/

#endif

 
