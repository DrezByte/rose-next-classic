#include "stdafx.h"

#undef __T_PACKET

#include "sho_ls_lib.h"

#include "rose/common/server_config.h"
#include "rose/common/util.h"

SHO_LS* g_instance;

int
shutdown(int exit_code = 0) {
    LOG_INFO("Shutting down...");
    g_instance->CloseClientSOCKET();
    g_instance->Shutdown();
    g_instance->Destroy();
    return exit_code;
}

BOOL WINAPI
CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            return shutdown(1);
        default:
            return FALSE;
    }
}

int
main(int argc, char** argv) {
    HWND console_window = GetConsoleWindow();
    HINSTANCE console_handle = GetModuleHandle(nullptr);
    SetConsoleTitle("ROSE Next - Login Server");

    // Parse args
    auto args = Rose::Util::parse_args(argc, argv);

    std::string config_path = get_exe_dir().append("server.toml");
    auto config_path_arg = args.find("--config");
    if (config_path_arg != args.end()) {
        config_path = config_path_arg->second;
    }

    // Load config
    Rose::Common::ServerConfig config;
    bool config_loaded = config.load(config_path, "ROSE");

    // Initialize the logger
    Rose::Common::logger_init(config.loginserver.log_path.c_str(), config.loginserver.log_level);

    if (!config_loaded) {
        LOG_WARN("Could not load config file, using default settings");
    }

    // Start server
    LOG_INFO("Initializing the server");
    g_instance = SHO_LS::InitInstance(console_handle);

    LOG_INFO("Starting the database connection.");
    bool db_connected = g_instance->connect_database(config.database);
    if (!db_connected) {
        return shutdown(1);
    }

    LOG_INFO("Starting the server socket");
    g_instance->StartServerSOCKET(console_window,
        (char*)config.database.ip.c_str(),
        config.loginserver.server_port,
        0,
        false);

    LOG_INFO("Starting the client socket");
    g_instance->StartClientSOCKET(config.loginserver.port,
        0,
        (byte*)config.loginserver.password.c_str());

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