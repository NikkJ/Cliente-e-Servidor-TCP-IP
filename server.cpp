//#define _WIN32_WINNT 0x501
#define _WIN32_WINNT 0x0600
#include <ws2tcpip.h>
#include <Winsock2.h>
#include <windows.h>
#include <iostream>

#include "func_inet.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main() {
    //iniciar winsock
    WSADATA dados;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &dados);
    if (wsOk != 0) {
        std::cerr << "Erro ao inciar: #" << wsOk << std::endl;
        return -1;
    }

    // criar socket

    SOCKET ouvindo = socket(AF_INET, SOCK_STREAM, 0);

    if (ouvindo == INVALID_SOCKET) {
        std::cerr << "Erro na criacao do socket: #" << WSAGetLastError() << std::endl;
        return -2;
    }

    // liga o socket a um ip e porta

    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(54000);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;  // pode usar inet_pton no lugar

    bind(ouvindo, (sockaddr*)&addr, sizeof(addr));
	
    //diz que winsock está ouvindo
    listen(ouvindo, SOMAXCONN);

    //espera por uma conexão

    sockaddr_in client;
    int clientSize = sizeof(client);
	
    SOCKET clientSocket = accept(ouvindo, (sockaddr*)&client, &clientSize);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Erro no socket de cliente: #" << WSAGetLastError() << std::endl;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST,service, NI_MAXSERV, 0)) {
        std::cout << host << " conectado na porta " << service << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " conectado na porta " << ntohs(client.sin_port) << std::endl;
    }
    // fecha o socket q ta ouvindo
    closesocket(ouvindo);

    //loop aceita e envia msg ao cliente
    char buff[4096];
    char comp_buff[4096] = "teste";
    char comp2_buff[4096] = "outro teste";
    char resposta[100];
    

    while (true) {
        ZeroMemory(buff, 4096);
        //espera cliente mandar dados
        int bytesRecebidos = recv(clientSocket, buff, 4096, 0);
        std::cout<<"Conexao com: "<<clientSocket<<std::endl;
        if (bytesRecebidos == SOCKET_ERROR) {
            std::cerr << "Erro no recv(): #" << bytesRecebidos << std::endl;
            break;
        }
        if (bytesRecebidos == 0) {
            break;
        }else{
        	if(strcmp(buff,comp_buff) == 0){
        		std::cin>>resposta;
        		send(clientSocket, resposta, strlen(resposta), 0);
			}else if(strcmp(buff,comp2_buff)==0){
				send(clientSocket, "recebido 2", strlen("recebido 2"), 0);
			}else{
				send(clientSocket, "sem comando", strlen("sem comando"), 0);
			}
		}

        //imprime msg para o cliente
        
    }

    //fecha o socket
    closesocket(clientSocket);
    // desliga winsock
    WSACleanup();
}
