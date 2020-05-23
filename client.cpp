#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <string>


#include "func_inet.hpp"

#pragma comment(lib,"ws2_32.lib")
//elisa 192.168.1.6
//pai 192.168.15.2
int main() {
	std::string ipAddr = "127.0.0.1"; //ip do server
	
	int porta = 54000;
	

	// iniciar winsock
	WSADATA dados;
	WORD ver = MAKEWORD(2,2);
	int wsResultado = WSAStartup(ver,&dados);
	if(wsResultado!=0){
		std::cerr<<"Erro ao inicializar WinSock, erro: #"<<wsResultado<<std::endl;
		return -1;
	}
	// criar socket
	SOCKET sock = socket(AF_INET,SOCK_STREAM,0);
	
	if(sock == INVALID_SOCKET){
		std::cerr<<"Erro ao criar socket, erro: #"<<WSAGetLastError()<<std::endl;
		WSACleanup();
		return -2;
	}
	// preencher em uma struct desejada

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(porta);
	inet_pton(AF_INET,ipAddr.c_str(),&hint.sin_addr);
	int len = sizeof(hint);
	// conectar no server
	
	int connResult = connect(sock,(sockaddr*)&hint,sizeof(hint));
	if(connResult == SOCKET_ERROR){
		std::cerr<<"Erro ao conectar ao servidor: #"<<WSAGetLastError()<<std::endl;
		closesocket(sock);
		WSACleanup();
		return -3;
	}
	// loop para enviar e receber dados
	char buffer[4096];
	std::string userInput;

	do{
		//pede ao usuario texto
		std::cout<<">";
		std::getline(std::cin,userInput);
		if(userInput.size()>0){
			//envia o texto
			int enviaResultado = send(sock,userInput.c_str(),userInput.size()+1,0);
			if(enviaResultado != SOCKET_ERROR){
				//espera resposta
				ZeroMemory(buffer,4096);
				int bytesRecebidos = recv(sock, buffer, 4096, 0);
				if(bytesRecebidos > 0){
					//imprime resposta no console
					std::cout<<"SERVIDOR>"<< std::string(buffer,0,bytesRecebidos) << std::endl;
				}
			}
			
		}
	}while(userInput.size()>0);
	
	// fechar o programa
	closesocket(sock);
	WSACleanup();
	return 0;
}
