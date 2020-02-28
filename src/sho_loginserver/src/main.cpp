#include "stdafx.h"


#undef __T_PACKET

#include "sho_ls_lib.h"

#include "rose/common/config.h"
#include "rose/common/util.h"

SHO_LS* g_instance;

BOOL WINAPI
CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            LOG_INFO("Shutting down...");
            g_instance->CloseClientSOCKET();
            g_instance->Shutdown();
            g_instance->Destroy();
            return true;

        default:
            return FALSE;
    }
}

int
main() {
    HWND console_window = GetConsoleWindow();
    HINSTANCE console_handle = GetModuleHandle(nullptr);
    SetConsoleTitle("ROSE Next - Login Server");

	// Load config
	Rose::Common::ServerConfig config;
	bool config_loaded = config.load(get_exe_dir().append("server.toml"), "ROSE");

    // Initialize the logger
    Rose::Common::logger_init(config.loginserver.log_path.c_str(), config.loginserver.log_level);

	if (!config_loaded) {
		LOG_WARN("Could not load config file, using default settings");
	}

    // Start server
    LOG_INFO("Initializing the server");
    g_instance = SHO_LS::InitInstance(console_handle);

    LOG_INFO("Starting the server socket");
    g_instance->StartServerSOCKET(console_window, (char*)config.database.ip.c_str(), config.loginserver.server_port, 0, false);

    LOG_INFO("Initializing the client socket");
    g_instance->StartClientSOCKET(config.loginserver.port, 0, (byte*)config.loginserver.password.c_str());

    SetConsoleCtrlHandler(CtrlHandler, true);

    LOG_INFO("Server ready");

    while (true) {
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}