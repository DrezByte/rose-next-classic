#include "stdafx.h"

#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_ANDMEAN
    #include <windows.h>
#endif

#include "lib_util.h"
#include "LIB_gsMAIN.h"

#include "rose/common/config.h"
#include "rose/common/util.h"

CLIB_GameSRV* g_instance;

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
            g_instance->Shutdown();
            g_instance->Destroy();
            std::cout << "Press enter to quit..." << std::endl;
            std::cin.get();

        default:
            return FALSE;
    }
}

int
main(int argc, char** argv) {
    HWND console_window = GetConsoleWindow();
    HINSTANCE console_handle = GetModuleHandle(nullptr);
    SetConsoleTitle("ROSE Next - Game Server");

    // Parse args
    auto args = Rose::Util::parse_args(argc, argv);

    std::string config_path = get_exe_dir().append("server.toml");
    auto config_path_arg = args.find("--config");
    if (config_path_arg != args.end()) {
        config_path = config_path_arg->second;
    }

    // Load Config
    Rose::Common::ServerConfig config;
    bool config_loaded = config.load(config_path, "ROSE");

    // Initialize the logger
    Rose::Common::logger_init(config.gameserver.log_path.c_str(), config.gameserver.log_level);

    if (!config_loaded) {
        LOG_WARN("Could not load config file, using default settings");
    }

    // Initialize and start the server
    LOG_INFO("Initializing the server");
    g_instance = CLIB_GameSRV::InitInstance(console_handle,
        (char*)config.gameserver.data_dir.c_str(),
        config.gameserver.language);

    LOG_INFO("Connecting to other servers");
    g_instance->ConnectSERVER((char*)config.database.ip.c_str(),
        (char*)config.database.name.c_str(),
        (char*)config.database.username.c_str(),
        (char*)config.database.password.c_str(),
        (char*)config.database.username.c_str(),
        (char*)config.database.password.c_str(),
        (char*)config.worldserver.ip.c_str(),
        config.worldserver.server_port,
        (char*)config.loginserver.ip.c_str(),
        // config.loginserver.server_port,
        19005 // Dummy value because actually connecting to that socket crashes the game server
    );

    LOG_INFO("Initializing all maps");
    g_instance->InitLocalZone(true);

    LOG_INFO("Starting the server");
    int channel_no = 1;
    int low_age = 0;
    int high_age = 0;

    g_instance->Start(console_window,
        (char*)config.gameserver.server_name.c_str(),
        (char*)config.gameserver.ip.c_str(),
        config.gameserver.port,
        channel_no,
        low_age,
        high_age);

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