#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <string>
#include<Windows.h>
#include<cstring>


using namespace std;

void main()
{
	string confirm;
	char message[200];

	//�}�l Winsock-DLL
	int r;
	WSAData wsaData;
	WORD DLLVersion;
	DLLVersion = MAKEWORD(2, 1);
	r = WSAStartup(DLLVersion, &wsaData);

	//�ŧi�� socket �ϥΪ� sockadder_in ���c
	SOCKADDR_IN addr;

	int addlen = sizeof(addr);

	//�]�w socket
	SOCKET sConnect;

	//AF_INET: internet-family
	//SOCKET_STREAM: connection-oriented socket
	sConnect = socket(AF_INET, SOCK_STREAM, NULL);

	//�]�w addr ���
	addr.sin_addr.s_addr = inet_addr("192.168.100.161");
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
			string input;
			while (true) {
				//���� server �ݪ��T��
				ZeroMemory(message, 200);
				r = recv(sConnect, message, sizeof(message), 0);
				cout << message << endl;

				if (GetAsyncKeyState(83)) {
					cout << "input";
					cin >> input;
					char *s_input = new char[input.size()+1];
					strcpy(s_input, input.c_str());
					send(sConnect, s_input, (int)strlen(s_input), 0);
					

				}

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