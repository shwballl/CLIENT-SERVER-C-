#include <iostream>
#include <sstream>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main(){

    WSADATA wsData;
    WORD ver = MAKEWORD(2,2);

    int wsOk = WSAStartup(ver, &wsData);
    if(wsOk != 0){
        cerr << "Socket initilaze failed" << endl;
        return 1;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == INVALID_SOCKET){
        cerr << "Cant create a socket" << endl;
        return 1;
    }


    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    listen(listening, SOMAXCONN);

    fd_set master;
    FD_ZERO(&master);

    FD_SET(listening, &master);

    while(true){
        fd_set copy = master;

        int socketCount = select(0,&copy,nullptr,nullptr,nullptr);

        for (int i = 0; i < socketCount; ++i)
        {
           SOCKET sock = copy.fd_array[i];
           if(sock == listening){
                SOCKET client = accept(listening, nullptr, nullptr);

                FD_SET(client, &master);
                cout << "SOCKET: " << sock << " connected!" << endl;
                string msg = "Welcome to the chat!\n";
                send(client, msg.c_str(), msg.size()+1,0);
           }else{
                char Buff[4096];
                ZeroMemory(Buff, 4096);
                int bytesR = recv(sock, Buff, 4096,0);
                if(bytesR <= 0){
                    closesocket(sock);
                    FD_CLR(sock, &master);
                }else{
                    for (int i = 0; i < master.fd_count; ++i)
                    {
                        SOCKET outSock = master.fd_array[i];
                        if(outSock != listening && outSock != sock){
                            ostringstream ss;
                            ss << "> " << sock << ": " << Buff << "\r\n";
                            string strOut = ss.str();

                            send(outSock, strOut.c_str(), strOut.size()+1, 0);
                        }
                    }
                }
           }
        }
    }


    WSACleanup();
    return 0;
}