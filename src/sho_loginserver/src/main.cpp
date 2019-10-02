#include "stdafx.h"

#undef __T_PACKET

#include "rose/common/util.h"
#include "sho_ls_lib.h"

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

    char* db_ip = (char*)"127.0.0.1";
    int server_port = 19000;
    int client_port = 29000;
    byte* server_password = (byte*)"rose-next";

    // Initialize the logger
    char buffer[256] = {0};
    Rose::Common::get_bin_dir(buffer, 256);

    std::string log_path(buffer);
    log_path.append("/log/loginserver.log");

    Rose::Common::logger_init(log_path.c_str(), Rose::Common::LogLevel::Info);

    // Start server
    LOG_INFO("Initializing the server");
    g_instance = SHO_LS::InitInstance(console_handle);

    LOG_INFO("Starting the server socket");
    g_instance->StartServerSOCKET(console_window, db_ip, server_port, 0, false);

    LOG_INFO("Initializing the client socket");
    g_instance->StartClientSOCKET(client_port, 0, server_password);

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