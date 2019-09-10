#include "stdafx.h"

#include <iostream>

#include "lib_util.h"
#include "rose/common/util.h"
#include "sho_ws_lib.h"

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
    // Let login server load
    Sleep(2000);

    HWND console_window = GetConsoleWindow();
    HINSTANCE console_handle = GetModuleHandle(nullptr);
    SetConsoleTitle("ROSE Next - World Server");

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

    // Initialize the logger
    char buffer[256] = {0};
    Rose::Common::get_bin_dir(buffer, 256);

    std::string log_path(buffer);
    log_path.append("/log/worldserver.log");

    Rose::Common::logger_init(log_path.c_str(), Rose::Common::LogLevel::Trace);

    // Start the server
    LOG_INFO("Initializing the server");
    g_instance = SHO_WS::InitInstance(console_handle, data_dir, ENGLISH);

    LOG_INFO("Connecting to the database");
    g_instance->ConnectDB(db_ip, db_name, db_user, db_password, db_user, db_password);

    LOG_INFO("Starting the server");
    g_instance->Start(console_window,
        loginserver_ip,
        loginserver_port,
        server_ip,
        server_port,
        world_name,
        map_port,
        server_port,
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