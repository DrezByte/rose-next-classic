#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_ANDMEAN
#include <windows.h>
#endif

#include "guicon.h"
#include "lib_util.h"
#include "cws_api.h"
#include "sho_ws_lib.h"


class ExeApi : public EXE_WS_API {
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

	void* __stdcall AddChannelITEM(void *pOwner, short nChannelNO, char *szChannelName, char *szServerIP, int iPortNO) {
		std::cout << "Adding channel: " << szChannelName << "(#" << nChannelNO << ")" << std::endl;
		return nullptr;
	}

	void  __stdcall DelChannelITEM(void *pListItem) {
		std::cout << "Deleting channel" << std::endl;
	}

	void* __stdcall AddUserITEM(void *pUser, char *szAccount, char *szCharName, char *szIP) {
		std::cout << "Adding user, account: " << szAccount << ", char: " << szCharName << ", ip: " << szIP << std::endl;
		return nullptr;
	}

	void  __stdcall DelUserITEM(void *pListItem) {
		std::cout << "Deleting user" << std::endl;
	}
};


SHO_WS* g_instance;


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
		g_instance->Active(false);
		g_instance->ShutdownCLI_SOCKET();
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

	HWND console_window = GetConsoleWindow();
	HINSTANCE console_handle = GetModuleHandle(nullptr);

	const int ENGLISH = 1;
	int language = ENGLISH;
	char* data_dir = (char*)"C:\\dev\\rose-next\\server\\data";
	char* db_ip = (char*)"127.0.0.1";
	char* db_name = (char*)"SHO";
	char* db_user = (char*)"seven";
	char* db_password = (char*)"tpqmsgkcm";
	char* world_name = (char*)"1Rose Next";
	char* server_ip = (char*)"127.0.0.1";
	int server_port = 29100;
	char* loginserver_ip = server_ip;
	int loginserver_port = 19000;
	int map_port = 19001;

	std::cout << "Initializing the server" << std::endl;
	g_instance = SHO_WS::InitInstance(console_handle, data_dir, exe_api, ENGLISH);

	std::cout << "Connecting to the database" << std::endl;
	g_instance->ConnectDB(db_ip, db_name, db_user, db_password, db_user, db_password);

	std::cout << "Starting the server" << std::endl;
	g_instance->Start(console_window , loginserver_ip, loginserver_port, server_ip, server_port, world_name, map_port, server_port, false);

	std::cout << "Starting the client socket" << std::endl;
	g_instance->StartCLI_SOCKET();

	std::cout << "Activating the server" << std::endl;
	g_instance->Active(true);

	SetConsoleCtrlHandler(CtrlHandler, true);

	std::cout << "Starting main loop" << std::endl;

	while (true) {
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}