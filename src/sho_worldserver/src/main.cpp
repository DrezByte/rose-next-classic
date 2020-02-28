#include "stdafx.h"


#include <iostream>

#include "lib_util.h"
#include "sho_ws_lib.h"

#include "rose/common/config.h"
#include "rose/common/util.h"

SHO_WS* g_instance;

BOOL WINAPI
CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
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

        default:
            return FALSE;
    }
}

int
main() {
    HWND console_window = GetConsoleWindow();
    HINSTANCE console_handle = GetModuleHandle(nullptr);
    SetConsoleTitle("ROSE Next - World Server");

	// Load Config
	Rose::Common::ServerConfig config;
	bool config_loaded = config.load(get_exe_dir().append("server.toml"), "ROSE");

	// Initialize the logger
	Rose::Common::logger_init(config.worldserver.log_path.c_str(), config.worldserver.log_level);

	if (!config_loaded) {
		LOG_WARN("Could not load config file, using default settings");
	}

    // Start the server
    LOG_INFO("Initializing the server");
    g_instance = SHO_WS::InitInstance(console_handle, (char*)config.worldserver.data_dir.c_str(), config.worldserver.language);

    LOG_INFO("Connecting to the database");
    g_instance->ConnectDB(
		(char*)config.database.ip.c_str(),
		(char*)config.database.name.c_str(),
		(char*)config.database.username.c_str(),
		(char*)config.database.password.c_str(),
		(char*)config.database.username.c_str(), // Log DB User
		(char*)config.database.password.c_str() // Log DB Password
	);

    LOG_INFO("Starting the server");
    g_instance->Start(console_window,
		(char*)config.loginserver.ip.c_str(),
        config.loginserver.server_port,
		(char*)config.worldserver.ip.c_str(), // Log Server IP
		config.worldserver.port, // Log server port
		(char*)config.worldserver.world_name.c_str(),
		config.worldserver.server_port,
		config.worldserver.port,
        false);

    LOG_INFO("Starting the client socket");
    g_instance->StartCLI_SOCKET();

    LOG_INFO("Activating the server");
    g_instance->Active(true);

    SetConsoleCtrlHandler(CtrlHandler, true);

    while (true) {
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}