#pragma comment(lib, "Ws2_32.lib")


#include <WinSock2.h>
#include <iostream>
#include <string>
#include<Windows.h>
#include<cstring>
#include"conio.h"
#pragma comment(lib, "Ws2_32.lib")






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
			cout << "Waiting for Server..." << endl;
			connect(sConnect, (SOCKADDR*)&addr, sizeof(addr));
			cout << "\nWelcome to TXT War!\nUse \"WASD\" and j to Control\n";
			int nNetTimeout = 1;//1��A

			//�]�m�����W��
			setsockopt(sConnect, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));


			while (true) {

				if (kbhit()) {
					HANDLE writeHandle;
					writeHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)writeProc, NULL, 0, NULL);
					WaitForSingleObject(writeHandle, INFINITE);
					//writeHandle = FALSE;
					CloseHandle(writeHandle);
					HeapFree(GetProcessHeap(), 0, NULL);
				}
				else {
					HANDLE readHandle;
					readHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)readProc, NULL, 0, NULL);
					WaitForSingleObject(readHandle, 100);
					//readHandle = FALSE;
					CloseHandle(readHandle);
					HeapFree(GetProcessHeap(), 0, NULL);
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
	r = recv(sConnect, message, sizeof(message), MSG_WAITALL);
	if (r <= 0)return;
	cout << message << "\nfrom server"<<endl;
	//FD_ZERO(&fdread);
	//cout << "listen\n";
	//ExitThread(0);
	return;
}
void writeProc() {
	//cout << "input\n";
	//input=getch();
	//char *s_input = new char[input.size() + 1];
	//strcpy(s_input, input.c_str());

	char input[200];
	input[0] = getch();

	input[1] = '\0';
	send(sConnect, input, (int)strlen(input), 0);
	input[0] = NULL;
	input[1] = NULL;
	return;


}