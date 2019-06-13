#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_ANDMEAN
#include <windows.h>
#endif

#undef __T_PACKET

#include "cls_api.h"
#include "lib_util.h"
#include "sho_ls_lib.h"

class ExeApi: public EXE_LS_API {
public:
    void __stdcall WriteLOG(char* szString) { std::cout << szString; }

    void __stdcall SetListItemINT(void* pListItem, int iSubStrIDX, int iValue) {
        std::cout << "Setting list item int" << std::endl;
    }

    void __stdcall SetListItemSTR(void* pListItem, int iSubStrIDX, char* szStr) {
        std::cout << "Setting list item str: " << szStr << std::endl;
    }

    void* __stdcall AddConnectorITEM(void* pOwner, char* szIP) {
        std::cout << "Adding connector item, ip: " << szIP << std::endl;
        return nullptr;
    }

    void __stdcall DelConnectorITEM(void* pListItem) {
        std::cout << "Deleting connector item" << std::endl;
    }

    void* __stdcall AddWorldITEM(void* pOwner,
        char* szWorld,
        char* szIP,
        int iPort,
        unsigned int dwRight) {
        std::cout << "Adding world item, world: " << szWorld << " , ip: " << szIP << std::endl;
        return nullptr;
    }

    void __stdcall DelWorldITEM(void* pListItem) {
        std::cout << "Deleting world item" << std::endl;
    }

    void* __stdcall AddBlockITEM(void* pOwner, char* szIP, unsigned int dwEndTime) {
        std::cout << "Adding block item, ip: " << szIP << std::endl;
        return nullptr;
    }

    void __stdcall DelBlockITEM(void* pListItem) {
        std::cout << "Deleting block item" << std::endl;
    }

    void __stdcall SetStatusBarTEXT(unsigned int iItemIDX, char* szText) {
        std::cout << "Setting status bar text: " << szText << std::endl;
    }
};

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
            std::cout << "Shutting down..." << std::endl;
            g_instance->CloseClientSOCKET();
            g_instance->Shutdown();
            g_instance->Destroy();
            std::cout << "Press enter to quit..." << std::endl;
            std::cin.get();

        default:
            return FALSE;
    }
}

int
main() {
    auto exe_api = new ExeApi();

    HWND console_window = GetConsoleWindow();
    HINSTANCE console_handle = GetModuleHandle(nullptr);
    SetConsoleTitle("ROSE Next - Login Server");

    char* db_ip = (char*)"127.0.0.1";
    int server_port = 19000;
    int client_port = 29000;
    byte* server_password = (byte*)"rose-next";

    std::cout << "Initializing the server" << std::endl;
    g_instance = SHO_LS::InitInstance(console_handle, exe_api);

    std::cout << "Starting the server socket" << std::endl;
    g_instance->StartServerSOCKET(console_window, db_ip, server_port, 0, nullptr, 0, false);

    std::cout << "Initializing the client socket" << std::endl;
    g_instance->StartClientSOCKET(client_port, 0, server_password);

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