#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_ANDMEAN
#include <windows.h>
#endif

#include "lib_util.h"
#include "cgs_api.h"
#include "LIB_gsMAIN.h"

class ExeApi : public EXE_GS_API {
public:
	void  __stdcall SetUserCNT(int iUserCNT) {
		std::cout << "Setting user count: " << iUserCNT << std::endl;
	}

	void  __stdcall WriteLOG(char *szString) {
		std::cout << szString;
	}

	void  __stdcall SetListItemINT(void *pListItem, int iSubStrIDX, int iValue) {
		std::cout << "Setting list item int" << std::endl;
	}

	void  __stdcall SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr) {
		std::cout << "Setting list item str: " << szStr << std::endl;
	}

	void* __stdcall AddZoneITEM(void *pOwner, short nZoneNO, char *szZoneName) {
		std::cout << "Adding zone: " << szZoneName << "(#" << nZoneNO << ")" << std::endl;
		return nullptr;
	}

	void  __stdcall DelZoneITEM(void *pListItem) {
		std::cout << "Deleting channel" << std::endl;
	}
};


CLIB_GameSRV* g_instance;


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		std::cout << "Shutting down..." << std::endl;
		g_instance->Shutdown();
		g_instance->Destroy();
		std::cout << "Press enter to quit..." << std::endl;
		std::cin.get();

	default:
		return FALSE;
	}
}

int main() {
	// Let login server load
	Sleep(2000);

	auto exe_api = new ExeApi();

	const int ENGLISH = 1;
	int language = ENGLISH;
	char* data_dir = (char*)"C:\\dev\\rose-next\\server\\data";
	char* db_ip = (char*)"127.0.0.1";
	char* db_name = (char*)"SHO";
	char* db_user = (char*)"seven";
	char* db_password = (char*)"tpqmsgkcm";
	char* server_name = (char*)"Channel 1";
	char* server_ip = (char*)"127.0.0.1";
	int server_port = 29200;
	char* loginserver_ip = server_ip;
	int loginserver_port = 19000;
	char* charserver_ip = server_ip;
	int charserver_port = 19001;

	std::cout << "Initializing the server" << std::endl;
	g_instance = CLIB_GameSRV::InitInstance(nullptr, data_dir, exe_api, ENGLISH);

	std::cout << "Connecting to other servers" << std::endl;
	g_instance->ConnectSERVER(
		db_ip,
		db_name,
		db_user,
		db_password,
		db_user,
		db_password,
		db_user,
		db_password,
		charserver_ip,
		charserver_port,
		loginserver_ip,
		loginserver_port
	);

	std::cout << "Initializing all maps" << std::endl;
	g_instance->InitLocalZone(true);

	std::cout << "Starting the server" << std::endl;
	g_instance->Start(nullptr, server_name, server_ip, server_port, 1, 0, 0);

	std::cout << "Setting user limit" << std::endl;
	g_instance->Set_UserLIMIT(0);

	SetConsoleCtrlHandler(CtrlHandler, true);

	while (true) {
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}