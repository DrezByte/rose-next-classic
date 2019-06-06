#include "stdafx.h"
#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_ANDMEAN
#include <windows.h>
#endif

#include "lib_util.h"
#include "LIB_gsMAIN.h"

#include "rose/common/util.h"

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

	HWND console_window = GetConsoleWindow();
	HINSTANCE console_handle = GetModuleHandle(nullptr);
	SetConsoleTitle("ROSE Next - Game Server");

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
	int loginserver_port = 19005; // Not the actual port, actually connecting crashes the game server
	char* charserver_ip = server_ip;
	int charserver_port = 19001;

	// Initialize the logger
	char buffer[256] = { 0 };
	Rose::Common::get_bin_dir(buffer, 256);

	std::string log_path(buffer);
	log_path.append("/log/gameserver.log");

	Rose::Common::logger_init(log_path.c_str(), Rose::Common::LogLevel::Info);

	// Initialize and start the server
	LOG_INFO("Initializing the server");
	g_instance = CLIB_GameSRV::InitInstance(console_handle, data_dir, ENGLISH);

	LOG_INFO("Connecting to other servers");
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

	LOG_INFO("Initializing all maps");
	g_instance->InitLocalZone(true);

	LOG_INFO("Starting the server");
	g_instance->Start(console_window, server_name, server_ip, server_port, 1, 0, 0);

	LOG_INFO("Setting user limit");
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