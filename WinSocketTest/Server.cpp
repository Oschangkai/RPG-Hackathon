#pragma comment(lib, "Ws2_32.lib")
#include <conio.h>
#include <WinSock2.h>
#include <iostream>
#include <string>

using namespace std;

//�}�l Winsock-DLL
WSAData wsaData
WORD DLLVSERION = MAKEWORD(2, 1);

//�� WSAStartup �}�l Winsocket-DLL
int r = WSAStartup(DLLVSERION, &wsaData);

//�ŧi socket ��}��T(���P���q�T,�����P����}��T,�ҥH�|�����P����Ƶ��c�s��o�Ǧ�}��T)
SOCKADDR_IN addr;
int addrlen = sizeof(addr);

//�إ� socket
SOCKET sListen;  //listening for an incoming connection
SOCKET sConnect; //operating if a connection was found

SOCKADDR_IN clinetAddr; //���ݳs�u


void getMessage();
void sendMessage();
void connecter();


void main()
{


	//AF_INET�G��ܫإߪ� socket �ݩ� internet family
	//SOCK_STREAM�G��ܫإߪ� socket �O connection-oriented socket 
	sConnect = socket(AF_INET, SOCK_STREAM, NULL);

	//�]�w��}��T�����
	addr.sin_addr.s_addr = inet_addr("192.168.100.161");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(25565);

	//�]�w Listen
	sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN); //SOMAXCONN: listening without any limit

								//���ݳs�u
	cout << "Waiting for Connection..." << endl;
	while (true)
	{
		HANDLE connecterHandle;
		connecterHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)connecter, NULL, 0, NULL);
		WaitForSingleObject(connecterHandle, 100);

		if (GetAsyncKeyState(83)) {
			HANDLE writeHandle;
			writeHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)getMessage, NULL, 0, NULL);
			WaitForSingleObject(writeHandle, INFINITE);
		}
		else {
			cout << "else here!";
			HANDLE readHandle;
			readHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendMessage, NULL, 0, NULL);
			WaitForSingleObject(readHandle, 100);
			readHandle = FALSE;
		}
	}

}
void sendMessage() {
	string Msg;
	cin >> Msg;
	send(sConnect, Msg.c_str(), (int)strlen(Msg.c_str()), 0);
}
void getMessage() {
	char message[200];
	ZeroMemory(message, 200);
	r = recv(sConnect, message, sizeof(message), 0);
	cout << message << endl;
}
void connecter() {
	if (sConnect = accept(sListen, (SOCKADDR*)&clinetAddr, &addrlen))
	{

		cout << "A Connection was Found" << endl;
		cout << "Server: Got Connection From: " << inet_ntoa(addr.sin_addr) << endl;
		//printf("server: got connection from %s\n", inet_ntoa(addr.sin_addr));
	}
}