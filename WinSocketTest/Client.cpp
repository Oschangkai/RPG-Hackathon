#pragma comment(lib, "Ws2_32.lib")


#include <WinSock2.h>
#include <iostream>
#include <string>
#include<Windows.h>
#include<cstring>



using namespace std;
int r;
SOCKET sConnect;
string confirm;
char message[200];
fd_set fdread;
int ret;
string input;


void readProc();
void writeProc();


void main()
{




	//�}�l Winsock-DLL

	WSAData wsaData;
	WORD DLLVersion;
	DLLVersion = MAKEWORD(2, 1);
	r = WSAStartup(DLLVersion, &wsaData);


	//�ŧi�� socket �ϥΪ� sockadder_in ���c
	SOCKADDR_IN addr;


	int addlen = sizeof(addr);



	//�]�w socket



	//AF_INET: internet-family
	//SOCKET_STREAM: connection-oriented socket
	sConnect = socket(AF_INET, SOCK_STREAM, NULL);


	//�]�w addr ���
	addr.sin_addr.s_addr = inet_addr("192.168.100.125");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(25565);


	cout << "connect to server?[Y] or [N]" << endl;
	cin >> confirm;


	if (confirm == "N")
	{
		exit(1);
	}
	else {
		if (confirm == "Y")
		{
			cout << "Success";
			connect(sConnect, (SOCKADDR*)&addr, sizeof(addr));
			cout << "Success";





			while (true) {




				if (GetAsyncKeyState(83)) {
					HANDLE writeHandle;
					writeHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)writeProc, NULL, 0, NULL);
					WaitForSingleObject(writeHandle, INFINITE);
				}
				else {
					HANDLE readHandle;
					readHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)readProc, NULL, 0, NULL);
					WaitForSingleObject(readHandle, 100);
					readHandle = FALSE;
					//cout << "ELSE\n";
				}


				//WSAEVENT event = WSACreateEvent();
				//WSANETWORKEVENTS eventt;
				//::WSAEnumNetworkEvents(sConnect, event, &eventt);
				//::WSAEventSelect(sConnect, event, FD_READ);
				//if (eventt.lNetworkEvents & FD_READ)
				//if ((ret=select(0, &fdread,NULL,NULL,NULL)>0)) {
				/*if (FD_ISSET(sConnect,&fdread)){
				r = recv(sConnect, message, sizeof(message), 0);
				cout << message << endl;
				FD_ZERO(&fdread);
				}
				*/


				//�]�w closesocket �ɡA���g�L TIME-WAIT �L�{,��������socket
				//BOOL bDontLinger = FALSE;
				//setsockopt(sConnect,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL));


				//�Y���ᤣ�A�ϥΡA�i�� closesocket �����s�u



			}
			closesocket(sConnect);


			getchar();
			getchar();
		}
	}


}

void readProc()
{
	//���� server �ݪ��T��
	ZeroMemory(message, 200);
	r = recv(sConnect, message, sizeof(message), 0);
	cout << message << endl;
	//FD_ZERO(&fdread);
	//cout << "listen\n";
}
void writeProc() {
	cout << "input\n";
	cin >> input;
	char *s_input = new char[input.size() + 1];
	strcpy(s_input, input.c_str());
	send(sConnect, s_input, (int)strlen(s_input), 0);
}